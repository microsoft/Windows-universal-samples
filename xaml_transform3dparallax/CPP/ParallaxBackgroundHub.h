//
// ParallaxBackgroundHub.h
// Declaration of the ParallaxBackgroundHub class.
//

#pragma once

namespace Transform3DParallax
{
	public ref class ParallaxBackgroundHub sealed : public Windows::UI::Xaml::Controls::Hub
	{
	public:
		ParallaxBackgroundHub();

	protected:
		virtual void OnApplyTemplate() override;

	private:
		Windows::UI::Xaml::Controls::ScrollViewer^ m_scrollViewer;
		Windows::UI::Xaml::Media::Media3D::CompositeTransform3D^ m_backgroundTransform;

		void UpdateForSizeChanged();
		void OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e);
	};
}
