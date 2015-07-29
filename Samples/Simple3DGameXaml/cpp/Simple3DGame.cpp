//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************

#include "pch.h"
#include "GameRenderer.h"

#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Level4.h"
#include "Level5.h"
#include "Level6.h"
#include "Animate.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Face.h"
#include "MediaReader.h"

using namespace concurrency;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::Storage;
using namespace Windows::UI::Core;

//----------------------------------------------------------------------

Simple3DGame::Simple3DGame():
    m_ammoCount(0),
    m_ammoNext(0),
    m_gameActive(false),
    m_levelActive(false),
    m_totalHits(0),
    m_totalShots(0),
    m_levelBonusTime(0.0),
    m_levelTimeRemaining(0.0),
    m_levelCount(0),
    m_currentLevel(0),
    m_activeBackground(0)
{
    m_topScore.totalHits = 0;
    m_topScore.totalShots = 0;
    m_topScore.levelCompleted = 0;
}

//----------------------------------------------------------------------

void Simple3DGame::Initialize(
    _In_ MoveLookController^ controller,
    _In_ GameRenderer^ renderer
    )
{
    // This method is expected to be called as an asynchronous task.
    // Care should be taken to not call rendering methods on the
    // m_renderer as this would result in the D3D Context being
    // used in multiple threads, which is not allowed.

    m_controller = controller;
    m_renderer = renderer;

    m_audioController = ref new Audio;
    m_audioController->CreateDeviceIndependentResources();

    InitializeGameConfig();

    m_ammo = std::vector<Sphere^>(GameConstants::MaxAmmo);
    m_objects = std::vector<GameObject^>();
    m_renderObjects = std::vector<GameObject^>();
    m_level = std::vector<Level^>();

    m_savedState = ref new PersistentState();
    m_savedState->Initialize(ApplicationData::Current->LocalSettings->Values, "Game");

    m_timer = ref new GameTimer();

    // Create a sphere primitive to represent the player.
    // The sphere will be used to handle collisions and constrain the player in the world.
    // It is not rendered so it is not added to the list of render objects.
    // It is added to the object list so it will be included in intersection calculations.
    m_player = ref new Sphere(XMFLOAT3(0.0f, -1.3f, 4.0f), 0.2f);
    m_objects.push_back(m_player);
    m_player->Active(true);

    m_camera = ref new Camera;
    m_camera->SetProjParams(XM_PI / 2, 1.0f, 0.01f, 100.0f);
    m_camera->SetViewParams(
        m_player->Position(),            // Eye point in world coordinates.
        XMFLOAT3 (0.0f, 0.7f, 0.0f),     // Look at point in world coordinates.
        XMFLOAT3 (0.0f, 1.0f, 0.0f)      // The Up vector for the camera.
        );

    m_controller->Pitch(m_camera->Pitch());
    m_controller->Yaw(m_camera->Yaw());

    // Instantiate a set of primitives to represent the containing world. These objects
    // maintain the geometry and material properties of the walls, floor and ceiling.
    // The TargetId is used to identify the world objects so that the right geometry
    // and textures can be associated with them later after those resources have
    // been created.
    GameObject^ world = ref new GameObject();
    world->TargetId(GameConstants::WorldFloorId);
    world->Active(true);
    m_renderObjects.push_back(world);

    world = ref new GameObject();
    world->TargetId(GameConstants::WorldCeilingId);
    world->Active(true);
    m_renderObjects.push_back(world);

    world = ref new GameObject();
    world->TargetId(GameConstants::WorldWallsId);
    world->Active(true);
    m_renderObjects.push_back(world);

    // Min and max Bound are defining the world space of the game.
    // All camera motion and dynamics are confined to this space.
    m_minBound = XMFLOAT3(-4.0f, -3.0f, -6.0f);
    m_maxBound = XMFLOAT3(4.0f, 3.0f, 6.0f);

    // Instantiate the Cylinders for use in the various game levels.
    // Each cylinder has a different initial position, radius and direction vector,
    // but share a common set of material properties.
    for (int a = 0; a < GameConstants::MaxCylinders; a++)
    {
        Cylinder^ cylinder;
        switch (a)
        {
        case 0:
            cylinder = ref new Cylinder(XMFLOAT3(-2.0f, -3.0f, 0.0f), 0.25f, XMFLOAT3(0.0f, 6.0f, 0.0f));
            break;
        case 1:
            cylinder = ref new Cylinder(XMFLOAT3(2.0f, -3.0f, 0.0f), 0.25f, XMFLOAT3(0.0f, 6.0f, 0.0f));
            break;
        case 2:
            cylinder = ref new Cylinder(XMFLOAT3(0.0f, -3.0f, -2.0f), 0.25f, XMFLOAT3(0.0f, 6.0f, 0.0f));
            break;
        case 3:
            cylinder = ref new Cylinder(XMFLOAT3(-1.5f, -3.0f, -4.0f), 0.25f, XMFLOAT3(0.0f, 6.0f, 0.0f));
            break;
        case 4:
            cylinder = ref new Cylinder(XMFLOAT3(1.5f, -3.0f, -4.0f), 0.50f, XMFLOAT3(0.0f, 6.0f, 0.0f));
            break;
        }
        cylinder->Active(true);
        m_objects.push_back(cylinder);
        m_renderObjects.push_back(cylinder);
    }

    MediaReader^ mediaReader = ref new MediaReader;
    auto targetHitSound = mediaReader->LoadMedia("Assets\\hit.wav");

    // Instantiate the targets for use in the game.
    // Each target has a different initial position, size and orientation,
    // but share a common set of material properties.
    // The target is defined by a position and two vectors that define both
    // the plane of the target in world space and the size of the parallelogram
    // based on the lengths of the vectors.
    // Each target is assigned a number for identification purposes.
    // The Target ID number is 1 based.
    // All targets have the same material properties.
    for (int a = 1; a < GameConstants::MaxTargets; a++)
    {
        Face^ target;
        switch (a)
        {
        case 1:
            target = ref new Face(XMFLOAT3(-2.5f, -1.0f, -1.5f), XMFLOAT3(-1.5f, -1.0f, -2.0f), XMFLOAT3(-2.5f, 1.0f, -1.5f));
            break;
        case 2:
            target = ref new Face(XMFLOAT3(-1.0f, 1.0f, -3.0f), XMFLOAT3(0.0f, 1.0f, -3.0f), XMFLOAT3(-1.0f, 2.0f, -3.0f));
            break;
        case 3:
            target = ref new Face(XMFLOAT3(1.5f, 0.0f, -3.0f), XMFLOAT3(2.5f, 0.0f, -2.0f), XMFLOAT3(1.5f, 2.0f, -3.0f));
            break;
        case 4:
            target = ref new Face(XMFLOAT3(-2.5f, -1.0f, -5.5f), XMFLOAT3(-0.5f, -1.0f, -5.5f), XMFLOAT3(-2.5f, 1.0f, -5.5f));
            break;
        case 5:
            target = ref new Face(XMFLOAT3(0.5f, -2.0f, -5.0f), XMFLOAT3(1.5f, -2.0f, -5.0f), XMFLOAT3(0.5f, 0.0f, -5.0f));
            break;
        case 6:
            target = ref new Face(XMFLOAT3(1.5f, -2.0f, -5.5f), XMFLOAT3(2.5f, -2.0f, -5.0f), XMFLOAT3(1.5f, 0.0f, -5.5f));
            break;
        case 7:
            target = ref new Face(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.5f, 0.0f));
            break;
        case 8:
            target = ref new Face(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.5f, 0.0f));
            break;
        case 9:
            target = ref new Face(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.5f, 0.0f));
            break;
        }

        target->Target(true);
        target->TargetId(a);
        target->Active(true);
        target->HitSound(ref new SoundEffect());
        target->HitSound()->Initialize(
            m_audioController->SoundEffectEngine(),
            mediaReader->GetOutputWaveFormatEx(),
            targetHitSound
            );

        m_objects.push_back(target);
        m_renderObjects.push_back(target);
    }

    // Instantiate a set of spheres to be used as ammunition for the game
    // and set the material properties of the spheres.
    auto ammoHitSound = mediaReader->LoadMedia("Assets\\bounce.wav");

    for (int a = 0; a < GameConstants::MaxAmmo; a++)
    {
        m_ammo[a] = ref new Sphere;
        m_ammo[a]->Radius(GameConstants::AmmoRadius);
        m_ammo[a]->HitSound(ref new SoundEffect());
        m_ammo[a]->HitSound()->Initialize(
            m_audioController->SoundEffectEngine(),
            mediaReader->GetOutputWaveFormatEx(),
            ammoHitSound
            );
        m_ammo[a]->Active(false);
        m_renderObjects.push_back(m_ammo[a]);
    }

    // Instantiate each of the game levels. The Level class contains methods
    // that initialize the objects in the world for the given level and also
    // define any motion paths for the objects in that level.

    m_level.push_back(ref new Level1);
    m_level.push_back(ref new Level2);
    if (!m_gameConfig.isTrial)
    {
        m_level.push_back(ref new Level3);
        m_level.push_back(ref new Level4);
        m_level.push_back(ref new Level5);
        m_level.push_back(ref new Level6);
    }
    m_levelCount = static_cast<uint32>(m_level.size());

    // Load the top score from disk if it exists.
    LoadHighScore();

    // Load the currentScore for saved state if it exists.
    LoadState();

    m_controller->Active(false);
}

