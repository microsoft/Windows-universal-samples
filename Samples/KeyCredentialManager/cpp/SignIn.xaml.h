#pragma once
#include "SignIn.g.h"
#include "MainPage.xaml.h"
#include "Account.h"
#include "pch.h"
#include "SignIn.xaml.h"
#include "MainPage.xaml.h"
#include "UserSelect.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// TODO
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class SignIn sealed
    {
    public:
        SignIn();

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void Button_SignIn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Button_Passport_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        concurrency::task<void> PassportAvailableCheckAsync();
        concurrency::task<void> SignInPassportAsync(bool passportIsPrimaryLogin);
        concurrency::task<bool> SignInPasswordAsync(bool calledFromPassport);
        concurrency::task<bool> AuthenticatePasswordCredentialsAsync();
        concurrency::task<bool> AuthenticatePassportAsync();
        concurrency::task<bool> AddPassportToAccountOnServerAsync();
        void SuccessfulSignIn(Account^ account);
        concurrency::task<bool> CreatePassportKeyAsync(Platform::String^ accountId);
        concurrency::task<Windows::Storage::Streams::IBuffer^> GetPassportAuthenticationMessageAsync(Windows::Storage::Streams::IBuffer^ message, Platform::String^ accountId);

        MainPage^ rootPage;
        bool m_addingAccount = true;
        Account^ m_account;
        bool m_passportAvailable = true;
    };
}