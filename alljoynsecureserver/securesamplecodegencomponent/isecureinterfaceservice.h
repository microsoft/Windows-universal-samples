//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

namespace org { namespace alljoyn { namespace Bus { namespace Samples { namespace Secure {

public interface class ISecureInterfaceService
{
public:
    // Implement this function to handle calls to the Cat method.
    Windows::Foundation::IAsyncOperation<SecureInterfaceCatResult^>^ CatAsync(Windows::Devices::AllJoyn::AllJoynMessageInfo^ info , _In_ Platform::String^ inStr1, _In_ Platform::String^ inStr2);

};

} } } } } 