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
#include "App.xaml.h"
#include "DirectXPage.xaml.h"
#include "ProductItem.h"

using namespace Simple3DGameXaml;

using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::UI::Popups;
using namespace concurrency;

//----------------------------------------------------------------------

DirectXPage::DirectXPage() :
    m_playActive(false),
    m_possiblePurchaseUpgrade(false)
{
    InitializeComponent();

#ifdef USE_STORE_SIMULATOR
    ResetLicense->Visibility = ::Visibility::Visible;
#endif

    // Register event handlers for page lifecycle.
    CoreWindow^ window = Window::Current->CoreWindow;

    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &DirectXPage::OnVisibilityChanged);
    window->SizeChanged +=
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &DirectXPage::OnWindowSizeChanged);

    Window::Current->Activated += 
        ref new WindowActivatedEventHandler(this, &DirectXPage::OnWindowActivationChanged);

    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDpiChanged);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnOrientationChanged);

    currentDisplayInformation->StereoEnabledChanged +=
        ref new TypedEventHandler<DisplayInformation^, Platform::Object^>(this, &DirectXPage::OnStereoEnabledChanged);

    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDisplayContentsInvalidated);

    DXSwapChainPanel->CompositionScaleChanged +=
        ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &DirectXPage::OnCompositionScaleChanged);

    DXSwapChainPanel->SizeChanged +=
        ref new SizeChangedEventHandler(this, &DirectXPage::OnSwapChainPanelSizeChanged);

    // Disable all pointer visual feedback for better performance when touching.
    auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
    pointerVisualizationSettings->IsContactFeedbackEnabled = false;
    pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;

    // At this point we have access to the device.
    // We can create the device-dependent resources.
    m_deviceResources = std::make_shared<DX::DeviceResources>();
    m_deviceResources->SetSwapChainPanel(DXSwapChainPanel);

    m_main = std::unique_ptr<GameMain>(new GameMain(m_deviceResources, this));

    InitializeLicense();

    m_main->StartRenderLoop();
}

//----------------------------------------------------------------------

void DirectXPage::HideGameInfoOverlay()
{
    // This function may be called from a different thread.
    // All XAML updates need to occur on the UI thread so dispatch to ensure this is true.
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this]()
        {
            VisualStateManager::GoToState(this, "NormalState", true);

            StoreFlyout->IsOpen = false;
            StoreFlyout->Visibility = ::Visibility::Collapsed;
            GameAppBar->IsOpen = false;
            Play->Label = "Pause";
            Play->Icon = ref new SymbolIcon(Symbol::Pause);
            m_playActive = true;
        })
        );
}

//----------------------------------------------------------------------

void DirectXPage::ShowGameInfoOverlay()
{
    // This function may be called from a different thread.
    // All XAML updates need to occur on the UI thread so dispatch to ensure this is true.
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this]()
        {
            VisualStateManager::GoToState(this, "GameInfoOverlayState", true);
            Play->Label = "Play";
            Play->Icon = ref new SymbolIcon(Symbol::Play);
            m_playActive = false;
        })
        );
}

//----------------------------------------------------------------------

void DirectXPage::SetAction(GameInfoOverlayCommand action)
{
    // This function may be called from a different thread.
    // All XAML updates need to occur on the UI thread so dispatch to ensure this is true.
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, action]()
        {
            // Enable only one of the four possible commands at the bottom of the
            // Game Info Overlay.

            PlayAgain->Visibility = ::Visibility::Collapsed;
            PleaseWait->Visibility = ::Visibility::Collapsed;
            TapToContinue->Visibility = ::Visibility::Collapsed;

            switch (action)
            {
            case GameInfoOverlayCommand::PlayAgain:
                PlayAgain->Visibility = ::Visibility::Visible;
                break;
            case GameInfoOverlayCommand::PleaseWait:
                PleaseWait->Visibility = ::Visibility::Visible;
                break;
            case GameInfoOverlayCommand::TapToContinue:
                TapToContinue->Visibility = ::Visibility::Visible;
                break;
            case GameInfoOverlayCommand::None:
                break;
            }
        })
        );
}

//----------------------------------------------------------------------

