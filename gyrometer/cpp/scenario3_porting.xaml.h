//
// Scenario3_Porting.xaml.h
// Declaration of the Scenario3_Porting class
//

#pragma once

#include "pch.h"
#include "Scenario3_Porting.g.h"
#include "MainPage.xaml.h"

namespace GyrometerCPP
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_Porting sealed
    {
    public:
        Scenario3_Porting();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;
        Windows::Devices::Sensors::Gyrometer^ gyrometerWP;
        Windows::Devices::Sensors::Gyrometer^ gyrometerWindows;

        void GetGyrometerSample(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}