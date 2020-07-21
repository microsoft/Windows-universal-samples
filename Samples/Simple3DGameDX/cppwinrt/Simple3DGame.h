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

#pragma once

// Simple3DGame:
// This is the main game class. It controls game play logic and game state.
// Some of the key object classes used by Simple3DGame are:
//     MoveLookController - for handling all input to control player movement, aiming,
//         and firing.
//     GameRenderer - for handling all graphics presentation.
//     Camera - for handling view projections.
//     Audio - for handling sound output.
// This class maintains several lists of objects:
//     m_ammo <Sphere> - is the list of the balls used to throw at targets. Simple3DGame
//         cycles through the list in a LRU fashion each time a ball is thrown by the player.
//     m_objects <GameObject> - is the list of all objects in the scene that participate in
//         game physics. This includes m_player <Sphere> to represent the player in the scene.
//         The player object (m_player) is not visible in the scene so it is not rendered.
//     m_renderObjects <GameObject> - is the list of all objects in the scene that may be
//         rendered. It includes both the m_ammo list, most of the m_objects list excluding m_player
//         object and the objects representing the bounding world.

#include "GameConstants.h"
#include "Audio.h"
#include "Camera.h"
#include "Level.h"
#include "GameTimer.h"
#include "Sphere.h"
#include "MoveLookController.h"

enum class GameState
{
    Waiting,
    Active,
    LevelComplete,
    TimeExpired,
    GameComplete,
};

struct HighScoreEntry
{
    std::wstring tag;
    int totalHits = 0;
    int totalShots = 0;
    int levelCompleted = 0;
};

class GameRenderer;

class Simple3DGame
{
public:
    Simple3DGame();

    void Initialize(
        _In_ std::shared_ptr<MoveLookController> const& controller,
        _In_ std::shared_ptr<GameRenderer> const& renderer
        );

    void LoadGame();
    winrt::Windows::Foundation::IAsyncAction LoadLevelAsync();
    void FinalizeLoadLevel();
    void StartLevel();
    void PauseGame();
    void ContinueGame();
    GameState RunGame();
    void SetCurrentLevelToSavedState();

    void OnSuspending();
    void OnResuming();

    bool IsActivePlay() { return m_timer.Active(); }
    int LevelCompleted() { return m_currentLevel; };
    int TotalShots() { return m_totalShots; };
    int TotalHits() { return m_totalHits; };
    float BonusTime() { return m_levelBonusTime; };
    bool GameActive() { return m_gameActive; };
    bool LevelActive() { return m_levelActive; };
    HighScoreEntry HighScore() { return m_topScore; };
    std::shared_ptr<Level> const& CurrentLevel() { return m_level[m_currentLevel]; };
    float TimeRemaining() { return m_levelTimeRemaining; };
    Camera& GameCamera() { return m_camera; };
    std::vector<std::shared_ptr<GameObject>> const& RenderObjects() { return m_renderObjects; };

private:
    void LoadState();
    void SaveState();
    void SaveHighScore();
    void LoadHighScore();
    void InitializeAmmo();
    void UpdateDynamics();

    std::shared_ptr<MoveLookController>      m_controller;
    std::shared_ptr<GameRenderer>            m_renderer;
    Camera                                   m_camera;

    Audio                                    m_audioController;

    std::vector<std::shared_ptr<Sphere>>     m_ammo;
    uint32_t                                 m_ammoCount;
    uint32_t                                 m_ammoNext;

    HighScoreEntry                           m_topScore;
    PersistentState                          m_savedState;

    GameTimer                                m_timer;
    bool                                     m_gameActive;
    bool                                     m_levelActive;
    int                                      m_totalHits;
    int                                      m_totalShots;
    float                                    m_levelDuration;
    float                                    m_levelBonusTime;
    float                                    m_levelTimeRemaining;
    std::vector<std::shared_ptr<Level>>      m_level;
    uint32_t                                 m_levelCount;
    uint32_t                                 m_currentLevel;

    std::shared_ptr<Sphere>                  m_player;
    std::vector<std::shared_ptr<GameObject>> m_objects;       // List of all objects to be included in intersection calculations.
    std::vector<std::shared_ptr<GameObject>> m_renderObjects; // List of all objects to be rendered.

    DirectX::XMFLOAT3                        m_minBound;
    DirectX::XMFLOAT3                        m_maxBound;
};