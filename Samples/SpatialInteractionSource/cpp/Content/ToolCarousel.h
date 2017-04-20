#pragma once

#include "Tool.h"
#include "..\Common\StepTimer.h"

namespace HolographicPaint
{
    enum class RotationDirection : int { RotateLeft = -1, RotateRight = 1 };

    // Class responsible for arranging tools in 3D Space and animating them
    class ToolCarousel
    {
    public:
        void SetPosition(
            Windows::Foundation::Numerics::float3 center,
            Windows::Foundation::Numerics::float3 radius,
            Windows::Foundation::Numerics::float3 rotationAxis);

        void ArrangeTools(std::vector<std::shared_ptr<Tool>>& tools, size_t selectedIndex, double baseAngle = 0.0);

        void StartRotation(RotationDirection rotationDirection, const DX::StepTimer& timer);

        void Update(std::vector<std::shared_ptr<Tool>>& tools, size_t& selectedIndex, const DX::StepTimer& timer);

        bool IsRotating() const { return m_isRotating; }

    private:
        Windows::Foundation::Numerics::float3 m_center;
        Windows::Foundation::Numerics::float3 m_radius;
        Windows::Foundation::Numerics::float3 m_rotationAxis;

        bool m_isRotating = false;
        RotationDirection m_rotationDirection;
        double m_rotationStart = 0.0;
    };

}