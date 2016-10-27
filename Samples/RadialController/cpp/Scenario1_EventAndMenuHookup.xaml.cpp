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
#include "Scenario1_EventAndMenuHookup.xaml.h"
#include <string>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Input;
using namespace Windows::Storage::Streams;
using namespace std::placeholders;
using std::vector;

Scenario1_EventAndMenuHookup::Scenario1_EventAndMenuHookup() : rootPage(MainPage::Current)
{
    InitializeComponent();

    InitializeController();
    CreateMenuItems();
}

void Scenario1_EventAndMenuHookup::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (controller != nullptr)
    {
        controller->Menu->Items->Clear();
    }
}

void Scenario1_EventAndMenuHookup::InitializeController()
{
    controller = RadialController::CreateForCurrentView();
    controller->RotationResolutionInDegrees = 1;

    controller->RotationChanged += ref new TypedEventHandler<RadialController^, RadialControllerRotationChangedEventArgs^>(this, &Scenario1_EventAndMenuHookup::OnRotationChanged);
    controller->ButtonClicked += ref new TypedEventHandler<RadialController^, RadialControllerButtonClickedEventArgs^>(this, &Scenario1_EventAndMenuHookup::OnButtonClicked);
    controller->ScreenContactStarted += ref new TypedEventHandler<RadialController^, RadialControllerScreenContactStartedEventArgs^>(this, &Scenario1_EventAndMenuHookup::OnScreenContactStarted);
    controller->ScreenContactContinued += ref new TypedEventHandler<RadialController^, RadialControllerScreenContactContinuedEventArgs^>(this, &Scenario1_EventAndMenuHookup::OnScreenContactContinued);
    controller->ScreenContactEnded += ref new TypedEventHandler<RadialController^, Object^>(this, &Scenario1_EventAndMenuHookup::OnScreenContactEnded);
    controller->ControlAcquired += ref new TypedEventHandler<RadialController^, RadialControllerControlAcquiredEventArgs^>(this, &Scenario1_EventAndMenuHookup::OnControlAcquired);
    controller->ControlLost += ref new TypedEventHandler<RadialController^, Object^>(this, &Scenario1_EventAndMenuHookup::OnControlLost);
}

void Scenario1_EventAndMenuHookup::CreateMenuItems()
{
    vector<RadialControllerMenuItem^> _menuItems =
    {
        RadialControllerMenuItem::CreateFromKnownIcon("Item0", RadialControllerMenuKnownIcon::InkColor),
        RadialControllerMenuItem::CreateFromKnownIcon("Item1", RadialControllerMenuKnownIcon::NextPreviousTrack),
        RadialControllerMenuItem::CreateFromKnownIcon("Item2", RadialControllerMenuKnownIcon::Volume),
        RadialControllerMenuItem::CreateFromIcon("Item3", RandomAccessStreamReference::CreateFromUri(ref new Uri("ms-appx:///Assets/Item3.png"))),
        RadialControllerMenuItem::CreateFromIcon("Item4", RandomAccessStreamReference::CreateFromUri(ref new Uri("ms-appx:///Assets/Item4.png"))),
        RadialControllerMenuItem::CreateFromIcon("Item5", RandomAccessStreamReference::CreateFromUri(ref new Uri("ms-appx:///Assets/Item5.png")))
    };
    menuItems = std::move(_menuItems);

    vector<Slider^> _sliders = { Slider0, Slider1, Slider2, Slider3, Slider4, Slider5 };
    sliders = std::move(_sliders);

    vector<ToggleSwitch^> _toggles = { Toggle0, Toggle1, Toggle2, Toggle3, Toggle4, Toggle5 };
    toggles = std::move(_toggles);

    for (unsigned int i = 0; i < menuItems.size(); ++i)
    {
        RadialControllerMenuItem^ radialControllerItem = menuItems[i];
        SetItemInvokedCallback(radialControllerItem, i);
    }
}

void Scenario1_EventAndMenuHookup::SetItemInvokedCallback(RadialControllerMenuItem^ menuItem, unsigned int index)
{
    // Register Invoked callback to set active item index
    auto weakThis = WeakReference(this);
    menuItem->Invoked += ref new TypedEventHandler<RadialControllerMenuItem^, Object^>(
        [weakThis, index](RadialControllerMenuItem^ sender, Object^ args)
    {
        auto strongThis = weakThis.Resolve<Scenario1_EventAndMenuHookup>();
        if (strongThis)
        {
            strongThis->OnItemInvoked(index);
        }
    });
}

void Scenario1_EventAndMenuHookup::OnItemInvoked(unsigned int selectedItemIndex)
{
    activeItemIndex = selectedItemIndex;
}

void Scenario1_EventAndMenuHookup::AddItem(Object^ sender, RoutedEventArgs^ args)
{
    RadialControllerMenuItem^ radialControllerMenuItem = GetRadialControllerMenuItemFromSender(sender);

    if (!IsItemInMenu(radialControllerMenuItem))
    {
        controller->Menu->Items->Append(radialControllerMenuItem);
    }
}

