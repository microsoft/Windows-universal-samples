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
#include "Scenario2_XmlWriter.h"
#include "Scenario2_XmlWriter.g.cpp"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_XmlWriter::Scenario2_XmlWriter()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario2_XmlWriter::DoSomething_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        hstring result;
        try
        {
            StorageFolder localFolder = ApplicationData::Current().LocalFolder();
            StorageFile file = co_await localFolder.CreateFileAsync(L"XmlWriterOutput.xml", CreationCollisionOption::OpenIfExists);
            IRandomAccessStream randomAccessWriteStream = co_await file.OpenAsync(FileAccessMode::ReadWrite);
            com_ptr<IStream> writeStream = capture<IStream>(::CreateStreamOverRandomAccessStream, winrt::get_unknown(randomAccessWriteStream));
            WriteXml(writeStream);
            result = L"File is written to " + file.Path() + L" successfully";
        }
        catch (hresult_error const& e)
        {
            result = L"Exception occured while writing the xml file: " + e.message() +
                L"\nError code: " + to_hstring(e.code());
        }
        OutputTextBlock1().Text(result);
    }

    void Scenario2_XmlWriter::WriteXml(com_ptr<IStream> const& writeStream)
    {
        com_ptr<IXmlWriter> writer;
        check_hresult(::CreateXmlWriter(IID_PPV_ARGS(&writer), nullptr));
        check_hresult(writer->SetOutput(writeStream.get()));
        check_hresult(writer->SetProperty(XmlWriterProperty_Indent, TRUE));

        check_hresult(writer->WriteStartDocument(XmlStandalone_Omit));
        check_hresult(writer->WriteDocType(L"root", nullptr, nullptr, nullptr));
        check_hresult(writer->WriteProcessingInstruction(L"xml-stylesheet",
            L"href=\"mystyle.css\" title=\"Compact\" type=\"text/css\""));
        check_hresult(writer->WriteStartElement(nullptr, L"root", nullptr));
        check_hresult(writer->WriteStartElement(nullptr, L"sub", nullptr));
        check_hresult(writer->WriteAttributeString(nullptr, L"myAttr", nullptr, L"1234"));
        check_hresult(writer->WriteString(L"Markup is <escaped> for this string"));
        check_hresult(writer->WriteFullEndElement());
        check_hresult(writer->WriteStartElement(nullptr, L"anotherChild", nullptr));
        check_hresult(writer->WriteString(L"some text"));
        check_hresult(writer->WriteFullEndElement());
        check_hresult(writer->WriteWhitespace(L"\n"));
        check_hresult(writer->WriteCData(L"This is CDATA text."));
        check_hresult(writer->WriteWhitespace(L"\n"));
        check_hresult(writer->WriteStartElement(nullptr, L"containsCharacterEntity", nullptr));
        check_hresult(writer->WriteCharEntity(L'a'));
        check_hresult(writer->WriteFullEndElement());
        check_hresult(writer->WriteWhitespace(L"\n"));
        check_hresult(writer->WriteStartElement(nullptr, L"containsChars", nullptr));
        check_hresult(writer->WriteChars(L"abcdefghijklm", 5));
        check_hresult(writer->WriteFullEndElement());
        check_hresult(writer->WriteWhitespace(L"\n"));
        check_hresult(writer->WriteStartElement(nullptr, L"containsName", nullptr));
        check_hresult(writer->WriteName(L"myName"));
        check_hresult(writer->WriteEndElement());
        check_hresult(writer->WriteWhitespace(L"\n"));
        check_hresult(writer->WriteStartElement(nullptr, L"containsNmToken", nullptr));
        check_hresult(writer->WriteNmToken(L"myNmToken"));
        check_hresult(writer->WriteEndElement());
        check_hresult(writer->WriteWhitespace(L"\n"));
        check_hresult(writer->WriteComment(L"This is a comment"));
        check_hresult(writer->WriteWhitespace(L"\n"));
        check_hresult(writer->WriteRaw(L"<elementWrittenRaw/>"));
        check_hresult(writer->WriteWhitespace(L"\n"));
        check_hresult(writer->WriteRawChars(L"<rawCharacters/>", 16));
        check_hresult(writer->WriteWhitespace(L"\n"));
        check_hresult(writer->WriteElementString(nullptr, L"myElement", nullptr, L"myValue"));
        check_hresult(writer->WriteFullEndElement());
        check_hresult(writer->WriteWhitespace(L"\n"));
        check_hresult(writer->WriteEndDocument());
    }
}
