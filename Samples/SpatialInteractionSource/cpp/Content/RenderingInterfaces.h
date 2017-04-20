#pragma once

#include "Common\DeviceResources.h"
#include "ShaderStructures.h"

namespace HolographicPaint
{
    interface IRenderer
    {
        virtual void SetConstantBuffer(ModelConstantBuffer* pModelConstantBuffer) = 0;
        virtual void DrawCube() = 0;
    };

    interface IBaseRenderedObject
    {
        virtual void Update(ID3D11DeviceContext* pDeviceContext) = 0;
        virtual void Render(ID3D11DeviceContext* pDeviceContext, IRenderer* pRenderingHelper) = 0;
    };

}