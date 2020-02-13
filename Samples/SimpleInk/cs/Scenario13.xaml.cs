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
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using System.Threading;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Input.Inking;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Shapes;

namespace SDKTemplate
{
    internal enum RecCornor
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    }

    internal enum ManipulationTypes
    {
        None,
        Move,
        Size
    }

    /// <summary>
    /// This page shows the code to configure the InkToolbar.
    /// </summary>
    public sealed partial class Scenario13 : Page
    {
        private Polyline lasso;
        private Rect boundingRect;
        private bool isBoundRect;

        Symbol LassoSelect = (Symbol)0xEF20;
        private Rectangle selectionRectangle;
        private ManipulationTypes currentManipulationType;
        private RecCornor resizePointerCornor;

        public Scenario13()
        {
            this.InitializeComponent();

            // Initialize the InkCanvas
            inkCanvas.InkPresenter.InputDeviceTypes = CoreInputDeviceTypes.Mouse | CoreInputDeviceTypes.Pen;
        }
        
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Registering handlers to clear the selection when inking or erasing is detected
            inkCanvas.InkPresenter.StrokeInput.StrokeStarted += StrokeInput_StrokeStarted;
            inkCanvas.InkPresenter.StrokesErased += InkPresenter_StrokesErased;
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            // Unregistering handlers to clear the selection when inking or erasing is detected
            inkCanvas.InkPresenter.StrokeInput.StrokeStarted -= StrokeInput_StrokeStarted;
            inkCanvas.InkPresenter.StrokesErased -= InkPresenter_StrokesErased;
        }

        private void StrokeInput_StrokeStarted(InkStrokeInput sender, PointerEventArgs args)
        {
            ClearSelection();
            inkCanvas.InkPresenter.UnprocessedInput.PointerPressed -= UnprocessedInput_PointerPressed;
            inkCanvas.InkPresenter.UnprocessedInput.PointerMoved -= UnprocessedInput_PointerMoved;
            inkCanvas.InkPresenter.UnprocessedInput.PointerReleased -= UnprocessedInput_PointerReleased;
        }

        private void InkPresenter_StrokesErased(InkPresenter sender, InkStrokesErasedEventArgs args)
        {
            ClearSelection();
            inkCanvas.InkPresenter.UnprocessedInput.PointerPressed -= UnprocessedInput_PointerPressed;
            inkCanvas.InkPresenter.UnprocessedInput.PointerMoved -= UnprocessedInput_PointerMoved;
            inkCanvas.InkPresenter.UnprocessedInput.PointerReleased -= UnprocessedInput_PointerReleased;
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            HelperFunctions.UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
        }

        private void UnprocessedInput_PointerPressed(InkUnprocessedInput sender, PointerEventArgs args)
        {
            if (SelectedBoudningBoxContainsPosition(args.CurrentPoint.Position))
            {
                return;
            }

            lasso = new Polyline()
            {
                Stroke = new SolidColorBrush(Windows.UI.Colors.Blue),
                StrokeThickness = 1,
                StrokeDashArray = new DoubleCollection() { 5, 2 },
            };

            lasso.Points.Add(args.CurrentPoint.RawPosition);
            selectionCanvas.Children.Add(lasso);
            isBoundRect = true;
        }

        private void UnprocessedInput_PointerMoved(InkUnprocessedInput sender, PointerEventArgs args)
        {
            args.Handled = true;

            if (isBoundRect)
            {
                lasso.Points.Add(args.CurrentPoint.RawPosition);
            }
        }

        private void UnprocessedInput_PointerReleased(InkUnprocessedInput sender, PointerEventArgs args)
        {
            lasso.Points.Add(args.CurrentPoint.RawPosition);

            boundingRect = inkCanvas.InkPresenter.StrokeContainer.SelectWithPolyLine(lasso.Points);
            isBoundRect = false;
            DrawBoundingRect();
        }

        private void DrawBoundingRect()
        {
            selectionCanvas.Children.Clear();
            if (boundingRect.Width <= 0 || boundingRect.Height <= 0)
            {
                return;
            }

            selectionRectangle = new Rectangle()
            {
                Name = "SelectedBox",
                Stroke = new SolidColorBrush(Windows.UI.Colors.Blue),
                Fill = new SolidColorBrush(Windows.UI.Colors.Transparent),
                StrokeThickness = 1,
                StrokeDashArray = new DoubleCollection() { 5, 2 },
                ManipulationMode = ManipulationModes.All,
                RenderTransform = new CompositeTransform(),
                Width = boundingRect.Width,
                Height = boundingRect.Height
            };

            Canvas.SetLeft(selectionRectangle, boundingRect.X);
            Canvas.SetTop(selectionRectangle, boundingRect.Y);
            selectionRectangle.ManipulationStarted += SelectionRectangle_ManipulationStarted;
            selectionRectangle.ManipulationCompleted += SelectionRectangle_ManipulationCompleted;
            selectionRectangle.ManipulationDelta += SelectionRectangle_ManipulationDelta;
            selectionRectangle.PointerExited += SelectionRectangle_PointerExited;
            selectionRectangle.PointerMoved += SelectionRectangle_PointerMoved;
            selectionRectangle.PointerPressed += SelectionRectangle_PointerPressed; ;

            selectionCanvas.Children.Add(selectionRectangle);

        }

        private void SelectionRectangle_ManipulationDelta(object sender, ManipulationDeltaRoutedEventArgs e)
        {
            e.Handled = true;
            var transform = (selectionRectangle.RenderTransform as CompositeTransform);

            switch (currentManipulationType)
            {
                case ManipulationTypes.Move:
                    MoveSelectedInkStrokes(e.Delta.Translation);
                    transform.TranslateX += e.Delta.Translation.X;
                    transform.TranslateY += e.Delta.Translation.Y;
                    break;

                case ManipulationTypes.Size:
                    //TODO: Play with Scaling factor to change speed at which the "box" will grow.
                    var scale = Math.Abs(1 + (float)e.Delta.Translation.X / 100);

                    //TODO: need to stop scaling when the area gets too small, otherwise app will crash. Could be refined.
                    if (selectionRectangle.Width <= 10 && scale < 1f)
                        return;

                    transform.ScaleX *= scale;
                    transform.ScaleY *= scale;

                    var offset = selectionRectangle.ActualOffset;

                    ResizeSelectedInkStrokes(scale, new Vector2(offset.X + (float)transform.TranslateX, offset.Y + (float)transform.TranslateY));
                    break;
            }
        }

        private void SelectionRectangle_ManipulationCompleted(object sender, ManipulationCompletedRoutedEventArgs e)
        {
            currentManipulationType = ManipulationTypes.None;


            Window.Current.CoreWindow.PointerCursor = new CoreCursor(CoreCursorType.Arrow, 0);
        }

        private void SelectionRectangle_ManipulationStarted(object sender, ManipulationStartedRoutedEventArgs e)
        {
            if (currentManipulationType != ManipulationTypes.None)
                return;

            if (Window.Current.CoreWindow.PointerCursor.Type == CoreCursorType.SizeAll)
            {
                currentManipulationType = ManipulationTypes.Move;
            }
            else
            {
                currentManipulationType = ManipulationTypes.Size;

                switch (resizePointerCornor)
                {
                    case RecCornor.TopLeft:
                    case RecCornor.BottomRight:
                        Window.Current.CoreWindow.PointerCursor = new CoreCursor(CoreCursorType.SizeNorthwestSoutheast, 0);
                        break;
                    case RecCornor.TopRight:
                    case RecCornor.BottomLeft:
                        Window.Current.CoreWindow.PointerCursor = new CoreCursor(CoreCursorType.SizeNortheastSouthwest, 0);
                        break;
                }
            }

        }

        private void SelectionRectangle_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            if (currentManipulationType != ManipulationTypes.None)
                return;

            if (Window.Current.CoreWindow.PointerCursor.Type == CoreCursorType.SizeAll)
            {
                currentManipulationType = ManipulationTypes.Move;
            }
            else
            {
                currentManipulationType = ManipulationTypes.Size;

                switch (resizePointerCornor)
                {
                    case RecCornor.TopLeft:
                    case RecCornor.BottomRight:
                        Window.Current.CoreWindow.PointerCursor = new CoreCursor(CoreCursorType.SizeNorthwestSoutheast, 0);
                        break;
                    case RecCornor.TopRight:
                    case RecCornor.BottomLeft:
                        Window.Current.CoreWindow.PointerCursor = new CoreCursor(CoreCursorType.SizeNortheastSouthwest, 0);
                        break;
                }
            }
        }

        private void SelectionRectangle_PointerMoved(object sender, PointerRoutedEventArgs e)
        {
            if (currentManipulationType != ManipulationTypes.None)
                return;

            var mousePos = e.GetCurrentPoint(selectionRectangle).Position;
            Window.Current.CoreWindow.PointerCursor = new CoreCursor(CoreCursorType.SizeAll, 0);
            var offset = 5;
            float recMinX, recMaxX, recMinY, recMaxY;
            recMinX = 0;
            recMinY = 0;

            recMaxX = (float)selectionRectangle.ActualWidth;
            recMaxY = (float)selectionRectangle.ActualHeight;

            if (mousePos.X >= recMinX - offset && mousePos.X <= recMinX + offset &&
                mousePos.Y >= recMinY - offset && mousePos.Y <= recMinY + offset)
            {
                Window.Current.CoreWindow.PointerCursor = new CoreCursor(CoreCursorType.SizeNorthwestSoutheast, 0);
                resizePointerCornor = RecCornor.TopLeft;
                return;
            }
            else if (mousePos.X >= recMaxX - offset && mousePos.X <= recMaxX + offset &&
                mousePos.Y >= recMaxY - offset && mousePos.Y <= recMaxY + offset)
            {
                Window.Current.CoreWindow.PointerCursor = new CoreCursor(CoreCursorType.SizeNorthwestSoutheast, 0);
                resizePointerCornor = RecCornor.BottomRight;
                return;
            }
            else if (mousePos.X >= recMinX - offset && mousePos.X <= recMinX + offset &&
                mousePos.Y >= recMaxY - offset && mousePos.Y <= recMaxY + offset)
            {
                Window.Current.CoreWindow.PointerCursor = new CoreCursor(CoreCursorType.SizeNortheastSouthwest, 0);
                resizePointerCornor = RecCornor.BottomLeft;
                return;
            }
            else if (mousePos.X >= recMaxX - offset && mousePos.X <= recMaxX + offset &&
                mousePos.Y >= recMinY - offset && mousePos.Y <= recMinY + offset)
            {
                Window.Current.CoreWindow.PointerCursor = new CoreCursor(CoreCursorType.SizeNortheastSouthwest, 0);
                resizePointerCornor = RecCornor.TopRight;
                return;
            }

            Debug.WriteLine($"SelectionRectangle_ManipulationCompleted::{resizePointerCornor}");
        }

        private void SelectionRectangle_PointerExited(object sender, PointerRoutedEventArgs e)
        {
            if (currentManipulationType != ManipulationTypes.None)
                return;

            Window.Current.CoreWindow.PointerCursor = new CoreCursor(CoreCursorType.Arrow, 0);
        }

        private void ToolButton_Lasso(object sender, RoutedEventArgs e)
        {
            // By default, pen barrel button or right mouse button is processed for inking
            // Set the configuration to instead allow processing these input on the UI thread
            inkCanvas.InkPresenter.InputProcessingConfiguration.RightDragAction = InkInputRightDragAction.LeaveUnprocessed;

            inkCanvas.InkPresenter.UnprocessedInput.PointerPressed += UnprocessedInput_PointerPressed;
            inkCanvas.InkPresenter.UnprocessedInput.PointerMoved += UnprocessedInput_PointerMoved;
            inkCanvas.InkPresenter.UnprocessedInput.PointerReleased += UnprocessedInput_PointerReleased;
        }

        private void ClearDrawnBoundingRect()
        {

            if (selectionCanvas.Children.Count > 0)
            {
                selectionCanvas.Children.Clear();
                selectionRectangle.ManipulationStarted -= SelectionRectangle_ManipulationStarted;
                selectionRectangle.ManipulationCompleted -= SelectionRectangle_ManipulationCompleted;
                selectionRectangle.ManipulationDelta -= SelectionRectangle_ManipulationDelta;
                selectionRectangle.PointerExited -= SelectionRectangle_PointerExited;
                selectionRectangle.PointerMoved -= SelectionRectangle_PointerMoved;
                selectionRectangle.PointerPressed -= SelectionRectangle_PointerPressed;

                selectionRectangle = null;
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
        
        private bool SelectedBoudningBoxContainsPosition(Point position)
        {
            var contains = !boundingRect.IsEmpty && RectHelper.Contains(boundingRect, position);
            return contains;
        }

        private void ResizeSelectedInkStrokes(float scale, Vector2 center)
        {

            IEnumerable<InkStroke> selectedStrokes = inkCanvas.InkPresenter.StrokeContainer.GetStrokes().Where(x => x.Selected);
            if (selectedStrokes == null)
                return;

            Matrix3x2 matrixSacale = Matrix3x2.CreateScale(scale, center);

            foreach (InkStroke inkStroke in selectedStrokes)
            {
                inkStroke.PointTransform *= matrixSacale;
            }
        }

        private void MoveSelectedInkStrokes(Point pos)
        {
            Matrix3x2 matrix = Matrix3x2.CreateTranslation((float)pos.X, (float)pos.Y);

            if (!matrix.IsIdentity)
            {
                IEnumerable<InkStroke> selectedStrokes = inkCanvas.InkPresenter.StrokeContainer.GetStrokes().Where(x => x.Selected);
                if (selectedStrokes == null)
                    return;
                foreach (InkStroke stroke in selectedStrokes)
                {
                    stroke.PointTransform *= matrix;
                }
            }
        }
    }
}