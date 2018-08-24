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

using System.Text;
using Windows.Data.Json;
using Windows.Web.Http;

namespace SDKTemplate.Logging.Extensions
{
    public static class HttpProgressStringExtensions
    {
        /// <summary>
        /// Provides a JSON object for HttpProgress
        /// </summary>
        public static JsonObject ToJsonObject(this HttpProgress p)
        {
            var builder = new JsonBuilder("HttpProgress");
            builder.AddString("Stage", p.Stage);
            builder.AddNumber("Retries", p.Retries);
            builder.AddNumber("BytesSent", p.BytesSent);
            builder.AddNumber("TotalBytesToSend", p.TotalBytesToSend);
            builder.AddNumber("BytesReceived", p.BytesReceived);
            builder.AddNumber("TotalBytesToReceive", p.TotalBytesToReceive);
            return builder.GetJsonObject();
        }
    }
}
