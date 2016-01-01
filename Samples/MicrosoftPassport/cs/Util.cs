using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Security.Credentials;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;
using Windows.Web.Http;

namespace SDKTemplate
{
    // Fluent class for posting JSON requests to the server and receiving the result.
    //
    // result - await JsonRequest.Create().AddString("key", "value").AddString("key", "value").PostAsync(relativeUri);
    //
    // For simplicity, we do not report the error to the caller.
    // A full program would record the error in order to provide more meaningful feedback to the user.
    class JsonRequest
    {
        private static Uri serverBaseUri = new Uri("http://localhost:59992/");

        private const int E_WINHTTP_TIMEOUT = unchecked((int)0x80072ee2);
        private const int E_WINHTTP_NAME_NOT_RESOLVED = unchecked((int)0x80072ee7);
        private const int E_WINHTTP_CANNOT_CONNECT = unchecked((int)0x80072efd);
        private const int E_WINHTTP_CONNECTION_ERROR = unchecked((int)0x80072efe);

        private JsonObject message = new JsonObject();

        public static JsonRequest Create()
        {
            return new JsonRequest();
        }

        public JsonRequest AddString(string key, string value)
        {
            message.Add(key, JsonValue.CreateStringValue(value));
            return this;
        }

        public async Task<JsonValue> PostAsync(string relativeUri)
        {
            HttpStringContent content = new HttpStringContent(message.Stringify(), UnicodeEncoding.Utf8, "application/json");

            HttpClient httpClient = new HttpClient();
            HttpResponseMessage httpResponse = null;
            try
            {
                httpResponse = await httpClient.PostAsync(new Uri(serverBaseUri, relativeUri), content);
            }
            catch (Exception ex)
            {
                switch (ex.HResult)
                {
                    case E_WINHTTP_TIMEOUT:
                    // The connection to the server timed out.
                    case E_WINHTTP_NAME_NOT_RESOLVED:
                    case E_WINHTTP_CANNOT_CONNECT:
                    case E_WINHTTP_CONNECTION_ERROR:
                    // Unable to connect to the server. Check that you have Internet access.
                    default:
                        // "Unexpected error connecting to server: ex.Message
                        return null;
                }
            }

            // We assume that if the server responds at all, it responds with valid JSON.
            return JsonValue.Parse(await httpResponse.Content.ReadAsStringAsync());
        }
    }

    class Util
    {
        const int NTE_NO_KEY = unchecked((int)0x8009000D);
        const int NTE_PERM = unchecked((int)0x80090010);

        public static async Task<bool> TryDeleteCredentialAccountAsync(string userId)
        {
            bool deleted = false;

            try
            {
                await KeyCredentialManager.DeleteAsync(userId);
                deleted = true;
            }
            catch (Exception ex)
            {
                switch (ex.HResult)
                {
                    case NTE_NO_KEY:
                        // Key is already deleted. Ignore this error.
                        break;
                    case NTE_PERM:
                        // Access is denied. Ignore this error. We tried our best.
                        break;
                    default:
                        throw;
                }
            }
            return deleted;
        }
    }
}