void DirectXPage::SetGameLoading()
{
    // This function may be called from a different thread.
    // All XAML updates need to occur on the UI thread so dispatch to ensure this is true.
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this]()
        {
            GameInfoOverlayTitle->Text = "Loading Resources";

            Loading->Visibility = ::Visibility::Visible;
            Stats->Visibility = ::Visibility::Collapsed;
            LevelStart->Visibility = ::Visibility::Collapsed;
            PauseData->Visibility = ::Visibility::Collapsed;
            LoadingProgress->IsActive = true;
        })
        );
}

//----------------------------------------------------------------------

void DirectXPage::SetGameStats(
    int maxLevel,
    int hitCount,
    int shotCount
    )
{
    // This function may be called from a different thread.
    // All XAML updates need to occur on the UI thread so dispatch to ensure this is true.
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, maxLevel, hitCount, shotCount]()
        {
            GameInfoOverlayTitle->Text = "Game Statistics";
            m_possiblePurchaseUpgrade = true;
            OptionalTrialUpgrade();

            Loading->Visibility = ::Visibility::Collapsed;
            Stats->Visibility = ::Visibility::Visible;
            LevelStart->Visibility = ::Visibility::Collapsed;
            PauseData->Visibility = ::Visibility::Collapsed;

            static const int bufferLength = 20;
            static char16 wsbuffer[bufferLength];

            int length = swprintf_s(wsbuffer, bufferLength, L"%d", maxLevel);
            LevelsCompleted->Text = ref new Platform::String(wsbuffer, length);

            length = swprintf_s(wsbuffer, bufferLength, L"%d", hitCount);
            TotalPoints->Text = ref new Platform::String(wsbuffer, length);

            length = swprintf_s(wsbuffer, bufferLength, L"%d", shotCount);
            TotalShots->Text = ref new Platform::String(wsbuffer, length);

            // High Score is not used for showing Game Statistics
            HighScoreTitle->Visibility = ::Visibility::Collapsed;
            HighScoreData->Visibility = ::Visibility::Collapsed;
        })
        );
}

//----------------------------------------------------------------------

void DirectXPage::SetGameOver(
    bool win,
    int maxLevel,
    int hitCount,
    int shotCount,
    int highScore
    )
{
    // This function may be called from a different thread.
    // All XAML updates need to occur on the UI thread so dispatch to ensure this is true.
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, win, maxLevel, hitCount, shotCount, highScore]()
        {
            if (win)
            {
                GameInfoOverlayTitle->Text = "You Won!";
                m_possiblePurchaseUpgrade = true;
                OptionalTrialUpgrade();
            }
            else
            {
                GameInfoOverlayTitle->Text = "Game Over";
                m_possiblePurchaseUpgrade = false;
                PurchaseUpgrade->Visibility = ::Visibility::Collapsed;
            }
            Loading->Visibility = ::Visibility::Collapsed;
            Stats->Visibility = ::Visibility::Visible;
            LevelStart->Visibility = ::Visibility::Collapsed;
            PauseData->Visibility = ::Visibility::Collapsed;

            static const int bufferLength = 20;
            static char16 wsbuffer[bufferLength];

            int length = swprintf_s(wsbuffer, bufferLength, L"%d", maxLevel);
            LevelsCompleted->Text = ref new Platform::String(wsbuffer, length);

            length = swprintf_s(wsbuffer, bufferLength, L"%d", hitCount);
            TotalPoints->Text = ref new Platform::String(wsbuffer, length);

            length = swprintf_s(wsbuffer, bufferLength, L"%d", shotCount);
            TotalShots->Text = ref new Platform::String(wsbuffer, length);

            // Show High Score
            HighScoreTitle->Visibility = ::Visibility::Visible;
            HighScoreData->Visibility = ::Visibility::Visible;
            length = swprintf_s(wsbuffer, bufferLength, L"%d", highScore);
            HighScore->Text = ref new Platform::String(wsbuffer, length);
        })
        );
}

//----------------------------------------------------------------------

