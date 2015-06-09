// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Add item to the index using the ContentIndexer",             "SDKTemplate.S1_AddWithAPI" }, 
    { "Update and delete indexed items using the ContentIndexer",   "SDKTemplate.S2_UpdateAndDeleteWithAPI" },
    { "Retrieve indexed items added using the ContentIndexer",      "SDKTemplate.S3_RetrieveWithAPI" },
    { "Check the index revision number",                            "SDKTemplate.S4_CheckIndexRevision" }, 
    { "Add indexed items by using appcontent-ms files",             "SDKTemplate.S5_AddWithAppContent" },
    { "Delete indexed appcontent-ms files",                         "SDKTemplate.S6_DeleteWithAppContent" },
    { "Retrieve indexed properties from appcontent-ms files",       "SDKTemplate.S7_RetrieveWithAppContent" }
};
