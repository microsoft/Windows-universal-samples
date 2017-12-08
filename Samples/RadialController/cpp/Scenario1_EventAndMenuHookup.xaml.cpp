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
using std::vector;

Scenario1_EventAndMenuHookup::Scenario1_EventAndMenuHookup()
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

    // Wire events
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
    AddKnownIconItems();
    AddCustomIconItems();
    AddFontGlyphItems();

    // Set the invoked callbacks for each menu item
    for (unsigned int i = 0; i < menuItems.size(); ++i)
    {
        RadialControllerMenuItem^ radialControllerItem = menuItems[i];
        SetItemInvokedCallback(radialControllerItem, i);
    }
}

void Scenario1_EventAndMenuHookup::AddKnownIconItems()
{
    menuItems.push_back(RadialControllerMenuItem::CreateFromKnownIcon("Item 0", RadialControllerMenuKnownIcon::InkColor));
    sliders.push_back(slider0);
    toggles.push_back(toggle0);

    menuItems.push_back(RadialControllerMenuItem::CreateFromKnownIcon("Item 1", RadialControllerMenuKnownIcon::NextPreviousTrack));
    sliders.push_back(slider1);
    toggles.push_back(toggle1);
}

void Scenario1_EventAndMenuHookup::AddCustomIconItems()
{
    menuItems.push_back(RadialControllerMenuItem::CreateFromIcon(L"Item 2", RandomAccessStreamReference::CreateFromUri(ref new Uri(L"ms-appx:///Assets/Item2.png"))));
    sliders.push_back(slider2);
    toggles.push_back(toggle2);

    menuItems.push_back(RadialControllerMenuItem::CreateFromIcon(L"Item 3", RandomAccessStreamReference::CreateFromUri(ref new Uri(L"ms-appx:///Assets/Item3.png"))));
    sliders.push_back(slider3);
    toggles.push_back(toggle3);
}

void Scenario1_EventAndMenuHookup::AddFontGlyphItems()
{
    menuItems.push_back(RadialControllerMenuItem::CreateFromFontGlyph(L"Item 4", L"\x2764", L"Segoe UI Emoji"));
    sliders.push_back(slider4);
    toggles.push_back(toggle4);

    menuItems.push_back(RadialControllerMenuItem::CreateFromFontGlyph(L"Item 5", L"\ue102", L"Symbols", ref new Uri(L"ms-appx:///Assets/Symbols.ttf")));
    sliders.push_back(slider5);
    toggles.push_back(toggle5);
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
        log->Text += "\n Added : " + radialControllerMenuItem->DisplayText;
    }
}

void Scenario1_EventAndMenuHookup::RemoveItem(Object^ sender, RoutedEventArgs^ args)
{
    RadialControllerMenuItem^ radialControllerMenuItem = GetRadialControllerMenuItemFromSender(sender);

    if (IsItemInMenu(radialControllerMenuItem))
    {
        unsigned int index = GetItemIndex(radialControllerMenuItem);
        controller->Menu->Items->RemoveAt(index);
        log->Text += "\n Removed : " + radialControllerMenuItem->DisplayText;
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
    controller->Menu->TrySelectPreviouslySelectedMenuItem();
    PrintSelectedItem();
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
    RadialControllerMenuItem^ selectedMenuItem = controller->Menu->GetSelectedMenuItem();
    String^ itemName = nullptr;

    if (selectedMenuItem)
    {
        itemName = selectedMenuItem->DisplayText;
    }
    else if (selectedMenuItem == nullptr)
    {
        itemName = L"System Item";
    }

    if (itemName != nullptr)
    {
        log->Text += "\n Selected : " + itemName;
    }
}

void Scenario1_EventAndMenuHookup::OnLogSizeChanged(Object^ sender, Object^ args)
{
    logViewer->ChangeView(nullptr, logViewer->ExtentHeight, nullptr);
}

void Scenario1_EventAndMenuHookup::OnControlAcquired(RadialController^ sender, RadialControllerControlAcquiredEventArgs^ args)
{
    log->Text += "\n Control Acquired";
    LogContactInfo(args->Contact);
}

void Scenario1_EventAndMenuHookup::OnControlLost(RadialController^ sender, Object^ args)
{
    log->Text += "\n Control Lost";
}

void Scenario1_EventAndMenuHookup::OnScreenContactStarted(RadialController^ sender, RadialControllerScreenContactStartedEventArgs^ args)
{
    log->Text += "\n Contact Started ";
    LogContactInfo(args->Contact);
}

void Scenario1_EventAndMenuHookup::OnScreenContactContinued(RadialController^ sender, RadialControllerScreenContactContinuedEventArgs^ args)
{
    log->Text += "\n Contact Continued ";
    LogContactInfo(args->Contact);
}

void Scenario1_EventAndMenuHookup::OnScreenContactEnded(RadialController^ sender, Object^ args)
{
    log->Text += "\n Contact Ended";
}

void Scenario1_EventAndMenuHookup::ToggleMenuSuppression(Object^ sender, RoutedEventArgs^ args)
{
    RadialControllerConfiguration^ radialControllerConfig = RadialControllerConfiguration::GetForCurrentView();

    if (MenuSuppressionToggleSwitch->IsOn)
    {
        radialControllerConfig->ActiveControllerWhenMenuIsSuppressed = controller;
    }

    radialControllerConfig->IsMenuSuppressed = MenuSuppressionToggleSwitch->IsOn;
}

void Scenario1_EventAndMenuHookup::OnButtonClicked(RadialController^ sender, RadialControllerButtonClickedEventArgs^ args)
{
    log->Text += "\n Button Clicked ";
    LogContactInfo(args->Contact);

    ToggleSwitch^ toggle = toggles[activeItemIndex];
    toggle->IsOn = !toggle->IsOn;
}

void Scenario1_EventAndMenuHookup::OnRotationChanged(RadialController^ sender, RadialControllerRotationChangedEventArgs^ args)
{
    log->Text += "\n Rotation Changed Delta = " + args->RotationDeltaInDegrees;
    LogContactInfo(args->Contact);

    sliders[activeItemIndex]->Value += args->RotationDeltaInDegrees;
}

void Scenario1_EventAndMenuHookup::LogContactInfo(RadialControllerScreenContact^ contact)
{
    if (contact != nullptr)
    {
        log->Text += "\n Bounds = " + contact->Bounds.ToString();
        log->Text += "\n Position = " + contact->Position.ToString();
    }
}