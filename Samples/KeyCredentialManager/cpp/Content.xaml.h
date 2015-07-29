// Copyright (c) Microsoft. All rights reserved.
#pragma once
#include "Content.g.h"
#include "MainPage.xaml.h"
#include "Account.h"

namespace SDKTemplate
{
    /// <summary>
    /// The main content page for the App that is reached after successful logon.
    /// Has the sign-out (Back to user list) and forget me (Remove Account) buttons.
    ///
    /// Choosing the remove account option calls into the proper APIs to remove the Passport
    /// account key if that exists on the machine and unregister it with the server side.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Content sealed
    {
    public:
        Content();

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;
        Account^ m_activeAccount;

        void Button_Restart_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Button_Forget_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PassportDelete();
        bool DeletePassportServerSide();
    };
}