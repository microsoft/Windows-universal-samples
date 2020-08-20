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
#include "Scenario1_XmlReader.h"
#include "Scenario1_XmlReader.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_XmlReader::Scenario1_XmlReader()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1_XmlReader::ReadXmlClick(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        std::wostringstream result;
        try
        {
            StorageFolder installFolder = Package::Current().InstalledLocation();
            StorageFile file = co_await installFolder.GetFileAsync(L"Stocks.xml");
            IRandomAccessStream randomAccessReadStream = co_await file.OpenAsync(FileAccessMode::Read);
            com_ptr<IStream> readStream = capture<IStream>(::CreateStreamOverRandomAccessStream, winrt::get_unknown(randomAccessReadStream));
            ReadXml(result, readStream);
        }
        catch (hresult_error const& e)
        {
            result << L"Exception occured while reading the xml file: " << std::wstring_view(e.message()) <<
                L"\nError code: " + e.code();
        }
        XmlTextbox().Text(result.str());
    }

    void Scenario1_XmlReader::ReadXml(std::wostream& result, com_ptr<IStream> const& readStream)
    {
        XmlNodeType nodeType;

        com_ptr<IXmlReader> reader;
        check_hresult(::CreateXmlReader(IID_PPV_ARGS(&reader), nullptr));
        check_hresult(reader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit));
        check_hresult(reader->SetInput(readStream.get()));

        // Reads until there are no more nodes.
        while (check_enumerator(reader->Read(&nodeType)))
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
                result << L"XmlDeclaration\n";
                ReadAttributes(result, reader);
                break;

            case XmlNodeType_Element:
                check_hresult(reader->GetPrefix(&prefix, &prefixSize));
                check_hresult(reader->GetLocalName(&localName, &localNameSize));
                result << L"Element: ";
                if (prefixSize > 0)
                {
                    result << std::wstring_view(prefix, prefixSize) << L':';
                }
                result << std::wstring_view(localName, localNameSize) << L"\n";

                ReadAttributes(result, reader);
                if (reader->IsEmptyElement())
                {
                    result << L" (empty)";
                }
                break;

            case XmlNodeType_EndElement:
                check_hresult(reader->GetPrefix(&prefix, &prefixSize));
                check_hresult(reader->GetLocalName(&localName, &localNameSize));
                result << L"End Element: ";
                if (prefixSize > 0)
                {
                    result << std::wstring_view(prefix, prefixSize) << L':';
                }
                result << std::wstring_view(localName, localNameSize) << L"\n";
                break;

            case XmlNodeType_Text:
                check_hresult(reader->GetValue(&value, &valueSize));
                result << L"Text: >" << std::wstring_view(value, valueSize) << L"<\n";
                break;

            case XmlNodeType_CDATA:
                check_hresult(reader->GetValue(&value, &valueSize));
                result << L"CDATA: " << std::wstring_view(value, valueSize) << L"\n";
                break;

            case XmlNodeType_ProcessingInstruction:
                check_hresult(reader->GetLocalName(&localName, &localNameSize));
                check_hresult(reader->GetValue(&value, &valueSize));
                result << L"Processing Instruction name:" << std::wstring_view(localName, localNameSize) << L", ";
                result << L"value:" << std::wstring_view(value, valueSize) << L"\n";
                break;

            case XmlNodeType_Comment:
                check_hresult(reader->GetValue(&value, &valueSize));
                result << L"Comment: " << std::wstring_view(value, valueSize) << L"\n";
                break;

            case XmlNodeType_DocumentType:
                result << L"DOCTYPE is not printed\n";
                break;

            case XmlNodeType_Whitespace:
                check_hresult(reader->GetValue(&value, &valueSize));
                result << L"WhiteSpace: " << std::wstring_view(value, valueSize) << L"\n";
                break;

            default:
                break;
            }
        }
    }

    void Scenario1_XmlReader::ReadAttributes(std::wostream& result, com_ptr<IXmlReader> const& reader)
    {
        check_hresult(reader->MoveToFirstAttribute());

        do
        {
            if (!reader->IsDefault())
            {
                PCWSTR prefix = nullptr;
                PCWSTR localName = nullptr;
                PCWSTR value = nullptr;

                UINT prefixSize = 0;
                UINT localNameSize = 0;
                UINT valueSize = 0;

                check_hresult(reader->GetPrefix(&prefix, &prefixSize));
                check_hresult(reader->GetLocalName(&localName, &localNameSize));
                check_hresult(reader->GetValue(&value, &valueSize));

                result << L"Attr: ";
                if (prefixSize > 0)
                {
                    result << std::wstring_view(prefix, prefixSize) << L':';
                }
                result << std::wstring_view(localName, localNameSize) <<
                    L"=\"" << std::wstring_view(value, valueSize) << L"\"\n";
            }
        } while (check_enumerator(reader->MoveToNextAttribute()));
    }
}
