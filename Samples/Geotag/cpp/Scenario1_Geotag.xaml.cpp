// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_Geotag.xaml.h"

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::FileProperties;

Scenario1_Geotag::Scenario1_Geotag()
{
    InitializeComponent();
}

void Scenario1_Geotag::ChooseFileButton_Click()
{
    auto picker = ref new FileOpenPicker();
    picker->FileTypeFilter->Append(".jpg");
    picker->FileTypeFilter->Append(".jpeg");
    picker->FileTypeFilter->Append(".mp4");
    picker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    create_task(picker->PickSingleFileAsync())
        .then([this](StorageFile^ pickedFile)
    {
        file = pickedFile;
        if (file != nullptr)
        {
            FileDisplayName->Text = file->DisplayName;
            FileOperationsPanel->Visibility = Windows::UI::Xaml::Visibility::Visible;
        }
        else
        {
            FileOperationsPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        }
    });
}

void Scenario1_Geotag::GetGeotagButton_Click()
{
    create_task(GeotagHelper::GetGeotagAsync(file))
        .then([this](Geopoint^ geopoint)
    {
        if (geopoint != nullptr)
        {
            rootPage->NotifyUser("Latitude = " + geopoint->Position.Latitude
                + ", Longitude = " + geopoint->Position.Longitude, NotifyType::StatusMessage);
        }
        else
        {
            // File I/O errors are converted to "No information".
            rootPage->NotifyUser("No location information available", NotifyType::ErrorMessage);
        }
    });
}

void Scenario1_Geotag::SetGeotagFromGeolocatorButton_Click()
{
    // Call RequestAccessAsync from the UI thread to get user permission on location usage.
    // Otherwise SetGeotagFromGeolocator will fail.
    create_task(Geolocator::RequestAccessAsync())
        .then([this](GeolocationAccessStatus status)
    {
        if (status != GeolocationAccessStatus::Allowed)
        {
            rootPage->NotifyUser("Location access is not allowed", NotifyType::ErrorMessage);
        }
        else
        {
            Geolocator^ geolocator = ref new Geolocator();
            geolocator->DesiredAccuracy = PositionAccuracy::High;

            create_task(GeotagHelper::SetGeotagFromGeolocatorAsync(file, geolocator))
                .then([this](task<void> previousTask)
            {
                try
                {
                    previousTask.get();
                    rootPage->NotifyUser("Geolocation set to current location", NotifyType::StatusMessage);
                }
                catch (Exception^ ex)
                {
                    // File I/O errors are reported as exceptions.
                    // AccessDeniedExcetion can be raised if the user revoked location access
                    // after RequestAccessAsync completed.
                    rootPage->NotifyUser("Exception: " + ex->Message, NotifyType::ErrorMessage);
                }
            });
        }
    });
}

void Scenario1_Geotag::SetGeotagButton_Click()
{
    // Set the approximate position of the observation deck of the Seattle Space Needle.
    BasicGeoposition position;
    position.Latitude = 47.620491;
    position.Longitude = -122.349319;
    position.Altitude = 158.12;
    Geopoint^ geopoint = ref new Geopoint(position);

    create_task(GeotagHelper::SetGeotagAsync(file, geopoint))
        .then([this](task<void> previousTask)
    {
        try
        {
            previousTask.get();
            rootPage->NotifyUser("Geolocation set to Seattle Space Needle", NotifyType::StatusMessage);
        }
        catch (Exception^ ex)
        {
            // File I/O errors are reported as exceptions
            rootPage->NotifyUser("Exception: " + ex->Message, NotifyType::ErrorMessage);
        }
    });
}
