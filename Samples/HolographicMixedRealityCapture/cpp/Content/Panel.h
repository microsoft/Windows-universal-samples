#pragma once

#include "ControlBase.h"

namespace HolographicMRCSample
{
    // This sample renderer instantiates a basic rendering pipeline.
    class Panel : public ControlBase
    {
    public:
        Panel(const float width, const float height, const float depth,
              const float red,   const float green,  const float blue, const float alpha);

    protected:
        virtual void DoCreateDeviceDependentResources();
        virtual void DoReleaseDeviceDependentResources();

        virtual void DoUpdate(
            const DX::StepTimer& timer,
            HolographicFrame^ holographicFrame,
            SpatialCoordinateSystem^ currentCoordinateSystem,
            SpatialInteractionSourceState^ interactionSource,
            const DirectX::XMMATRIX &parentTransform);
        virtual void DoRender();

    private:
        // Direct3D resources for cube geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>    m_geometryShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_modelConstantBuffer;

        // System resources for cube geometry.
        ModelConstantBuffer                             m_modelConstantBufferData;
        uint32                                          m_indexCount = 0;

        Windows::Foundation::Numerics::float3           m_size;
        Windows::Foundation::Numerics::float4           m_color;

        // Variables used with the rendering loop.
        bool                                            m_loadingComplete = false;

        // If the current D3D Device supports VPRT, we can avoid using a geometry
        // shader just to set the render target array index.
        bool                                            m_usingVprtShaders = false;
    };
}
