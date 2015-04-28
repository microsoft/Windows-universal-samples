// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_ParallaxHub.xaml.h"

using namespace Transform3DParallax;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_ParallaxHub::Scenario2_ParallaxHub()
{
    InitializeComponent();
}

// See https://msdn.microsoft.com/en-us/library/hh920267.aspx
DependencyProperty^ Scenario2_ParallaxHub::s_desiredHubSectionWidthProperty = nullptr;

void Scenario2_ParallaxHub::RegisterDependencyProperties()
{
	s_desiredHubSectionWidthProperty =
		DependencyProperty::Register(
			L"DesiredHubSectionWidth",
			TypeName(double::typeid),
			TypeName(Scenario2_ParallaxHub::typeid),
			ref new PropertyMetadata(560.0)
			);
}

void Scenario2_ParallaxHub::ParallaxBackgroundHub_SizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
	// For adaptability, we want to keep hub sections within one screen width.
	DesiredHubSectionWidth = e->NewSize.Width * .9;
}
