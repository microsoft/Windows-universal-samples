//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "Scenario3.xaml.h"
#include <WindowsNumerics.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <ppltasks.h>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Input::Inking;
using namespace Windows::UI::Core;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Provider;
using namespace Windows::Storage::Streams;
using namespace concurrency;

Scenario3::Scenario3() : rootPage(MainPage::Current)
{
    InitializeComponent();

	penSize = minPenSize + penSizeIncrement * PenThickness->SelectedIndex;

	InkDrawingAttributes^ drawingAttributes = ref new InkDrawingAttributes();
	drawingAttributes->Color = Windows::UI::Colors::Red;
	drawingAttributes->Size = Size((float)penSize, (float)penSize);
	drawingAttributes->IgnorePressure = false;
	drawingAttributes->FitToCurve = true;

	inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
	inkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen;
	inkCanvas->InkPresenter->StrokesCollected += ref new TypedEventHandler<InkPresenter^, InkStrokesCollectedEventArgs^>(this, &Scenario3::InkPresenter_StrokesCollected);
	inkCanvas->InkPresenter->StrokesErased += ref new TypedEventHandler<InkPresenter^, InkStrokesErasedEventArgs^>(this, &Scenario3::InkPresenter_StrokesErased);
}

void Scenario3::OnSizeChanged(Platform::Object^ sender, SizeChangedEventArgs e)
{
	HelperFunctions::UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
}


void Scenario3::InkPresenter_StrokesErased(InkPresenter^ sender, InkStrokesErasedEventArgs^ args)
{
	rootPage->NotifyUser(args->Strokes->Size + " stroke(s) erased!", NotifyType::StatusMessage);
}

void Scenario3::InkPresenter_StrokesCollected(InkPresenter^ sender, InkStrokesCollectedEventArgs^ args)
{
	rootPage->NotifyUser(args->Strokes->Size + " stroke(s) collected!", NotifyType::StatusMessage);
}

void Scenario3::OnSizeChanged(Platform::Object^ sender, SizeChangedEventArgs^ e)
{
	HelperFunctions::UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
}

void Scenario3::OnPenColorChanged(Platform::Object^ sender, RoutedEventArgs^ e)
{
	if (inkCanvas != nullptr)
	{
		InkDrawingAttributes^ drawingAttributes = inkCanvas->InkPresenter->CopyDefaultDrawingAttributes();

		// Use button's background to set new pen's color
		Button^ btnSender = (Button^)sender;
		Media::SolidColorBrush^ brush = (Media::SolidColorBrush^)btnSender->Background;

		drawingAttributes->Color = brush->Color;
		inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
	}
}

void Scenario3::OnPenThicknessChanged(Platform::Object^ sender, RoutedEventArgs^ e)
{
	if (inkCanvas != nullptr)
	{
		InkDrawingAttributes^ drawingAttributes = inkCanvas->InkPresenter->CopyDefaultDrawingAttributes();
		penSize = minPenSize + penSizeIncrement * PenThickness->SelectedIndex;
		Platform::String^ value = ((ComboBoxItem^)PenType->SelectedItem)->Content->ToString();
		if (value == "Highlighter" || value == "Calligraphy")
		{
			// Make the pen tip rectangular for highlighter and calligraphy pen
			drawingAttributes->Size = Size((float)penSize, (float)(penSize * 2));
		}
		else
		{
			// Otherwise, use a square pen tim.
			drawingAttributes->Size = Size((float)penSize, (float)penSize);
		}
		inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
	}
}

