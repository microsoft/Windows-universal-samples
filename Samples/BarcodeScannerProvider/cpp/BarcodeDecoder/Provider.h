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

#include "pch.h"
#include "DecodeEngine.h"

namespace BarcodeDecoder
{
    ref class Provider sealed
    {
    public:
        Provider(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ backgroundTask);
        virtual ~Provider();

    private:
        Windows::Devices::PointOfService::Provider::BarcodeScannerProviderConnection^ m_connection;
        bool m_scannerEnabled = false;
        std::map <UINT, Windows::Devices::PointOfService::BarcodeSymbologyAttributes^> m_symbologyAttributes;

        Windows::ApplicationModel::Background::IBackgroundTaskInstance^ m_backgroundTask;
        Platform::Agile<Windows::ApplicationModel::Background::BackgroundTaskDeferral> m_deferral;

        Windows::Devices::PointOfService::Provider::BarcodeScannerFrameReader^ m_frameReader;
        DecodeEngine^ m_decodeEngine;

        DecodeResult^ m_previousDecodeResult;

        Windows::Foundation::EventRegistrationToken m_frameArrivedToken;
        Windows::Foundation::EventRegistrationToken m_enableScannerRequestedToken;
        Windows::Foundation::EventRegistrationToken m_disableScannerRequestedToken;
        Windows::Foundation::EventRegistrationToken m_getBarcodeSymbologyAttributesRequestedToken;
        Windows::Foundation::EventRegistrationToken m_setBarcodeSymbologyAttributesRequestedToken;
        Windows::Foundation::EventRegistrationToken m_startSoftwareTriggerRequestedToken;
        Windows::Foundation::EventRegistrationToken m_stopSoftwareTriggerRequestedToken;
        Windows::Foundation::EventRegistrationToken m_hideVideoPreviewRequestedToken;
        Windows::Foundation::EventRegistrationToken m_getActiveSymbologiesRequestedToken;

        void Initialize();
        void CleanUp();
        void RegisterEventHandlers();
        Windows::Devices::PointOfService::BarcodeScannerReport^ CreateBarcodeScannerReportFromString(Platform::String^ barcodeString, UINT barcodeSymbology);

        void FrameReader_FrameArrived(Windows::Devices::PointOfService::Provider::BarcodeScannerFrameReader^ sender, Windows::Devices::PointOfService::Provider::BarcodeScannerFrameReaderFrameArrivedEventArgs^ args);

        void Task_Completed(Windows::ApplicationModel::Background::BackgroundTaskRegistration^ sender, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs^ args);
        void BackgroundTask_Canceled(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ sender, Windows::ApplicationModel::Background::BackgroundTaskCancellationReason args);

        void BarcodeScannerProviderConnection_EnableScannerRequested(Windows::Devices::PointOfService::Provider::BarcodeScannerProviderConnection^ sender, Windows::Devices::PointOfService::Provider::BarcodeScannerEnableScannerRequestEventArgs^ args);
        void BarcodeScannerProviderConnection_DisableScannerRequested(Windows::Devices::PointOfService::Provider::BarcodeScannerProviderConnection^ sender, Windows::Devices::PointOfService::Provider::BarcodeScannerDisableScannerRequestEventArgs^ args);
        void BarcodeScannerProviderConnection_GetBarcodeSymbologyAttributesRequested(Windows::Devices::PointOfService::Provider::BarcodeScannerProviderConnection^ sender, Windows::Devices::PointOfService::Provider::BarcodeScannerGetSymbologyAttributesRequestEventArgs^ args);
        void BarcodeScannerProviderConnection_SetBarcodeSymbologyAttributesRequested(Windows::Devices::PointOfService::Provider::BarcodeScannerProviderConnection^ sender, Windows::Devices::PointOfService::Provider::BarcodeScannerSetSymbologyAttributesRequestEventArgs^ args);
        void BarcodeScannerProviderConnection_StartSoftwareTriggerRequested(Windows::Devices::PointOfService::Provider::BarcodeScannerProviderConnection^ sender, Windows::Devices::PointOfService::Provider::BarcodeScannerStartSoftwareTriggerRequestEventArgs^ args);
        void BarcodeScannerProviderConnection_StopSoftwareTriggerRequested(Windows::Devices::PointOfService::Provider::BarcodeScannerProviderConnection^ sender, Windows::Devices::PointOfService::Provider::BarcodeScannerStopSoftwareTriggerRequestEventArgs^ args);
        void BarcodeScannerProviderConnection_HideVideoPreviewRequested(Windows::Devices::PointOfService::Provider::BarcodeScannerProviderConnection^ sender, Windows::Devices::PointOfService::Provider::BarcodeScannerHideVideoPreviewRequestEventArgs^ args);
        void BarcodeScannerProviderConnection_SetActiveSymbologiesRequested(Windows::Devices::PointOfService::Provider::BarcodeScannerProviderConnection^ sender, Windows::Devices::PointOfService::Provider::BarcodeScannerSetActiveSymbologiesRequestEventArgs^ args);
    };
}