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
#include "Helpers.h"
#include "MainPage.xaml.h"
#include <PlugInFilter.h>

using namespace concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace SDKTemplate;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Web;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Web::Http::Headers;

void Helpers::WriteOutputText(Page^ page, TextBox^ output, String^ text)
{
    page->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([=]()
    {
        output->Text += text;
    }));
}

task<HttpResponseMessage^> Helpers::DisplayTextResultAsync(
    HttpResponseMessage^ response,
    TextBox^ output,
    cancellation_token token)
{
    output->Text += Helpers::SerializeHeaders(response);

    // Read content as string. We need to use use_current() with the continuations since the tasks are completed on
    // background threads and we need to run on the UI thread to update the UI.
    task<String^> readAsStringTask(response->Content->ReadAsStringAsync(), token);
    return readAsStringTask.then([=](String^ responseBodyAsText){
        // Convert all instances of <br> to newline.
        std::wstring ws = responseBodyAsText->Data();
        Helpers::ReplaceAll(ws, L"<br>", L"\n");

        output->Text += ref new String(ws.c_str());

        return response;
    }, task_continuation_context::use_current());
}

String^ Helpers::SerializeHeaders(HttpResponseMessage^ response)
{
    String^ output = "";

    UINT32 statusCode = (UINT32)response->StatusCode;
    String^ statusCodeString = statusCode.ToString();
    output += statusCodeString + " " + response->ReasonPhrase + "\n";

    output += SerializeHeaderCollection(response->Headers);
    output += SerializeHeaderCollection(response->Content->Headers);
    output += "\r\n";

    return output;
}

String^ Helpers::SerializeHeaderCollection(IIterable<IKeyValuePair<String^, String^>^>^ headers)
{
    String^ output = "";

    for each(IKeyValuePair<String^, String^>^ pair in headers)
    {
        output += pair->Key + ": " + pair->Value + "\r\n";
    }

    return output;
}

HttpClient^ Helpers::CreateHttpClient()
{
    // HttpClient functionality can be extended by plugging multiple filters together and providing
    // HttpClient with the configured filter pipeline.
    IHttpFilter^ filter = ref new HttpBaseProtocolFilter();
    filter = ref new PlugInFilter(filter); // Adds a custom header to every request and response message.
    HttpClient^ httpClient = ref new HttpClient(filter);

    // The following line sets a "User-Agent" request header as a default header on the HttpClient instance.
    // Default headers will be sent with every request sent from this HttpClient instance.
    httpClient->DefaultRequestHeaders->UserAgent->Append(ref new HttpProductInfoHeaderValue("Sample", "v8"));

    return httpClient;
}

void Helpers::ScenarioStarted(Button^ startButton, Button^ cancelButton, TextBox^ outputField)
{
    startButton->IsEnabled = false;
    cancelButton->IsEnabled = true;
    if (outputField != nullptr)
    {
        outputField->Text = "";
    }
}

void Helpers::ScenarioCompleted(Button^ startButton, Button^ cancelButton)
{
    startButton->IsEnabled = true;
    cancelButton->IsEnabled = false;
}

void Helpers::ReplaceQueryString(TextBox^ addressField, String^ newQueryString)
{
    String^ resourceAddress = addressField->Text;

    // Remove previous query string.
    std::wstring::size_type questionMarkIndex = IndexOf(resourceAddress, '?');
    if (questionMarkIndex != std::wstring::npos)
    {
        resourceAddress = Substring(resourceAddress, 0, questionMarkIndex);
    }

    addressField->Text = resourceAddress + newQueryString;
}

String^ Helpers::Trim(String^ s)
{
    const char16* first = s->Begin();
    const char16* last = s->End();

    while (first != last && iswspace(*first))
    {
        ++first;
    }

    while (first != last && iswspace(last[-1]))
    {
        --last;
    }

    return ref new String(first, (int)(last - first));
}

void Helpers::ReplaceAll(
    std::wstring& value,
    _In_z_ const char16* from,
    _In_z_ const char16* to)
{
    auto fromLength = wcslen(from);
    std::wstring::size_type offset;

    for (offset = value.find(from); offset != std::wstring::npos; offset = value.find(from, offset))
    {
        value.replace(offset, fromLength, to);
    }
}

std::wstring::size_type Helpers::IndexOf(String^ s, const wchar_t value)
{
    std::wstring sourceString(s->Data());
    return sourceString.find(value);
}

String^ Helpers::Substring(String^ s, std::wstring::size_type startIndex, std::wstring::size_type length)
{
    std::wstring sourceString(s->Data());
    std::wstring substring = sourceString.substr(startIndex, length);
    return ref new String(substring.data());
}
