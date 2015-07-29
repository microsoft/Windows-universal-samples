// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace Transform3DParallax;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Parallax Background", "Transform3DParallax.Scenario1_ParallaxBackground" },
    { "Parallax Hub Control", "Transform3DParallax.Scenario2_ParallaxHub" }
};
