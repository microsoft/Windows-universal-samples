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
#include "ViewModelBase.h"

using namespace AdventureWorks;

using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Data;

ViewModelBase::ViewModelBase()
{
}

/// <summary> Propagate a PropertyChanged notification to any object inheriting from this
/// base</summary>
/// <param name="propertyName">The property the UI should refresh</param>
void ViewModelBase::NotifyPropertyChanged(String^ propertyName)
{
    CoreApplicationView^ view = CoreApplication::MainView;
    view->CoreWindow->Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, propertyName]()
    {
        PropertyChanged(this, ref new PropertyChangedEventArgs(propertyName));

    }));
}
