#include "pch.h"
#include "MeshGeometry.h"
#include "..\360VideoPlaybackMain.h"
#include "DeviceResources.h"
#include "DirectXHelper.h"
#include "..\Content\ShaderStructures.h"

using namespace _360VideoPlayback;
using namespace DirectX;
using namespace DX;
using namespace Microsoft::WRL;
using namespace std;
using namespace Windows::Foundation::Numerics;

shared_ptr<MeshGeometry> MeshGeometry::MakeTexturedCube()
{
    _360VideoPlayback::VertexPositionTexture cubeVertices[] =
    {
        { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT2(-1.0f, -1.0f) }, // +Y (top face)
        { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT2(-1.0f, -1.0f) },
        { XMFLOAT3(0.5f, 0.5f,  0.5f), XMFLOAT2(-1.0f, -1.0f) },
        { XMFLOAT3(-0.5f, 0.5f,  0.5f), XMFLOAT2(-1.0f, -1.0f) },

        { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT2(-1.0f, -1.0f) }, // -Y (bottom face)
        { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT2(-1.0f, -1.0f) },
        { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(-1.0f, -1.0f) },
        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(-1.0f, -1.0f) },

        { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT2(-1.0f, -1.0f) }, // +X (right face)
        { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT2(-1.0f, -1.0f) },
        { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(-1.0f, -1.0f) },
        { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT2(-1.0f, -1.0f) },

        { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT2(-1.0f, -1.0f) }, // -X (left face)
        { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT2(-1.0f, -1.0f) },
        { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT2(-1.0f, -1.0f) },
        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(-1.0f, -1.0f) },

        { XMFLOAT3(-0.5f,  0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) }, // +Z (front face)
        { XMFLOAT3(0.5f,  0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) }, // WARNING: This Front face is tied directly to the DEFAULT_CUBE_BOUNDING_PLANE. If you modify this, update both
        { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },

        { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT2(-1.0f, -1.0f) }, // -Z (back face)
        { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT2(-1.0f, -1.0f) },
        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(-1.0f, -1.0f) },
        { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(-1.0f, -1.0f) },
    };

    unsigned short cubeIndices[] =
    {
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7,

        8, 9, 10,
        8, 10, 11,

        12, 13, 14,
        12, 14, 15,

        16, 17, 18,
        16, 18, 19,

        20, 21, 22,
        20, 22, 23
    };

    ComPtr<ID3D11Buffer> vertexBuffer = CreateVertexBuffer(ARRAYSIZE(cubeVertices), cubeVertices);
    ComPtr<ID3D11Buffer> indexBuffer = CreateIndexBuffer(ARRAYSIZE(cubeIndices), cubeIndices);

    return make_shared<MeshGeometry>(
        D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        vertexBuffer,
        indexBuffer,
        static_cast<uint32>(sizeof(_360VideoPlayback::VertexPositionTexture)),
        static_cast<uint32>(ARRAYSIZE(cubeIndices))
        );
}

