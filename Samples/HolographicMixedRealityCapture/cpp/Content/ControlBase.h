#pragma once

#include "IControl.h"
#include "ShaderStructures.h"
#include <vector>
#include <ppltasks.h>

using namespace Windows::Graphics::Holographic;
using namespace Windows::Perception::Spatial;
using namespace Windows::UI::Input::Spatial;

namespace HolographicMRCSample
{
    // This sample renderer instantiates a basic rendering pipeline.
    class ControlBase : public IControl
    {
    public:
        ControlBase();
        virtual ~ControlBase();

        // IObject
        virtual void Initialize(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        virtual void CreateDeviceDependentResources();
        virtual void ReleaseDeviceDependentResources();
        virtual void Update(
            const DX::StepTimer& timer,
            HolographicFrame^ holographicFrame,
            SpatialCoordinateSystem^ currentCoordinateSystem,
            SpatialInteractionSourceState^ interactionSource);
        virtual void Update(
            const DX::StepTimer& timer,
            HolographicFrame^ holographicFrame,
            SpatialCoordinateSystem^ currentCoordinateSystem,
            SpatialInteractionSourceState^ interactionSource,
            const DirectX::XMMATRIX &parentTransform);
        virtual void Render();

        virtual void AddChild(const std::shared_ptr<IControl>& object) { m_children.push_back(object); }

        virtual void SetPosition(Windows::Foundation::Numerics::float3 pos) { m_position = pos; }
        virtual void SetRotation(Windows::Foundation::Numerics::float3 rot) { m_rotation = rot; }
        virtual void SetScale(Windows::Foundation::Numerics::float3 scale)  { m_scale = scale; }

        virtual Windows::Foundation::Numerics::float3 GetPosition() { return m_position; }
        virtual Windows::Foundation::Numerics::float3 GetRotation() { return m_rotation; }
        virtual Windows::Foundation::Numerics::float3 GetScale()    { return m_scale; }

        virtual ControlType Type() { return m_type; }
        virtual bool IsFocused()   { return m_focused; }
        virtual bool IsEnabled()   { return m_enabled; }

        virtual void SetEnabled(const bool enabled) { m_enabled = enabled; }

        virtual void GetBoundingBox(DirectX::BoundingOrientedBox &boundingBox)
        {
            m_boundingBox.Transform(boundingBox, DirectX::XMLoadFloat4x4(&m_transform));
        }

        virtual void SetOnAirTapCallback(std::function<void()> callback)
        {
            m_onAirTapCallback = callback;
        }

    protected:
        virtual void DoCreateDeviceDependentResources()  = 0;
        virtual void DoReleaseDeviceDependentResources() = 0;

        virtual void DoUpdate(
            const DX::StepTimer& timer,
            HolographicFrame^ holographicFrame,
            SpatialCoordinateSystem^ currentCoordinateSystem,
            SpatialInteractionSourceState^ interactionSource,
            const DirectX::XMMATRIX &parentTransform) = 0;
        virtual void DoRender() = 0;

    protected:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>    m_deviceResources;

        Windows::Foundation::Numerics::float3   m_position = { 0.0f, 0.0f, 0.0f };
        Windows::Foundation::Numerics::float3   m_rotation = { 0.0f, 0.0f, 0.0f };
        Windows::Foundation::Numerics::float3   m_scale    = { 1.0f, 1.0f, 1.0f };

        ControlType                             m_type = Control_Unknown;
        bool                                    m_enabled = true;
        bool                                    m_focused = false;

        std::function<void()>                   m_onAirTapCallback;

        DirectX::XMFLOAT4X4                     m_transform;

        std::vector<std::shared_ptr<IControl>>  m_children;

        DirectX::BoundingOrientedBox            m_boundingBox;
    };
}
