#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "ShaderStructures.h"
#include "SourceHandler.h"
#include "RenderingInterfaces.h"

namespace HolographicPaint
{

    // This class stores the objects required for D3D rendering such as the shaders
    // It also provides methods to draw simple shapes such as Cube
    class RenderingHelper : public IRenderer
    {
    public:

        RenderingHelper(const std::shared_ptr<DX::DeviceResources>& deviceResources);

        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();

        bool IsLoadingComplete() const { return m_loadingComplete; }

        // IRenderer implementation
        bool PrepareRendering();

        virtual void SetConstantBuffer(ModelConstantBuffer* pModelConstantBuffer) override;

        virtual void DrawCube() override;

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>            m_deviceResources;

        // Direct3D resources for cube geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>    m_geometryShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_modelConstantBuffer;

        bool                                            m_loadingComplete = false;

        // If the current D3D Device supports VPRT, we can avoid using a geometry
        // shader just to set the render target array index.
        bool                                            m_usingVprtShaders = false;

        unsigned int                                    m_indexCount;
    };
}
