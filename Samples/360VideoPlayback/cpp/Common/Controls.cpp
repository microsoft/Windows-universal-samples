#include "pch.h"
#include "Controls.h"
#include "DeviceResources.h"
#include "DirectXHelper.h"
#include "..\AppView.h"
#include "..\VideoGallery.xaml.h"

using namespace _360VideoPlayback;
using namespace D2D1;
using namespace DirectX;
using namespace DX;
using namespace Microsoft::WRL;
using namespace std;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Media::Core;
using namespace Windows::Media::Playback;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input::Spatial;
using namespace Windows::UI::ViewManagement;

const ColorF focusOutlineColor(0xFFFFFF);
const ColorF pressedBackgroundFillColor(0x00AFFF);
const ColorF enabledForegroundColor(0xFFFFFF);
const float c_maxSecondsInPerformPressedFeedbackState = 0.25f;
UINT DEFAULT_SIZE = 18;
ElementMargin TEXT_MARGIN{ 15.0f, 12.0f, 15.0f, 12.0f };

Button::Button(UINT initialSize, ButtonShape shape) :
    FocusableElement(),
    m_pressingTimeOut(0.0f),
    m_shape(shape)
{
    m_elementSize = float2(static_cast<float>(initialSize));

    // Add the button pressed UI element first so it appears under the focus rect.
    if (m_shape == ButtonShape::Rectangle)
    {
        m_RectPressedBackgroundFill = make_shared<FilledRectangle>(m_elementSize.x, m_elementSize.y, pressedBackgroundFillColor);
        m_RectFocusOutline = make_shared<Rectangle>(m_elementSize.x, m_elementSize.y, 1.5f, focusOutlineColor);
        m_RectPressedBackgroundFill->SetIgnoreLayout(false);
        m_RectFocusOutline->SetIgnoreLayout(false);
        m_RectPressedBackgroundFill->SetIsVisible(false);
        m_RectFocusOutline->SetIsVisible(false);
        this->AddChildElement(m_RectPressedBackgroundFill);
        this->AddChildElement(m_RectFocusOutline);

    }
    else if (m_shape == ButtonShape::Circle)
    {
        m_ElliPressedBackgroundFill = make_shared<FilledEllipse>(m_elementSize.x * 0.5f, m_elementSize.y * 0.5f, pressedBackgroundFillColor);
        m_ElliFocusOutline = make_shared<Ellipse>(m_elementSize.x * 0.5f, m_elementSize.y* 0.5f, 5.0f, focusOutlineColor);
        m_ElliPressedBackgroundFill->SetIgnoreLayout(false);
        m_ElliFocusOutline->SetIgnoreLayout(false);
        m_ElliPressedBackgroundFill->SetIsVisible(false);
        m_ElliFocusOutline->SetIsVisible(false);
        this->AddChildElement(m_ElliPressedBackgroundFill);
        this->AddChildElement(m_ElliFocusOutline);
    }
}

void Button::ArrangeElements()
{
    __super::ArrangeElements();

    if (m_shape == ButtonShape::Rectangle)
    {
        // Update the focus rect to be the same size as the button.
        FLOAT selectionBorderStrokeThickness = m_RectFocusOutline->GetStrokeThickness();
        m_RectFocusOutline->SetRect({
    selectionBorderStrokeThickness,
    selectionBorderStrokeThickness,
    m_elementSize.x - selectionBorderStrokeThickness,
    m_elementSize.y - selectionBorderStrokeThickness });

        m_RectPressedBackgroundFill->SetRect({ 0.0f, 0.0f, m_elementSize.x, m_elementSize.y });
    }
    else if (m_shape == ButtonShape::Circle)
    {
        m_ElliFocusOutline->SetRadius(m_elementSize.x * 0.5f, m_elementSize.y * 0.5f);
        m_ElliPressedBackgroundFill->SetRadius(m_elementSize.x * 0.5f, m_elementSize.y * 0.5f);
    }
}

void Button::Initialize()
{
    auto d2dcontext = m_deviceResources->GetD2DDeviceContext();

    __super::Initialize();

    // Recreate the brush for the glyphs, font, etc. This is the final brush we want to hold on to in the button
    DX::ThrowIfFailed(
        d2dcontext->CreateSolidColorBrush(
            enabledForegroundColor,
            &m_brush));
}

