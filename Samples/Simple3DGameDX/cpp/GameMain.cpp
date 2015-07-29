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
#include "MoveLookController.h"
#include "GameMain.h"

using namespace GameControl;
using namespace Simple3DGameDX;
using namespace concurrency;
using namespace DirectX;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::ViewManagement;
using namespace Windows::System::Threading;
using namespace Concurrency;


GameMain::GameMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    m_windowClosed(false),
    m_haveFocus(false),
    m_gameInfoOverlayCommand(GameInfoOverlayCommand::None),
    m_visible(true),
    m_loadingCount(0),
    m_updateState(UpdateEngineState::WaitingForResources)
{
    m_deviceResources->RegisterDeviceNotify(this);

    m_renderer = ref new GameRenderer(m_deviceResources);
    m_game = ref new Simple3DGame();

    m_uiControl = m_renderer->GameUIControl();

    m_controller = ref new MoveLookController(CoreWindow::GetForCurrentThread());

    auto bounds = m_deviceResources->GetLogicalSize();

    m_controller->SetMoveRect(
        XMFLOAT2(0.0f, bounds.Height - GameConstants::TouchRectangleSize),
        XMFLOAT2(GameConstants::TouchRectangleSize, bounds.Height)
        );
    m_controller->SetFireRect(
        XMFLOAT2(bounds.Width - GameConstants::TouchRectangleSize, bounds.Height - GameConstants::TouchRectangleSize),
        XMFLOAT2(bounds.Width, bounds.Height)
        );

    SetGameInfoOverlay(GameInfoOverlayState::Loading);
    m_uiControl->SetAction(GameInfoOverlayCommand::None);
    m_uiControl->ShowGameInfoOverlay();

    create_task([this]()
    {
        // Asynchronously initialize the game class and load the renderer device resources.
        // By doing all this asynchronously, the game gets to its main loop more quickly
        // and in parallel all the necessary resources are loaded on other threads.
        m_game->Initialize(m_controller, m_renderer);

        return m_renderer->CreateGameDeviceResourcesAsync(m_game);

    }).then([this]()
    {
        // The finalize code needs to run in the same thread context
        // as the m_renderer object was created because the D3D device context
        // can ONLY be accessed on a single thread.
        m_renderer->FinalizeCreateGameDeviceResources();

        InitializeGameState();

        if (m_updateState == UpdateEngineState::WaitingForResources)
        {
            // In the middle of a game so spin up the async task to load the level.
            return m_game->LoadLevelAsync().then([this]()
            {
                // The m_game object may need to deal with D3D device context work so
                // again the finalize code needs to run in the same thread
                // context as the m_renderer object was created because the D3D
                // device context can ONLY be accessed on a single thread.
                m_game->FinalizeLoadLevel();
                m_game->SetCurrentLevelToSavedState();
                m_updateState = UpdateEngineState::ResourcesLoaded;

            }, task_continuation_context::use_current());
        }
        else
        {
            // The game is not in the middle of a level so there aren't any level
            // resources to load.  Creating a no-op task so that in both cases
            // the same continuation logic is used.
            return create_task([]()
            {
            });
        }
    }, task_continuation_context::use_current()).then([this]()
    {
        // Since Game loading is an async task, the app visual state
        // may be too small or not have focus.  Put the state machine
        // into the correct state to reflect these cases.

        if (m_deviceResources->GetLogicalSize().Width < GameConstants::MinPlayableWidth)
        {
            m_updateStateNext = m_updateState;
            m_updateState = UpdateEngineState::TooSmall;
            m_controller->Active(false);
            m_uiControl->HideGameInfoOverlay();
            m_uiControl->ShowTooSmall();
            m_renderNeeded = true;
        }
        else if (!m_haveFocus)
        {
            m_updateStateNext = m_updateState;
            m_updateState = UpdateEngineState::Deactivated;
            m_controller->Active(false);
            m_uiControl->SetAction(GameInfoOverlayCommand::None);
            m_renderNeeded = true;
        }
    }, task_continuation_context::use_current());
}

//--------------------------------------------------------------------------------------

GameMain::~GameMain()
{
    // Deregister device notification
    m_deviceResources->RegisterDeviceNotify(nullptr);
}

//--------------------------------------------------------------------------------------

