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

#pragma once

#include "Scenario2_PrintReceipt.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_PrintReceipt sealed
    {
    public:
        Scenario2_PrintReceipt();

    protected:
        void PrintLine_Click();
        void PrintReceipt_Click();
        void PrintBitmap_Click();
        void PrintBarcode_Click();

    private:
        MainPage^ rootPage = MainPage::Current;

        bool IsPrinterClaimed();
        Concurrency::task<bool> ExecuteJobAndReportResultAsync(Windows::Devices::PointOfService::ReceiptPrintJob^ job);

        Platform::String^ GetMerchantFooter();
        Platform::String^ GetCustomerFooter();
        void PrintLineFeedAndCutPaper(Windows::Devices::PointOfService::ReceiptPrintJob^ job, Platform::String^ receipt);
        Concurrency::task<Windows::Graphics::Imaging::BitmapFrame^> LoadLogoBitmapAsync();
    };
}
