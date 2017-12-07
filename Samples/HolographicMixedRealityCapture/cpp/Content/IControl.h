#pragma once

#include <memory>
#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include <DirectXCollision.h>

namespace HolographicMRCSample
{
    typedef enum
    {
        Control_Unknown = 0,
        Control_Panel,
        Control_Button,
        Control_CameraPreview
    } ControlType;

    class IControl
    {
    public:
        virtual void Initialize(const std::shared_ptr<DX::DeviceResources>& deviceResources) = 0;
        virtual void CreateDeviceDependentResources() = 0;
        virtual void ReleaseDeviceDependentResources() = 0;

        virtual void Update(
            const DX::StepTimer& timer,
            Windows::Graphics::Holographic::HolographicFrame^ holographicFrame,
            Windows::Perception::Spatial::SpatialCoordinateSystem^ currentCoordinateSystem,
            Windows::UI::Input::Spatial::SpatialInteractionSourceState^ interactionSource) = 0;
        virtual void Update(
            const DX::StepTimer& timer,
            Windows::Graphics::Holographic::HolographicFrame^ holographicFrame,
            Windows::Perception::Spatial::SpatialCoordinateSystem^ currentCoordinateSystem,
            Windows::UI::Input::Spatial::SpatialInteractionSourceState^ interactionSource,
            const DirectX::XMMATRIX &parentTransform) = 0;
        virtual void Render() = 0;

        virtual void AddChild(const std::shared_ptr<IControl>& object) = 0;

        virtual void SetPosition(Windows::Foundation::Numerics::float3 pos) = 0;
        virtual void SetRotation(Windows::Foundation::Numerics::float3 rot) = 0;
        virtual void SetScale(Windows::Foundation::Numerics::float3 scale) = 0;

        virtual Windows::Foundation::Numerics::float3 GetPosition() = 0;
        virtual Windows::Foundation::Numerics::float3 GetRotation() = 0;
        virtual Windows::Foundation::Numerics::float3 GetScale() = 0;

        virtual ControlType Type() = 0;
        virtual bool IsFocused() = 0;
        virtual bool IsEnabled() = 0;

        virtual void SetEnabled(const bool enabled) = 0;

        virtual void GetBoundingBox(DirectX::BoundingOrientedBox &boundingBox) = 0;

        virtual void SetOnAirTapCallback(std::function<void()> callback) = 0;
    };
}
