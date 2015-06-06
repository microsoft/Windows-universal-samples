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
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using Windows.Data.Json;

namespace Json
{
    class User
    {
        private const string idKey = "id";
        private const string phoneKey = "phone";
        private const string nameKey = "name";
        private const string educationKey = "education";
        private const string timezoneKey = "timezone";
        private const string verifiedKey = "verified";

        private string id;
        private string phone;
        private string name;
        private ObservableCollection<School> education;

        public User()
        {
            Id = "";
            Phone = null;
            Name = "";
            education = new ObservableCollection<School>();
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
                if (jsonValue.ValueType == JsonValueType.Object)
                {
                    Education.Add(new School(jsonValue.GetObject()));
                }
            }
        }

        public string Stringify()
        {
            JsonArray jsonArray = new JsonArray();
            foreach (School school in Education)
            {
                jsonArray.Add(school.ToJsonObject());
            }

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
            jsonObject[educationKey] = jsonArray;
            jsonObject[timezoneKey] = JsonValue.CreateNumberValue(Timezone);
            jsonObject[verifiedKey] = JsonValue.CreateBooleanValue(Verified);

            return jsonObject.Stringify();
        }

        public string Id {
            get
            {
                return id;
            }
            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("value");
                }
                id = value;
            }
        }

        public string Phone
        {
            get
            {
                return phone;
            }
            set
            {
                phone = value;
            }
        }

        public string Name
        {
            get
            {
                return name;
            }
            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("value");
                }
                name = value;
            }
        }

        public ObservableCollection<School> Education
        {
            get
            {
                return education;
            }
        }

        public double Timezone { get; set; }
        public bool Verified { get; set; }
    }
}
