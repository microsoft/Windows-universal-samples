#pragma once
#include "SourceHandler.h"

namespace HolographicPaint
{
    // Implements handling of interactions for Controllers
    class SpatialControllerHandler : public SourceHandler
    {
    public:
        SpatialControllerHandler(Windows::UI::Input::Spatial::SpatialInteractionSource^ source, ISourceCommandHandler* eventsHandler, std::shared_ptr<Whiteboard>& target);
        ~SpatialControllerHandler();

        virtual void OnInteractionDetected(Windows::UI::Input::Spatial::SpatialInteraction^ interaction, Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem) override;
        virtual void OnSourceUpdated(Windows::UI::Input::Spatial::SpatialInteractionSourceState^ state, Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem) override;
        virtual void OnSourcePressed(Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs^ args) override;

        virtual Windows::Foundation::Numerics::float3 GetToolScale() const override;

        void TryGetContinousBuzz(Windows::Devices::Haptics::SimpleHapticsController^ controller);

    protected:
        bool m_insideTargetBoard = false;
        bool m_strokeInsideTargetBoard = false;
        bool m_waitForRelease = false;

        std::shared_ptr<IDPadHelper> m_spDPadHelper;
        Windows::Devices::Haptics::SimpleHapticsController^ m_hapticsController;
        Windows::Devices::Haptics::SimpleHapticsControllerFeedback^ m_hapticsBuzzFeedback;

        Windows::UI::Input::Spatial::SpatialGestureRecognizer^ m_gestureRecognizer;
        Windows::Foundation::EventRegistrationToken m_tappedToken;

        void OnTapped(Windows::UI::Input::Spatial::SpatialGestureRecognizer ^sender, Windows::UI::Input::Spatial::SpatialTappedEventArgs ^args);
    };

}