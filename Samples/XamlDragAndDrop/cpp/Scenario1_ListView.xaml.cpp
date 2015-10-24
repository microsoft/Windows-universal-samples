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
#include "Scenario1_ListView.xaml.h"

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

Scenario1_ListView::Scenario1_ListView() : rootPage(MainPage::Current)
{
    InitializeComponent();

    _reference = GetSampleData();
    _selection = ref new Vector<String^>();
    SourceListView->ItemsSource = _reference;
    TargetListView->ItemsSource = _selection;
}

Vector<String^>^ Scenario1_ListView::GetSampleData()
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
void Scenario1_ListView::SourceListView_DragItemsStarting(Platform::Object^ sender, Windows::UI::Xaml::Controls::DragItemsStartingEventArgs^ e)
{
    // Prepare a string with one dragged item per line
    String^ items = nullptr;
    unsigned int n = e->Items->Size;
    for (unsigned int i = 0; i < n; i++)
    {
        if (items != nullptr) items += ref new String(L"\n");
        items += dynamic_cast<String^>(e->Items->GetAt(i));
    }
    // Set the content of the DataPackage
    e->Data->SetText(items);
    // As we want our Reference list to say intact, we only allow Copy
    e->Data->RequestedOperation = DataPackageOperation::Copy;
}

/// <summary>
/// DragOver is called when the dragged pointer moves over a UIElement with AllowDrop=True
/// We need to return an AcceptedOperation != None in either DragOver or DragEnter
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_ListView::TargetListView_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    // Our list only accepts text
    e->AcceptedOperation = (e->DataView->Contains(StandardDataFormats::Text)) ? DataPackageOperation::Copy : DataPackageOperation::None;
}

/// <summary>
/// We need to return the effective operation from Drop
/// This is not important for our source ListView, but it might be if the user
/// drags text from another source
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_ListView::TargetListView_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    // This test is in theory not needed as we returned DataPackageOperation.None if
    // the DataPackage did not contained text. However, it is always better if each
    // method is robust by itself
    if (e->DataView->Contains(StandardDataFormats::Text))
    {
        // We need to take a Deferral as we won't be able to confirm the end
        // of the operation synchronously
        auto def = e->GetDeferral();
        create_task(e->DataView->GetTextAsync()).then([def, this, e](String^ s)
        {
            // Parse the string to add items corresponding to each line
            auto wsText = s->Data();
            while (wsText) {
                auto wsNext = wcschr(wsText, L'\n');
                if (wsNext == nullptr)
                {
                    // No more separator
                    _selection->Append(ref new String(wsText));
                    wsText = wsNext;
                }
                else
                {
                    _selection->Append(ref new String(wsText, wsNext - wsText));
                    wsText = wsNext + 1;
                }

            }
            e->AcceptedOperation = DataPackageOperation::Copy;
            def->Complete();
        });
    }
}

/// <summary>
/// DragtemsStarting is called for D&D and reorder as the framework does not
/// know wherer the user will drop the items. Reorder means that the target
/// and the source ListView are the same.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_ListView::TargetListView_DragItemsStarting(Platform::Object^ sender, Windows::UI::Xaml::Controls::DragItemsStartingEventArgs^ e)
{
    // The ListView is declared with selection mode set to Single.
    // But we want the code to be robust
    if (e->Items->Size == 1)
    {
        e->Data->SetText(dynamic_cast<String^>(e->Items->GetAt(0)));
        // Reorder or move to trash are always a move
        e->Data->RequestedOperation = DataPackageOperation::Move;
        _deletedItem = nullptr;
    }
}

/// <summary>
/// Called at the end of the operation, whether it was a reorder or move to trash
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void Scenario1_ListView::TargetListView_DragItemsCompleted(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::DragItemsCompletedEventArgs^ args)
{
    // args.DropResult is always Move and therefore we have to rely on _deletedItem to distinguish
    // between reorder and move to trash
    // Another solution would be to listen for events in the ObservableCollection
    unsigned index;
    if ((_deletedItem != nullptr) && _selection->IndexOf(_deletedItem, &index))
    {
        _selection->RemoveAt(index);
        _deletedItem = nullptr;
    }
}

/// <summary>
/// Entering the Trash icon
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_ListView::TargetTextBlock_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    // Trash only accepts text
    e->AcceptedOperation = (e->DataView->Contains(StandardDataFormats::Text) ? DataPackageOperation::Move : DataPackageOperation::None);
    // We don't want to show the Move icon
    e->DragUIOverride->IsGlyphVisible = false;
    e->DragUIOverride->Caption = ref new String(L"Drop item here to remove it from selection");
}

/// <summary>
/// Drop on the Trash
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_ListView::TargetTextBlock_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    if (e->DataView->Contains(StandardDataFormats::Text))
    {
        // We need to take the deferral as the source will read _deletedItem which
        // we cannot set synchronously
        auto def = e->GetDeferral();
        create_task(e->DataView->GetTextAsync()).then([this, e, def](String^ s)
        {
            _deletedItem = s;
            e->AcceptedOperation = DataPackageOperation::Move;
            def->Complete();
        });
    }
}

