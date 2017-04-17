#pragma once

#include "Panel.h"

namespace HolographicMRCSample
{
    class Cursor
    {
    public:
        Cursor(const float width, const float height);
        void Initialize(const std::shared_ptr<DX::DeviceResources>& deviceResources);

        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();

        void SetDistance(const float distance) { m_distance = distance; }

        void Update(
            const DX::StepTimer& timer,
            HolographicFrame^ holographicFrame,
            SpatialCoordinateSystem^ currentCoordinateSystem,
            SpatialInteractionSourceState^ interactionSource);
        void Render();

    private:
        // Repositions the sample hologram.
        void PositionCursor(Windows::UI::Input::Spatial::SpatialPointerPose^ pointerPose,
                            DirectX::XMMATRIX &modelTransform);

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>                m_deviceResources;

        Microsoft::WRL::ComPtr<ID3D11InputLayout>           m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>        m_geometryShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_modelConstantBuffer;
        Microsoft::WRL::ComPtr<ID3D11SamplerState>          m_samplerState;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_cursorTextureView;

        ModelColorConstantBuffer                            m_modelConstantBufferData;
        uint32                                              m_indexCount = 0;

        Windows::Foundation::Numerics::float2               m_size;
        DX::TextureIndex                                    m_textureIndex;
        float                                               m_distance = 2.0f;

        // Variables used with the rendering loop.
        bool                                                m_loadingComplete = false;

        // If the current D3D Device supports VPRT, we can avoid using a geometry
        // shader just to set the render target array index.
        bool                                                m_usingVprtShaders = false;
    };
}
