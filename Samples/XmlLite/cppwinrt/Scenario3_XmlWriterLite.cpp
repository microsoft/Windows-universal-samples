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
#include "Scenario3_XmlWriterLite.h"
#include "Scenario3_XmlWriterLite.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_XmlWriterLite::Scenario3_XmlWriterLite()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario3_XmlWriterLite::DoSomething_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        hstring result;
        try
        {
            StorageFolder installFolder = Package::Current().InstalledLocation();
            StorageFile inputFile = co_await installFolder.GetFileAsync(L"Stocks.xml");
            IRandomAccessStream randomAccessReadStream = co_await inputFile.OpenAsync(FileAccessMode::Read);
            com_ptr<IStream> readStream = capture<IStream>(::CreateStreamOverRandomAccessStream, winrt::get_unknown(randomAccessReadStream));

            StorageFolder localFolder = ApplicationData::Current().LocalFolder();
            StorageFile outputFile = co_await localFolder.CreateFileAsync(L"XmlWriterLiteOutput.xml", CreationCollisionOption::ReplaceExisting);
            IRandomAccessStream randomAccessWriteStream = co_await outputFile.OpenAsync(FileAccessMode::ReadWrite);
            com_ptr<IStream> writeStream = capture<IStream>(::CreateStreamOverRandomAccessStream, winrt::get_unknown(randomAccessWriteStream));

            ReadAndWrite(readStream, writeStream);

            result = L"File is written to " + outputFile.Path() + L" successfully";
        }
        catch (hresult_error const& e)
        {
            result = L"Exception occured while writing the xml file: " + e.message() +
                L"\nError code: " + to_hstring(e.code());
        }
        OutputTextBlock1().Text(result);
    }

    void Scenario3_XmlWriterLite::ReadAndWrite(com_ptr<IStream> const& readStream, com_ptr<IStream> const& writeStream)
    {
        com_ptr<IXmlReader> reader;
        check_hresult(::CreateXmlReader(IID_PPV_ARGS(&reader), nullptr));
        check_hresult(reader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit));
        check_hresult(reader->SetInput(readStream.get()));

        com_ptr<IXmlWriterLite> writer;
        check_hresult(::CreateXmlWriter(IID_PPV_ARGS(&writer), nullptr));
        check_hresult(writer->SetOutput(writeStream.get()));

        XmlNodeType nodeType;

        while (check_enumerator(reader->Read(&nodeType)))
        {
            PCWSTR     qualifiedName = nullptr;
            PCWSTR     value = nullptr;
            UINT       qualifiedNameSize = 0;
            UINT       valueSize = 0;

            switch (nodeType)
            {
            case XmlNodeType_Element:
            {
                check_hresult(reader->GetQualifiedName(&qualifiedName, &qualifiedNameSize));
                check_hresult(writer->WriteStartElement(qualifiedName, qualifiedNameSize));
                while (check_enumerator(reader->MoveToNextAttribute()))
                {
                    check_hresult(reader->GetQualifiedName(&qualifiedName, &qualifiedNameSize));
                    check_hresult(reader->GetValue(&value, &valueSize));
                    check_hresult(writer->WriteAttributeString(qualifiedName, qualifiedNameSize, value, valueSize));
                }
                check_hresult(reader->MoveToElement());
                if (reader->IsEmptyElement())
                {
                    check_hresult(reader->GetQualifiedName(&qualifiedName, &qualifiedNameSize));
                    check_hresult(writer->WriteEndElement(qualifiedName, qualifiedNameSize));
                }
                break;
            }
            case XmlNodeType_Text:
                check_hresult(reader->GetValue(&value, nullptr));
                check_hresult(writer->WriteString(value));
                break;

            case XmlNodeType_CDATA:
                check_hresult(reader->GetValue(&value, nullptr));
                check_hresult(writer->WriteCData(value));
                break;

            case XmlNodeType_ProcessingInstruction:
                check_hresult(reader->GetQualifiedName(&qualifiedName, nullptr));
                check_hresult(reader->GetValue(&value, nullptr));
                check_hresult(writer->WriteProcessingInstruction(qualifiedName, value));
                break;

            case XmlNodeType_Comment:
                check_hresult(reader->GetValue(&value, nullptr));
                check_hresult(writer->WriteComment(value));
                break;

            case XmlNodeType_DocumentType:
            {
                PCWSTR publicId = nullptr;
                PCWSTR systemId = nullptr;

                check_hresult(reader->GetQualifiedName(&qualifiedName, nullptr));
                check_hresult(reader->GetValue(&value, nullptr));
                if (check_enumerator(reader->MoveToAttributeByName(L"PUBLIC", nullptr)))
                {
                    reader->GetValue(&publicId, nullptr);
                }
                if (check_enumerator(reader->MoveToAttributeByName(L"SYSTEM", nullptr)))
                {
                    check_hresult(reader->GetValue(&systemId, nullptr));
                }
                check_hresult(writer->WriteDocType(
                    qualifiedName,
                    publicId,
                    systemId,
                    value));
                break;
            }
            case XmlNodeType_Whitespace:
                check_hresult(reader->GetValue(&value, nullptr));
                check_hresult(writer->WriteWhitespace(value));
                break;

            case XmlNodeType_EndElement:
                check_hresult(reader->GetQualifiedName(&qualifiedName, &qualifiedNameSize));
                check_hresult(writer->WriteFullEndElement(qualifiedName, qualifiedNameSize));
                break;

            default:
                break;
            }
        }
    }
}
