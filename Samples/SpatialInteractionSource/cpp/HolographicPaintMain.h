#pragma once

#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"
#include "Content\RenderingHelper.h"
#include "Content\Stroke.h"
#include "Content\RenderingInterfaces.h"
#include "Content\Tool.h"
#include "Content\ToolCarousel.h"
#include "Content\Whiteboard.h"
#include "SourceHandler.h"
#include "DPadHelper.h"

// Updates, renders, and presents holographic content using Direct3D.
namespace HolographicPaint
{

    class HolographicPaintMain : public DX::IDeviceNotify, public ISourceCommandHandler
    {
    public:
        HolographicPaintMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~HolographicPaintMain();

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

        // ISourceCommandHandler
        virtual void OnDPadAction(SourceHandler* sender, DPadAction action) override;
        virtual void OnStrokeAdded(SourceHandler* sender, std::shared_ptr<IBaseRenderedObject> stroke) override;
        virtual void OnEnterToolChoice(SourceHandler* sender, Windows::Perception::PerceptionTimestamp^ timestamp) override;
        virtual void OnSelectTool(SourceHandler* sender) override;

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

        // Events Handlers
        void OnSourceDetected(Windows::UI::Input::Spatial::SpatialInteractionManager ^sender, Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^args);
        void OnSourceLost(Windows::UI::Input::Spatial::SpatialInteractionManager ^sender, Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^args);
        void OnSourcePressed(Windows::UI::Input::Spatial::SpatialInteractionManager ^sender, Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^args);
        void OnSourceUpdated(Windows::UI::Input::Spatial::SpatialInteractionManager ^sender, Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^args);
        void OnInteractionDetected(Windows::UI::Input::Spatial::SpatialInteractionManager ^sender, Windows::UI::Input::Spatial::SpatialInteractionDetectedEventArgs ^args);

        // Position tools based on current heading
        void PositionTools(Windows::UI::Input::Spatial::SpatialPointerPose^ pointerPose, HandAssignment side);
        // Returns the tool's index from a source
        size_t FindToolIndex(unsigned int sourceId);
        // Assign new tool to a source
        void SetToolByIndex(unsigned int sourceId, size_t toolIndex);
        // Get the SourceHandler associated to a SpatialInteractionSource Id
        SourceHandler* GetSourceHandler(unsigned int sourceId)
        {
            auto it = m_sourceMap.find(sourceId);
            return (it == m_sourceMap.end()) ? nullptr : it->second.get();
        }
        // Detects on which side the given source is located
        HandAssignment TryGetHandedness(SourceHandler* sender, Windows::UI::Input::Spatial::SpatialPointerPose^ pointerPose);

        // Renders a colorful holographic cube that's 20 centimeters wide. This sample content
        // is used to demonstrate world-locked rendering.
        std::unique_ptr<RenderingHelper>                                m_renderingHelper;

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
        Windows::Foundation::EventRegistrationToken                     m_sourceDetectedToken;
        Windows::Foundation::EventRegistrationToken                     m_sourceLostToken;
        Windows::Foundation::EventRegistrationToken                     m_sourcePressedToken;
        Windows::Foundation::EventRegistrationToken                     m_sourceUpdatedToken;
        Windows::Foundation::EventRegistrationToken                     m_interactionDetectedToken;

        Windows::UI::Input::Spatial::SpatialInteractionManager^         m_spatialInteractionManager;

        Windows::Foundation::Numerics::float3                           m_focusPoint = { 0.0f, 0.0f, 2.0f };

        // Current application mode and slection
        ApplicationMode                                                 m_currentMode = ApplicationMode::Drawing;
        unsigned int                                                    m_currentSourceId = NoSourceId;
        size_t                                                          m_currentToolIndex;

        // Currently detected SpatialInteractionSources
        std::map<unsigned int, std::shared_ptr<SourceHandler>>          m_sourceMap;
        std::vector<std::shared_ptr<SourceHandler>>                     m_sources;

        // Drawing itself
        std::vector<std::shared_ptr<IBaseRenderedObject>>               m_strokes;

        // Tools collection, known tools index, tool layout helper
        std::vector<std::shared_ptr<Tool>>                              m_tools;
        size_t                                                          m_clearToolIndex;
        ToolCarousel                                                    m_toolsCarousel;

        std::shared_ptr<Whiteboard>                                     m_whiteboard;
    };
}
