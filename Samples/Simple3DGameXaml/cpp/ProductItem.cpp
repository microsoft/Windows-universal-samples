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

#include "pch.h"
#include "ProductItem.h"

using namespace Simple3DGameXaml;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::ApplicationModel::Store;

ProductItem::ProductItem(
    ListingInformation^ listing,
    LicenseInformation^ license,
    Platform::String^ productId,
    bool isApp
    )
{
    m_isApp = isApp;
    m_isAppActive = license->IsActive;
    m_isAppTrial = license->IsTrial;
    m_productId = productId;

    if (m_isApp)
    {
        SetStatus(license);
        if (listing != nullptr)
        {
            m_title = listing->Name;
            m_formattedPrice = listing->FormattedPrice;
        }
        else
        {
            m_title = productId;
            m_formattedPrice = "";
        }
    }
    else
    {
        if (listing != nullptr)
        {
            ProductListing^ productListing = listing->ProductListings->Lookup(m_productId);
            m_title = productListing->Name;
            m_formattedPrice = productListing->FormattedPrice;
        }
        else
        {
            m_title = m_productId;
            m_formattedPrice = "";
        }
        SetStatus(license);
    }
}

void ProductItem::SetStatus(LicenseInformation^ license)
{
    if (m_isApp)
    {
        if (m_isAppActive)
        {
            if (m_isAppTrial)
            {
                m_isAvailableForPurchase = true;
                m_foreground = "#a0a0a0";
                m_status = "Trial";
            }
            else
            {
                m_isAvailableForPurchase = false;
                m_foreground = "#707070";
                m_status = "Purchased";
            }
        }
        else
        {
            m_isAvailableForPurchase = true;
            m_foreground = "#a0a0a0";
            m_status = "No License";
        }
    }
    else
    {
        if (m_isAppActive)
        {
            if (m_isAppTrial)
            {
                m_isProductActive = false;
                m_isAvailableForPurchase = false;
                m_foreground = "#707070";
                m_status = "unavailable";
            }
            else
            {
                ProductLicense^ productLicense = license->ProductLicenses->Lookup(m_productId);

                m_isProductActive = productLicense->IsActive;
                m_isAvailableForPurchase = !m_isProductActive;
                if (m_isProductActive)
                {
                    m_status = "Purchased";
                    m_foreground = "#707070";
                }
                else
                {
                    m_status = "No License";
                    m_foreground = "#a0a0a0";
                }
            }
        }
        else
        {
            m_isProductActive = false;
            m_isAvailableForPurchase = false;
            m_foreground = "#707070";
        }
    }
}

void ProductItem::UpdateContent(LicenseInformation^ license)
{
    if (m_isAppActive != license->IsActive ||
        m_isAppTrial != license->IsTrial)
    {
        m_isAppActive = license->IsActive;
        m_isAppTrial = license->IsTrial;
        SetStatus(license);
        PropertyChanged(this, ref new PropertyChangedEventArgs("Status"));
        PropertyChanged(this, ref new PropertyChangedEventArgs("BuyVisibility"));
        PropertyChanged(this, ref new PropertyChangedEventArgs("Foreground"));
    }
    else if (!m_isApp && m_isAppActive && !m_isAppTrial)
    {
        if (license->ProductLicenses->Lookup(m_productId)->IsActive != m_isProductActive)
        {
            SetStatus(license);
            PropertyChanged(this, ref new PropertyChangedEventArgs("Status"));
            PropertyChanged(this, ref new PropertyChangedEventArgs("BuyVisibility"));
            PropertyChanged(this, ref new PropertyChangedEventArgs("Foreground"));
        }
    }
}

void ProductItem::UpdateContent(ListingInformation^ listing)
{
    if (m_isApp)
    {
        if (m_title != listing->Name)
        {
            m_title = listing->Name;
            PropertyChanged(this, ref new PropertyChangedEventArgs("Title"));
        }
        if (m_formattedPrice != listing->FormattedPrice)
        {
            m_formattedPrice = listing->FormattedPrice;
            PropertyChanged(this, ref new PropertyChangedEventArgs("Price"));
        }
    }
    else
    {
        ProductListing^ productListing = listing->ProductListings->Lookup(m_productId);
        if (m_title != productListing->Name)
        {
            m_title = productListing->Name;
            PropertyChanged(this, ref new PropertyChangedEventArgs("Title"));
        }
        if (m_formattedPrice != productListing->FormattedPrice)
        {
            m_formattedPrice = productListing->FormattedPrice;
            PropertyChanged(this, ref new PropertyChangedEventArgs("Price"));
        }
    }
}
