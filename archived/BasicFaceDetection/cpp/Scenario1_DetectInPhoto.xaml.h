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

#pragma once

#include "Scenario1_DetectInPhoto.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// Page for demonstrating FaceDetection on an image file.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class DetectFacesInPhoto sealed
    {
    public:

        /// <summary>
        /// Initializes a new instance of the <see cref="DetectFacesInPhoto" /> class.
        /// </summary>
        DetectFacesInPhoto();

    protected:

        /// <summary>
        /// Responds when we navigate to this page.
        /// </summary>
        /// <param name="e">Event data</param>
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        /// <summary>
        /// Brush for drawing the bounding box around each detected face.
        /// </summary>
        Windows::UI::Xaml::Media::SolidColorBrush^ lineBrush = ref new  Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Yellow);

        /// <summary>
        /// Thickness of the face bounding box lines.
        /// </summary>
        const double lineThickness = 2.0;

        /// <summary>
        /// Transparent fill for the bounding box.
        /// </summary>
        Windows::UI::Xaml::Media::SolidColorBrush^ fillBrush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Transparent);

        /// <summary>
        /// Limit on the height of the source image (in pixels) passed into FaceDetector for performance considerations.
        /// Images larger that this size will be downscaled proportionally.
        /// </summary>
        /// <remarks>
        /// This is an arbitrary value that was chosen for this scenario, in which FaceDetector performance isn't too important but face
        /// detection accuracy is; a generous size is used.
        /// Your application may have different performance and accuracy needs and you'll need to decide how best to control input.
        /// </remarks>
        const unsigned int sourceImageHeightLimit = 1280;

        /// <summary>
        /// Reference back to the "root" page of the app.
        /// </summary>
        SDKTemplate::MainPage^ rootPage;

        /// <summary>
        /// Takes the photo image and FaceDetector results and assembles the visualization onto the Canvas.
        /// </summary>
        /// <param name="displaySource">Bitmap object holding the image we're going to display</param>
        /// <param name="foundFaces">List of detected faces; output from FaceDetector</param>
        void SetupVisualization(
            Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ displaySource,
            Windows::Foundation::Collections::IVector<Windows::Media::FaceAnalysis::DetectedFace^>^ foundFaces);

        /// <summary>
        /// Clears the display of image and face boxes.
        /// </summary>
        void ClearVisualization();

        /// <summary>
        /// Computes a BitmapTransform to downscale the source image if it's too large. 
        /// </summary>
        /// <remarks>
        /// Performance of the FaceDetector degrades significantly with large images, and in most cases it's best to downscale
        /// the source bitmaps if they're too large before passing them into FaceDetector. Remember through, your application's performance needs will vary.
        /// </remarks>
        /// <param name="sourceDecoder">Source image decoder</param>
        /// <returns>A BitmapTransform object holding scaling values if source image is too large</returns>
        Windows::Graphics::Imaging::BitmapTransform^ ComputeScalingTransformForSourceImage(Windows::Graphics::Imaging::BitmapDecoder^ sourceDecoder);

        /// <summary>
        /// Opens the passed in image file and runs the FaceDetector on the loaded bitmap. If successful calls SetupVisualization to display the results.
        /// </summary>
        /// <param name="photoFile">Image file to open and perform FaceDetection on</param>
        void ProcessFaceDetectionOnImage(Windows::Storage::StorageFile^ photoFile);

        /// <summary>
        /// Loads an image file (selected by the user) and runs the FaceDetector on the loaded bitmap. If successful calls SetupVisualization to display the results.
        /// </summary>
        /// <param name="sender">Button user clicked</param>
        /// <param name="e">Event data</param>
        void OpenFile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /// <summary>
        /// Updates any existing face bounding boxes in response to changes in the size of the Canvas.
        /// </summary>
        /// <param name="sender">Canvas object whose size has changed</param>
        /// <param name="e">Event data</param>
        void PhotoCanvas_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
    };
}
