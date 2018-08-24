#pragma once

namespace HolographicPaint
{

    enum class DPadAction { None, Left, Right, Top, Bottom };

    // This class and the class derived from it allows the application to receive the same commands
    // for controllers with a Touchpad or a Thumbstick.
    class IDPadHelper
    {
    public:

        virtual DPadAction Update(Windows::UI::Input::Spatial::SpatialInteractionControllerProperties^ controllerState)
        {
            return DPadAction::None;
        }

        virtual DPadAction SourcePressed(Windows::UI::Input::Spatial::SpatialInteractionPressKind pressKind, Windows::UI::Input::Spatial::SpatialInteractionControllerProperties^ controllerState)
        {
            return DPadAction::None;
        }
    };

    class ThumbstickDPadHelper : public IDPadHelper
    {
    public:

        ThumbstickDPadHelper(double threshold = .5);

        virtual DPadAction Update(Windows::UI::Input::Spatial::SpatialInteractionControllerProperties^ controllerState) override;

    private:
        bool m_waitForCenter;
        double m_threshold;
    };

    class TouchpadDPadHelper : public IDPadHelper
    {
    public:

        TouchpadDPadHelper(double threshold = .3);

        virtual DPadAction SourcePressed(
            Windows::UI::Input::Spatial::SpatialInteractionPressKind pressKind,
            Windows::UI::Input::Spatial::SpatialInteractionControllerProperties^ controllerState) override;

    private:
        double m_threshold;
    };
}