shared_ptr<MeshGeometry> MeshGeometry::MakeTexturedCylinder(unsigned short tessellation)
{
    const float frontFaceZ = 0.5f;
    const float backFaceZ = -0.5f;
    vector<VertexPositionTexture> vertices = {
        {{ 0.0f, 0.0f, frontFaceZ }, { 0.5f, 0.5f }}, // Front face center vertex
        {{ 0.0f, 0.0f, backFaceZ },  { 0.0f, 0.0f }}  // Back face center vertex
    };

    const unsigned short frontFaceCenterVertexIndex = 0;
    const unsigned short backFaceCenterVertexIndex = 1;

    unsigned short frontFaceVerticesStartIndex = static_cast<unsigned short>(vertices.size());
    auto frontFaceVertices = MeshGeometry::MakeCylinderFaceVertices(
        tessellation,
        frontFaceZ,
        [](float x, float y) -> XMFLOAT2
    {
        const float u = x + 0.5f;
        const float v = -y + 0.5f;

        return { u, v };
    }
    );
    copy(begin(frontFaceVertices), end(frontFaceVertices), back_inserter(vertices));

    // While the vertex positions of the front face and the "front" side vertices are the same,
    // we want the UV coordinates to be different.
    // So duplicate the vertices and set the UV coordinates to [0,0]
    unsigned short frontFaceSideVerticesStartIndex = static_cast<unsigned short>(vertices.size());
    auto frontFaceSideVertices = MeshGeometry::MakeCylinderFaceVertices(
        tessellation,
        frontFaceZ,
        [](float /*x*/, float /*y*/) -> XMFLOAT2
    {
        return { 0.0f, 0.0f };
    }
    );
    copy(begin(frontFaceSideVertices), end(frontFaceSideVertices), back_inserter(vertices));

    // Shared by the back face and "back" side vertices.
    unsigned short backFaceVerticesStartIndex = static_cast<unsigned short>(vertices.size());
    auto backFaceVertices = MeshGeometry::MakeCylinderFaceVertices(
        tessellation,
        backFaceZ,
        [](float /*x*/, float /*y*/) -> XMFLOAT2
    {
        return { 0.0f, 0.0f };
    }
    );
    copy(begin(backFaceVertices), end(backFaceVertices), back_inserter(vertices));

    // Front face indices
    vector<unsigned short> indices = MeshGeometry::MakeCylinderFaceIndices(
        tessellation,
        /*vertexIndexOffset*/ frontFaceVerticesStartIndex,
        /*centerVertexIndex*/ frontFaceCenterVertexIndex,
        /*isFrontFace*/ true);

    // Back face indices.
    // This may be shown brifly during intro/outro animations so we need to show it.
    vector<unsigned short> backFaceIndices = MeshGeometry::MakeCylinderFaceIndices(
        tessellation,
        /*vertexIndexOffset*/ backFaceVerticesStartIndex,
        /*centerVertexIndex*/ backFaceCenterVertexIndex,
        /*isFrontFace*/ false);
    copy(begin(backFaceIndices), end(backFaceIndices), back_inserter(indices));

    // Side indices
    vector<unsigned short> sideIndices = MeshGeometry::MakeCylinderSideIndices(
        tessellation,
        /*vertexIndexOffset*/ frontFaceSideVerticesStartIndex,
        /*cylinderFaceVertexStride*/ static_cast<unsigned short>(frontFaceSideVertices.size()));
    copy(begin(sideIndices), end(sideIndices), back_inserter(indices));

    ComPtr<ID3D11Buffer> vertexBuffer = CreateVertexBuffer(
        static_cast<unsigned int>(vertices.size()),
        vertices.data()
    );

    ComPtr<ID3D11Buffer> indexBuffer = CreateIndexBuffer(
        static_cast<unsigned int>(indices.size()),
        indices.data());

    return make_shared<MeshGeometry>(
        D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        vertexBuffer,
        indexBuffer,
        static_cast<uint32>(sizeof(VertexPositionTexture)),
        static_cast<uint32>(indices.size())
        );
}

vector<VertexPositionTexture> MeshGeometry::MakeCylinderFaceVertices(unsigned short tessellation, float z, function<XMFLOAT2(float x, float y)> uvGenerator)
{
    // Create a fan of triangles from the center of the circle.
    // Reserve space for all vertices along the outer edges and the center vertex.
    vector<VertexPositionTexture> vertices;
    vertices.resize(tessellation + 1);
    for (int i = 0; i < tessellation; i++)
    {
        const float angle = XM_2PI * (static_cast<float>(i) / tessellation);

        const float x = 0.5f * cos(angle);
        const float y = 0.5f * sin(angle);

        const XMFLOAT3 pos{ x, y, z };
        const XMFLOAT2 uv = uvGenerator(x, y);

        vertices[i] = { pos, uv };
    }

    return vertices;
}

