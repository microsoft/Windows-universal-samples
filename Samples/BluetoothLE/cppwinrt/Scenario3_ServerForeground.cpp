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
#include "Scenario3_ServerForeground.h"
#include "Scenario3_ServerForeground.g.cpp"
#include "SampleConfiguration.h"
#include "PresentationFormats.h"

using namespace winrt;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt
{
    hstring to_hstring(BluetoothError error)
    {
        switch (error)
        {
        case BluetoothError::Success: return L"Success";
        case BluetoothError::RadioNotAvailable: return L"RadioNotAvailable";
        case BluetoothError::ResourceInUse: return L"ResourceInUse";
        case BluetoothError::DeviceNotConnected: return L"DeviceNotConnected";
        case BluetoothError::OtherError: return L"OtherError";
        case BluetoothError::DisabledByPolicy: return L"DisabledByPolicy";
        case BluetoothError::NotSupported: return L"NotSupported";
        case BluetoothError::DisabledByUser: return L"DisabledByUser";
        case BluetoothError::ConsentRequired: return L"ConsentRequired";
        case BluetoothError::TransportNotSupported: return L"TransportNotSupported";
        }
        return L"Code " + to_hstring(static_cast<int>(error));
    }

    hstring to_hstring(GattServiceProviderAdvertisementStatus status)
    {
        switch (status)
        {
        case GattServiceProviderAdvertisementStatus::Created: return L"Created";
        case GattServiceProviderAdvertisementStatus::Stopped: return L"Stopped";
        case GattServiceProviderAdvertisementStatus::Started: return L"Started";
        case GattServiceProviderAdvertisementStatus::Aborted: return L"Aborted";
        }
        return L"Code " + to_hstring(static_cast<int>(status));
    }
}

namespace winrt::SDKTemplate::implementation
{
    // This scenario declares support for a calculator service. 
    // Remote clients (including this sample on another machine) can supply:
    // - Operands 1 and 2
    // - an operator (+,-,*,/)
    // and get a result

#pragma region UI Code
    Scenario3_ServerForeground::Scenario3_ServerForeground()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario3_ServerForeground::OnNavigatedTo(NavigationEventArgs const&)
    {
        auto lifetime = get_strong();
        peripheralSupported = co_await CheckPeripheralRoleSupportAsync();
        if (peripheralSupported)
        {
            ServerPanel().Visibility(Visibility::Visible);
        }
        else
        {
            PeripheralWarning().Visibility(Visibility::Visible);
        }
    }

    void Scenario3_ServerForeground::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (op1CharacteristicWriteToken)
        {
            op1Characteristic.WriteRequested(std::exchange(op1CharacteristicWriteToken, {}));
        }
        if (op2CharacteristicWriteToken)
        {
            op2Characteristic.WriteRequested(std::exchange(op2CharacteristicWriteToken, {}));
        }
        if (operatorCharacteristicWriteToken)
        {
            operatorCharacteristic.WriteRequested(std::exchange(operatorCharacteristicWriteToken, {}));
        }
        if (resultCharacteristicReadToken)
        {
            resultCharacteristic.ReadRequested(std::exchange(resultCharacteristicReadToken, {}));
        }
        if (resultCharacteristicClientsChangedToken)
        {
            resultCharacteristic.SubscribedClientsChanged(std::exchange(resultCharacteristicClientsChangedToken, {}));
        }
        if (serviceProviderAdvertisementChangedToken)
        {
            serviceProvider.AdvertisementStatusChanged(std::exchange(serviceProviderAdvertisementChangedToken, {}));
        }

