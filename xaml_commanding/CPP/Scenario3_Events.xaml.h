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

#include "Scenario3_Events.g.h"

namespace Commanding
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_Events sealed
    {
    public:
        Scenario3_Events();

	private:
		void CommandBar_Opening(Platform::Object^ sender, Platform::Object^ e);
		void CommandBar_Closing(Platform::Object^ sender, Platform::Object^ e);
		void CommandBar_Opened(Platform::Object^ sender, Platform::Object^ e);
		void CommandBar_Closed(Platform::Object^ sender, Platform::Object^ e);
	};
}