//----------------------------------------------------------------------

void Simple3DGame::LoadGame()
{
    m_player->Position(XMFLOAT3 (0.0f, -1.3f, 4.0f));

    m_camera->SetViewParams(
        m_player->Position(),            // Eye point in world coordinates.
        XMFLOAT3 (0.0f, 0.7f, 0.0f),     // Look at point in world coordinates.
        XMFLOAT3 (0.0f, 1.0f, 0.0f)      // The Up vector for the camera.
        );

    m_controller->Pitch(m_camera->Pitch());
    m_controller->Yaw(m_camera->Yaw());
    m_currentLevel = 0;
    m_levelTimeRemaining = 0.0f;
    m_levelBonusTime = m_levelTimeRemaining;
    m_levelDuration = m_level[m_currentLevel]->TimeLimit() + m_levelBonusTime;
    InitializeAmmo();
    m_totalHits = 0;
    m_totalShots = 0;
    m_gameActive = false;
    m_levelActive = false;
    m_timer->Reset();
}

//----------------------------------------------------------------------

task<void> Simple3DGame::LoadLevelAsync()
{
    // Initialize the level and spin up the async loading of the rendering
    // resources for the level.
    // This will run in a separate thread, so for Direct3D 11, only Device
    // methods are allowed.  Any DeviceContext method calls need to be
    // done in FinalizeLoadLevel.

    m_level[m_currentLevel]->Initialize(m_objects);
    m_levelDuration = m_level[m_currentLevel]->TimeLimit() + m_levelBonusTime;

    return m_renderer->LoadLevelResourcesAsync();
}

