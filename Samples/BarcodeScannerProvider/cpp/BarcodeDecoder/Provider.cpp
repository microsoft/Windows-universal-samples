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
#include "Provider.h"

using namespace concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Devices::PointOfService::Provider;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Security::Cryptography;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;

namespace BarcodeDecoder
{
    Provider::Provider(IBackgroundTaskInstance^ backgroundTask) :
        m_backgroundTask(backgroundTask)
    {
        auto triggerDetails = safe_cast<BarcodeScannerProviderTriggerDetails^>(backgroundTask->TriggerDetails);
        m_connection = triggerDetails->Connection;

        Initialize();
    }

    Provider::~Provider()
    {
        CleanUp();
    }

    void Provider::CleanUp()
    {
        delete m_frameReader;
        m_frameReader = nullptr;

        delete m_connection;
        m_connection = nullptr;

        if (m_deferral != nullptr)
        {
            m_deferral->Complete();
            m_deferral = nullptr;
        }
    }

    BarcodeScannerReport^ Provider::CreateBarcodeScannerReportFromString(String^ barcodeString, UINT barcodeSymbology)
    {
        IBuffer^ barcodeDataBuffer = CryptographicBuffer::ConvertStringToBinary(barcodeString, BinaryStringEncoding::Utf8);
        return ref new BarcodeScannerReport(barcodeSymbology, barcodeDataBuffer, barcodeDataBuffer);
    }

    void Provider::Initialize()
    {
        m_deferral = m_backgroundTask->GetDeferral();
        RegisterEventHandlers();

        // Set provider information. This information is localized, so read it from our resources.
        ResourceLoader^ resourceLoader = ResourceLoader::GetForViewIndependentUse();
        m_connection->CompanyName = resourceLoader->GetString("Company");
        m_connection->Name = resourceLoader->GetString("Product");
        m_connection->Version = resourceLoader->GetString("Version");

        m_decodeEngine = ref new DecodeEngine();

        BarcodeSymbologyAttributesBuilder^ attributesBuilder = ref new BarcodeSymbologyAttributesBuilder();

        // Our decode engine does not support any of these features.
        attributesBuilder->IsCheckDigitTransmissionSupported = false;
        attributesBuilder->IsCheckDigitValidationSupported = false;
        attributesBuilder->IsDecodeLengthSupported = false;

        // Tell the system which symbologies we support.
        for (UINT symbology : m_decodeEngine->SupportedSymbologies)
        {
            // Set symbologies supported by the provider
            m_connection->SupportedSymbologies->Append(symbology);

            // Populate a std::map for tracking symbology attributes for each supported symbology.
            // (This step is unnecessary in the case of this sample because none of these properties
            // is marked as supported in our BarcodeSymbologyAttributesBuilder, but we track them
            // in this sample in order to show how it could be done.)
            BarcodeSymbologyAttributes^ defaultAttributes = attributesBuilder->CreateAttributes();
            if (attributesBuilder->IsDecodeLengthSupported)
            {
                // These are the default values, but we set them explicitly for demonstration purposes.
                defaultAttributes->DecodeLength1 = 0;
                defaultAttributes->DecodeLength2 = 0;
                defaultAttributes->DecodeLengthKind = BarcodeSymbologyDecodeLengthKind::AnyLength;
            }
            if (attributesBuilder->IsCheckDigitTransmissionSupported)
            {
                // This is the default value, but we set it explicitly for demonstration purposes.
                defaultAttributes->IsCheckDigitTransmissionEnabled = false;
            }
            if (attributesBuilder->IsCheckDigitValidationSupported)
            {
                // This is the default value, but we set it explicitly for demonstration purposes.
                defaultAttributes->IsCheckDigitValidationEnabled = false;
            }

            m_symbologyAttributes[symbology] = defaultAttributes;
        }

        // Start the connection after
        // 1. Provider information is set
        // 2. Supported symbologies are added
        // 3. All event handlers are registered
        m_connection->Start();
    }