void GameMain::Run()
{
    while (!m_windowClosed)
    {
        if (m_visible)
        {
            switch (m_updateState)
            {
            case UpdateEngineState::Deactivated:
            case UpdateEngineState::TooSmall:
                if (m_updateStateNext == UpdateEngineState::WaitingForResources)
                {
                    WaitingForResourceLoading();
                    m_renderNeeded = true;
                }
                else if (m_updateStateNext == UpdateEngineState::ResourcesLoaded)
                {
                    // In the device lost case, we transition to the final waiting state
                    // and make sure the display is updated.
                    switch (m_pressResult)
                    {
                    case PressResultState::LoadGame:
                        SetGameInfoOverlay(GameInfoOverlayState::GameStats);
                        break;

                    case PressResultState::PlayLevel:
                        SetGameInfoOverlay(GameInfoOverlayState::LevelStart);
                        break;

                    case PressResultState::ContinueLevel:
                        SetGameInfoOverlay(GameInfoOverlayState::Pause);
                        break;
                    }
                    m_updateStateNext = UpdateEngineState::WaitingForPress;
                    m_uiControl->ShowGameInfoOverlay();
                    m_renderNeeded = true;
                }

                if (!m_renderNeeded)
                {
                    // The App is not currently the active window and not in a transient state so just wait for events.
                    CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
                    break;
                }
                // otherwise fall through and do normal processing to get the rendering handled.
            default:
                CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
                Update();
                m_renderer->Render();
                m_deviceResources->Present();
                m_renderNeeded = false;
            }
        }
        else
        {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
        }
    }
    m_game->OnSuspending();  // exiting due to window close.  Make sure to save state.
}

//--------------------------------------------------------------------------------------

void GameMain::InitializeGameState()
{
    // Set up the initial state machine for handling Game playing state.
    if (m_game->GameActive() && m_game->LevelActive())
    {
        // The last time the game terminated it was in the middle
        // of a level.
        // We are waiting for the user to continue the game.
        m_updateState = UpdateEngineState::WaitingForResources;
        m_pressResult = PressResultState::ContinueLevel;
        SetGameInfoOverlay(GameInfoOverlayState::Pause);
        m_uiControl->SetAction(GameInfoOverlayCommand::PleaseWait);
    }
    else if (!m_game->GameActive() && (m_game->HighScore().totalHits > 0))
    {
        // The last time the game terminated the game had been completed.
        // Show the high score.
        // We are waiting for the user to acknowledge the high score and start a new game.
        // The level resources for the first level will be loaded later.
        m_updateState = UpdateEngineState::WaitingForPress;
        m_pressResult = PressResultState::LoadGame;
        SetGameInfoOverlay(GameInfoOverlayState::GameStats);
        m_controller->WaitForPress(m_renderer->GameInfoOverlayUpperLeft(), m_renderer->GameInfoOverlayLowerRight());
        m_uiControl->SetAction(GameInfoOverlayCommand::TapToContinue);
    }
    else
    {
        // This is either the first time the game has run or
        // the last time the game terminated the level was completed.
        // We are waiting for the user to begin the next level.
        m_updateState = UpdateEngineState::WaitingForResources;
        m_pressResult = PressResultState::PlayLevel;
        SetGameInfoOverlay(GameInfoOverlayState::LevelStart);
        m_uiControl->SetAction(GameInfoOverlayCommand::PleaseWait);
    }
    m_uiControl->ShowGameInfoOverlay();
}

//--------------------------------------------------------------------------------------

void GameMain::WaitingForResourceLoading()
{
    static uint32 loadCount = 0;

    // Waiting for initial load.  Display an update once per 60 updates.
    if (((loadCount++) % 60) == 0)
    {
        m_loadingCount++;
        SetGameInfoOverlay(m_gameInfoOverlayState);
        m_renderNeeded = true;
    }
}

//--------------------------------------------------------------------------------------

