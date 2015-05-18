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
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Data.Json;

namespace Json
{
    class School
    {
        private const string idKey = "id";
        private const string schoolKey = "school";
        private const string nameKey = "name";
        private const string typeKey = "type";

        private string id;
        private string name;
        private string type;

        public School()
        {
            Id = "";
            Name = "";
            Type = "";
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
            JsonObject schoolObject = new JsonObject();
            schoolObject.SetNamedValue(idKey, JsonValue.CreateStringValue(Id));
            schoolObject.SetNamedValue(nameKey, JsonValue.CreateStringValue(Name));

            JsonObject jsonObject = new JsonObject();
            jsonObject.SetNamedValue(schoolKey, schoolObject);
            jsonObject.SetNamedValue(typeKey, JsonValue.CreateStringValue(Type));

            return jsonObject;
        }

        public string Id
        {
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

        public string Type
        {
            get
            {
                return type;
            }
            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("value");
                }
                type = value;
            }
        }
    }
}
