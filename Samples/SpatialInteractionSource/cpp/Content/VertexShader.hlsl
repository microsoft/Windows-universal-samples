// Per-vertex data passed to the geometry shader.
struct VertexShaderOutput
{
    min16float4 pos     : SV_POSITION;
    float4 color   : COLOR0;

    // The render target array index will be set by the geometry shader.
    uint        viewId  : TEXCOORD0;
};

#include "VertexShaderShared.hlsl"
