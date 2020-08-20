// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "BasicLoader.h"
#include "BasicShapes.h"
#include "DDSTextureLoader.h"
#include <cwctype>

BasicLoader::BasicLoader(
    _In_ winrt::com_ptr<ID3D11Device3> d3dDevice,
    _In_opt_ IWICImagingFactory2* wicFactory
    ) :
    m_d3dDevice(std::move(d3dDevice))
{
    m_wicFactory.copy_from(wicFactory);
}

template <class DeviceChildType>
inline void BasicLoader::SetDebugName(
    _In_ DeviceChildType* object,
    _In_ winrt::hstring const& name
    )
{
#if defined(_DEBUG)
    // Only assign debug names in debug builds.

    char nameString[1024];
    int nameStringLength = WideCharToMultiByte(
        CP_ACP,
        0,
        name.c_str(),
        -1,
        nameString,
        1024,
        nullptr,
        nullptr
        );

    if (nameStringLength == 0)
    {
        char defaultNameString[] = "BasicLoaderObject";
        winrt::check_hresult(
            object->SetPrivateData(
                WKPDID_D3DDebugObjectName,
                sizeof(defaultNameString) - 1,
                defaultNameString
                )
            );
    }
    else
    {
        winrt::check_hresult(
            object->SetPrivateData(
                WKPDID_D3DDebugObjectName,
                nameStringLength - 1,
                nameString
                )
            );
    }
#endif
}

std::wstring BasicLoader::GetExtension(
    _In_ winrt::hstring const& filename
    )
{
    std::wstring extension;
    auto lastDot = std::find(filename.rbegin(), filename.rend(), L'.');
    if (lastDot != filename.rend())
    {
        std::transform(lastDot.base(), filename.end(), std::back_inserter(extension),
            [](wchar_t c) { return std::towlower(c); });
    }
    return extension;
}

void BasicLoader::CreateTexture(
    _In_ bool decodeAsDDS,
    _In_reads_bytes_(dataSize) const byte* data,
    _In_ uint32_t dataSize,
    _Out_opt_ ID3D11Texture2D** texture,
    _Out_opt_ ID3D11ShaderResourceView** textureView,
    _In_opt_ winrt::hstring const& debugName
    )
{
    winrt::com_ptr<ID3D11ShaderResourceView> shaderResourceView;
    winrt::com_ptr<ID3D11Texture2D> texture2D;

    if (decodeAsDDS)
    {
        winrt::com_ptr<ID3D11Resource> resource;

        if (textureView == nullptr)
        {
            CreateDDSTextureFromMemory(
                m_d3dDevice.get(),
                data,
                dataSize,
                resource.put(),
                nullptr
                );
        }
        else
        {
            CreateDDSTextureFromMemory(
                m_d3dDevice.get(),
                data,
                dataSize,
                resource.put(),
                shaderResourceView.put()
                );
        }

        texture2D = resource.as<ID3D11Texture2D>();
    }
    else
    {
        if (m_wicFactory.get() == nullptr)
        {
            // A WIC factory object is required in order to load texture
            // assets stored in non-DDS formats. If BasicLoader was not
            // initialized with one, create one as needed.
            winrt::check_hresult(
                CoCreateInstance(
                    CLSID_WICImagingFactory,
                    nullptr,
                    CLSCTX_INPROC_SERVER,
                    IID_PPV_ARGS(&m_wicFactory)
                    )
                );
        }

        winrt::com_ptr<IWICStream> stream;
        winrt::check_hresult(
            m_wicFactory->CreateStream(stream.put())
            );

        winrt::check_hresult(
            stream->InitializeFromMemory(
                const_cast<WICInProcPointer>(data),
                dataSize
                )
            );

        winrt::com_ptr<IWICBitmapDecoder> bitmapDecoder;
        winrt::check_hresult(
            m_wicFactory->CreateDecoderFromStream(
                stream.get(),
                nullptr,
                WICDecodeMetadataCacheOnDemand,
                bitmapDecoder.put()
                )
            );

        winrt::com_ptr<IWICBitmapFrameDecode> bitmapFrame;
        winrt::check_hresult(
            bitmapDecoder->GetFrame(0, bitmapFrame.put())
            );

        winrt::com_ptr<IWICFormatConverter> formatConverter;
        winrt::check_hresult(
            m_wicFactory->CreateFormatConverter(formatConverter.put())
            );

        winrt::check_hresult(
            formatConverter->Initialize(
                bitmapFrame.get(),
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                nullptr,
                0.0,
                WICBitmapPaletteTypeCustom
                )
            );

        uint32_t width;
        uint32_t height;
        winrt::check_hresult(
            bitmapFrame->GetSize(&width, &height)
            );

        std::unique_ptr<byte[]> bitmapPixels(new byte[width * height * 4]);
        winrt::check_hresult(
            formatConverter->CopyPixels(
                nullptr,
                width * 4,
                width * height * 4,
                bitmapPixels.get()
                )
            );

        D3D11_SUBRESOURCE_DATA initialData;
        ZeroMemory(&initialData, sizeof(initialData));
        initialData.pSysMem = bitmapPixels.get();
        initialData.SysMemPitch = width * 4;
        initialData.SysMemSlicePitch = 0;

        CD3D11_TEXTURE2D_DESC textureDesc(
            DXGI_FORMAT_B8G8R8A8_UNORM,
            width,
            height,
            1,
            1
            );

        winrt::check_hresult(
            m_d3dDevice->CreateTexture2D(
                &textureDesc,
                &initialData,
                texture2D.put()
                )
            );

        if (textureView != nullptr)
        {
            CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(
                texture2D.get(),
                D3D11_SRV_DIMENSION_TEXTURE2D
                );

            winrt::check_hresult(
                m_d3dDevice->CreateShaderResourceView(
                    texture2D.get(),
                    &shaderResourceViewDesc,
                    shaderResourceView.put()
                    )
                );
        }
    }

    SetDebugName(texture2D.get(), debugName);

    if (texture != nullptr)
    {
        *texture = texture2D.detach();
    }
    if (textureView != nullptr)
    {
        *textureView = shaderResourceView.detach();
    }
}

