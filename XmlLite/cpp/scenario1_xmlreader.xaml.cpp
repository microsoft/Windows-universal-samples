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
#include "Scenario1_XmlReader.xaml.h"

using namespace SDKTemplate::XmlLite;

using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario1::Scenario1() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario1::ReadXmlClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Button^ b = safe_cast<Button^>(sender);
    StorageFolder^ installFolder = Windows::ApplicationModel::Package::Current->InstalledLocation;

    create_task(installFolder->GetFileAsync("Stocks.xml")).then([](StorageFile^ file)
    {
        return file->OpenAsync(FileAccessMode::Read);
    }).then([this](IRandomAccessStream^ readStream)
    {
        HRESULT hr = ReadXml(readStream);
        if (FAILED(hr))
        {
            XmlTextbox->Text = "Exception occured while reading the xml file, and the error code is " + hr.ToString();
            rootPage->NotifyUser("Exception occured!", NotifyType::ErrorMessage);
        }
    });
}

HRESULT Scenario1::ReadXml(IRandomAccessStream^ randomAccessReadStream)
{
    HRESULT hr;
    ComPtr<IStream> readStream;
    ComPtr<IXmlReader> reader;
    XmlNodeType nodeType;

    ChkHr(::CreateStreamOverRandomAccessStream(randomAccessReadStream, IID_PPV_ARGS(&readStream)));
    ChkHr(::CreateXmlReader(IID_PPV_ARGS(&reader), nullptr));
    ChkHr(reader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit));
    ChkHr(reader->SetInput(readStream.Get()));

    // Reads until there are no more nodes.
    while (S_OK == (hr = reader->Read(&nodeType)))
    {
        PCWSTR prefix = nullptr;
        PCWSTR localName = nullptr;
        PCWSTR value = nullptr;

        UINT prefixSize = 0;
        UINT localNameSize = 0;
        UINT valueSize = 0;

        switch (nodeType)
        {
        case XmlNodeType_XmlDeclaration:
            XmlTextbox->Text += L"XmlDeclaration\n";
            ChkHr(ReadAttributes(reader.Get()));
            break;

        case XmlNodeType_Element:
            ChkHr(reader->GetPrefix(&prefix, &prefixSize));
            ChkHr(reader->GetLocalName(&localName, &localNameSize));
            if (prefixSize > 0)
            {
                XmlTextbox->Text += L"Element: ";
                ChkHr(ConcatToOutput(prefix, prefixSize));
                ChkHr(ConcatToOutput(localName, localNameSize));
                XmlTextbox->Text += L"\n";
            }
            else
            {
                XmlTextbox->Text += L"Element: ";
                ChkHr(ConcatToOutput(localName, localNameSize));
                XmlTextbox->Text += L"\n";
            }
            ChkHr(ReadAttributes(reader.Get()));

            if (reader->IsEmptyElement())
            {
                XmlTextbox->Text += L" (empty)";
            }
            break;

        case XmlNodeType_EndElement:
            ChkHr(reader->GetPrefix(&prefix, &prefixSize));
            ChkHr(reader->GetLocalName(&localName, &localNameSize));
            if (prefixSize > 0)
            {
                XmlTextbox->Text += L"End Element: ";
                ChkHr(ConcatToOutput(prefix, prefixSize));
                XmlTextbox->Text += L":";
                ChkHr(ConcatToOutput(localName, localNameSize));
                XmlTextbox->Text += L"\n";
            }
            else
            {
                XmlTextbox->Text += L"End Element: ";
                ChkHr(ConcatToOutput(localName, localNameSize));
                XmlTextbox->Text += L"\n";
            }
            break;

        case XmlNodeType_Text:
            ChkHr(reader->GetValue(&value, &valueSize));
            XmlTextbox->Text += L"Text: >";
            ChkHr(ConcatToOutput(value, valueSize));
            XmlTextbox->Text += L"<\n";
            break;

        case XmlNodeType_CDATA:
            ChkHr(reader->GetValue(&value, &valueSize));
            XmlTextbox->Text += L"CDATA: ";
            XmlTextbox->Text += ref new String(value, valueSize);
            XmlTextbox->Text += L"\n";
            break;

        case XmlNodeType_ProcessingInstruction:
            ChkHr(reader->GetLocalName(&localName, &localNameSize));
            ChkHr(reader->GetValue(&value, &valueSize));
            XmlTextbox->Text += L"Processing Instruction name:";
            ChkHr(ConcatToOutput(localName, localNameSize));
            XmlTextbox->Text += L"value:";
            ChkHr(ConcatToOutput(value, valueSize));
            XmlTextbox->Text += L"\n";
            break;

        case XmlNodeType_Comment:
            ChkHr(reader->GetValue(&value, &valueSize));
            XmlTextbox->Text += L"Comment: ";
            ChkHr(ConcatToOutput(value, valueSize));
            XmlTextbox->Text += L"\n";
            break;

        case XmlNodeType_DocumentType:
            XmlTextbox->Text += L"DOCTYPE is not printed\n";
            break;

        case XmlNodeType_Whitespace:
            ChkHr(reader->GetValue(&value, &valueSize));
            XmlTextbox->Text += L"WhiteSpace: ";
            ChkHr(ConcatToOutput(value, valueSize));
            XmlTextbox->Text += L"\n";
            break;

        default:
            break;
        }
    }
    return hr;
}

HRESULT Scenario1::ReadAttributes(IXmlReader* reader)
{
    HRESULT hr = S_OK;

    ChkHr(reader->MoveToFirstAttribute());

    for (;;)
    {
        if (!reader->IsDefault())
        {
            PCWSTR prefix = nullptr;
            PCWSTR localName = nullptr;
            PCWSTR value = nullptr;

            UINT prefixSize = 0;
            UINT localNameSize = 0;
            UINT valueSize = 0;

            ChkHr(reader->GetPrefix(&prefix, &prefixSize));
            ChkHr(reader->GetLocalName(&localName, &localNameSize));
            ChkHr(reader->GetValue(&value, &valueSize));

            if (prefixSize > 0)
            {
                XmlTextbox->Text += L"Attr: ";
                ChkHr(ConcatToOutput(prefix, prefixSize));
                XmlTextbox->Text += L":";
                ChkHr(ConcatToOutput(localName, localNameSize));
                XmlTextbox->Text += L"=\"";
                ChkHr(ConcatToOutput(value, valueSize));
                XmlTextbox->Text += L"\"\n";
            }
            else
            {
                XmlTextbox->Text += L"Attr: ";
                ChkHr(ConcatToOutput(localName, localNameSize));
                XmlTextbox->Text += L"=\"";
                ChkHr(ConcatToOutput(value, valueSize));
                XmlTextbox->Text += L"\"\n";
            }
        }

        if (S_OK != (hr = reader->MoveToNextAttribute()))
            break;
    }

    return hr;
}

HRESULT Scenario1::ConcatToOutput(PCWSTR str, UINT strLen)
{
    HRESULT hr = S_OK;

    try
    {
        XmlTextbox->Text += ref new String(str, strLen);
    }
    catch (Platform::Exception^ e)
    {
        hr = e->HResult;
    }

    return hr;
}