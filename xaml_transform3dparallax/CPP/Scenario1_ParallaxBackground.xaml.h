// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_ParallaxBackground.g.h"
#include "MainPage.xaml.h"

namespace Transform3DParallax
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_ParallaxBackground sealed
    {
    public:
        Scenario1_ParallaxBackground();
    private:
        MainPage^ rootPage;

		void NormalizeParallax(Windows::UI::Xaml::UIElement^ target);
		void RootGrid_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
	};
}