void DirectXPage::SetLevelStart(
    int level,
    Platform::String^ objective,
    float timeLimit,
    float bonusTime
    )
{
    // This function may be called from a different thread.
    // All XAML updates need to occur on the UI thread so dispatch to ensure this is true.
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, level, objective, timeLimit, bonusTime]()
        {
            static const int bufferLength = 20;
            static char16 wsbuffer[bufferLength];

            int length = swprintf_s(wsbuffer, bufferLength, L"Level %d", level);
            GameInfoOverlayTitle->Text = ref new Platform::String(wsbuffer, length);

            Loading->Visibility = ::Visibility::Collapsed;
            Stats->Visibility = ::Visibility::Collapsed;
            LevelStart->Visibility = ::Visibility::Visible;
            PauseData->Visibility = ::Visibility::Collapsed;

            Objective->Text = objective;

            length = swprintf_s(wsbuffer, bufferLength, L"%6.1f sec", timeLimit);
            TimeLimit->Text = ref new Platform::String(wsbuffer, length);

            if (bonusTime > 0.0)
            {
                BonusTimeTitle->Visibility = ::Visibility::Visible;
                BonusTimeData->Visibility = ::Visibility::Visible;
                length = swprintf_s(wsbuffer, bufferLength, L"%6.1f sec", bonusTime);
                BonusTime->Text = ref new Platform::String(wsbuffer, length);
            }
            else
            {
                BonusTimeTitle->Visibility = ::Visibility::Collapsed;
                BonusTimeData->Visibility = ::Visibility::Collapsed;
            }
        })
        );
}

//----------------------------------------------------------------------

void DirectXPage::SetPause(int level, int hitCount, int shotCount, float timeRemaining)
{
    // This function may be called from a different thread.
    // All XAML updates need to occur on the UI thread so dispatch to ensure this is true.
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, level, hitCount, shotCount, timeRemaining]()
        {
            GameInfoOverlayTitle->Text = "Paused";
            Loading->Visibility = ::Visibility::Collapsed;
            Stats->Visibility = ::Visibility::Collapsed;
            LevelStart->Visibility = ::Visibility::Collapsed;
            PauseData->Visibility = ::Visibility::Visible;

            static const int bufferLength = 20;
            static char16 wsbuffer[bufferLength];

            int length = swprintf_s(wsbuffer, bufferLength, L"%d", level);
            PauseLevel->Text = ref new Platform::String(wsbuffer, length);

            length = swprintf_s(wsbuffer, bufferLength, L"%d", hitCount);
            PauseHits->Text = ref new Platform::String(wsbuffer, length);

            length = swprintf_s(wsbuffer, bufferLength, L"%d", shotCount);
            PauseShots->Text = ref new Platform::String(wsbuffer, length);

            length = swprintf_s(wsbuffer, bufferLength, L"%6.1f sec", timeRemaining);
            PauseTimeRemaining->Text = ref new Platform::String(wsbuffer, length);
        })
        );
}

//----------------------------------------------------------------------

void DirectXPage::ShowTooSmall()
{
    // This function may be called from a different thread.
    // All XAML updates need to occur on the UI thread so dispatch to ensure this is true.
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this]()
        {
            VisualStateManager::GoToState(this, "TooSmallState", true);
        })
        );
}

//----------------------------------------------------------------------

void DirectXPage::HideTooSmall()
{
    // This function may be called from a different thread.
    // All XAML updates need to occur on the UI thread so dispatch to ensure this is true.
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this]()
        {
            VisualStateManager::GoToState(this, "NotTooSmallState", true);
        })
        );
}

// Window event handlers.
//----------------------------------------------------------------------

void DirectXPage::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
    m_windowVisible = args->Visible;
    if (m_windowVisible)
    {
        m_main->StartRenderLoop();
    }
    else
    {
        m_main->StopRenderLoop();
    }
}

//----------------------------------------------------------------------

void DirectXPage::OnWindowActivationChanged(
    _In_ Platform::Object^ /* sender */,
    _In_ Windows::UI::Core::WindowActivatedEventArgs^ args
    )
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_main->WindowActivationChanged(args->WindowActivationState);
}

// DisplayInformation event handlers.
//----------------------------------------------------------------------

void DirectXPage::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_deviceResources->SetDpi(sender->LogicalDpi);
    m_main->CreateWindowSizeDependentResources();
}

//----------------------------------------------------------------------

void DirectXPage::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
    m_main->CreateWindowSizeDependentResources();
}

//----------------------------------------------------------------------

void DirectXPage::OnStereoEnabledChanged(DisplayInformation^ /* sender */, Object^ /* args */)
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_deviceResources->UpdateStereoState();
    m_main->CreateWindowSizeDependentResources();
}

//----------------------------------------------------------------------

void DirectXPage::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_deviceResources->ValidateDevice();
}

//----------------------------------------------------------------------

void DirectXPage::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ args)
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_deviceResources->SetCompositionScale(sender->CompositionScaleX, sender->CompositionScaleY);
    m_main->CreateWindowSizeDependentResources();
}

//----------------------------------------------------------------------

