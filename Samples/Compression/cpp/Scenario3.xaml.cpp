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
// Scenario3.xaml.cpp
// Implementation of the Scenario3 class
//

#include "pch.h"
#include "Scenario3.xaml.h"
#include "CompressionUtils.h"

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::Compression;

using namespace SDKTemplate::Compression;

using namespace concurrency;

Scenario3::Scenario3()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

void ::SDKTemplate::Compression::Scenario3::DoScenario(Windows::Storage::Compression::CompressAlgorithm Algorithm)
{
    Progress->Text = "";
    rootPage->NotifyUser("Working...", NotifyType::StatusMessage);

    auto context = std::make_shared<ScenarioContext>();
    context->raStream = ref new InMemoryRandomAccessStream();

    auto picker = ref new Pickers::FileOpenPicker();
    picker->FileTypeFilter->Append("*");

    // First pick a test file and open it for reading
    create_task(picker->PickSingleFileAsync()).then([=](StorageFile^ OriginalFile)
    {
        if (!OriginalFile)
        {
            throw std::runtime_error("No file has been selected");
        }

        Progress->Text += "File \"" + OriginalFile->Name + "\" has been picked\n";

        return OriginalFile->OpenAsync(FileAccessMode::Read);
    })

    // Then read the whole file into memory buffer
    .then([=](IRandomAccessStream^ OriginalStream)
    {
        return ReadStreamTask(OriginalStream, context->originalData).RunTask();
    })

    // Then compress data using "classic" Compression API
    .then([=](size_t BytesRead)
    {
        Progress->Text += BytesRead + " bytes have been read from disk\n";

        // IMPORTANT:
        // Tasks, that are created over IAsyncOperation (and the likes) or descended from them
        // have their continuations scheduled to the same ASTA apartment effectively blocking
        // the GUI for the length of processing continuation. Create an independent task here
        // instead of adding continuation on the upper level task (which descends from
        // "apartment-aware" task) so we will not block the GUI (ASTA) thread while compressing.
        return task<size_t>([=]
        {
            // We are by convention the only thread to touch context->compressedData and
            // context->decompressedData at this point - no synchronization necessary

            COMPRESSOR_HANDLE compressorHandle = nullptr;

            try
            {
                // Enumaration values of Windows::Storage::Compression::CompressAlgorithm are
                // guaranteed to match values from compressapi.h
                ULONG compressAlgorithm = static_cast<ULONG>(Algorithm);
                SIZE_T compressedDataSize = 0;
                std::vector<byte> compressedData;

                // Compress algorithm should always be explicit for "classic" Compression API
                if (compressAlgorithm == COMPRESS_ALGORITHM_INVALID)
                {
                    compressAlgorithm = COMPRESS_ALGORITHM_XPRESS;
                }

                if (!CreateCompressor(compressAlgorithm, nullptr, &compressorHandle))
                {
                    throw std::runtime_error("Cannot create compressor");
                }

                if (!Compress(compressorHandle, context->originalData.data(), context->originalData.size(), nullptr, 0, &compressedDataSize) &&
                    (GetLastError() != ERROR_INSUFFICIENT_BUFFER))
                {
                    throw std::runtime_error("Cannot get decompressed data size");
                }

                compressedData.resize(compressedDataSize);

                if (!Compress(compressorHandle,
                                context->originalData.data(),
                                context->originalData.size(),
                                compressedData.data(),
                                compressedData.size(),
                                &compressedDataSize))
                {
                    throw std::runtime_error("Cannot decompress data");
                }
                compressedData.resize(compressedDataSize);

                CloseCompressor(compressorHandle);

                context->compressedData = std::move(compressedData);
                return context->compressedData.size();
            }
            catch (...)
            {
                if (compressorHandle != nullptr)
                {
                    CloseDecompressor(compressorHandle);
                }

                throw;
            }
        });
    })

    // Then write compressed data into in-memory stream
    .then([=](size_t CompressedSize)
    {
        Progress->Text += "Compressed into " + CompressedSize + " bytes\n";

        auto dataWriter = ref new DataWriter(context->raStream);
        context->writer = dataWriter;
        auto data = ref new Platform::Array<byte>(context->compressedData.data(), (unsigned int)context->compressedData.size());
        dataWriter->WriteBytes(data);

        return dataWriter->StoreAsync();
    })

    // Then decompress in-memory stream into vector
    .then([=](size_t CompressedSize)
    {
        auto decompressor = ref new Decompressor(context->raStream->GetInputStreamAt(0));

        Progress->Text += "Decompressor object has been created\n";

        return ReadStreamTask(decompressor, context->decompressedData).RunTask();
    })

    // Then verify if any data has been lost in action
    .then([=](size_t BytesRead)
    {
        Progress->Text += BytesRead + " bytes have been decompressed\n";

        if (context->decompressedData.size() != BytesRead)
        {
            throw std::runtime_error("Decompressed data size doesn't match number of bytes read from in-memory stream");
        }

        if (context->originalData.size() != context->decompressedData.size())
        {
            throw std::runtime_error("Decompressed data size doesn't match original one");
        }

        if (!std::equal(context->originalData.begin(), context->originalData.end(), context->decompressedData.begin()))
        {
            throw std::runtime_error("Decompressed data doesn't match original one");
        }

        Progress->Text += "Decompressed data matches original\n";
    })

    // Final task based continuation is used to handle exceptions in the chain above
    .then([=](task<void> FinalContinuation)
    {
        try
        {
            // Transport all exceptions to this thread. This task is guaranteed to be completed by now.
            FinalContinuation.get();

            Progress->Text += "All done\n";
            rootPage->NotifyUser("Finished", NotifyType::StatusMessage);
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
    });
}

void ::SDKTemplate::Compression::Scenario3::CompressFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

void ::SDKTemplate::Compression::Scenario3::CompressAlgorithmComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

void ::SDKTemplate::Compression::Scenario3::UpdateDescriptionVisibility(ComboBoxItem^ item, Windows::UI::Xaml::Visibility visibility)
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

int ::SDKTemplate::Compression::Scenario3::CompressAlgorithmFromComboBoxItem(ComboBoxItem^ item)
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
