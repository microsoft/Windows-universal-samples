// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario2_ParallaxHub.g.h"

namespace Transform3DParallax
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_ParallaxHub sealed
    {
    public:
        Scenario2_ParallaxHub();

		static property Windows::UI::Xaml::DependencyProperty^ DesiredHubSectionWidthProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get() { return s_desiredHubSectionWidthProperty; }
		}

		property double DesiredHubSectionWidth
		{
			double get() { return static_cast<double>(GetValue(s_desiredHubSectionWidthProperty)); }
			void set(double value) { SetValue(s_desiredHubSectionWidthProperty, value); }
		}

	internal:
		static void RegisterDependencyProperties();

	private:
		static Windows::UI::Xaml::DependencyProperty^ s_desiredHubSectionWidthProperty;

		void ParallaxBackgroundHub_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
	};
}
