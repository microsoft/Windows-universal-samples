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

using System;
using System.Collections.Generic;
using Windows.ApplicationModel.Background;
using Windows.ApplicationModel.Resources;
using Windows.Devices.PointOfService;
using Windows.Devices.PointOfService.Provider;
using Windows.Graphics.Imaging;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;

namespace BarcodeDecoder
{
    internal class Provider : IDisposable
    {
        private DecodeEngine decodeEngine = null;
        private DecodeResult previousDecodeResult = null;
        private IBackgroundTaskInstance backgroundTask = null;
        private bool scannerEnabled = false;
        private Dictionary<uint, BarcodeSymbologyAttributes> symbologyAttributes = new Dictionary<uint, BarcodeSymbologyAttributes>();
        private BackgroundTaskDeferral deferral = null;
        private BarcodeScannerFrameReader frameReader = null;
        private bool disposed = false;
        private BarcodeScannerProviderConnection connection = null;

        readonly int E_BOUNDS = unchecked((int)0x8000000B);

        public Provider(IBackgroundTaskInstance backgroundTask)
        {
            this.backgroundTask = backgroundTask;

            var triggerDetails = (BarcodeScannerProviderTriggerDetails)backgroundTask.TriggerDetails;
            connection = triggerDetails.Connection;

            Initialize();
        }

        ~Provider()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                frameReader?.Dispose();
                frameReader = null;

                connection?.Dispose();
                connection = null;

                deferral?.Complete();
                deferral = null;