void DirectXPage::OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_deviceResources->SetLogicalSize(e->NewSize);
    m_main->CreateWindowSizeDependentResources();
}

//----------------------------------------------------------------------

void DirectXPage::OnSuspending()
{
    critical_section::scoped_lock lock(m_main->GetCriticalSection());
    m_main->Suspend();
    // Stop rendering when the app is suspended.
    m_main->StopRenderLoop();

    m_deviceResources->Trim();

}

//----------------------------------------------------------------------

void DirectXPage::OnResuming()
{
    m_main->Resume();
    m_main->StartRenderLoop();
}

//----------------------------------------------------------------------

void DirectXPage::OnGameInfoOverlayTapped(Object^ /* sender */, TappedRoutedEventArgs^ /* args */)
{
    m_main->PressComplete();
}

//----------------------------------------------------------------------

void DirectXPage::OnPlayButtonClicked(Object^ /* sender */, RoutedEventArgs^ /* args */)
{
    if (m_playActive)
    {
        m_main->PauseRequested();
    }
    else
    {
        m_main->PressComplete();
    }
}

//----------------------------------------------------------------------

void DirectXPage::OnResetButtonClicked(Object^ /* sender */, RoutedEventArgs^ /* args */)
{
    m_main->ResetGame();
    GameAppBar->IsOpen = false;
}

//----------------------------------------------------------------------
void DirectXPage::InitializeLicense()
{
#ifdef USE_STORE_SIMULATOR
    m_licenseState = ref new PersistentState();
    m_licenseState->Initialize(ApplicationData::Current->LocalSettings->Values, "CurrentAppSimulator");
    m_isTrial = m_licenseState->LoadBool(":isTrial", true);

    Platform::String^ license;
    if (this->m_isTrial)
    {
        license = "TrialLicense.xml";
    }
    else
    {
        license = "FullLicense.xml";
    }
    task<StorageFile^> fileTask(Package::Current->InstalledLocation->GetFileAsync(license));
    fileTask.then([=](StorageFile^ sourceFile)
    {
        return create_task(CurrentAppSimulator::ReloadSimulatorAsync(sourceFile));
    }).then([this]()
    {
        this->InitializeLicenseCore();
    });
#else
    this->InitializeLicenseCore();
#endif
}

//--------------------------------------------------------------------------------------

void DirectXPage::InitializeLicenseCore()
{
#ifdef USE_STORE_SIMULATOR
    this->m_licenseInformation = CurrentAppSimulator::LicenseInformation;
    task<ListingInformation^> listingTask(CurrentAppSimulator::LoadListingInformationAsync());
#else
    m_licenseInformation = CurrentApp::LicenseInformation;
    task<ListingInformation^> listingTask(CurrentApp::LoadListingInformationAsync());
#endif

    this->m_licenseInformation->LicenseChanged += ref new LicenseChangedEventHandler(this, &DirectXPage::OnLicenseChanged);
    this->SetProductItems(nullptr, m_licenseInformation);
    this->OnLicenseChanged();

    listingTask.then([=](ListingInformation^ listing)
    {
        this->m_listingInformation = listing;
        this->OnLicenseChanged();
    });
}

//--------------------------------------------------------------------------------------

#ifdef USE_STORE_SIMULATOR
void DirectXPage::ResetLicenseFromFile()
{
    task<StorageFile^> fileTask(Package::Current->InstalledLocation->GetFileAsync("TrialLicense.xml"));
    fileTask.then([=](StorageFile^ sourceFile)
    {
        CurrentAppSimulator::ReloadSimulatorAsync(sourceFile);
    });
}
#endif

//--------------------------------------------------------------------------------------

