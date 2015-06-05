// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Pick a single photo",   "FilePicker.Scenario1" },
    { "Pick multiple files",   "FilePicker.Scenario2" },
    { "Pick a folder",         "FilePicker.Scenario3" },
    { "Save a file",           "FilePicker.Scenario4" },
    { "Open a cached file",    "FilePicker.Scenario5" },
    { "Update a cached file",  "FilePicker.Scenario6" },
};
