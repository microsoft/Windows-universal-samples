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
using SDKTemplate;
using System;
using System.Collections.Generic;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Input.Inking;
using Windows.UI.Input.Inking.Analysis;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Shapes;

namespace SDKTemplate
{
    /// <summary>
    /// This scenerio demostrates how to do shape recognition with InkAanlyzer.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage = MainPage.Current;
        InkPresenter inkPresenter;
        InkAnalyzer inkAnalyzer;
        DispatcherTimer dispatcherTimer;

        public Scenario1()
        {
            this.InitializeComponent();

            inkPresenter = inkCanvas.InkPresenter;
            inkPresenter.StrokesCollected += InkPresenter_StrokesCollected;
            inkPresenter.StrokesErased += InkPresenter_StrokesErased;
            inkPresenter.StrokeInput.StrokeStarted += StrokeInput_StrokeStarted;
            inkPresenter.InputDeviceTypes = CoreInputDeviceTypes.Pen | CoreInputDeviceTypes.Mouse | CoreInputDeviceTypes.Touch;

            inkAnalyzer = new InkAnalyzer();

            dispatcherTimer = new DispatcherTimer();
            dispatcherTimer.Tick += DispatcherTimer_Tick;

            // We perform analysis when there has been a change to the
            // ink presenter and the user has been idle for 200ms.
            dispatcherTimer.Interval = TimeSpan.FromMilliseconds(200);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            dispatcherTimer.Stop();
        }

        private void StrokeInput_StrokeStarted(InkStrokeInput sender, PointerEventArgs args)
        {
            // Don't perform analysis while a stroke is in progress.
            dispatcherTimer.Stop();
        }

        private void ClearButton_Click(object sender, RoutedEventArgs e)
        {
            // Don't run analysis when there is nothing to analyze.
            dispatcherTimer.Stop();

            inkPresenter.StrokeContainer.Clear();
            inkAnalyzer.ClearDataForAllStrokes();
            canvas.Children.Clear();
        }

        private void InkPresenter_StrokesCollected(InkPresenter sender, InkStrokesCollectedEventArgs args)
        {
            dispatcherTimer.Stop();
            inkAnalyzer.AddDataForStrokes(args.Strokes);
            dispatcherTimer.Start();
        }

        private void InkPresenter_StrokesErased(InkPresenter sender, InkStrokesErasedEventArgs args)
        {
            dispatcherTimer.Stop();
            foreach (var stroke in args.Strokes)
            {
                inkAnalyzer.RemoveDataForStroke(stroke.Id);
            }
            dispatcherTimer.Start();
        }

        private async void DispatcherTimer_Tick(object sender, object e)
        {
            dispatcherTimer.Stop();
            if (!inkAnalyzer.IsAnalyzing)
            {
                InkAnalysisResult results = await inkAnalyzer.AnalyzeAsync();
                if (results.Status == InkAnalysisStatus.Updated)
                {
                    ConvertShapes();
                }
            }
            else
            {
                // Ink analyzer is busy. Wait a while and try again.
                dispatcherTimer.Start();
            }
        }

        private void ConvertShapes()
        {
            IReadOnlyList<IInkAnalysisNode> drawings = inkAnalyzer.AnalysisRoot.FindNodes(InkAnalysisNodeKind.InkDrawing);
            foreach (IInkAnalysisNode drawing in drawings)
            {
                var shape = (InkAnalysisInkDrawing)drawing;
                if (shape.DrawingKind == InkAnalysisDrawingKind.Drawing)
                {
                    // Omit unsupported shape
                    continue;
                }

                if (shape.DrawingKind == InkAnalysisDrawingKind.Circle || shape.DrawingKind == InkAnalysisDrawingKind.Ellipse)
                {
                    // Create a Circle or Ellipse on the canvas.
                    AddEllipseToCanvas(shape);
                }
                else
                {
                    // Create a Polygon on the canvas.
                    AddPolygonToCanvas(shape);
                }

                // Select the strokes that were recognized, so we can delete them.
                // The effect is that the shape added to the canvas replaces the strokes.
                foreach (var strokeId in shape.GetStrokeIds())
                {
                    InkStroke stroke = inkPresenter.StrokeContainer.GetStrokeById(strokeId);
                    stroke.Selected = true;
                }

                // Remove the recognized strokes from the analyzer
                // so it won't re-analyze them.
                inkAnalyzer.RemoveDataForStrokes(shape.GetStrokeIds());
            }
            inkPresenter.StrokeContainer.DeleteSelected();
        }

        private void AddPolygonToCanvas(InkAnalysisInkDrawing shape)
        {
            Polygon polygon = new Polygon();

            // The points of the polygon are reported clockwise.
            foreach (var point in shape.Points)
            {
                polygon.Points.Add(point);
            }

            canvas.Children.Add(polygon);
        }

        static double Distance(Point p0, Point p1)
        {
            double dX = p1.X - p0.X;
            double dY = p1.Y - p0.Y;
            return Math.Sqrt(dX * dX + dY * dY);
        }

        private void AddEllipseToCanvas(InkAnalysisInkDrawing shape)
        {
            Ellipse ellipse = new Ellipse();

            // Ellipses and circles are reported as four points
            // in clockwise orientation.
            // Points 0 and 2 are the extrema of one axis,
            // and points 1 and 3 are the extrema of the other axis.
            // See Ellipse.svg for a diagram.
            IReadOnlyList<Point> points = shape.Points;

            // Calculate the geometric center of the ellipse.
            var center = new Point((points[0].X + points[2].X) / 2.0, (points[0].Y + points[2].Y) / 2.0);

            // Calculate the length of one axis.
            ellipse.Width = Distance(points[0], points[2]);

            var compositeTransform = new CompositeTransform();
            if(shape.DrawingKind == InkAnalysisDrawingKind.Circle)
            {
                ellipse.Height = ellipse.Width;
            }
            else
            {
                // Calculate the length of the other axis.
                ellipse.Height = Distance(points[1], points[3]);

                // Calculate the amount by which the ellipse has been rotated
                // by looking at the angle our "width" axis has been rotated.
                // Since the Y coordinate is inverted, this calculates the amount
                // by which the ellipse has been rotated clockwise.
                double rotationAngle = Math.Atan2(points[2].Y - points[0].Y, points[2].X - points[0].X);

                RotateTransform rotateTransform = new RotateTransform();
                // Convert radians to degrees.
                compositeTransform.Rotation = rotationAngle * 180.0 / Math.PI;
                compositeTransform.CenterX = ellipse.Width / 2.0;
                compositeTransform.CenterY = ellipse.Height / 2.0;                
            }

            compositeTransform.TranslateX = center.X - ellipse.Width / 2.0;
            compositeTransform.TranslateY = center.Y - ellipse.Height / 2.0;

            ellipse.RenderTransform = compositeTransform;

            canvas.Children.Add(ellipse);
        }
    }
}