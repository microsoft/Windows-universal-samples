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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Input.Inking;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using Windows.UI.Xaml.Shapes;
using Windows.Foundation;
using System.Linq;
using Windows.UI.Xaml.Media;

namespace SimpleInk
{
    /// <summary>
    /// This page shows the code to do ink selection and cut/copy/paste
    /// </summary>
    public sealed partial class Scenario3 : Page
    {
        private Polyline lasso;
        private Rect boundingRect;

        private MainPage rootPage;

        public Scenario3()
        {
            this.InitializeComponent();

            // Initialize the InkCanvas
            inkCanvas.InkPresenter.InputDeviceTypes = Windows.UI.Core.CoreInputDeviceTypes.Mouse | Windows.UI.Core.CoreInputDeviceTypes.Pen | Windows.UI.Core.CoreInputDeviceTypes.Touch;

            // By default, pen barrel button or right mouse button is processed for inking
            // Set the configuration to instead allow processing these input on the UI thread
            inkCanvas.InkPresenter.InputProcessingConfiguration.RightDragAction = InkInputRightDragAction.LeaveUnprocessed;

            inkCanvas.InkPresenter.UnprocessedInput.PointerPressed += UnprocessedInput_PointerPressed;
            inkCanvas.InkPresenter.UnprocessedInput.PointerMoved += UnprocessedInput_PointerMoved;
            inkCanvas.InkPresenter.UnprocessedInput.PointerReleased += UnprocessedInput_PointerReleased;

            // Handlers to clear the selection when inking or erasing is detected
            inkCanvas.InkPresenter.StrokeInput.StrokeStarted += StrokeInput_StrokeStarted;
            inkCanvas.InkPresenter.StrokesErased += InkPresenter_StrokesErased;

            SizeChanged += Scenario3_SizeChanged;
        }

        private void StrokeInput_StrokeStarted(InkStrokeInput sender, Windows.UI.Core.PointerEventArgs args)
        {
            ClearSelection();
        }

        private void InkPresenter_StrokesErased(InkPresenter sender, InkStrokesErasedEventArgs args)
        {
            ClearSelection();
        }

        private void UnprocessedInput_PointerPressed(InkUnprocessedInput sender, Windows.UI.Core.PointerEventArgs args)
        {
            lasso = new Polyline()
            {
                Stroke = new SolidColorBrush(Windows.UI.Colors.Blue),
                StrokeThickness = 1,
                StrokeDashArray = new DoubleCollection() {5, 2},
            };

            lasso.Points.Add(args.CurrentPoint.RawPosition);

            selectionCanvas.Children.Add(lasso);
        }

        private void UnprocessedInput_PointerMoved(InkUnprocessedInput sender, Windows.UI.Core.PointerEventArgs args)
        {
            lasso.Points.Add(args.CurrentPoint.RawPosition);
        }

        private void UnprocessedInput_PointerReleased(InkUnprocessedInput sender, Windows.UI.Core.PointerEventArgs args)
        {
            lasso.Points.Add(args.CurrentPoint.RawPosition);

            boundingRect = inkCanvas.InkPresenter.StrokeContainer.SelectWithPolyLine(lasso.Points);

            DrawBoundingRect();
        }

        private void DrawBoundingRect()
        {
            selectionCanvas.Children.Clear();

            if ((boundingRect.Width == 0) || (boundingRect.Height == 0) || boundingRect.IsEmpty)
            {
                return;
            }

            var rectangle = new Rectangle()
            {
                Stroke = new SolidColorBrush(Windows.UI.Colors.Blue),
                StrokeThickness = 1,
                StrokeDashArray = new DoubleCollection() {5, 2},
                Width = boundingRect.Width,
                Height = boundingRect.Height
            };

            Canvas.SetLeft(rectangle, boundingRect.X);
            Canvas.SetTop(rectangle, boundingRect.Y);

            selectionCanvas.Children.Add(rectangle);
        }

        private void ClearDrawnBoundingRect()
        {
            if (selectionCanvas.Children.Any())
            {
                selectionCanvas.Children.Clear();
                boundingRect = Rect.Empty;
            }
        }

        private void ClearSelection()
        {
            var strokes = inkCanvas.InkPresenter.StrokeContainer.GetStrokes();
            foreach (var stroke in strokes)
            {
                stroke.Selected = false;
            }
            ClearDrawnBoundingRect();
        }

        private void Scenario3_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            SetCanvasSize();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            SetCanvasSize();
        }

        private void SetCanvasSize()
        {
            outputGrid.Width = RootGrid.ActualWidth;
            outputGrid.Height = RootGrid.ActualHeight / 2;
            inkCanvas.Width = RootGrid.ActualWidth;
            inkCanvas.Height = RootGrid.ActualHeight / 2;
        }

        void OnClear(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            inkCanvas.InkPresenter.StrokeContainer.Clear();
            ClearDrawnBoundingRect();
        }

        void OnCut(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            inkCanvas.InkPresenter.StrokeContainer.CopySelectedToClipboard();
            inkCanvas.InkPresenter.StrokeContainer.DeleteSelected();
            ClearDrawnBoundingRect();
        }

        void OnCopy(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            inkCanvas.InkPresenter.StrokeContainer.CopySelectedToClipboard();
        }

        void OnPaste(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (inkCanvas.InkPresenter.StrokeContainer.CanPasteFromClipboard())
            {
                inkCanvas.InkPresenter.StrokeContainer.PasteFromClipboard(new Point((scrollViewer.HorizontalOffset + 10) / scrollViewer.ZoomFactor, (scrollViewer.VerticalOffset + 10) / scrollViewer.ZoomFactor));
            }
            else
            {
                rootPage.NotifyUser("Cannot paste from clipboard.", NotifyType.ErrorMessage);
            }
        }
    }
}