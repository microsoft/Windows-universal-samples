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
#include "SampleConfiguration.h"
#include "Scenario2_Load.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;

namespace SDKTemplate
{
    // Observable collection representing a text message conversation
    // that can load more items incrementally.
    public ref class Conversation sealed : IBindableObservableVector, ISupportIncrementalLoading
    {
    public:
        Conversation()
        {
            _token = _storage->VectorChanged += ref new VectorChangedEventHandler<TextMessage^>(this, &Conversation::Storage_VectorChanged);
        }

        virtual ~Conversation()
        {
            _storage->VectorChanged -= _token;
        }

#pragma region ISupportIncrementalLoading
        property bool HasMoreItems { virtual bool get() { return true; } }

        virtual IAsyncOperation<LoadMoreItemsResult>^ LoadMoreItemsAsync(unsigned int count)
        {
            CreateMessages(count);
            return create_async([count]()
            {
                //return task_from_result(LoadMoreItemsResult{ count });
                return LoadMoreItemsResult{ count };
            });
        }
#pragma endregion

#pragma region IBindableObservableVector

        virtual event BindableVectorChangedEventHandler^ VectorChanged
        {
            virtual EventRegistrationToken add(BindableVectorChangedEventHandler^ e)
            {
                return _storageChanged += e;
            }

            virtual void remove(EventRegistrationToken t)
            {
                _storageChanged -= t;
            }
        }

#pragma endregion 

#pragma region IBindableIterator

        virtual IBindableIterator^ First()
        {
            return dynamic_cast<IBindableIterator^>(_storage->First());
        }

#pragma endregion

#pragma region IBindableVector 

        virtual void Append(Object^ value)
        {
            _storage->Append(safe_cast<TextMessage^>(value));
        }

        virtual void Clear()
        {
            _storage->Clear();
        }

        virtual Object^ GetAt(unsigned int index)
        {
            return _storage->GetAt(index);
        }

        virtual IBindableVectorView^ GetView()
        {
            return safe_cast<IBindableVectorView^>(_storage->GetView());
        }

        virtual bool IndexOf(Object^ value, unsigned int* index)
        {
            return _storage->IndexOf(safe_cast<TextMessage^>(value), index);
        }

        virtual void InsertAt(unsigned int index, Object^ value)
        {
            _storage->InsertAt(index, safe_cast<TextMessage^>(value));
        }

        virtual void RemoveAt(unsigned int index)
        {
            _storage->RemoveAt(index);
        }

        virtual void RemoveAtEnd()
        {
            _storage->RemoveAtEnd();
        }

        virtual void SetAt(unsigned int index, Object^ value)
        {
            _storage->SetAt(index, safe_cast<TextMessage^>(value));
        }

        virtual property unsigned int Size
        {
            unsigned int get() { return _storage->Size; }
        }
#pragma endregion

        static String^ CreateRandomMessage()
        {
            static auto fillerText = L"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
            int length = wcslen(fillerText);
            int randomLength = 5 + rand() % (length - 5);
            return ref new String(fillerText, randomLength);
        }

    private:
        void CreateMessages(unsigned int count)
        {
            for (unsigned int i = 0; i < count; i++)
            {
                auto message = ref new TextMessage();
                message->Body = messageCount.ToString() + ": " + CreateRandomMessage();
                message->IsSent = 0 == messageCount++ % 2;
                message->DisplayTime = Helpers::GetCurrentDateTimeAsString();
                _storage->InsertAt(0, message);
            }
        }

        void Storage_VectorChanged(IObservableVector<TextMessage^>^ sender, IVectorChangedEventArgs^ args)
        {
            _storageChanged(this, args);
        }

        unsigned int messageCount = 0;
        Vector<TextMessage^>^ _storage = ref new Vector<TextMessage^>();
        event BindableVectorChangedEventHandler^ _storageChanged;
        EventRegistrationToken _token;
    };
}

ChatListView::ChatListView()
{
    // We'll manually trigger the loading of data incrementally and buffer for 2 pages worth of data
    IncrementalLoadingTrigger = Windows::UI::Xaml::Controls::IncrementalLoadingTrigger::None;

    // Since we'll have variable sized items we compute a running average of height to help estimate
    // how much data to request for incremental loading
    ContainerContentChanging += UpdateRunningAverageContainerHeightHandler;
}

void ChatListView::OnApplyTemplate()
{
    auto scrollViewer = dynamic_cast<ScrollViewer^>(GetTemplateChild("ScrollViewer"));

    if (scrollViewer != nullptr)
    {
        scrollViewer->ViewChanged += ref new EventHandler<ScrollViewerViewChangedEventArgs^>(this, &ChatListView::ScrollViewer_ViewChanged);
    }

    __super::OnApplyTemplate();
}

void ChatListView::ScrollViewer_ViewChanged(Object^ sender, ScrollViewerViewChangedEventArgs^ args)
{
    // Check if we should load more data when the scroll position changes.
    // We only get this once the content/panel is large enough to be scrollable.
    StartProcessingDataVirtualizationScrollOffsets(ActualHeight);
};

// We use ArrangeOverride to trigger incrementally loading data (if needed) when the panel is too small to be scrollable.
Size ChatListView::ArrangeOverride(Size finalSize)
{
    // Allow the panel to arrange first
    Size result = __super::ArrangeOverride(finalSize);

    StartProcessingDataVirtualizationScrollOffsets(finalSize.Height);

    return result;
}

