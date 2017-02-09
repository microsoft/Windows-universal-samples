#include "pch.h"
#include "VisualProperties.h"

#include "windowsnumerics.h"

using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::UI::Composition;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;

//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------


// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto visualPropertiesFactory = ref new VisualPropertiesFactory();
	CoreApplication::Run(visualPropertiesFactory);
	return 0;
}


VisualProperties::VisualProperties()
{
}

//------------------------------------------------------------------------------
//
// VisualProperties.Initialize
//
// This method is called during startup to associate the IFrameworkView with the
// CoreApplicationView.
//
//------------------------------------------------------------------------------

void VisualProperties::Initialize(CoreApplicationView^ view)
{
	_view = view;
}

//------------------------------------------------------------------------------
//
// VisualProperties.SetWindow
//
// This method is called when the CoreApplication has created a new CoreWindow,
// allowing the application to configure the window and start producing content
// to display.
//
//------------------------------------------------------------------------------

void VisualProperties::SetWindow(Windows::UI::Core::CoreWindow^ window)
{
	_window = window;
	InitNewComposition();

	_window->PointerPressed += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &VisualProperties::OnPointerPressed);
	_window->PointerMoved += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &VisualProperties::OnPointerMoved);
	_window->PointerReleased += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &VisualProperties::OnPointerReleased);
}

//------------------------------------------------------------------------------
//
// VisualProperties.OnPointerPressed
//
// This method is called when the user touches the screen, taps it with a stylus
// or clicks the mouse.
//
//------------------------------------------------------------------------------

void VisualProperties::OnPointerPressed(Windows::UI::Core::CoreWindow^ window, Windows::UI::Core::PointerEventArgs^ args)
{
	auto position = args->CurrentPoint->Position;

	//
	// Walk our list of visuals to determine who, if anybody, was selected
	//
	for (Visual^ child : _root->Children)
	{
		//
		// Did we hit this child?
		//
		float3 offset = child->Offset;
		float2 size = child->Size;

		if ((position.X >= offset.x) &&
			(position.X < offset.x + size.x) &&
			(position.Y >= offset.y) &&
			(position.Y < offset.y + size.y))
		{
			//
			// This child was hit. Since the children are stored back to front,
			// the last one hit is the front-most one so it wins
			//
			_currentVisual = dynamic_cast<ContainerVisual^>(child);
			_offsetBias = new float2((float)(offset.x - position.X),
				(float)(offset.y - position.Y));
		}
	}

	//
	// If a visual was hit, bring it to the front of the Z order
	//
	if (_currentVisual != nullptr)
	{
		ContainerVisual^ parent = dynamic_cast<ContainerVisual^>(_currentVisual->Parent);
		parent->Children->Remove(_currentVisual);
		parent->Children->InsertAtTop(_currentVisual);
	}
}

//------------------------------------------------------------------------------
//
// VisualProperties.OnPointerMoved
//
// This method is called when the user moves their finger, stylus or mouse with
// a button pressed over the screen.
//
//------------------------------------------------------------------------------

void VisualProperties::OnPointerMoved(Windows::UI::Core::CoreWindow^ window, Windows::UI::Core::PointerEventArgs^ args)
{
	//
	// If a visual is selected, drag it with the pointer position and
	// make it opaque while we drag it
	//
	if (_currentVisual != nullptr)
	{
		//
		// Set up the properties of the visual the first time it is
		// dragged. This will last for the duration of the drag
		//
		if (!_dragging)
		{
			_currentVisual->Opacity = 1.0f;

			//
			// Transform the first child of the current visual so that
			// the image is rotated
			//
            for (auto child : _currentVisual->Children)
			{
				child->RotationAngleInDegrees = 45.0f;
				child->CenterPoint = float3(_currentVisual->Size.x / 2, _currentVisual->Size.y / 2, 0);
                break;
			}

			//
			// Clip the visual to its original layout rect by using an inset
			// clip with a one-pixel margin all around
			//
			auto clip = _compositor->CreateInsetClip();
			clip->LeftInset = 1.0f;
			clip->RightInset = 1.0f;
			clip->TopInset = 1.0f;
			clip->BottomInset = 1.0f;
			_currentVisual->Clip = clip;

			_dragging = true;
		}

		Point position = args->CurrentPoint->Position;
		_currentVisual->Offset = float3((float)(position.X + _offsetBias->x),
			(float)(position.Y + _offsetBias->y),
			0.0f);
	}
}

