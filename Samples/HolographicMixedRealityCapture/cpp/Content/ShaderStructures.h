#pragma once

namespace HolographicMRCSample
{
    // Constant buffer used to send hologram position transform to the shader pipeline.
    struct ModelConstantBuffer
    {
        DirectX::XMFLOAT4X4 model;
    };

    // Assert that the constant buffer remains 16-byte aligned (best practice).
    static_assert((sizeof(ModelConstantBuffer) % (sizeof(float) * 4)) == 0, "Model constant buffer size must be 16-byte aligned (16 bytes is the length of four floats).");

    // Constant buffer used to send hologram position transform to the shader pipeline.
    struct ModelColorConstantBuffer
    {
        DirectX::XMFLOAT4X4 model;
        DirectX::XMFLOAT4   color;
    };

    // Assert that the constant buffer remains 16-byte aligned (best practice).
    static_assert((sizeof(ModelColorConstantBuffer) % (sizeof(float) * 4)) == 0, "Model color constant buffer size must be 16-byte aligned (16 bytes is the length of four floats).");

    // Used to send per-vertex data to the vertex shader.
    struct VertexPosition
    {
        DirectX::XMFLOAT3 pos;
    };

    struct VertexPositionColor
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 color;
    };

    struct VertexPositionTexture
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 uv;
    };
}