void ChatListView::StartProcessingDataVirtualizationScrollOffsets(double actualHeight)
{
    // Avoid re-entrancy. If we are already processing, then defer this request.
    if (processingScrollOffsets)
    {
        processingScrollOffsetsDeferred = true;
        return;
    }

    processingScrollOffsets = true;
    ProcessDataVirtualizationScrollOffsetsAsync(actualHeight);
}

task<void> ChatListView::ProcessDataVirtualizationScrollOffsetsAsync(double actualHeight)
{
    task<void> loadTask = task_from_result();

    processingScrollOffsetsDeferred = false;

    auto panel = dynamic_cast<ItemsStackPanel^>(ItemsPanelRoot);
    if (panel != nullptr)
    {
        if ((panel->FirstVisibleIndex != -1 && panel->FirstVisibleIndex * averageContainerHeight < actualHeight * IncrementalLoadingThreshold) ||
            (Items->Size == 0))
        {
            auto virtualizingDataSource = dynamic_cast<ISupportIncrementalLoading^>(ItemsSource);
            if (virtualizingDataSource != nullptr)
            {
                if (virtualizingDataSource->HasMoreItems)
                {
                    unsigned int itemsToLoad;
                    if (averageContainerHeight == 0.0)
                    {
                        // We don't have any items yet. Load the first one so we can get an
                        // estimate of the height of one item, and then we can load the rest.
                        itemsToLoad = 1;
                    }
                    else
                    {
                        double avgItemsPerPage = actualHeight / averageContainerHeight;
                        // We know there's data to be loaded so load at least one item
                        itemsToLoad = (std::max)((unsigned int)(DataFetchSize * avgItemsPerPage), 1U);
                    }

                    loadTask = create_task(virtualizingDataSource->LoadMoreItemsAsync(itemsToLoad)).then([](LoadMoreItemsResult) {});
                }
            }
        }
    }

    return loadTask.then([this, actualHeight]()
    {
        // If a request to process scroll offsets occurred while we were processing
        // the previous request, then process the deferred request now.
        if (processingScrollOffsetsDeferred)
        {
            return ProcessDataVirtualizationScrollOffsetsAsync(actualHeight);
        }

        // We have finished. Allow new requests to be processed.
        processingScrollOffsets = false;
        return task_from_result();
    }, task_continuation_context::get_current_winrt_context());
}

void ChatListView::UpdateRunningAverageContainerHeight(ListViewBase^ sender, ContainerContentChangingEventArgs^ args)
{
    if (args->ItemContainer != nullptr && !args->InRecycleQueue)
    {
        switch (args->Phase)
        {
        case 0:
            // use the size of the very first placeholder as a starting point until
            // we've seen the first item
            if (averageContainerHeight == 0)
            {
                averageContainerHeight = args->ItemContainer->DesiredSize.Height;
            }

            args->RegisterUpdateCallback(1, UpdateRunningAverageContainerHeightHandler);
            args->Handled = true;
            break;

        case 1:
            // set the content
            args->ItemContainer->Content = args->Item;
            args->RegisterUpdateCallback(2, UpdateRunningAverageContainerHeightHandler);
            args->Handled = true;
            break;

        case 2:
            // refine the estimate based on the item's DesiredSize
            averageContainerHeight = (averageContainerHeight * itemsSeen + args->ItemContainer->DesiredSize.Height) / ++itemsSeen;
            args->Handled = true;
        }
    }
}

Scenario2_Load::Scenario2_Load() : conversation(ref new Conversation())
{
    InitializeComponent();
    chatView->ItemsSource = conversation;
    chatView->ContainerContentChanging += ref new TypedEventHandler<ListViewBase^, ContainerContentChangingEventArgs^>(
        this, &Scenario2_Load::OnChatViewContainerContentChanging);
}

void Scenario2_Load::SendTextMessage()
{
    if (MessageTextBox->Text->Length() > 0)
    {
        auto message = ref new TextMessage();
        message->Body = MessageTextBox->Text;
        message->DisplayTime = Helpers::GetCurrentDateTimeAsString();
        message->IsSent = true;
        conversation->Append(message);

        MessageTextBox->Text = "";

        // Send a simulated reply after a brief delay.
        create_task(Helpers::DelayAsync(Helpers::TimeSpanFromSeconds(2))
            .then([this]()
        {
            auto message = ref new TextMessage();
            message->Body = Conversation::CreateRandomMessage();
            message->DisplayTime = Helpers::GetCurrentDateTimeAsString();
            message->IsSent = false;
            conversation->Append(message);
        }, task_continuation_context::get_current_winrt_context()));
    }
}

void Scenario2_Load::OnChatViewContainerContentChanging(ListViewBase^ sender, ContainerContentChangingEventArgs^ args)
{
    if (args->InRecycleQueue) return;
    TextMessage^ message = safe_cast<TextMessage^>(args->Item);
    args->ItemContainer->HorizontalAlignment = message->IsSent ? Windows::UI::Xaml::HorizontalAlignment::Right : Windows::UI::Xaml::HorizontalAlignment::Left;
}

void Scenario2_Load::MessageTextBox_KeyUp(Object^ sender, KeyRoutedEventArgs^ e)
{
    if (e->Key == Windows::System::VirtualKey::Enter)
    {
        SendTextMessage();
    }
}