//----------------------------------------------------------------------

void Simple3DGame::FinalizeLoadLevel()
{
    // This method is called on the main thread, so Direct3D 11 DeviceContext
    // method calls are allowable here.

    // Finalize the Level loading.
    m_renderer->FinalizeLoadLevelResources();
}

//----------------------------------------------------------------------

void Simple3DGame::StartLevel()
{
    m_timer->Reset();
    m_timer->Start();
    if (m_currentLevel == 0)
    {
        m_gameActive = true;
    }
    m_levelActive = true;
    m_controller->Active(true);
}

//----------------------------------------------------------------------

void Simple3DGame::PauseGame()
{
    m_timer->Stop();
    SaveState();
}

//----------------------------------------------------------------------

void Simple3DGame::ContinueGame()
{
    m_timer->Start();
    m_controller->Active(true);
}

//----------------------------------------------------------------------

GameState Simple3DGame::RunGame()
{
    // This method is called to execute a single time interval for active game play.
    // It returns the resulting state of game play after the interval has been executed.

    m_timer->Update();

    m_levelTimeRemaining = m_levelDuration - m_timer->PlayingTime();

    if (m_levelTimeRemaining <= 0.0f)
    {
        // Time expired, so the game is over.
        m_levelTimeRemaining = 0.0f;
        InitializeAmmo();
        m_timer->Reset();
        m_gameActive = false;
        m_levelActive = false;
        SaveState();

        if (m_totalHits > m_topScore.totalHits)
        {
            // There is a new high score so save it.
            m_topScore.totalHits = m_totalHits;
            m_topScore.totalShots = m_totalShots;
            m_topScore.levelCompleted = m_currentLevel;

            SaveHighScore();
        }
        return GameState::TimeExpired;
    }
    else
    {
        // Time has not expired, so run one frame of game play.
        m_player->Velocity(m_controller->Velocity());
        m_camera->LookDirection(m_controller->LookDirection());

        UpdateDynamics();

        // Update the Camera with the player position updates from the dynamics calculations.
        m_camera->Eye(m_player->Position());
        m_camera->LookDirection(m_controller->LookDirection());

        if (m_level[m_currentLevel]->Update(m_timer->PlayingTime(), m_timer->DeltaTime(), m_levelTimeRemaining, m_objects))
        {
            // The level has been completed.
            m_levelActive = false;
            InitializeAmmo();

            if (m_currentLevel < m_levelCount-1)
            {
                // More levels to go so increment the level number.
                // Actual level loading will occur in the LoadLevelAsync / FinalizeLoadLevel
                // methods.
                m_timer->Reset();
                m_currentLevel++;
                m_levelBonusTime = m_levelTimeRemaining;
                SaveState();
                return GameState::LevelComplete;
            }
            else
            {
                // All levels have been completed.
                m_timer->Reset();
                m_gameActive = false;
                m_levelActive = false;
                SaveState();

                if (m_totalHits > m_topScore.totalHits)
                {
                    // There is a new high score so save it.
                    m_topScore.totalHits = m_totalHits;
                    m_topScore.totalShots = m_totalShots;
                    m_topScore.levelCompleted = m_currentLevel;

                    SaveHighScore();
                }
                return GameState::GameComplete;
            }
        }
    }
    return GameState::Active;
}

//----------------------------------------------------------------------

void Simple3DGame::OnSuspending()
{
    m_audioController->SuspendAudio();
}

//----------------------------------------------------------------------

void Simple3DGame::OnResuming()
{
    m_audioController->ResumeAudio();
}

//----------------------------------------------------------------------

