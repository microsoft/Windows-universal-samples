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
namespace AdventureWorks_Shared
{
    [Windows::UI::Xaml::Data::Bindable]
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Trip sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        Trip();

        // Inherited via INotifyPropertyChanged
        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged
        {
            Windows::Foundation::EventRegistrationToken add(Windows::UI::Xaml::Data::PropertyChangedEventHandler^ handler)
            {
                subscribers++;
                return _PropertyChanged += handler;
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                subscribers--;
                return _PropertyChanged -= token;
            }

        internal:
            void raise(Object^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs^ args)
            {
                if (subscribers > 0)
                {
                    return _PropertyChanged(sender, args);
                }
                return;
            }
        };

        property Platform::String^ Destination
        {
            Platform::String^ get();
            void set(Platform::String^ value);
        }

        property Platform::String^ Description
        {
            Platform::String^ get();
            void set(Platform::String^ value);
        }

        property Windows::Globalization::Calendar^ StartDate
        {
            Windows::Globalization::Calendar^ get();
            void set(Windows::Globalization::Calendar^ value);
        }

        property Windows::Globalization::Calendar^ EndDate
        {
            Windows::Globalization::Calendar^ get();
            void set(Windows::Globalization::Calendar^ value);
        }

        property Platform::String^ Notes
        {
            Platform::String^ get();
            void set(Platform::String^ value);
        }

    private:
        int subscribers;
        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ _PropertyChanged;
        Platform::String^ destination;
        Platform::String^ description;
        Windows::Globalization::Calendar^ startDate;
        Windows::Globalization::Calendar^ endDate;
        Platform::String^ notes;

        void NotifyPropertyChanged(Platform::String^ propertyName);
    };

}
