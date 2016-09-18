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

namespace WindowsPreview {
    namespace Media {
        namespace Capture {
            namespace Frames {

                // Target API will return a IBuffer providing this partial view of a larger IBuffer
                // And this class will disappear
                public ref class BufferView sealed
                {
                public:
                    property Windows::Storage::Streams::IBuffer^ Buffer
                    {
                        Windows::Storage::Streams::IBuffer^ get() { return _buffer; }
                    }
                    property uint32_t Offset
                    {
                        uint32_t get() { return _offset; }
                    }
                    property uint32_t Size
                    {
                        uint32_t get() { return _size; }
                    }
                private:
                    Windows::Storage::Streams::IBuffer^ _buffer;
                    uint32_t _offset;
                    uint32_t _size;
                internal:
                    BufferView(Windows::Storage::Streams::IBuffer^ buffer, uint32_t offset, uint32_t size)
                        : _buffer(buffer), _offset(offset), _size(size)
                    {}
                };

                class BufferHelper
                {
                public:
                    BufferHelper(Windows::Storage::Streams::IBuffer^ buffer)
                        : _buffer(buffer), _size(buffer->Capacity)
                    {
                        HRESULT hr = (reinterpret_cast<IInspectable*>(_buffer))->QueryInterface(_bufferByteAccess.ReleaseAndGetAddressOf());
                        if (FAILED(hr))
                        {
                            throw ref new Platform::Exception(hr);
                        }

                        hr = _bufferByteAccess->Buffer(&_bytes);
                        if (FAILED(hr))
                        {
                            throw ref new Platform::Exception(hr);
                        }
                    }
                    BufferHelper(BufferView^ bufferView)
                        : _buffer(bufferView->Buffer), _size(bufferView->Size)
                    {
                        HRESULT hr = (reinterpret_cast<IInspectable*>(_buffer))->QueryInterface(_bufferByteAccess.ReleaseAndGetAddressOf());
                        if (FAILED(hr))
                        {
                            throw ref new Platform::Exception(hr);
                        }

                        hr = _bufferByteAccess->Buffer(&_bytes);
                        if (FAILED(hr))
                        {
                            throw ref new Platform::Exception(hr);
                        }
                        _bytes += bufferView->Offset;
                    }

                    template<typename T>
                    const T* GetPtrAt(uint32_t byteOffset) const
                    {
                        if (byteOffset + sizeof(T) > _size)
                        {
                            throw ref new OutOfBoundsException();
                        }
                        return reinterpret_cast<const T*>(_bytes + byteOffset);
                    }

                    template<typename parentType, typename repeatedType, uint32_t declaredCount = 1>
                    void CheckBufferSize(const parentType* ptr, uint32_t effectiveCount)
                    {
                        auto offset = reinterpret_cast<const BYTE*>(ptr) - _bytes;
                        auto effectiveSize = sizeof_composedStructure<parentType, repeatedType, declaredCount>(effectiveCount);
                        if (offset + effectiveSize > _size)
                        {
                            throw ref new OutOfBoundsException();
                        }
                    }
                private:
                    BYTE* _bytes = nullptr;
                    uint32_t _offset = 0;
                    uint32_t _size = 0;
                    Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> _bufferByteAccess;
                    Windows::Storage::Streams::IBuffer^ _buffer;
                };

            }
        }
    }
}