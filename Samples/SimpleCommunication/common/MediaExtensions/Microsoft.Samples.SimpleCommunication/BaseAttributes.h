//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

namespace Microsoft { namespace Samples { namespace Common {

    template <class TBase=IMFAttributes>
    class CBaseAttributes : public TBase
    {
    protected:
        // This version of the constructor does not initialize the 
        // attribute store. The derived class must call Initialize() in 
        // its own constructor.
        CBaseAttributes()
        {
        }

        // This version of the constructor initializes the attribute 
        // store, but the derived class must pass an HRESULT parameter 
        // to the constructor.

        CBaseAttributes(HRESULT &hr, UINT32 cInitialSize = 0)
        {
            hr = Initialize(cInitialSize);
        }

        // The next version of the constructor uses a caller-provided 
        // implementation of IMFAttributes.

        // (Sometimes you want to delegate IMFAttributes calls to some 
        // other object that implements IMFAttributes, rather than using 
        // MFCreateAttributes.)

        CBaseAttributes(HRESULT &hr, IUnknown *pUnk)
        {
            hr = Initialize(pUnk);
        }

        virtual ~CBaseAttributes()
        {
        }

        // Initializes the object by creating the standard Media Foundation attribute store.
        HRESULT Initialize(UINT32 cInitialSize = 0)
        {
            if (_spAttributes.Get() == nullptr)
            {
                return MFCreateAttributes(&_spAttributes, cInitialSize); 
            }
            else
            {
                return S_OK;
            }
        }

        // Initializes this object from a caller-provided attribute store.
        // pUnk: Pointer to an object that exposes IMFAttributes.
        HRESULT Initialize(IUnknown *pUnk)
        {
            if (_spAttributes)
            {
                _spAttributes->Release();
                _spAttributes = NULL;
            }


            return pUnk->QueryInterface(IID_PPV_ARGS(&_spAttributes));
        }

    public:

        // IMFAttributes methods

        STDMETHODIMP GetItem(REFGUID guidKey, PROPVARIANT *pValue)
        {
            assert(_spAttributes);
            return _spAttributes->GetItem(guidKey, pValue);
        }

        STDMETHODIMP GetItemType(REFGUID guidKey, MF_ATTRIBUTE_TYPE *pType)
        {
            assert(_spAttributes);
            return _spAttributes->GetItemType(guidKey, pType);
        }

        STDMETHODIMP CompareItem(REFGUID guidKey, REFPROPVARIANT Value, BOOL *pbResult)
        {
            assert(_spAttributes);
            return _spAttributes->CompareItem(guidKey, Value, pbResult);
        }

        STDMETHODIMP Compare(
            IMFAttributes *pTheirs, 
            MF_ATTRIBUTES_MATCH_TYPE MatchType, 
            BOOL *pbResult
            )
        {
            assert(_spAttributes);
            return _spAttributes->Compare(pTheirs, MatchType, pbResult);
        }

        STDMETHODIMP GetUINT32(REFGUID guidKey, UINT32 *punValue)
        {
            assert(_spAttributes);
            return _spAttributes->GetUINT32(guidKey, punValue);
        }

        STDMETHODIMP GetUINT64(REFGUID guidKey, UINT64 *punValue)
        {
            assert(_spAttributes);
            return _spAttributes->GetUINT64(guidKey, punValue);
        }

        STDMETHODIMP GetDouble(REFGUID guidKey, double *pfValue)
        {
            assert(_spAttributes);
            return _spAttributes->GetDouble(guidKey, pfValue);
        }

        STDMETHODIMP GetGUID(REFGUID guidKey, GUID *pguidValue)
        {
            assert(_spAttributes);
            return _spAttributes->GetGUID(guidKey, pguidValue);
        }

        STDMETHODIMP GetStringLength(REFGUID guidKey, UINT32 *pcchLength)
        {
            assert(_spAttributes);
            return _spAttributes->GetStringLength(guidKey, pcchLength);
        }

