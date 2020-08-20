// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "BasicReaderWriter.h"

// A simple loader class that provides support for loading shaders, textures,
// and meshes from files on disk. Provides synchronous and asynchronous methods.
class BasicLoader
{
public:
    BasicLoader(
        _In_ winrt::com_ptr<ID3D11Device3> d3dDevice,
        _In_opt_ IWICImagingFactory2* wicFactory = nullptr
        );

    void LoadTexture(
        _In_ winrt::hstring const& filename,
        _Out_opt_ ID3D11Texture2D** texture,
        _Out_opt_ ID3D11ShaderResourceView** textureView
        );

    winrt::Windows::Foundation::IAsyncAction LoadTextureAsync(
        _In_ winrt::hstring filename,
        _Out_opt_ ID3D11Texture2D** texture,
        _Out_opt_ ID3D11ShaderResourceView** textureView
        );

    void LoadShader(
        _In_ winrt::hstring const& filename,
        _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC const layoutDesc[],
        _In_ uint32_t layoutDescNumElements,
        _Out_ ID3D11VertexShader** shader,
        _Out_opt_ ID3D11InputLayout** layout
        );

    winrt::Windows::Foundation::IAsyncAction LoadShaderAsync(
        _In_ winrt::hstring filename,
        _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC const layoutDesc[],
        _In_ uint32_t layoutDescNumElements,
        _Out_ ID3D11VertexShader** shader,
        _Out_opt_ ID3D11InputLayout** layout
        );

    void LoadShader(
        _In_ winrt::hstring const& filename,
        _Out_ ID3D11PixelShader** shader
        );

    winrt::Windows::Foundation::IAsyncAction LoadShaderAsync(
        _In_ winrt::hstring filename,
        _Out_ ID3D11PixelShader** shader
        );

    void LoadShader(
        _In_ winrt::hstring const& filename,
        _Out_ ID3D11ComputeShader** shader
        );

    winrt::Windows::Foundation::IAsyncAction LoadShaderAsync(
        _In_ winrt::hstring const filename,
        _Out_ ID3D11ComputeShader** shader
        );

    void LoadShader(
        _In_ winrt::hstring const& filename,
        _Out_ ID3D11GeometryShader** shader
        );

    winrt::Windows::Foundation::IAsyncAction LoadShaderAsync(
        _In_ winrt::hstring filename,
        _Out_ ID3D11GeometryShader** shader
        );

    void LoadShader(
        _In_ winrt::hstring const& filename,
        _In_reads_opt_(numEntries) const D3D11_SO_DECLARATION_ENTRY* streamOutDeclaration,
        _In_ uint32_t numEntries,
        _In_reads_opt_(numStrides) const uint32_t* bufferStrides,
        _In_ uint32_t numStrides,
        _In_ uint32_t rasterizedStream,
        _Out_ ID3D11GeometryShader** shader
        );

    winrt::Windows::Foundation::IAsyncAction LoadShaderAsync(
        _In_ winrt::hstring filename,
        _In_reads_opt_(numEntries) const D3D11_SO_DECLARATION_ENTRY* streamOutDeclaration,
        _In_ uint32_t numEntries,
        _In_reads_opt_(numStrides) const uint32_t* bufferStrides,
        _In_ uint32_t numStrides,
        _In_ uint32_t rasterizedStream,
        _Out_ ID3D11GeometryShader** shader
        );

    void LoadShader(
        _In_ winrt::hstring const& filename,
        _Out_ ID3D11HullShader** shader
        );

    winrt::Windows::Foundation::IAsyncAction LoadShaderAsync(
        _In_ winrt::hstring filename,
        _Out_ ID3D11HullShader** shader
        );

    void LoadShader(
        _In_ winrt::hstring const& filename,
        _Out_ ID3D11DomainShader** shader
        );

    winrt::Windows::Foundation::IAsyncAction LoadShaderAsync(
        _In_ winrt::hstring filename,
        _Out_ ID3D11DomainShader** shader
        );

    void LoadMesh(
        _In_ winrt::hstring const& filename,
        _Out_ ID3D11Buffer** vertexBuffer,
        _Out_ ID3D11Buffer** indexBuffer,
        _Out_opt_ uint32_t* vertexCount,
        _Out_opt_ uint32_t* indexCount
        );

    winrt::Windows::Foundation::IAsyncAction LoadMeshAsync(
        _In_ winrt::hstring filename,
        _Out_ ID3D11Buffer** vertexBuffer,
        _Out_ ID3D11Buffer** indexBuffer,
        _Out_opt_ uint32_t* vertexCount,
        _Out_opt_ uint32_t* indexCount
        );

private:
    winrt::com_ptr<ID3D11Device> m_d3dDevice;
    winrt::com_ptr<IWICImagingFactory2> m_wicFactory;
    BasicReaderWriter m_basicReaderWriter;

    template <class DeviceChildType>
    inline void SetDebugName(
        _In_ DeviceChildType* object,
        _In_ winrt::hstring const& name
        );

    std::wstring GetExtension(
        _In_ winrt::hstring const& filename
        );

    void CreateTexture(
        _In_ bool decodeAsDDS,
        _In_reads_bytes_(dataSize) const byte* data,
        _In_ uint32_t dataSize,
        _Out_opt_ ID3D11Texture2D** texture,
        _Out_opt_ ID3D11ShaderResourceView** textureView,
        _In_opt_ winrt::hstring const& debugName
        );

    void CreateInputLayout(
        _In_reads_bytes_(bytecodeSize) const byte* bytecode,
        _In_ uint32_t bytecodeSize,
        _In_reads_opt_(layoutDescNumElements) const D3D11_INPUT_ELEMENT_DESC* layoutDesc,
        _In_ uint32_t layoutDescNumElements,
        _Out_ ID3D11InputLayout** layout
        );

    void CreateMesh(
        _In_ byte* meshData,
        _Out_ ID3D11Buffer** vertexBuffer,
        _Out_ ID3D11Buffer** indexBuffer,
        _Out_opt_ uint32_t* vertexCount,
        _Out_opt_ uint32_t* indexCount,
        _In_opt_ winrt::hstring const& debugName
        );
};
