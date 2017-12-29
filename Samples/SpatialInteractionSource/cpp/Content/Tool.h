#pragma once

#include "Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "RenderingInterfaces.h"

namespace HolographicPaint
{

    // Base drawing object
    class Tool : public IBaseRenderedObject
    {
    public:
        Tool(Windows::UI::Color color, float diameter);

        void SetPosition(Windows::Foundation::Numerics::float3 position);
        void SetOrientation(Windows::Foundation::Numerics::quaternion orientation);

        Windows::UI::Color GetColor() const { return m_color; }
        float GetDiameter() const { return m_diameter; }

        // IBaseRenderedObject
        virtual void Update(ID3D11DeviceContext* pDeviceContext) override;
        virtual void Render(ID3D11DeviceContext* pDeviceContext, IRenderer* pRenderingHelper) override;

    protected:
        float m_diameter;
        Windows::UI::Color m_color;
        Windows::Foundation::Numerics::float3 m_position;
        Windows::Foundation::Numerics::quaternion m_orientation;

        bool m_refreshModelConstantBufferData = true;
        ModelConstantBuffer m_modelConstantBufferData;
    };

    // Known tool represented by a cross inside a square and allowing the user to clear the current drawing
    class ClearTool : public Tool
    {
    public:
        ClearTool(Windows::UI::Color color, float diameter);

        // IBaseRenderedObject
        virtual void Update(ID3D11DeviceContext* pDeviceContext) override;
        virtual void Render(ID3D11DeviceContext* pDeviceContext, IRenderer* pRenderingHelper) override;

    protected:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
        uint32 m_indexCount = 0;
        bool m_resetBuffers = true;
    };
}