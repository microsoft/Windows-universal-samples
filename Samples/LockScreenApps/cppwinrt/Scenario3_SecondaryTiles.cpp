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
#include "Scenario3_SecondaryTiles.h"
#include "Scenario3_SecondaryTiles.g.cpp"

using namespace winrt;
using namespace winrt::NotificationsExtensions::Badges;
using namespace winrt::NotificationsExtensions::Tiles;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::UI::Popups;
using namespace winrt::Windows::UI::StartScreen;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_SecondaryTiles::Scenario3_SecondaryTiles()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario3_SecondaryTiles::CreateBadgeTile_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        if (!SecondaryTile::Exists(BADGE_TILE_ID))
        {
            SecondaryTile secondTile(
                BADGE_TILE_ID,
                L"LockScreen C++/WinRT - Badge only",
                L"BADGE_ARGS",
                Uri(L"ms-appx:///Assets/squareTile-sdk.png"),
                TileSize::Square150x150
                );
            secondTile.LockScreenBadgeLogo(Uri(L"ms-appx:///Assets/badgelogo-sdk.png"));

            bool isPinned = co_await secondTile.RequestCreateForSelectionAsync(GetElementRect(sender.as<FrameworkElement>()), Placement::Above);
            if (isPinned)
            {
                BadgeNumericNotificationContent badgeContent(2);
                BadgeUpdateManager::CreateBadgeUpdaterForSecondaryTile(BADGE_TILE_ID).Update(BadgeNotification(badgeContent.GetXml()));
                rootPage.NotifyUser(L"Secondary tile created and badge updated. Go to Settings to add it to the lock screen.", NotifyType::StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(L"Tile not created.", NotifyType::ErrorMessage);
            }
        }
        else
        {
            rootPage.NotifyUser(L"Badge secondary tile already exists.", NotifyType::ErrorMessage);
        }
    }

    fire_and_forget Scenario3_SecondaryTiles::CreateBadgeAndTextTile_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        if (!SecondaryTile::Exists(TEXT_TILE_ID))
        {
            SecondaryTile secondTile(
                TEXT_TILE_ID,
                L"LockScreen C++/WinRT - Badge and tile text",
                L"TEXT_ARGS",
                Uri(L"ms-appx:///Assets/squareTile-sdk.png"),
                TileSize::Wide310x150
                );
            secondTile.VisualElements().Wide310x150Logo(Uri(L"ms-appx:///Assets/tile-sdk.png"));
            secondTile.LockScreenBadgeLogo(Uri(L"ms-appx:///Assets/badgelogo-sdk.png"));
            secondTile.LockScreenDisplayBadgeAndTileText(true);

            bool isPinned = co_await secondTile.RequestCreateForSelectionAsync(GetElementRect(sender.as<FrameworkElement>()), Placement::Above);
            if (isPinned)
            {
                auto tileDOM = TileUpdateManager::GetTemplateContent(TileTemplateType::TileWide310x150Text03);
                tileDOM.SelectSingleNode(L"//text[@id=1]").InnerText(L"Text for the lock screen");
                TileUpdateManager::CreateTileUpdaterForSecondaryTile(TEXT_TILE_ID).Update(TileNotification(tileDOM));
                rootPage.NotifyUser(L"Secondary tile created and updated. Go to PC settings to add it to the lock screen.", NotifyType::StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(L"Tile not created.", NotifyType::ErrorMessage);
            }
        }
        else
        {
            rootPage.NotifyUser(L"Badge and text secondary tile already exists.", NotifyType::ErrorMessage);
        }
    }

    Rect Scenario3_SecondaryTiles::GetElementRect(FrameworkElement const& element)
    {
        GeneralTransform transform = element.TransformToVisual(nullptr);
        return transform.TransformBounds({ {}, Size(static_cast<float>(element.ActualWidth()), static_cast<float>(element.ActualHeight())) });
    }
}
