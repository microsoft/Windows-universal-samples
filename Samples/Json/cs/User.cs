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
using System.Collections.ObjectModel;
using Windows.Data.Json;

namespace SDKTemplate
{
    class User
    {
        private const string idKey = "id";
        private const string phoneKey = "phone";
        private const string nameKey = "name";
        private const string educationKey = "education";
        private const string timezoneKey = "timezone";
        private const string verifiedKey = "verified";

        public User()
        {
        }

        public User(string jsonString) : this()
        {
            JsonObject jsonObject = JsonObject.Parse(jsonString);
            Id = jsonObject.GetNamedString(idKey, "");

            IJsonValue phoneJsonValue = jsonObject.GetNamedValue(phoneKey);
            if (phoneJsonValue.ValueType == JsonValueType.Null)
            {
                Phone = null;
            }
            else
            {
                Phone = phoneJsonValue.GetString();
            }

            Name = jsonObject.GetNamedString(nameKey, "");
            Timezone = jsonObject.GetNamedNumber(timezoneKey, 0);
            Verified = jsonObject.GetNamedBoolean(verifiedKey, false);

            foreach (IJsonValue jsonValue in jsonObject.GetNamedArray(educationKey, new JsonArray()))
            {
                Education.Add(new School(jsonValue.GetObject()));
            }
        }

        public string Stringify()
        {
            JsonObject jsonObject = new JsonObject();
            jsonObject[idKey] = JsonValue.CreateStringValue(Id);

            // Treating a blank string as null
            if (String.IsNullOrEmpty(Phone))
            {
                jsonObject[phoneKey] = JsonValue.CreateNullValue();
            }
            else
            {
                jsonObject[phoneKey] = JsonValue.CreateStringValue(Phone);
            }

            jsonObject[nameKey] = JsonValue.CreateStringValue(Name);

            jsonObject[timezoneKey] = JsonValue.CreateNumberValue(Timezone);
            jsonObject[verifiedKey] = JsonValue.CreateBooleanValue(Verified);

            JsonArray jsonArray = new JsonArray();
            foreach (School school in Education)
            {
                jsonArray.Add(school.ToJsonObject());
            }
            jsonObject[educationKey] = jsonArray;

            return jsonObject.Stringify();
        }

        public string Id { get; set; } = "";

        public string Phone { get; set; } = null;

        public string Name { get; set; } = "";

        public ObservableCollection<School> Education { get; } = new ObservableCollection<School>();

        public double Timezone { get; set; } = 0.0;
        public bool Verified { get; set; } = false;
    }
}
