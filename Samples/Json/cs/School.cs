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

using Windows.Data.Json;

namespace SDKTemplate
{
    class School
    {
        private const string idKey = "id";
        private const string schoolKey = "school";
        private const string nameKey = "name";
        private const string typeKey = "type";

        public School()
        {
        }

        public School(JsonObject jsonObject)
        {
            JsonObject schoolObject = jsonObject.GetNamedObject(schoolKey, null);
            if (schoolObject != null)
            {
                Id = schoolObject.GetNamedString(idKey, "");
                Name = schoolObject.GetNamedString(nameKey, "");
            }
            Type = jsonObject.GetNamedString(typeKey);
        }

        public JsonObject ToJsonObject()
        {
            // C# provides multiple ways to initialize a JsonObject.
            // Here, we use indexed collection initializer syntax.
            JsonObject schoolObject = new JsonObject()
            {
                [idKey] = JsonValue.CreateStringValue(Id),
                [nameKey] = JsonValue.CreateStringValue(Name),
            };

            JsonObject jsonObject = new JsonObject();
            // Here we use indexer syntax.
            jsonObject[schoolKey] = schoolObject;

            // Here we use an explicit call to SetNamedValue.
            jsonObject.SetNamedValue(typeKey, JsonValue.CreateStringValue(Type));

            return jsonObject;
        }

        public string Id { get; set; } = "";

        public string Name { get; set; } = "";

        public string Type { get; set; } = "";
    }
}
