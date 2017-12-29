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

// GameMain:
// This class is the main class required for Windows Runtime apps.
// It it called on Launch activation and maintains the overall state of the game.
// The GameMain class drives and maintains a state machine for the game.  It can be in
// one of seven major states defined by the UpdateEngineState enum class.
// These are:
//     WaitingForResources - the game has requested the game object or the renderer object
//         to load resources asynchronously.
//     ResourcesLoaded - the asynchronous loading of resources has been completed.  This
//         is a transient state.
//     WaitingForPress - the game is waiting for the player to indicate they are ready to proceed.
//         There are three possible actions from this state.  This is controlled by m_pressResult.
//         The possible outcomes are:
//             LoadGame - The player is ready to start a new game and has acknowledged the status
//                 information provided about the previous state.
//             PlayLevel - The player is ready to play the next level.  The level has already been
//                 loaded so active game play will start.
//             ContinueLevel - The player is ready to continue playing the current level.  Part of the
//                 current level has already been played.
//     Dynamics - the game is active play mode.
//     TooSmall - the window is currently too small to play the game.
//     Suspended - the game was suspended by PLM.
//     Deactivated - the game has lost focus.
//
// GameMain creates and maintains references to three major objects used for the game:
//     MoveLookController (m_controller) - this object handles all the game specific user input and
//         aggregates touch, mouse/keyboard and Xbox controller input into a unified input control.
//     Simple3DGame (m_game) - this object handles all the game specific logic and game dynamics.
//     GameRenderer (m_renderer) - This object handles all the graphics rendering for the game.
//

#pragma once

#include "DeviceResources.h"
#include "MoveLookController.h"
#include "GameRenderer.h"
#include "Simple3DGame.h"
#include "GameUIControl.h"


namespace GameControl
{
    private enum class UpdateEngineState
    {
        WaitingForResources,
        ResourcesLoaded,
        WaitingForPress,
        Dynamics,
        TooSmall,
        Suspended,
        Deactivated,
    };

    private enum class PressResultState
    {
        LoadGame,
        PlayLevel,
        ContinueLevel,
    };

    private enum class GameInfoOverlayState
    {
        Loading,
        GameStats,
        GameOverExpired,
        GameOverCompleted,
        LevelStart,
        Pause,
    };
};

class GameMain : public DX::IDeviceNotify
{
public:
    GameMain(const std::shared_ptr<DX::DeviceResources>& deviceResources, Simple3DGameXaml::IGameUIControl^ UIControl);
    ~GameMain();
    void CreateWindowSizeDependentResources();
    void StartRenderLoop();
    void StopRenderLoop();
    void Suspend();
    void Resume();
    Concurrency::critical_section& GetCriticalSection() { return m_criticalSection; }

    // IDeviceNotify
    virtual void OnDeviceLost();
    virtual void OnDeviceRestored();

    void PauseRequested() { if (m_updateState == GameControl::UpdateEngineState::Dynamics) m_pauseRequested = true; };
    void PressComplete()  { if (m_updateState == GameControl::UpdateEngineState::WaitingForPress) m_pressComplete = true; };
    void ResetGame();
    void SetBackground(unsigned int background);
    void CycleBackground();
    void LicenseChanged(
        Windows::ApplicationModel::Store::ListingInformation^ listing,
        Windows::ApplicationModel::Store::LicenseInformation^ license
        );

    void WindowActivationChanged(Windows::UI::Core::CoreWindowActivationState activationState);

private:
    void SetGameInfoOverlay(GameControl::GameInfoOverlayState state);
    void InitializeGameState();
    void UpdateLayoutState();
    void Update();

    void InitializeLicense();
    void InitializeLicenseCore();
    void OnLicenseChanged();

private:
    bool                                                m_pauseRequested;
    bool                                                m_pressComplete;
    bool                                                m_haveFocus;

    std::shared_ptr<DX::DeviceResources>                m_deviceResources;
    Simple3DGameXaml::IGameUIControl^                   m_uiControl;

    MoveLookController^                                 m_controller;
    GameRenderer^                                       m_renderer;
    Simple3DGame^                                       m_game;

    GameControl::UpdateEngineState                      m_updateState;
    GameControl::UpdateEngineState                      m_updateStateNext;
    GameControl::PressResultState                       m_pressResult;
    GameControl::GameInfoOverlayState                   m_gameInfoOverlayState;
    Windows::ApplicationModel::Store::LicenseInformation^ m_licenseInformation;
    Windows::ApplicationModel::Store::ListingInformation^ m_listingInformation;
#ifdef USE_STORE_SIMULATOR
    PersistentState^                                    m_licenseState;
    bool                                                m_isTrial;
#endif
    Concurrency::critical_section                       m_criticalSection;
    Windows::Foundation::IAsyncAction^                  m_renderLoopWorker;
};