void BasicLoader::CreateInputLayout(
    _In_reads_bytes_(bytecodeSize) const byte* bytecode,
    _In_ uint32_t bytecodeSize,
    _In_reads_opt_(layoutDescNumElements) const D3D11_INPUT_ELEMENT_DESC* layoutDesc,
    _In_ uint32_t layoutDescNumElements,
    _Out_ ID3D11InputLayout** layout
    )
{
    if (layoutDesc == nullptr)
    {
        // If no input layout is specified, use the BasicVertex layout.
        static constexpr D3D11_INPUT_ELEMENT_DESC basicVertexLayoutDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        winrt::check_hresult(
            m_d3dDevice->CreateInputLayout(
                basicVertexLayoutDesc,
                ARRAYSIZE(basicVertexLayoutDesc),
                bytecode,
                bytecodeSize,
                layout
                )
            );
    }
    else
    {
        winrt::check_hresult(
            m_d3dDevice->CreateInputLayout(
                layoutDesc,
                layoutDescNumElements,
                bytecode,
                bytecodeSize,
                layout
                )
            );
    }
}

void BasicLoader::CreateMesh(
    _In_ byte* meshData,
    _Out_ ID3D11Buffer** vertexBuffer,
    _Out_ ID3D11Buffer** indexBuffer,
    _Out_opt_ uint32_t* vertexCount,
    _Out_opt_ uint32_t* indexCount,
    _In_opt_ winrt::hstring const& debugName
    )
{
    // The first 4 bytes of the BasicMesh format define the number of vertices in the mesh.
    uint32_t numVertices = *reinterpret_cast<uint32_t*>(meshData);

    // The following 4 bytes define the number of indices in the mesh.
    uint32_t numIndices = *reinterpret_cast<uint32_t*>(meshData + sizeof(uint32_t));

    // The next segment of the BasicMesh format contains the vertices of the mesh.
    BasicVertex* vertices = reinterpret_cast<BasicVertex*>(meshData + sizeof(uint32_t) * 2);

    // The last segment of the BasicMesh format contains the indices of the mesh.
    uint16_t* indices = reinterpret_cast<uint16_t*>(meshData + sizeof(uint32_t) * 2 + sizeof(BasicVertex) * numVertices);

    // Create the vertex and index buffers with the mesh data.

    D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
    vertexBufferData.pSysMem = vertices;
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;
    CD3D11_BUFFER_DESC vertexBufferDesc(numVertices * sizeof(BasicVertex), D3D11_BIND_VERTEX_BUFFER);
    winrt::check_hresult(
        m_d3dDevice->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferData,
            vertexBuffer
            )
        );

    D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
    indexBufferData.pSysMem = indices;
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;
    CD3D11_BUFFER_DESC indexBufferDesc(numIndices * sizeof(uint16_t), D3D11_BIND_INDEX_BUFFER);
    winrt::check_hresult(
        m_d3dDevice->CreateBuffer(
            &indexBufferDesc,
            &indexBufferData,
            indexBuffer
            )
        );

    SetDebugName(*vertexBuffer, debugName + L"_VertexBuffer");
    SetDebugName(*indexBuffer, debugName + L"_IndexBuffer");

    if (vertexCount != nullptr)
    {
        *vertexCount = numVertices;
    }
    if (indexCount != nullptr)
    {
        *indexCount = numIndices;
    }
}

