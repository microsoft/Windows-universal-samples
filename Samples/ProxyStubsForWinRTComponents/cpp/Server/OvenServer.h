//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once

namespace Microsoft { namespace SDKSamples { namespace Kitchen {

    ref class Oven;

    // Explicitly declare IIDs to make it easier to author the proxy stub interface registration
    [uuid(332FD2F1-1C69-4C91-949E-4BB67A85BDC5)]
    public interface class IAppliance
    {
        property double Volume
        {
            double get();
        }
    };

    // Explicitly declare IIDs to make it easier to author the proxy stub interface registration
    [uuid(6A112353-4F87-4460-A908-2944E92686F3)]
    public interface class IOven : public IAppliance
    {
        void ConfigurePreheatTemperature(OvenTemperature temperature);
        void BakeBread(Platform::String^ flavor);

        event Windows::Foundation::TypedEventHandler<Oven^, Bread^>^ BreadBaked;
    };

    public ref class Oven sealed : public [Windows::Foundation::Metadata::Default] IOven
    {
    public:
        [Windows::Foundation::Metadata::Overload("CreateOven")]
        Oven(Dimensions dimensions)
        {
            _dims = dimensions;
        }

        Oven()
        {
            _dims.Height = 1.0;
            _dims.Width = 1.0;
            _dims.Depth = 1.0;
        }

    public:

        virtual property double Volume
        {
            // IAppliance::get_Volume
            virtual double get();
        }

        // IOven::ConfigurePreheatTemperature
        virtual void ConfigurePreheatTemperature(OvenTemperature temperature);

        // IOven::BakeBread
        virtual void BakeBread(Platform::String^ flavor);

        virtual event Windows::Foundation::TypedEventHandler<Oven^, Bread^>^ BreadBaked;

    private:
        Dimensions _dims;
        OvenTemperature _temperature;
    };

} /* Kitchen */ } /* SDKSamples */ } /* Microsoft */