        STDMETHODIMP GetString(REFGUID guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32 *pcchLength)
        {
            assert(_spAttributes);
            return _spAttributes->GetString(guidKey, pwszValue, cchBufSize, pcchLength);
        }

        STDMETHODIMP GetAllocatedString(REFGUID guidKey, LPWSTR *ppwszValue, UINT32 *pcchLength)
        {
            assert(_spAttributes);
            return _spAttributes->GetAllocatedString(guidKey, ppwszValue, pcchLength);
        }

        STDMETHODIMP GetBlobSize(REFGUID guidKey, UINT32 *pcbBlobSize)
        {
            assert(_spAttributes);
            return _spAttributes->GetBlobSize(guidKey, pcbBlobSize);
        }

        STDMETHODIMP GetBlob(REFGUID guidKey, UINT8 *pBuf, UINT32 cbBufSize, UINT32 *pcbBlobSize)
        {
            assert(_spAttributes);
            return _spAttributes->GetBlob(guidKey, pBuf, cbBufSize, pcbBlobSize);
        }

        STDMETHODIMP GetAllocatedBlob(REFGUID guidKey, UINT8 **ppBuf, UINT32 *pcbSize)
        {
            assert(_spAttributes);
            return _spAttributes->GetAllocatedBlob(guidKey, ppBuf, pcbSize);
        }

        STDMETHODIMP GetUnknown(REFGUID guidKey, REFIID riid, LPVOID *ppv)
        {
            assert(_spAttributes);
            return _spAttributes->GetUnknown(guidKey, riid, ppv);
        }

        STDMETHODIMP SetItem(REFGUID guidKey, REFPROPVARIANT Value)
        {
            assert(_spAttributes);
            return _spAttributes->SetItem(guidKey, Value);
        }

        STDMETHODIMP DeleteItem(REFGUID guidKey)
        {
            assert(_spAttributes);
            return _spAttributes->DeleteItem(guidKey);
        }

        STDMETHODIMP DeleteAllItems()
        {
            assert(_spAttributes);
            return _spAttributes->DeleteAllItems();
        }

        STDMETHODIMP SetUINT32(REFGUID guidKey, UINT32 unValue)
        {
            assert(_spAttributes);
            return _spAttributes->SetUINT32(guidKey, unValue);
        }

        STDMETHODIMP SetUINT64(REFGUID guidKey,UINT64 unValue)
        {
            assert(_spAttributes);
            return _spAttributes->SetUINT64(guidKey, unValue);
        }

        STDMETHODIMP SetDouble(REFGUID guidKey, double fValue)
        {
            assert(_spAttributes);
            return _spAttributes->SetDouble(guidKey, fValue);
        }

        STDMETHODIMP SetGUID(REFGUID guidKey, REFGUID guidValue)
        {
            assert(_spAttributes);
            return _spAttributes->SetGUID(guidKey, guidValue);
        }

        STDMETHODIMP SetString(REFGUID guidKey, LPCWSTR wszValue)
        {
            assert(_spAttributes);
            return _spAttributes->SetString(guidKey, wszValue);
        }

        STDMETHODIMP SetBlob(REFGUID guidKey, const UINT8 *pBuf, UINT32 cbBufSize)
        {
            assert(_spAttributes);
            return _spAttributes->SetBlob(guidKey, pBuf, cbBufSize);
        }

        STDMETHODIMP SetUnknown(REFGUID guidKey, IUnknown *pUnknown)
        {
            assert(_spAttributes);
            return _spAttributes->SetUnknown(guidKey, pUnknown);
        }

        STDMETHODIMP LockStore()
        {
            assert(_spAttributes);
            return _spAttributes->LockStore();
        }

        STDMETHODIMP UnlockStore()
        {
            assert(_spAttributes);
            return _spAttributes->UnlockStore();
        }

        STDMETHODIMP GetCount(UINT32 *pcItems)
        {
            assert(_spAttributes);
            return _spAttributes->GetCount(pcItems);
        }