        if (serviceProvider != nullptr)
        {
            if (serviceProvider.AdvertisementStatus() != GattServiceProviderAdvertisementStatus::Stopped)
            {
                serviceProvider.StopAdvertising();
            }
            serviceProvider = nullptr;
        }
    }

    fire_and_forget Scenario3_ServerForeground::PublishButton_ClickAsync()
    {
        auto lifetime = get_strong();

        // Server not initialized yet - initialize it and start publishing
        if (serviceProvider == nullptr)
        {
            bool serviceStarted = co_await ServiceProviderInitAsync();
            if (serviceStarted)
            {
                rootPage.NotifyUser(L"Service successfully started", NotifyType::StatusMessage);
                PublishButton().Content(box_value(L"Stop Service"));
            }
            else
            {
                rootPage.NotifyUser(L"Service not started", NotifyType::ErrorMessage);
            }
        }
        else
        {
            // BT_Code: Stops advertising support for custom GATT Service 
            serviceProvider.StopAdvertising();
            serviceProvider = nullptr;
            PublishButton().Content(box_value(L"Start Service"));
        }
    }

    fire_and_forget Scenario3_ServerForeground::UpdateUX()
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        switch (operatorReceived)
        {
        case CalculatorOperators::Add:
            OperationText().Text(L"+");
            break;
        case CalculatorOperators::Subtract:
            OperationText().Text(L"-");
            break;
        case CalculatorOperators::Multiply:
            OperationText().Text(L"*");
            break;
        case CalculatorOperators::Divide:
            OperationText().Text(L"/");
            break;
        default:
            OperationText().Text(L"INV");
            break;
        }
        Operand1Text().Text(to_hstring(operand1Received));
        Operand2Text().Text(to_hstring(operand2Received));
        resultVal = ComputeResult();
        ResultText().Text(to_hstring(resultVal));
    }
