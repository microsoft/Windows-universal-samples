// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_Geotag.xaml.h"

using namespace SDKTemplate;
using namespace GeotagCPPSample;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Devices::Geolocation;
using namespace concurrency;

Scenario1_Geotag::Scenario1_Geotag() : rootPage(MainPage::Current)
{
    InitializeComponent();
    RequestLocationAccess();
}

void Scenario1_Geotag::LogStatus(Platform::String^ message)
{
    rootPage->NotifyUser(message, NotifyType::StatusMessage);
}

void Scenario1_Geotag::LogError(Platform::String^ message)
{
    rootPage->NotifyUser(message, NotifyType::ErrorMessage);
}

// Apps need to call RequestAccessAsync to get user permission on location usage,
// otherwise SetGeotagFromGeolocator will fail. Also RequestAccessAsync needs 
// to be called from a UI thread.
void Scenario1_Geotag::RequestLocationAccess()
{
    create_task(Geolocator::RequestAccessAsync())
        .then([this](task<GeolocationAccessStatus> task)
    {
        try
        {
            GeolocationAccessStatus status = task.get();
            if (status != GeolocationAccessStatus::Allowed)
            {
                LogError("Location access is NOT allowed");
            }
        }
        catch (Exception^ e)
        {
            LogError("Exception: " + e->ToString());
        }
    });
}

void Scenario1_Geotag::GetGeotag(StorageFile^ file)
{
    create_task(GeotagHelper::GetGeotagAsync(file))
        .then([this](task<Geopoint^> geopointTask)
    {
        Geopoint^ geopoint = geopointTask.get();
        if (geopoint != nullptr)
        {
            LogStatus("GetGeotagAsync completed - " + "latitude: " + geopoint->Position.Latitude + " longitude: " + geopoint->Position.Longitude);
        }
        else
        {
            LogStatus("GetGeotagAsync completed - location info not available");
        }
    });
}

void Scenario1_Geotag::SetGeotag(StorageFile^ file, Geopoint^ geopoint)
{
    create_task(GeotagHelper::SetGeotagAsync(file, geopoint))
        .then([this](task<void> task)
    {
        try
        {
            task.get();
            LogStatus("SetGeotagAsync completed");
        }
        catch (Exception^ e)
        {
            LogError("Exception: " + e->ToString());
        }
    });
}

void Scenario1_Geotag::SetGeotagFromGeolocator(StorageFile^ file, Geolocator^ geolocator)
{
    create_task(GeotagHelper::SetGeotagFromGeolocatorAsync(file, geolocator))
        .then([this](task<void> task)
    {
        try
        {
            task.get();
            LogStatus("SetGeotagFromGeolocatorAsync completed");
        }
        catch (Exception^ e)
        {
            LogError("Exception: " + e->ToString());
        }
    });
}

void Scenario1_Geotag::Button_Click_GetGeotag(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    create_task(KnownFolders::PicturesLibrary->GetFileAsync(filename->Text))
        .then([this](task<StorageFile^> fileTask)
    {
        try
        {
            StorageFile^ file = fileTask.get();
            GetGeotag(file);
        }
        catch (Exception^ e)
        {
            LogError("Exception: " + e->ToString());
        }
    });
}

void Scenario1_Geotag::Button_Click_SetGeotag(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    create_task(KnownFolders::PicturesLibrary->GetFileAsync(filename->Text))
        .then([this](task<StorageFile^> fileTask)
    {
        try
        {
            StorageFile^ file = fileTask.get();
            Geopoint^ geopoint = ref new Geopoint({ 10.0, 20.0, 0.0 }); // Use latitude 10.0, longitude 20.0 as an example
            SetGeotag(file, geopoint);
        }
        catch (Exception^ e)
        {
            LogError("Exception: " + e->ToString());
        }
    });
}

void Scenario1_Geotag::Button_Click_SetGeotagFromGeolocator(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    create_task(KnownFolders::PicturesLibrary->GetFileAsync(filename->Text))
        .then([this](task<StorageFile^> fileTask)
    {
        try
        {
            StorageFile^ file = fileTask.get();
            Geolocator^ geolocator = ref new Geolocator;
            geolocator->DesiredAccuracy = PositionAccuracy::High;
            SetGeotagFromGeolocator(file, geolocator);
        }
        catch (Exception^ e)
        {
            LogError("Exception: " + e->ToString());
        }
    });
}
