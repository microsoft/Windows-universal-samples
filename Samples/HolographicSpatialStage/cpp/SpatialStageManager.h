#pragma once

#include "Common\DeviceResources.h"
#include "Content\SceneController.h"

namespace HolographicSpatialStage
{
    class SpatialStageManager
    {
    public:
        SpatialStageManager(std::shared_ptr<DX::DeviceResources> const& deviceResources, std::shared_ptr<SceneController> const& sceneController);    
        SpatialStageManager(const SpatialStageManager&) = delete;
        SpatialStageManager(SpatialStageManager&&) = default;
        ~SpatialStageManager();

        Windows::Perception::Spatial::SpatialCoordinateSystem^ GetCoordinateSystemForCurrentStage();

    private:
        void OnCurrentChanged(Platform::Object^ /*o*/);

        std::vector<Windows::Foundation::Numerics::float3> CreateCircle(Windows::Foundation::Numerics::float3 center, float radius, int divisions, float radians);
        std::vector<unsigned short> TriangulatePoints(std::vector<Windows::Foundation::Numerics::float3> const& vertices);

        std::shared_ptr<DX::DeviceResources>                                m_deviceResources;
        std::shared_ptr<SceneController>                                    m_sceneController;

        Windows::Foundation::EventRegistrationToken                         m_spatialStageChangedEventToken;

        std::shared_ptr<SceneObject>                                        m_stageBoundsShape;
        std::shared_ptr<SceneObject>                                        m_stageVisibleAreaIndicatorShape;

        Windows::Perception::Spatial::SpatialCoordinateSystem^              m_currentCoordinateSystem;
        Windows::Perception::Spatial::SpatialStageFrameOfReference^         m_currentStage;
        Windows::Perception::Spatial::SpatialStationaryFrameOfReference^    m_stationaryFrameOfReference;
    };
}