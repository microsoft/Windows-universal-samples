#include "pch.h"
#include "SmartCardHelpers.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Devices::SmartCards;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Security::Cryptography::Core;

namespace winrt::SDKTemplate
{
    hstring g_smartCardReaderDeviceId;
    IBuffer g_adminKey;

    bool ValidateTPMSmartCard()
    {
        return !g_smartCardReaderDeviceId.empty();
    }

    hstring GetSmartCardReaderDeviceId()
    {
        return g_smartCardReaderDeviceId;
    }

    void SetSmartCardReaderDeviceId(hstring const& id)
    {
        g_smartCardReaderDeviceId = id;
    }

    IBuffer GetAdminKey()
    {
        return g_adminKey;
    }

    void SetAdminKey(IBuffer const& buffer)
    {
        g_adminKey = buffer;
    }

    IAsyncOperation<SmartCard> GetSmartCardAsync()
    {
        SmartCardReader reader = co_await SmartCardReader::FromIdAsync(g_smartCardReaderDeviceId);
        auto cards = co_await reader.FindAllCardsAsync();

        if (cards.Size() != 1)
        {
            throw winrt::hresult_error(E_UNEXPECTED);
        }
        else
        {
            co_return cards.GetAt(0);
        }
    }

    IBuffer CalculateChallengeResponse(IBuffer const& challenge, IBuffer const& adminKey)
    {
        SymmetricKeyAlgorithmProvider provider = SymmetricKeyAlgorithmProvider::OpenAlgorithm(SymmetricAlgorithmNames::TripleDesCbc());
        CryptographicKey key = provider.CreateSymmetricKey(adminKey);
        return CryptographicEngine::Encrypt(key, challenge, nullptr);
    }
}