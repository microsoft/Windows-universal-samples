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

#include "Scenario3_ServerForeground.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_ServerForeground : Scenario3_ServerForegroundT<Scenario3_ServerForeground>
    {
        Scenario3_ServerForeground();

        fire_and_forget OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget PublishButton_ClickAsync();
    private:
        enum class CalculatorCharacteristics
        {
            Operand1 = 1,
            Operand2 = 2,
            Operator = 3,
        };

        enum class CalculatorOperators
        {
            Add = 1,
            Subtract = 2,
            Multiply = 3,
            Divide = 4,
        };

        bool IsValidOperator(CalculatorOperators op)
        {
            return op >= CalculatorOperators::Add && op < CalculatorOperators::Divide;
        }

        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Devices::Bluetooth::GenericAttributeProfile::GattServiceProvider serviceProvider{ nullptr };
        Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalCharacteristic op1Characteristic{ nullptr };
        Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalCharacteristic op2Characteristic{ nullptr };
        Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalCharacteristic operatorCharacteristic{ nullptr };
        Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalCharacteristic resultCharacteristic{ nullptr };
        int operand1Received = 0;
        int operand2Received = 0;
        CalculatorOperators operatorReceived;
        int resultVal = 0;
        bool peripheralSupported = false;

        event_token op1CharacteristicWriteToken;
        event_token op2CharacteristicWriteToken;
        event_token operatorCharacteristicWriteToken;
        event_token resultCharacteristicReadToken;
        event_token resultCharacteristicClientsChangedToken;
        event_token serviceProviderAdvertisementChangedToken;

        fire_and_forget UpdateUX();
        static Windows::Foundation::IAsyncOperation<bool> CheckPeripheralRoleSupportAsync();
        Windows::Foundation::IAsyncOperation<bool> ServiceProviderInitAsync();
        void ResultCharacteristic_SubscribedClientsChanged(Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalCharacteristic const& sender, IInspectable const& args);
        void ServiceProvider_AdvertisementStatusChanged(Windows::Devices::Bluetooth::GenericAttributeProfile::GattServiceProvider const& sender, Windows::Devices::Bluetooth::GenericAttributeProfile::GattServiceProviderAdvertisementStatusChangedEventArgs const& args);
        fire_and_forget ResultCharacteristic_ReadRequestedAsync(Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalCharacteristic const& sender, Windows::Devices::Bluetooth::GenericAttributeProfile::GattReadRequestedEventArgs args);
        int ComputeResult();
        fire_and_forget NotifyClientDevices(int computedValue);
        fire_and_forget Op1Characteristic_WriteRequestedAsync(Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalCharacteristic const& sender, Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteRequestedEventArgs args);
        fire_and_forget Op2Characteristic_WriteRequestedAsync(Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalCharacteristic const& sender, Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteRequestedEventArgs args);
        fire_and_forget OperatorCharacteristic_WriteRequestedAsync(Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalCharacteristic const& sender, Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteRequestedEventArgs args);
        void ProcessWriteCharacteristic(Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteRequest const& request, CalculatorCharacteristics opCode);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_ServerForeground : Scenario3_ServerForegroundT<Scenario3_ServerForeground, implementation::Scenario3_ServerForeground>
    {
    };
}
