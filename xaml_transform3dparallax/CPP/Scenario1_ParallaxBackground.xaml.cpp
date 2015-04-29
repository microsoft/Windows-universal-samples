// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_ParallaxBackground.xaml.h"

using namespace Transform3DParallax;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Media3D;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario1_ParallaxBackground::Scenario1_ParallaxBackground() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

/*
* This is where the magic happens!
*
* We need to do two things:
*
*   1. Make sure that the center of the perspective transform and image transform are
*   the same. To do this, we manually set the center of the image transform to
*   the center of ParallaxRoot, which is the center of the perspective transform by default.
*
*   2. Scale up the element to compensate for the perspective effect. This will make the
*   transformed element appear like the source asset even though it is "further away" from the
*   viewer than the foreground. The formula for this is:
*
*      scaleFactor = -translateZ / perspectiveDepth + 1.0
*
*/
void Scenario1_ParallaxBackground::NormalizeParallax(UIElement^ target)
{
	auto transform = dynamic_cast<CompositeTransform3D^>(target->Transform3D);

	if (transform != nullptr)
	{
		auto transformToVisual = ParallaxRoot->TransformToVisual(target);
		auto center = Point(
			static_cast<float>(ParallaxRoot->ActualWidth) / 2.0f,
			static_cast<float>(RootGrid->ActualHeight) / 2.0f
			);

		// Center of ParallaxRoot in the coordinates of target.
		auto transformedCenter = transformToVisual->TransformPoint(center);

		transform->CenterX = transformedCenter.X;

		// TransformToVisual doesn't account for ScrollViewer offset
		transform->CenterY = transformedCenter.Y - ParallaxRoot->VerticalOffset;

		// This could be done statically in markup but it's easier to show here.
		auto desiredScale = -transform->TranslateZ / PerspectiveTransform->Depth + 1.0;
		transform->ScaleX = desiredScale;
		transform->ScaleY = desiredScale;
	}
}

void Scenario1_ParallaxBackground::RootGrid_SizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
	NormalizeParallax(RainierBackgroundImage);
	NormalizeParallax(CliffBackgroundImage);
}
