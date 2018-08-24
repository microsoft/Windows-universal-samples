#pragma once
#include "BaseElement.h"

namespace DX
{
    class SeekBarElement : public FocusableElement
    {
    public:
        SeekBarElement(
            float elementWidth,
            float elementHeight = FONT_SIZE * 5.0f,
            float leftRightPadding = FONT_SIZE * 2.0f,
            float topBottomPadding = FONT_SIZE * 2.0f);

        virtual void Initialize() override;
        virtual void Update(const DX::StepTimer& timer, Windows::UI::Input::Spatial::SpatialPointerPose^ cameraPose) override;

        virtual void PerformAction() override;


    private:
        void UpdateLayoutFromCurrentTime();
        void UpdateSliderPosition(float position);
        Platform::String^ FormatTimeString(int64 intervalValue);

        float m_sliderPosition;
        float m_sliderWidth;
        float m_sliderHeight;
        float m_leftRightPadding;
        float m_topBottomPadding;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_frontSegmentBrush;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_endSegmentBrush;
        D2D1_POINT_2F m_sliderFrontSegmentStart;
        D2D1_POINT_2F m_sliderFrontSegmentEnd;
        D2D1_POINT_2F m_sliderRemainingSegmentStart;
        D2D1_POINT_2F m_sliderRemainingSegmentEnd;
        std::shared_ptr<Line> m_sliderFrontSegment;
        std::shared_ptr<Line> m_sliderRemainingSegment;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;
        int64 m_currentVideoDuration;
        int64 m_currentPlayPosition;
        std::wstring m_elapsedTimeString;
        std::wstring m_remainingTimeString;
        std::shared_ptr<Text> m_elapsedTimeText;
        std::shared_ptr<Text> m_remainingTimeText;
        float m_textSize;
        double m_secondsElapsedSinceTimeUpdate;

    };

}