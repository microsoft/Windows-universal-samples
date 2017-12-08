#pragma once

//
// Comment out this preprocessor definition to disable all of the
// sample content.
//
// To remove the content after disabling it:
//     * Remove the unused code from your app's Main class.
//     * Delete the Content folder provided with this template.
//
#define DRAW_SAMPLE_CONTENT

#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"

#ifdef DRAW_SAMPLE_CONTENT
#include "Content\VideoRenderer.h"
#include "Content\SpatialInputHandler.h"
#include "Content\MediaTransportControls.h"
#endif

// Updates, renders, and presents holographic content using Direct3D.
namespace _360VideoPlayback
{
    class _360VideoPlaybackMain : public DX::IDeviceNotify
    {
    public:
        _360VideoPlaybackMain();
        ~_360VideoPlaybackMain();

        // Sets the holographic space. This is our closest analogue to setting a new window
        // for the app.
        void SetHolographicSpace(Windows::Graphics::Holographic::HolographicSpace^ holographicSpace);

        // Starts the holographic frame and updates the content.
        Windows::Graphics::Holographic::HolographicFrame^ Update();

        // Renders holograms, including world-locked content.
        bool Render(Windows::Graphics::Holographic::HolographicFrame^ holographicFrame);

        // Handle saving and loading of app state owned by AppMain.
        void SaveAppState();
        void LoadAppState();

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();
        void CreateVideoShaders();
        // Clears event registration state. Used when changing to a new HolographicSpace
        // and when tearing down AppMain.
        void UnregisterHolographicEventHandlers();
        static std::shared_ptr<DX::DeviceResources> GetDeviceResources();

    private:
        // Asynchronously creates resources for new holographic cameras.
        void OnCameraAdded(
            Windows::Graphics::Holographic::HolographicSpace^ sender,
            Windows::Graphics::Holographic::HolographicSpaceCameraAddedEventArgs^ args);

        // Synchronously releases resources for holographic cameras that are no longer
        // attached to the system.
        void OnCameraRemoved(
            Windows::Graphics::Holographic::HolographicSpace^ sender,
            Windows::Graphics::Holographic::HolographicSpaceCameraRemovedEventArgs^ args);

        // Used to notify the app when the positional tracking state changes.
        void OnLocatabilityChanged(
            Windows::Perception::Spatial::SpatialLocator^ sender,
            Platform::Object^ args);

#ifdef DRAW_SAMPLE_CONTENT
        // Renders a colorful holographic cube that's 20 centimeters wide. This sample content
        // is used to demonstrate world-locked rendering.
        std::unique_ptr<VideoRenderer> m_videoRenderer;
        std::unique_ptr<MediaTransportControls> m_mediaTransportControls;

        // Listens for the Pressed spatial input event.
        std::shared_ptr<SpatialInputHandler> m_spatialInputHandler;
#endif

        // static pointer to device resources.
        static std::shared_ptr<DX::DeviceResources> m_deviceResources;

        // Render loop timer.
        DX::StepTimer m_timer;

        // Represents the holographic space around the user.
        Windows::Graphics::Holographic::HolographicSpace^ m_holographicSpace;

        // SpatialLocator that is attached to the primary camera.
        Windows::Perception::Spatial::SpatialLocator^ m_locator;

        // A reference frame attached to the holographic camera.
        Windows::Perception::Spatial::SpatialLocatorAttachedFrameOfReference^ m_referenceFrame;

        // Event registration tokens.
        Windows::Foundation::EventRegistrationToken m_cameraAddedToken;
        Windows::Foundation::EventRegistrationToken m_cameraRemovedToken;
        Windows::Foundation::EventRegistrationToken m_locatabilityChangedToken;
        bool m_isGamepadPressed = false;
        bool m_isMouseLeftKeyPressed = false;
    };
}
