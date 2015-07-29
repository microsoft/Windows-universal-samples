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

namespace Simple3DGameXaml
{
    [Windows::UI::Xaml::Data::Bindable]
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ProductItem sealed :
        public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    internal:
        ProductItem(
            Windows::ApplicationModel::Store::ListingInformation^ listing,
            Windows::ApplicationModel::Store::LicenseInformation^ license,
            Platform::String^ productId,
            bool isApp
            );

    public:
        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

        property Platform::String^ Title
        {
            Platform::String^ get()
            {
                return m_title;
            }
        }

        property Platform::String^ Price
        {
            Platform::String^ get()
            {
                return m_formattedPrice;
            }
        }

        property Platform::String^ Status
        {
            Platform::String^ get()
            {
                return m_status;
            }
        }

        property int BuyVisibility
        {
            int get()
            {
                return static_cast<int>(m_isAvailableForPurchase ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed);
            }
        }

        property Platform::String^ ProductId
        {
            Platform::String^ get()
            {
                return m_productId;
            }
        }

        property Platform::String^ Foreground
        {
            Platform::String^ get()
            {
                return m_foreground;
            }
        }

        [Windows::Foundation::Metadata::DefaultOverload]
        void UpdateContent(Windows::ApplicationModel::Store::LicenseInformation^ license);
        void UpdateContent(Windows::ApplicationModel::Store::ListingInformation^ listing);

    private:
        void SetStatus(Windows::ApplicationModel::Store::LicenseInformation^ license);

        Platform::String^ m_title;
        Platform::String^ m_formattedPrice;
        Platform::String^ m_productId;
        Platform::String^ m_status;
        Platform::String^ m_foreground;
        bool              m_isApp;
        bool              m_isAppActive;
        bool              m_isAppTrial;
        bool              m_isProductActive;
        bool              m_isAvailableForPurchase;
    };
}
