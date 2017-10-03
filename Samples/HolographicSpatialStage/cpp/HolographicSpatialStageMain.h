#pragma once

#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"

#include "Content\SceneController.h"
#include "SpatialStageManager.h"

// Updates, renders, and presents holographic content using Direct3D.
namespace HolographicSpatialStage
{
    class HolographicSpatialStageMain : public DX::IDeviceNotify
    {
    public:
        HolographicSpatialStageMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~HolographicSpatialStageMain();

        // Sets the holographic space. This is our closest analogue to setting a new window
        // for the app.
        void SetHolographicSpace(Windows::Graphics::Holographic::HolographicSpace^ holographicSpace);

        // Starts the holographic frame and updates the content.
        Windows::Graphics::Holographic::HolographicFrame^ Update();

        // Renders holograms, including world-locked content.
        bool Render(Windows::Graphics::Holographic::HolographicFrame^ holographicFrame);

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

        std::shared_ptr<SceneController>                    m_sceneController;

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>                m_deviceResources;

        std::shared_ptr<SpatialStageManager>                m_spatialStageManager;

        // Render loop timer.
        DX::StepTimer                                       m_timer;

        // Represents the holographic space around the user.
        Windows::Graphics::Holographic::HolographicSpace^   m_holographicSpace;

        // Event registration tokens.
        Windows::Foundation::EventRegistrationToken         m_cameraAddedToken;
        Windows::Foundation::EventRegistrationToken         m_cameraRemovedToken;
        Windows::Foundation::EventRegistrationToken         m_locatabilityChangedToken;
        Windows::Foundation::EventRegistrationToken         m_gamepadAddedEventToken;
        Windows::Foundation::EventRegistrationToken         m_gamepadRemovedEventToken;
    };
}
