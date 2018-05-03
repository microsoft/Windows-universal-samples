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
#include "Scenario2.xaml.h"

using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace concurrency;

Scenario2::Scenario2()
{
    // This sample UI is interested in events from
    // the LoggingSessionScenario class so the UI can be updated.
    LoggingScenario->StatusChanged += ref new StatusChangedHandler(this, &Scenario2::OnStatusChanged);
    InitializeComponent();
}

void Scenario2::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    UpdateControls();
}

Windows::UI::Xaml::Controls::ScrollViewer^ Scenario2::FindScrollViewer(DependencyObject^ depObject)
{
    if (depObject == nullptr)
    {
        return nullptr;
    }

    int countThisLevel = Windows::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(depObject);
    if (countThisLevel <= 0)
    {
        return nullptr;
    }

    for (int childIndex = 0; childIndex < countThisLevel; childIndex++)
    {
        DependencyObject^ childDepObject = Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(depObject, childIndex);
        ScrollViewer^ sv = dynamic_cast<ScrollViewer^>(childDepObject);
        if (sv != nullptr)
        {
            return sv;
        }

        sv = FindScrollViewer(childDepObject);
        if (sv != nullptr)
        {
            return sv;
        }
    }

    return nullptr;
}

// Add a message to the UI control which displays status while the sample is running.
void Scenario2::AddMessage(Platform::String^ message)
{
    Platform::String^ messageToAdd = "";
    messageToAdd += message;
    messageToAdd += "\r\n";

    StatusMessageList->Text += messageToAdd;
    StatusMessageList->Select(StatusMessageList->Text->Length(), 0);

    ScrollViewer^ sv = FindScrollViewer(StatusMessageList);
    if (sv != nullptr)
    {
        sv->ChangeView(nullptr, StatusMessageList->ActualHeight, nullptr);
    }

}

// Call AddMessage on the UI thread.
task<void> Scenario2::AddMessageDispatch(Platform::String^ message)
{
    return create_task(Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([=]() {
        AddMessage(message);
    })));
}

// AddLogFileMessageDispatch updates the UI with status information when
// a new log file is created.
task<void> Scenario2::AddLogFileMessageDispatch(Platform::String^ message, const std::wstring& logFileFullPath)
{
    return create_task(Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([=]() {

        // On the UI thread...

        Platform::String^ finalMessage;
        if (!logFileFullPath.empty())
        {
            // Add a message to the UI indicating a new log file was created.
            Platform::String^ path = ref new Platform::String(logFileFullPath.c_str());
            Platform::String^ directoryName;
            Platform::String^ fileName;
            std::wstring::size_type posLastSlash = logFileFullPath.find_last_of(L'\\');
            if (posLastSlash > 0 && posLastSlash < logFileFullPath.size() - 1)
            {
                directoryName = ref new Platform::String(logFileFullPath.substr(0, posLastSlash).c_str());
                fileName = ref new Platform::String(logFileFullPath.substr(posLastSlash + 1, logFileFullPath.size() - (posLastSlash + 1)).c_str());
            }
            else
            {
                directoryName = "";
                fileName = "";
            }
            finalMessage = message + ": " + fileName;
            finalMessage = message + ": " + fileName;
            ViewLogInfo->Text =
                "Log folder: \"" + directoryName + "\"\r\n" +
                "- To view with tracerpt: tracerpt.exe \"" + path + "\" -of XML -o LogFile.xml\r\n" +
                "- To view with Windows Performance Toolkit (WPT):\n" +
                "   xperf -merge \"" + path + "\" merged.etl\n" +
                "   wpa.exe merged.etl";
        }
        else
        {
            finalMessage = message + ": none, nothing logged since saving the last file.";
        }
        AddMessage(finalMessage);
    })));
}

void Scenario2::OnStatusChanged(Platform::Object^ sender, LoggingScenarioEventArgs^ e)
{
    if (e->Type == LoggingScenarioEventType::BusyStatusChanged)
    {
        create_task(Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([=]() {
            UpdateControls();
        })));
    }
    else if (e->Type == LoggingScenarioEventType::LogFileGenerated)
    {
        AddLogFileMessageDispatch("LogFileGenerated", e->LogFileFullPath).then([]() {
        });
    }
    else if (e->Type == LoggingScenarioEventType::LoggingEnabledDisabled)
    {
        Platform::String^ message = "Logging has been " + (e->Enabled ? "enabled" : "disabled") + ".";
        AddMessageDispatch(message);
    }
}

void Scenario2::UpdateControls()
{
    if (LoggingScenario->LoggingEnabled)
    {
        InputTextBlock1->Text = "Logging is enabled. Click 'Disable Logging' to disable logging. With logging enabled, you can click 'Log Messages' to use the logging API to generate log files.";
        EnableDisableLoggingButton->Content = "Disable Logging";

        if (LoggingScenario->IsBusy)
        {
            EnableDisableLoggingButton->IsEnabled = false;
            DoScenarioButton->IsEnabled = false;
        }
        else
        {
            EnableDisableLoggingButton->IsEnabled = true;
            DoScenarioButton->IsEnabled = true;
        }
    }
    else
    {
        InputTextBlock1->Text = "Logging is disabled. Click 'Enable Logging' to enable logging. After you enable logging you can click 'Log Messages' to use the logging API to generate log files.";
        EnableDisableLoggingButton->Content = "Enable Logging";
        DoScenarioButton->IsEnabled = false;

        if (LoggingScenario->IsBusy)
        {
            EnableDisableLoggingButton->IsEnabled = false;
        }
        else
        {
            EnableDisableLoggingButton->IsEnabled = true;
        }
    }
}

// Enabled/disabled logging.
void Scenario2::EnableDisableLogging(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (LoggingScenario->LoggingEnabled)
    {
        MainPage::Current->NotifyUser("Disabling logging...", NotifyType::StatusMessage);
    }
    else
    {
        MainPage::Current->NotifyUser("Enabling logging...", NotifyType::StatusMessage);
    }

    LoggingScenario->ToggleLoggingEnabledDisabled();
    UpdateControls();

    if (this->LoggingScenario->LoggingEnabled)
    {
        MainPage::Current->NotifyUser("Logging enabled.", NotifyType::StatusMessage);
    }
    else
    {
        MainPage::Current->NotifyUser("Logging disabled.", NotifyType::StatusMessage);
    }
}

// Run a sample scenario which logs lots of messages to produce several log files.
void Scenario2::DoScenario(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    DoScenarioButton->IsEnabled = false;
    MainPage::Current->NotifyUser("Scenario running...", NotifyType::StatusMessage);
    // Allow the UI to update...
    create_task(DoScenarioButton->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([=]() {}))).then([this]()
    {
        // Run the scenario asynchronously.
        return LoggingScenario->DoScenarioAsync();
    }).then([this](task<void> previousTask) {
        // After the scenario completes, re-enable UI controls and display a message.
        DoScenarioButton->IsEnabled = true;
        MainPage::Current->NotifyUser("Scenario finished.", NotifyType::StatusMessage);
    }, task_continuation_context::use_current());
}
