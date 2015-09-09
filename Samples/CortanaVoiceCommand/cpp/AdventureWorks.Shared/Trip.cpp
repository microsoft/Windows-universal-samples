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
#include "Trip.h"

using namespace AdventureWorks_Shared;

using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Core;

Trip::Trip()
{
}

String^ Trip::Destination::get()
{
    return this->destination;
}

void Trip::Destination::set(String^ value)
{
    this->destination = value;
    NotifyPropertyChanged("Destination");
}

String^ Trip::Description::get()
{
    return this->description;
}

void Trip::Description::set(String^ value)
{
    this->description = value;
    NotifyPropertyChanged("Description");
}

String^ Trip::Notes::get()
{
    return this->notes;
}

void Trip::Notes::set(String^ value)
{
    this->notes = value;
    NotifyPropertyChanged("Notes");
}


Calendar^ Trip::StartDate::get()
{
    return this->startDate;
}

void Trip::StartDate::set(Calendar^ value)
{
    this->startDate = value;
    NotifyPropertyChanged("StartDate");
}

Calendar^ Trip::EndDate::get()
{
    return this->endDate;
}

void Trip::EndDate::set(Calendar^ value)
{
    this->endDate = value;
    NotifyPropertyChanged("EndDate");
}

/// <summary> Fire property changed events if a subscriber is listening. </summary>
/// <param name="propertyName">The name of the property modified.</param>
void Trip::NotifyPropertyChanged(String^ propertyName)
{
    try
    {
        if (subscribers > 0)
        {
            CoreApplicationView^ view = CoreApplication::MainView;
            view->CoreWindow->Dispatcher->RunAsync(
                CoreDispatcherPriority::Normal,
                ref new DispatchedHandler([this, propertyName]()
            {
                PropertyChanged(this, ref new PropertyChangedEventArgs(propertyName));
            }));
        }
    }
    catch (Platform::Exception^)
    {
        // swallow exceptions here, we might be running on the background thread.
    }
}
