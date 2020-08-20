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
#include "Scenario6_SecondaryTileNotification.h"
#include "Scenario6_SecondaryTileNotification.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace NotificationsExtensions;
using namespace NotificationsExtensions::Badges;
using namespace NotificationsExtensions::Tiles;
using namespace Windows::Foundation;
using namespace Windows::UI::Notifications;
using namespace Windows::UI::StartScreen;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario6_SecondaryTileNotification::Scenario6_SecondaryTileNotification()
    {
        InitializeComponent();
    }

    bool Scenario6_SecondaryTileNotification::VerifyTileIsPinned()
    {
        if (SecondaryTile::Exists(SampleHelpers::DynamicTileId))
        {
            return true;
        }
        rootPage.NotifyUser(hstring(SampleHelpers::DynamicTileId) + L" is not pinned.", NotifyType::ErrorMessage);
        return false;
    }

    fire_and_forget Scenario6_SecondaryTileNotification::PinLiveTile_Click(IInspectable sender, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Prepare the images for our tile to be pinned.
        Uri square150x150Logo(L"ms-appx:///Assets/square150x150Tile-sdk.png");
        Uri wide310x150Logo(L"ms-appx:///Assets/wide310x150Tile-sdk.png");

        // During creation of the secondary tile, an application may set additional arguments on the tile that will be passed in during activation.
        // These arguments should be meaningful to the application. In this sample, we'll pass in the date and time the secondary tile was pinned.
        hstring tileActivationArguments = hstring(SampleHelpers::DynamicTileId) + L" WasPinnedAt=" + SampleHelpers::CurrentTimeAsString();

        // Create a Secondary tile with all the required properties and sets perfered size to Wide310x150.
        SecondaryTile secondaryTile(SampleHelpers::DynamicTileId,
            L"A Live Secondary Tile",
            tileActivationArguments,
            square150x150Logo,
            TileSize::Wide310x150);

        // Adding the wide tile logo.
        secondaryTile.VisualElements().Wide310x150Logo(wide310x150Logo);

        // The display of the app name can be controlled for each tile size.
        secondaryTile.VisualElements().ShowNameOnSquare150x150Logo(true);
        secondaryTile.VisualElements().ShowNameOnWide310x150Logo(true);

        // Specify a foreground text value.
        // The tile background color is inherited from the parent unless a separate value is specified.
        secondaryTile.VisualElements().ForegroundText(ForegroundText::Dark);

        // OK, the tile is created and we can now attempt to pin the tile.
        // Note that the status message is updated when the async operation to pin the tile completes.
        bool isPinned = co_await secondaryTile.RequestCreateForSelectionAsync(SampleHelpers::GetElementRect(sender), Windows::UI::Popups::Placement::Below);
        if (isPinned)
        {
            rootPage.NotifyUser(L"Secondary tile successfully pinned.", NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"Secondary tile not pinned.", NotifyType::ErrorMessage);
        }
    }

    void Scenario6_SecondaryTileNotification::SendTileNotification_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (VerifyTileIsPinned())
        {
            AdaptiveText adaptiveText;
            adaptiveText.Text(L"Sent to a secondary tile from NotificationsExtensions!");
            adaptiveText.HintWrap(true);
            TileBindingContentAdaptive contentAdaptive;
            contentAdaptive.Children().Append(adaptiveText);
            TileBinding tileBinding;
            tileBinding.Content(contentAdaptive);
            TileVisual tileVisual;
            tileVisual.TileWide(tileBinding);
            TileContent tileContent;
            tileContent.Visual(tileVisual);
            // Send the notification to the secondary tile by creating a secondary tile updater
            TileUpdateManager::CreateTileUpdaterForSecondaryTile(SampleHelpers::DynamicTileId).Update(TileNotification(tileContent.GetXml()));

            rootPage.NotifyUser(L"Tile notification sent to " + hstring(SampleHelpers::DynamicTileId), NotifyType::StatusMessage);
        }
    }

    void Scenario6_SecondaryTileNotification::SendBadgeNotification_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (VerifyTileIsPinned())
        {
            BadgeNumericNotificationContent badgeContent(6);

            // Send the notification to the secondary tile
            BadgeUpdateManager::CreateBadgeUpdaterForSecondaryTile(SampleHelpers::DynamicTileId).Update(BadgeNotification(badgeContent.GetXml()));

            rootPage.NotifyUser(L"Badge notification sent to " + hstring(SampleHelpers::DynamicTileId), NotifyType::StatusMessage);
        }
    }

    void Scenario6_SecondaryTileNotification::SendTileNotificationWithStringManipulation_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (VerifyTileIsPinned())
        {
            static constexpr wchar_t tileXmlString[] = L"<tile>"
                L"<visual version='2'>"
                L"<binding template='TileWide310x150Text04' fallback='TileWideText04'>"
                L"<text id='1'>Send to a secondary tile from strings</text>"
                L"</binding>"
                L"<binding template='TileSquare150x150Text04' fallback='TileSquareText04'>"
                L"<text id='1'>Send to a secondary tile from strings</text>"
                L"</binding>"
                L"</visual>"
                L"</tile>";

            Windows::Data::Xml::Dom::XmlDocument tileDOM;
            tileDOM.LoadXml(tileXmlString);
            TileNotification tile(tileDOM);

            // Send the notification to the secondary tile by creating a secondary tile updater
            TileUpdateManager::CreateTileUpdaterForSecondaryTile(SampleHelpers::DynamicTileId).Update(tile);

            rootPage.NotifyUser(L"Tile notification sent to " + hstring(SampleHelpers::DynamicTileId), NotifyType::StatusMessage);
        }
    }

    void Scenario6_SecondaryTileNotification::SendBadgeNotificationWithStringManipulation_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (VerifyTileIsPinned())
        {
            static constexpr wchar_t badgeXmlString[] = L"<badge value='9'/>";
            Windows::Data::Xml::Dom::XmlDocument badgeDOM;
            badgeDOM.LoadXml(badgeXmlString);
            BadgeNotification badge(badgeDOM);

            // Send the notification to the secondary tile
            BadgeUpdateManager::CreateBadgeUpdaterForSecondaryTile(SampleHelpers::DynamicTileId).Update(badge);

            rootPage.NotifyUser(L"Badge notification sent to " + hstring(SampleHelpers::DynamicTileId), NotifyType::StatusMessage);
        }
    }
}