void DirectXPage::OnLicenseChanged()
{
#ifdef USE_STORE_SIMULATOR
    m_isTrial = (m_licenseInformation->IsActive && m_licenseInformation->IsTrial);
    m_licenseState->SaveBool(":isTrial", m_isTrial);
#endif

    m_main->LicenseChanged(m_listingInformation, m_licenseInformation);

    // This function may be called from a different thread.
    // All XAML updates need to occur on the UI thread so dispatch to ensure this is true.
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this]()
        {
            if (m_licenseInformation->IsActive)
            {
                if (!m_licenseInformation->IsTrial)
                {
                    PurchaseUpgrade->Visibility = ::Visibility::Collapsed;
                }
            }
            else
            {
                ChangeBackground->Visibility = ::Visibility::Collapsed;
            }

            if (m_licenseInformation->IsActive && m_licenseInformation->IsTrial)
            {
                if (m_listingInformation != nullptr)
                {
                    PurchaseMessage->Text =
                        "You are running a trial version. Purchase the full version for: " + m_listingInformation->FormattedPrice;
                }
                else
                {
                    PurchaseMessage->Text =
                        "You are running a trial version. Purchase the full version.";
                }
                if (m_possiblePurchaseUpgrade)
                {
                    PurchaseUpgrade->Visibility = ::Visibility::Visible;
                }
            }

            if (m_licenseInformation != nullptr)
            {
                auto items = dynamic_cast<Platform::Collections::Vector<Platform::Object^>^>(ProductListView->ItemsSource);
                for (uint32 i = 0; i < items->Size; i++)
                {
                    dynamic_cast<ProductItem^>(items->GetAt(i))->UpdateContent(m_licenseInformation);
                }
            }
            if (m_listingInformation != nullptr)
            {
                auto items = dynamic_cast<Platform::Collections::Vector<Platform::Object^>^>(ProductListView->ItemsSource);
                for (uint32 i = 0; i < items->Size; i++)
                {
                    dynamic_cast<ProductItem^>(items->GetAt(i))->UpdateContent(m_listingInformation);
                }
            }
        })
        );
}

//----------------------------------------------------------------------

void DirectXPage::OnBuyAppButtonTapped(Object^ sender, TappedRoutedEventArgs^ args)
{
    args->Handled = true;
    OnBuySelectorClicked(sender, args);
}

//----------------------------------------------------------------------

void DirectXPage::OnBuySelectorClicked(Object^ sender, RoutedEventArgs^ /* args */)
{
    Platform::String^ tag = dynamic_cast<Platform::String^>(dynamic_cast<Button^>(sender)->CommandParameter);
    StoreUnavailable->Visibility = ::Visibility::Collapsed;

    if (tag == "MainApp")
    {
        if ((m_licenseInformation != nullptr) && m_licenseInformation->IsActive)
        {
            if (m_licenseInformation->IsTrial)
            {
#ifdef USE_STORE_SIMULATOR
                task<Platform::String^> purchaseTask(CurrentAppSimulator::RequestAppPurchaseAsync(false));
#else
                task<Platform::String^> purchaseTask(CurrentApp::RequestAppPurchaseAsync(false));
#endif
                purchaseTask.then([this](task<Platform::String^> previousTask)
                {
                    try
                    {
                        // Try getting all exceptions from the continuation chain above this point.
                        previousTask.get();
                        if (m_licenseInformation->IsActive && !m_licenseInformation->IsTrial)
                        {
                            auto msgDlg = ref new MessageDialog("You successfully upgraded your app to the fully-licensed version.", "Information");
                            msgDlg->ShowAsync();
                        }
                        else
                        {
                            auto msgDlg = ref new MessageDialog("You still have a trial license for this app.", "Information");
                            msgDlg->ShowAsync();
                        }
                    }
                    catch (Platform::Exception^ exception)
                    {
                        if (exception->HResult == E_FAIL)
                        {
                            StoreUnavailable->Visibility = ::Visibility::Visible;
                        }
                    }
                });
            }
        }
    }
    else
    {
        if ((m_licenseInformation != nullptr) && m_licenseInformation->IsActive && !m_licenseInformation->IsTrial)
        {
            if (!m_licenseInformation->ProductLicenses->Lookup(tag)->IsActive)
            {
#ifdef USE_STORE_SIMULATOR
                task<PurchaseResults^> purchaseTask(CurrentAppSimulator::RequestProductPurchaseAsync(tag));
#else
                task<PurchaseResults^> purchaseTask(CurrentApp::RequestProductPurchaseAsync(tag));
#endif
                purchaseTask.then([=](task<PurchaseResults^> previousTask)
                {
                    try
                    {
                        // Try getting all exceptions from the continuation chain above this point
                        previousTask.get();

                        if (m_licenseInformation->IsActive && !m_licenseInformation->IsTrial && m_licenseInformation->ProductLicenses->Lookup(tag)->IsActive)
                        {
                            auto msgDlg = ref new MessageDialog(
                                Platform::String::Concat("You successfully upgraded your app with ",
                                m_licenseInformation->ProductLicenses->Lookup(tag)->ProductId),
                                "Information");
                            msgDlg->ShowAsync();
                        }
                        else
                        {
                            auto msgDlg = ref new MessageDialog("You did not upgrade your app.", "Information");
                            msgDlg->ShowAsync();
                        }
                    }
                    catch (Platform::Exception^ exception)
                    {
                        if (exception->HResult == E_FAIL)
                        {
                            StoreUnavailable->Visibility = ::Visibility::Visible;
                        }
                    }
                });
            }
        }
    }
}

