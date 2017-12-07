#include "pch.h"
#include "SeekBarElement.h"
#include "DeviceResources.h"
#include "DirectXHelper.h"
#include "..\360VideoPlaybackMain.h"
#include "..\AppView.h"

using namespace _360VideoPlayback;
using namespace DX;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Globalization::NumberFormatting;
using namespace Windows::Media::Core;
using namespace Windows::Media::Playback;
using namespace Windows::UI::Input::Spatial;

const long long s_hnsPerMillisecond = 10000;
const int64 ticksPerSecond = 10000000LL;
const UINT millisecondsPerSecond = 1000;
const unsigned int END_OF_PLAYBACK_BUFFER_MS = 200;

SeekBarElement::SeekBarElement(float elementWidth, float elementHeight, float leftRightPadding, float topBottomPadding) :
    m_leftRightPadding(leftRightPadding),
    m_topBottomPadding(topBottomPadding),
    m_currentPlayPosition(0),
    m_currentVideoDuration(0),
    m_secondsElapsedSinceTimeUpdate(0.0)
{
    m_isFocusIntersectionCalculated = true;
    m_elementSize = float2(elementWidth, elementHeight);
    m_sliderPosition = 0.0f;
    m_sliderWidth = max((m_elementSize.x - (m_leftRightPadding * 2.0f)), 0.0f);
    m_sliderHeight = max((m_elementSize.y - (m_topBottomPadding * 2.0f)), 0.0f);

    m_sliderFrontSegment = std::make_shared<Line>(m_sliderHeight);
    AddChildElement(m_sliderFrontSegment);

    m_sliderRemainingSegment = std::make_shared<Line>(m_sliderHeight);
    AddChildElement(m_sliderRemainingSegment);

    float seekBarYCoordinate = m_topBottomPadding + (m_sliderHeight * 0.5f);
    m_sliderFrontSegmentStart = D2D1::Point2F(m_leftRightPadding, 0.0f);
    m_sliderFrontSegment->SetStartPoint(m_sliderFrontSegmentStart);
    m_sliderFrontSegment->SetTranslation({ 0.0f, seekBarYCoordinate });

    m_sliderRemainingSegmentEnd = D2D1::Point2F(m_elementSize.x - m_leftRightPadding, 0.0f);
    m_sliderRemainingSegment->SetEndPoint(m_sliderRemainingSegmentEnd);
    m_sliderRemainingSegment->SetTranslation({ 0.0f, seekBarYCoordinate });
    m_textSize = FONT_SIZE * 1.25f;
    m_elapsedTimeText = std::make_shared<Text>(L"", m_textSize, DWRITE_FONT_WEIGHT_NORMAL);
    AddChildElement(m_elapsedTimeText);

    m_remainingTimeText = std::make_shared<Text>(L"", m_textSize, DWRITE_FONT_WEIGHT_NORMAL);
    AddChildElement(m_remainingTimeText);
}

void SeekBarElement::Initialize()
{
    auto d2dcontext = m_deviceResources->GetD2DDeviceContext();
    __super::Initialize();

    DX::ThrowIfFailed(
        d2dcontext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White),
            &m_frontSegmentBrush));

    DX::ThrowIfFailed(
        d2dcontext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Gray),
            &m_endSegmentBrush));

    DX::ThrowIfFailed(
        d2dcontext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White),
            &m_brush));

    m_sliderFrontSegment->SetBrush(m_frontSegmentBrush.Get());
    m_sliderFrontSegment->Initialize();

    m_sliderRemainingSegment->SetBrush(m_endSegmentBrush.Get());
    m_sliderRemainingSegment->Initialize();


    m_elapsedTimeText->SetBrush(m_brush.Get());
    m_elapsedTimeText->Initialize();
    m_remainingTimeText->SetBrush(m_brush.Get());
    m_remainingTimeText->Initialize();
    // Text goes below the seekbar
    float startYCoordinateOfText = m_elementSize.y - m_topBottomPadding + 4.0f;
    m_elapsedTimeText->SetTranslation({ m_leftRightPadding, startYCoordinateOfText });
    m_remainingTimeText->SetTranslation({ (m_elementSize.x - m_leftRightPadding - (FONT_SIZE * 3.0f)), startYCoordinateOfText });

    UpdateLayoutFromCurrentTime();
}

void SeekBarElement::UpdateSliderPosition(float value)
{
    m_sliderPosition = value;
    // Clamp [0,1]
    if (m_sliderPosition < 0.0f)
    {
        m_sliderPosition = 0.0f;
    }
    else if (m_sliderPosition > 1.0f)
    {
        m_sliderPosition = 1.0f;
    }

    float sliderUIPosition = m_sliderPosition > 0.0f ? (m_sliderWidth * m_sliderPosition) + m_leftRightPadding : m_leftRightPadding;
    m_sliderFrontSegmentEnd = D2D1::Point2F(sliderUIPosition, 0.0f);
    m_sliderFrontSegment->SetEndPoint(m_sliderFrontSegmentEnd);
    m_sliderRemainingSegmentStart = D2D1::Point2F(sliderUIPosition, 0.0f);
    m_sliderRemainingSegment->SetStartPoint(m_sliderRemainingSegmentStart);
}

