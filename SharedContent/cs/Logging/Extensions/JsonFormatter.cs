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
using Windows.Data.Json;

namespace SDKTemplate.Logging.Extensions
{
    /// <summary>
    /// Provides very basic Json formatting support to the string extensions.
    /// </summary>
    public class JsonBuilder
    {
        private JsonObject container = new JsonObject();
        private JsonObject o = new JsonObject();

        public JsonBuilder(string className)
        {
            container.Add(className, o);
        }

        public void AddNumber(string key, double? value)
        {
            o.Add(key, value == null ? JsonValue.CreateNullValue() : JsonValue.CreateNumberValue(value.Value));
        }

        public void AddBoolean(string key, bool? value)
        {
            o.Add(key, value == null ? JsonValue.CreateNullValue() : JsonValue.CreateBooleanValue(value.Value));
        }

        public void AddString(string key, object value)
        {
            o.Add(key, value == null ? JsonValue.CreateNullValue() : JsonValue.CreateStringValue(value.ToString()));
        }

        public void AddTimeSpan(string key, TimeSpan? value)
        {
            o.Add(key, value == null ? JsonValue.CreateNullValue() : JsonValue.CreateStringValue(value.ToString()));
        }

        public void AddDateTime(string key, DateTimeOffset? value)
        {
            o.Add(key, value == null ? JsonValue.CreateNullValue() : JsonValue.CreateStringValue(value.Value.ToString("u")));
        }

        public void AddJsonValue(string key, IJsonValue value)
        {
            o.Add(key, value == null ? JsonValue.CreateNullValue() : value);
        }

        public JsonObject GetJsonObject()
        {
            return container;
        }

        public override string ToString()
        {
            return container.ToString();
        }
    }
}
