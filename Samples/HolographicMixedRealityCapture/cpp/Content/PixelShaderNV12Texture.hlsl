// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    min16float4 pos   : SV_POSITION;
    min16float4 color : COLOR0;
    min16float2 uv    : TEXCOORD0;
};

Texture2D<min16float>  LumaData   : register(t0);
Texture2D<min16float2> ChromaData : register(t1);
SamplerState samp   : register(s0);

min16float3 ToRGB(min16float Luma, min16float2 Chroma)
{
    min16float OneHalf = (min16float)  0.5f;
    min16float One = (min16float)  1.0f;
    min16float Zero = (min16float)  0.0f;
    min16float RVCoeff = (min16float)  1.402f;
    min16float GUCoeff = (min16float) - 0.344f;
    min16float GVCoeff = (min16float) - 0.714f;
    min16float BUCoeff = (min16float)  1.772f;

    min16float3 RCoeffs = min16float3(One, Zero, RVCoeff);
    min16float3 GCoeffs = min16float3(One, GUCoeff, GVCoeff);
    min16float3 BCoeffs = min16float3(One, BUCoeff, Zero);

    min16float3 Yuv = min16float3(Luma, Chroma - OneHalf);
    min16float R = dot(Yuv, RCoeffs);
    min16float G = dot(Yuv, GCoeffs);
    min16float B = dot(Yuv, BCoeffs);

    return min16float3(R, G, B);
}

// The pixel shader passes through the color data. The color data from 
// is interpolated and assigned to a pixel at the rasterization step.
min16float4 main(PixelShaderInput input) : SV_TARGET
{
    min16float  textureLuma = LumaData.Sample(samp, input.uv);
    min16float2 textureChroma = ChromaData.Sample(samp, input.uv);

    return min16float4(ToRGB(textureLuma, textureChroma), 1.0f);
}
