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
#include "Scenario8.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Services::Maps::OfflineMaps;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Maps;

Scenario8::Scenario8()
{
    InitializeComponent();
}

void Scenario8::myMap_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    myMap->Center = MainPage::SeattleGeopoint;
    myMap->ZoomLevel = 12;
}

void Scenario8::findAtCenterButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    resultsGrid->Children->Clear();
    resultsGrid->RowDefinitions->Clear();
    resultStatusTextBlock->Text = "";

    // finds all available Offline Map packages at the map center geopoint
    create_task(OfflineMapPackage::FindPackagesAsync(myMap->Center))
        .then([this](OfflineMapPackageQueryResult^ result)
    {
        if (result->Status != OfflineMapPackageQueryStatus::Success)
        {
            resultStatusTextBlock->Text = "Error: " + result->Status.ToString();
        }
        else if (result->Packages->Size == 0)
        {
            resultStatusTextBlock->Text = "No maps found";
        }
        else
        {
            for (OfflineMapPackage^ package : result->Packages)
            {
                addToResultGrid(package);
            }
        }
    });
}

void Scenario8::addToResultGrid(OfflineMapPackage^ package)
{
    // a descriptive name for the package
    // ex: "Washington, USA (160 MB)"
    String^ descriptiveName =
        package->DisplayName + ", " +
        package->EnclosingRegionName +
        " (" + nearestMegabyte(package->EstimatedSizeInBytes) + " MB)";

    // current download status (Downloaded, NotDownloaded, Downloading, etc)
    String^ downloadStatus = package->Status.ToString();

    TextBlock^ packageNameTextBlock = ref new TextBlock();
    packageNameTextBlock->Text = descriptiveName;
    packageNameTextBlock->Margin = Thickness(5);
    packageNameTextBlock->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;

    TextBlock^ packageStatusTextBlock = ref new TextBlock();
    packageStatusTextBlock->Text = downloadStatus;
    packageStatusTextBlock->Margin = Thickness(5);
    packageStatusTextBlock->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;

    Button^ packageDownloadButton = ref new Button();
    packageDownloadButton->Content = "Download";
    packageDownloadButton->IsEnabled = canDownload(package);

    // click event handler for Download button
    packageDownloadButton->Click += ref new RoutedEventHandler(
        [package, packageStatusTextBlock](Object^ sender, RoutedEventArgs^ args)
    {
        // queue up the package for download
        // requires offlineMapsManagement capability (see Package.appxmanifest)
        create_task(package->RequestStartDownloadAsync())
            .then([packageStatusTextBlock](OfflineMapPackageStartDownloadResult^ result)
        {
            // show any error
            if (result->Status != OfflineMapPackageStartDownloadStatus::Success)
            {
                packageStatusTextBlock->Text = result->Status.ToString();
            }
        });
    });

    // listen for package status changes
    package->StatusChanged += ref new TypedEventHandler<OfflineMapPackage ^, Platform::Object ^>(
        [package, packageStatusTextBlock, packageDownloadButton](OfflineMapPackage^ sender, Platform::Object^ args)
    {
        // event might not be on the UI thread, so marshal if we need to update UI
        packageStatusTextBlock->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
            [package, packageStatusTextBlock, packageDownloadButton]()
        {
            // update the UI (download status, download button enable/disable)
            packageStatusTextBlock->Text = package->Status.ToString();
            packageDownloadButton->IsEnabled = canDownload(package);
        }));
    });

    Grid::SetRow(packageNameTextBlock, resultsGrid->RowDefinitions->Size);
    Grid::SetColumn(packageNameTextBlock, 0);

    Grid::SetRow(packageStatusTextBlock, resultsGrid->RowDefinitions->Size);
    Grid::SetColumn(packageStatusTextBlock, 1);

    Grid::SetRow(packageDownloadButton, resultsGrid->RowDefinitions->Size);
    Grid::SetColumn(packageDownloadButton, 2);

    RowDefinition^ rowDef = ref new RowDefinition();
    rowDef->Height = GridLength::Auto;
    resultsGrid->RowDefinitions->Append(rowDef);

    resultsGrid->Children->Append(packageNameTextBlock);
    resultsGrid->Children->Append(packageStatusTextBlock);
    resultsGrid->Children->Append(packageDownloadButton);
}

unsigned long long Scenario8::nearestMegabyte(unsigned long long bytes)
{
    return (unsigned long long)round(bytes / (1024.0 * 1024.0));
}

bool Scenario8::canDownload(OfflineMapPackage^ package)
{
    // we can start a download if the package isn't downloaded or it's currently being deleted
    return package->Status == OfflineMapPackageStatus::NotDownloaded || package->Status == OfflineMapPackageStatus::Deleting;
}