void BasicLoader::LoadTexture(
    _In_ winrt::hstring const& filename,
    _Out_opt_ ID3D11Texture2D** texture,
    _Out_opt_ ID3D11ShaderResourceView** textureView
    )
{
    auto textureData{ m_basicReaderWriter.ReadData(filename) };

    CreateTexture(
        GetExtension(filename) == L"dds",
        textureData.data(),
        textureData.size(),
        texture,
        textureView,
        filename
        );
}

winrt::Windows::Foundation::IAsyncAction BasicLoader::LoadTextureAsync(
    _In_ winrt::hstring filename,
    _Out_opt_ ID3D11Texture2D** texture,
    _Out_opt_ ID3D11ShaderResourceView** textureView
    )
{
    auto textureData = co_await m_basicReaderWriter.ReadDataAsync(filename);
    CreateTexture(
        GetExtension(filename) == L"dds",
        textureData.data(),
        textureData.Length(),
        texture,
        textureView,
        filename
        );
}

void BasicLoader::LoadShader(
    _In_ winrt::hstring const& filename,
    _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC const layoutDesc[],
    _In_ uint32_t layoutDescNumElements,
    _Out_ ID3D11VertexShader** shader,
    _Out_opt_ ID3D11InputLayout** layout
    )
{
    std::vector<byte> bytecode{ m_basicReaderWriter.ReadData(filename) };

    winrt::check_hresult(
        m_d3dDevice->CreateVertexShader(
            bytecode.data(),
            bytecode.size(),
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);

    if (layout != nullptr)
    {
        CreateInputLayout(
            bytecode.data(),
            bytecode.size(),
            layoutDesc,
            layoutDescNumElements,
            layout
            );

        SetDebugName(*layout, filename);
    }
}

winrt::Windows::Foundation::IAsyncAction BasicLoader::LoadShaderAsync(
    _In_ winrt::hstring filename,
    _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC const layoutDesc[],
    _In_ uint32_t layoutDescNumElements,
    _Out_ ID3D11VertexShader** shader,
    _Out_opt_ ID3D11InputLayout** layout
    )
{
    // This method assumes that the lifetime of input arguments may be shorter
    // than the duration of this operation. In order to ensure accurate results, a
    // copy of all arguments passed by pointer must be made. The method then
    // ensures that the lifetime of the copied data exceeds that of the coroutine.

    // Create copies of the layoutDesc array as well as the SemanticName strings,
    // both of which are pointers to data whose lifetimes may be shorter than that
    // of this method's coroutine.
    std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDescCopy;
    std::vector<std::string> layoutDescSemanticNamesCopy;
    if (layoutDesc != nullptr)
    {
        layoutDescCopy = { layoutDesc, layoutDesc + layoutDescNumElements };
        layoutDescSemanticNamesCopy.reserve(layoutDescNumElements);

        for (auto&& desc : layoutDescCopy)
        {
            desc.SemanticName = layoutDescSemanticNamesCopy.emplace(layoutDescSemanticNamesCopy.end(), desc.SemanticName)->c_str();
        }
    }

    auto bytecode = co_await m_basicReaderWriter.ReadDataAsync(filename);

    winrt::check_hresult(
        m_d3dDevice->CreateVertexShader(
            bytecode.data(),
            bytecode.Length(),
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);

    if (layout != nullptr)
    {
        CreateInputLayout(
            bytecode.data(),
            bytecode.Length(),
            layoutDesc == nullptr ? nullptr : layoutDescCopy.data(),
            layoutDescNumElements,
            layout
            );

        SetDebugName(*layout, filename);
    }
}

void BasicLoader::LoadShader(
    _In_ winrt::hstring const& filename,
    _Out_ ID3D11PixelShader** shader
    )
{
    std::vector<byte> bytecode{ m_basicReaderWriter.ReadData(filename) };

    winrt::check_hresult(
        m_d3dDevice->CreatePixelShader(
            bytecode.data(),
            bytecode.size(),
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);
}

winrt::Windows::Foundation::IAsyncAction BasicLoader::LoadShaderAsync(
    _In_ winrt::hstring filename,
    _Out_ ID3D11PixelShader** shader
    )
{
    auto bytecode = co_await m_basicReaderWriter.ReadDataAsync(filename);

    winrt::check_hresult(
        m_d3dDevice->CreatePixelShader(
            bytecode.data(),
            bytecode.Length(),
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);
}

void BasicLoader::LoadShader(
    _In_ winrt::hstring const& filename,
    _Out_ ID3D11ComputeShader** shader
    )
{
    std::vector<byte> bytecode{ m_basicReaderWriter.ReadData(filename) };

    winrt::check_hresult(
        m_d3dDevice->CreateComputeShader(
            bytecode.data(),
            bytecode.size(),
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);
}

winrt::Windows::Foundation::IAsyncAction BasicLoader::LoadShaderAsync(
    _In_ winrt::hstring filename,
    _Out_ ID3D11ComputeShader** shader
    )
{
    auto bytecode = co_await m_basicReaderWriter.ReadDataAsync(filename);

    winrt::check_hresult(
        m_d3dDevice->CreateComputeShader(
            bytecode.data(),
            bytecode.Length(),
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);
}

void BasicLoader::LoadShader(
    _In_ winrt::hstring const& filename,
    _Out_ ID3D11GeometryShader** shader
    )
{
    std::vector<byte> bytecode{ m_basicReaderWriter.ReadData(filename) };

    winrt::check_hresult(
        m_d3dDevice->CreateGeometryShader(
            bytecode.data(),
            bytecode.size(),
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);
}

winrt::Windows::Foundation::IAsyncAction BasicLoader::LoadShaderAsync(
    _In_ winrt::hstring filename,
    _Out_ ID3D11GeometryShader** shader
    )
{
    auto bytecode = co_await m_basicReaderWriter.ReadDataAsync(filename);

    winrt::check_hresult(
        m_d3dDevice->CreateGeometryShader(
            bytecode.data(),
            bytecode.Length(),
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);
}

void BasicLoader::LoadShader(
    _In_ winrt::hstring const& filename,
    _In_reads_opt_(numEntries) const D3D11_SO_DECLARATION_ENTRY* streamOutDeclaration,
    _In_ uint32_t numEntries,
    _In_reads_opt_(numStrides) const uint32_t* bufferStrides,
    _In_ uint32_t numStrides,
    _In_ uint32_t rasterizedStream,
    _Out_ ID3D11GeometryShader** shader
    )
{
    std::vector<byte> bytecode{ m_basicReaderWriter.ReadData(filename) };

    winrt::check_hresult(
        m_d3dDevice->CreateGeometryShaderWithStreamOutput(
            bytecode.data(),
            bytecode.size(),
            streamOutDeclaration,
            numEntries,
            bufferStrides,
            numStrides,
            rasterizedStream,
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);
}

winrt::Windows::Foundation::IAsyncAction BasicLoader::LoadShaderAsync(
    _In_ winrt::hstring filename,
    _In_reads_opt_(numEntries) const D3D11_SO_DECLARATION_ENTRY* streamOutDeclaration,
    _In_ uint32_t numEntries,
    _In_reads_opt_(numStrides) const uint32_t* bufferStrides,
    _In_ uint32_t numStrides,
    _In_ uint32_t rasterizedStream,
    _Out_ ID3D11GeometryShader** shader
    )
{
    // This method assumes that the lifetime of input arguments may be shorter
    // than the duration of this coroutine. In order to ensure accurate results, a
    // copy of all arguments passed by pointer must be made. The method then
    // ensures that the lifetime of the copied data exceeds that of the coroutine.

    // Create copies of the streamOutDeclaration array as well as the SemanticName
    // strings, both of which are pointers to data whose lifetimes may be shorter
    // than that of this method's coroutine.
    std::vector<D3D11_SO_DECLARATION_ENTRY> streamOutDeclarationCopy;
    std::vector<std::string> streamOutDeclarationSemanticNamesCopy;
    if (streamOutDeclaration != nullptr)
    {
        streamOutDeclarationCopy = { streamOutDeclaration, streamOutDeclaration + numEntries };
        streamOutDeclarationSemanticNamesCopy.reserve(numEntries);

        for (auto&& desc : streamOutDeclarationCopy)
        {
            desc.SemanticName = streamOutDeclarationSemanticNamesCopy.emplace(streamOutDeclarationSemanticNamesCopy.end(), desc.SemanticName)->c_str();
        }
    }

    // Create a copy of the bufferStrides array, which is a pointer to data
    // whose lifetime may be shorter than that of this method's coroutine.
    std::vector<uint32_t> bufferStridesCopy;
    if (bufferStrides != nullptr)
    {
        bufferStridesCopy = { bufferStrides, bufferStrides + numStrides };
    }

    auto bytecode = co_await m_basicReaderWriter.ReadDataAsync(filename);

    winrt::check_hresult(
        m_d3dDevice->CreateGeometryShaderWithStreamOutput(
            bytecode.data(),
            bytecode.Length(),
            streamOutDeclaration == nullptr ? nullptr : streamOutDeclarationCopy.data(),
            numEntries,
            bufferStrides == nullptr ? nullptr : bufferStridesCopy.data(),
            numStrides,
            rasterizedStream,
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);
}

void BasicLoader::LoadShader(
    _In_ winrt::hstring const& filename,
    _Out_ ID3D11HullShader** shader
    )
{
    std::vector<byte> bytecode{ m_basicReaderWriter.ReadData(filename) };

    winrt::check_hresult(
        m_d3dDevice->CreateHullShader(
            bytecode.data(),
            bytecode.size(),
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);
}

winrt::Windows::Foundation::IAsyncAction BasicLoader::LoadShaderAsync(
    _In_ winrt::hstring filename,
    _Out_ ID3D11HullShader** shader
    )
{
    auto bytecode = co_await m_basicReaderWriter.ReadDataAsync(filename);

    winrt::check_hresult(
        m_d3dDevice->CreateHullShader(
            bytecode.data(),
            bytecode.Length(),
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);
}

void BasicLoader::LoadShader(
    _In_ winrt::hstring const& filename,
    _Out_ ID3D11DomainShader** shader
    )
{
    std::vector<byte> bytecode{ m_basicReaderWriter.ReadData(filename) };

    winrt::check_hresult(
        m_d3dDevice->CreateDomainShader(
            bytecode.data(),
            bytecode.size(),
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);
}

winrt::Windows::Foundation::IAsyncAction BasicLoader::LoadShaderAsync(
    _In_ winrt::hstring filename,
    _Out_ ID3D11DomainShader** shader
    )
{
    auto bytecode = co_await m_basicReaderWriter.ReadDataAsync(filename);

    winrt::check_hresult(
        m_d3dDevice->CreateDomainShader(
            bytecode.data(),
            bytecode.Length(),
            nullptr,
            shader
            )
        );

    SetDebugName(*shader, filename);
}

void BasicLoader::LoadMesh(
    _In_ winrt::hstring const& filename,
    _Out_ ID3D11Buffer** vertexBuffer,
    _Out_ ID3D11Buffer** indexBuffer,
    _Out_opt_ uint32_t* vertexCount,
    _Out_opt_ uint32_t* indexCount
    )
{
    std::vector<byte> meshData{ m_basicReaderWriter.ReadData(filename) };

    CreateMesh(
        meshData.data(),
        vertexBuffer,
        indexBuffer,
        vertexCount,
        indexCount,
        filename
        );
}

winrt::Windows::Foundation::IAsyncAction BasicLoader::LoadMeshAsync(
    _In_ winrt::hstring filename,
    _Out_ ID3D11Buffer** vertexBuffer,
    _Out_ ID3D11Buffer** indexBuffer,
    _Out_opt_ uint32_t* vertexCount,
    _Out_opt_ uint32_t* indexCount
    )
{
    auto meshData = co_await m_basicReaderWriter.ReadDataAsync(filename);

    CreateMesh(
        meshData.data(),
        vertexBuffer,
        indexBuffer,
        vertexCount,
        indexCount,
        filename
        );
}
