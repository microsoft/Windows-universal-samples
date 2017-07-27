cbuffer FocusPointConstantBuffer : register(b1)
{
    min16float4 focusPointOrigin;
    min16float4 focusPointDirection;
    min16float4 focusPointColor;
    min16float4 focusPointRadius; // Using XMFLOAT4 for byte alignment
    min16float4 focusPointIntensity; // Using XMFLOAT4 for byte alignment
};

Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    min16float4 pos     : SV_POSITION;
    min16float2 tex     : TEXCOORD1;
    float4      worldCoord : TEXCOORD2;
};

// Apply the glass shader color and look to anything marked
// as glass.
min16float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 textureColor = Texture.Sample(Sampler, input.tex);

    // Apply light
    float fallOffDistance = focusPointRadius.x;
    float3 worldPos = float3(input.worldCoord.x, input.worldCoord.y, input.worldCoord.z);
    float3 normalizedFocusDirectionVector = normalize(float3(focusPointDirection.x, focusPointDirection.y, focusPointDirection.z));
    float3 normalizedFocusToPointVect = normalize(worldPos - float3(focusPointOrigin.x, focusPointOrigin.y, focusPointOrigin.z));
    float dotProduct = dot(normalizedFocusDirectionVector, normalizedFocusToPointVect);
    float focusDirectionToPointAngle = acos(dotProduct);
    float sinAngle = sin(focusDirectionToPointAngle);
    float focusToPointVectLength = (length(worldPos - float3(focusPointOrigin.x, focusPointOrigin.y, focusPointOrigin.z)));
    float distance = sinAngle * focusToPointVectLength;
    float focusIntensity = focusPointIntensity.x * (1.0f - saturate(distance / fallOffDistance));

    float4 outputColor = lerp(textureColor, focusPointColor, focusIntensity);
    return (min16float4)outputColor;
}