//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************

#pragma once

// Material:
// This class maintains the properties that represent how an object will
// look when it is rendered.  This includes the color of the object, the
// texture used to render the object, and the vertex and pixel shader that
// should be used for rendering.
// The RenderSetup method sets the appropriate values into the constantBuffer
// and calls the appropriate D3D11 context methods to set up the rendering pipeline
// in the graphics hardware.

#include "ConstantBuffers.h"

ref class Material
{
internal:
    Material(
        DirectX::XMFLOAT4 meshColor,
        DirectX::XMFLOAT4 diffuseColor,
        DirectX::XMFLOAT4 specularColor,
        float specularExponent,
        _In_ ID3D11ShaderResourceView* textureResourceView,
        _In_ ID3D11VertexShader* vertexShader,
        _In_ ID3D11PixelShader* pixelShader
        );

    void RenderSetup(
        _In_ ID3D11DeviceContext* context,
        _Inout_ ConstantBufferChangesEveryPrim* constantBuffer
        );

    void SetTexture(_In_ ID3D11ShaderResourceView* textureResourceView)
    {
        m_textureRV = textureResourceView;
    }

protected private:
    DirectX::XMFLOAT4   m_meshColor;
    DirectX::XMFLOAT4   m_diffuseColor;
    DirectX::XMFLOAT4   m_hitColor;
    DirectX::XMFLOAT4   m_specularColor;
    float               m_specularExponent;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureRV;
};
