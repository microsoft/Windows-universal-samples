#pragma once

namespace winrt::SDKTemplate
{
    const uint32_t ADMIN_KEY_LENGTH_IN_BYTES = 24;

    bool ValidateTPMSmartCard();

    winrt::hstring GetSmartCardReaderDeviceId();
    void SetSmartCardReaderDeviceId(winrt::hstring const& id);

    winrt::Windows::Storage::Streams::IBuffer GetAdminKey();
    void SetAdminKey(winrt::Windows::Storage::Streams::IBuffer const& buffer);

    Windows::Foundation::IAsyncOperation<Windows::Devices::SmartCards::SmartCard> GetSmartCardAsync();

    Windows::Storage::Streams::IBuffer CalculateChallengeResponse(
        Windows::Storage::Streams::IBuffer const& challenge,
        Windows::Storage::Streams::IBuffer const& adminKey);
}
