#pragma once

#include "ControlBase.h"

namespace HolographicMRCSample
{
    // This sample renderer instantiates a basic rendering pipeline.
    class Button : public ControlBase
    {
    public:
        Button(const Windows::Foundation::Numerics::float3 size,
               const Windows::Foundation::Numerics::float4 color,
               const Windows::Foundation::Numerics::float4 focusColor,
               const Windows::Foundation::Numerics::float4 turnedOnColor,
               const DX::TextureIndex textureId);

        void SetSwitch(const bool isOn) { m_turnedOn = isOn; }
        bool GetSwitch()                { return m_turnedOn; }

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
        Microsoft::WRL::ComPtr<ID3D11InputLayout>           m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>        m_geometryShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_modelConstantBuffer;
        Microsoft::WRL::ComPtr<ID3D11SamplerState>          m_samplerState;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_textureView;

        // System resources for cube geometry.
        ModelColorConstantBuffer                            m_modelConstantBufferData;
        uint32                                              m_indexCount = 0;

        Windows::Foundation::Numerics::float3               m_size;
        Windows::Foundation::Numerics::float4               m_color;
        Windows::Foundation::Numerics::float4               m_colorFocus;
        Windows::Foundation::Numerics::float4               m_colorOn;
        DX::TextureIndex                                    m_textureIndex;

        bool                                                m_turnedOn = false;

        // Variables used with the rendering loop.
        bool                                                m_loadingComplete = false;

        // If the current D3D Device supports VPRT, we can avoid using a geometry
        // shader just to set the render target array index.
        bool                                                m_usingVprtShaders = false;
    };
}
