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



using System;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;

namespace SDKTemplate.Helpers
{
    public class CommonLicenseRequest
    {
        private string lastErrorMessage;
        private HttpClient httpClient;
        public string GetLastErrorMessage()
        {
            return lastErrorMessage;
        }
        public CommonLicenseRequest(HttpClient client = null)
        {
            if (client == null)
                httpClient = new HttpClient();
            else
                httpClient = client;
            lastErrorMessage = string.Empty;
        }
        /// <summary>
        /// Invoked to acquire the PlayReady license.
        /// </summary>
        /// <param name="licenseServerUri">License Server URI to retrieve the PlayReady license.</param>
        /// <param name="httpRequestContent">HttpContent including the Challenge transmitted to the PlayReady server.</param>
        public async virtual Task<HttpContent> AcquireLicenseAsync(Uri licenseServerUri, HttpContent httpRequestContent)
        {
            try
            {
                httpClient.DefaultRequestHeaders.Add("msprdrm_server_redirect_compat", "false");
                httpClient.DefaultRequestHeaders.Add("msprdrm_server_exception_compat", "false");

                HttpResponseMessage response = await httpClient.PostAsync(licenseServerUri, httpRequestContent);
                response.EnsureSuccessStatusCode();

                if (response.StatusCode == HttpStatusCode.OK)
                {
                    lastErrorMessage = string.Empty;
                    return response.Content;
                }
                else
                {
                    lastErrorMessage = "AcquireLicense - Http Response Status Code: " + response.StatusCode.ToString();
                }
            }
            catch (Exception exception)
            {
                lastErrorMessage = exception.Message;
                return null;
            }
            return null;
        }
    }
}