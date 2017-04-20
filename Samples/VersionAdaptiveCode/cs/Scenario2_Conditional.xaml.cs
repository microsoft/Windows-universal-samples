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

using Microsoft.Graphics.Canvas.Effects;
using System.Numerics;
using Windows.Foundation.Metadata;
using Windows.UI.Composition;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Navigation;


namespace SDKTemplate
{
    /// <summary>
    /// This page shows how to use the ApiInformation class in app code to
    // use new blur effects only if supported by the OS.
    /// </summary>
    public sealed partial class Scenario2_Conditional : Page
    {
        private MainPage rootPage;

        private SpriteVisual _destVisual;
        private Compositor _compositor;
        private bool _destinationEffectsPresent = false;
        private Visual _rootVisual;

        public Scenario2_Conditional()
        {
            this.InitializeComponent();

            SetupBlur();
        }

        public void SetupBlur()
        {
            _rootVisual = ElementCompositionPreview.GetElementVisual(this as UIElement);
            _compositor = _rootVisual.Compositor;

            // If the Compositor.CreateBackdropBrush method exists, then set up the blur.
            if (ApiInformation.IsMethodPresent(_compositor.GetType().ToString(), "CreateBackdropBrush"))
            {
                _destinationEffectsPresent = true;
                SetupDestinationEffect(_compositor);
            }
        }

        public void SetupDestinationEffect(Compositor compositor)
        {
            // Create SpriteVisual for blur effect
            _destVisual = compositor.CreateSpriteVisual();
            _destVisual.Size = new Vector2(0, 0);

            // Create graphics effect for blur
            var graphicsEffect = new GaussianBlurEffect
            {
                Name = "GB",
                Source = new CompositionEffectSourceParameter("destinationSource"),
                BlurAmount = 10f,
                BorderMode = EffectBorderMode.Hard,
                Optimization = EffectOptimization.Balanced

            };

            var effectFactory = compositor.CreateEffectFactory(graphicsEffect);

            var blurEffect = effectFactory.CreateBrush();
            var destinationBrush = compositor.CreateBackdropBrush();

            blurEffect.SetSourceParameter("destinationSource", destinationBrush);

            // Set the SpriteVisual's brush as blur effect
            _destVisual.Brush = blurEffect;

            // Insert the destination visual as child of the BlurRect UIElement.
            // Since the BlurRect is between the text of the flyout and the content of the page,
            // the destination visual will blur the content of the page.
            ElementCompositionPreview.SetElementChildVisual(BlurRect, _destVisual);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private void Flyout_Opened(object sender, object e)
        {
            BlurRect.IsHitTestVisible = true;
            if (_destVisual != null)
            {
                _destVisual.Size = new Vector2((float)BlurRect.ActualWidth, (float)BlurRect.ActualHeight);
            }
        }

        private void Flyout_Closed(object sender, object e)
        {
            BlurRect.IsHitTestVisible = false;
            if (_destVisual != null)
            {
                _destVisual.Size = new Vector2(0, 0);
            }
        }
    }
}
