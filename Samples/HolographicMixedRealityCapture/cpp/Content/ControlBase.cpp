#include "pch.h"
#include "ControlBase.h"
#include "Common\DirectXHelper.h"

using namespace HolographicMRCSample;
using namespace Concurrency;
using namespace DirectX;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
ControlBase::ControlBase()
{
}

void ControlBase::Initialize(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
    m_deviceResources = deviceResources;

    CreateDeviceDependentResources();
}

ControlBase::~ControlBase()
{
    m_children.clear();
}

void ControlBase::CreateDeviceDependentResources()
{
    DoCreateDeviceDependentResources();

    for (auto it = m_children.begin(); it != m_children.end(); it++)
    {
        it->get()->CreateDeviceDependentResources();
    }
}

void ControlBase::ReleaseDeviceDependentResources()
{
    DoReleaseDeviceDependentResources();

    for (auto it = m_children.begin(); it != m_children.end(); it++)
    {
        it->get()->ReleaseDeviceDependentResources();
    }
}

void ControlBase::Update(
    const DX::StepTimer& timer,
    HolographicFrame^ holographicFrame,
    SpatialCoordinateSystem^ currentCoordinateSystem,
    SpatialInteractionSourceState^ interactionSource)
{
    Update(timer, holographicFrame, currentCoordinateSystem, interactionSource, DirectX::XMMatrixIdentity());
}

void ControlBase::Update(
    const DX::StepTimer& timer,
    HolographicFrame^ holographicFrame,
    SpatialCoordinateSystem^ currentCoordinateSystem,
    SpatialInteractionSourceState^ interactionSource,
    const DirectX::XMMATRIX &parentTransform)
{
    DoUpdate(timer, holographicFrame, currentCoordinateSystem, interactionSource, parentTransform);

    for (auto it = m_children.begin(); it != m_children.end(); it++)
    {
        it->get()->Update(timer, holographicFrame, currentCoordinateSystem, interactionSource, DirectX::XMLoadFloat4x4(&m_transform));
    }
}

void ControlBase::Render()
{
    DoRender();

    for (auto it = m_children.begin(); it != m_children.end(); it++)
    {
        it->get()->Render();
    }
}
