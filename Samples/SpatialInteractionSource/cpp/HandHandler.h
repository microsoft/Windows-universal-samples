#pragma once
#include "SourceHandler.h"

namespace HolographicPaint
{

    // Implements handling of interactions for hands
    class HandHandler : public SourceHandler
    {

    public:
        HandHandler(Windows::UI::Input::Spatial::SpatialInteractionSource^ source, ISourceCommandHandler* eventsHandler, std::shared_ptr<Whiteboard>& target);
        ~HandHandler();

        virtual void OnSourceUpdated(Windows::UI::Input::Spatial::SpatialInteractionSourceState^ state, Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem);
        virtual void OnInteractionDetected(Windows::UI::Input::Spatial::SpatialInteraction^ interaction, Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem) override;

    private:
        // For Hands, we map commands based on Gestures detection
        Windows::UI::Input::Spatial::SpatialGestureRecognizer^ m_gestureRecognizer;
        Windows::Perception::Spatial::SpatialCoordinateSystem^ m_coordinateSystem;
        Windows::Foundation::Numerics::float3 m_initialPosition;

        Windows::Foundation::EventRegistrationToken m_tappedToken;
        Windows::Foundation::EventRegistrationToken m_navigationStartedToken;
        Windows::Foundation::EventRegistrationToken m_navigationCompletedToken;
        Windows::Foundation::EventRegistrationToken m_manipulationStartedToken;
        Windows::Foundation::EventRegistrationToken m_manipulationCompletedToken;
        Windows::Foundation::EventRegistrationToken m_manipulationCanceledToken;
        Windows::Foundation::EventRegistrationToken m_manipulationUpdatedToken;

        void OnTapped(Windows::UI::Input::Spatial::SpatialGestureRecognizer ^sender, Windows::UI::Input::Spatial::SpatialTappedEventArgs ^args);
        void OnNavigationStarted(Windows::UI::Input::Spatial::SpatialGestureRecognizer ^sender, Windows::UI::Input::Spatial::SpatialNavigationStartedEventArgs ^args);
        void OnNavigationCompleted(Windows::UI::Input::Spatial::SpatialGestureRecognizer ^sender, Windows::UI::Input::Spatial::SpatialNavigationCompletedEventArgs ^args);
        void OnManipulationStarted(Windows::UI::Input::Spatial::SpatialGestureRecognizer ^sender, Windows::UI::Input::Spatial::SpatialManipulationStartedEventArgs ^args);
        void OnManipulationCanceled(Windows::UI::Input::Spatial::SpatialGestureRecognizer ^sender, Windows::UI::Input::Spatial::SpatialManipulationCanceledEventArgs ^args);
        void OnManipulationCompleted(Windows::UI::Input::Spatial::SpatialGestureRecognizer ^sender, Windows::UI::Input::Spatial::SpatialManipulationCompletedEventArgs ^args);
        void OnManipulationUpdated(Windows::UI::Input::Spatial::SpatialGestureRecognizer ^sender, Windows::UI::Input::Spatial::SpatialManipulationUpdatedEventArgs ^args);

        void HitTestTargetWithGaze(Windows::UI::Input::Spatial::SpatialPointerPose^ pointerPose);
    };

}
