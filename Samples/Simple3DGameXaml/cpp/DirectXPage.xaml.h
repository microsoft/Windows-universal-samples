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

#include "DirectXPage.g.h"
#include "GameMain.h"
#include "ProductItem.h"

// When submitting the app to the Microsoft Store, disable the Store Simulator by commenting out this definition.
#define USE_STORE_SIMULATOR 1

namespace Simple3DGameXaml
{
    /// <summary>
    /// A page that hosts a DirectX SwapChainPanel.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class DirectXPage sealed : Simple3DGameXaml::IGameUIControl
    {
    public:
        DirectXPage();

        void OnSuspending();
        void OnResuming();

        // IGameUIControl methods.
        virtual void SetGameLoading();
        virtual void SetGameStats(int maxLevel, int hitCount, int shotCount);
        virtual void SetGameOver(bool win, int maxLevel, int hitCount, int shotCount, int highScore);
        virtual void SetLevelStart(int level, Platform::String^ objective, float timeLimit, float bonusTime);
        virtual void SetPause(int level, int hitCount, int shotCount, float timeRemaining);
        virtual void ShowTooSmall();
        virtual void HideTooSmall();
        virtual void SetAction(GameInfoOverlayCommand action);
        virtual void HideGameInfoOverlay();
        virtual void ShowGameInfoOverlay();

    private:
        // Window event handlers.
        void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
        void OnWindowActivationChanged(
            _In_ Platform::Object^ sender,
            _In_ Windows::UI::Core::WindowActivatedEventArgs^ args
            );

        // DisplayInformation event handlers.
        void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
        void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
        void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
        void OnStereoEnabledChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

        // Other event handlers.
        void OnCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel^ sender, Object^ args);
        void OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
        void OnWindowSizeChanged(
            _In_ Windows::UI::Core::CoreWindow^ sender,
            _In_ Windows::UI::Core::WindowSizeChangedEventArgs^ args
            );

        // License handlers.
        void InitializeLicense();
        void InitializeLicenseCore();
        void OnLicenseChanged();
        void SetProductItems(
            Windows::ApplicationModel::Store::ListingInformation^ listing,
            Windows::ApplicationModel::Store::LicenseInformation^ license
            );
#ifdef USE_STORE_SIMULATOR
        void ResetLicenseFromFile();
#endif

        // Page code behind.
        void OnPlayButtonClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnResetButtonClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnBuyAppButtonTapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ args);
        void OnBuySelectorClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnChangeBackgroundButtonClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnResetLicenseButtonClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnGameInfoOverlayTapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ args);
        void OnAppBarOpened(Object^ sender, Object^ args);
        void OnStoreReturnClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnLoadStoreClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);

        void OptionalTrialUpgrade();
        void ShowStoreFlyout();

    private:
        // Resources used to render the DirectX content in the XAML page background.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;
        std::unique_ptr<GameMain> m_main;
        bool m_windowVisible;
        bool m_playActive;

        Windows::ApplicationModel::Store::LicenseInformation^ m_licenseInformation;
        Windows::ApplicationModel::Store::ListingInformation^ m_listingInformation;
        bool m_possiblePurchaseUpgrade;
#ifdef USE_STORE_SIMULATOR
        PersistentState^                                    m_licenseState;
        bool                                                m_isTrial;
#endif
    };
}
