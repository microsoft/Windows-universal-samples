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
#include "Scenario3_XmlWriterLite.xaml.h"

using namespace SDKTemplate::XmlLite;

using namespace Concurrency;
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

Scenario3::Scenario3() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario3::DoSomething_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Button^ b = safe_cast<Button^>(sender);
    StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;

    create_task(folder->CreateFileAsync(L"XmlWriterLiteOutput.xml", CreationCollisionOption::ReplaceExisting)).then([this](StorageFile^ file)
    {
        create_task(file->OpenAsync(FileAccessMode::ReadWrite)).then([file, this](IRandomAccessStream^ writeStream)
        {
            HRESULT hr = WriteXml(writeStream);
            if (FAILED(hr))
            {
                OutputTextBlock1->Text = L"Exception occured while writing the xml file, and the error code is " + hr.ToString();
                rootPage->NotifyUser("Exception occured!", NotifyType::ErrorMessage);
            }
            else
            {
                OutputTextBlock1->Text = L"File is written to " + file->Path + L" successfully";
            }
        });
    });
}

HRESULT Scenario3::WriteXml(IRandomAccessStream^ randomAccessWriteStream)
{
    StorageFolder^ installFolder = Windows::ApplicationModel::Package::Current->InstalledLocation;

    create_task(installFolder->GetFileAsync("Stocks.xml")).then([](StorageFile^ file)
    {
        return file->OpenAsync(FileAccessMode::Read);
    }).then([=](IRandomAccessStream^ randomAccessReadStream)
    {
        HRESULT hr = S_OK;
        ComPtr<IStream> readStream;
        ComPtr<IStream> writeStream;
        ComPtr<IXmlReader> reader;
        ComPtr<IXmlWriterLite> writer;

        ChkHr(::CreateStreamOverRandomAccessStream(randomAccessWriteStream, IID_PPV_ARGS(&writeStream)));
        ChkHr(::CreateXmlWriter(IID_PPV_ARGS(&writer), nullptr));
        ChkHr(writer->SetOutput(writeStream.Get()));

        ChkHr(::CreateStreamOverRandomAccessStream(randomAccessReadStream, IID_PPV_ARGS(&readStream)));
        ChkHr(::CreateXmlReader(IID_PPV_ARGS(&reader), nullptr));
        ChkHr(reader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit));
        ChkHr(reader->SetInput(readStream.Get()));

        ChkHr(ReadAndWrite(reader.Get(), writer.Get()));

        return hr;
    });

    return S_FALSE;
}

HRESULT Scenario3::ReadAndWrite(IXmlReader* reader, IXmlWriterLite* writer)
{
    HRESULT     hr;
    XmlNodeType nodeType;

    while ((hr = reader->Read(&nodeType)) == S_OK)
    {
        PCWSTR     qualifiedName = nullptr;
        PCWSTR     value = nullptr;
        UINT       qualifiedNameSize = 0;
        UINT       valueSize = 0;

        switch (nodeType)
        {
        case XmlNodeType_Element:
        {
            ChkHr(reader->GetQualifiedName(&qualifiedName, &qualifiedNameSize));
            ChkHr(writer->WriteStartElement(qualifiedName, qualifiedNameSize));
            while ((hr = reader->MoveToNextAttribute()) == S_OK)
            {
                ChkHr(reader->GetQualifiedName(&qualifiedName, &qualifiedNameSize));
                ChkHr(reader->GetValue(&value, &valueSize));
                ChkHr(writer->WriteAttributeString(qualifiedName, qualifiedNameSize, value, valueSize));
            }
            ChkHr(reader->MoveToElement());
            if (reader->IsEmptyElement())
            {
                ChkHr(reader->GetQualifiedName(&qualifiedName, &qualifiedNameSize));
                ChkHr(writer->WriteEndElement(qualifiedName, qualifiedNameSize));
            }
            break;
        }
        case XmlNodeType_Text:
            ChkHr(reader->GetValue(&value, nullptr));
            ChkHr(writer->WriteString(value));
            break;

        case XmlNodeType_CDATA:
            ChkHr(reader->GetValue(&value, nullptr));
            ChkHr(writer->WriteCData(value));
            break;

        case XmlNodeType_ProcessingInstruction:
            ChkHr(reader->GetQualifiedName(&qualifiedName, nullptr));
            ChkHr(reader->GetValue(&value, nullptr));
            ChkHr(writer->WriteProcessingInstruction(qualifiedName, value));
            break;

        case XmlNodeType_Comment:
            ChkHr(reader->GetValue(&value, nullptr));
            ChkHr(writer->WriteComment(value));
            break;

        case XmlNodeType_DocumentType:
        {
            PCWSTR publicId = nullptr;
            PCWSTR systemId = nullptr;

            ChkHr(reader->GetQualifiedName(&qualifiedName, nullptr));
            ChkHr(reader->GetValue(&value, nullptr));
            if (reader->MoveToAttributeByName(L"PUBLIC", nullptr) == S_OK)
            {
                reader->GetValue(&publicId, nullptr);
            }
            if (reader->MoveToAttributeByName(L"SYSTEM", nullptr) == S_OK)
            {
                ChkHr(reader->GetValue(&systemId, nullptr));
            }
            ChkHr(writer->WriteDocType(
                qualifiedName,
                publicId,
                systemId,
                value));
            break;
        }
        case XmlNodeType_Whitespace:
            ChkHr(reader->GetValue(&value, nullptr));
            ChkHr(writer->WriteWhitespace(value));
            break;

        case XmlNodeType_EndElement:
            ChkHr(reader->GetQualifiedName(&qualifiedName, &qualifiedNameSize));
            ChkHr(writer->WriteFullEndElement(qualifiedName, qualifiedNameSize));
            break;

        default:
            break;
        }
    }

    return hr;
}