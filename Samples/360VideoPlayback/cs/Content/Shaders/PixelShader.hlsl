Texture2D objectTexture: t0;
SamplerState samp: s0;

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    min16float4 pos     : SV_POSITION;
    min16float2 tex     : TEXCOORD1;
};

// NOTE: This should be created in .cpp but this is just simpler for now
SamplerState basicLinearWrap
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};


// The pixel shader passes through the color data. The color data from
// is interpolated and assigned to a pixel at the rasterization step.
min16float4 main(PixelShaderInput input) : SV_TARGET
{
    min16float4 output = min16float4(objectTexture.Sample(samp, input.tex));
    return output;
}
