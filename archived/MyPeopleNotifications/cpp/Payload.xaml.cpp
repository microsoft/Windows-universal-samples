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
#include "Payload.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Notifications;
using namespace Windows::UI::Xaml;
//using namespace Windows::UI::Xaml::Controls;
//using namespace Windows::UI::Xaml::Controls::Primitives;
//using namespace Windows::UI::Xaml::Data;
//using namespace Windows::UI::Xaml::Input;
//using namespace Windows::UI::Xaml::Media;
//using namespace Windows::UI::Xaml::Navigation;

DependencyProperty^ Payload::s_markupProperty = nullptr;
DependencyProperty^ Payload::s_formattedMarkupProperty = nullptr;

Payload::Payload()
{
    RegisterDependencyProperties();
    InitializeComponent();
}

void Payload::RegisterDependencyProperties()
{
    if (s_markupProperty == nullptr)
    {
        s_markupProperty = DependencyProperty::Register("Markup", String::typeid, Payload::typeid, nullptr);
    }

    if (s_formattedMarkupProperty == nullptr)
    {
        s_formattedMarkupProperty = DependencyProperty::Register("FormattedMarkup", String::typeid, Payload::typeid, nullptr);
    }
}

void replace_all(std::wstring& s, const std::wstring& original, const std::wstring& replacement)
{
    std::wstring::size_type pos = 0;
    while ((pos = s.find(original, pos)) != std::wstring::npos)
    {
        s.replace(pos, original.length(), replacement);
        pos += replacement.length();
    }
}

void Payload::SetFormattedMarkup(String^ markup)
{
    std::wstring result(markup->Data(), markup->Length());
    replace_all(result, L"{email}", Constants::c_ContactEmail.Data());
    replace_all(result, L"{tel}", Constants::c_ContactPhone.Data());
    replace_all(result, L"{remoteid}", Constants::c_ContactRemoteId.Data());
    FormattedMarkup = ref new String(result.c_str(), result.size());
}

void Payload::OnSendClicked(Object^ sender, RoutedEventArgs^ e)
{
    XmlDocument^ toastXml = ref new XmlDocument();
    toastXml->LoadXml(this->FormattedMarkup);
    ToastNotification^ notification = ref new ToastNotification(toastXml);
    ToastNotificationManager::CreateToastNotifier()->Show(notification);
}
