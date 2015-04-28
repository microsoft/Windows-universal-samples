//
// ParallaxBackgroundHub.cpp
// Implementation of the ParallaxBackgroundHub class.
//

#include "pch.h"
#include "ParallaxBackgroundHub.h"

using namespace Transform3DParallax;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Media3D;

// The Templated Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234235

ParallaxBackgroundHub::ParallaxBackgroundHub()
{
	DefaultStyleKey = "Transform3DParallax.ParallaxBackgroundHub";
}


void ParallaxBackgroundHub::OnApplyTemplate()
{
	Hub::OnApplyTemplate();

	m_scrollViewer = dynamic_cast<ScrollViewer^>(GetTemplateChild(L"ScrollViewer"));
	m_backgroundTransform = dynamic_cast<CompositeTransform3D^>(GetTemplateChild(L"BackgroundTransform"));

	if (m_scrollViewer != nullptr && m_backgroundTransform != nullptr)
	{
		m_scrollViewer->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &Transform3DParallax::ParallaxBackgroundHub::OnSizeChanged);
	}
}

void ParallaxBackgroundHub::UpdateForSizeChanged()
{
	// Since the background Border will always start in the top left, we don't need to use
	// the TransformToVisual() trick here.

	auto horizontalOffset = m_scrollViewer->ViewportWidth / 2.0;
	auto verticalOffset = m_scrollViewer->ViewportHeight / 2.0;

	m_backgroundTransform->CenterX = horizontalOffset;
	m_backgroundTransform->CenterY = verticalOffset;
}


void ParallaxBackgroundHub::OnSizeChanged(Object ^sender, SizeChangedEventArgs ^e)
{
	UpdateForSizeChanged();
}
