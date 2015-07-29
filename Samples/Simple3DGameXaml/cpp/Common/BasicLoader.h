//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "BasicReaderWriter.h"

// A simple loader class that provides support for loading shaders, textures,
// and meshes from files on disk. Provides synchronous and asynchronous methods.
ref class BasicLoader
{
internal:
    BasicLoader(
        _In_ ID3D11Device* d3dDevice,
        _In_opt_ IWICImagingFactory2* wicFactory = nullptr
        );

    void LoadTexture(
        _In_ Platform::String^ filename,
        _Out_opt_ ID3D11Texture2D** texture,
        _Out_opt_ ID3D11ShaderResourceView** textureView
        );

    concurrency::task<void> LoadTextureAsync(
        _In_ Platform::String^ filename,
        _Out_opt_ ID3D11Texture2D** texture,
        _Out_opt_ ID3D11ShaderResourceView** textureView
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC layoutDesc[],
        _In_ uint32 layoutDescNumElements,
        _Out_ ID3D11VertexShader** shader,
        _Out_opt_ ID3D11InputLayout** layout
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC layoutDesc[],
        _In_ uint32 layoutDescNumElements,
        _Out_ ID3D11VertexShader** shader,
        _Out_opt_ ID3D11InputLayout** layout
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _Out_ ID3D11PixelShader** shader
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _Out_ ID3D11PixelShader** shader
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _Out_ ID3D11ComputeShader** shader
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _Out_ ID3D11ComputeShader** shader
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _Out_ ID3D11GeometryShader** shader
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _Out_ ID3D11GeometryShader** shader
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _In_reads_opt_(numEntries) const D3D11_SO_DECLARATION_ENTRY* streamOutDeclaration,
        _In_ uint32 numEntries,
        _In_reads_opt_(numStrides) const uint32* bufferStrides,
        _In_ uint32 numStrides,
        _In_ uint32 rasterizedStream,
        _Out_ ID3D11GeometryShader** shader
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _In_reads_opt_(numEntries) const D3D11_SO_DECLARATION_ENTRY* streamOutDeclaration,
        _In_ uint32 numEntries,
        _In_reads_opt_(numStrides) const uint32* bufferStrides,
        _In_ uint32 numStrides,
        _In_ uint32 rasterizedStream,
        _Out_ ID3D11GeometryShader** shader
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _Out_ ID3D11HullShader** shader
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _Out_ ID3D11HullShader** shader
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _Out_ ID3D11DomainShader** shader
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _Out_ ID3D11DomainShader** shader
        );

    void LoadMesh(
        _In_ Platform::String^ filename,
        _Out_ ID3D11Buffer** vertexBuffer,
        _Out_ ID3D11Buffer** indexBuffer,
        _Out_opt_ uint32* vertexCount,
        _Out_opt_ uint32* indexCount
        );

    concurrency::task<void> LoadMeshAsync(
        _In_ Platform::String^ filename,
        _Out_ ID3D11Buffer** vertexBuffer,
        _Out_ ID3D11Buffer** indexBuffer,
        _Out_opt_ uint32* vertexCount,
        _Out_opt_ uint32* indexCount
        );

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
    Microsoft::WRL::ComPtr<IWICImagingFactory2> m_wicFactory;
    BasicReaderWriter^ m_basicReaderWriter;

    template <class DeviceChildType>
    inline void SetDebugName(
        _In_ DeviceChildType* object,
        _In_ Platform::String^ name
        );

    Platform::String^ GetExtension(
        _In_ Platform::String^ filename
        );

    void CreateTexture(
        _In_ bool decodeAsDDS,
        _In_reads_bytes_(dataSize) byte* data,
        _In_ uint32 dataSize,
        _Out_opt_ ID3D11Texture2D** texture,
        _Out_opt_ ID3D11ShaderResourceView** textureView,
        _In_opt_ Platform::String^ debugName
        );

    void CreateInputLayout(
        _In_reads_bytes_(bytecodeSize) byte* bytecode,
        _In_ uint32 bytecodeSize,
        _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC* layoutDesc,
        _In_ uint32 layoutDescNumElements,
        _Out_ ID3D11InputLayout** layout
        );

    void CreateMesh(
        _In_ byte* meshData,
        _Out_ ID3D11Buffer** vertexBuffer,
        _Out_ ID3D11Buffer** indexBuffer,
        _Out_opt_ uint32* vertexCount,
        _Out_opt_ uint32* indexCount,
        _In_opt_ Platform::String^ debugName
        );
};
