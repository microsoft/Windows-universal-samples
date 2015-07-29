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

// The IGameUIControl interface provides an abstraction to the GameMain class
// for controlling UI elements on the screen in response to game state.  It helps to
// provide separation between the game control and how the UI is displayed to the user.
//    SetGameLoading - is called to have the UI indicate the game is currently loading
//        resources for the game or a particular level.  GameMain will call this
//        method repeatedly to provide update progress.  'dots' is the number of
//        dots to display to indicate progress.
//    SetGameStats - is called to have the UI indicate the status of the last completed game.
//    SetGameOver - is called to have the UI indicate this iteration of the game as finished.
//    SetLevelStart - is called to have the UI indicate the level is ready to begin.  This includes
//        providing the objectives for the level and other current data.
//    SetPause - is called to have the UI indicate the game is currently in the Paused state.
//    ShowTooSmall - this method indicates the UI should display to the user that the current
//        display configuration is too small for the game to be played.
//    HideTooSmall - this method inticates the UI should stop displaying to the user that the
//        current display configuration is too small for game play.
//    SetAction - is called to have the UI indicate the next action the user should take.
//    ShowGameInfoOverlay - is called to have the UI display to the user the current state
//        as defined by the other interface calls above.
//    HideGameInfoOverlay - is called to have the UI remove the display of the current state
//        when the game transitions into active game play.

namespace Simple3DGameDX
{
    public enum class GameInfoOverlayCommand
    {
        None,
        TapToContinue,
        PleaseWait,
        PlayAgain,
    };

    public interface class IGameUIControl
    {
        void SetGameLoading(uint32 dots);
        void SetGameStats(int maxLevel, int hitCount, int shotCount);
        void SetGameOver(bool win, int maxLevel, int hitCount, int shotCount, int highScore);
        void SetLevelStart(int level, Platform::String^ objective, float timeLimit, float bonusTime);
        void SetPause(int level, int hitCount, int shotCount, float timeRemaining);
        void ShowTooSmall();
        void HideTooSmall();
        void SetAction(GameInfoOverlayCommand action);
        void HideGameInfoOverlay();
        void ShowGameInfoOverlay();
    };
};
