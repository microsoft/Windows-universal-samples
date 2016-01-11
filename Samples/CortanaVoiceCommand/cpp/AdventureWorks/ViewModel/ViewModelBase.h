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

namespace AdventureWorks
{
    public ref class ViewModelBase : public Windows::UI::Xaml::DependencyObject, Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    internal:
        ViewModelBase();

    public:
        // Inherited via INotifyPropertyChanged
        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

    protected:
        void NotifyPropertyChanged(Platform::String^ propertyName);
    };
}