#pragma endregion

    IAsyncOperation<bool> Scenario3_ServerForeground::CheckPeripheralRoleSupportAsync()
    {
        // BT_Code: New for Creator's Update - Bluetooth adapter has properties of the local BT radio.
        auto localAdapter = co_await BluetoothAdapter::GetDefaultAsync();

        if (localAdapter != nullptr)
        {
            co_return localAdapter.IsPeripheralRoleSupported();
        }
        else
        {
            // Bluetooth is not turned on 
            co_return false;
        }
    }

    /// <summary>
    /// Uses the relevant Service/Characteristic UUIDs to initialize, hook up event handlers and start a service on the local system.
    /// </summary>
    /// <returns></returns>
    IAsyncOperation<bool> Scenario3_ServerForeground::ServiceProviderInitAsync()
    {
        // BT_Code: Initialize and starting a custom GATT Service using GattServiceProvider.
        auto lifetime = get_strong();

        GattServiceProviderResult serviceResult = co_await GattServiceProvider::CreateAsync(Constants::CalcServiceUuid);
        if (serviceResult.Error() == BluetoothError::Success)
        {
            serviceProvider = serviceResult.ServiceProvider();
        }
        else
        {
            rootPage.NotifyUser(L"Could not create service provider: " + to_hstring(serviceResult.Error()), NotifyType::ErrorMessage);
            co_return false;
        }

        // BT_Code: Initializes custom local parameters w/ properties, protection levels as well as common descriptors like User Description. 
        GattLocalCharacteristicParameters gattOperandParameters;
        gattOperandParameters.CharacteristicProperties(GattCharacteristicProperties::Write | GattCharacteristicProperties::WriteWithoutResponse);
        gattOperandParameters.WriteProtectionLevel(GattProtectionLevel::Plain);
        gattOperandParameters.UserDescription(L"Operand Characteristic");

        GattLocalCharacteristicResult result = co_await serviceProvider.Service().CreateCharacteristicAsync(Constants::Op1CharacteristicUuid, gattOperandParameters);
        if (result.Error() == BluetoothError::Success)
        {
            op1Characteristic = result.Characteristic();
        }
        else
        {
            rootPage.NotifyUser(L"Could not create operand1 characteristic: " + to_hstring(result.Error()), NotifyType::ErrorMessage);
            co_return false;
        }
        op1CharacteristicWriteToken = op1Characteristic.WriteRequested({ get_weak(), &Scenario3_ServerForeground::Op1Characteristic_WriteRequestedAsync });

        // Create the second operand characteristic.
        result = co_await serviceProvider.Service().CreateCharacteristicAsync(Constants::Op2CharacteristicUuid, gattOperandParameters);
        if (result.Error() == BluetoothError::Success)
        {
            op2Characteristic = result.Characteristic();
        }
        else
        {
            rootPage.NotifyUser(L"Could not create operand2 characteristic: " + to_hstring(result.Error()), NotifyType::ErrorMessage);
            co_return false;
        }

        op2CharacteristicWriteToken = op2Characteristic.WriteRequested({ get_weak(), &Scenario3_ServerForeground::Op2Characteristic_WriteRequestedAsync });

        // Create the operator characteristic.
        GattLocalCharacteristicParameters gattOperatorParameters;
        gattOperandParameters.CharacteristicProperties(GattCharacteristicProperties::Write | GattCharacteristicProperties::WriteWithoutResponse);
        gattOperandParameters.WriteProtectionLevel(GattProtectionLevel::Plain);
        gattOperandParameters.UserDescription(L"Operator Characteristic");

        result = co_await serviceProvider.Service().CreateCharacteristicAsync(Constants::OperatorCharacteristicUuid, gattOperatorParameters);
        if (result.Error() == BluetoothError::Success)
        {
            operatorCharacteristic = result.Characteristic();
        }
        else
        {
            rootPage.NotifyUser(L"Could not create operator characteristic: " + to_hstring(result.Error()), NotifyType::ErrorMessage);
            co_return false;
        }

        operatorCharacteristicWriteToken = operatorCharacteristic.WriteRequested({ get_weak(), &Scenario3_ServerForeground::OperatorCharacteristic_WriteRequestedAsync });

        // Create the result characteristic.
        GattLocalCharacteristicParameters gattResultParameters;
        gattResultParameters.CharacteristicProperties(GattCharacteristicProperties::Read | GattCharacteristicProperties::Notify);
        gattResultParameters.WriteProtectionLevel(GattProtectionLevel::Plain);
        gattResultParameters.UserDescription(L"Result  Characteristic");

        // Add presentation format - 32-bit unsigned integer, with exponent 0, the unit is unitless, with no company description
        GattPresentationFormat intFormat = GattPresentationFormat::FromParts(
            GattPresentationFormatTypes::UInt32(),
            PresentationFormats::Exponent,
            static_cast<uint16_t>(PresentationFormats::Units::Unitless),
            static_cast<uint8_t>(PresentationFormats::NamespaceId::BluetoothSigAssignedNumber),
            PresentationFormats::Description);

        gattResultParameters.PresentationFormats().Append(intFormat);

        result = co_await serviceProvider.Service().CreateCharacteristicAsync(Constants::ResultCharacteristicUuid, gattResultParameters);
        if (result.Error() == BluetoothError::Success)
        {
            resultCharacteristic = result.Characteristic();
        }
        else
        {
            rootPage.NotifyUser(L"Could not create result characteristic: " + to_hstring(result.Error()), NotifyType::ErrorMessage);
            co_return false;
        }
        resultCharacteristicReadToken = resultCharacteristic.ReadRequested({ get_weak(), &Scenario3_ServerForeground::ResultCharacteristic_ReadRequestedAsync });
        resultCharacteristicClientsChangedToken = resultCharacteristic.SubscribedClientsChanged({ get_weak(), &Scenario3_ServerForeground::ResultCharacteristic_SubscribedClientsChanged });

        // BT_Code: Indicate if your sever advertises as connectable and discoverable.
        GattServiceProviderAdvertisingParameters advParameters;

        // IsConnectable determines whether a call to publish will attempt to start advertising and 
        // put the service UUID in the ADV packet (best effort)
        advParameters.IsConnectable(peripheralSupported);

        // IsDiscoverable determines whether a remote device can query the local device for support 
        // of this service
        advParameters.IsDiscoverable(true);

        serviceProviderAdvertisementChangedToken = serviceProvider.AdvertisementStatusChanged({ get_weak(), &Scenario3_ServerForeground::ServiceProvider_AdvertisementStatusChanged });
        serviceProvider.StartAdvertising(advParameters);
        co_return true;
    }

    void Scenario3_ServerForeground::ResultCharacteristic_SubscribedClientsChanged(GattLocalCharacteristic const& sender, IInspectable const&)
    {
        rootPage.NotifyUser(L"New device subscribed. New subscribed count: " + to_hstring(sender.SubscribedClients().Size()), NotifyType::StatusMessage);
    }

    void Scenario3_ServerForeground::ServiceProvider_AdvertisementStatusChanged(GattServiceProvider const& sender, GattServiceProviderAdvertisementStatusChangedEventArgs const&)
    {
        // Created - The default state of the advertisement, before the service is published for the first time.
        // Stopped - Indicates that the application has canceled the service publication and its advertisement.
        // Started - Indicates that the system was successfully able to issue the advertisement request.
        // Aborted - Indicates that the system was unable to submit the advertisement request, or it was canceled due to resource contention.

        rootPage.NotifyUser(L"New Advertisement Status: AdvertisementStatus = " + to_hstring(sender.AdvertisementStatus()), NotifyType::StatusMessage);
    }

    fire_and_forget Scenario3_ServerForeground::ResultCharacteristic_ReadRequestedAsync(GattLocalCharacteristic const&, GattReadRequestedEventArgs args)
    {
        // BT_Code: Process a read request. 
        auto lifetime = get_strong();
        auto deferral = args.GetDeferral();

        // Get the request information.  This requires device access before an app can access the device's request. 
        GattReadRequest request = co_await args.GetRequestAsync();
        if (request == nullptr)
        {
            // No access allowed to the device.  Application should indicate this to the user.
            rootPage.NotifyUser(L"Access to device not allowed", NotifyType::ErrorMessage);
        }
        else
        {
            DataWriter writer;
            writer.ByteOrder(ByteOrder::LittleEndian);
            writer.WriteInt32(resultVal);

            // Can get details about the request such as the size and offset, as well as monitor the state to see if it has been completed/cancelled externally.
            // request.Offset
            // request.Length
            // request.State
            // request.StateChanged += <Handler>

            // Gatt code to handle the response
            request.RespondWithValue(writer.DetachBuffer());
        }

        deferral.Complete();
    }

    int Scenario3_ServerForeground::ComputeResult()
    {
        int computedValue = 0;
        switch (operatorReceived)
        {
        case CalculatorOperators::Add:
            computedValue = operand1Received + operand2Received;
            break;
        case CalculatorOperators::Subtract:
            computedValue = operand1Received - operand2Received;
            break;
        case CalculatorOperators::Multiply:
            computedValue = operand1Received * operand2Received;
            break;
        case CalculatorOperators::Divide:
            if (operand2Received == 0 || (operand1Received == std::numeric_limits<int>::min() && operand2Received == -1))
            {
                rootPage.NotifyUser(L"Division overflow", NotifyType::ErrorMessage);
            }
            else
            {
                computedValue = operand1Received / operand2Received;
            }
            break;
        default:
            rootPage.NotifyUser(L"Invalid Operator", NotifyType::ErrorMessage);
            break;
        }
        NotifyClientDevices(computedValue);
        return computedValue;
    }

    fire_and_forget Scenario3_ServerForeground::NotifyClientDevices(int computedValue)
    {
        auto lifetime = get_strong();
        DataWriter writer;
        writer.ByteOrder(ByteOrder::LittleEndian);
        writer.WriteInt32(computedValue);

        // BT_Code: Returns a collection of all clients that the notification was attempted and the result.
        IVectorView<GattClientNotificationResult> results = co_await resultCharacteristic.NotifyValueAsync(writer.DetachBuffer());

        rootPage.NotifyUser(L"Sent value " + to_hstring(computedValue) + L" to clients.", NotifyType::StatusMessage);
        for (GattClientNotificationResult&& result : results)
        {
            // An application can iterate through each registered client that was notified and retrieve the results:
            //
            // result.SubscribedClient(): The details on the remote client.
            // result.Status(): The GattCommunicationStatus
            // result.ProtocolError(): iff Status() == GattCommunicationStatus::ProtocolError
        }
    }

    fire_and_forget Scenario3_ServerForeground::Op1Characteristic_WriteRequestedAsync(GattLocalCharacteristic const&, GattWriteRequestedEventArgs args)
    {
        // BT_Code: Processing a write request.
        auto lifetime = get_strong();
        auto deferral = args.GetDeferral();

        // Get the request information.  This requires device access before an app can access the device's request.
        GattWriteRequest request = co_await args.GetRequestAsync();
        if (request == nullptr)
        {
            // No access allowed to the device.  Application should indicate this to the user.
        }
        else
        {
            ProcessWriteCharacteristic(request, CalculatorCharacteristics::Operand1);
        }
        deferral.Complete();
    }

    fire_and_forget Scenario3_ServerForeground::Op2Characteristic_WriteRequestedAsync(GattLocalCharacteristic const&, GattWriteRequestedEventArgs args)
    {
        auto lifetime = get_strong();
        auto deferral = args.GetDeferral();

        // Get the request information.  This requires device access before an app can access the device's request.
        GattWriteRequest request = co_await args.GetRequestAsync();
        if (request == nullptr)
        {
            // No access allowed to the device.  Application should indicate this to the user.
        }
        else
        {
            ProcessWriteCharacteristic(request, CalculatorCharacteristics::Operand2);
        }
        deferral.Complete();
    }

    fire_and_forget Scenario3_ServerForeground::OperatorCharacteristic_WriteRequestedAsync(GattLocalCharacteristic const&, GattWriteRequestedEventArgs args)
    {
        auto lifetime = get_strong();
        auto deferral = args.GetDeferral();

        // Get the request information.  This requires device access before an app can access the device's request.
        GattWriteRequest request = co_await args.GetRequestAsync();
        if (request == nullptr)
        {
            // No access allowed to the device.  Application should indicate this to the user.
        }
        else
        {
            ProcessWriteCharacteristic(request, CalculatorCharacteristics::Operator);
        }
        deferral.Complete();
    }

    void Scenario3_ServerForeground::ProcessWriteCharacteristic(GattWriteRequest const& request, CalculatorCharacteristics opCode)
    {
        if (request.Value().Length() != 4)
        {
            // Input is the wrong length. Respond with a protocol error if requested.
            if (request.Option() == GattWriteOption::WriteWithResponse)
            {
                request.RespondWithProtocolError(GattProtocolError::InvalidAttributeValueLength());
            }
            return;
        }

        DataReader reader = DataReader::FromBuffer(request.Value());
        reader.ByteOrder(ByteOrder::LittleEndian);
        int val = reader.ReadInt32();

        switch (opCode)
        {
        case CalculatorCharacteristics::Operand1:
            operand1Received = val;
            break;
        case CalculatorCharacteristics::Operand2:
            operand2Received = val;
            break;
        case CalculatorCharacteristics::Operator:
            if (!IsValidOperator(static_cast<CalculatorOperators>(val)))
            {
                if (request.Option() == GattWriteOption::WriteWithResponse)
                {
                    request.RespondWithProtocolError(GattProtocolError::InvalidPdu());
                }
                return;
            }
            operatorReceived = static_cast<CalculatorOperators>(val);
            break;
        }
        // Complete the request if needed
        if (request.Option() == GattWriteOption::WriteWithResponse)
        {
            request.Respond();
        }

        UpdateUX();
    }
}