void Simple3DGame::UpdateDynamics()
{
    float timeTotal = m_timer->PlayingTime();
    float timeFrame = m_timer->DeltaTime();
    bool fire = m_controller->IsFiring();

#pragma region Shoot Ammo
    // Shoot ammo.
    if (fire)
    {
        static float lastFired;  // Timestamp of last ammo fired.

        if (timeTotal < lastFired)
        {
            // timeTotal is not guarenteed to be monotomically increasing because it is
            // reset at each level.
            lastFired = timeTotal - GameConstants::Physics::AutoFireDelay;
        }

        if (timeTotal - lastFired >= GameConstants::Physics::AutoFireDelay)
        {
            // Get inverse view matrix.
            XMMATRIX invView;
            XMVECTOR det;
            invView = XMMatrixInverse(&det, m_camera->View());

            // Compute initial velocity in world space from camera space.
            XMFLOAT4 initialVelocity(0.0f, 0.0f, 15.0f, 0.0f);
            m_ammo[m_ammoNext]->Velocity(XMVector4Transform(XMLoadFloat4(&initialVelocity), invView));

            // Set the initial position of the ammo to be fired. The position is offset from the player
            // to avoid an initial collision with the player object.
            XMFLOAT4 initialPosition(0.0f, -0.15f, m_player->Radius() + GameConstants::AmmoSize, 1.0f);
            m_ammo[m_ammoNext]->Position(XMVector4Transform(XMLoadFloat4(&initialPosition), invView));

            // Initially not laying on ground.
            m_ammo[m_ammoNext]->OnGround(false);
            m_ammo[m_ammoNext]->Active(true);

            // Set position in array of next Ammo to use.
            // We will re-use ammo taking the least recently used once we have hit the
            // MaxAmmo in use.
            m_ammoNext = (m_ammoNext + 1) % GameConstants::MaxAmmo;
            m_ammoCount = min(m_ammoCount + 1, GameConstants::MaxAmmo);

            lastFired = timeTotal;
            m_totalShots++;
        }
    }
#pragma endregion

#pragma region Animate Objects
    // Walk the list of objects looking for any objects that have an animation associated with it.
    // Update the position of the object based on evaluating the animation object with the current time.
    // Once the current time (timeTotal) is past the end of the animation time remove
    // the animation object since it is no longer needed.
    for (uint32 i = 0; i < m_objects.size(); i++)
    {
        if (m_objects[i]->AnimatePosition())
        {
            m_objects[i]->Position(m_objects[i]->AnimatePosition()->Evaluate(timeTotal));
            if (m_objects[i]->AnimatePosition()->IsFinished(timeTotal))
            {
                m_objects[i]->AnimatePosition(nullptr);
            }
        }
    }
#pragma endregion

    // If the elapsed time is too long, we slice up the time and handle physics over several
    // smaller time steps to avoid missing collisions.
    float timeLeft = timeFrame;
    float elapsedFrameTime;
    while (timeLeft > 0.0f)
    {
        elapsedFrameTime = min(timeLeft, GameConstants::Physics::FrameLength);
        timeLeft -= elapsedFrameTime;

        // Update the player position.
        m_player->Position(m_player->VectorPosition() + m_player->VectorVelocity() * elapsedFrameTime);

        // Do m_player / object intersections.
        for (uint32 a = 0; a < m_objects.size(); a++)
        {
            if (m_objects[a]->Active() && m_objects[a] != m_player)
            {
                XMFLOAT3 contact;
                XMFLOAT3 normal;

                if (m_objects[a]->IsTouching(m_player->Position(), m_player->Radius(), &contact, &normal))
                {
                    // Player is in contact with m_objects[a].
                    XMVECTOR oneToTwo;
                    oneToTwo = -XMLoadFloat3(&normal);

                    float impact = XMVectorGetX(
                        XMVector3Dot (oneToTwo, m_player->VectorVelocity())
                        );
                    // Make sure that the player is actually headed towards the object. At grazing angles there
                    // could appear to be an impact when the player is actually already hit and moving away.
                    if (impact > 0.0f)
                    {
                        // Compute the normal and tangential components of the player's velocity.
                        XMVECTOR velocityOneNormal = XMVector3Dot(oneToTwo, m_player->VectorVelocity()) * oneToTwo;
                        XMVECTOR velocityOneTangent = m_player->VectorVelocity() - velocityOneNormal;

                        // Compute post-collision velocity.
                        m_player->Velocity(velocityOneTangent - velocityOneNormal);

                        // Fix the positions so that the player is just touching the object.
                        float distanceToMove = m_player->Radius();
                        m_player->Position(XMLoadFloat3(&contact) - (oneToTwo * distanceToMove));
                    }
                }
            }
        }
        {
            // Do collision detection of the player with the bounding world.
            XMFLOAT3 position = m_player->Position();
            XMFLOAT3 velocity = m_player->Velocity();
            float radius = m_player->Radius();

            // Check for player collisions with the walls floor or ceiling and adjust the position.
            float limit = m_minBound.x + radius;
            if (position.x < limit)
            {
                position.x = limit;
                velocity.x = -velocity.x * GameConstants::Physics::GroundRestitution;
            }
            limit = m_maxBound.x - radius;
            if (position.x > limit)
            {
                position.x = limit;
                velocity.x = -velocity.x + GameConstants::Physics::GroundRestitution;
            }
            limit = m_minBound.y + radius;
            if (position.y < limit)
            {
                position.y = limit;
                velocity.y = -velocity.y * GameConstants::Physics::GroundRestitution;
            }
            limit = m_maxBound.y - radius;
            if (position.y > limit)
            {
                position.y = limit;
                velocity.y = -velocity.y * GameConstants::Physics::GroundRestitution;
            }
            limit = m_minBound.z + radius;
            if (position.z < limit)
            {
                position.z = limit;
                velocity.z = -velocity.z * GameConstants::Physics::GroundRestitution;
            }
            limit = m_maxBound.z - radius;
            if (position.z > limit)
            {
                position.z = limit;
                velocity.z = -velocity.z * GameConstants::Physics::GroundRestitution;
            }
            m_player->Position(position);
            m_player->Velocity(velocity);
        }

        // Animate the ammo.
        if (m_ammoCount > 0)
        {
            // Check for collisions between ammo.
#pragma region inter-ammo collision detection
            if (m_ammoCount > 1)
            {
                for (uint32 one = 0; one < m_ammoCount; one++)
                {
                    for (uint32 two = (one + 1); two < m_ammoCount; two++)
                    {
                        // Check collision between instances One and Two.
                        // OneToTwo is the vector between the centers of the two ammo that are being checked.
                        XMVECTOR oneToTwo;
                        oneToTwo = m_ammo[two]->VectorPosition() - m_ammo[one]->VectorPosition();
                        float distanceSquared;
                        distanceSquared = XMVectorGetX(
                            XMVector3LengthSq(oneToTwo)
                            );
                        if (distanceSquared < (GameConstants::AmmoSize * GameConstants::AmmoSize))
                        {
                            // The two ammo are intersecting.
                            oneToTwo = XMVector3Normalize(oneToTwo);

                            // Check if the two instances are already moving away from each other.
                            // If so, skip collision.  This can happen when a lot of instances are
                            // bunched up next to each other.
                            float impact;
                            impact = XMVectorGetX(
                                XMVector3Dot(oneToTwo, m_ammo[one]->VectorVelocity()) -
                                XMVector3Dot(oneToTwo, m_ammo[two]->VectorVelocity())
                                );
                            if (impact > 0.0f)
                            {
                                // Compute the normal and tangential components of one's velocity.
                                XMVECTOR velocityOne = (1 - GameConstants::Physics::BounceLost) * m_ammo[one]->VectorVelocity();
                                XMVECTOR velocityOneNormal = XMVector3Dot(oneToTwo, velocityOne) * oneToTwo;
                                XMVECTOR velocityOneTangent = velocityOne - velocityOneNormal;
                                // Compute the normal and tangential components of two's velocity.
                                XMVECTOR velocityTwo = (1 - GameConstants::Physics::BounceLost) * m_ammo[two]->VectorVelocity();
                                XMVECTOR velocityTwoNormal = XMVector3Dot(oneToTwo, velocityTwo) * oneToTwo;
                                XMVECTOR velocityTwoTangent = velocityTwo - velocityTwoNormal;

                                // Compute the post-collision velocities.
                                m_ammo[one]->Velocity(velocityOneTangent - velocityOneNormal * (1 - GameConstants::Physics::BounceTransfer) +
                                    velocityTwoNormal * GameConstants::Physics::BounceTransfer
                                    );
                                m_ammo[two]->Velocity(velocityTwoTangent - velocityTwoNormal * (1 - GameConstants::Physics::BounceTransfer) +
                                    velocityOneNormal * GameConstants::Physics::BounceTransfer
                                    );

                                // Fix the positions so that the two balls are exactly GameConstants::AmmoSize apart.
                                float distanceToMove = (GameConstants::AmmoSize - sqrtf(distanceSquared)) * 0.5f;
                                m_ammo[one]->Position(m_ammo[one]->VectorPosition() - (oneToTwo * distanceToMove));
                                m_ammo[two]->Position(m_ammo[two]->VectorPosition() + (oneToTwo * distanceToMove));

                                // Flag the two instances so that they are not laying on ground.
                                m_ammo[one]->OnGround(false);
                                m_ammo[two]->OnGround(false);

                                // Start playing the sounds for the impact between the two balls.
                                m_ammo[one]->PlaySound(impact, m_player->Position());
                                m_ammo[two]->PlaySound(impact, m_player->Position());
                            }
                        }
                    }
                }
            }
#pragma endregion

#pragma region Ammo-Object intersections
            // Check for intersections between the ammo and the other objects in the scene.
            for (uint32 one = 0; one < m_ammoCount; one++)
            {
                if (m_objects.size() > 0)
                {
                    if (!m_ammo[one]->OnGround())
                    {
                        for (uint32 i = 0; i < m_objects.size(); i++)
                        {
                            if (m_objects[i]->Active())
                            {
                                // The object is currently active in the scene. There may be objects in the list
                                // that are currently inactive, so those are skipped.
                                XMFLOAT3 contact;
                                XMFLOAT3 normal;

                                if (m_objects[i]->IsTouching(m_ammo[one]->Position(), GameConstants::AmmoRadius, &contact, &normal))
                                {
                                    // Ball is in contact with Object.
                                    XMVECTOR oneToTwo;
                                    oneToTwo = -XMLoadFloat3(&normal);

                                    float impact;
                                    impact = XMVectorGetX(
                                        XMVector3Dot (oneToTwo, m_ammo[one]->VectorVelocity())
                                        );
                                    // Make sure that the ball is actually headed towards the object. At grazing angles there
                                    // could appear to be an impact when the ball is actually already hit and moving away.
                                    if (impact > 0.0f)
                                    {
                                        // Compute the normal and tangential components of the ammo's velocity.
                                        XMVECTOR velocityOne = (1 - GameConstants::Physics::BounceLost) * m_ammo[one]->VectorVelocity();
                                        XMVECTOR velocityOneNormal = XMVector3Dot(oneToTwo, velocityOne) * oneToTwo;
                                        XMVECTOR velocityOneTangent = velocityOne - velocityOneNormal;

                                        // Compute post-collision velocity of the ammo.
                                        m_ammo[one]->Velocity(velocityOneTangent - velocityOneNormal * (1 - GameConstants::Physics::BounceTransfer));

                                        // Fix the position so that the ball is exactly GameConstants::AmmoRadius from target.
                                        float distanceToMove = GameConstants::AmmoSize;
                                        m_ammo[one]->Position(XMLoadFloat3(&contact) - (oneToTwo * distanceToMove));

                                        // Flag the Ammo as not laying on ground.
                                        m_ammo[one]->OnGround(false);

                                        // Play the sound associated with the Ammo hitting something.
                                        m_ammo[one]->PlaySound(impact, m_player->Position());

                                        if (m_objects[i]->Target() && !m_objects[i]->Hit())
                                        {
                                            // The object is a target and isn't currently hit, so mark it as hit and
                                            // play the sound associated with the impact.
                                            m_objects[i]->Hit(true);
                                            m_objects[i]->HitTime(timeTotal);
                                            m_totalHits++;

                                            m_objects[i]->PlaySound(impact, m_player->Position());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
#pragma endregion

#pragma region Apply Gravity and world intersection
            // Apply gravity and check for collision against enclosing volume.
            for (uint32 i = 0; i < m_ammoCount; i++)
            {
                // Update the position and velocity of the ammo instance.
                m_ammo[i]->Position(m_ammo[i]->VectorPosition() + m_ammo[i]->VectorVelocity() * elapsedFrameTime);

                XMFLOAT3 velocity = m_ammo[i]->Velocity();
                XMFLOAT3 position = m_ammo[i]->Position();

                velocity.x -= velocity.x * 0.1f * elapsedFrameTime;
                velocity.z -= velocity.z * 0.1f * elapsedFrameTime;
                if (!m_ammo[i]->OnGround())
                {
                    // Apply gravity if the ammo instance is not already resting on the ground.
                    velocity.y -= GameConstants::Physics::Gravity * elapsedFrameTime;
                }

                if (!m_ammo[i]->OnGround())
                {
                    float limit = m_minBound.y + GameConstants::AmmoRadius;
                    if (position.y < limit)
                    {
                        // The ammo instance hit the ground.
                        // Align the ammo instance to the ground, invert the Y component of the velocity and
                        // play the impact sound. The X and Z velocity components will be reduced
                        // because of friction.
                        position.y = limit;
                        m_ammo[i]->PlaySound(-velocity.y, m_player->Position());

                        velocity.y = -velocity.y * GameConstants::Physics::GroundRestitution;
                        velocity.x *= GameConstants::Physics::Friction;
                        velocity.z *= GameConstants::Physics::Friction;
                    }
                }
                else
                {
                    // The ammo instance is resting or rolling on ground.
                    // X and Z velocity components are reduced because of friction.
                    velocity.x *= GameConstants::Physics::Friction;
                    velocity.z *= GameConstants::Physics::Friction;
                }

                float limit = m_maxBound.y - GameConstants::AmmoRadius;
                if (position.y > limit)
                {
                    // The ammo instance hit the ceiling.
                    // Align the ammo instance to the ceiling, invert the Y component of the velocity and
                    // play the impact sound. The X and Z velocity components will be reduced
                    // because of friction.
                    position.y = limit;
                    m_ammo[i]->PlaySound(-velocity.y, m_player->Position());

                    velocity.y = -velocity.y * GameConstants::Physics::GroundRestitution;
                    velocity.x *= GameConstants::Physics::Friction;
                    velocity.z *= GameConstants::Physics::Friction;
                }

                limit = m_minBound.y + GameConstants::AmmoRadius;
                if ((GameConstants::Physics::Gravity * (position.y - limit) + 0.5f * velocity.y * velocity.y) < GameConstants::Physics::RestThreshold)
                {
                    // The Y velocity component is below the resting threshold so flag the instance as
                    // laying on the ground and set the Y velocity component to zero.
                    position.y = limit;
                    velocity.y = 0.0f;
                    m_ammo[i]->OnGround(true);
                }

                limit = m_minBound.z + GameConstants::AmmoRadius;
                if (position.z < limit)
                {
                    // The ammo instance hit the a wall in the min Z direction.
                    // Align the ammo instance to the wall, invert the Z component of the velocity and
                    // play the impact sound.
                    position.z = limit;
                    m_ammo[i]->PlaySound(-velocity.z, m_player->Position());
                    velocity.z = -velocity.z * GameConstants::Physics::GroundRestitution;
                }

                limit = m_maxBound.z - GameConstants::AmmoRadius;
                if (position.z > limit)
                {
                    // The ammo instance hit the a wall in the max Z direction.
                    // Align the ammo instance to the wall, invert the Z component of the velocity and
                    // play the impact sound.
                    position.z = limit;
                    m_ammo[i]->PlaySound(-velocity.z, m_player->Position());
                    velocity.z = -velocity.z * GameConstants::Physics::GroundRestitution;
                }

                limit = m_minBound.x + GameConstants::AmmoRadius;
                if (position.x < limit)
                {
                    // The ammo instance hit the a wall in the min X direction.
                    // Align the ammo instance to the wall, invert the X component of the velocity and
                    // play the impact sound.
                    position.x = limit;
                    m_ammo[i]->PlaySound(-velocity.x, m_player->Position());
                    velocity.x = -velocity.x * GameConstants::Physics::GroundRestitution;
                }

                limit = m_maxBound.x - GameConstants::AmmoRadius;
                if (position.x > limit)
                {
                    // The ammo instance hit the a wall in the max X direction.
                    // Align the ammo instance to the wall, invert the X component of the velocity and
                    // play the impact sound.
                    position.x = limit;
                    m_ammo[i]->PlaySound(-velocity.x, m_player->Position());
                    velocity.x = -velocity.x * GameConstants::Physics::GroundRestitution;
                }

                // Save the updated velocity and position for the ammo instance.
                m_ammo[i]->Velocity(velocity);
                m_ammo[i]->Position(position);
            }
        }
    }
#pragma endregion
}

//----------------------------------------------------------------------

void Simple3DGame::SaveState()
{
    // Save basic state of the game.
    m_savedState->SaveBool(":GameActive", m_gameActive);
    m_savedState->SaveBool(":LevelActive", m_levelActive);
    m_savedState->SaveInt32(":LevelCompleted", m_currentLevel);
    m_savedState->SaveInt32(":TotalShots", m_totalShots);
    m_savedState->SaveInt32(":TotalHits", m_totalHits);
    m_savedState->SaveSingle(":BonusRoundTime", m_levelBonusTime);
    m_savedState->SaveXMFLOAT3(":PlayerPosition", m_player->Position());
    m_savedState->SaveXMFLOAT3(":PlayerLookDirection", m_controller->LookDirection());

    if (m_levelActive)
    {
        // The game is currently in the middle of a level, so save the extended state of
        // the game.
        m_savedState->SaveSingle(":LevelDuration", m_levelDuration);
        m_savedState->SaveSingle(":LevelPlayingTime", m_timer->PlayingTime());

        m_savedState->SaveInt32(":AmmoCount", m_ammoCount);
        m_savedState->SaveInt32(":AmmoNext", m_ammoNext);

        const int bufferLength = 16;
        char16 str[bufferLength];

        for (uint32 i = 0; i < m_ammoCount; i++)
        {
            int len = swprintf_s(str, bufferLength, L"%d", i);
            Platform::String^ string = ref new Platform::String(str, len);

            m_savedState->SaveBool(Platform::String::Concat(":AmmoActive", string), m_ammo[i]->Active());
            m_savedState->SaveXMFLOAT3(Platform::String::Concat(":AmmoPosition", string), m_ammo[i]->Position());
            m_savedState->SaveXMFLOAT3(Platform::String::Concat(":AmmoVelocity", string), m_ammo[i]->Velocity());
        }

        m_savedState->SaveInt32(":ObjectCount", static_cast<int>(m_objects.size()));

        for (uint32 i = 0; i < m_objects.size(); i++)
        {
            int len = swprintf_s(str, bufferLength, L"%d", i);
            Platform::String^ string = ref new Platform::String(str, len);

            m_savedState->SaveBool(Platform::String::Concat(":ObjectActive", string), m_objects[i]->Active());
            m_savedState->SaveBool(Platform::String::Concat(":ObjectTarget", string), m_objects[i]->Target());
            m_savedState->SaveXMFLOAT3(Platform::String::Concat(":ObjectPosition", string), m_objects[i]->Position());
        }

        m_level[m_currentLevel]->SaveState(m_savedState);
    }
}

//----------------------------------------------------------------------

void Simple3DGame::LoadState()
{
    m_gameActive = m_savedState->LoadBool(":GameActive", m_gameActive);
    m_levelActive = m_savedState->LoadBool(":LevelActive", m_levelActive);

    if (m_gameActive)
    {
        // Loading from the last known state means the game wasn't finished when it was last played,
        // so set the current level.

        m_totalShots = m_savedState->LoadInt32(":TotalShots", 0);
        m_totalHits = m_savedState->LoadInt32(":TotalHits", 0);
        m_currentLevel = m_savedState->LoadInt32(":LevelCompleted", 0);
        m_levelBonusTime = m_savedState->LoadSingle(":BonusRoundTime", 0.0f);

        m_levelTimeRemaining = m_levelBonusTime;

        // Reload the current player position and set both the camera and the controller
        // with the current Look Direction.
        m_player->Position(
            m_savedState->LoadXMFLOAT3(":PlayerPosition", XMFLOAT3(0.0f, 0.0f, 0.0f))
            );
        m_camera->Eye(m_player->Position());
        m_camera->LookDirection(
            m_savedState->LoadXMFLOAT3(":PlayerLookDirection", XMFLOAT3(0.0f, 0.0f, 1.0f))
            );
        m_controller->Pitch(m_camera->Pitch());
        m_controller->Yaw(m_camera->Yaw());
    }
    else
    {
        // The game was not being played when it was last saved, so initialize to the beginning.
        m_currentLevel = 0;
        m_levelBonusTime = 0;
    }

    if (m_currentLevel >= m_levelCount)
    {
        // The current level is not valid so, reset to a known state and abandon the current game.
        m_currentLevel = 0;
        m_levelBonusTime = 0;
        m_gameActive = false;
        m_levelActive = false;
    }
}

//----------------------------------------------------------------------

void Simple3DGame::SetCurrentLevelToSavedState()
{
    if (m_gameActive)
    {
        if (m_levelActive)
        {
            // Middle of a level so restart where left off.
            m_levelDuration = m_savedState->LoadSingle(":LevelDuration", 0.0f);

            m_timer->Reset();
            m_timer->PlayingTime(m_savedState->LoadSingle(":LevelPlayingTime", 0.0f));

            m_ammoCount = m_savedState->LoadInt32(":AmmoCount", 0);

            m_ammoNext = m_savedState->LoadInt32(":AmmoNext", 0);

            const int bufferLength = 16;
            char16 str[bufferLength];

            for (uint32 i = 0; i < m_ammoCount; i++)
            {
                int len = swprintf_s(str, bufferLength, L"%d", i);
                Platform::String^ string = ref new Platform::String(str, len);

                m_ammo[i]->Active(
                    m_savedState->LoadBool(
                        Platform::String::Concat(":AmmoActive", string),
                        m_ammo[i]->Active()
                        )
                    );
                if (m_ammo[i]->Active())
                {
                    m_ammo[i]->OnGround(false);
                }

                m_ammo[i]->Position(
                    m_savedState->LoadXMFLOAT3(
                        Platform::String::Concat(":AmmoPosition", string),
                        m_ammo[i]->Position()
                        )
                    );

                m_ammo[i]->Velocity(
                    m_savedState->LoadXMFLOAT3(
                        Platform::String::Concat(":AmmoVelocity", string),
                        m_ammo[i]->Velocity()
                        )
                    );
            }

            int storedObjectCount = 0;
            storedObjectCount = m_savedState->LoadInt32(":ObjectCount", 0);

            storedObjectCount = min(storedObjectCount, static_cast<int>(m_objects.size()));

            for (int i = 0; i < storedObjectCount; i++)
            {
                int len = swprintf_s(str, bufferLength, L"%d", i);
                Platform::String^ string = ref new Platform::String(str, len);

                m_objects[i]->Active(
                    m_savedState->LoadBool(
                        Platform::String::Concat(":ObjectActive", string),
                        m_objects[i]->Active()
                        )
                    );

                m_objects[i]->Target(
                    m_savedState->LoadBool(
                        Platform::String::Concat(":ObjectTarget", string),
                        m_objects[i]->Target()
                        )
                    );

                m_objects[i]->Position(
                    m_savedState->LoadXMFLOAT3(
                        Platform::String::Concat(":ObjectPosition", string),
                        m_objects[i]->Position()
                        )
                    );
            }

            m_level[m_currentLevel]->LoadState(m_savedState);
            m_levelTimeRemaining = m_level[m_currentLevel]->TimeLimit() + m_levelBonusTime - m_timer->PlayingTime();
        }
    }
}

//----------------------------------------------------------------------

void Simple3DGame::SaveHighScore()
{
    m_savedState->SaveInt32(":HighScore:LevelCompleted", m_topScore.levelCompleted);
    m_savedState->SaveInt32(":HighScore:TotalShots", m_topScore.totalShots);
    m_savedState->SaveInt32(":HighScore:TotalHits", m_topScore.totalHits);
}

//----------------------------------------------------------------------

void Simple3DGame::LoadHighScore()
{
    m_topScore.levelCompleted = m_savedState->LoadInt32(":HighScore:LevelCompleted", 0);
    m_topScore.totalShots = m_savedState->LoadInt32(":HighScore:TotalShots", 0);
    m_topScore.totalHits = m_savedState->LoadInt32(":HighScore:TotalHits", 0);
}

//----------------------------------------------------------------------

void Simple3DGame::InitializeAmmo()
{
    m_ammoCount = 0;
    m_ammoNext = 0;
    for (uint32 i = 0; i < GameConstants::MaxAmmo; i++)
    {
        m_ammo[i]->Active(false);
    }
}

//----------------------------------------------------------------------

void Simple3DGame::InitializeGameConfig()
{
    m_gameConfig.isTrial = true;
    m_gameConfig.autoFire = false;
    m_controller->AutoFire(false);
    m_activeBackground = 0;
    m_gameConfig.backgroundAvailable[0] = true;
    for (int i = 1; i < GameConstants::MaxBackgroundTextures; i++)
    {
        m_gameConfig.backgroundAvailable[i] = false;
    }
}

//--------------------------------------------------------------------------------------

void Simple3DGame::UpdateGameConfig(LicenseInformation^ licenseInformation)
{
    if (licenseInformation->IsActive)
    {
        m_gameConfig.isTrial = licenseInformation->IsTrial;
        if (!m_gameConfig.isTrial && licenseInformation->ProductLicenses->Lookup("AutoFire")->IsActive)
        {
            m_gameConfig.autoFire = true;
            m_controller->AutoFire(true);
        }
        else
        {
            m_gameConfig.autoFire = false;
            m_controller->AutoFire(false);
        }
        if (!m_gameConfig.isTrial && licenseInformation->ProductLicenses->Lookup("NightBackground")->IsActive)
        {
            m_gameConfig.backgroundAvailable[1] = true;
        }
        else
        {
            m_gameConfig.backgroundAvailable[1] = false;
        }
        if (!m_gameConfig.isTrial && licenseInformation->ProductLicenses->Lookup("DayBackground")->IsActive)
        {
            m_gameConfig.backgroundAvailable[2] = true;
        }
        else
        {
            m_gameConfig.backgroundAvailable[2] = false;
        }
    }
    else
    {
        // If no active license then default back to trial version.
        InitializeGameConfig();
    }

    if (m_gameConfig.isTrial)
    {
        if (m_level.size() > 2)
        {
            m_level.erase(m_level.begin() + 2, m_level.end());
        }
    }
    else
    {
        if (m_level.size() == 2)
        {
            m_level.push_back(ref new Level3);
            m_level.push_back(ref new Level4);
            m_level.push_back(ref new Level5);
            m_level.push_back(ref new Level6);
        }
    }
    m_levelCount = static_cast<uint32>(m_level.size());
}

//--------------------------------------------------------------------------------------

void Simple3DGame::SetBackground(uint32 background)
{
    if (background < GameConstants::MaxBackgroundTextures)
    {
        if (m_gameConfig.backgroundAvailable[background])
        {
            m_activeBackground = background;
            m_renderer->SetBackground(background);
        }
    }
}

//--------------------------------------------------------------------------------------

void Simple3DGame::CycleBackground()
{
    // There may be gaps in the background textures list that are available for use because the
    // user has not purchased all them in order or at all.
    // Cycle through the list looking for the next one that is valid.  This may require wrapping
    // back to the beginning.

    unsigned int newBackground = m_activeBackground;

    while (newBackground < GameConstants::MaxBackgroundTextures)
    {
        newBackground++;
        if (newBackground >= GameConstants::MaxBackgroundTextures)
        {
            newBackground = 0;
            break;
        }
        else if (m_gameConfig.backgroundAvailable[newBackground])
        {
            break;
        }
    }

    if (newBackground != m_activeBackground)
    {
        m_activeBackground = newBackground;
        m_renderer->SetBackground(newBackground);
    }
}
//--------------------------------------------------------------------------------------

