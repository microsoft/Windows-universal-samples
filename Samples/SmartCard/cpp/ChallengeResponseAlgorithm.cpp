#include "pch.h"
#include "ChallengeResponseAlgorithm.h"

using namespace SDKTemplate;
using namespace Windows::Storage::Streams;

using namespace Windows::Security::Cryptography;
using namespace Windows::Security::Cryptography::Core;

// Calculates the response by encrypting the challenge by using Triple DES (3DES).
// If the resulting values are the same, the authentication is successful.
IBuffer^ ChallengeResponseAlgorithm::CalculateResponse(IBuffer^ challenge, IBuffer^ adminkey)
{
    SymmetricKeyAlgorithmProvider^ objAlg = SymmetricKeyAlgorithmProvider::OpenAlgorithm(SymmetricAlgorithmNames::TripleDesCbc);
    auto symetricKey = objAlg->CreateSymmetricKey(adminkey);
    auto buffEncrypted = CryptographicEngine::Encrypt(symetricKey, challenge, nullptr);
    return buffEncrypted;
}
