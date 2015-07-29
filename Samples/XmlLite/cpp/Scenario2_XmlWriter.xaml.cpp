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
#include "Scenario2_XmlWriter.xaml.h"

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

Scenario2::Scenario2() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario2::DoSomething_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Button^ b = safe_cast<Button^>(sender);
    StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;

    create_task(folder->CreateFileAsync(L"XmlWriterOutput.xml", CreationCollisionOption::OpenIfExists)).then([this](StorageFile^ file)
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

HRESULT Scenario2::WriteXml(IRandomAccessStream^ randomAccessWriteStream)
{
    HRESULT hr = S_OK;
    ComPtr<IStream> writeStream;
    ComPtr<IXmlWriter> writer;

    ChkHr(::CreateStreamOverRandomAccessStream(randomAccessWriteStream, IID_PPV_ARGS(&writeStream)));
    ChkHr(::CreateXmlWriter(IID_PPV_ARGS(&writer), nullptr));
    ChkHr(writer->SetOutput(writeStream.Get()));
    ChkHr(writer->SetProperty(XmlWriterProperty_Indent, TRUE));

    ChkHr(writer->WriteStartDocument(XmlStandalone_Omit));
    ChkHr(writer->WriteDocType(L"root", nullptr, nullptr, nullptr));
    ChkHr(writer->WriteProcessingInstruction(L"xml-stylesheet",
        L"href=\"mystyle.css\" title=\"Compact\" type=\"text/css\""));
    ChkHr(writer->WriteStartElement(nullptr, L"root", nullptr));
    ChkHr(writer->WriteStartElement(nullptr, L"sub", nullptr));
    ChkHr(writer->WriteAttributeString(nullptr, L"myAttr", nullptr, L"1234"));
    ChkHr(writer->WriteString(L"Markup is <escaped> for this string"));
    ChkHr(writer->WriteFullEndElement());
    ChkHr(writer->WriteStartElement(nullptr, L"anotherChild", nullptr));
    ChkHr(writer->WriteString(L"some text"));
    ChkHr(writer->WriteFullEndElement());
    ChkHr(writer->WriteWhitespace(L"\n"));
    ChkHr(writer->WriteCData(L"This is CDATA text."));
    ChkHr(writer->WriteWhitespace(L"\n"));
    ChkHr(writer->WriteStartElement(nullptr, L"containsCharacterEntity", nullptr));
    ChkHr(writer->WriteCharEntity(L'a'));
    ChkHr(writer->WriteFullEndElement());
    ChkHr(writer->WriteWhitespace(L"\n"));
    ChkHr(writer->WriteStartElement(nullptr, L"containsChars", nullptr));
    ChkHr(writer->WriteChars(L"abcdefghijklm", 5));
    ChkHr(writer->WriteFullEndElement());
    ChkHr(writer->WriteWhitespace(L"\n"));
    ChkHr(writer->WriteStartElement(nullptr, L"containsName", nullptr));
    ChkHr(writer->WriteName(L"myName"));
    ChkHr(writer->WriteEndElement());
    ChkHr(writer->WriteWhitespace(L"\n"));
    ChkHr(writer->WriteStartElement(nullptr, L"containsNmToken", nullptr));
    ChkHr(writer->WriteNmToken(L"myNmToken"));
    ChkHr(writer->WriteEndElement());
    ChkHr(writer->WriteWhitespace(L"\n"));
    ChkHr(writer->WriteComment(L"This is a comment"));
    ChkHr(writer->WriteWhitespace(L"\n"));
    ChkHr(writer->WriteRaw(L"<elementWrittenRaw/>"));
    ChkHr(writer->WriteWhitespace(L"\n"));
    ChkHr(writer->WriteRawChars(L"<rawCharacters/>", 16));
    ChkHr(writer->WriteWhitespace(L"\n"));
    ChkHr(writer->WriteElementString(nullptr, L"myElement", nullptr, L"myValue"));
    ChkHr(writer->WriteFullEndElement());
    ChkHr(writer->WriteWhitespace(L"\n"));
    ChkHr(writer->WriteEndDocument());

    return hr;
}