void Button::Update(const DX::StepTimer& timer, SpatialPointerPose^ cameraPose)
{
    __super::Update(timer, cameraPose);
    if (m_buttonState == ButtonState::Pressing)
    {
        if (m_pressingTimeOut > m_maxSecondsInPressingState)
        {
            m_buttonState = ButtonState::Pressed;
            if (m_shape == ButtonShape::Rectangle)
            {
                m_RectPressedBackgroundFill->SetIsVisible(false);
            }
            else
            {
                m_ElliPressedBackgroundFill->SetIsVisible(false);
            }
            Button::PerformAction();
            m_pressingTimeOut = 0.0f;
        }
        else
        {
            m_pressingTimeOut += static_cast<float>(timer.GetElapsedSeconds());
            if (m_shape == ButtonShape::Rectangle)
            {
                if (!m_RectPressedBackgroundFill->GetIsVisible())
                {
                    m_RectPressedBackgroundFill->SetIsVisible(true);
                }
            }
            else
            {
                if (!m_ElliPressedBackgroundFill->GetIsVisible())
                {
                    m_ElliPressedBackgroundFill->SetIsVisible(true);
                }
            }
        }
    }
}

void Button::FocusUpdate()
{
    bool update = false;
    if (this->IsFocused())
    {
        update = true;
    }
    (m_shape == ButtonShape::Rectangle) ? m_RectFocusOutline->SetIsVisible(update) : m_ElliFocusOutline->SetIsVisible(update);
}

void Button::PerformAction()
{
    // If the user invokes a button we need to trigger the invoked state
    // and give visual feedback to the user that the button has been pressed.
    if (m_buttonState == ButtonState::None)
    {
        m_buttonState = ButtonState::Pressing;
        m_pressingTimeOut = 0.0f;
    }
    else if (m_buttonState == ButtonState::Pressed)
    {
        __super::PerformAction();
        m_buttonState = ButtonState::None;
    }
}

PlayPauseButton::PlayPauseButton(UINT buttonSize, UINT iconSize) :
    Button(iconSize, ButtonShape::Circle)
{
    this->SetElementSize({ static_cast<float>(buttonSize), static_cast<float>(buttonSize) });
    m_playText = std::make_shared<Text>(L"Play", FONT_SIZE * 4, DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL);
    m_playText->SetElementMargin(TEXT_MARGIN);
    this->AddChildElement(m_playText);

    m_pauseText = std::make_shared<Text>(L"Pause", FONT_SIZE * 4, DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL);
    m_pauseText->SetElementMargin(TEXT_MARGIN);
    this->AddChildElement(m_pauseText);

    this->ArrangeElements();
}

void PlayPauseButton::Initialize()
{
    __super::Initialize();
    m_playText->SetBrush(m_brush.Get());
    m_playText->Initialize();
    m_pauseText->SetBrush(m_brush.Get());
    m_pauseText->Initialize();
}

void PlayPauseButton::Update(const StepTimer& timer, SpatialPointerPose^ cameraPose)
{
    __super::Update(timer, cameraPose);

    // Update the playback state.
    bool isPlaying = (AppView::GetMediaPlayer()->PlaybackSession->PlaybackState == MediaPlaybackState::Playing);

    // When the video is playing, you show the Pause Text since hitting the button in this state will pause the video
    if (isPlaying)
    {
        m_playText->SetIsVisible(false);
        m_pauseText->SetIsVisible(true);
    }
    // When the video is Paused, you show the P:ay Text since hitting the button in this state will play the video
    else
    {
        m_playText->SetIsVisible(true);
        m_pauseText->SetIsVisible(false);
    }
}

void PlayPauseButton::PerformAction()
{
    __super::PerformAction();
    if (AppView::GetMediaPlayer()->PlaybackSession->PlaybackState != MediaPlaybackState::None)
    {
        if (AppView::GetMediaPlayer()->PlaybackSession->PlaybackState == MediaPlaybackState::Playing)
        {
            AppView::GetMediaPlayer()->Pause();
        }
        else
        {
            AppView::GetMediaPlayer()->Play();
        }
    }
}

ExitButton::ExitButton() :
    Button(DEFAULT_SIZE)
{
    m_exitText = std::make_shared<Text>(L"Exit Playback", FONT_SIZE * 2, DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL);
    m_exitText->SetElementMargin(TEXT_MARGIN);
    this->AddChildElement(m_exitText);

    this->SetLayoutHorizontal(true);
}

void ExitButton::Initialize()
{
    __super::Initialize();

    m_exitText->SetBrush(m_brush.Get());

    m_exitText->Initialize();
}

void ExitButton::PerformAction()
{
    __super::PerformAction();
    ApplicationViewSwitcher::SwitchAsync(VideoGallery::GetMainViewId(), ApplicationView::GetForCurrentView()->Id, ApplicationViewSwitchingOptions::ConsolidateViews);
}