                disposed = true;
            }
        }

        public static BarcodeScannerReport CreateBarcodeScannerReportFromString(string scanData, uint symbology)
        {
            IBuffer scanDataBuffer = CryptographicBuffer.ConvertStringToBinary(scanData, BinaryStringEncoding.Utf8);
            return new BarcodeScannerReport(symbology, scanDataBuffer, scanDataBuffer);
        }

        private void Initialize()
        {
            deferral = backgroundTask.GetDeferral();
            RegisterEventHandlers();

            // Set provider information. This information is localized, so read it from our resources.
            ResourceLoader resourceLoader = ResourceLoader.GetForViewIndependentUse();
            connection.CompanyName = resourceLoader.GetString("Company");
            connection.Name = resourceLoader.GetString("Product");
            connection.Version = resourceLoader.GetString("Version");

            decodeEngine = new DecodeEngine();

            // Our decode engine does not support any of these features.
            var attributesBuilder = new BarcodeSymbologyAttributesBuilder
            {
                IsCheckDigitTransmissionSupported = false,
                IsCheckDigitValidationSupported = false,
                IsDecodeLengthSupported = false,
            };

            // Tell the system which symbologies we support.
            foreach (uint symbology in decodeEngine.SupportedSymbologies)
            {
                connection.SupportedSymbologies.Add(symbology);

                // Populate a dictionary for tracking symbology attributes for each supported symbology.
                // (This step is unnecessary in the case of this sample because none of these properties
                // is marked as supported in our BarcodeSymbologyAttributesBuilder, but we track them
                // in this sample in order to show how it could be done.)
                BarcodeSymbologyAttributes defaultAttributes = attributesBuilder.CreateAttributes();
                if (attributesBuilder.IsDecodeLengthSupported)
                {
                    // These are the default values, but we set them explicitly for demonstration purposes.
                    defaultAttributes.DecodeLength1 = 0;
                    defaultAttributes.DecodeLength2 = 0;
                    defaultAttributes.DecodeLengthKind = BarcodeSymbologyDecodeLengthKind.AnyLength;
                }
                if (attributesBuilder.IsCheckDigitTransmissionSupported)
                {
                    // This is the default value, but we set it explicitly for demonstration purposes.
                    defaultAttributes.IsCheckDigitTransmissionEnabled = false;
                }
                if (attributesBuilder.IsCheckDigitValidationSupported)
                {
                    // This is the default value, but we set it explicitly for demonstration purposes.
                    defaultAttributes.IsCheckDigitValidationEnabled = false;
                }
                symbologyAttributes.Add(symbology, defaultAttributes);
            }

            // Start the connection after
            // 1. Provider information is set
            // 2. Supported symbologies are added
            // 3. All event handlers are registered
            connection.Start();
        }

        private void RegisterEventHandlers()
        {
            backgroundTask.Canceled += BackgroundTask_Canceled;
            backgroundTask.Task.Completed += Task_Completed;

            connection.EnableScannerRequested += BarcodeScannerProviderConnection_EnableScannerRequested;
            connection.DisableScannerRequested += BarcodeScannerProviderConnection_DisableScannerRequested;

            connection.GetBarcodeSymbologyAttributesRequested += BarcodeScannerProviderConnection_GetBarcodeSymbologyAttributesRequested;
            connection.SetBarcodeSymbologyAttributesRequested += BarcodeScannerProviderConnection_SetBarcodeSymbologyAttributesRequested;

            connection.StartSoftwareTriggerRequested += BarcodeScannerProviderConnection_StartSoftwareTriggerRequested;
            connection.StopSoftwareTriggerRequested += BarcodeScannerProviderConnection_StopSoftwareTriggerRequested;

            connection.HideVideoPreviewRequested += BarcodeScannerProviderConnection_HideVideoPreviewRequested;
            connection.SetActiveSymbologiesRequested += BarcodeScannerProviderConnection_SetActiveSymbologiesRequested;
        }

        private void Task_Completed(BackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs args)
        {
            Dispose();
        }

        private void BackgroundTask_Canceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            Dispose();
        }

        private async void BarcodeScannerProviderConnection_EnableScannerRequested(BarcodeScannerProviderConnection sender, BarcodeScannerEnableScannerRequestEventArgs args)
        {
            // For BarcodeScannerProviderConnection events,
            // a deferral is not required if the only async operation
            // is ReportCompletedAsync or ReportFailedAsync.
            scannerEnabled = true;
            await args.Request.ReportCompletedAsync();
        }

        private async void BarcodeScannerProviderConnection_DisableScannerRequested(BarcodeScannerProviderConnection sender, BarcodeScannerDisableScannerRequestEventArgs args)
        {
            using (args.GetDeferral())
            {
                scannerEnabled = false;

                // Stop frame reader when disabled
                if (frameReader != null)
                {
                    await frameReader.StopAsync();
                }
                await args.Request.ReportCompletedAsync();
            }
        }

        private async void BarcodeScannerProviderConnection_GetBarcodeSymbologyAttributesRequested(BarcodeScannerProviderConnection sender, BarcodeScannerGetSymbologyAttributesRequestEventArgs args)
        {
            BarcodeSymbologyAttributes attributes;
            if (symbologyAttributes.TryGetValue(args.Request.Symbology, out attributes))
            {
                await args.Request.ReportCompletedAsync(attributes);
            }
            else
            {
                await args.Request.ReportFailedAsync(E_BOUNDS, "The requested symbology is not supported.");
            }
        }

        private async void BarcodeScannerProviderConnection_SetBarcodeSymbologyAttributesRequested(BarcodeScannerProviderConnection sender, BarcodeScannerSetSymbologyAttributesRequestEventArgs args)
        {
            // Update the symbology attributes for the specified symbology.
            BarcodeSymbologyAttributes newAttributes = args.Request.Attributes;
            BarcodeSymbologyAttributes existingAttributes;
            if (symbologyAttributes.TryGetValue(args.Request.Symbology, out existingAttributes))
            {
                // Our sample does not support these properties, but we update them for illustration purposes.
                if (existingAttributes.IsDecodeLengthSupported)
                {
                    existingAttributes.DecodeLength1 = newAttributes.DecodeLength1;
                    existingAttributes.DecodeLength2 = newAttributes.DecodeLength2;
                    existingAttributes.DecodeLengthKind = newAttributes.DecodeLengthKind;
                }
                if (existingAttributes.IsCheckDigitTransmissionSupported)
                {
                    existingAttributes.IsCheckDigitTransmissionEnabled = newAttributes.IsCheckDigitTransmissionEnabled;
                }
                if (existingAttributes.IsCheckDigitValidationSupported)
                {
                    existingAttributes.IsCheckDigitValidationEnabled = newAttributes.IsCheckDigitValidationEnabled;
                }
                // If we supported these properties, we would also have to update the parameters of the decoder
                // at this point.
                await args.Request.ReportCompletedAsync();
            }
            else
            {
                await args.Request.ReportFailedAsync(E_BOUNDS, "The requested symbology is not supported.");
            }
        }

        private async void FrameReader_FrameArrived(BarcodeScannerFrameReader sender, BarcodeScannerFrameReaderFrameArrivedEventArgs args)
        {
            if (!scannerEnabled)
            {
                // Return immediately if decoder is not enabled
                return;
            }

            using (args.GetDeferral())
            {
                SoftwareBitmap bitmap = null;
                using (BarcodeScannerVideoFrame frame = await sender.TryAcquireLatestFrameAsync())
                {
                    if (frame != null)
                    {
                        // Build software bitmap from pixel data
                        bitmap = SoftwareBitmap.CreateCopyFromBuffer(frame.PixelData, frame.Format, (int)frame.Width, (int)frame.Height);
                    }
                }

                if (bitmap != null)
                {
                    DecodeResult decodeResult = await decodeEngine.DecodeAsync(bitmap);

                    bitmap.Dispose();
                    bitmap = null;

                    // Report only if we have a result that is different from previous one.
                    if (decodeResult != null && !decodeResult.Equals(previousDecodeResult))
                    {
                        previousDecodeResult = decodeResult;
                        await connection.ReportScannedDataAsync(CreateBarcodeScannerReportFromString(decodeResult.Text, decodeResult.Symbology));
                    }
                }
            }
        }

        private async void BarcodeScannerProviderConnection_StartSoftwareTriggerRequested(BarcodeScannerProviderConnection sender, BarcodeScannerStartSoftwareTriggerRequestEventArgs args)
        {
            using (args.GetDeferral())
            {
                bool readerStarted = false;

                if (scannerEnabled)
                {
                    if (frameReader == null)
                    {
                        // Create frame reader to read video frames
                        frameReader = await connection.CreateFrameReaderAsync(BitmapPixelFormat.Nv12, new BitmapSize() { Width = 1280, Height = 720 });
                        frameReader.FrameArrived += FrameReader_FrameArrived;
                    }

                    readerStarted = await frameReader.StartAsync();
                }

                if (readerStarted)
                {
                    await args.Request.ReportCompletedAsync();
                }
                else
                {
                    await args.Request.ReportFailedAsync();
                }
            }
        }

        private async void BarcodeScannerProviderConnection_StopSoftwareTriggerRequested(BarcodeScannerProviderConnection sender, BarcodeScannerStopSoftwareTriggerRequestEventArgs args)
        {
            using (args.GetDeferral())
            {
                if (frameReader != null)
                {
                    await frameReader.StopAsync();
                }
                await args.Request.ReportCompletedAsync();
            }
        }

        private async void BarcodeScannerProviderConnection_HideVideoPreviewRequested(BarcodeScannerProviderConnection sender, BarcodeScannerHideVideoPreviewRequestEventArgs args)
        {
            // There is no video preview running as background task. Simply report completed.
            await args.Request.ReportCompletedAsync();
        }

        private async void BarcodeScannerProviderConnection_SetActiveSymbologiesRequested(BarcodeScannerProviderConnection sender, BarcodeScannerSetActiveSymbologiesRequestEventArgs args)
        {
            // Ensure all symbologies being set to active are supported.
            foreach (var symbology in args.Request.Symbologies)
            {
                if (!decodeEngine.SupportedSymbologies.Contains(symbology))
                {
                    // Found invalid symbology.
                    await args.Request.ReportFailedAsync();
                    return;
                }
            }
            decodeEngine.ActiveSymbologies = new List<uint>(args.Request.Symbologies);
            await args.Request.ReportCompletedAsync();
        }
    }
}
