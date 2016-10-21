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
using Windows.Foundation;
using Windows.UI.Input.Inking;
using Windows.UI.Input.Inking.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// This page shows the code to do ink recognition
    /// </summary>
    public sealed partial class Scenario8 : Page
    {
        const float radius = 150.0f;
        const float circleStrokeThickness = 2.0f;
        const float penSize = 4.0f;
        Point circleCenter;

        CoreWetStrokeUpdateSource coreWetStrokeUpdateSource;

        // Follow Circle is true if Ink input should be redirected to the circle edge
        bool followCircle = false;

        // discontinueStroke is true is the Ink input that follows for the wet stroke is to be ignored
        bool discontinueStroke = false;

        // disableFollowCircle is set to true to disable Follow Circle behavior for the current stroke
        bool disableFollowCircle = false;

        public Scenario8()
        {
            this.InitializeComponent();

            InkCanvas.InkPresenter.InputDeviceTypes = Windows.UI.Core.CoreInputDeviceTypes.Mouse | Windows.UI.Core.CoreInputDeviceTypes.Pen;

            var drawingAttributes = InkCanvas.InkPresenter.CopyDefaultDrawingAttributes();
            drawingAttributes.Size = new Size(penSize, penSize);
            InkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(drawingAttributes);

            coreWetStrokeUpdateSource = CoreWetStrokeUpdateSource.Create(InkCanvas.InkPresenter);
            coreWetStrokeUpdateSource.WetStrokeStarting += CoreWetStrokeUpdateSource_WetStrokeStarting;
            coreWetStrokeUpdateSource.WetStrokeContinuing += CoreWetStrokeUpdateSource_WetStrokeContinuing;
            coreWetStrokeUpdateSource.WetStrokeStopping += CoreWetStrokeUpdateSource_WetStrokeStopping;
            coreWetStrokeUpdateSource.WetStrokeCompleted += CoreWetStrokeUpdateSource_WetStrokeCompleted;
            coreWetStrokeUpdateSource.WetStrokeCanceled += CoreWetStrokeUpdateSource_WetStrokeCanceled;
        }

        private void InkStackPanel_Loaded(object sender, RoutedEventArgs e)
        {
            InkCanvas.Width = InkStackPanel.ActualWidth;
            InkCanvas.Height = InkStackPanel.ActualHeight;

            circleCenter = new Point(InkCanvas.Width / 2.0f, InkCanvas.Height / 2.0f);
            InkCircle.Center = circleCenter;
            InkCircle.RadiusX = radius;
            InkCircle.RadiusY = radius;
            InkCirclePath.StrokeThickness = circleStrokeThickness;
        }

        private void CoreWetStrokeUpdateSource_WetStrokeStarting(CoreWetStrokeUpdateSource sender, CoreWetStrokeUpdateEventArgs args)
        {
            followCircle = disableFollowCircle ? false : FollowCircleTest(args);
            if (followCircle)
            {
                HandleFollowCircleInput(args);
            }
            else
            {
                disableFollowCircle = true;
                HandleRegularInput(args);
            }
        }

        private void CoreWetStrokeUpdateSource_WetStrokeContinuing(CoreWetStrokeUpdateSource sender, CoreWetStrokeUpdateEventArgs args)
        {
            if (followCircle)
            {
                HandleFollowCircleInput(args);
            }
            else
            {
                HandleRegularInput(args);
            }
        }

        private void CoreWetStrokeUpdateSource_WetStrokeStopping(CoreWetStrokeUpdateSource sender, CoreWetStrokeUpdateEventArgs args)
        {
            if (followCircle)
            {
                HandleFollowCircleInput(args);
            }
            else
            {
                HandleRegularInput(args);
            }

            disableFollowCircle = false;
        }

        private void CoreWetStrokeUpdateSource_WetStrokeCompleted(CoreWetStrokeUpdateSource sender, CoreWetStrokeUpdateEventArgs args)
        {
            followCircle = false;
            discontinueStroke = false;
        }

        private void CoreWetStrokeUpdateSource_WetStrokeCanceled(CoreWetStrokeUpdateSource sender, CoreWetStrokeUpdateEventArgs args)
        {
            followCircle = false;
            discontinueStroke = false;
            disableFollowCircle = false;
        }

        private bool FollowCircleTest(CoreWetStrokeUpdateEventArgs args)
        {
            var position = args.NewInkPoints[0].Position;
            var distanceFromCenter = DistanceFromCenter(position);

            return InCircleEdgeZone(distanceFromCenter);
        }

        private void HandleFollowCircleInput(CoreWetStrokeUpdateEventArgs args)
        {
            if (!discontinueStroke)
            {
                for (int i = 0; i < args.NewInkPoints.Count; i++)
                {
                    var position = args.NewInkPoints[i].Position;
                    var distanceFromCenter = DistanceFromCenter(position);

                    if (InCircleEdgeZone(distanceFromCenter))
                    {
                        // Calculate position of new InkPoint so that it lies on the edge of the circle
                        var radians = Math.Atan2(position.Y - circleCenter.Y, position.X - circleCenter.X);
                        var totalRadius = (radius + circleStrokeThickness / 2.0f + penSize / 2.0f);
                        Point newPoint = new Point((Math.Cos(radians) * totalRadius) + circleCenter.X, (Math.Sin(radians) * totalRadius) + circleCenter.Y);

                        args.NewInkPoints[i] = new InkPoint(newPoint, 0.5f);
                    }
                    else
                    {
                        // InkPoint is not in the follow circle zone, add no more InkPoints to Stroke
                        discontinueStroke = true;

                        for (int j = args.NewInkPoints.Count - 1; j >= i; j--)
                        {
                            args.NewInkPoints.RemoveAt(j);
                        }
                        break;
                    }
                }
            }
            else
            {
                // Stroke has been discontinued, remove all InkPoints
                for (int j = args.NewInkPoints.Count - 1; j >= 0; j--)
                {
                    args.NewInkPoints.RemoveAt(j);
                }
            }
        }

        private void HandleRegularInput(CoreWetStrokeUpdateEventArgs args)
        {
            for (int i = 0; i < args.NewInkPoints.Count; i++)
            {
                var position = args.NewInkPoints[i].Position;
                var distanceFromCenter = DistanceFromCenter(position);

                if (distanceFromCenter < radius + circleStrokeThickness / 2 + penSize / 2)
                {
                    // Ink input entered the circle. Handle this by removing remaining InkPoints
                    // and completing the stroke.

                    for (int j = args.NewInkPoints.Count - 1; j >= i; j--)
                    {
                        args.NewInkPoints.RemoveAt(j);
                    }

                    args.Disposition = CoreWetStrokeDisposition.Completed;
                    break;
                }
            }
        }

        private double DistanceFromCenter(Point position)
        {
            return Math.Sqrt((position.X - circleCenter.X) * (position.X - circleCenter.X) +
                             (position.Y - circleCenter.Y) * (position.Y - circleCenter.Y));
        }

        private bool InCircleEdgeZone(double distanceFromCenter)
        {
            return (distanceFromCenter > radius * 0.8) && (distanceFromCenter < radius * 1.5);
        }
    }
}