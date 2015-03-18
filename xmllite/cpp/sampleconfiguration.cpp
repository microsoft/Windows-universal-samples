// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace XmlLite;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "XmlLite Reader", "XmlLite.Scenario1" },
    { "Write XML with XmlWriter", "XmlLite.Scenario2" },
    { "Write XML with XmlWriterLite", "XmlLite.Scenario3" }
};