void GameMain::Update()
{
    m_controller->Update();

    switch (m_updateState)
    {
    case UpdateEngineState::WaitingForResources:
        WaitingForResourceLoading();
        break;

    case UpdateEngineState::ResourcesLoaded:
        switch (m_pressResult)
        {
        case PressResultState::LoadGame:
            SetGameInfoOverlay(GameInfoOverlayState::GameStats);
            break;

        case PressResultState::PlayLevel:
            SetGameInfoOverlay(GameInfoOverlayState::LevelStart);
            break;

        case PressResultState::ContinueLevel:
            SetGameInfoOverlay(GameInfoOverlayState::Pause);
            break;
        }
        m_updateState = UpdateEngineState::WaitingForPress;
        m_uiControl->SetAction(GameInfoOverlayCommand::TapToContinue);
        m_controller->WaitForPress(m_renderer->GameInfoOverlayUpperLeft(), m_renderer->GameInfoOverlayLowerRight());
        m_uiControl->ShowGameInfoOverlay();
        m_renderNeeded = true;
        break;

    case UpdateEngineState::WaitingForPress:
        if (m_controller->IsPressComplete())
        {
            switch (m_pressResult)
            {
            case PressResultState::LoadGame:
                m_updateState = UpdateEngineState::WaitingForResources;
                m_pressResult = PressResultState::PlayLevel;
                m_controller->Active(false);
                m_game->LoadGame();
                m_uiControl->SetAction(GameInfoOverlayCommand::PleaseWait);
                SetGameInfoOverlay(GameInfoOverlayState::LevelStart);
                m_uiControl->ShowGameInfoOverlay();

                m_game->LoadLevelAsync().then([this]()
                {
                    m_game->FinalizeLoadLevel();
                    m_updateState = UpdateEngineState::ResourcesLoaded;
                }, task_continuation_context::use_current());
                break;

            case PressResultState::PlayLevel:
                m_updateState = UpdateEngineState::Dynamics;
                m_uiControl->HideGameInfoOverlay();
                m_controller->Active(true);
                m_game->StartLevel();
                break;

            case PressResultState::ContinueLevel:
                m_updateState = UpdateEngineState::Dynamics;
                m_uiControl->HideGameInfoOverlay();
                m_controller->Active(true);
                m_game->ContinueGame();
                break;
            }
        }
        break;

    case UpdateEngineState::Dynamics:
        if (m_controller->IsPauseRequested())
        {
            m_game->PauseGame();
            SetGameInfoOverlay(GameInfoOverlayState::Pause);
            m_uiControl->SetAction(GameInfoOverlayCommand::TapToContinue);
            m_updateState = UpdateEngineState::WaitingForPress;
            m_pressResult = PressResultState::ContinueLevel;
            m_uiControl->ShowGameInfoOverlay();
        }
        else
        {
            GameState runState = m_game->RunGame();
            switch (runState)
            {
            case GameState::TimeExpired:
                m_uiControl->SetAction(GameInfoOverlayCommand::TapToContinue);
                SetGameInfoOverlay(GameInfoOverlayState::GameOverExpired);
                m_uiControl->ShowGameInfoOverlay();
                m_updateState = UpdateEngineState::WaitingForPress;
                m_pressResult = PressResultState::LoadGame;
                break;

            case GameState::LevelComplete:
                m_uiControl->SetAction(GameInfoOverlayCommand::PleaseWait);
                SetGameInfoOverlay(GameInfoOverlayState::LevelStart);
                m_uiControl->ShowGameInfoOverlay();
                m_updateState = UpdateEngineState::WaitingForResources;
                m_pressResult = PressResultState::PlayLevel;

                m_game->LoadLevelAsync().then([this]()
                {
                    m_game->FinalizeLoadLevel();
                    m_updateState = UpdateEngineState::ResourcesLoaded;

                }, task_continuation_context::use_current());
                break;

            case GameState::GameComplete:
                m_uiControl->SetAction(GameInfoOverlayCommand::TapToContinue);
                SetGameInfoOverlay(GameInfoOverlayState::GameOverCompleted);
                m_uiControl->ShowGameInfoOverlay();
                m_updateState = UpdateEngineState::WaitingForPress;
                m_pressResult = PressResultState::LoadGame;
                break;
            }
        }

        if (m_updateState == UpdateEngineState::WaitingForPress)
        {
            // Transitioning state, so enable waiting for the press event
            m_controller->WaitForPress(m_renderer->GameInfoOverlayUpperLeft(), m_renderer->GameInfoOverlayLowerRight());
        }
        if (m_updateState == UpdateEngineState::WaitingForResources)
        {
            // Transitioning state, so shut down the input controller until resources are loaded
            m_controller->Active(false);
        }
        break;
    }
}

//--------------------------------------------------------------------------------------

