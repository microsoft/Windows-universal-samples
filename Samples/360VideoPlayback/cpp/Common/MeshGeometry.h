#pragma once
#include "..\Content\ShaderStructures.h"
namespace DX
{    
    struct MeshGeometry
    {
    public:
        const D3D11_PRIMITIVE_TOPOLOGY Topology;
        const Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
        const Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
        const uint32 VertexStride;
        const uint32 IndexCount;

        static std::shared_ptr<MeshGeometry> MakeTexturedCube();
        static std::shared_ptr<MeshGeometry> MakeTexturedCylinder(unsigned short tessellation);
        
        MeshGeometry(
            D3D11_PRIMITIVE_TOPOLOGY topology,
            const Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer,
            const Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer,
            uint32 vertexStride,
            uint32 indexCount) :
                Topology(topology),
                VertexBuffer(vertexBuffer),
                IndexBuffer(indexBuffer),
                VertexStride(vertexStride),
                IndexCount(indexCount)
        {
        }

    private:
        static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateVertexBuffer(unsigned int numVertices, const _360VideoPlayback::VertexPositionTexture* vertexData);
        static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateIndexBuffer(unsigned int numIndices, const unsigned short* indexData);

        static std::vector<_360VideoPlayback::VertexPositionTexture> MakeCylinderFaceVertices(unsigned short tessellation, float z, std::function<DirectX::XMFLOAT2(float x, float y)> uvGenerator);
        static std::vector<unsigned short> MakeCylinderFaceIndices(unsigned short tessellation, unsigned short vertexIndexOffset, unsigned short centerVertexIndex, bool isFrontFace);
        static std::vector<unsigned short> MakeCylinderSideIndices(unsigned short tessellation, unsigned short vertexIndexOffset, unsigned short cylinderFaceVertexStride);
    };
} 
