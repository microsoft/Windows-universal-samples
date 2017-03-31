// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    min16float4 pos   : SV_POSITION;
    min16float4 color : COLOR0;
    min16float2 uv    : TEXCOORD0;
};

Texture2D<min16float4> texturedata : register(t0);
SamplerState samp   : register(s0);

// The pixel shader passes through the color data. The color data from 
// is interpolated and assigned to a pixel at the rasterization step.
min16float4 main(PixelShaderInput input) : SV_TARGET
{
    min16float4 textureColor = texturedata.Sample(samp, input.uv);
    return min16float4(lerp(input.color.rgb, textureColor.rgb, textureColor.a), input.color.a);
}
