#pragma once
#include "Content\Stroke.h"
#include "Content\RenderingInterfaces.h"
#include "Content\Tool.h"
#include "Content\Whiteboard.h"
#include "DPadHelper.h"

namespace HolographicPaint
{
    constexpr unsigned int NoSourceId = static_cast<unsigned int>(-1);
    constexpr unsigned int AnyHandSourceId = static_cast<unsigned int>(-2);

    constexpr float DefaultControllerScale = .05f;

    enum class ApplicationMode { Drawing, ChooseTool };

    enum class HandAssignment { Unknown, LeftHand, RightHand };

    class SourceHandler;

    // Callbacks allowing the application to ignore the differences between different kind of SpatialInteractionSource
    interface ISourceCommandHandler
    {
        virtual void OnDPadAction(SourceHandler* sender, DPadAction action) = 0;
        virtual void OnStrokeAdded(SourceHandler* sender, std::shared_ptr<IBaseRenderedObject> stroke) = 0;
        virtual void OnEnterToolChoice(SourceHandler* sender, Windows::Perception::PerceptionTimestamp^ timestamp) = 0;
        virtual void OnSelectTool(SourceHandler* sender) = 0;
    };

    // This class and the classes derived from it provide identical services and events to the application
    // for two kinds of SpatialInteractionSource: Hands and Six DOF Controllers
    // It handles input and associates a drawing tool to the SpatialInteractionSource.
    class SourceHandler : public IBaseRenderedObject
    {
    protected:
        SourceHandler(Windows::UI::Input::Spatial::SpatialInteractionSource^ source, ISourceCommandHandler* eventsHandler, std::shared_ptr<Whiteboard>& target);

    public:
        static bool Make(
            std::shared_ptr<SourceHandler>* handler,
            Windows::UI::Input::Spatial::SpatialInteractionSource^ source,
            ISourceCommandHandler* eventsHandler,
            std::shared_ptr<Whiteboard>& target);

        virtual void Update(ID3D11DeviceContext* pDeviceContext) override;
        virtual void Render(ID3D11DeviceContext* pDeviceContext, IRenderer* pRenderingHelper) override;

        // Update the source and returns a Pointer pose if applicable
        virtual void OnSourceUpdated(Windows::UI::Input::Spatial::SpatialInteractionSourceState^ state, Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem);
        virtual void OnInteractionDetected(Windows::UI::Input::Spatial::SpatialInteraction^ interaction, Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem) {}
        virtual void OnSourcePressed(Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs^ args) {}

        unsigned int SourceId() const { return m_sourceId; }
        Windows::UI::Input::Spatial::SpatialInteractionSourceKind GetSourceKind() const { return m_sourceKind; }

        bool IsPositionCurrent() const { return m_position.IsCurrent; }
        Windows::Foundation::Numerics::float3 GetPosition() const { return m_position.Position; }

        void SetCurrentTool(std::shared_ptr<Tool> tool);

        Tool* GetCurrentTool() const { return m_currentTool.get(); }

        virtual Windows::Foundation::Numerics::float3 GetToolPosition() const
        {
            return m_toolPosition.Position;
        }

        virtual Windows::Foundation::Numerics::float3 GetToolScale() const
        {
            return Windows::Foundation::Numerics::float3(m_currentTool == nullptr ? DefaultControllerScale : m_currentTool->GetDiameter());
        }

        void SetApplicationMode(ApplicationMode newMode)
        {
            m_currentMode = newMode;
        }

    protected:
        unsigned int m_sourceId;
        Windows::UI::Input::Spatial::SpatialInteractionSourceKind m_sourceKind;

        ApplicationMode m_currentMode;

        struct PositionInformation
        {
            bool HasValue = false;
            bool IsCurrent= false;
            Windows::Foundation::Numerics::float3 Position = Windows::Foundation::Numerics::float3::zero();
            Windows::Foundation::Numerics::quaternion Orientation = Windows::Foundation::Numerics::quaternion::identity();
            long long LastValueTime = 0;

            void CheckIfObsolete(long long now);
        };

        // A SpatialInteractionSource has a position and can also (in the case of controllers) have a pointing direction
        // m_poosition keeps track of the position of the source itself
        PositionInformation m_position;
        // m_toolPosition keeps track of the position where drawing will happen and is computed either from the pointing
        // direction of the controller, or from the gaze in the case of Hands
        // It will be either on the target board if the pointing ray intersects the board
        // or - in the case of a controller - a few centimeters in the pointing direction if it does not intersect the board
        PositionInformation m_toolPosition;

        // Currently selected tool
        std::shared_ptr<Tool> m_currentTool;

        // Caching of location
        bool m_refreshModelConstantBufferData = true;
        ModelConstantBuffer m_modelConstantBufferData;

        ISourceCommandHandler* m_eventsHandler = nullptr;

        // Drawing methods and state
        virtual void ComposeStroke();
        virtual void TerminateStroke();
        std::shared_ptr<Stroke> m_currentStroke;

        std::shared_ptr<Whiteboard> m_targetBoard;
    };
    
}
