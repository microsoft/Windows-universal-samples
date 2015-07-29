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

using System.ComponentModel;
using Windows.Foundation;
using Windows.Media.Ocr;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media;

namespace SDKTemplate
{
    /// <summary>
    /// Wraps recognized word and provides word overlay position and size based on current scale.
    /// </summary>
    class WordOverlay : INotifyPropertyChanged
    {
        private OcrWord word;

        /// <summary>
        /// Left and Right properties of Thickess define word box position.
        /// </summary>
        public Thickness WordPosition { get; private set; }

        /// <summary>
        /// Scaled word box width.
        /// </summary>
        public double WordWidth { get; private set; }

        /// <summary>
        /// Scaled word box height.
        /// </summary>
        public double WordHeight { get; private set; }

        public event PropertyChangedEventHandler PropertyChanged;

        public WordOverlay(OcrWord ocrWord)
        {
            word = ocrWord;

            UpdateProps(word.BoundingRect);
        }

        public void Transform(ScaleTransform scale)
        {
            // Scale word box bounding rect and update properties.
            UpdateProps(scale.TransformBounds(word.BoundingRect));
        }

        public Binding CreateWordPositionBinding()
        {
            Binding positionBinding = new Binding()
            {
                Path = new PropertyPath("WordPosition"),
                Source = this
            };

            return positionBinding;
        }

        public Binding CreateWordWidthBinding()
        {
            Binding widthBinding = new Binding()
            {
                Path = new PropertyPath("WordWidth"),
                Source = this
            };

            return widthBinding;
        }

        public Binding CreateWordHeightBinding()
        {
            Binding heightBinding = new Binding()
            {
                Path = new PropertyPath("WordHeight"),
                Source = this
            };

            return heightBinding;
        }

        private void UpdateProps(Rect wordBoundingBox)
        {
            WordPosition = new Thickness(wordBoundingBox.Left, wordBoundingBox.Top, 0, 0);
            WordWidth = wordBoundingBox.Width;
            WordHeight = wordBoundingBox.Height;

            OnPropertyChanged("WordPosition");
            OnPropertyChanged("WordWidth");
            OnPropertyChanged("WordHeight");
        }

        protected void OnPropertyChanged(string PropertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropertyName));
            }
        }
    }
}
