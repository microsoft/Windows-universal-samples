//
// Scenario5_TriggerCFU.xaml.h
// Declaration of the Scenario5_TriggerCFU class
//

#pragma once

#include "pch.h"
#include "Scenario5_TriggerCFU.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    public ref class Scenario5 sealed
    {
    public:
        Scenario5();

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::Storage::StorageFile^ m_afterWriteFile = nullptr;
        Windows::Storage::StorageFile^ m_beforeReadFile = nullptr;
        Platform::String^ m_faToken;

        void UpdateButtons();
        void CreateFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void WriteToFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void WriteToFileWithExplicitCFUButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SaveToFutureAccessListButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void GetFileFromFutureAccessListButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PickAFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