void Scenario3::OnPenTypeChanged(Platform::Object^ sender, RoutedEventArgs^ e)
{
	if (inkCanvas != nullptr)
	{
		InkDrawingAttributes^ drawingAttributes = inkCanvas->InkPresenter->CopyDefaultDrawingAttributes();
		Platform::String^ value = ((ComboBoxItem^)PenType->SelectedItem)->Content->ToString();

		if (value == "Ballpoint")
		{
			if (drawingAttributes->Kind != InkDrawingAttributesKind::Default)
			{
				auto newDrawingAttributes = ref new InkDrawingAttributes();
				newDrawingAttributes->Color = drawingAttributes->Color;
				drawingAttributes = newDrawingAttributes;
			}
			drawingAttributes->Size = Size((float)penSize, (float)penSize);
			drawingAttributes->PenTip = PenTipShape::Circle;
			drawingAttributes->DrawAsHighlighter = false;
			drawingAttributes->PenTipTransform = float3x2::identity();
		}
		else if (value == "Highlighter")
		{
			if (drawingAttributes->Kind != InkDrawingAttributesKind::Default)
			{
				auto newDrawingAttributes = ref new InkDrawingAttributes();
				newDrawingAttributes->Color = drawingAttributes->Color;
				drawingAttributes = newDrawingAttributes;
			}
			// Make the pen rectangular for highlighter
			drawingAttributes->Size = Size((float)penSize, (float)(penSize * 2));
			drawingAttributes->PenTip = PenTipShape::Rectangle;
			drawingAttributes->DrawAsHighlighter = true;
			drawingAttributes->PenTipTransform = float3x2::identity();
		}
		else if (value == "Calligraphy")
		{
			if (drawingAttributes->Kind != InkDrawingAttributesKind::Default)
			{
				auto newDrawingAttributes = ref new InkDrawingAttributes();
				newDrawingAttributes->Color = drawingAttributes->Color;
				drawingAttributes = newDrawingAttributes;
			}
			drawingAttributes->Size = Size((float)penSize, (float)(penSize * 2));
			drawingAttributes->PenTip = PenTipShape::Rectangle;
			drawingAttributes->DrawAsHighlighter = false;

			// Set a 45 degree rotation on the pen tip
			double radians = 45.0 * M_PI / 180.0;
			drawingAttributes->PenTipTransform = make_float3x2_rotation((float)radians, float2::zero());
		}
		else if (value == "Windows.UI.Xaml.Controls.TextBlock")
		{
			if (drawingAttributes->Kind != InkDrawingAttributesKind::Pencil)
			{
				auto newDrawingAttributes = InkDrawingAttributes::CreateForPencil();
				newDrawingAttributes->Color = drawingAttributes->Color;
				newDrawingAttributes->Size = drawingAttributes->Size;
				drawingAttributes = newDrawingAttributes;
			}
		}
		inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
	}
}

void Scenario3::OnClear(Platform::Object^ sender, RoutedEventArgs^ e)
{
	inkCanvas->InkPresenter->StrokeContainer->Clear();
	rootPage->NotifyUser("Cleared Canvas", NotifyType::StatusMessage);
}

void Scenario3::OnSaveAsync(Platform::Object^ sender, RoutedEventArgs^ e)
{
	// We don't want to save an empty file
	if (inkCanvas->InkPresenter->StrokeContainer->GetStrokes()->Size == 0)
	{
		return;
	}

	auto savePicker = ref new FileSavePicker();
	savePicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;

	auto extensions = ref new Platform::Collections::Vector<String^>();
	extensions->Append(".gif");
	savePicker->FileTypeChoices->Insert("Plain Text", extensions);
	savePicker->SuggestedFileName = "Scenario3";

	auto t = create_task(savePicker->PickSaveFileAsync()).then([this](StorageFile^ file)
	{
		if (file != nullptr)
		{
			// Prevent updates to the remote version of the file until we finish making changes and call CompleteUpdatesAsync.
			CachedFileManager::DeferUpdates(file);

			// write to file
			return create_task(file->OpenAsync(FileAccessMode::ReadWrite)).then([this, file](IRandomAccessStream^ stream)
			{
				// Let Windows know that we're finished changing the file so the other app can update the remote version of the file.
				// Completing updates may require Windows to ask for user input.
				return create_task(inkCanvas->InkPresenter->StrokeContainer->SaveAsync(stream)).then([this, file](std::size_t size)
				{
					// Let Windows know that we're finished changing the file so the other app can update the remote version of the file.
					// Completing updates may require Windows to ask for user input.
					return create_task(CachedFileManager::CompleteUpdatesAsync(file)).then([this, file](FileUpdateStatus status)
					{
						return status == FileUpdateStatus::Complete;
					});
				});
			});
		}
		else
		{
			return create_task([]{
				return false;
			});
		}
	})
	.then([this](task<bool> t)
	{
		try
		{
			bool saved = t.get();
			if (saved)
			{
				rootPage->NotifyUser("File was saved", NotifyType::StatusMessage);
			}
			else
			{
				rootPage->NotifyUser("File was not saved", NotifyType::StatusMessage);
			}
		}
		catch (Platform::Exception^ ex)
		{
			//Example output: The system cannot find the specified file.
			rootPage->NotifyUser(ex->Message, NotifyType::ErrorMessage);
		}
 	});
}