    void Provider::RegisterEventHandlers()
    {
        m_backgroundTask->Canceled += ref new BackgroundTaskCanceledEventHandler(this, &Provider::BackgroundTask_Canceled);
        m_backgroundTask->Task->Completed += ref new BackgroundTaskCompletedEventHandler(this, &Provider::Task_Completed);

        m_connection->EnableScannerRequested += ref new TypedEventHandler<BarcodeScannerProviderConnection^, BarcodeScannerEnableScannerRequestEventArgs^>(this, &Provider::BarcodeScannerProviderConnection_EnableScannerRequested);
        m_connection->DisableScannerRequested += ref new TypedEventHandler<BarcodeScannerProviderConnection^, BarcodeScannerDisableScannerRequestEventArgs^>(this, &Provider::BarcodeScannerProviderConnection_DisableScannerRequested);

        m_connection->GetBarcodeSymbologyAttributesRequested += ref new TypedEventHandler<BarcodeScannerProviderConnection^, BarcodeScannerGetSymbologyAttributesRequestEventArgs^>(this, &Provider::BarcodeScannerProviderConnection_GetBarcodeSymbologyAttributesRequested);
        m_connection->SetBarcodeSymbologyAttributesRequested += ref new TypedEventHandler<BarcodeScannerProviderConnection^, BarcodeScannerSetSymbologyAttributesRequestEventArgs^>(this, &Provider::BarcodeScannerProviderConnection_SetBarcodeSymbologyAttributesRequested);

        m_connection->StartSoftwareTriggerRequested += ref new TypedEventHandler<BarcodeScannerProviderConnection^, BarcodeScannerStartSoftwareTriggerRequestEventArgs^>(this, &Provider::BarcodeScannerProviderConnection_StartSoftwareTriggerRequested);
        m_connection->StopSoftwareTriggerRequested += ref new TypedEventHandler<BarcodeScannerProviderConnection^, BarcodeScannerStopSoftwareTriggerRequestEventArgs^>(this, &Provider::BarcodeScannerProviderConnection_StopSoftwareTriggerRequested);

        m_connection->HideVideoPreviewRequested += ref new TypedEventHandler<BarcodeScannerProviderConnection^, BarcodeScannerHideVideoPreviewRequestEventArgs^>(this, &Provider::BarcodeScannerProviderConnection_HideVideoPreviewRequested);
        m_connection->SetActiveSymbologiesRequested += ref new TypedEventHandler<BarcodeScannerProviderConnection^, BarcodeScannerSetActiveSymbologiesRequestEventArgs^>(this, &Provider::BarcodeScannerProviderConnection_SetActiveSymbologiesRequested);
    }

    void Provider::Task_Completed(BackgroundTaskRegistration^ sender, BackgroundTaskCompletedEventArgs^ args)
    {
        CleanUp();
    }

    void Provider::BackgroundTask_Canceled(IBackgroundTaskInstance^ sender, BackgroundTaskCancellationReason args)
    {
        CleanUp();
    }

    void Provider::BarcodeScannerProviderConnection_EnableScannerRequested(BarcodeScannerProviderConnection^ sender, BarcodeScannerEnableScannerRequestEventArgs^ args)
    {
        // For BarcodeScannerProviderConnection events,
        // a deferral is not required if the only async operation
        // is ReportCompletedAsync or ReportFailedAsync.
        m_scannerEnabled = true;
        create_task(args->Request->ReportCompletedAsync());
    }

    void Provider::BarcodeScannerProviderConnection_DisableScannerRequested(BarcodeScannerProviderConnection^ sender, BarcodeScannerDisableScannerRequestEventArgs^ args)
    {
        m_scannerEnabled = false;

        // Stop the frame reader if we have one.
        task<void> stopFrameReaderTask = task_from_result();
        if (m_frameReader != nullptr)
        {
            stopFrameReaderTask = create_task(m_frameReader->StopAsync());
        }

        stopFrameReaderTask.then([args]()
        {
            return args->Request->ReportCompletedAsync();
        }).then([deferral = args->GetDeferral()]()
        {
            deferral->Complete();
        });
    }

    void Provider::BarcodeScannerProviderConnection_GetBarcodeSymbologyAttributesRequested(BarcodeScannerProviderConnection^ sender, BarcodeScannerGetSymbologyAttributesRequestEventArgs^ args)
    {
        UINT requestedSymbology = args->Request->Symbology;
        if (m_symbologyAttributes.find(requestedSymbology) != m_symbologyAttributes.end())
        {
            create_task(args->Request->ReportCompletedAsync(m_symbologyAttributes[requestedSymbology]));
        }
        else
        {
            create_task(args->Request->ReportFailedAsync(static_cast<int>(E_BOUNDS), L"The requested symbology is not supported."));
        }
    }

    void Provider::BarcodeScannerProviderConnection_SetBarcodeSymbologyAttributesRequested(BarcodeScannerProviderConnection^ sender, BarcodeScannerSetSymbologyAttributesRequestEventArgs^ args)
    {
        UINT requestedSymbology = args->Request->Symbology;
        BarcodeSymbologyAttributes^ newAttributes = args->Request->Attributes;

        if (m_symbologyAttributes.find(requestedSymbology) != m_symbologyAttributes.end())
        {
            BarcodeSymbologyAttributes^ existingAttributes = m_symbologyAttributes[requestedSymbology];
            if (existingAttributes->IsDecodeLengthSupported)
            {
                existingAttributes->DecodeLength1 = newAttributes->DecodeLength1;
                existingAttributes->DecodeLength2 = newAttributes->DecodeLength2;
                existingAttributes->DecodeLengthKind = newAttributes->DecodeLengthKind;
            }
            if (existingAttributes->IsCheckDigitTransmissionSupported)
            {
                existingAttributes->IsCheckDigitTransmissionEnabled = newAttributes->IsCheckDigitTransmissionEnabled;
            }
            if (existingAttributes->IsCheckDigitValidationSupported)
            {
                existingAttributes->IsCheckDigitValidationEnabled = newAttributes->IsCheckDigitValidationEnabled;
            }

            create_task(args->Request->ReportCompletedAsync());
        }
        else
        {
            create_task(args->Request->ReportFailedAsync(static_cast<int>(E_BOUNDS), L"The requested symbology is not supported."));
        }
    }

