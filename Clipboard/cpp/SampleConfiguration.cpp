// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

Array<Scenario>^ MainPage::scenariosInner = ref new Array<Scenario>
{
    { "Copy and paste text",        "SDKTemplate.CopyText" },
    { "Copy and paste an image",    "SDKTemplate.CopyImage" },
    { "Copy and paste files",       "SDKTemplate.CopyFiles" },
    { "Other Clipboard operations", "SDKTemplate.OtherScenarios" }
};

bool MainPage::windowActive = true;

void MainPage::NotifyUserBackgroundThread(String^ message, NotifyType type)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
        [this, message, type]()
    {
        NotifyUser(message, type);
    }));
}

void MainPage::EnableClipboardContentChangedNotifications(bool enable)
{
    if (enable)
    {
        contentChangedToken = DataTransfer::Clipboard::ContentChanged += ref new EventHandler<Object^>(this, &MainPage::OnClipboardChanged);
        windowActivatedToken = Window::Current->Activated += ref new WindowActivatedEventHandler(this, &MainPage::OnWindowActivated);
    }
    else
    {
        DataTransfer::Clipboard::ContentChanged -= contentChangedToken;
        Window::Current->Activated -= windowActivatedToken;
    }
}

String^ MainPage::BuildClipboardFormatsOutputString()
{
    DataPackageView^ clipboardContent = DataTransfer::Clipboard::GetContent();
    String^ clipboardFormats;
    if ((clipboardContent != nullptr) && (clipboardContent->AvailableFormats->Size > 0))
    {
        clipboardFormats = "Available formats in the clipboard:";
        for (const auto& format : clipboardContent->AvailableFormats)
        {
            clipboardFormats += "\n" + " * " + format;
        }
    }
    else
    {
        clipboardFormats = "The clipboard is empty";
    }
    return clipboardFormats;
}

void MainPage::DisplayChangedFormats()
{
    String^ output = "Clipboard content has changed!\n";
    output += BuildClipboardFormatsOutputString();
    NotifyUser(output, NotifyType::StatusMessage);
}

void MainPage::HandleClipboardChanged()
{
    if (windowActive)
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

void MainPage::OnWindowActivated(Object^ sender, WindowActivatedEventArgs^ e)
{
    windowActive = (e->WindowActivationState != CoreWindowActivationState::Deactivated);
    if (needToPrintClipboardFormat)
    {
        HandleClipboardChanged();
    }
}

void MainPage::OnClipboardChanged(Object^ sender, Object^ e)
{
    HandleClipboardChanged();
}
