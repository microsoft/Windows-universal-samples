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

// GameInfoOverlay:
// This class maintains a D2D1 Bitmap surface to provide information to the player
// on the state of the game when the game is not in active play.
// This class has several different status modes:
//     GameLoading - this displays a title and a series of dots to show progress.
//     GameStats - this displays a title and the Max level achieved, the hit and shot counts.
//     GameOver - this displays a title and the game status.
//     Pause - this displays only a title - 'Paused'.
// In addition to the status modes, there is a region at the bottom that displays what the
// player is expected to do next. This can be one of four things:
//     None - the action is blank. This is usually set when the game does not currently have focus.
//     TapToContinue - the game is waiting for the player to provide input that they are ready to
//         proceed.
//     PleaseWait - the game is actively doing some background processing (like loading a level).
//     PlayAgain - the game has completed and is waiting for the player to indicate they are ready
//         to play another round of the game.

#include "GameUIControl.h"

class GameInfoOverlay : public Simple3DGameDX::IGameUIControl
{
public:
    GameInfoOverlay(std::shared_ptr<DX::DeviceResources> const& deviceResources);

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources(D2D_SIZE_F overlaySize);
    void ReleaseDeviceDependentResources();

    bool Visible() { return m_visible; };
    ID2D1Bitmap1* Bitmap();

public:
    // IGameUIControl interface
    virtual void SetGameLoading(uint32_t dots);
    virtual void SetGameStats(int maxLevel, int hitCount, int shotCount);
    virtual void SetGameOver(bool win, int maxLevel, int hitCount, int shotCount, int highScore);
    virtual void SetLevelStart(int level, std::wstring const& objective, float timeLimit, float bonusTime);
    virtual void SetPause(int level, int hitCount, int shotCount, float timeRemaining);
    virtual void SetAction(Simple3DGameDX::GameInfoOverlayCommand action);
    virtual void HideGameInfoOverlay() { m_visible = false; };
    virtual void ShowGameInfoOverlay() { m_visible = true; };
    virtual void ShowTooSmall();
    virtual void HideTooSmall();

private:
    // Cached pointer to device resources.
    std::shared_ptr<DX::DeviceResources>    m_deviceResources;

    bool                                    m_visible{ true };
    bool                                    m_tooSmallActive{ false };

    D2D1_RECT_F                             m_titleRectangle;
    D2D1_RECT_F                             m_bodyRectangle;
    D2D1_RECT_F                             m_actionRectangle;

    winrt::com_ptr<ID2D1Bitmap1>            m_levelBitmap;
    winrt::com_ptr<ID2D1Bitmap1>            m_tooSmallBitmap;
    winrt::com_ptr<IDWriteTextFormat>       m_textFormatTitle;
    winrt::com_ptr<IDWriteTextFormat>       m_textFormatBody;
    winrt::com_ptr<ID2D1SolidColorBrush>    m_textBrush;
    winrt::com_ptr<ID2D1SolidColorBrush>    m_backgroundBrush;
    winrt::com_ptr<ID2D1SolidColorBrush>    m_actionBrush;
    std::wstring                            m_titleString;
    std::wstring                            m_bodyString;
    std::wstring                            m_actionString;
};
