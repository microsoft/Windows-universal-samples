//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Windows.Media.Capture.Frames;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

namespace SDKTemplate
{
    [ComImport]
    [Guid("5B0D3235-4DBA-4D44-865E-8F1D0E4FD04D")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    unsafe interface IMemoryBufferByteAccess
    {
        void GetBuffer(out byte* buffer, out uint capacity);
    }

    class FrameRenderer
    {
        private Image _imageElement;
        private SoftwareBitmap _backBuffer;
        private bool _taskRunning = false;

        public FrameRenderer(Image imageElement)
        {
            _imageElement = imageElement;
            _imageElement.Source = new SoftwareBitmapSource();
        }

        public void ProcessFrame(MediaFrameReference frame)
        {
            var softwareBitmap = FrameRenderer.ConvertToDisplayableImage(frame?.VideoMediaFrame);

            if (softwareBitmap != null)
            {
                // Swap the processed frame to _backBuffer and trigger UI thread to render it
                softwareBitmap = Interlocked.Exchange(ref _backBuffer, softwareBitmap);

                // UI thread always reset _backBuffer before using it.  Unused bitmap should be disposed.
                softwareBitmap?.Dispose();

                // Changes to xaml ImageElement must happen in UI thread through Dispatcher
                var task = _imageElement.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
                    async () =>
                    {
                        // Don't let two copies of this task run at the same time.
                        if (_taskRunning)
                        {
                            return;
                        }
                        _taskRunning = true;

                        // Keep draining frames from the backbuffer until the backbuffer is empty.
                        SoftwareBitmap latestBitmap;
                        while ((latestBitmap = Interlocked.Exchange(ref _backBuffer, null)) != null)
                        {
                            var imageSource = (SoftwareBitmapSource)_imageElement.Source;
                            await imageSource.SetBitmapAsync(latestBitmap);
                            latestBitmap.Dispose();
                        }

                        _taskRunning = false;
                    });
            }
        }

        // Function delegate that transforms a scanline from an input image to an output image.
        private unsafe delegate void TransformScanline(int pixelWidth, byte* inputRowBytes, byte* outputRowBytes);

        /// <summary>
        /// Converts a frame to a SoftwareBitmap of a valid format to display in an Image control.
        /// </summary>
        /// <param name="inputFrame">Frame to convert.</param>
        public static unsafe SoftwareBitmap ConvertToDisplayableImage(VideoMediaFrame inputFrame)
        {
            SoftwareBitmap result = null;
            using (var inputBitmap = inputFrame?.SoftwareBitmap)
            {
                if (inputBitmap != null)
                {
                    if (inputBitmap.BitmapPixelFormat == BitmapPixelFormat.Bgra8 &&
                        inputBitmap.BitmapAlphaMode == BitmapAlphaMode.Premultiplied)
                    {
                        // SoftwareBitmap is already in the correct format for an Image control, so just return a copy.
                        result = SoftwareBitmap.Copy(inputBitmap);
                    }
                    else if (inputBitmap.BitmapPixelFormat == BitmapPixelFormat.Gray16)
                    {
                        if (inputFrame.FrameReference.SourceKind == MediaFrameSourceKind.Depth)
                        {
                            // Use a special pseudo color to render 16 bits depth frame.
                            var depthScale = (float)inputFrame.DepthMediaFrame.DepthFormat.DepthScaleInMeters;
                            result = TransformBitmap(inputBitmap, (w, i, o) => PseudoColorHelper.PseudoColorForDepth(w, i, o, depthScale));
                        }
                        else
                        {
                            // Use pseudo color to render 16 bits frames.
                            result = TransformBitmap(inputBitmap, PseudoColorHelper.PseudoColorFor16BitInfrared);
                        }
                    }
                    else if (inputBitmap.BitmapPixelFormat == BitmapPixelFormat.Gray8)
                    {
                        // Use pseudo color to render 8 bits frames.
                        result = TransformBitmap(inputBitmap, PseudoColorHelper.PseudoColorFor8BitInfrared);
                    }
                    else
                    {
                        try
                        {
                            // Convert to Bgra8 Premultiplied SoftwareBitmap, so xaml can display in UI.
                            result = SoftwareBitmap.Convert(inputBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
                        }
                        catch (ArgumentException exception)
                        {
                            // Conversion of software bitmap format is not supported.  Drop this frame.
                            System.Diagnostics.Debug.WriteLine(exception.Message);
                        }

                    }
                }
            }
            return result;
        }

        /// <summary>
        /// Transform image into Bgra8 image using given transform method.
        /// </summary>
        /// <param name="softwareBitmap">Input image to transform.</param>
        /// <param name="transformScanline">Method to map pixels in a scanline.</param>
        private static unsafe SoftwareBitmap TransformBitmap(SoftwareBitmap softwareBitmap, TransformScanline transformScanline)
        {
            // XAML Image control only supports premultiplied Bgra8 format.
            var outputBitmap = new SoftwareBitmap(BitmapPixelFormat.Bgra8,
                softwareBitmap.PixelWidth, softwareBitmap.PixelHeight, BitmapAlphaMode.Premultiplied);

            using (var input = softwareBitmap.LockBuffer(BitmapBufferAccessMode.Read))
            using (var output = outputBitmap.LockBuffer(BitmapBufferAccessMode.Write))
            {
                // Get stride values to calculate buffer position for a given pixel x and y position.
                int inputStride = input.GetPlaneDescription(0).Stride;
                int outputStride = output.GetPlaneDescription(0).Stride;
                int pixelWidth = softwareBitmap.PixelWidth;
                int pixelHeight = softwareBitmap.PixelHeight;

                using (var outputReference = output.CreateReference())
                using (var inputReference = input.CreateReference())
                {
                    // Get input and output byte access buffers.
                    byte* inputBytes;
                    uint inputCapacity;
                    ((IMemoryBufferByteAccess)inputReference).GetBuffer(out inputBytes, out inputCapacity);
                    byte* outputBytes;
                    uint outputCapacity;
                    ((IMemoryBufferByteAccess)outputReference).GetBuffer(out outputBytes, out outputCapacity);

                    // Iterate over all pixels and store converted value.
                    for (int y = 0; y < pixelHeight; y++)
                    {
                        byte* inputRowBytes = inputBytes + y * inputStride;
                        byte* outputRowBytes = outputBytes + y * outputStride;

                        transformScanline(pixelWidth, inputRowBytes, outputRowBytes);
                    }
                }
            }
            return outputBitmap;
        }

        /// <summary>
        /// A helper class to manage look-up-table for pseudo-colors.
        /// </summary>
        private static class PseudoColorHelper
        {
            #region Constructor, private members and methods

            private const int TableSize = 1024;   // Look up table size
            private static readonly uint[] PseudoColorTable;
            private static readonly uint[] InfraredRampTable;

            // Color palette mapping value from 0 to 1 to blue to red colors.
            private static readonly Color[] ColorRamp =
            {
                Color.FromArgb(a:0xFF, r:0x7F, g:0x00, b:0x00),
                Color.FromArgb(a:0xFF, r:0xFF, g:0x00, b:0x00),
                Color.FromArgb(a:0xFF, r:0xFF, g:0x7F, b:0x00),
                Color.FromArgb(a:0xFF, r:0xFF, g:0xFF, b:0x00),
                Color.FromArgb(a:0xFF, r:0x7F, g:0xFF, b:0x7F),
                Color.FromArgb(a:0xFF, r:0x00, g:0xFF, b:0xFF),
                Color.FromArgb(a:0xFF, r:0x00, g:0x7F, b:0xFF),
                Color.FromArgb(a:0xFF, r:0x00, g:0x00, b:0xFF),
                Color.FromArgb(a:0xFF, r:0x00, g:0x00, b:0x7F),
            };

            static PseudoColorHelper()
            {
                PseudoColorTable = InitializePseudoColorLut();
                InfraredRampTable = InitializeInfraredRampLut();
            }

            /// <summary>
            /// Maps an input infrared value between [0, 1] to corrected value between [0, 1].
            /// </summary>
            /// <param name="value">Input value between [0, 1].</param>
            [MethodImpl(MethodImplOptions.AggressiveInlining)]  // Tell the compiler to inline this method to improve performance
            private static uint InfraredColor(float value)
            {
                int index = (int)(value * TableSize);
                index = index < 0 ? 0 : index > TableSize - 1 ? TableSize - 1 : index;
                return InfraredRampTable[index];
            }

            /// <summary>
            /// Initializes the pseudo-color look up table for infrared pixels
            /// </summary>
            private static uint[] InitializeInfraredRampLut()
            {
                uint[] lut = new uint[TableSize];
                for (int i = 0; i < TableSize; i++)
                {
                    var value = (float)i / TableSize;
                    // Adjust to increase color change between lower values in infrared images
                    var alpha = (float)Math.Pow(1 - value, 12);
                    lut[i] = ColorRampInterpolation(alpha);
                }
                return lut;
            }

            /// <summary>
            /// Initializes pseudo-color look up table for depth pixels
            /// </summary>
            private static uint[] InitializePseudoColorLut()
            {
                uint[] lut = new uint[TableSize];
                for (int i = 0; i < TableSize; i++)
                {
                    lut[i] = ColorRampInterpolation((float)i / TableSize);
                }
                return lut;
            }

            /// <summary>
            /// Maps a float value to a pseudo-color pixel
            /// </summary>
            private static uint ColorRampInterpolation(float value)
            {
                // Map value to surrounding indexes on the color ramp
                int rampSteps = ColorRamp.Length - 1;
                float scaled = value * rampSteps;
                int integer = (int)scaled;
                int index =
                    integer < 0 ? 0 :
                    integer >= rampSteps - 1 ? rampSteps - 1 :
                    integer;
                Color prev = ColorRamp[index];
                Color next = ColorRamp[index + 1];

                // Set color based on ratio of closeness between the surrounding colors
                uint alpha = (uint)((scaled - integer) * 255);
                uint beta = 255 - alpha;
                return
                    ((prev.A * beta + next.A * alpha) / 255) << 24 | // Alpha
                    ((prev.R * beta + next.R * alpha) / 255) << 16 | // Red
                    ((prev.G * beta + next.G * alpha) / 255) << 8 |  // Green
                    ((prev.B * beta + next.B * alpha) / 255);        // Blue
            }

            /// <summary>
            /// Maps a value in [0, 1] to a pseudo RGBA color.
            /// </summary>
            /// <param name="value">Input value between [0, 1].</param>
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            private static uint PseudoColor(float value)
            {
                int index = (int)(value * TableSize);
                index = index < 0 ? 0 : index > TableSize - 1 ? TableSize - 1 : index;
                return PseudoColorTable[index];
            }

            #endregion

            /// <summary>
            /// Maps each pixel in a scanline from a 16 bit depth value to a pseudo-color pixel.
            /// </summary>
            /// /// <param name="pixelWidth">Width of the input scanline, in pixels.</param>
            /// /// <param name="inputRowBytes">Pointer to the start of the input scanline.</param>
            /// /// <param name="outputRowBytes">Pointer to the start of the output scanline.</param>
            /// /// /// <param name="depthScale">Physical distance that corresponds to one unit in the input scanline.</param>
            public static unsafe void PseudoColorForDepth(int pixelWidth, byte* inputRowBytes, byte* outputRowBytes, float depthScale)
            {
                // Visualize space in front of your desktop.
                const float min = 0.5f;  // 0.5 meters
                const float max = 4.0f;  // 4 meters
                const float one_min = 1.0f / min;
                const float range = 1.0f / max - one_min;

                ushort* inputRow = (ushort*)inputRowBytes;
                uint* outputRow = (uint*)outputRowBytes;
                for (int x = 0; x < pixelWidth; x++)
                {
                    var depth = inputRow[x] * depthScale;

                    if (depth == 0)
                    {
                        // Map invalid depth values to transparent pixels.
                        // This happens when depth information cannot be calculated, e.g. when objects are too close.
                        outputRow[x] = 0;
                    }
                    else
                    {
                        var alpha = (1.0f / depth - one_min) / range;
                        outputRow[x] = PseudoColor(alpha * alpha);
                    }
                }
            }

            /// <summary>
            /// Maps each pixel in a scanline from a 8 bit infrared value to a pseudo-color pixel.
            /// </summary>
            /// /// <param name="pixelWidth">Width of the input scanline, in pixels.</param>
            /// <param name="inputRowBytes">Pointer to the start of the input scanline.</param>
            /// <param name="outputRowBytes">Pointer to the start of the output scanline.</param>
            public static unsafe void PseudoColorFor8BitInfrared(
                int pixelWidth, byte* inputRowBytes, byte* outputRowBytes)
            {
                byte* inputRow = inputRowBytes;
                uint* outputRow = (uint*)outputRowBytes;
                for (int x = 0; x < pixelWidth; x++)
                {
                    outputRow[x] = InfraredColor(inputRow[x] / (float)Byte.MaxValue);
                }
            }

            /// <summary>
            /// Maps each pixel in a scanline from a 16 bit infrared value to a pseudo-color pixel.
            /// </summary>
            /// <param name="pixelWidth">Width of the input scanline.</param>
            /// <param name="inputRowBytes">Pointer to the start of the input scanline.</param>
            /// <param name="outputRowBytes">Pointer to the start of the output scanline.</param>
            public static unsafe void PseudoColorFor16BitInfrared(int pixelWidth, byte* inputRowBytes, byte* outputRowBytes)
            {
                ushort* inputRow = (ushort*)inputRowBytes;
                uint* outputRow = (uint*)outputRowBytes;
                for (int x = 0; x < pixelWidth; x++)
                {
                    outputRow[x] = InfraredColor(inputRow[x] / (float)UInt16.MaxValue);
                }
            }
        }
    }
}
