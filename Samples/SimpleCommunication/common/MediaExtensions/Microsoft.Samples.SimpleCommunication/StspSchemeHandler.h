//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

namespace Microsoft { namespace Samples { namespace SimpleCommunication {

class CSchemeHandler
    : public Microsoft::WRL::RuntimeClass<
           Microsoft::WRL::RuntimeClassFlags< Microsoft::WRL::RuntimeClassType::WinRtClassicComMix >, 
           ABI::Windows::Media::IMediaExtension,
           IMFSchemeHandler >
{
    InspectableClass(L"Microsoft.Samples.SimpleCommunication.StspSchemeHandler",BaseTrust)

public:
    CSchemeHandler(void);
    ~CSchemeHandler(void);

    // IMediaExtension
    IFACEMETHOD (SetProperties) (ABI::Windows::Foundation::Collections::IPropertySet *pConfiguration);

    // IMFSchemeHandler
    IFACEMETHOD (BeginCreateObject) ( 
            _In_ LPCWSTR pwszURL,
            _In_ DWORD dwFlags,
            _In_ IPropertyStore *pProps,
            _COM_Outptr_opt_  IUnknown **ppIUnknownCancelCookie,
            _In_ IMFAsyncCallback *pCallback,
            _In_ IUnknown *punkState);
        
    IFACEMETHOD (EndCreateObject) ( 
            _In_ IMFAsyncResult *pResult,
            _Out_  MF_OBJECT_TYPE *pObjectType,
            _Out_  IUnknown **ppObject);
        
    IFACEMETHOD (CancelObjectCreation) ( 
            _In_ IUnknown *pIUnknownCancelCookie);
};

}}} // namespace Microsoft::Samples::SimpleCommunication
