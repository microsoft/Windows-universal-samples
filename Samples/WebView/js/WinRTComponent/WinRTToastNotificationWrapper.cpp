// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "pch.h"
#include "WinRTToastNotificationWrapper.h"


using namespace WinRTComponent;
using namespace Platform;

using namespace Windows::UI::Notifications;
using namespace Windows::Data::Xml::Dom;

WinRTToastNotificationWrapper::WinRTToastNotificationWrapper()
{
}

void WinRTToastNotificationWrapper::toastMessage(Platform::String^ message)
{
    ToastTemplateType toastTemplate = ToastTemplateType::ToastText01;
    XmlDocument^ toastXml = ToastNotificationManager::GetTemplateContent(toastTemplate);

    XmlNodeList^ toastTextElements = toastXml->GetElementsByTagName("text");
    toastTextElements->Item(0)->InnerText = message;

    IXmlNode^ toastNode = toastXml->SelectSingleNode("/toast");
    static_cast<XmlElement^>(toastNode)->SetAttribute("duration", "long");

    static_cast<XmlElement^>(toastNode)->SetAttribute("launch", "{\"type\":\"toast\",\"param1\":\"12345\",\"param2\":\"67890\"}");

    ToastNotification^ toast = ref new ToastNotification(toastXml);

    ToastNotificationManager::CreateToastNotifier()->Show(toast);
}
