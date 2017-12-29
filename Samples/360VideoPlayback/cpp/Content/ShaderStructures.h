#pragma once

namespace _360VideoPlayback
{
    // Constant buffer used to send hologram position transform to the shader pipeline.
    struct ModelConstantBuffer
    {
        DirectX::XMFLOAT4X4 model;
    };

    // Assert that the constant buffer remains 16-byte aligned (best practice).
    static_assert((sizeof(ModelConstantBuffer) % (sizeof(float) * 4)) == 0, "Model constant buffer size must be 16-byte aligned (16 bytes is the length of four floats).");


    // Used to send per-vertex data to the vertex shader.
    struct VertexPositionTexture
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 textureCoordinate;
    };

    struct FocusPointConstantBuffer
    {
        DirectX::XMFLOAT4 focusPointOrigin;
        DirectX::XMFLOAT4 focusPointDirection;
        DirectX::XMFLOAT4 focusPointColor;
        DirectX::XMFLOAT4 focusPointRadius; // Using XMFLOAT4 for byte alignment
        DirectX::XMFLOAT4 focusPointIntensity; // Using XMFLOAT4 for byte alignment
    };
    static_assert((sizeof(FocusPointConstantBuffer) % (sizeof(float) * 4)) == 0, "FocusPointConstantBuffer size must be 16-byte aligned (16 bytes is the length of four floats).");
}