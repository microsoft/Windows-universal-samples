#pragma once

namespace HolographicMRCSample
{
    // Sample gesture handler.
    // Hooks up events to recognize a tap gesture, and keeps track of input using a boolean value.
    class SpatialInputHandler
    {
    public:
        SpatialInputHandler();
        ~SpatialInputHandler();

        Windows::UI::Input::Spatial::SpatialInteractionSourceState^ CheckForInput();

    private:
        // Interaction event handler.
        void OnSourcePressed(
            Windows::UI::Input::Spatial::SpatialInteractionManager^ sender,
            Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs^ args);

        // API objects used to process gesture input, and generate gesture events.
        Windows::UI::Input::Spatial::SpatialInteractionManager^     m_interactionManager;

        // Event registration token.
        Windows::Foundation::EventRegistrationToken                 m_sourcePressedEventToken;

        // Used to indicate that a Pressed input event was received this frame.
        Windows::UI::Input::Spatial::SpatialInteractionSourceState^ m_sourceState = nullptr;
    };
}
