#pragma once

#include "Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "RenderingInterfaces.h"

namespace HolographicPaint
{

    // This class represent a part of a drawing: it is made of squares positioned in 3D space and joined by quadrilaterals.
    class Stroke : public IBaseRenderedObject
    {
    public:

        Stroke(Windows::UI::Color color);

        void AddPosition(Windows::Foundation::Numerics::float3 position, Windows::Foundation::Numerics::quaternion orientation, float diameter);

        virtual void Update(ID3D11DeviceContext* pDeviceContext) override;
        virtual void Render(ID3D11DeviceContext* pDeviceContext, IRenderer* pRenderingHelper) override;

    private:

        ModelConstantBuffer m_modelConstantBufferData;

        struct Square
        {
            DirectX::XMFLOAT3 TopLeft, TopRight, BottomLeft, BottomRight;
        };

        std::vector<Square> m_squares;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
        uint32 m_indexCount = 0;
        bool m_resetBuffers = true;
    };
}