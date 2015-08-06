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
#include "Scenario3_StartDragAsync.xaml.h"

using namespace SDKTemplate;
using namespace concurrency;
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
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::ApplicationModel::DataTransfer;


Scenario3_StartDragAsync::Scenario3_StartDragAsync() : rootPage(MainPage::Current)
{
    InitializeComponent();

    // Initialize the symbols from the Text file
    auto uri = ref new Uri(L"ms-appx:///Assets/Symbols.txt");
    create_task(StorageFile::GetFileFromApplicationUriAsync(uri)).then([](StorageFile^ file)
    {
        return FileIO::ReadLinesAsync(file);
    }).then([this](IVector<String^>^ lines)
    {
        _symbols = ref new Vector<Platform::String^>();
        for (unsigned int i = 0; i < lines->Size;i++)
        {
            _symbols->Append(lines->GetAt(i));
        }
        DropGridView->ItemsSource = _symbols;

    });
}

/// <summary>
/// Start button clicked: select a symbol, update the display
/// and start the timer according to the difficulty level
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_StartDragAsync::StartButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (!_isActive)
    {
        int index = _rnd() % _symbols->Size;
        _symbol = _symbols->GetAt(index);
        SourceTextBlock->Text = _symbol;
        ResultTextBlock->Text = nullptr;
        _isActive = true;

        if (_timer == nullptr)
        {
            _timer = ref new DispatcherTimer();
            _timer->Tick += ref new EventHandler<Platform::Object ^>(this, &Scenario3_StartDragAsync::OnTick);
        }
        bool easy = (EasyRB->IsChecked != nullptr) && EasyRB->IsChecked->Value;
        TimeSpan duration;
        duration.Duration = easy ? 50000000 : 15000000;
        _timer->Interval = duration;
        _timer->Start();
        StartButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        SourceTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
}

/// <summary>
///  End of the timer: player lost and we might have to cancel the drag operation
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void Scenario3_StartDragAsync::OnTick(Platform::Object ^sender, Platform::Object ^args)
{
    _timer->Stop();
    if (_dragOperation != nullptr)
    {
        // Cancel the drag operation: this is equivalent of the user pressing Escape
        // or releasing the pointer on a non-target
        // DropCompleted event would return DataPackageOperation.None as the result
        _dragOperation->Cancel();
    }
    else
    {
        // We test _isActive just in case the timer expired when we were handling the drop
        if (_isActive)
        {
            EndRound(false);
        }
    }
}

/// <summary>
/// If the pointer is moved and we are not yet dragging, start a drag operation
/// using StartDragAsync, which will allow a later cancellation
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_StartDragAsync::SourceTextBlock_PointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    if (_isActive
        && e->Pointer->IsInContact
        && (_dragOperation == nullptr))
    {
        _dragOperation = SourceTextBlock->StartDragAsync(e->GetCurrentPoint(SourceTextBlock));
        _dragOperation->Completed = ref new AsyncOperationCompletedHandler<DataPackageOperation>(this, &Scenario3_StartDragAsync::DragCompleted);
    }
}

/// <summary>
/// Update the user interface when the round is finished
/// </summary>
/// <param name="hasWon"></param>
void Scenario3_StartDragAsync::EndRound(bool hasWon)
{
    _isActive = false;
    ResultTextBlock->Text = ref new String(hasWon ? L"You win :-) !!" : L"You lose :-( !!");
    StartButton->Visibility = Windows::UI::Xaml::Visibility::Visible;
    SourceTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

/// <summary>
/// Completion callback for the asynchronous call:
/// it is called whether the drop succeeded, or the user released the pointer on a non-target
/// or else we cancelled the operation from the timer's callback
/// </summary>
/// <param name="asyncInfo"></param>
/// <param name="asyncStatus"></param>
void Scenario3_StartDragAsync::DragCompleted(IAsyncOperation<DataPackageOperation>^ asyncInfo, AsyncStatus asyncStatus)
{
    _dragOperation = nullptr;
    if (_timer != nullptr) _timer->Stop();
    EndRound((asyncStatus == AsyncStatus::Completed) && (asyncInfo->GetResults() == DataPackageOperation::Copy));
}

/// <summary>
/// DragStarting is called even if we are starting the Drag Operation ourselved with StartDragAsync
/// In this sample, we don't really need to handle it
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_StartDragAsync::SourceTextBlock_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ e)
{
    e->Data->RequestedOperation = DataPackageOperation::Copy;
    e->Data->SetText(_symbol);
}

/// <summary>
/// Check if the sender is a border AND has the dragged symbol
/// </summary>
/// <param name="sender"></param>
/// <returns></returns>
bool Scenario3_StartDragAsync::IsTargetBorder(Platform::Object^ sender)
{
    auto border = dynamic_cast<Border^>(sender);
    return (border != nullptr) && (dynamic_cast<String^>(border->DataContext) == _symbol);
}

/// <summary>
/// Entering a target: check if this will result in a win
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_StartDragAsync::DropBorder_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    bool win = (_dragOperation != nullptr) && e->DataView->Contains(StandardDataFormats::Text) && IsTargetBorder(sender);
    e->DragUIOverride->IsCaptionVisible = false;
    e->AcceptedOperation = win ? DataPackageOperation::Copy : DataPackageOperation::None;
}

/// <summary>
/// Drop: check if this results in a win
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_StartDragAsync::DropBorder_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    bool win = (_dragOperation != nullptr) && e->DataView->Contains(StandardDataFormats::Text) && IsTargetBorder(sender);
    e->AcceptedOperation = win ? DataPackageOperation::Copy : DataPackageOperation::None;
}
