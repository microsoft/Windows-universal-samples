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
#include "Scenario3.h"
#include "Scenario3.g.cpp"
#include "LoggingScenarioHelpers.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace winrt::SDKTemplate::implementation;

Scenario3::Scenario3()
{
    InitializeComponent();
}

fire_and_forget
Scenario3::DoScenario(IInspectable const&, RoutedEventArgs const&)
{
    MainPage::Current().NotifyUser(L"Scenario running...", NotifyType::StatusMessage);
    DoScenarioButton().IsEnabled(false);

    // Run the scenario asynchronously so the UI can update.
    co_await resume_background();
    co_await _instance.DoScenarioAsync();

    // After the scenario completes, re-enable UI controls and display a message.
    co_await resume_foreground(Dispatcher());
    DoScenarioButton().IsEnabled(true);
    MainPage::Current().NotifyUser(L"Scenario finished.", NotifyType::StatusMessage);
}

fire_and_forget
Scenario3::EnableDisableLogging(IInspectable const&, RoutedEventArgs const&)
{
    if (_instance.LoggingEnabled())
    {
        MainPage::Current().NotifyUser(L"Disabling logging...", NotifyType::StatusMessage);
    }
    else
    {
        MainPage::Current().NotifyUser(L"Enabling logging...", NotifyType::StatusMessage);
    }

    co_await _instance.ToggleLoggingEnabledDisabledAsync();

    co_await resume_foreground(Dispatcher());

    UpdateControls();

    if (_instance.LoggingEnabled())
    {
        MainPage::Current().NotifyUser(L"Logging enabled.", NotifyType::StatusMessage);
    }
    else
    {
        MainPage::Current().NotifyUser(L"Logging disabled.", NotifyType::StatusMessage);
    }
}

void
Scenario3::OnNavigatedTo(NavigationEventArgs const&)
{
    _statusChangedToken = _instance.StatusChanged({ this, &Scenario3::OnStatusChanged });
    UpdateControls();
}

void
Scenario3::OnNavigatedFrom(NavigationEventArgs const&)
{
    _instance.StatusChanged(_statusChangedToken);
    UpdateControls();
}

void
Scenario3::UpdateControls()
{
    if (_instance.LoggingEnabled())
    {
        InputTextBlock1().Text(
            L"Logging is enabled. Click 'Disable Logging' to disable logging. With logging enabled, you can click 'Log Messages' to use the logging API to generate log files.");
        EnableDisableLoggingButton().Content(box_value(L"Disable Logging"));

        if (_instance.IsBusy())
        {
            EnableDisableLoggingButton().IsEnabled(false);
            DoScenarioButton().IsEnabled(false);
        }
        else
        {
            EnableDisableLoggingButton().IsEnabled(true);
            DoScenarioButton().IsEnabled(true);
        }
    }
    else
    {
        InputTextBlock1().Text(
            L"Logging is disabled. Click 'Enable Logging' to enable logging. After you enable logging you can click 'Log Messages' to use the logging API to generate log files.");
        EnableDisableLoggingButton().Content(box_value(L"Enable Logging"));
        DoScenarioButton().IsEnabled(false);
        EnableDisableLoggingButton().IsEnabled(!_instance.IsBusy());
    }
}

void
Scenario3::AddMessage(_In_ LPCWSTR message)
{
    TextBox statusMessageList = StatusMessageList();
    std::wstring statusText(statusMessageList.Text());
    statusText += message;
    statusText += L"\r\n";
    statusMessageList.Text(statusText);
    statusMessageList.Select(static_cast<int32_t>(statusText.size()), 0);

    ScrollViewer viewer = FindScrollViewer(statusMessageList);
    if (viewer != nullptr)
    {
        viewer.ChangeView(nullptr, statusMessageList.ActualHeight(), nullptr);
    }
}

void
Scenario3::AddLogFileMessage(_In_ LPCWSTR message, _In_opt_ LPCWSTR logFileFullPath)
{
    std::wstring finalMessage = message;

    if (logFileFullPath == nullptr || logFileFullPath[0] == 0)
    {
        finalMessage += L": none, nothing logged since saving the last file.";
    }
    else
    {
        // Add a message to the UI indicating a new log file was created.
        std::wstring fullPath(logFileFullPath);
        std::wstring directoryName;
        std::wstring fileName;
        std::size_t lastSlashPos = fullPath.find_last_of(L'\\');
        if (lastSlashPos > 0 && lastSlashPos < fullPath.size() - 1)
        {
            directoryName = fullPath.substr(0, lastSlashPos);
            fileName = fullPath.substr(lastSlashPos + 1, fullPath.size() - (lastSlashPos + 1));
        }

        ViewLogInfo().Text(
            L"Log folder: \"" + directoryName + L"\"\r\n"
            L"- To view with tracerpt: tracerpt.exe \"" + fullPath + L"\" -of XML -o LogFile.xml\r\n"
            L"- To view with Windows Performance Toolkit (WPT):\n"
            L"   xperf -merge \"" + fullPath + "\" merged.etl\n"
            L"   wpa.exe merged.etl");

        finalMessage += L": ";
        finalMessage += fileName;
    }

    AddMessage(finalMessage.c_str());
}

fire_and_forget
Scenario3::OnStatusChanged(SDKTemplate::LoggingScenarioEventArgs args)
{
    // Switch to UI thread.
    co_await resume_foreground(Dispatcher());

    switch (args.Type())
    {
    case LoggingScenarioEventType::BusyStatusChanged:
        UpdateControls();
        break;

    case LoggingScenarioEventType::LogFileGenerated:
        AddLogFileMessage(L"LogFileGenerated", args.LogFileFullPath().c_str());
        break;

    case LoggingScenarioEventType::LogFileGeneratedAtSuspend:
        AddLogFileMessage(L"Log file at suspend", args.LogFileFullPath().c_str());
        break;

    case LoggingScenarioEventType::LogFileGeneratedAtDisable:
        AddLogFileMessage(L"Log file at disable", args.LogFileFullPath().c_str());
        break;

    case LoggingScenarioEventType::LoggingEnabledDisabled:
        AddMessage(args.Enabled()
            ? L"Logging has been enabled."
            : L"Logging has been disabled.");
        break;

    default:
        AddMessage(L"Unrecognized event.");
        break;
    }
}
