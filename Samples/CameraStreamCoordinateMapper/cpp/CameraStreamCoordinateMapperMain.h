//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************

#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"

#include "Content\GpuCoordinateMapper.h"

namespace DX {
    class GpuPerformanceTimer;
}

ref class FirstPersonCamera;

// Renders Direct2D and 3D content on the screen.
namespace CameraStreamCoordinateMapper
{
    class QuadRenderer;
    class TextRenderer;
    class CameraCoordinateMapper;

    class CameraStreamCoordinateMapperMain : public DX::IDeviceNotify
    {
    public:
        CameraStreamCoordinateMapperMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        concurrency::task<void> CreateResourcesAsync();

        ~CameraStreamCoordinateMapperMain();
        void CreateWindowSizeDependentResources();
        void Update();
        bool Render();

        concurrency::task<void> OnSuspendingAsync();
        void OnResuming();

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

    private:
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        DX::StepTimer m_timer;

        FirstPersonCamera^ m_camera;

        std::unique_ptr<QuadRenderer> m_quadRenderer;
        std::unique_ptr<TextRenderer> m_statisticsTextRenderer;
        std::unique_ptr<TextRenderer> m_helpTextRenderer;

        std::unique_ptr<DX::GpuPerformanceTimer> m_gpuPerformanceTimer;
        std::shared_ptr<CameraCoordinateMapper> m_cameraCoordinateMapper;

        bool m_streamingEnabled = true;
        bool m_freeRoamCameraEnabled = true;
        bool m_showHelpText = false;
        bool m_readyToRender = false;
    };
}