void Scenario3::OnLoadAsync(Platform::Object^ sender, RoutedEventArgs^ e)
{
	auto openPicker = ref new Pickers::FileOpenPicker();
	openPicker->SuggestedStartLocation = Pickers::PickerLocationId::PicturesLibrary;
	openPicker->FileTypeFilter->Append(".gif");
	openPicker->FileTypeFilter->Append(".isf");

	auto t = create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file)
	{
		if (file != nullptr)
		{
			// read file
			return create_task(file->OpenSequentialReadAsync()).then([this, file](IInputStream^ stream)
			{
				// Let Windows know that we're finished changing the file so the other app can update the remote version of the file.
				// Completing updates may require Windows to ask for user input.
				return create_task(inkCanvas->InkPresenter->StrokeContainer->LoadAsync(stream)).then([]()
				{
					// Let Windows know that we're finished changing the file so the other app can update the remote version of the file.
					return true;
				});
			});
		}
		else
		{
			return create_task([] {
				return false;
			});
		}
	})
	.then([this](task<bool> t)
	{
		try
		{
			bool saved = t.get();
			if (saved)
			{
				rootPage->NotifyUser("File was loaded", NotifyType::StatusMessage);
			}
			else
			{
				rootPage->NotifyUser("File was not loaded", NotifyType::StatusMessage);
			}
		}
		catch (Platform::Exception^ ex)
		{
			rootPage->NotifyUser(ex->Message, NotifyType::ErrorMessage);
		}
	});
}

void Scenario3::TouchInkingCheckBox_Checked(Platform::Object^ sender, RoutedEventArgs^ e)
{
	auto types = inkCanvas->InkPresenter->InputDeviceTypes | CoreInputDeviceTypes::Touch;
	inkCanvas->InkPresenter->InputDeviceTypes = types;
	rootPage->NotifyUser("Enable Touch Inking", NotifyType::StatusMessage);
}

void Scenario3::TouchInkingCheckBox_Unchecked(Platform::Object^ sender, RoutedEventArgs^ e)
{
	auto types = inkCanvas->InkPresenter->InputDeviceTypes & ~CoreInputDeviceTypes::Touch;
	inkCanvas->InkPresenter->InputDeviceTypes = types;
	rootPage->NotifyUser("Disable Touch Inking", NotifyType::StatusMessage);
}

void Scenario3::ErasingModeCheckBox_Checked(Platform::Object^ sender, RoutedEventArgs^ e)
{
	inkCanvas->InkPresenter->InputProcessingConfiguration->Mode = InkInputProcessingMode::Erasing;
	rootPage->NotifyUser("Enable Erasing Mode", NotifyType::StatusMessage);
}

void Scenario3::ErasingModeCheckBox_Unchecked(Platform::Object^ sender, RoutedEventArgs^ e)
{
	inkCanvas->InkPresenter->InputProcessingConfiguration->Mode = InkInputProcessingMode::Inking;
	rootPage->NotifyUser("Disable Erasing Mode", NotifyType::StatusMessage);
}
