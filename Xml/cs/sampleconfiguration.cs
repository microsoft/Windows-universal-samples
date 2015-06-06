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
using System.Threading.Tasks;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Xml;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Xml";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Build New RSS", ClassType = typeof(Xml.BuildNewRss) },
            new Scenario() { Title = "DOM Load/Save", ClassType = typeof(Xml.MarkHotProducts) },
            new Scenario() { Title = "Set Load Settings", ClassType = typeof(Xml.XmlLoading) },
            new Scenario() { Title = "XPath Query", ClassType = typeof(Xml.GiftDispatch) },
            new Scenario() { Title = "XSLT Transformation", ClassType = typeof(Xml.XSLTTransform) }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }


        public override string ToString()
        {
            return Title;
        }

        public static async Task<Windows.Data.Xml.Dom.XmlDocument> LoadXmlFile(String folder, String file)
        {
            Windows.Storage.StorageFolder storageFolder = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFolderAsync(folder);
            Windows.Storage.StorageFile storageFile = await storageFolder.GetFileAsync(file);
            Windows.Data.Xml.Dom.XmlLoadSettings loadSettings = new Windows.Data.Xml.Dom.XmlLoadSettings();
            loadSettings.ProhibitDtd = false;
            loadSettings.ResolveExternals = false;
            return await Windows.Data.Xml.Dom.XmlDocument.LoadFromFileAsync(storageFile, loadSettings);
        }

        public static void RichEditBoxSetText(RichEditBox richEditBox, String msg, Windows.UI.Color color, bool fReadOnly)
        {
            richEditBox.IsReadOnly = false;
            richEditBox.Document.SetText(Windows.UI.Text.TextSetOptions.None, msg);
            richEditBox.Foreground = new SolidColorBrush(color);
            richEditBox.IsReadOnly = fReadOnly;
        }

        public static void RichEditBoxSetError(RichEditBox richEditBox, String errorMsg)
        {
            RichEditBoxSetText(richEditBox, errorMsg, Windows.UI.Colors.Red, true);
        }

        public static void RichEditBoxSetMsg(RichEditBox richEditBox, String msg, bool fReadOnly)
        {
            RichEditBoxSetText(richEditBox, msg, Windows.UI.Colors.White, fReadOnly);
        }
    }
}
