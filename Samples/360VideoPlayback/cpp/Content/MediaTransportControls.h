#pragma once

#include "..\Common\CameraResources.h"
#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "..\Common\BaseControl.h"

using namespace DX;
namespace _360VideoPlayback
{
    // Media Transport Controls (MTC) for playback in a 3D environment
    // Although structured to a RenderObject, the MTC itself has no Geometry, it contains other items which are renderable
    class MediaTransportControls
    {
    public:
        MediaTransportControls();

        void Initialize();
        void Update(const DX::StepTimer& timer, Windows::UI::Input::Spatial::SpatialPointerPose^ cameraPose);
        void Render();
        void PerformPressedAction();
        void ApplyShaders();
        void Show(Windows::UI::Input::Spatial::SpatialPointerPose^ cameraPose, float distance);
        void Hide();
        bool IsFocused();
        bool IsVisible() { return m_isVisible; };

    private:
        float m_inactiveSecondsCounter = 0.0f;
        void AddExitButton();
        void AddPlayPauseButton();
        void AddSeekbar();
        inline Windows::Foundation::Numerics::float4 ComputeLookAtQuaternion(Windows::Foundation::Numerics::float3 objectPosition, Windows::Foundation::Numerics::float3 targetPosition, Windows::Foundation::Numerics::float3 upVector)
        {
            // The Windows::Foundation::Numerics::make_float4x4_look_at() and DirectX::XMLookAtRH() functions are both designed to compute ViewProjection Matrices
            // View Projection Matrices are typically the inverse of the Camera's transform matrices and are used to convert vertices from World space to View space
            // To "fix" this for our application of pointing models at things, we just need to inverse the resulting ViewProjection from your inputs.
            // You can also roll your own vector math to just compute the rotation directly, but we'll stick with the platform calls for now since the inverse call is a minor tax.
            Windows::Foundation::Numerics::float4x4 lookAtViewMatrix = Windows::Foundation::Numerics::make_float4x4_look_at(targetPosition, objectPosition, upVector);
            Windows::Foundation::Numerics::quaternion worldSpaceLookAtQuaternion = Windows::Foundation::Numerics::inverse(Windows::Foundation::Numerics::make_quaternion_from_rotation_matrix(lookAtViewMatrix));
            return Windows::Foundation::Numerics::float4(worldSpaceLookAtQuaternion.x, worldSpaceLookAtQuaternion.y, worldSpaceLookAtQuaternion.z, worldSpaceLookAtQuaternion.w);
        };

        std::shared_ptr<BaseControl> m_exitButtonControl;
        std::list<std::shared_ptr<BaseControl>> m_baseControls;
        std::shared_ptr<BaseControl> m_playPauseButtonControl;
        std::shared_ptr<BaseControl> m_seekbarControl;
        bool m_isVisible;
        bool m_shadersLoaded;
        Windows::Foundation::Numerics::float4 m_rotation;
        Windows::Foundation::Numerics::float3 m_position;
        Windows::Foundation::Numerics::float4x4 m_transform;
    };

}