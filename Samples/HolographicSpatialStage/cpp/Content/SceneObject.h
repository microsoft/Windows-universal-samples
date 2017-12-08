#pragma once

#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"
#include "Content\ShaderStructures.h"

namespace HolographicSpatialStage
{
    class SceneObject
    {
    public:
        SceneObject(
            const std::shared_ptr<DX::DeviceResources>& deviceResources,
            DirectX::XMFLOAT3 const& color,
            Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem);

        SceneObject(
            const std::shared_ptr<DX::DeviceResources>& deviceResources,
            std::vector<DirectX::XMFLOAT3> const& vertices,
            DirectX::XMFLOAT3 const& color,
            Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem);

        SceneObject(
            const std::shared_ptr<DX::DeviceResources>& deviceResources,
            std::vector<DirectX::XMFLOAT3> const& vertices,
            std::vector<unsigned short> const& indices,
            DirectX::XMFLOAT3 const& color,
            Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem);

        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();
        void Update(const DX::StepTimer& timer, Windows::Perception::Spatial::SpatialCoordinateSystem^ viewCoords);
        void Render();

        void EnableWireframe() { m_useWireframe = true; }

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>            m_deviceResources;

        // Direct3D resources.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>    m_geometryShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_modelConstantBuffer;

        // Rasterizer states, for different rendering modes.
        Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_defaultRasterizerState;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_wireframeRasterizerState;

        // System resources.
        ModelConstantBuffer                             m_modelConstantBufferData;
        std::vector<VertexPositionColor>                m_vertices;
        std::vector<unsigned short>                     m_indices;
        unsigned int                                    m_indexCount;

        Windows::Perception::Spatial::SpatialCoordinateSystem^ m_coordinateSystem;

        // Variables used with the rendering loop.
        bool                                            m_loadingComplete = false;
        bool                                            m_canRenderThisFrame = false;
        bool                                            m_useWireframe = false;

        // If the current D3D Device supports VPRT, we can avoid using a geometry
        // shader just to set the render target array index.
        bool                                            m_usingVprtShaders = false;

        // Vertices for a cube.
        const std::vector<DirectX::XMFLOAT3> c_cubeVertices =
            { {
                { DirectX::XMFLOAT3(-0.01f, -0.01f, -0.01f) },
                { DirectX::XMFLOAT3(-0.01f, -0.01f,  0.01f) },
                { DirectX::XMFLOAT3(-0.01f,  0.01f, -0.01f) },
                { DirectX::XMFLOAT3(-0.01f,  0.01f,  0.01f) },
                { DirectX::XMFLOAT3( 0.01f, -0.01f, -0.01f) },
                { DirectX::XMFLOAT3( 0.01f, -0.01f,  0.01f) },
                { DirectX::XMFLOAT3( 0.01f,  0.01f, -0.01f) },
                { DirectX::XMFLOAT3( 0.01f,  0.01f,  0.01f) },
            } };

        // Indices for a six-sided shape.
        const std::vector<unsigned short> c_cubeIndices =
            { {
                2,1,0, // -x
                2,3,1,

                6,4,5, // +x
                6,5,7,

                0,1,5, // -y
                0,5,4,

                2,6,7, // +y
                2,7,3,

                0,4,6, // -z
                0,6,2,

                1,3,7, // +z
                1,7,5,
            } };
    };
}
