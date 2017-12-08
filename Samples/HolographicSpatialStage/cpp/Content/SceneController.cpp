#include "pch.h"

#include "SceneController.h"
#include "Common\DirectXHelper.h"

using namespace HolographicSpatialStage;
using namespace Windows::Perception::Spatial;

SceneController::SceneController(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
}

void SceneController::Update(const DX::StepTimer& timer, SpatialCoordinateSystem^ viewCoords)
{
    for (auto& object : m_sceneObjects)
    {
        object->Update(timer, viewCoords);
    }
}

void SceneController::Render()
{
    for (auto& object : m_sceneObjects)
    {
        object->Render();
    }
}

void SceneController::CreateDeviceDependentResources()
{
    for (auto& object : m_sceneObjects)
    {
        object->CreateDeviceDependentResources();
    }
}

void SceneController::ReleaseDeviceDependentResources()
{
    for (auto& object : m_sceneObjects)
    {
        object->ReleaseDeviceDependentResources();
    }
}