    void Provider::FrameReader_FrameArrived(BarcodeScannerFrameReader^ sender, BarcodeScannerFrameReaderFrameArrivedEventArgs^ args)
    {
        if (!m_scannerEnabled)
        {
            // Return immediately if decoder is not enabled
            return;
        }

        create_task(sender->TryAcquireLatestFrameAsync()).then([](BarcodeScannerVideoFrame^ frame)
        {
            // Build software bitmap from pixel data if we have any.
            return frame ? SoftwareBitmap::CreateCopyFromBuffer(frame->PixelData, frame->Format, static_cast<int>(frame->Width), static_cast<int>(frame->Height)) : nullptr;
        }).then([this](SoftwareBitmap^ frameBitmap)
        {
            return frameBitmap ? m_decodeEngine->DecodeAsync(frameBitmap) : task_from_result(static_cast<DecodeResult^>(nullptr));
        }).then([this](DecodeResult^ decodeResult)
        {
            // Report only if the result is different from previous one
            if (decodeResult && !DecodeResult::Equals(decodeResult, m_previousDecodeResult))
            {
                m_previousDecodeResult = decodeResult;

                BarcodeScannerReport^ report = CreateBarcodeScannerReportFromString(decodeResult->Text, decodeResult->Symbology);
                return create_task(m_connection->ReportScannedDataAsync(report));
            }
            else
            {
                return task_from_result();
            }
        }).then([deferral = args->GetDeferral()]()
        {
            deferral->Complete();
        });
    }

    void Provider::BarcodeScannerProviderConnection_StartSoftwareTriggerRequested(BarcodeScannerProviderConnection^ sender, BarcodeScannerStartSoftwareTriggerRequestEventArgs^ args)
    {
        task<bool> readerStartedTask = task_from_result(false);

        if (m_scannerEnabled)
        {
            // Create a frame reader to read video frames if we don't have one already.
            task<void> readerCreatedTask = task_from_result();
            if (m_frameReader == nullptr)
            {
                readerCreatedTask = create_task(m_connection->CreateFrameReaderAsync(BitmapPixelFormat::Nv12, BitmapSize{ 1280, 720 }))
                    .then([this](BarcodeScannerFrameReader^ newFrameReader)
                {
                    m_frameReader = newFrameReader;
                    m_frameReader->FrameArrived += ref new TypedEventHandler<BarcodeScannerFrameReader^, BarcodeScannerFrameReaderFrameArrivedEventArgs^>(this, &Provider::FrameReader_FrameArrived);
                });
            }

            readerStartedTask = readerCreatedTask.then([this]()
            {
                return m_frameReader->StartAsync();
            });
        }

        readerStartedTask.then([args](bool success)
        {
            if (success)
            {
                return args->Request->ReportCompletedAsync();
            }
            else
            {
                return args->Request->ReportFailedAsync();
            }
        }).then([deferral = args->GetDeferral()]()
        {
            deferral->Complete();
        });
    }

    void Provider::BarcodeScannerProviderConnection_StopSoftwareTriggerRequested(BarcodeScannerProviderConnection^ sender, BarcodeScannerStopSoftwareTriggerRequestEventArgs^ args)
    {
        // Stop the frame reader if we have one.
        task<void> stopTask = task_from_result();
        if (m_frameReader != nullptr)
        {
            stopTask = create_task(m_frameReader->StopAsync());
        }

        stopTask.then([args]()
        {
            return args->Request->ReportCompletedAsync();
        }).then([deferral = args->GetDeferral()]()
        {
            deferral->Complete();
        });
    }

    void Provider::BarcodeScannerProviderConnection_HideVideoPreviewRequested(BarcodeScannerProviderConnection^ sender, BarcodeScannerHideVideoPreviewRequestEventArgs^ args)
    {
        // There is no video preview running as background task. Simply report completed.
        create_task(args->Request->ReportCompletedAsync());
    }

    void Provider::BarcodeScannerProviderConnection_SetActiveSymbologiesRequested(BarcodeScannerProviderConnection^ sender, BarcodeScannerSetActiveSymbologiesRequestEventArgs^ args)
    {
        auto newActiveSymbologies = ref new Platform::Collections::Vector<UINT>();

        // Ensure symbologies being set to active are supported
        for (auto symbology : args->Request->Symbologies)
        {
            UINT index;
            if (!m_decodeEngine->SupportedSymbologies->IndexOf(symbology, &index))
            {
                // Found invalid symbology
                create_task(args->Request->ReportFailedAsync());
                return;
            }

            newActiveSymbologies->Append(symbology);
        }

        m_decodeEngine->ActiveSymbologies = newActiveSymbologies;

        create_task(args->Request->ReportCompletedAsync());
    }
}