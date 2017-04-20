#include "pch.h"
#include "ToolCarousel.h"

using namespace HolographicPaint;
using namespace DirectX;
using namespace Windows::Foundation::Numerics;

static const double RotationTimeInSeconds = .5;

void ToolCarousel::SetPosition(
    float3 center,
    float3 radius,
    float3 rotationAxis)
{
    m_center = center;
    m_radius = radius;
    m_rotationAxis = rotationAxis;
}

void ToolCarousel::ArrangeTools(std::vector<std::shared_ptr<Tool>>& tools, size_t selectedIndex, double baseAngle)
{
    const XMVECTOR radiusVector = XMLoadFloat3(&m_radius);
    const XMVECTOR rotationAxisVector = XMLoadFloat3(&m_rotationAxis);

    const size_t toolsCount = tools.size();
    for (size_t idx = 0; idx < toolsCount; ++idx)
    {
        // First controller is oriented towards the user
        const size_t angleIndex = idx + toolsCount - selectedIndex;
        const double angle = baseAngle + M_PI + (2.0 * M_PI * static_cast<double>(angleIndex) / static_cast<double>(toolsCount));

        const XMMATRIX rotationMatrix = XMMatrixRotationAxis(rotationAxisVector, static_cast<float>(angle));

        float3 toolOffset;
        XMStoreFloat3(&toolOffset, XMVector3Transform(radiusVector, rotationMatrix));
        tools[idx]->SetPosition(m_center + toolOffset);
    }
}

void ToolCarousel::StartRotation(RotationDirection rotationDirection, const DX::StepTimer& timer)
{
    m_isRotating = true;
    m_rotationStart = timer.GetTotalSeconds();
    m_rotationDirection = rotationDirection;
}

void ToolCarousel::Update(std::vector<std::shared_ptr<Tool>>& tools, size_t& selectedIndex, const DX::StepTimer& timer)
{
    if (m_isRotating)
    {
        double elapsed = timer.GetTotalSeconds() - m_rotationStart;
        if (elapsed > RotationTimeInSeconds)
        {
            m_isRotating = false;
            selectedIndex = (selectedIndex + tools.size() + static_cast<int>(m_rotationDirection)) % tools.size();
        }
        else
        {
            double baseAngle = -static_cast<double>(m_rotationDirection) * (elapsed / RotationTimeInSeconds) * (M_PI * 2.0 / static_cast<double>(tools.size()));
            ArrangeTools(tools, selectedIndex, baseAngle);
        }
    }
}
