#pragma once

#include <ppltasks.h>    // For create_task

namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch Win32 API errors.
            throw Platform::Exception::CreateException(hr);
        }
    }

    // Function that reads from a binary file asynchronously.
    inline Concurrency::task<std::vector<byte>> ReadDataAsync(const std::wstring& filename)
    {
        using namespace Windows::Storage;
        using namespace Concurrency;

        return create_task(PathIO::ReadBufferAsync(Platform::StringReference(filename.c_str()))).then(
            [] (Streams::IBuffer^ fileBuffer) -> std::vector<byte>
            {
                std::vector<byte> returnBuffer;
                returnBuffer.resize(fileBuffer->Length);
                Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(Platform::ArrayReference<byte>(returnBuffer.data(), static_cast<unsigned int>(returnBuffer.size())));
                return returnBuffer;
            });
    }

    // Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
    inline float ConvertDipsToPixels(float dips, float dpi)
    {
        constexpr float dipsPerInch = 96.0f;
        return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
    }

#if defined(_DEBUG)
    // Check for SDK Layer support.
    inline bool SdkLayersAvailable()
    {
        HRESULT hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
            0,
            D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
            nullptr,                    // Any feature level will do.
            0,
            D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
            nullptr,                    // No need to keep the D3D device reference.
            nullptr,                    // No need to know the feature level.
            nullptr                     // No need to keep the D3D device context reference.
            );

        return SUCCEEDED(hr);
    }
#endif


    inline DirectX::XMFLOAT4 GetDXColor(Windows::UI::Color color)
    {
        return DirectX::XMFLOAT4(
            static_cast<float>(color.R) / 255.0f,
            static_cast<float>(color.G) / 255.0f,
            static_cast<float>(color.B) / 255.0f,
            static_cast<float>(color.A) / 255.0f);
    }

    inline DXGI_FORMAT GetDxgiFormat()
    {
        return sizeof(VertexIndex_t) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    }

    inline DirectX::XMVECTOR GetQuaternionVectorFromRay(Windows::Foundation::Numerics::float3 forward, Windows::Foundation::Numerics::float3 up)
    {
        Windows::Foundation::Numerics::float3 right;
        DirectX::XMStoreFloat3(&right, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&forward), DirectX::XMLoadFloat3(&up)));
        // Create the matrix switching from current coordinatesystem to right, up, backwards
        const DirectX::XMMATRIX orientationMatrix = DirectX::XMMatrixSet(
            right.x, right.y, right.z, 0.0f,
            up.x, up.y, up.z, 0.0f,
            -forward.x, -forward.y, -forward.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        // And return the associated quaternion
        return DirectX::XMQuaternionRotationMatrix(orientationMatrix);
    }
}
