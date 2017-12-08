#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "ShaderStructures.h"
#include "SceneObject.h"

namespace HolographicSpatialStage
{
    // This sample renderer instantiates a basic rendering pipeline.
    class SceneController
    {
    public:
        SceneController(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();
        void Update(const DX::StepTimer& timer, Windows::Perception::Spatial::SpatialCoordinateSystem^ viewCoords);
        void Render();

        void AddSceneObject(std::shared_ptr<SceneObject>& newObject) { m_sceneObjects.push_back(newObject); }
        void ClearSceneObjects() { m_sceneObjects.clear(); }

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>        m_deviceResources;
        std::vector<std::shared_ptr<SceneObject>>   m_sceneObjects;
    };
}