        STDMETHODIMP GetItemByIndex(UINT32 unIndex, GUID *pguidKey, PROPVARIANT *pValue)
        {
            assert(_spAttributes);
            return _spAttributes->GetItemByIndex(unIndex, pguidKey, pValue);
        }

        STDMETHODIMP CopyAllItems(IMFAttributes *pDest)
        {
            assert(_spAttributes);
            return _spAttributes->CopyAllItems(pDest);
        }

        // Helper functions
    
        HRESULT SerializeToStream(DWORD dwOptions, IStream *pStm)      
            // dwOptions: Flags from MF_ATTRIBUTE_SERIALIZE_OPTIONS
        {
            assert(_spAttributes);
            return MFSerializeAttributesToStream(_spAttributes.Get(), dwOptions, pStm);
        }

        HRESULT DeserializeFromStream(DWORD dwOptions, IStream *pStm)
        {
            assert(_spAttributes);
            return MFDeserializeAttributesFromStream(_spAttributes.Get(), dwOptions, pStm);
        }

        // SerializeToBlob: Stores the attributes in a byte array. 
        // 
        // ppBuf: Receives a pointer to the byte array. 
        // pcbSize: Receives the size of the byte array.
        //
        // The caller must free the array using CoTaskMemFree.
        HRESULT SerializeToBlob(UINT8 **ppBuffer, UINT *pcbSize)
        {
            assert(_spAttributes);

            if (ppBuffer == NULL)
            {
                return E_POINTER;
            }
            if (pcbSize == NULL)
            {
                return E_POINTER;
            }

            HRESULT hr = S_OK;
            UINT32 cbSize = 0;
            BYTE *pBuffer = NULL;

            CHECK_HR(hr = MFGetAttributesAsBlobSize(_spAttributes.Get(), &cbSize));

            pBuffer = (BYTE*)CoTaskMemAlloc(cbSize);
            if (pBuffer == NULL)
            {
                CHECK_HR(hr = E_OUTOFMEMORY);
            }

            CHECK_HR(hr = MFGetAttributesAsBlob(_spAttributes.Get(), pBuffer, cbSize));

            *ppBuffer = pBuffer;
            *pcbSize = cbSize;

    done:
            if (FAILED(hr))
            {
                *ppBuffer = NULL;
                *pcbSize = 0;
                CoTaskMemFree(pBuffer);
            }
            return hr;
        }
    
        HRESULT DeserializeFromBlob(const UINT8 *pBuffer, UINT cbSize)
        {
            assert(_spAttributes);
            return MFInitAttributesFromBlob(_spAttributes.Get(), pBuffer, cbSize);
        }

        HRESULT GetRatio(REFGUID guidKey, UINT32 *pnNumerator, UINT32 *punDenominator)
        {
            assert(_spAttributes);
            return MFGetAttributeRatio(_spAttributes.Get(), guidKey, pnNumerator, punDenominator);
        }

        HRESULT SetRatio(REFGUID guidKey, UINT32 unNumerator, UINT32 unDenominator)
        {
            assert(_spAttributes);
            return MFSetAttributeRatio(_spAttributes.Get(), guidKey, unNumerator, unDenominator);
        }

        // Gets an attribute whose value represents the size of something (eg a video frame).
        HRESULT GetSize(REFGUID guidKey, UINT32 *punWidth, UINT32 *punHeight)
        {
            assert(_spAttributes);
            return MFGetAttributeSize(_spAttributes.Get(), guidKey, punWidth, punHeight);
        }

        // Sets an attribute whose value represents the size of something (eg a video frame).
        HRESULT SetSize(REFGUID guidKey, UINT32 unWidth, UINT32 unHeight)
        {
            assert(_spAttributes);
            return MFSetAttributeSize (_spAttributes.Get(), guidKey, unWidth, unHeight);
        }

    protected:
        ComPtr<IMFAttributes> _spAttributes;
    };

}}}