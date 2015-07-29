//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario1.xaml.cpp
// Implementation of the Scenario1 class
//

#include "pch.h"
#include "Scenario1.xaml.h"
#include "CompressionUtils.h"
#include <robuffer.h>

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::Compression;
using namespace Windows::Foundation;

using namespace SDKTemplate::Compression;

using namespace concurrency;

using namespace std;

Scenario1::Scenario1()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

byte* GetUnderlyingBuffer(IBuffer^ buf) 
{
    Microsoft::WRL::ComPtr<IBufferByteAccess> bufferByteAccess;
    HRESULT hr = reinterpret_cast<IUnknown*>(buf)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));
    byte* raw_buffer;
    hr = bufferByteAccess->Buffer(&raw_buffer);
    return raw_buffer;
}

struct PipeEvents {
    task_completion_event<IBuffer^> on_write;
    task_completion_event<void> on_read;
    IBuffer^ unread_buffer;
    unsigned int read_bytes;
    unsigned long long total_pipe_bytes;
};

ref class PipeOut : IOutputStream 
{
internal:
    PipeOut(shared_ptr<PipeEvents> e) 
    {
        events = e;
    }

public:
    virtual IAsyncOperationWithProgress<unsigned int, unsigned int>^ WriteAsync(IBuffer^ buf) 
    {
        return create_async([this, buf](progress_reporter<unsigned int> reporter) -> unsigned int 
        {
            events->total_pipe_bytes += buf->Length;
            events->on_write.set(buf);
            create_task(events->on_read).wait();
            events->on_read = task_completion_event<void>();
            return buf->Length;
        });
    }
    
    virtual IAsyncOperation<bool>^ FlushAsync() 
    {
        return create_async([this]() 
        {
            return true;
        });
    }
    virtual ~PipeOut() { }
private:
    shared_ptr<PipeEvents> events;
};

ref class PipeIn : IInputStream 
{
internal:
    PipeIn(shared_ptr<PipeEvents> e) 
    {
        events = e;
    }
public:
    virtual IAsyncOperationWithProgress<IBuffer^, unsigned int>^ ReadAsync(IBuffer^ buf, unsigned int count, InputStreamOptions options) 
    {
        return create_async([this, buf, count](progress_reporter<unsigned int> reporter) -> IBuffer^ 
        {
            buf->Length = 0;
            while (buf->Length < count) 
            {
                if (events->unread_buffer == nullptr) 
                {
                    events->unread_buffer = create_task(events->on_write).get();
                    events->on_write = task_completion_event<IBuffer^>();
                    events->read_bytes = 0;
                }
                unsigned int to_copy = min(min(count, buf->Capacity) - buf->Length, events->unread_buffer->Length - events->read_bytes);
                memcpy(GetUnderlyingBuffer(buf) + buf->Length, GetUnderlyingBuffer(events->unread_buffer) + events->read_bytes, to_copy);
                buf->Length += to_copy;
                events->read_bytes += to_copy;
                if (events->read_bytes >= events->unread_buffer->Length) 
                {
                    events->unread_buffer = nullptr;
                    events->on_read.set();
                }
            }
            return buf;
        });
    }
    virtual ~PipeIn() { }
private:
    shared_ptr<PipeEvents> events;
};

void ::SDKTemplate::Compression::Scenario1::DoScenario(CompressAlgorithm Algorithm)
{
    Progress->Text = "";
    rootPage->NotifyUser("Working...", NotifyType::StatusMessage);

    auto picker = ref new Pickers::FileOpenPicker();
    picker->FileTypeFilter->Append("*");

    auto events = make_shared<PipeEvents>();
    events->total_pipe_bytes = 0;

    // First pick a test file and open it for reading
    create_task(picker->PickSingleFileAsync()).then([=](StorageFile^ OriginalFile)
    {
        if (!OriginalFile)
        {
            throw std::runtime_error("No file has been selected");
        }

        Progress->Text += "File \"" + OriginalFile->Name + "\" has been picked\n";

        return create_task(OriginalFile->OpenAsync(FileAccessMode::Read)) && create_task(OriginalFile->OpenAsync(FileAccessMode::Read));

    }).then([=](vector<IRandomAccessStream^> FileStream) 
    {

        auto out_pipe = ref new PipeOut(events);
        auto in_pipe = ref new PipeIn(events);

        Compressor^ compressor;

        // Use default constructor if you don't need any specific algorithm and/or block size, use
        // "extended" constructor otherwise. This samples shows both versions for demonstration
        // purposes only.
        if (Algorithm == CompressAlgorithm::InvalidAlgorithm) 
        {
            compressor = ref new Compressor(out_pipe);
        } 
        else 
        {
            compressor = ref new Compressor(out_pipe, Algorithm, 0);
        }

        Decompressor^ decompressor = ref new Decompressor(in_pipe);

        return create_task([=]() 
        {
            // Use a 1MB buffer to read file content in, compress, then decompress.
            unsigned int total = 0;
            IBuffer^ buf = ref new Buffer(1024 * 1024);
            for (;;) 
            {
                create_task(FileStream[0]->ReadAsync(buf, buf->Capacity, InputStreamOptions::None)).wait();
                if (buf->Length == 0) 
                {
                    break;
                }
                total += buf->Length;
                create_task(compressor->WriteAsync(buf)).wait();
            }
            create_task(compressor->FinishAsync()).wait();
            return total;
        }) &&
            create_task([=]() 
            {
                unsigned int total = 0;
                IBuffer^ buf = ref new Buffer(256 * 1024);
                IBuffer^ file_buf = ref new Buffer(buf->Capacity);
                for (;;) 
                {
                    create_task(decompressor->ReadAsync(buf, buf->Capacity, InputStreamOptions::Partial)).wait();
                    if (buf->Length == 0) 
                    {
                        break;
                    }

                    create_task(FileStream[1]->ReadAsync(file_buf, buf->Length, InputStreamOptions::None)).wait();
                    if (file_buf->Length != buf->Length) 
                    {
                        throw runtime_error("unexpected read length");
                    }

                    if (memcmp(GetUnderlyingBuffer(buf), GetUnderlyingBuffer(file_buf), buf->Length) != 0) 
                    {
                        throw runtime_error("compression mismatch!");
                    }
                    total += buf->Length;
                }
                return total;
        });
    })

    .then([=](task<vector<unsigned int> > x) 
    {
        try {
            if (x.get()[0] != x.get()[1]) {
                rootPage->NotifyUser("size mismatch", NotifyType::ErrorMessage);
            } 
            else 
            {
                Progress->Text += "Original size: " + x.get()[0] + "\nCompressed size: " + events->total_pipe_bytes + "\n";
                rootPage->NotifyUser("Done", NotifyType::StatusMessage);
            }
        } 
        catch (Platform::Exception ^e) 
        {
            rootPage->NotifyUser(e->Message, NotifyType::ErrorMessage);
        } 
        catch (const std::exception &e) 
        {
            std::wstringstream wss;
            wss << e.what();
            rootPage->NotifyUser(ref new Platform::String(wss.str().c_str()), NotifyType::ErrorMessage);
        } 
        catch (...) 
        {
            rootPage->NotifyUser("unknown exception", NotifyType::ErrorMessage);
        }
    });
}