void GameMain::CreateWindowSizeDependentResources()
{
    UpdateLayoutState();
    m_renderer->CreateWindowSizeDependentResources();

    // The location of the Control regions may have changed with the size change so update the controller.
    auto bounds = m_deviceResources->GetLogicalSize();

    m_controller->SetMoveRect(
        XMFLOAT2(0.0f, bounds.Height - GameConstants::TouchRectangleSize),
        XMFLOAT2(GameConstants::TouchRectangleSize, bounds.Height)
        );
    m_controller->SetFireRect(
        XMFLOAT2(bounds.Width - GameConstants::TouchRectangleSize, bounds.Height - GameConstants::TouchRectangleSize),
        XMFLOAT2(bounds.Width, bounds.Height)
        );

    if (m_updateState == UpdateEngineState::WaitingForPress)
    {
        // A side effect of m_renderer->UpdateForWindowSizeChange() is that the GameInfoOverlay rectangle may have
        // changed as a result of layout, so make sure the WaitForPress rectangle is set to the new size.
        m_controller->WaitForPress(m_renderer->GameInfoOverlayUpperLeft(), m_renderer->GameInfoOverlayLowerRight());
    }

    m_renderNeeded = true;
}

//--------------------------------------------------------------------------------------

void GameMain::OnDeviceLost()
{
    if (m_updateState == UpdateEngineState::Dynamics)
    {
        // Game is in the active game play state, Stop Game Timer and Pause play and save state
        m_game->PauseGame();
    }
    m_renderNeeded = false;
    m_renderer->ReleaseDeviceDependentResources();
}

//--------------------------------------------------------------------------------------

void GameMain::OnDeviceRestored()
{
    m_renderer->CreateDeviceDependentResources();
    m_renderer->CreateWindowSizeDependentResources();

    m_loadingCount = 0;
    m_uiControl->SetAction(GameInfoOverlayCommand::PleaseWait);
    SetGameInfoOverlay(GameInfoOverlayState::Loading);
    m_updateState = UpdateEngineState::WaitingForResources;
    m_renderNeeded = true;

    create_task([this]()
    {
        return m_renderer->CreateGameDeviceResourcesAsync(m_game);

    }).then([this]()
    {
        // The finalize code needs to run in the same thread context
        // as the m_renderer object was created because the D3D device context
        // can ONLY be accessed on a single thread.
        m_renderer->FinalizeCreateGameDeviceResources();

        // Reset the main state machine based on the current game state now
        // that the device resources have been restored.
        InitializeGameState();

        if (m_updateState == UpdateEngineState::WaitingForResources)
        {
            // In the middle of a game so spin up the async task to load the level.
            return m_game->LoadLevelAsync().then([this]()
            {
                // The m_game object may need to deal with D3D device context work so
                // again the finalize code needs to run in the same thread
                // context as the m_renderer object was created because the D3D
                // device context can ONLY be accessed on a single thread.
                m_game->FinalizeLoadLevel();
                m_game->SetCurrentLevelToSavedState();
                m_updateState = UpdateEngineState::ResourcesLoaded;
                m_renderNeeded = true;

            }, task_continuation_context::use_current());
        }
        else
        {
            // The game is not in the middle of a level so there aren't any level
            // resources to load.  Creating a no-op task so that in both cases
            // the same continuation logic is used.
            return create_task([]()
            {
            });
        }

    }, task_continuation_context::use_current()).then([this]()
    {
        // Since Device lost is an unexpected event, the app visual state
        // may be TooSmall or not have focus.  Put the state machine
        // into the correct state to reflect these cases.
        if (CoreWindow::GetForCurrentThread()->Bounds.Width < GameConstants::MinPlayableWidth)
        {
            m_updateStateNext = m_updateState;
            m_updateState = UpdateEngineState::TooSmall;
            m_controller->Active(false);
            m_uiControl->HideGameInfoOverlay();
            m_uiControl->ShowTooSmall();
            m_renderNeeded = true;
        }
        else if (!m_haveFocus)
        {
            m_updateStateNext = m_updateState;
            m_updateState = UpdateEngineState::Deactivated;
            m_controller->Active(false);
            m_uiControl->SetAction(GameInfoOverlayCommand::None);
            m_renderNeeded = true;
        }
    }, task_continuation_context::use_current());
}

//--------------------------------------------------------------------------------------

