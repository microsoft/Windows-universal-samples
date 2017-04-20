#include "pch.h"
#include "SourceHandler.h"
#include "SpatialControllerHandler.h"
#include "HandHandler.h"
#include "..\Common\DirectXHelper.h"

using namespace HolographicPaint;
using namespace DirectX;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Devices::Haptics;
using namespace Windows::Perception::Spatial;
using namespace Windows::UI::Input::Spatial;
using namespace std::placeholders;

// If we lose tracking of a controller for too long, we consider its position as definitely invalid
// Note that TimeStamp's unit is 100 nanoseconds
static const long long TrackingTimeout = 50000000; // 5 seconds

void SourceHandler::PositionInformation::CheckIfObsolete(long long now)
{
    if (IsCurrent)
    {
        LastValueTime = now;
    }
    else if (HasValue && (now - LastValueTime > TrackingTimeout))
    {
        HasValue = false;
    }
}

SourceHandler::SourceHandler(SpatialInteractionSource^ source, ISourceCommandHandler* eventsHandler, std::shared_ptr<Whiteboard>& target)
    : m_sourceId(source->Id), m_sourceKind(source->Kind), m_eventsHandler(eventsHandler), m_targetBoard(target)
{
}

void SourceHandler::OnSourceUpdated(SpatialInteractionSourceState^ state, SpatialCoordinateSystem^ coordinateSystem)
{
    SpatialInteractionSourceProperties^ properties = state->Properties;
    SpatialInteractionSourceLocation^ location = properties->TryGetLocation(coordinateSystem);
    m_position.IsCurrent = false;
    if (location != nullptr)
    {
        IBox<float3>^ position = location->Position;
        if (position != nullptr)
        {
            m_position.HasValue = true;
            m_position.Position = position->Value;
            m_position.IsCurrent = true;
            IBox<quaternion>^ orientation = location->Orientation;
            if (orientation == nullptr)
            {
                m_position.Orientation = quaternion::identity();
            }
            else
            {
                m_position.Orientation = orientation->Value;
            }
        }
    }
    m_position.CheckIfObsolete(state->Timestamp->TargetTime.UniversalTime);
}

void SourceHandler::Update(ID3D11DeviceContext*)
{
    if (m_refreshModelConstantBufferData)
    {
        const XMMATRIX modelRotation = XMMatrixRotationQuaternion(XMLoadQuaternion(&m_toolPosition.Orientation));

        const float3 toolScale = GetToolScale();
        const XMMATRIX modelScale = XMMatrixScaling(toolScale.x, toolScale.y, toolScale.z);

        const XMMATRIX modelTranslation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_toolPosition.Position));

        // Multiply to get the transform matrix.
        // Note that this transform does not enforce a particular coordinate system. The calling
        // class is responsible for rendering this content in a consistent manner.
        const XMMATRIX modelTransform = modelScale * modelRotation * modelTranslation;

        // The view and projection matrices are provided by the system; they are associated
        // with holographic cameras, and updated on a per-camera basis.
        // Here, we provide the model transform for the sample hologram. The model transform
        // matrix is transposed to prepare it for the shader.
        XMStoreFloat4x4(&m_modelConstantBufferData.model, XMMatrixTranspose(modelTransform));


        m_refreshModelConstantBufferData = false;
    }

    if (m_toolPosition.IsCurrent)
    {
        m_modelConstantBufferData.color = DX::GetDXColor(m_currentTool->GetColor());
    }
    else
    {
        m_modelConstantBufferData.color = XMFLOAT4(1.0f, .3f, .3f, .3f);
    }

}

void SourceHandler::Render(ID3D11DeviceContext* pDeviceContext, IRenderer* pRenderingHelper)
{
    // We do not render the controller until it has been displayed at least once
    if (m_toolPosition.HasValue)
    {
        pRenderingHelper->SetConstantBuffer(&m_modelConstantBufferData);
        pRenderingHelper->DrawCube();
    }
}

void SourceHandler::SetCurrentTool(std::shared_ptr<Tool> tool)
{
    m_currentTool = tool;
    m_refreshModelConstantBufferData = true;
}

void SourceHandler::ComposeStroke()
{
    if (m_currentTool)
    {
        if (!m_currentStroke)
        {
            m_currentStroke = std::make_shared<Stroke>(m_currentTool->GetColor());
            m_eventsHandler->OnStrokeAdded(this, m_currentStroke);
        }
    }
    m_currentStroke->AddPosition(m_toolPosition.Position, m_toolPosition.Orientation, m_currentTool->GetDiameter());
}

void SourceHandler::TerminateStroke()
{
    m_currentStroke.reset();
}

bool SourceHandler::Make(std::shared_ptr<SourceHandler>* handler, SpatialInteractionSource^ source, ISourceCommandHandler* eventsHandler, std::shared_ptr<Whiteboard>& target)
{
    switch (source->Kind)
    {
    case SpatialInteractionSourceKind::Hand:
        *handler = std::make_shared<HandHandler>(source, eventsHandler, target);
        return true;
    case SpatialInteractionSourceKind::Controller:
        if (source->IsPointingSupported)
        {
            *handler = std::make_shared<SpatialControllerHandler>(source, eventsHandler, target);
            return true;
        }
    }
    return false;
}