//------------------------------------------------------------------------------
//
// VisualProperties.OnPointerReleased
//
// This method is called when the user lifts their finger or stylus from the
// screen, or lifts the mouse button.
//
//------------------------------------------------------------------------------

void VisualProperties::OnPointerReleased(Windows::UI::Core::CoreWindow^ window, Windows::UI::Core::PointerEventArgs^ args)
{
	//
	// If a visual was selected, make it transparent again when it is
	// released and restore the transform and clip
	//
	if (_currentVisual != nullptr)
	{
		if (_dragging)
		{
			//
			// Remove the transform from the first child
			//
            for (auto child : _currentVisual->Children)
			{
				child->RotationAngleInDegrees = 0;
				child->CenterPoint = float3(0, 0, 0);
                break;
			}

			_currentVisual->Opacity = 0.8f;
			_currentVisual->Clip = nullptr;
			_dragging = false;
		}

		_currentVisual = nullptr;
	}
}

//------------------------------------------------------------------------------
//
// VisualProperties.InitNewComposition
//
// This method is called by SetWindow(), where we initialize Composition after
// the CoreWindow has been created.
//
//------------------------------------------------------------------------------

void VisualProperties::InitNewComposition()
{
	//
	// Set up Windows.UI.Composition Compositor, root ContainerVisual, and associate with
	// the CoreWindow.
	//

	_compositor = ref new Compositor();

	_root = _compositor->CreateContainerVisual();

	_compositionTarget = _compositor->CreateTargetForCurrentView();
	_compositionTarget->Root = _root;

	//
	// Create a few visuals for our window
	//
	for (int index = 0; index < 20; index++)
	{
		_root->Children->InsertAtTop(CreateChildElement());
	}
}

//------------------------------------------------------------------------------
//
// VisualProperties.CreateChildElement
//
// Creates a small sub-tree to represent a visible element in our application.
//
//------------------------------------------------------------------------------

Visual^ VisualProperties::CreateChildElement()
{
	//
	// Each element consists of three visuals, which produce the appearance
	// of a framed rectangle
	//
	auto element = _compositor->CreateContainerVisual();
	element->Size = float2(100.0f, 100.0f);

	//
	// Position this visual randomly within our window
	//
	element->Offset = float3(((float)rand() / RAND_MAX) * 400, ((float)rand() / RAND_MAX) * 400, 0.0f);

	//
	// The outer rectangle is always white
	//
	auto visual = _compositor->CreateSpriteVisual();
	element->Children->InsertAtTop(visual);

	visual->Brush = _compositor->CreateColorBrush(ColorHelper::FromArgb(0xFF, 0xFF, 0xFF, 0xFF));
	visual->Size = float2(100.0f, 100.0f);

	//
	// The inner rectangle is inset from the outer by three pixels all around
	//
	auto child = _compositor->CreateSpriteVisual();
	visual->Children->InsertAtTop(child);
	child->Offset = float3(3.0f, 3.0f, 0.0f);
	child->Size = float2(94.0f, 94.0f);

	//
	// Pick a random color for every rectangle
	//
	byte red = (byte)(0xFF * (0.2f + (((float)rand() / RAND_MAX) / 0.8f)));
	byte green = (byte)(0xFF * (0.2f + (((float)rand() / RAND_MAX) / 0.8f)));
	byte blue = (byte)(0xFF * (0.2f + (((float)rand() / RAND_MAX) / 0.8f)));
	child->Brush = _compositor->CreateColorBrush(ColorHelper::FromArgb(0xFF, red, green, blue));

	//
	// Make the subtree root visual partially transparent. This will cause each visual in the subtree
	// to render partially transparent, since a visual's opacity is multiplied with its parent's
	// opacity
	//
	element->Opacity = 0.8f;

	return element;
}

//------------------------------------------------------------------------------
//
// VisualProperties.Load
//
// This method is called when a specific page is being loaded in the
// application.  It is not used for this application.
//
//------------------------------------------------------------------------------

void VisualProperties::Load(Platform::String ^ unused)
{
}

//------------------------------------------------------------------------------
//
// VisualProperties.Run
//
// This method is called by CoreApplication.Run() to actually run the
// dispatcher's message pump.
//
//------------------------------------------------------------------------------

void VisualProperties::Run()
{
	_window->Activate();
	_window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
}

//------------------------------------------------------------------------------
//
// VisualProperties.Uninitialize
//
// This method is called during shutdown to disconnect the CoreApplicationView,
// and CoreWindow from the IFrameworkView.
//
//------------------------------------------------------------------------------

void VisualProperties::Uninitialize()
{
	_window = nullptr;
	_view = nullptr;
}