void GameMain::UpdateLayoutState()
{
    m_renderNeeded = true;

    if (m_deviceResources->GetLogicalSize().Width < GameConstants::MinPlayableWidth)
    {
        switch (m_updateState)
        {
        case UpdateEngineState::Dynamics:
            // From Dynamic mode, when coming out of TooSmall layout rather than going directly back into game play
            // go to the paused state waiting for user input to continue
            m_updateStateNext = UpdateEngineState::WaitingForPress;
            m_pressResult = PressResultState::ContinueLevel;
            SetGameInfoOverlay(GameInfoOverlayState::Pause);
            m_uiControl->SetAction(GameInfoOverlayCommand::TapToContinue);
            m_game->PauseGame();
            break;

        case UpdateEngineState::WaitingForResources:
            // There is outstanding async loading in progress.  This state change will be picked up after the
            // async task is completed.
            return;

        case UpdateEngineState::WaitingForPress:
            // Avoid corrupting the m_updateStateNext on a transition from TooSmall -> TooSmall.
            // Otherwise just cache the current state and return to it when leaving TooSmall layout

            m_updateStateNext = m_updateState;
            break;

        default:
            break;
        }

        m_updateState = UpdateEngineState::TooSmall;
        m_controller->Active(false);
        m_uiControl->HideGameInfoOverlay();
        m_uiControl->ShowTooSmall();
    }
    else
    {
        if (m_updateState == UpdateEngineState::TooSmall)
        {
            m_uiControl->HideTooSmall();
            m_uiControl->ShowGameInfoOverlay();
            m_renderNeeded = true;

            if (m_haveFocus)
            {
                if (m_updateStateNext == UpdateEngineState::WaitingForPress)
                {
                    m_uiControl->SetAction(GameInfoOverlayCommand::TapToContinue);
                }
                else if (m_updateStateNext == UpdateEngineState::WaitingForResources)
                {
                    m_uiControl->SetAction(GameInfoOverlayCommand::PleaseWait);
                }

                m_updateState = m_updateStateNext;
            }
            else
            {
                m_updateState = UpdateEngineState::Deactivated;
                m_uiControl->SetAction(GameInfoOverlayCommand::None);
            }
        }
        else if (m_updateState == UpdateEngineState::Dynamics)
        {
            // In Dynamic mode, when a resize event happens, go to the paused state waiting for user input to continue.
            m_pressResult = PressResultState::ContinueLevel;
            SetGameInfoOverlay(GameInfoOverlayState::Pause);
            m_game->PauseGame();
            if (m_haveFocus)
            {
                m_updateState = UpdateEngineState::WaitingForPress;
                m_uiControl->SetAction(GameInfoOverlayCommand::TapToContinue);
            }
            else
            {
                m_updateState = UpdateEngineState::Deactivated;
                m_uiControl->SetAction(GameInfoOverlayCommand::None);
            }
            m_uiControl->ShowGameInfoOverlay();
            m_renderNeeded = true;
        }
    }
}

//--------------------------------------------------------------------------------------

void GameMain::ResetGame()
{
    m_updateState = UpdateEngineState::WaitingForResources;
    m_pressResult = PressResultState::PlayLevel;
    m_controller->Active(false);
    m_game->LoadGame();
    m_uiControl->SetAction(GameInfoOverlayCommand::PleaseWait);
    SetGameInfoOverlay(GameInfoOverlayState::LevelStart);
    m_uiControl->ShowGameInfoOverlay();
    m_renderNeeded = true;

    m_game->LoadLevelAsync().then([this]()
    {
        m_game->FinalizeLoadLevel();
        m_updateState = UpdateEngineState::ResourcesLoaded;
        m_renderNeeded = true;

    }, task_continuation_context::use_current());
}

//--------------------------------------------------------------------------------------

void GameMain::SetGameInfoOverlay(GameInfoOverlayState state)
{
    m_gameInfoOverlayState = state;
    switch (state)
    {
    case GameInfoOverlayState::Loading:
        m_uiControl->SetGameLoading(m_loadingCount);
        break;

    case GameInfoOverlayState::GameStats:
        m_uiControl->SetGameStats(
            m_game->HighScore().levelCompleted + 1,
            m_game->HighScore().totalHits,
            m_game->HighScore().totalShots
            );
        break;

    case GameInfoOverlayState::LevelStart:
        m_uiControl->SetLevelStart(
            m_game->LevelCompleted() + 1,
            m_game->CurrentLevel()->Objective(),
            m_game->CurrentLevel()->TimeLimit(),
            m_game->BonusTime()
            );
        break;

    case GameInfoOverlayState::GameOverCompleted:
        m_uiControl->SetGameOver(
            true,
            m_game->LevelCompleted() + 1,
            m_game->TotalHits(),
            m_game->TotalShots(),
            m_game->HighScore().totalHits
            );
        break;

    case GameInfoOverlayState::GameOverExpired:
        m_uiControl->SetGameOver(
            false,
            m_game->LevelCompleted(),
            m_game->TotalHits(),
            m_game->TotalShots(),
            m_game->HighScore().totalHits
            );
        break;

    case GameInfoOverlayState::Pause:
        m_uiControl->SetPause(
            m_game->LevelCompleted() + 1,
            m_game->TotalHits(),
            m_game->TotalShots(),
            m_game->TimeRemaining()
            );
        break;
    }
}