void ::SDKTemplate::Compression::Scenario1::CompressFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ComboBoxItem^ item = safe_cast<ComboBoxItem^>(CompressAlgorithmComboBox->SelectedItem);

    if (item == nullptr)
    {
        return;
    }

    CompressAlgorithm Algorithm = (CompressAlgorithm) CompressAlgorithmFromComboBoxItem(item);

    if (Algorithm == CompressAlgorithm::InvalidAlgorithm)
    {
        return;
    }

    DoScenario(Algorithm);
}

void ::SDKTemplate::Compression::Scenario1::CompressAlgorithmComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    SelectionChangedEventArgs^ args = safe_cast<SelectionChangedEventArgs^>(e);

    if (args == nullptr)
    {
        return;
    }
    if (CompressAlgorithmComboBox == nullptr)
    {
        return;
    }

    if (args->AddedItems->Size != 0 && args->RemovedItems->Size == 0)
    {
        ComboBoxItem^ addedItem = safe_cast<ComboBoxItem^>(args->AddedItems->GetAt(0));
        if (addedItem->Content->Equals("Default"))
        {
                UpdateDescriptionVisibility(addedItem, Windows::UI::Xaml::Visibility::Visible);
        }
        return;
    }
    else
    {
        ComboBoxItem^ addedItem = safe_cast<ComboBoxItem^>(args->AddedItems->GetAt(0));
        ComboBoxItem^ removedItem = safe_cast<ComboBoxItem^>(args->RemovedItems->GetAt(0));

        UpdateDescriptionVisibility(addedItem, Windows::UI::Xaml::Visibility::Visible);
        UpdateDescriptionVisibility(removedItem, Windows::UI::Xaml::Visibility::Collapsed);
    }
}

void ::SDKTemplate::Compression::Scenario1::UpdateDescriptionVisibility(ComboBoxItem^ item, Windows::UI::Xaml::Visibility visibility)
{
    TextBlock^ textBlock = nullptr;

    CompressAlgorithm algorithm = (CompressAlgorithm) CompressAlgorithmFromComboBoxItem(item);
    switch (algorithm)
    {
    case CompressAlgorithm::NullAlgorithm:
        textBlock = DefaultTextBlock;
        break;
    case CompressAlgorithm::Xpress:
        textBlock = XpressTextBlock;
        break;
    case CompressAlgorithm::XpressHuff:
        textBlock = XpressHuffTextBlock;
        break;
    case CompressAlgorithm::Mszip:
        textBlock = MszipTextBlock;
        break;
    case CompressAlgorithm::Lzms:
        textBlock = LzmsTextBlock;
        break;
    default:
        return;
    }

    textBlock->Visibility = visibility;
}

int ::SDKTemplate::Compression::Scenario1::CompressAlgorithmFromComboBoxItem(ComboBoxItem^ item)
{
    CompressAlgorithm Algorithm = CompressAlgorithm::NullAlgorithm;
        
    if (item == DefaultComboBoxItem)
    {
        Algorithm = CompressAlgorithm::NullAlgorithm;
    }
    else if (item == XpressComboBoxItem)
    {
        Algorithm = CompressAlgorithm::Xpress;
    }
    else if (item == XpressHuffComboBoxItem)
    {
        Algorithm = CompressAlgorithm::XpressHuff;
    }
    else if (item == MszipComboBoxItem)
    {
        Algorithm = CompressAlgorithm::Mszip;
    }
    else if (item == LzmsComboBoxItem)
    {
        Algorithm = CompressAlgorithm::Lzms;
    }
    else 
    {
        Algorithm = CompressAlgorithm::InvalidAlgorithm;
    }

    return (int) Algorithm;
}
