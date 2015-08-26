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

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation::Collections;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "User preferences", "SDKTemplate.Scenario1_Prefs" },
    { "Language characteristics", "SDKTemplate.Scenario2_Lang" },
    { "Region characteristics", "SDKTemplate.Scenario3_Region" },
    { "Current input language", "SDKTemplate.Scenario4_Input" },
};

String^ SDKTemplate::StringJoin(String^ delimiter, IVectorView<String^>^ vector)
{
    String^ result = "";

    if (vector->Size > 0)
    {
        result = vector->GetAt(0);
        for (unsigned int index = 1; index < vector->Size; index++)
        {
            result += delimiter + vector->GetAt(index);
        }
    }
    return result;
}