void Scenario1_EventAndMenuHookup::RemoveItem(Object^ sender, RoutedEventArgs^ args)
{
    RadialControllerMenuItem^ radialControllerMenuItem = GetRadialControllerMenuItemFromSender(sender);

    if (IsItemInMenu(radialControllerMenuItem))
    {
        unsigned int index = GetItemIndex(radialControllerMenuItem);
        controller->Menu->Items->RemoveAt(index);
    }
}

void Scenario1_EventAndMenuHookup::SelectItem(Object^ sender, RoutedEventArgs^ args)
{
    RadialControllerMenuItem^ radialControllerMenuItem = GetRadialControllerMenuItemFromSender(sender);

    if (IsItemInMenu(radialControllerMenuItem))
    {
        controller->Menu->SelectMenuItem(radialControllerMenuItem);
        PrintSelectedItem();
    }
}

void Scenario1_EventAndMenuHookup::SelectPreviouslySelectedItem(Object^ sender, RoutedEventArgs^ args)
{
    if (controller->Menu->TrySelectPreviouslySelectedMenuItem())
    {
        PrintSelectedItem();
    }
}

unsigned int Scenario1_EventAndMenuHookup::GetItemIndex(RadialControllerMenuItem^ item)
{
    unsigned int index = 0;
    controller->Menu->Items->IndexOf(item, &index);

    return index;
}

bool Scenario1_EventAndMenuHookup::IsItemInMenu(RadialControllerMenuItem^ item)
{
    unsigned int index = 0;
    return controller->Menu->Items->IndexOf(item, &index);
}


RadialControllerMenuItem^ Scenario1_EventAndMenuHookup::GetRadialControllerMenuItemFromSender(Object^ sender)
{
    Button^ button = safe_cast<Button^>(sender);
    String^ indexString = button->CommandParameter->ToString();
    int index = std::stoi(indexString->Data());

    return menuItems[index];
}

void Scenario1_EventAndMenuHookup::PrintSelectedItem(Object^ sender, RoutedEventArgs^ args)
{
    PrintSelectedItem();
}

void Scenario1_EventAndMenuHookup::PrintSelectedItem()
{
    log->Text += "\n Selected : " + GetSelectedMenuItemName();
}

String^ Scenario1_EventAndMenuHookup::GetSelectedMenuItemName()
{
    RadialControllerMenuItem^ selectedMenuItem = controller->Menu->GetSelectedMenuItem();

    if (selectedMenuItem)
    {
        return selectedMenuItem->DisplayText;
    }
    else
    {
        return L"System Item";
    }
}

void Scenario1_EventAndMenuHookup::OnLogSizeChanged(Platform::Object ^sender, Platform::Object ^args)
{
    logViewer->ChangeView(nullptr, logViewer->ExtentHeight, nullptr);
}

void Scenario1_EventAndMenuHookup::OnControlAcquired(Windows::UI::Input::RadialController ^sender, Windows::UI::Input::RadialControllerControlAcquiredEventArgs ^args)
{
    log->Text += "\nControl Acquired";
    LogContactInfo(args->Contact);
}

void Scenario1_EventAndMenuHookup::OnControlLost(Windows::UI::Input::RadialController ^sender, Platform::Object ^args)
{
    log->Text += "\nControl Lost";
}

void Scenario1_EventAndMenuHookup::OnScreenContactStarted(Windows::UI::Input::RadialController ^sender, Windows::UI::Input::RadialControllerScreenContactStartedEventArgs ^args)
{
    log->Text += "\nContact Started ";
    LogContactInfo(args->Contact);
}

void Scenario1_EventAndMenuHookup::OnScreenContactContinued(Windows::UI::Input::RadialController ^sender, Windows::UI::Input::RadialControllerScreenContactContinuedEventArgs ^args)
{
    log->Text += "\nContact Continued ";
    LogContactInfo(args->Contact);
}

void Scenario1_EventAndMenuHookup::OnScreenContactEnded(Windows::UI::Input::RadialController ^sender, Platform::Object ^args)
{
    log->Text += "\nContact Ended";
}

void Scenario1_EventAndMenuHookup::OnButtonClicked(Windows::UI::Input::RadialController ^sender, Windows::UI::Input::RadialControllerButtonClickedEventArgs ^args)
{
    log->Text += "\nButton Clicked ";
    LogContactInfo(args->Contact);

    ToggleSwitch^ toggle = toggles[activeItemIndex];
    toggle->IsOn = !toggle->IsOn;
}

void Scenario1_EventAndMenuHookup::OnRotationChanged(Windows::UI::Input::RadialController ^sender, Windows::UI::Input::RadialControllerRotationChangedEventArgs ^args)
{
    log->Text += "\nRotation Changed Delta = " + args->RotationDeltaInDegrees;
    LogContactInfo(args->Contact);

    sliders[activeItemIndex]->Value += args->RotationDeltaInDegrees;
}

void Scenario1_EventAndMenuHookup::LogContactInfo(RadialControllerScreenContact^ contact)
{
    if (contact != nullptr)
    {
        log->Text += "\nBounds = " + contact->Bounds.ToString();
        log->Text += "\nPosition = " + contact->Position.ToString();
    }
}