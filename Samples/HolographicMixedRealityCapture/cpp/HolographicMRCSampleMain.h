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
#include "Content\SpinningCubeRenderer.h"
#include "Content\Panel.h"
#include "Content\Button.h"
#include "Content\Cursor.h"
#include "Content\SpatialInputHandler.h"

#include "MediaCaptureManager.h"
#endif

// Updates, renders, and presents holographic content using Direct3D.
namespace HolographicMRCSample
{
    class HolographicMRCSampleMain : public DX::IDeviceNotify
    {
    public:
        HolographicMRCSampleMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~HolographicMRCSampleMain();

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

        // Clears event registration state. Used when changing to a new HolographicSpace
        // and when tearing down AppMain.
        void UnregisterHolographicEventHandlers();

        void OnButtonInitTapped();
        void OnButtonVideoTapped();
        void OnButtonPhotoTapped();
        void OnButtonHologramTapped();
        void OnButtonSysAudioTapped();

        void DisableAllButtons();

#ifdef DRAW_SAMPLE_CONTENT
        // Renders a colorful holographic cube that's 20 centimeters wide. This sample content
        // is used to demonstrate world-locked rendering.
        std::unique_ptr<SpinningCubeRenderer>                           m_cube1;
        std::unique_ptr<SpinningCubeRenderer>                           m_cube2;
        std::unique_ptr<SpinningCubeRenderer>                           m_cube3;
        std::unique_ptr<SpinningCubeRenderer>                           m_cube4;

        std::unique_ptr<Panel>                                          m_mainPanel;
        std::shared_ptr<Button>                                         m_lockButton;
        std::shared_ptr<Button>                                         m_initButton;
        std::shared_ptr<Button>                                         m_photoButton;
        std::shared_ptr<Button>                                         m_videoButton;
        std::shared_ptr<Button>                                         m_hologramButton;
        std::shared_ptr<Button>                                         m_sysAudioButton;

        std::unique_ptr<Cursor>                                         m_cursor;

        // Listens for the Pressed spatial input event.
        std::shared_ptr<SpatialInputHandler>                            m_spatialInputHandler;
#endif

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>                            m_deviceResources;

        // Render loop timer.
        DX::StepTimer                                                   m_timer;

        // Represents the holographic space around the user.
        Windows::Graphics::Holographic::HolographicSpace^               m_holographicSpace;

        // SpatialLocator that is attached to the primary camera.
        Windows::Perception::Spatial::SpatialLocator^                   m_locator;

        // A reference frame attached to the holographic camera.
        Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ m_referenceFrame;

        // Event registration tokens.
        Windows::Foundation::EventRegistrationToken                     m_cameraAddedToken;
        Windows::Foundation::EventRegistrationToken                     m_cameraRemovedToken;
        Windows::Foundation::EventRegistrationToken                     m_locatabilityChangedToken;

        // Mixed Reality Capture
        std::unique_ptr<MediaCaptureManager>                            m_mediaCapture;
        bool                                                            m_recording;
    };
}
