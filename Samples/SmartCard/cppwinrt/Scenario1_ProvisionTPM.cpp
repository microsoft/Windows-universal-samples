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
#include "SampleConfiguration.h"
#include "Scenario1_ProvisionTPM.h"
#include "Scenario1_ProvisionTPM.g.cpp"

namespace winrt
{
    using namespace Windows::Devices::SmartCards;
    using namespace Windows::Foundation;
    using namespace Windows::Security::Cryptography;
    using namespace Windows::Security::Cryptography::Core;
    using namespace Windows::Storage::Streams;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Controls;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario1_ProvisionTPM::Scenario1_ProvisionTPM()
    {
        InitializeComponent();
    }


    /// <summary>
    /// Click handler for the 'create' button. 
    /// </summary>
    /// <param name="sender">The 'Create' button</param>
    fire_and_forget Scenario1_ProvisionTPM::Create_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        auto b = sender.as<Button>();
        b.IsEnabled(false);
        m_rootPage.NotifyUser(L"Creating TPM virtual smart card...", NotifyType::StatusMessage);

        try
        {
            SmartCardPinPolicy pinPolicy = ParsePinPolicy();

            IBuffer adminkey = CryptographicBuffer::GenerateRandom(ADMIN_KEY_LENGTH_IN_BYTES);

            SmartCardProvisioning provisioning = co_await SmartCardProvisioning::RequestVirtualSmartCardCreationAsync(FriendlyName().Text(), adminkey, pinPolicy);

            // If card creation is cancelled by the user,
            // RequestVirtualSmartCard will return nullptr instead of a
            // SmartCardProvisioning object.
            if (!provisioning)
            {
                m_rootPage.NotifyUser(L"TPM virtual smart card creation was canceled by the user.", NotifyType::StatusMessage);
                b.IsEnabled(true);
                co_return;
            }

            // The following two lines are not directly related to TPM virtual
            // smart card creation, but are used to demonstrate how to handle
            // CardAdded events by registering an event handler with a
            // SmartCardReader object.  Since we are using a TPM virtual smart
            // card in this case, the card cannot actually be added to or
            // removed from the reader, but a CardAdded event will fire as
            // soon as the event handler is added, since the card is already
            // inserted.
            //
            // We must retain a reference to the SmartCardReader object to
            // which we are adding the event handler.  Hence we assign the
            // reader object associated with the SmartCardProvisioning we
            // received from RequestVirtualSmartCardCreationAsync to the
            // class's "reader" member. Then, we add the HandleCardAdded method
            // as an event handler. Adding the handler with auto_revoke
            // returns an object which unregisters the handler at destruction.
            // We store that object into a member variable so that the handler
            // is unregistered when the page is destructed.
            m_reader = provisioning.SmartCard().Reader();
            m_cardAddedToken = m_reader.CardAdded(winrt::auto_revoke, { get_weak(), &Scenario1_ProvisionTPM::HandleCardAdded });

            // Store the reader's device ID and admin key to enable the
            // following scenarios in the sample.
            SetSmartCardReaderDeviceId(provisioning.SmartCard().Reader().DeviceId());
            SetAdminKey(adminkey);

            // Once RequestVirtualSmartCardCreationAsync has returned, the card
            // is already provisioned and ready to use.  Therefore, the steps
            // in this block are not actually necessary at this point.
            // However, should you want to re-provision the card in the future,
            // you would follow this sequence: acquire a challenge context,
            // calculate a response, and then call ProvisionAsync on the
            // challenge context with the calculated response.

            // We rely on this object's destructor to clean up the context.
            SmartCardChallengeContext context = co_await provisioning.GetChallengeContextAsync();
            IBuffer response = CalculateChallengeResponse(context.Challenge(), adminkey);
            co_await context.ProvisionAsync(response, true);

            m_rootPage.NotifyUser(L"TPM virtual smart card is provisioned and ready for use.", NotifyType::StatusMessage);
            // context destructed here
        }
        catch (hresult_error const& ex)
        {
            // Two potentially common error scenarios when creating a TPM
            // virtual smart card are that the user's machine may not have
            // a TPM, or the TPM may not be ready for use.  It is important
            // to explicitly check for these scenarios by checking the
            // error code of any exceptions thrown by
            // RequestVirtualSmartCardCreationAsync and gracefully
            // providing a suitable message to the user.
            if (NTE_DEVICE_NOT_FOUND == ex.code())
            {
                m_rootPage.NotifyUser(L"We were unable to find a Trusted Platform Module on your machine. A TPM is required to use a TPM Virtual Smart Card.", NotifyType::ErrorMessage);
            }
            else if (NTE_DEVICE_NOT_READY == ex.code())
            {
                m_rootPage.NotifyUser(L"Your Trusted Platform Module is not ready for use. Please contact your administrator for assistance with initializing your TPM.", NotifyType::ErrorMessage);
            }
            else
            {
                m_rootPage.NotifyUser(L"TPM virtual smart card creation failed with exception: " + ex.message(), NotifyType::ErrorMessage);
            }
        }

        b.IsEnabled(true);
    }

    void Scenario1_ProvisionTPM::HandleCardAdded(SmartCardReader const&, CardAddedEventArgs const&)
    {
        // Note that this event is raised from a background thread.
        // However, the NotifyUser method is safe to call from background threads.
        m_rootPage.NotifyUser(L"Card added to reader " + m_reader.Name() + L".", NotifyType::StatusMessage);
    }

    SmartCardPinCharacterPolicyOption ParsePinPolicyOption(hstring const& value)
    {
        static constexpr wchar_t pinPolicyDisallowed[] = L"Disallowed";
        static constexpr wchar_t pinPolicyAllowed[] = L"Allowed";
        static constexpr wchar_t pinPolicyRequireOne[] = L"Require At Least One";

        if (value == pinPolicyDisallowed)
        {
            return SmartCardPinCharacterPolicyOption::Disallow;
        }
        else if (value == pinPolicyAllowed)
        {
            return SmartCardPinCharacterPolicyOption::Allow;
        }
        else if (value == pinPolicyRequireOne)
        {
            return SmartCardPinCharacterPolicyOption::RequireAtLeastOne;
        }
        else
        {
            throw hresult_invalid_argument();
        }
    }

    SmartCardPinPolicy Scenario1_ProvisionTPM::ParsePinPolicy()
    {
        SmartCardPinPolicy pinPolicy;
        pinPolicy.MinLength(std::stoul(PinMinLength().Text().c_str()));
        pinPolicy.MaxLength(std::stoul(PinMaxLength().Text().c_str()));

        const auto upperCaseSelection = winrt::unbox_value<hstring>(PinUppercase().SelectedValue());
        pinPolicy.UppercaseLetters(ParsePinPolicyOption(upperCaseSelection));

        const auto lowerCaseSelection = winrt::unbox_value<hstring>(PinLowercase().SelectedValue());
        pinPolicy.LowercaseLetters(ParsePinPolicyOption(lowerCaseSelection));

        const auto digitsSelection = winrt::unbox_value<hstring>(PinDigits().SelectedValue());
        pinPolicy.Digits(ParsePinPolicyOption(digitsSelection));

        const auto specialSelection = winrt::unbox_value<hstring>(PinSpecial().SelectedValue());
        pinPolicy.SpecialCharacters(ParsePinPolicyOption(specialSelection));

        return pinPolicy;
    }
}