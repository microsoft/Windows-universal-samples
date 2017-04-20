#pragma once
#include "RenderingInterfaces.h"

namespace HolographicPaint
{

    class Whiteboard sealed : public IBaseRenderedObject
    {
    public:
        Whiteboard(float width, float height, Windows::Foundation::Numerics::float3 position);

        virtual void Update(ID3D11DeviceContext* pDeviceContext) override;
        virtual void Render(ID3D11DeviceContext* pDeviceContext, IRenderer* pRenderingHelper) override;

        // return a negative value if there is no intersection between ray and board
        bool Intersects(const Windows::Foundation::Numerics::float3& origin, const Windows::Foundation::Numerics::float3& direction, float* distance);

        bool IsValidPosition(const Windows::Foundation::Numerics::float3& position);

    private:
        float m_width;
        float m_height;
        Windows::Foundation::Numerics::float3 m_position;
        ModelConstantBuffer m_modelConstantBufferData;

        DirectX::BoundingBox m_boundingBox;

    };

}