//--------------------------------------------------------------------------------------

void GameMain::WindowActivationChanged(CoreWindowActivationState activationState)
{
    if (activationState == CoreWindowActivationState::Deactivated)
    {
        m_haveFocus = false;

        switch (m_updateState)
        {
        case UpdateEngineState::Dynamics:
            // From Dynamic mode, when coming out of Deactivated rather than going directly back into game play
            // go to the paused state waiting for user input to continue
            m_updateStateNext = UpdateEngineState::WaitingForPress;
            m_pressResult = PressResultState::ContinueLevel;
            SetGameInfoOverlay(GameInfoOverlayState::Pause);
            m_uiControl->ShowGameInfoOverlay();
            m_game->PauseGame();
            m_updateState = UpdateEngineState::Deactivated;
            m_uiControl->SetAction(GameInfoOverlayCommand::None);
            m_renderNeeded = true;
            break;

        case UpdateEngineState::WaitingForResources:
        case UpdateEngineState::WaitingForPress:
            m_updateStateNext = m_updateState;
            m_updateState = UpdateEngineState::Deactivated;
            m_uiControl->SetAction(GameInfoOverlayCommand::None);
            m_uiControl->ShowGameInfoOverlay();
            m_renderNeeded = true;
            break;
        }
        // Don't have focus so shutdown input processing
        m_controller->Active(false);
    }
    else if (activationState == CoreWindowActivationState::CodeActivated
        || activationState == CoreWindowActivationState::PointerActivated)
    {
        m_haveFocus = true;

        if (m_updateState == UpdateEngineState::Deactivated)
        {
            m_updateState = m_updateStateNext;

            if (m_updateState == UpdateEngineState::WaitingForPress)
            {
                m_uiControl->SetAction(GameInfoOverlayCommand::TapToContinue);
                m_controller->WaitForPress(m_renderer->GameInfoOverlayUpperLeft(), m_renderer->GameInfoOverlayLowerRight());
            }
            else if (m_updateStateNext == UpdateEngineState::WaitingForResources)
            {
                m_uiControl->SetAction(GameInfoOverlayCommand::PleaseWait);
            }
        }
    }
}

//--------------------------------------------------------------------------------------

void GameMain::Suspend()
{
    // Save application state.
    switch (m_updateState)
    {
    case UpdateEngineState::Dynamics:
        // Game is in the active game play state, Stop Game Timer and Pause play and save state
        m_uiControl->SetAction(GameInfoOverlayCommand::None);
        SetGameInfoOverlay(GameInfoOverlayState::Pause);
        m_updateStateNext = UpdateEngineState::WaitingForPress;
        m_pressResult = PressResultState::ContinueLevel;
        m_game->PauseGame();
        break;

    case UpdateEngineState::WaitingForResources:
    case UpdateEngineState::WaitingForPress:
        m_updateStateNext = m_updateState;
        break;

    default:
        // any other state don't save as next state as they are trancient states and have already set m_updateStateNext
        break;
    }
    m_updateState = UpdateEngineState::Suspended;

    m_controller->Active(false);
    m_game->OnSuspending();
}

//--------------------------------------------------------------------------------------

void GameMain::Resume()
{
    if (m_haveFocus)
    {
        m_updateState = m_updateStateNext;
    }
    else
    {
        m_updateState = UpdateEngineState::Deactivated;
    }

    if (m_updateState == UpdateEngineState::WaitingForPress)
    {
        m_uiControl->SetAction(GameInfoOverlayCommand::TapToContinue);
        m_controller->WaitForPress(m_renderer->GameInfoOverlayUpperLeft(), m_renderer->GameInfoOverlayLowerRight());
    }
    m_game->OnResuming();
    m_uiControl->ShowGameInfoOverlay();
    m_renderNeeded = true;
}

//--------------------------------------------------------------------------------------

void GameMain::Visibility(bool visible)
{
    if (visible && !m_visible)
    {
        m_renderNeeded = true;
    }
    m_visible = visible;
}

//--------------------------------------------------------------------------------------

