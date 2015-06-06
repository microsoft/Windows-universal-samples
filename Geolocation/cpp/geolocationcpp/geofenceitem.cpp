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

//
// GeofenceItem.cpp
// Implementation of the GeofenceItem class
//

#include "pch.h"
#include "GeofenceItem.h"

using namespace SDKTemplate;
using namespace SDKTemplate::GeolocationCPP;

using namespace Windows::Devices::Geolocation;
using namespace Windows::Devices::Geolocation::Geofencing;

Windows::Devices::Geolocation::Geofencing::Geofence^ GeofenceItem::Geofence::get()
{
    return geofence;
}

Platform::String^ GeofenceItem::Id::get()
{
    return geofence->Id;
}

double GeofenceItem::Latitude::get()
{
    Geocircle^ circle = safe_cast<Geocircle^> (geofence->Geoshape);
    return circle->Center.Latitude;
}

double GeofenceItem::Longitude::get()
{
    Geocircle^ circle = safe_cast<Geocircle^> (geofence->Geoshape);
    return circle->Center.Longitude;
}

double GeofenceItem::Radius::get()
{
    Geocircle^ circle = safe_cast<Geocircle^> (geofence->Geoshape);
    return circle->Radius;
}

long long GeofenceItem::DwellTime::get()
{
    return geofence->DwellTime.Duration;
}

bool GeofenceItem::SingleUse::get()
{
    return geofence->SingleUse;
}

unsigned int GeofenceItem::MonitoredStates::get()
{
    return safe_cast<int>(geofence->MonitoredStates);
}

long long GeofenceItem::Duration::get()
{
    return geofence->Duration.Duration;
}

long long GeofenceItem::StartTime::get()
{
    return geofence->StartTime.UniversalTime;
}
