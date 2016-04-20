using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.Foundation;
using Windows.Graphics.DirectX;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace AnimatedGif
{
    public sealed partial class GifView : UserControl
    {

        public static DependencyProperty StretchProperty { get; } = DependencyProperty.Register(
            nameof(Stretch),
            typeof(Stretch),
            typeof(GifView),
            new PropertyMetadata(Stretch.Uniform)
            );

        public Stretch Stretch
        {
            get { return (Stretch)GetValue(StretchProperty); }
            set { SetValue(StretchProperty, value); }
        }

        public static DependencyProperty UriSourceProperty { get; } = DependencyProperty.Register(
            nameof(UriSource),
            typeof(Uri),
            typeof(GifView),
            new PropertyMetadata(null, new PropertyChangedCallback(OnSourcePropertyChanged))
            );

        public Uri UriSource
        {
            get { return (Uri)GetValue(UriSourceProperty); }
            set { SetValue(UriSourceProperty, value); }
        }

        private GifPresenter _gifPresenter;
        private bool _isLoaded;

        private CancellationTokenSource _initializationCancellationTokenSource;

        public GifView()
        {
            this.InitializeComponent();

            // Register for visibility changed to stop the timer when minimized
            Window.Current.VisibilityChanged += Window_VisibilityChanged;
            // Register for SurfaceContentsLost to recreate the image source if necessary
            CompositionTarget.SurfaceContentsLost += CompositionTarget_SurfaceContentsLost;
        }

        private async static void OnSourcePropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var that = d as GifView;
            await that?.UpdateSourceAsync();
        }

        private async Task UpdateSourceAsync()
        {
            _gifPresenter?.StopAnimation();
            _initializationCancellationTokenSource?.Cancel();

            GifImage.Source = null;
            _gifPresenter = null;

            if (UriSource != null)
            {
                var uriSource = UriSource;
                var cancellationTokenSource = new CancellationTokenSource();

                _initializationCancellationTokenSource = cancellationTokenSource;

                try
                {
                    var streamReference = RandomAccessStreamReference.CreateFromUri(uriSource);
                    var inMemoryStream = new InMemoryRandomAccessStream();

                    using (inMemoryStream)
                    {
                        var readStream = await streamReference.OpenReadAsync().AsTask(cancellationTokenSource.Token);

                        if (readStream.ContentType.ToLowerInvariant() != "image/gif")
                        {
                            throw new ArgumentException("Unsupported content type: " + readStream.ContentType);
                        }

                        var copyAction = RandomAccessStream.CopyAndCloseAsync(
                                readStream.GetInputStreamAt(0L),
                                inMemoryStream.GetOutputStreamAt(0L)
                                );
                        await copyAction.AsTask(cancellationTokenSource.Token);
                        
                        if (uriSource.Equals(UriSource))
                        {
                            var gifPresenter = new GifPresenter();

                            GifImage.Source = await gifPresenter.InitializeAsync(inMemoryStream);
                            _gifPresenter = gifPresenter;

                            if (_isLoaded)
                            {
                                _gifPresenter.StartAnimation();
                            }
                        }
                    }
                }
                catch (TaskCanceledException)
                {
                    // Just keep the empty image source.
                }
                catch (FileNotFoundException)
                {
                    // Just keep the empty image source.
                }
            }
        }

        #region Control lifecycle

        private void Window_VisibilityChanged(object sender, VisibilityChangedEventArgs e)
        {
            if (e.Visible)
            {
                _gifPresenter?.StartAnimation();
            }
            else if (!e.Visible)
            {
                _gifPresenter?.StopAnimation(); // Prevent unnecessary work
            }
        }

        private void GifImage_Loaded(object sender, RoutedEventArgs e)
        {
            _isLoaded = true;
            _gifPresenter?.StartAnimation();
        }

        private void GifImage_Unloaded(object sender, RoutedEventArgs e)
        {
            _isLoaded = false;
            _gifPresenter?.StopAnimation();
        }

        private void CompositionTarget_SurfaceContentsLost(object sender, object e)
        {
            GifImage.Source = _gifPresenter?.RecreateSurfaces();
        }

        #endregion

        private class GifPresenter
        {
            #region Private struct declarations
            private struct ImageProperties
            {
                public readonly int PixelWidth;
                public readonly int PixelHeight;
                public readonly bool IsAnimated;
                public readonly int LoopCount;

                public ImageProperties(int pixelWidth, int pixelHeight, bool isAnimated, int loopCount)
                {
                    PixelWidth = pixelWidth;
                    PixelHeight = pixelHeight;
                    IsAnimated = isAnimated;
                    LoopCount = loopCount;
                }
            }

            private struct FrameProperties
            {
                public readonly Rect Rect;
                public readonly double DelayMilliseconds;
                public readonly bool ShouldDispose;

                public FrameProperties(Rect rect, double delayMilliseconds, bool shouldDispose)
                {
                    Rect = rect;
                    DelayMilliseconds = delayMilliseconds;
                    ShouldDispose = shouldDispose;
                }
            }

            #endregion

            private DispatcherTimer _animationTimer;

            private int _currentFrameIndex;
            private int _completedLoops;

            private BitmapDecoder _bitmapDecoder;
            private ImageProperties _imageProperties;
            private IList<FrameProperties> _frameProperties;

            private CanvasImageSource _canvasImageSource;
            private CanvasRenderTarget _accumulationRenderTarget;

            private bool _isInitialized;
            private bool _isAnimating;
            private bool _hasCanvasResources;

            public async Task<ImageSource> InitializeAsync(IRandomAccessStream streamSource)
            {
                var bitmapDecoder = await BitmapDecoder.CreateAsync(BitmapDecoder.GifDecoderId, streamSource);
                var imageProperties = await RetrieveImagePropertiesAsync(bitmapDecoder);
                var frameProperties = new List<FrameProperties>();

                for (var i = 0u; i < bitmapDecoder.FrameCount; i++)
                {
                    var bitmapFrame = await bitmapDecoder.GetFrameAsync(i);
                    frameProperties.Add(await RetrieveFramePropertiesAsync(bitmapFrame));
                }

                _frameProperties = frameProperties;
                _bitmapDecoder = bitmapDecoder;
                _imageProperties = imageProperties;

                CreateCanvasResources();

                _isInitialized = true;
                return _canvasImageSource;
            }

            public void StartAnimation()
            {
                if (_isInitialized)
                {
                    _currentFrameIndex = 0;
                    _completedLoops = 0;

                    _animationTimer?.Stop();

                    _animationTimer = new DispatcherTimer();
                    _animationTimer.Tick += AnimationTimer_Tick;
                    _animationTimer.Interval = TimeSpan.Zero;
                    _animationTimer.Start();

                    _isAnimating = true;
                }
            }

            private async void AnimationTimer_Tick(object sender, object e)
            {
                await AdvanceFrame();
            }

            public void StopAnimation()
            {
                _animationTimer?.Stop();
                _isAnimating = false;
            }

            private async Task AdvanceFrame()
            {
                if (_bitmapDecoder.FrameCount == 0)
                {
                    return;
                }

                var frameIndex = _currentFrameIndex;
                var frameProperties = _frameProperties[frameIndex];

                // Increment frame index and loop count
                _currentFrameIndex++;
                if (_currentFrameIndex >= _bitmapDecoder.FrameCount)
                {
                    _completedLoops++;
                    _currentFrameIndex = 0;
                }

                // Set up the timer to display the next frame
                if (_imageProperties.IsAnimated &&
                    (_imageProperties.LoopCount == 0 || _completedLoops < _imageProperties.LoopCount))
                {
                    _animationTimer.Interval = TimeSpan.FromMilliseconds(frameProperties.DelayMilliseconds);
                }
                else
                {
                    _animationTimer.Stop();
                }

                // Decode the frame
                var frame = await _bitmapDecoder.GetFrameAsync((uint)frameIndex);
                var pixelData = await frame.GetPixelDataAsync(
                    BitmapPixelFormat.Bgra8,
                    BitmapAlphaMode.Premultiplied,
                    new BitmapTransform(),
                    ExifOrientationMode.IgnoreExifOrientation,
                    ColorManagementMode.DoNotColorManage
                    );
                var pixels = pixelData.DetachPixelData();
                var frameRectangle = frameProperties.Rect;
                var disposeRectangle = Rect.Empty;

                if (frameIndex > 0)
                {
                    var previousFrameProperties = _frameProperties[frameIndex - 1];
                    if (previousFrameProperties.ShouldDispose)
                    {
                        // Clear the pixels from the last frame
                        disposeRectangle = previousFrameProperties.Rect;
                    }
                }
                else
                {
                    disposeRectangle = new Rect(0, 0, _imageProperties.PixelWidth, _imageProperties.PixelHeight);
                }


                // Compose and display the frame
                try
                {
                    PrepareFrame(pixels, frameRectangle, disposeRectangle);
                    UpdateImageSource(frameRectangle);
                }
                catch (Exception e) when (_canvasImageSource.Device.IsDeviceLost(e.HResult))
                {
                    // XAML will also raise a SurfaceContentsLost event, and we use this to trigger
                    // redrawing the surface. Therefore, ignore this error.
                }
            }

            private void CreateCanvasResources()
            {
                const float desiredDpi = 96.0f; // GIF does not encode DPI

                var sharedDevice = GetSharedDevice();
                var pixelWidth = _imageProperties.PixelWidth;
                var pixelHeight = _imageProperties.PixelHeight;

                _canvasImageSource = new CanvasImageSource(
                    sharedDevice,
                    pixelWidth,
                    pixelHeight,
                    desiredDpi,
                    CanvasAlphaMode.Premultiplied
                    );
                _accumulationRenderTarget = new CanvasRenderTarget(
                    sharedDevice,
                    pixelWidth,
                    pixelHeight,
                    desiredDpi,
                    DirectXPixelFormat.B8G8R8A8UIntNormalized,
                    CanvasAlphaMode.Premultiplied
                    );

                _hasCanvasResources = true;
            }

            public ImageSource RecreateSurfaces()
            {
                if (_hasCanvasResources)
                {
                    var sharedDevice = GetSharedDevice();

                    _canvasImageSource = new CanvasImageSource(
                        sharedDevice,
                        _imageProperties.PixelWidth,
                        _imageProperties.PixelHeight,
                        _canvasImageSource.Dpi,
                        _canvasImageSource.AlphaMode
                        );
                    _accumulationRenderTarget = new CanvasRenderTarget(
                        sharedDevice,
                        _imageProperties.PixelWidth,
                        _imageProperties.PixelHeight,
                        _accumulationRenderTarget.Dpi,
                        _accumulationRenderTarget.Format,
                        _accumulationRenderTarget.AlphaMode
                        );

                    if (_isAnimating)
                    {
                        StartAnimation(); // Reset animation as the accumulation buffer is invalid
                    }

                    return _canvasImageSource;
                }
                else
                {
                    return null;
                }
            }

            private void PrepareFrame(byte[] pixels, Rect frameRectangle, Rect disposeRectangle)
            {
                var sharedDevice = GetSharedDevice();

                // Create hardware bitmap from decoded frame
                var frameBitmap = CanvasBitmap.CreateFromBytes(
                    sharedDevice,
                    pixels,
                    (int)frameRectangle.Width,
                    (int)frameRectangle.Height,
                    DirectXPixelFormat.B8G8R8A8UIntNormalized
                    );

                // Draw bitmap to render target, potentially on top of the previous frame
                using (var drawingSession = _accumulationRenderTarget.CreateDrawingSession())
                using (frameBitmap)
                {
                    if (!disposeRectangle.IsEmpty)
                    {
                        using (drawingSession.CreateLayer(1.0f, disposeRectangle))
                        {
                            drawingSession.Clear(Colors.Transparent);
                        }
                    }

                    drawingSession.DrawImage(frameBitmap, frameRectangle);
                }
            }

            private void UpdateImageSource(Rect updateRectangle)
            {
                if (Window.Current.Visible)
                {
                    var imageRectangle = new Rect(new Point(), _canvasImageSource.Size);
                    updateRectangle.Intersect(imageRectangle);

                    using (var drawingSession = _canvasImageSource.CreateDrawingSession(Colors.Transparent))
                    {
                        drawingSession.DrawImage(_accumulationRenderTarget); // Render target has the composed frame
                    }
                }
            }

            private CanvasDevice GetSharedDevice()
            {
                return CanvasDevice.GetSharedDevice(forceSoftwareRenderer: false);
            }

            #region GIF file format helpers

            private static async Task<ImageProperties> RetrieveImagePropertiesAsync(BitmapDecoder bitmapDecoder)
            {
                // Properties not currently supported: background color, pixel aspect ratio.
                const string widthProperty = "/logscrdesc/Width";
                const string heightProperty = "/logscrdesc/Height";
                const string applicationProperty = "/appext/application";
                const string dataProperty = "/appext/data";

                var propertiesView = bitmapDecoder.BitmapContainerProperties;
                var requiredProperties = new[] { widthProperty, heightProperty };
                var properties = await propertiesView.GetPropertiesAsync(requiredProperties);

                var pixelWidth = (ushort)properties[widthProperty].Value;
                var pixelHeight = (ushort)properties[heightProperty].Value;

                var loopCount = 0; // Repeat forever by default
                var isAnimated = true;

                try
                {
                    var extensionProperties = new[] { applicationProperty, dataProperty };
                    properties = await propertiesView.GetPropertiesAsync(extensionProperties);

                    if (properties.ContainsKey(applicationProperty) &&
                        properties[applicationProperty].Type == PropertyType.UInt8Array)
                    {
                        var bytes = (byte[])properties[applicationProperty].Value;
                        var applicationName = Encoding.UTF8.GetString(bytes, 0, bytes.Length);

                        if (applicationName == "NETSCAPE2.0" || applicationName == "ANIMEXTS1.0")
                        {
                            if (properties.ContainsKey(dataProperty) && properties[dataProperty].Type == PropertyType.UInt8Array)
                            {
                                //  The data is in the following format: 
                                //  byte 0: extsize (must be > 1) 
                                //  byte 1: loopType (1 == animated gif) 
                                //  byte 2: loop count (least significant byte) 
                                //  byte 3: loop count (most significant byte) 
                                //  byte 4: set to zero 

                                var data = (byte[])properties[dataProperty].Value;
                                loopCount = data[2] | data[3] << 8;
                                isAnimated = data[1] == 1;
                            }
                        }
                    }
                }
                catch
                {
                    // These properties are not required, so it's okay to ignore failure.
                }

                return new ImageProperties(pixelWidth, pixelHeight, isAnimated, loopCount);
            }

            private async Task<FrameProperties> RetrieveFramePropertiesAsync(BitmapFrame frame)
            {
                const string leftProperty = "/imgdesc/Left";
                const string topProperty = "/imgdesc/Top";
                const string widthProperty = "/imgdesc/Width";
                const string heightProperty = "/imgdesc/Height";
                const string delayProperty = "/grctlext/Delay";
                const string disposalProperty = "/grctlext/Disposal";

                var propertiesView = frame.BitmapProperties;
                var requiredProperties = new[] { leftProperty, topProperty, widthProperty, heightProperty };
                var properties = await propertiesView.GetPropertiesAsync(requiredProperties);

                var left = (ushort)properties[leftProperty].Value;
                var top = (ushort)properties[topProperty].Value;
                var width = (ushort)properties[widthProperty].Value;
                var height = (ushort)properties[heightProperty].Value;

                var delayMilliseconds = 30.0;
                var shouldDispose = false;

                try
                {
                    var extensionProperties = new[] { delayProperty, disposalProperty };
                    properties = await propertiesView.GetPropertiesAsync(extensionProperties);

                    if (properties.ContainsKey(delayProperty) && properties[delayProperty].Type == PropertyType.UInt16)
                    {
                        var delayInHundredths = (ushort)properties[delayProperty].Value;
                        if (delayInHundredths >= 3u) // Prevent degenerate frames with no delay time
                        {
                            delayMilliseconds = 10.0 * delayInHundredths;
                        }
                    }

                    if (properties.ContainsKey(disposalProperty) && properties[disposalProperty].Type == PropertyType.UInt8)
                    {
                        var disposal = (byte)properties[disposalProperty].Value;
                        if (disposal == 2)
                        {
                            // 0 = undefined
                            // 1 = none (compose next frame on top of this one, default)
                            // 2 = dispose
                            // 3 = revert to previous (not supported)
                            shouldDispose = true;
                        }
                    }
                }
                catch
                {
                    // These properties are not required, so it's okay to ignore failure.
                }

                return new FrameProperties(
                    new Rect(left, top, width, height),
                    delayMilliseconds,
                    shouldDispose
                    );
            }

            #endregion

        }
    }
}
