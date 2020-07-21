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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::UI::Core;
using namespace Windows::UI::Notifications;
using namespace Windows::UI::Xaml;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Clipboard C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Copy and paste text", xaml_typename<SDKTemplate::CopyText>() },
    Scenario{ L"Copy and paste an image", xaml_typename<SDKTemplate::CopyImage>() },
    Scenario{ L"Copy and paste files", xaml_typename<SDKTemplate::CopyFiles>() },
    Scenario{ L"History and roaming", xaml_typename<SDKTemplate::HistoryAndRoaming>() },
    Scenario{ L"Other Clipboard operations", xaml_typename<SDKTemplate::OtherScenarios>() },
});

event_token SampleState::clipboardContentChangedToken;
event_token SampleState::activatedToken;
bool SampleState::isClipboardContentChangedEnabled = false;
bool SampleState::needToPrintClipboardFormat = false;
bool SampleState::isApplicationWindowActive = true;

ToastNotification SampleState::DisplayToast(hstring const& message)
{
    static constexpr wchar_t xml[] =
        LR"(<toast activationType='foreground'>
               <visual>
                   <binding template='ToastGeneric'/>
               </visual>
           </toast>)";

    XmlDocument doc;
    doc.LoadXml(xml);

    auto binding = doc.SelectSingleNode(L"//binding");

    auto el = doc.CreateElement(L"text");
    el.InnerText(MainPage::FEATURE_NAME());
    binding.AppendChild(el);

    el = doc.CreateElement(L"text");
    el.InnerText(message);
    binding.AppendChild(el);

    ToastNotification toast(doc);

    ToastNotificationManager::CreateToastNotifier().Show(toast);
    return toast;
}

bool SampleState::IsClipboardContentChangedEnabled()
{
    return isClipboardContentChangedEnabled;
}

bool SampleState::EnableClipboardContentChangedNotifications(bool enable)
{
    if (isClipboardContentChangedEnabled == enable)
    {
        return false;
    }

    if (enable)
    {
        clipboardContentChangedToken = Clipboard::ContentChanged(OnClipboardChanged);
        activatedToken = Window::Current().Activated(OnWindowActivated);
    }
    else
    {
        Clipboard::ContentChanged(clipboardContentChangedToken);
        Window::Current().Activated(activatedToken);
    }
    return true;
}

hstring SampleState::BuildClipboardFormatsOutputString()
{
    std::wostringstream output;
    AddClipboardFormatsOutputString(output);
    return hstring{ output.str() };
}

void SampleState::AddClipboardFormatsOutputString(std::wostringstream& output)
{
    DataPackageView clipboardContent = Clipboard::GetContent();

    if (clipboardContent && clipboardContent.AvailableFormats().Size() > 0)
    {
        output << L"Available formats in the clipboard:";
        for (auto&& format : clipboardContent.AvailableFormats())
        {
            output << std::endl << L" * " << std::wstring_view(format);
        }
    }
    else
    {
        output << L"The clipboard is empty";
    }
}

void SampleState::DisplayChangedFormats()
{
    std::wostringstream output;
    output << L"Clipboard content has changed!" << std::endl;
    AddClipboardFormatsOutputString(output);
    MainPage::Current().NotifyUser(output.str(), NotifyType::StatusMessage);
}

void SampleState::HandleClipboardChanged()
{
    if (isApplicationWindowActive)
    {
        DisplayChangedFormats();
    }
    else
    {
        // Background applications can't access clipboard
        // Deferring processing of update notification until the application returns to foreground
        needToPrintClipboardFormat = true;
    }
}
void SampleState::OnWindowActivated(IInspectable const&, WindowActivatedEventArgs const& e)
{
    isApplicationWindowActive = (e.WindowActivationState() != CoreWindowActivationState::Deactivated);
    if (needToPrintClipboardFormat)
    {
        // The clipboard was updated while the sample was in the background. If the sample is now in the foreground, 
        // display the new content. 
        HandleClipboardChanged();
    }
}

void SampleState::OnClipboardChanged(IInspectable const&, IInspectable const&)
{
    DisplayToast(L"Clipboard changed.");
    HandleClipboardChanged();
}

