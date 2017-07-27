#pragma once
#include "BaseElement.h"

namespace DX
{
    enum class ButtonShape
    {
        Rectangle,
        Circle,
    };

    enum class ButtonState
    {
        None,
        Pressing,
        Pressed
    };

    class Button : public FocusableElement
    {
    public:
        Button(
            UINT initialSize = BUTTON_SIZE,
            ButtonShape shape = ButtonShape::Rectangle);

        virtual void ArrangeElements() override;
        virtual void Initialize() override;
        virtual void Update(const DX::StepTimer& timer, Windows::UI::Input::Spatial::SpatialPointerPose^ cameraPose) override;
        virtual void PerformAction() override;
        virtual void FocusUpdate() override;
    protected:
        bool m_forceIconCentered;
        Windows::Foundation::Numerics::float4x4 m_parentControlTransform;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;
        ButtonState m_buttonState = ButtonState::None;
        float m_pressingTimeOut;
        const ButtonShape m_shape = ButtonShape::Rectangle;
        const float m_maxSecondsInPressingState = 0.25f;
    private:
        std::shared_ptr<DX::Rectangle> m_RectFocusOutline;
        std::shared_ptr<DX::FilledRectangle> m_RectPressedBackgroundFill;
        std::shared_ptr<DX::Ellipse> m_ElliFocusOutline;
        std::shared_ptr<DX::FilledEllipse> m_ElliPressedBackgroundFill;
    };

    class PlayPauseButton : public Button
    {
    public:
        PlayPauseButton(UINT buttonSize, UINT iconSize);
        virtual void Initialize() override;
        virtual void Update(const DX::StepTimer& timer, Windows::UI::Input::Spatial::SpatialPointerPose^ cameraPose) override;
        virtual void PerformAction() override;

    private:
        std::shared_ptr<Text> m_playText;
        std::shared_ptr<Text> m_pauseText;
    };

    class ExitButton : public Button
    {
    public:
        ExitButton();
        virtual void Initialize() override;
        virtual void PerformAction() override;

    private:
        std::shared_ptr<Text> m_exitText;
    };
}
