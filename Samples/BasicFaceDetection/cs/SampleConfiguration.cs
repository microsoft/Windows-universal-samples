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
using Windows.Graphics.Imaging;
using Windows.Media.FaceAnalysis;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Face detection C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Detect Faces in Photos", ClassType=typeof(Scenario1_DetectInPhoto)},
            new Scenario() { Title="Detect Faces in Webcam", ClassType=typeof(Scenario2_DetectInWebcam)},
        };

        /// <summary>
        /// Rescale the size and position of the face highlight box
        /// to account for the difference between the size of the image and
        /// the canvas.
        /// </summary>
        /// <param name="box">The element to rescale</param>
        /// <param name="widthScale">Horizontal adjustment factor</param>
        /// <param name="heightScale">Vertical adjustment factor</param>
        static void ApplyScale(FrameworkElement box, double widthScale, double heightScale)
        {
            // We saved the original size of the face box in the element's Tag field.
            BitmapBounds faceBox = (BitmapBounds)box.Tag;
            box.Width = faceBox.Width * widthScale;
            box.Height = faceBox.Height * heightScale;
            box.Margin = new Thickness(faceBox.X * widthScale, faceBox.Y * heightScale, 0, 0);
        }

        public static void HighlightFaces(WriteableBitmap displaySource, IList<DetectedFace> foundFaces, Canvas canvas, DataTemplate template)
        {
            double widthScale = canvas.ActualWidth / displaySource.PixelWidth;
            double heightScale = canvas.ActualHeight / displaySource.PixelHeight;

            foreach (DetectedFace face in foundFaces)
            {
                // Create an  element for displaying the face box but since we're using a Canvas
                // we must scale it according to the image's actual size.
                // The original FaceBox values are saved in the element's Tag field so we can update the
                // boxes when the Canvas is resized.
                var box = (FrameworkElement)template.LoadContent();
                box.Tag = face.FaceBox;
                ApplyScale(box, widthScale, heightScale);

                canvas.Children.Add(box);
            }

            string message;
            if (foundFaces.Count == 0)
            {
                message = "Didn't find any human faces in the image";
            }
            else if (foundFaces.Count == 1)
            {
                message = "Found a human face in the image";
            }
            else
            {
                message = "Found " + foundFaces.Count + " human faces in the image";
            }

            MainPage.Current.NotifyUser(message, NotifyType.StatusMessage);
        }

        public static void RepositionFaces(WriteableBitmap displaySource, Canvas canvas)
        {
            double widthScale = canvas.ActualWidth / displaySource.PixelWidth;
            double heightScale = canvas.ActualHeight / displaySource.PixelHeight;

            foreach (var item in canvas.Children)
            {
                ApplyScale((FrameworkElement)item, widthScale, heightScale);
            }
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