//----------------------------------------------------------------------

void DirectXPage::OnChangeBackgroundButtonClicked(Object^ /* sender */, RoutedEventArgs^ /* args */)
{

    if ((m_licenseInformation != nullptr) && m_licenseInformation->IsActive)
    {
        if (m_licenseInformation->IsTrial ||
            (!m_licenseInformation->ProductLicenses->Lookup("NightBackground")->IsActive  &&
            !m_licenseInformation->ProductLicenses->Lookup("DayBackground")->IsActive))
        {
            if (m_playActive)
            {
                m_main->PauseRequested();
            }
            ShowStoreFlyout();
        }
        else
        {
            m_main->CycleBackground();
        }
    }
}

//----------------------------------------------------------------------

void DirectXPage::OnResetLicenseButtonClicked(Object^ /* sender */, RoutedEventArgs^ /* args */)
{
#ifdef USE_STORE_SIMULATOR
    ResetLicenseFromFile();
#endif
    m_main->SetBackground(0);
}

//----------------------------------------------------------------------

void DirectXPage::OptionalTrialUpgrade()
{
    PurchaseUpgrade->Visibility = ::Visibility::Collapsed;

    if (m_licenseInformation != nullptr)
    {
        if (m_licenseInformation->IsActive && m_licenseInformation->IsTrial)
        {
            if (m_listingInformation != nullptr)
            {
                PurchaseMessage->Text =
                    "You are running a trial version. Purchase the full version for: " + m_listingInformation->FormattedPrice;
            }
            else
            {
                PurchaseMessage->Text =
                    "You are running a trial version. Purchase the full version.";
            }
            PurchaseUpgrade->Visibility = ::Visibility::Visible;
        }
    }
}

//----------------------------------------------------------------------

void DirectXPage::OnStoreReturnClicked(Object^ /* sender */, RoutedEventArgs^ /* args */)
{
    StoreFlyout->IsOpen = false;
    StoreFlyout->Visibility = ::Visibility::Collapsed;
}

//----------------------------------------------------------------------

void DirectXPage::OnLoadStoreClicked(Object^ /* sender */, RoutedEventArgs^ /* args */)
{
    m_main->PauseRequested();
    ShowStoreFlyout();
}

//----------------------------------------------------------------------

void DirectXPage::SetProductItems(
    ListingInformation^ listing,
    LicenseInformation^ license
    )
{
    auto items = ref new Platform::Collections::Vector<Platform::Object^>();
    items->Append(ref new ProductItem(listing, license, "MainApp", true));
    items->Append(ref new ProductItem(listing, license, "AutoFire", false));
    items->Append(ref new ProductItem(listing, license, "NightBackground", false));
    items->Append(ref new ProductItem(listing, license, "DayBackground", false));
    ProductListView->ItemsSource = items;
    StoreUnavailable->Visibility = ::Visibility::Collapsed;
}

//----------------------------------------------------------------------
void DirectXPage::OnWindowSizeChanged(
    _In_ CoreWindow^ /* window */,
    _In_ WindowSizeChangedEventArgs^ /* args */
    )
{
    StoreGrid->Height = Window::Current->Bounds.Height;
    StoreFlyout->HorizontalOffset = Window::Current->Bounds.Width - StoreGrid->Width;
}

//----------------------------------------------------------------------

void DirectXPage::OnAppBarOpened(Object^ /* sender */, Object^ /* args */)
{
    m_main->PauseRequested();
}

//----------------------------------------------------------------------

void DirectXPage::ShowStoreFlyout()
{
    StoreGrid->Height = Window::Current->Bounds.Height;
    StoreUnavailable->Visibility = ::Visibility::Collapsed;
    StoreFlyout->HorizontalOffset = Window::Current->Bounds.Width - StoreGrid->Width;
    StoreFlyout->IsOpen = true;
    StoreFlyout->Visibility = ::Visibility::Visible;
    GameAppBar->IsOpen = false;
}

//----------------------------------------------------------------------


