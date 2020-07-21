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
#include "Scenario2_Notifications.h"
#include "Scenario2_Notifications.g.cpp"

using namespace winrt;
using namespace winrt::NotificationsExtensions::Badges;
using namespace winrt::NotificationsExtensions::Tiles;
using namespace winrt::Windows::Data::Xml::Dom;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Notifications::Scenario2_Notifications()
    {
        InitializeComponent();
    }

    void Scenario2_Notifications::ClearBadge_Click(IInspectable const&, RoutedEventArgs const&)
    {
        BadgeUpdateManager::CreateBadgeUpdaterForApplication().Clear();
        rootPage.NotifyUser(L"Badge notification cleared", NotifyType::StatusMessage);
    }

    void Scenario2_Notifications::SendBadge_Click(IInspectable const&, RoutedEventArgs const&)
    {
        BadgeNumericNotificationContent badgeContent(6);
        BadgeUpdateManager::CreateBadgeUpdaterForApplication().Update(BadgeNotification(badgeContent.GetXml()));
        rootPage.NotifyUser(L"Badge notification sent", NotifyType::StatusMessage);
    }

    void Scenario2_Notifications::SendBadgeWithStringManipulation_Click(IInspectable const&, RoutedEventArgs const&)
    {
        hstring badgeXmlString{ L"<badge value='6'/>" };
        XmlDocument badgeDOM;
        badgeDOM.LoadXml(badgeXmlString);
        BadgeNotification badge(badgeDOM);
        BadgeUpdateManager::CreateBadgeUpdaterForApplication().Update(badge);
        rootPage.NotifyUser(L"Badge notification sent", NotifyType::StatusMessage);
    }

    void Scenario2_Notifications::SendTile_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto tileDOM = TileUpdateManager::GetTemplateContent(TileTemplateType::TileWide310x150SmallImageAndText03);
        tileDOM.SelectSingleNode(L"//image[@id=1]/@src").NodeValue(box_value(L"'ms-appx:///Assets/tile-sdk.png"));
        tileDOM.SelectSingleNode(L"//text[@id=1]").InnerText(L"This tile notification has an image, but it won't be displayed on the lock screen");
        TileUpdateManager::CreateTileUpdaterForApplication().Update(TileNotification(tileDOM));
        rootPage.NotifyUser(L"Tile notification sent", NotifyType::StatusMessage);
    }

    void Scenario2_Notifications::SendTileWithStringManipulation_Click(IInspectable const&, RoutedEventArgs const&)
    {
        hstring tileXmlString = L"<tile>"
            L"<visual version='2'>"
            L"<binding template='TileWide310x150SmallImageAndText03' fallback='TileWideSmallImageAndText03'>"
            L"<image id='1' src='ms-appx:///Assets/tile-sdk.png'/>"
            L"<text id='1'>This tile notification has an image, but it won't be displayed on the lock screen</text>"
            L"</binding>"
            L"</visual>"
            L"</tile>";

        XmlDocument tileDOM;
        tileDOM.LoadXml(tileXmlString);
        TileNotification tile(tileDOM);
        TileUpdateManager::CreateTileUpdaterForApplication().Update(tile);
        rootPage.NotifyUser(L"Tile notification sent", NotifyType::StatusMessage);
    }

    void Scenario2_Notifications::ClearTile_Click(IInspectable const&, RoutedEventArgs const&)
    {
        TileUpdateManager::CreateTileUpdaterForApplication().Clear();
        rootPage.NotifyUser(L"Tile notification cleared", NotifyType::StatusMessage);
    }

}
