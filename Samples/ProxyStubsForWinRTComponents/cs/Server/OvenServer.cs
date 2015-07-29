//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.System.Threading;

namespace Microsoft.SDKSamples.Kitchen
{
    public struct Dimensions
    {
        public double Depth;
        public double Height;
        public double Width;
    }

    public enum OvenTemperature
    {
        Low,
        Medium,
        High
    }

    // Explicitly declare IIDs to make it easier to author the proxy stub interface registration
    [Guid(0x332FD2F1, 0x1C69, 0x4C91, 0x94, 0x9E, 0x4B, 0xB6, 0x7A, 0x85, 0xBD, 0xC5)]
    public interface IAppliance
    {
        double Volume { get; }
    }

    // Explicitly declare IIDs to make it easier to author the proxy stub interface registration
    [Guid(0x6A112353, 0x4F87, 0x4460, 0xA9, 0x08, 0x29, 0x44, 0xE9, 0x26, 0x86, 0xF3)]
    public interface IOven : IAppliance
    {
        void ConfigurePreheatTemperature(OvenTemperature level);
        void BakeBread(String flavor);
        event TypedEventHandler<Oven, Bread> BreadBaked;
    }

    public sealed class Oven : IOven
    {
        public Oven(Dimensions dimensions)
        {
            _dimensions = dimensions;
        }

        public Oven()
        {
            _dimensions.Height = 1.0;
            _dimensions.Width = 1.0;
            _dimensions.Depth = 1.0;
        }

        public double Volume
        {
            get
            {
                return _dimensions.Depth * _dimensions.Height * _dimensions.Width;
            }
        }

        public event TypedEventHandler<Oven, Bread> BreadBaked;

        public void BakeBread(String flavor)
        {
            TimeSpan preheatTime;

            // Determine how long to wait for the oven to get to temperature
            switch (_temperature)
            {
                case OvenTemperature.Low:
                    // Set the preheat time to 100ms
                    preheatTime = new TimeSpan(1000);
                    break;

                case OvenTemperature.Medium:
                    // Set the preheat time to 200ms
                    preheatTime = new TimeSpan(2000);
                    break;

                case OvenTemperature.High:
                    // Set the preheat time to 300ms
                    preheatTime = new TimeSpan(3000);
                    break;

                default:
                    throw new ArgumentException("Temperature is out of range. The temperature should be low, medium, or high.");
            }

            // Set up a threadpool work item to bake the bread and notify listeners after the oven has preheated
            ThreadPoolTimer.CreateTimer(new TimerElapsedHandler(
                (ThreadPoolTimer timer) =>
                {
                    Bread bread = new Bread(flavor);
                    if (BreadBaked != null)
                    {
                        BreadBaked(this, bread);
                    }
                }),
                preheatTime);
        }

        public void ConfigurePreheatTemperature(OvenTemperature level)
        {
            if (level >= OvenTemperature.Low && level <= OvenTemperature.High)
            {
                _temperature = level;
            }
            else
            {
                throw new ArgumentException("Temperature is out of range. The temperature should be low, medium, or high.");
            }
        }

        private Dimensions _dimensions;
        private OvenTemperature _temperature = OvenTemperature.Medium;
    }
}
