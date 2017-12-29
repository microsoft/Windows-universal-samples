#pragma once


ref class VisualProperties sealed : public Windows::ApplicationModel::Core::IFrameworkView 
{
	Windows::ApplicationModel::Core::CoreApplicationView^ _view;
	Windows::UI::Core::CoreWindow^ _window;
	Windows::UI::Composition::Compositor^ _compositor;
	Windows::UI::Composition::ContainerVisual^ _root;
	Windows::UI::Composition::CompositionTarget^ _compositionTarget;
	Windows::UI::Composition::ContainerVisual^ _currentVisual;
	Windows::Foundation::Numerics::float2* _offsetBias;
	bool _dragging;

	void InitNewComposition();
	Windows::UI::Composition::Visual^ CreateChildElement();

public:
	VisualProperties();

    // IFrameworkView
	void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ view) override;
    void SetWindow(Windows::UI::Core::CoreWindow^ window) override;
    void Load(Platform::String^ unused) override;
    void Run() override;
    void Uninitialize() override;

	void OnPointerPressed(Windows::UI::Core::CoreWindow^ window, Windows::UI::Core::PointerEventArgs^ args);
	void OnPointerMoved(Windows::UI::Core::CoreWindow^ window, Windows::UI::Core::PointerEventArgs^ args);
	void OnPointerReleased(Windows::UI::Core::CoreWindow^ window, Windows::UI::Core::PointerEventArgs^ args);
};


ref class VisualPropertiesFactory : Windows::ApplicationModel::Core::IFrameworkViewSource
{
	//------------------------------------------------------------------------------
	//
	// VisualPropertiesFactory.CreateView
	//
	// This method is called by CoreApplication to provide a new IFrameworkView for
	// a CoreWindow that is being created.
	//
	//------------------------------------------------------------------------------
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
	{
		return ref new VisualProperties();
	}
};


