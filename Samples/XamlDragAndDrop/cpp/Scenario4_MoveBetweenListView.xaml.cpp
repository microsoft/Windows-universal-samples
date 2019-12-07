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
#include "Scenario4_MoveBetweenListView.xaml.h"

using namespace concurrency;
using namespace SDKTemplate;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel::DataTransfer;

Scenario4_MoveBetweenListView::Scenario4_MoveBetweenListView() : rootPage(MainPage::Current)
{
    InitializeComponent();

    _source = GetSampleData();
    _target = ref new Vector<String^>();
    SourceListView->ItemsSource = _source;
    TargetListView->ItemsSource = _target;
}

Vector<String^>^ Scenario4_MoveBetweenListView::GetSampleData()
{
    return ref new Vector<String^>(
    {
        ref new String(L"My Research Paper"),
        ref new String(L"Electricity Bill"),
        ref new String(L"My To-do list"),
        ref new String(L"TV sales receipt"),
        ref new String(L"Water Bill"),
        ref new String(L"Grocery List"),
        ref new String(L"Superbowl schedule"),
        ref new String(L"World Cup E-ticket")
    });
}

/// <summary>
/// DragItemsStarting is called when the Drag and Drop operation starts
/// We take advantage of it to set the content of the DataPackage
/// as well as indicate which operations are supported
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_MoveBetweenListView::ListView_DragItemsStarting(Platform::Object^ sender, Windows::UI::Xaml::Controls::DragItemsStartingEventArgs^ e)
{
	// Prepare a string with one dragged item per line
	// Set the content of the DataPackage
	if (e->Items->Size > 0)
	{
		e->Data->SetText(dynamic_cast<String^>(e->Items->GetAt(0)));
		// As we want our Reference list to say intact, we only allow Move
		e->Data->RequestedOperation = DataPackageOperation::Move;
	}
}

/// <summary>
/// DragOver is called when the dragged pointer moves over a UIElement with AllowDrop=True
/// We need to return an AcceptedOperation != None in either DragOver or DragEnter
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_MoveBetweenListView::ListView_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    // Our list only accepts text
    e->AcceptedOperation = (e->DataView->Contains(StandardDataFormats::Text)) ? DataPackageOperation::Move : DataPackageOperation::None;
}

/// <summary>
/// We need to return the effective operation from Drop
/// This is not important for our source ListView, but it might be if the user
/// drags text from another source
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_MoveBetweenListView::ListView_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    // This test is in theory not needed as we returned DataPackageOperation.None if
    // the DataPackage did not contained text. However, it is always better if each
    // method is robust by itself
    if (e->DataView->Contains(StandardDataFormats::Text))
    {
        // We need to take a Deferral as we won't be able to confirm the end
        // of the operation synchronously
        auto def = e->GetDeferral();
        create_task(e->DataView->GetTextAsync()).then([def, this, sender, e](String^ s)
        {
			// Get the lists we need to work with.
			auto sourceList = (sender->Equals(TargetListView) ? _source : _target);
			auto targetList = (sender->Equals(TargetListView) ? _target : _source);

			// Remove item from other list
			unsigned j;
			if (sourceList->IndexOf(s, &j))
			{
				sourceList->RemoveAt(j);
			}

			// First we need to get the position in the List to drop to
			auto listview = dynamic_cast<ListView^>(sender);
			int index = -1;

			// Determine which items in the list our pointer is inbetween.
			for (int i = 0; i < listview->Items->Size; i++)
			{
				auto item = dynamic_cast<ListViewItem^>(listview->ContainerFromIndex(i));

				auto p = e->GetPosition(item);

				if (p.Y - item->ActualHeight < 0)
				{
					index = i;
					break;
				}
			}

			if (index < 0)
			{
				// We didn't find a transition point, so we're at the end of the list
				targetList->Append(s);
			}
			else if (index < listview->Items->Size)
			{
				// Otherwise, insert at the provided index.
				targetList->InsertAt(index, s);
			}

            e->AcceptedOperation = DataPackageOperation::Copy;
            def->Complete();
        });
    }
}
