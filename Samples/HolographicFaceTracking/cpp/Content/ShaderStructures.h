#pragma once

namespace HolographicFaceTracker
{
    // Constant buffers used to send hologram position transform to the shader pipeline.
    struct CubeModelConstantBuffer
    {
        Windows::Foundation::Numerics::float4x4 model;
    };

    // Assert that the constant buffer remains 16-byte aligned (best practice).
    static_assert((sizeof(CubeModelConstantBuffer) % (sizeof(float) * 4)) == 0, "Model constant buffer size must be 16-byte aligned (16 bytes is the length of four floats).");

    struct QuadModelConstantBuffer
    {
        Windows::Foundation::Numerics::float4x4 model;
        Windows::Foundation::Numerics::float2 texCoordScale;
        Windows::Foundation::Numerics::float2 texCoordOffset;
    };

    // Assert that the constant buffer remains 16-byte aligned (best practice).
    static_assert((sizeof(QuadModelConstantBuffer) % (sizeof(float) * 4)) == 0, "Model constant buffer size must be 16-byte aligned (16 bytes is the length of four floats).");

    // Used to send per-vertex data to the vertex shader.
    struct VertexPositionColor
    {
        Windows::Foundation::Numerics::float3 pos;
        Windows::Foundation::Numerics::float3 color;
    };

    struct VertexPositionTex
    {
        Windows::Foundation::Numerics::float3 pos;
        Windows::Foundation::Numerics::float2 tex;
    };
}