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

using namespace Windows::Foundation;
using namespace Windows::Media::Ocr;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Media;

namespace SDKTemplate
{
    /// <summary>
    /// Wraps recognized word and provides word overlay position and size based on current scale. 
    /// </summary>
    [Windows::UI::Xaml::Data::Bindable]
    public ref class WordOverlay sealed : INotifyPropertyChanged
    {
    public:
        WordOverlay(OcrWord^ ocrWord);

        /// <summary>
        /// Left and Right properties of Thickess define word box position.
        /// </summary>
        property Thickness WordPosition
        {
            Thickness get()
            {
                return wordPosition;
            }
        }

        /// <summary>
        /// Scaled word box width.
        /// </summary>
        property double WordWidth
        {
            double get()
            {
                return wordWidth;
            }
        }

        /// <summary>
        /// Scaled word box height.
        /// </summary>
        property double WordHeight
        {
            double get()
            {
                return wordHeight;
            }
        }

        virtual event PropertyChangedEventHandler^ PropertyChanged
        {
            virtual EventRegistrationToken add(PropertyChangedEventHandler^ e)
            {
                isPropertyChangedObserved = true;
                return privatePropertyChanged += e;
            }

            virtual void remove(EventRegistrationToken t)
            {
                privatePropertyChanged -= t;
            }

        protected:
            virtual void raise(Object^ sender, PropertyChangedEventArgs^ e)
            {
                if (isPropertyChangedObserved)
                {
                    privatePropertyChanged(sender, e);
                }
            }
        }

        void Transform(ScaleTransform^ transform);
        Binding^ CreateWordPositionBinding();
        Binding^ CreateWordWidthBinding();
        Binding^ CreateWordHeightBinding();

    protected:
        void OnPropertyChanged(Platform::String^ propertyName);

    private:
        OcrWord^ word;

        Thickness wordPosition;
        double wordWidth;
        double wordHeight;

        bool isPropertyChangedObserved;
        event PropertyChangedEventHandler^ privatePropertyChanged;

        void UpdateProps(Rect boundingBox);
    };
}