void SeekBarElement::Update(const DX::StepTimer& timer, SpatialPointerPose^ cameraPose)
{
    __super::Update(timer, cameraPose);
    m_secondsElapsedSinceTimeUpdate += timer.GetElapsedSeconds();

    // Update the current seek bar time no more than once every second.
    if (m_secondsElapsedSinceTimeUpdate > 1.0)
    {
        UpdateLayoutFromCurrentTime();
        m_secondsElapsedSinceTimeUpdate = 0.0;
    }
}

void SeekBarElement::UpdateLayoutFromCurrentTime()
{
    m_currentVideoDuration = AppView::GetMediaPlayer()->PlaybackSession->NaturalDuration.Duration / s_hnsPerMillisecond;
    if (m_currentVideoDuration != 0)
    {
        m_currentPlayPosition = AppView::GetMediaPlayer()->PlaybackSession->Position.Duration / s_hnsPerMillisecond;
        if (m_currentPlayPosition > m_currentVideoDuration)
        {
            m_currentVideoDuration = m_currentPlayPosition;
        }
    }
    // Update current and remaining time indicators
    m_elapsedTimeString = FormatTimeString(m_currentPlayPosition)->Data();
    m_remainingTimeString = FormatTimeString(m_currentVideoDuration - m_currentPlayPosition)->Data();
    m_elapsedTimeText->SetText(m_elapsedTimeString);
    m_remainingTimeText->SetText(m_remainingTimeString);

    float currentTimeSliderPosition = static_cast<float>(m_currentVideoDuration) > 0.0f ? static_cast<float>(m_currentPlayPosition) / static_cast<float>(m_currentVideoDuration) : 0.0f;
    UpdateSliderPosition(currentTimeSliderPosition);
}

void SeekBarElement::PerformAction()
{
    // Update the Slider's position
    float xIntersectionPoint = m_elementCoordIntersectionPoint.x;
    // Clamp before and after the slider
    // Otherwise compute slider based on relative position
    if (xIntersectionPoint < m_leftRightPadding)
    {
        UpdateSliderPosition(0.0f);
    }
    else if (xIntersectionPoint > (m_leftRightPadding + m_sliderWidth))
    {
        UpdateSliderPosition(1.0f);
    }
    else
    {
        UpdateSliderPosition((xIntersectionPoint - m_leftRightPadding) / m_sliderWidth);
    }

    unsigned int seekTime = static_cast<unsigned int>(m_currentVideoDuration * m_sliderPosition);

    if (seekTime >= (static_cast<unsigned int>(AppView::GetMediaPlayer()->PlaybackSession->NaturalDuration.Duration / s_hnsPerMillisecond)) - END_OF_PLAYBACK_BUFFER_MS)
    {
        seekTime = (static_cast<unsigned int>(AppView::GetMediaPlayer()->PlaybackSession->NaturalDuration.Duration / s_hnsPerMillisecond)) - END_OF_PLAYBACK_BUFFER_MS;
    }
    auto seekTimeSpan = TimeSpan();
    seekTimeSpan.Duration = static_cast<int64>(seekTime) * s_hnsPerMillisecond;
    AppView::GetMediaPlayer()->PlaybackSession->Position = seekTimeSpan;
}

String^ SeekBarElement::FormatTimeString(int64 intervalValue)
{
    String^ formattedTimeSpan;
    std::chrono::milliseconds duration{ intervalValue };
    auto seconds = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
    auto minutes = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::minutes>(duration).count());
    seconds -= minutes * 60;
    auto hours = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::hours>(duration).count());
    minutes -= hours * 60;

    DecimalFormatter^ twoDigitNumberFormatter = ref new DecimalFormatter(
        Windows::System::UserProfile::GlobalizationPreferences::Languages,
        Windows::System::UserProfile::GlobalizationPreferences::HomeGeographicRegion);
    twoDigitNumberFormatter->FractionDigits = 0;
    twoDigitNumberFormatter->IsGrouped = false;
    twoDigitNumberFormatter->IntegerDigits = 2;

    if (hours > 0)
    {
        // Display Hours naturally, but display minutes and seconds using 2 digits
        formattedTimeSpan = ref new String(
            (twoDigitNumberFormatter->FormatUInt(hours) + L":" +
                twoDigitNumberFormatter->FormatUInt(minutes) + L":" +
                twoDigitNumberFormatter->FormatUInt(seconds))->Data());
    }
    else
    {
        // If the hour count is 0, then only display minutes and seconds
        formattedTimeSpan = ref new String(
            (twoDigitNumberFormatter->FormatUInt(minutes) + L":" +
                twoDigitNumberFormatter->FormatUInt(seconds))->Data());
    }
    return formattedTimeSpan;
}