vector<unsigned short> MeshGeometry::MakeCylinderFaceIndices(unsigned short tessellation, unsigned short vertexIndexOffset, unsigned short centerVertexIndex, bool isFrontFace)
{
    vector<unsigned short> indices;
    indices.resize(tessellation * 3);

    for (unsigned short i = 0; i < tessellation; i++)
    {
        // The next 3 vertices are for the i-th triangle
        const int baseTriangleIndex = (i * 3);

        unsigned short currentVertexIndex = vertexIndexOffset + i;
        unsigned short nextVertexIndex = vertexIndexOffset + ((i + 1) % tessellation);

        if (isFrontFace)
        {
            // Clockwise ordering
            indices[baseTriangleIndex] = currentVertexIndex;
            indices[baseTriangleIndex + 1] = centerVertexIndex;
            indices[baseTriangleIndex + 2] = nextVertexIndex;
        }
        else
        {
            // Counter-clockwise ordering
            indices[baseTriangleIndex] = nextVertexIndex;
            indices[baseTriangleIndex + 1] = centerVertexIndex;
            indices[baseTriangleIndex + 2] = currentVertexIndex;
        }
    }

    return indices;
}

vector<unsigned short> MeshGeometry::MakeCylinderSideIndices(unsigned short tessellation, unsigned short vertexIndexOffset, unsigned short cylinderFaceVertexStride)
{
    vector<unsigned short> indices;
    indices.resize(tessellation * 6);

    for (unsigned short i = 0; i < tessellation; i++)
    {
        // The next 6 vertices are for the i-th quad making up the i-th side of the cylinder
        const int baseTriangleIndex = (i * 6);

        unsigned short currentVertexIndexFrontFace = vertexIndexOffset + i;
        unsigned short currentVertexIndexBackFace = vertexIndexOffset + i + cylinderFaceVertexStride;
        unsigned short nextVertexIndexFrontFace = vertexIndexOffset + ((i + 1) % tessellation);
        unsigned short nextVertexIndexBackFace = vertexIndexOffset + ((i + 1) % tessellation) + cylinderFaceVertexStride;

        indices[baseTriangleIndex] = currentVertexIndexFrontFace;
        indices[baseTriangleIndex + 1] = nextVertexIndexFrontFace;
        indices[baseTriangleIndex + 2] = currentVertexIndexBackFace;

        indices[baseTriangleIndex + 3] = nextVertexIndexFrontFace;
        indices[baseTriangleIndex + 4] = nextVertexIndexBackFace;
        indices[baseTriangleIndex + 5] = currentVertexIndexBackFace;
    }

    return indices;
}

ComPtr<ID3D11Buffer> MeshGeometry::CreateVertexBuffer(unsigned int numVertices, const VertexPositionTexture* vertexData)
{
    ComPtr<ID3D11Buffer> vertexBufferInternal;
    D3D11_BUFFER_DESC VertexBufferDesc;
    VertexBufferDesc.ByteWidth = sizeof(_360VideoPlayback::VertexPositionTexture) * numVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA VertexBufferData;
    VertexBufferData.pSysMem = vertexData;
    VertexBufferData.SysMemPitch = 0;
    VertexBufferData.SysMemSlicePitch = 0;

    DX::ThrowIfFailed(
        _360VideoPlaybackMain::GetDeviceResources()->GetD3DDevice()->CreateBuffer(
            &VertexBufferDesc,
            &VertexBufferData,
            &vertexBufferInternal));

    return vertexBufferInternal;
}

ComPtr<ID3D11Buffer> MeshGeometry::CreateIndexBuffer(unsigned int numIndices, const unsigned short* indexData)
{
    ComPtr<ID3D11Buffer> indexBufferInternal;

    D3D11_BUFFER_DESC IndexBufferDesc;
    IndexBufferDesc.ByteWidth = sizeof(unsigned short) * numIndices;
    IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;
    IndexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA IndexBufferData;
    IndexBufferData.pSysMem = indexData;
    IndexBufferData.SysMemPitch = 0;
    IndexBufferData.SysMemSlicePitch = 0;

    DX::ThrowIfFailed(
        _360VideoPlaybackMain::GetDeviceResources()->GetD3DDevice()->CreateBuffer(
            &IndexBufferDesc,
            &IndexBufferData,
            &indexBufferInternal));

    return indexBufferInternal;
}
