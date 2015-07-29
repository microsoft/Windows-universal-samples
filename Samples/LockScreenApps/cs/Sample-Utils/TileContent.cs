// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

using System;
using System.Text;
#if !WINRT_NOT_PRESENT
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;
#endif

namespace NotificationsExtensions.TileContent
{
    internal static class TileUtil
    {
        public const int NOTIFICATION_CONTENT_VERSION = 2;
    }

    internal abstract class TileNotificationBase : NotificationBase
    {
        public TileNotificationBase(string templateName, string fallbackName, int imageCount, int textCount)
            : base(templateName, fallbackName, imageCount, textCount)
        {
        }

        public TileBranding Branding
        {
            get { return m_Branding; }
            set
            {
                if (!Enum.IsDefined(typeof(TileBranding), value))
                {
                    throw new ArgumentOutOfRangeException("value");
                }
                m_Branding = value;
            }
        }

        public string ContentId
        {
            get { return m_ContentId; }
            set { m_ContentId = value; }
        }

#if !WINRT_NOT_PRESENT
        public TileNotification CreateNotification()
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.LoadXml(GetContent());
            return new TileNotification(xmlDoc);
        }
#endif

        private TileBranding m_Branding = TileBranding.Logo;
        private string m_ContentId = null;
    }

    internal interface ISquare150x150TileInternal
    {
        string SerializeBinding(string globalLang, string globalBaseUri, TileBranding globalBranding, bool globalAddImageQuery);
    }

    internal class TileSquare150x150Base : TileNotificationBase, ISquare150x150TileInternal
    {
        public TileSquare150x150Base(string templateName, string fallbackName, int imageCount, int textCount) : base(templateName, fallbackName, imageCount, textCount)
        {
        }               

        public override string GetContent()
        {
            StringBuilder builder = new StringBuilder(String.Empty);
            builder.AppendFormat("<tile><visual version='{0}'", TileUtil.NOTIFICATION_CONTENT_VERSION);
            if (!String.IsNullOrWhiteSpace(Lang))
            {
                builder.AppendFormat(" lang='{0}'", Util.HttpEncode(Lang));
            }
            if (Branding != TileBranding.Logo)
            {
                builder.AppendFormat(" branding='{0}'", Branding.ToString().ToLowerInvariant());
            }
            if (!String.IsNullOrWhiteSpace(BaseUri))
            {
                builder.AppendFormat(" baseUri='{0}'", Util.HttpEncode(BaseUri));
            }
            if (AddImageQuery)
            {
                builder.AppendFormat(" addImageQuery='true'");
            }
            builder.Append(">");
            builder.Append(SerializeBinding(Lang, BaseUri, Branding, AddImageQuery));
            builder.Append("</visual></tile>");
            return builder.ToString();
        }

        public string SerializeBinding(string globalLang, string globalBaseUri, TileBranding globalBranding, bool globalAddImageQuery)
        {
            StringBuilder bindingNode = new StringBuilder(String.Empty);
            bindingNode.AppendFormat("<binding template='{0}'", TemplateName);
            if (!String.IsNullOrWhiteSpace(FallbackName))
            {
                bindingNode.AppendFormat(" fallback='{0}'", FallbackName);
            }
            if (!String.IsNullOrWhiteSpace(Lang) && !Lang.Equals(globalLang))
            {
                bindingNode.AppendFormat(" lang='{0}'", Util.HttpEncode(Lang));
                globalLang = Lang;
            }
            if (Branding != TileBranding.Logo && Branding != globalBranding)
            {
                bindingNode.AppendFormat(" branding='{0}'", Branding.ToString().ToLowerInvariant());
            }
            if (!String.IsNullOrWhiteSpace(BaseUri) && !BaseUri.Equals(globalBaseUri))
            {
                bindingNode.AppendFormat(" baseUri='{0}'", Util.HttpEncode(BaseUri));
                globalBaseUri = BaseUri;
            }
            if (AddImageQueryNullable != null && AddImageQueryNullable != globalAddImageQuery)
            {
                bindingNode.AppendFormat(" addImageQuery='{0}'", AddImageQuery.ToString().ToLowerInvariant());
                globalAddImageQuery = AddImageQuery;
            }
            if (!String.IsNullOrWhiteSpace(ContentId))
            {
                bindingNode.AppendFormat(" contentId='{0}'", ContentId.ToLowerInvariant());
            }
            bindingNode.AppendFormat(">{0}</binding>", SerializeProperties(globalLang, globalBaseUri, globalAddImageQuery));

            return bindingNode.ToString();
        }
    }

    internal interface IWide310x150TileInternal
    {
        string SerializeBindings(string globalLang, string globalBaseUri, TileBranding globalBranding, bool globalAddImageQuery);
    }

    internal class TileWide310x150Base : TileNotificationBase, IWide310x150TileInternal
    {
        public TileWide310x150Base(string templateName, string fallbackName, int imageCount, int textCount) : base(templateName, fallbackName, imageCount, textCount)
        {
        }

        public ISquare150x150TileNotificationContent Square150x150Content
        {
            get { return m_Square150x150Content; }
            set { m_Square150x150Content = value; }
        }

        public bool RequireSquare150x150Content
        {
            get { return m_RequireSquare150x150Content; }
            set { m_RequireSquare150x150Content = value; }
        }

        public override string GetContent()
        {
            if (RequireSquare150x150Content && Square150x150Content == null)
            {
                throw new NotificationContentValidationException(
                    "Square150x150 tile content should be included with each wide tile. " +
                    "If this behavior is undesired, use the RequireSquare150x150Content property.");
            }

            StringBuilder visualNode = new StringBuilder(String.Empty);
            visualNode.AppendFormat("<visual version='{0}'", TileUtil.NOTIFICATION_CONTENT_VERSION);
            if (!String.IsNullOrWhiteSpace(Lang))
            {
                visualNode.AppendFormat(" lang='{0}'", Util.HttpEncode(Lang));
            }
            if (Branding != TileBranding.Logo)
            {
                visualNode.AppendFormat(" branding='{0}'", Branding.ToString().ToLowerInvariant());
            }
            if (!String.IsNullOrWhiteSpace(BaseUri))
            {
                visualNode.AppendFormat(" baseUri='{0}'", Util.HttpEncode(BaseUri));
            }
            if (AddImageQuery)
            {
                visualNode.AppendFormat(" addImageQuery='true'");
            }
            visualNode.Append(">");

            StringBuilder builder = new StringBuilder(String.Empty);
            builder.AppendFormat("<tile>{0}", visualNode);
            if (Square150x150Content != null)
            {
                ISquare150x150TileInternal squareBase = Square150x150Content as ISquare150x150TileInternal;
                if (squareBase == null)
                {
                    throw new NotificationContentValidationException("The provided square tile content class is unsupported.");
                }
                builder.Append(squareBase.SerializeBinding(Lang, BaseUri, Branding, AddImageQuery));
            }
            builder.AppendFormat("<binding template='{0}'", TemplateName);
            if (!String.IsNullOrWhiteSpace(FallbackName))
            {
                builder.AppendFormat(" fallback='{0}'", FallbackName);
            }
            builder.AppendFormat(">{0}</binding></visual></tile>", SerializeProperties(Lang, BaseUri, AddImageQuery));
            return builder.ToString();
        }

        public string SerializeBindings(string globalLang, string globalBaseUri, TileBranding globalBranding, bool globalAddImageQuery)
        {
            StringBuilder bindingNode = new StringBuilder(String.Empty);
            if (Square150x150Content != null)
            {
                ISquare150x150TileInternal squareBase = Square150x150Content as ISquare150x150TileInternal;
                if (squareBase == null)
                {
                    throw new NotificationContentValidationException("The provided square tile content class is unsupported.");
                }
                bindingNode.Append(squareBase.SerializeBinding(Lang, BaseUri, Branding, AddImageQuery));
            }

            bindingNode.AppendFormat("<binding template='{0}'", TemplateName);
            if (!String.IsNullOrWhiteSpace(FallbackName))
            {
                bindingNode.AppendFormat(" fallback='{0}'", FallbackName);
            }
            if (!String.IsNullOrWhiteSpace(Lang) && !Lang.Equals(globalLang))
            {
                bindingNode.AppendFormat(" lang='{0}'", Util.HttpEncode(Lang));
                globalLang = Lang;
            }
            if (Branding != TileBranding.Logo && Branding != globalBranding)
            {
                bindingNode.AppendFormat(" branding='{0}'", Branding.ToString().ToLowerInvariant());
            }
            if (!String.IsNullOrWhiteSpace(BaseUri) && !BaseUri.Equals(globalBaseUri))
            {
                bindingNode.AppendFormat(" baseUri='{0}'", Util.HttpEncode(BaseUri));
                globalBaseUri = BaseUri;
            }
            if (AddImageQueryNullable != null && AddImageQueryNullable != globalAddImageQuery)
            {
                bindingNode.AppendFormat(" addImageQuery='{0}'", AddImageQuery.ToString().ToLowerInvariant());
                globalAddImageQuery = AddImageQuery;
            }
            if (!String.IsNullOrWhiteSpace(ContentId))
            {
                bindingNode.AppendFormat(" contentId='{0}'", ContentId.ToLowerInvariant());
            }
            bindingNode.AppendFormat(">{0}</binding>", SerializeProperties(globalLang, globalBaseUri, globalAddImageQuery));

            return bindingNode.ToString();
        }

        private ISquare150x150TileNotificationContent m_Square150x150Content = null;
        private bool m_RequireSquare150x150Content = true;
    }

    internal class TileSquare310x310Base : TileNotificationBase
    {
        public TileSquare310x310Base(string templateName, string fallbackName, int imageCount, int textCount)
            : base(templateName, null, imageCount, textCount)
        {
        }

        public IWide310x150TileNotificationContent Wide310x150Content
        {
            get { return m_Wide310x150Content; }
            set { m_Wide310x150Content = value; }
        }

        public bool RequireWide310x150Content
        {
            get { return m_RequireWide310x150Content; }
            set { m_RequireWide310x150Content = value; }
        }

        public override string GetContent()
        {
            if (RequireWide310x150Content && Wide310x150Content == null)
            {
                throw new NotificationContentValidationException(
                    "Wide310x150 tile content should be included with each large tile. " +
                    "If this behavior is undesired, use the RequireWide310x150Content property.");
            }

            if (Wide310x150Content != null && Wide310x150Content.RequireSquare150x150Content && Wide310x150Content.Square150x150Content == null)
            {
                throw new NotificationContentValidationException(
                    "This tile's wide content requires square content. " +
                    "If this behavior is undesired, use the Wide310x150Content.RequireSquare150x150Content property.");
            }

            StringBuilder visualNode = new StringBuilder(String.Empty);
            visualNode.AppendFormat("<visual version='{0}'", TileUtil.NOTIFICATION_CONTENT_VERSION);
            if (!String.IsNullOrWhiteSpace(Lang))
            {
                visualNode.AppendFormat(" lang='{0}'", Util.HttpEncode(Lang));
            }
            if (Branding != TileBranding.Logo)
            {
                visualNode.AppendFormat(" branding='{0}'", Branding.ToString().ToLowerInvariant());
            }
            if (!String.IsNullOrWhiteSpace(BaseUri))
            {
                visualNode.AppendFormat(" baseUri='{0}'", Util.HttpEncode(BaseUri));
            }
            if (AddImageQuery)
            {
                visualNode.AppendFormat(" addImageQuery='true'");
            }
            visualNode.Append(">");

            StringBuilder builder = new StringBuilder(String.Empty);
            builder.AppendFormat("<tile>{0}", visualNode);
            if (Wide310x150Content != null)
            {
                IWide310x150TileInternal wideBase = Wide310x150Content as IWide310x150TileInternal;
                if (wideBase == null)
                {
                    throw new NotificationContentValidationException("The provided wide tile content class is unsupported.");
                }
                builder.Append(wideBase.SerializeBindings(Lang, BaseUri, Branding, AddImageQuery));
            }
            builder.AppendFormat("<binding template='{0}'", TemplateName);
            if (!String.IsNullOrWhiteSpace(FallbackName))
            {
                builder.AppendFormat(" fallback='{0}'", FallbackName);
            }
            if (!String.IsNullOrWhiteSpace(ContentId))
            {
                builder.AppendFormat(" contentId='{0}'", ContentId.ToLowerInvariant());
            }
            builder.AppendFormat(">{0}</binding></visual></tile>", SerializeProperties(Lang, BaseUri, AddImageQuery));

            return builder.ToString();
        }

        private IWide310x150TileNotificationContent m_Wide310x150Content = null;
        private bool m_RequireWide310x150Content = true;
    }

    internal interface ISquare99x99TileInternal
    {
        string SerializeBinding(string globalLang, string globalBaseUri, TileBranding globalBranding, bool globalAddImageQuery);
    }

    internal class TileSquare99x99Base : TileNotificationBase, ISquare99x99TileInternal
    {
        public TileSquare99x99Base(string templateName, string fallbackName, int imageCount, int textCount)
            : base(templateName, fallbackName, imageCount, textCount)
        {
        }

        public override string GetContent()
        {
            StringBuilder builder = new StringBuilder(String.Empty);
            builder.AppendFormat("<tile><visual version='{0}'", TileUtil.NOTIFICATION_CONTENT_VERSION);
            if (!String.IsNullOrWhiteSpace(Lang))
            {
                builder.AppendFormat(" lang='{0}'", Util.HttpEncode(Lang));
            }
            if (Branding != TileBranding.Logo)
            {
                builder.AppendFormat(" branding='{0}'", Branding.ToString().ToLowerInvariant());
            }
            if (!String.IsNullOrWhiteSpace(BaseUri))
            {
                builder.AppendFormat(" baseUri='{0}'", Util.HttpEncode(BaseUri));
            }
            if (AddImageQuery)
            {
                builder.AppendFormat(" addImageQuery='true'");
            }
            builder.Append(">");
            builder.Append(SerializeBinding(Lang, BaseUri, Branding, AddImageQuery));
            builder.Append("</visual></tile>");
            return builder.ToString();
        }

        public string SerializeBinding(string globalLang, string globalBaseUri, TileBranding globalBranding, bool globalAddImageQuery)
        {
            StringBuilder bindingNode = new StringBuilder(String.Empty);
            bindingNode.AppendFormat("<binding template='{0}'", TemplateName);
            if (!String.IsNullOrWhiteSpace(FallbackName))
            {
                bindingNode.AppendFormat(" fallback='{0}'", FallbackName);
            }
            if (!String.IsNullOrWhiteSpace(Lang) && !Lang.Equals(globalLang))
            {
                bindingNode.AppendFormat(" lang='{0}'", Util.HttpEncode(Lang));
                globalLang = Lang;
            }
            if (Branding != TileBranding.Logo && Branding != globalBranding)
            {
                bindingNode.AppendFormat(" branding='{0}'", Branding.ToString().ToLowerInvariant());
            }
            if (!String.IsNullOrWhiteSpace(BaseUri) && !BaseUri.Equals(globalBaseUri))
            {
                bindingNode.AppendFormat(" baseUri='{0}'", Util.HttpEncode(BaseUri));
                globalBaseUri = BaseUri;
            }
            if (AddImageQueryNullable != null && AddImageQueryNullable != globalAddImageQuery)
            {
                bindingNode.AppendFormat(" addImageQuery='{0}'", AddImageQuery.ToString().ToLowerInvariant());
                globalAddImageQuery = AddImageQuery;
            }
            bindingNode.AppendFormat(">{0}</binding>", SerializeProperties(globalLang, globalBaseUri, globalAddImageQuery));

            return bindingNode.ToString();
        }
    }

    internal interface ISquare210x210TileInternal
    {
        string SerializeBindings(string globalLang, string globalBaseUri, TileBranding globalBranding, bool globalAddImageQuery);
    }

    internal class TileSquare210x210Base : TileNotificationBase, ISquare210x210TileInternal
    {
        public TileSquare210x210Base(string templateName, string fallbackName, int imageCount, int textCount)
            : base(templateName, fallbackName, imageCount, textCount)
        {
        }

        public ISquare99x99TileNotificationContent Square99x99Content
        {
            get { return m_Square99x99Content; }
            set { m_Square99x99Content = value; }
        }

        public bool RequireSquare99x99Content
        {
            get { return m_RequireSquare99x99Content; }
            set { m_RequireSquare99x99Content = value; }
        }

        public override string GetContent()
        {
            if (RequireSquare99x99Content && Square99x99Content == null)
            {
                throw new NotificationContentValidationException(
                    "Square99x99 tile content should be included with each medium tile. " +
                    "If this behavior is undesired, use the RequireSquare99x99Content property.");
            }

            StringBuilder visualNode = new StringBuilder(String.Empty);
            visualNode.AppendFormat("<visual version='{0}'", TileUtil.NOTIFICATION_CONTENT_VERSION);
            if (!String.IsNullOrWhiteSpace(Lang))
            {
                visualNode.AppendFormat(" lang='{0}'", Util.HttpEncode(Lang));
            }
            if (Branding != TileBranding.Logo)
            {
                visualNode.AppendFormat(" branding='{0}'", Branding.ToString().ToLowerInvariant());
            }
            if (!String.IsNullOrWhiteSpace(BaseUri))
            {
                visualNode.AppendFormat(" baseUri='{0}'", Util.HttpEncode(BaseUri));
            }
            if (AddImageQuery)
            {
                visualNode.AppendFormat(" addImageQuery='true'");
            }
            visualNode.Append(">");

            StringBuilder builder = new StringBuilder(String.Empty);
            builder.AppendFormat("<tile>{0}", visualNode);
            if (Square99x99Content != null)
            {
                ISquare99x99TileInternal smallTileBase = Square99x99Content as ISquare99x99TileInternal;
                if (smallTileBase == null)
                {
                    throw new NotificationContentValidationException("The provided small tile content class is unsupported.");
                }
                builder.Append(smallTileBase.SerializeBinding(Lang, BaseUri, Branding, AddImageQuery));
            }
            builder.AppendFormat("<binding template='{0}'", TemplateName);
            if (!String.IsNullOrWhiteSpace(FallbackName))
            {
                builder.AppendFormat(" fallback='{0}'", FallbackName);
            }
            builder.AppendFormat(">{0}</binding></visual></tile>", SerializeProperties(Lang, BaseUri, AddImageQuery));
            return builder.ToString();
        }

        public string SerializeBindings(string globalLang, string globalBaseUri, TileBranding globalBranding, bool globalAddImageQuery)
        {
            StringBuilder bindingNode = new StringBuilder(String.Empty);
            if (Square99x99Content != null)
            {
                ISquare99x99TileInternal smallTileBase = Square99x99Content as ISquare99x99TileInternal;
                if (smallTileBase == null)
                {
                    throw new NotificationContentValidationException("The provided small tile content class is unsupported.");
                }
                bindingNode.Append(smallTileBase.SerializeBinding(Lang, BaseUri, Branding, AddImageQuery));
            }

            bindingNode.AppendFormat("<binding template='{0}'", TemplateName);
            if (!String.IsNullOrWhiteSpace(FallbackName))
            {
                bindingNode.AppendFormat(" fallback='{0}'", FallbackName);
            }
            if (!String.IsNullOrWhiteSpace(Lang) && !Lang.Equals(globalLang))
            {
                bindingNode.AppendFormat(" lang='{0}'", Util.HttpEncode(Lang));
                globalLang = Lang;
            }
            if (Branding != TileBranding.Logo && Branding != globalBranding)
            {
                bindingNode.AppendFormat(" branding='{0}'", Branding.ToString().ToLowerInvariant());
            }
            if (!String.IsNullOrWhiteSpace(BaseUri) && !BaseUri.Equals(globalBaseUri))
            {
                bindingNode.AppendFormat(" baseUri='{0}'", Util.HttpEncode(BaseUri));
                globalBaseUri = BaseUri;
            }
            if (AddImageQueryNullable != null && AddImageQueryNullable != globalAddImageQuery)
            {
                bindingNode.AppendFormat(" addImageQuery='{0}'", AddImageQuery.ToString().ToLowerInvariant());
                globalAddImageQuery = AddImageQuery;
            }
            bindingNode.AppendFormat(">{0}</binding>", SerializeProperties(globalLang, globalBaseUri, globalAddImageQuery));

            return bindingNode.ToString();
        }

        private ISquare99x99TileNotificationContent m_Square99x99Content = null;
        private bool m_RequireSquare99x99Content = true;
    }

    internal class TileWide432x210Base : TileNotificationBase
    {
        public TileWide432x210Base(string templateName, string fallbackName, int imageCount, int textCount)
            : base(templateName, null, imageCount, textCount)
        {
        }

        public ISquare210x210TileNotificationContent Square210x210Content
        {
            get { return m_Square210x210Content; }
            set { m_Square210x210Content = value; }
        }

        public bool RequireSquare210x210Content
        {
            get { return m_RequireSquare210x210Content; }
            set { m_RequireSquare210x210Content = value; }
        }

        public override string GetContent()
        {
            if (RequireSquare210x210Content && Square210x210Content == null)
            {
                throw new NotificationContentValidationException(
                    "Square210x210 tile content should be included with each large tile. " +
                    "If this behavior is undesired, use the RequireSquare210x210Content property.");
            }

            if (Square210x210Content != null && Square210x210Content.RequireSquare99x99Content && Square210x210Content.Square99x99Content == null)
            {
                throw new NotificationContentValidationException(
                    "This tile's medium tile content requires small tile content. " +
                    "If this behavior is undesired, use the Square210x210Content.RequireSquare99x99Content property.");
            }

            StringBuilder visualNode = new StringBuilder(String.Empty);
            visualNode.AppendFormat("<visual version='{0}'", TileUtil.NOTIFICATION_CONTENT_VERSION);
            if (!String.IsNullOrWhiteSpace(Lang))
            {
                visualNode.AppendFormat(" lang='{0}'", Util.HttpEncode(Lang));
            }
            if (Branding != TileBranding.Logo)
            {
                visualNode.AppendFormat(" branding='{0}'", Branding.ToString().ToLowerInvariant());
            }
            if (!String.IsNullOrWhiteSpace(BaseUri))
            {
                visualNode.AppendFormat(" baseUri='{0}'", Util.HttpEncode(BaseUri));
            }
            if (AddImageQuery)
            {
                visualNode.AppendFormat(" addImageQuery='true'");
            }
            visualNode.Append(">");

            StringBuilder builder = new StringBuilder(String.Empty);
            builder.AppendFormat("<tile>{0}", visualNode);
            if (Square210x210Content != null)
            {
                ISquare210x210TileInternal mediumTileBase = Square210x210Content as ISquare210x210TileInternal;
                if (mediumTileBase == null)
                {
                    throw new NotificationContentValidationException("The provided medium tile content class is unsupported.");
                }
                builder.Append(mediumTileBase.SerializeBindings(Lang, BaseUri, Branding, AddImageQuery));
            }
            builder.AppendFormat("<binding template='{0}'", TemplateName);
            if (!String.IsNullOrWhiteSpace(FallbackName))
            {
                builder.AppendFormat(" fallback='{0}'", FallbackName);
            }
            builder.AppendFormat(">{0}</binding></visual></tile>", SerializeProperties(Lang, BaseUri, AddImageQuery));

            return builder.ToString();
        }

        private ISquare210x210TileNotificationContent m_Square210x210Content = null;
        private bool m_RequireSquare210x210Content = true;
    }

    internal class TileSquare150x150Block : TileSquare150x150Base, ITileSquare150x150Block
    {
        public TileSquare150x150Block() : base(templateName: "TileSquare150x150Block",  fallbackName: "TileSquareBlock", imageCount: 0, textCount: 2)
        {
        }

        public INotificationContentText TextBlock { get { return TextFields[0]; } }
        public INotificationContentText TextSubBlock { get { return TextFields[1]; } }
    }

    internal class TileSquare150x150Image : TileSquare150x150Base, ITileSquare150x150Image
    {
        public TileSquare150x150Image() : base(templateName: "TileSquare150x150Image",  fallbackName: "TileSquareImage", imageCount: 1, textCount: 0)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }
    }

    internal class TileSquare150x150PeekImageAndText01 : TileSquare150x150Base, ITileSquare150x150PeekImageAndText01
    {
        public TileSquare150x150PeekImageAndText01() : base(templateName: "TileSquare150x150PeekImageAndText01",  fallbackName: "TileSquarePeekImageAndText01", imageCount: 1, textCount: 4)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }
        
        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBody1 { get { return TextFields[1]; } }
        public INotificationContentText TextBody2 { get { return TextFields[2]; } }
        public INotificationContentText TextBody3 { get { return TextFields[3]; } }
    }

    internal class TileSquare150x150PeekImageAndText02 : TileSquare150x150Base, ITileSquare150x150PeekImageAndText02
    {
        public TileSquare150x150PeekImageAndText02() : base(templateName: "TileSquare150x150PeekImageAndText02",  fallbackName: "TileSquarePeekImageAndText02", imageCount: 1, textCount: 2)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBodyWrap { get { return TextFields[1]; } }
    }

    internal class TileSquare150x150PeekImageAndText03 : TileSquare150x150Base, ITileSquare150x150PeekImageAndText03
    {
        public TileSquare150x150PeekImageAndText03() : base(templateName: "TileSquare150x150PeekImageAndText03",  fallbackName: "TileSquarePeekImageAndText03", imageCount: 1, textCount: 4)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextBody1 { get { return TextFields[0]; } }
        public INotificationContentText TextBody2 { get { return TextFields[1]; } }
        public INotificationContentText TextBody3 { get { return TextFields[2]; } }
        public INotificationContentText TextBody4 { get { return TextFields[3]; } }
    }

    internal class TileSquare150x150PeekImageAndText04 : TileSquare150x150Base, ITileSquare150x150PeekImageAndText04
    {
        public TileSquare150x150PeekImageAndText04() : base(templateName: "TileSquare150x150PeekImageAndText04",  fallbackName: "TileSquarePeekImageAndText04", imageCount: 1, textCount: 1)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextBodyWrap { get { return TextFields[0]; } }
    }

    internal class TileSquare150x150Text01 : TileSquare150x150Base, ITileSquare150x150Text01
    {
        public TileSquare150x150Text01() : base(templateName: "TileSquare150x150Text01",  fallbackName: "TileSquareText01", imageCount: 0, textCount: 4)
        {
        }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBody1 { get { return TextFields[1]; } }
        public INotificationContentText TextBody2 { get { return TextFields[2]; } }
        public INotificationContentText TextBody3 { get { return TextFields[3]; } }
    }

    internal class TileSquare150x150Text02 : TileSquare150x150Base, ITileSquare150x150Text02
    {
        public TileSquare150x150Text02() : base(templateName: "TileSquare150x150Text02",  fallbackName: "TileSquareText02", imageCount: 0, textCount: 2)
        {
        }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBodyWrap { get { return TextFields[1]; } }
    }

    internal class TileSquare150x150Text03 : TileSquare150x150Base, ITileSquare150x150Text03
    {
        public TileSquare150x150Text03() : base(templateName: "TileSquare150x150Text03",  fallbackName: "TileSquareText03", imageCount: 0, textCount: 4) 
        {
        }

        public INotificationContentText TextBody1 { get { return TextFields[0]; } }
        public INotificationContentText TextBody2 { get { return TextFields[1]; } }
        public INotificationContentText TextBody3 { get { return TextFields[2]; } }
        public INotificationContentText TextBody4 { get { return TextFields[3]; } }
    }

    internal class TileSquare150x150Text04 : TileSquare150x150Base, ITileSquare150x150Text04
    {
        public TileSquare150x150Text04() : base(templateName: "TileSquare150x150Text04",  fallbackName: "TileSquareText04", imageCount: 0, textCount: 1)
        {
        }

        public INotificationContentText TextBodyWrap { get { return TextFields[0]; } }
    }

    internal class TileWide310x150BlockAndText01 : TileWide310x150Base, ITileWide310x150BlockAndText01
    {
        public TileWide310x150BlockAndText01() : base(templateName: "TileWide310x150BlockAndText01",  fallbackName: "TileWideBlockAndText01", imageCount: 0, textCount: 6)
        {
        }

        public INotificationContentText TextBody1 { get { return TextFields[0]; } }
        public INotificationContentText TextBody2 { get { return TextFields[1]; } }
        public INotificationContentText TextBody3 { get { return TextFields[2]; } }
        public INotificationContentText TextBody4 { get { return TextFields[3]; } }
        public INotificationContentText TextBlock { get { return TextFields[4]; } }
        public INotificationContentText TextSubBlock { get { return TextFields[5]; } }
    }

    internal class TileWide310x150BlockAndText02 : TileWide310x150Base, ITileWide310x150BlockAndText02
    {
        public TileWide310x150BlockAndText02() : base(templateName: "TileWide310x150BlockAndText02",  fallbackName: "TileWideBlockAndText02", imageCount: 0, textCount: 6)
        {
        }

        public INotificationContentText TextBodyWrap { get { return TextFields[0]; } }
        public INotificationContentText TextBlock { get { return TextFields[1]; } }
        public INotificationContentText TextSubBlock { get { return TextFields[2]; } }
    }

    internal class TileWide310x150Image : TileWide310x150Base, ITileWide310x150Image
    {
        public TileWide310x150Image() : base(templateName: "TileWide310x150Image",  fallbackName: "TileWideImage", imageCount: 1, textCount: 0)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }
    }

    internal class TileWide310x150ImageAndText01 : TileWide310x150Base, ITileWide310x150ImageAndText01
    {
        public TileWide310x150ImageAndText01() : base(templateName: "TileWide310x150ImageAndText01",  fallbackName: "TileWideImageAndText01", imageCount: 1, textCount: 1)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextCaptionWrap { get { return TextFields[0]; } }
    }

    internal class TileWide310x150ImageAndText02 : TileWide310x150Base, ITileWide310x150ImageAndText02
    {
        public TileWide310x150ImageAndText02() : base(templateName: "TileWide310x150ImageAndText02",  fallbackName: "TileWideImageAndText02", imageCount: 1, textCount: 2)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextCaption1 { get { return TextFields[0]; } }
        public INotificationContentText TextCaption2 { get { return TextFields[1]; } }
    }

    internal class TileWide310x150ImageCollection : TileWide310x150Base, ITileWide310x150ImageCollection
    {
        public TileWide310x150ImageCollection() : base(templateName: "TileWide310x150ImageCollection",  fallbackName: "TileWideImageCollection", imageCount: 5, textCount: 0)
        {
        }

        public INotificationContentImage ImageMain { get { return Images[0]; } }
        public INotificationContentImage ImageSmallColumn1Row1 { get { return Images[1]; } }
        public INotificationContentImage ImageSmallColumn2Row1 { get { return Images[2]; } }
        public INotificationContentImage ImageSmallColumn1Row2 { get { return Images[3]; } }
        public INotificationContentImage ImageSmallColumn2Row2 { get { return Images[4]; } }
    }

    internal class TileWide310x150PeekImage01 : TileWide310x150Base, ITileWide310x150PeekImage01
    {
        public TileWide310x150PeekImage01() : base(templateName: "TileWide310x150PeekImage01",  fallbackName: "TileWidePeekImage01", imageCount: 1, textCount: 2)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBodyWrap { get { return TextFields[1]; } }
    }

    internal class TileWide310x150PeekImage02 : TileWide310x150Base, ITileWide310x150PeekImage02
    {
        public TileWide310x150PeekImage02() : base(templateName: "TileWide310x150PeekImage02",  fallbackName: "TileWidePeekImage02", imageCount: 1, textCount: 5)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBody1 { get { return TextFields[1]; } }
        public INotificationContentText TextBody2 { get { return TextFields[2]; } }
        public INotificationContentText TextBody3 { get { return TextFields[3]; } }
        public INotificationContentText TextBody4 { get { return TextFields[4]; } }
    }

    internal class TileWide310x150PeekImage03 : TileWide310x150Base, ITileWide310x150PeekImage03
    {
        public TileWide310x150PeekImage03() : base(templateName: "TileWide310x150PeekImage03",  fallbackName: "TileWidePeekImage03", imageCount: 1, textCount: 1)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextHeadingWrap { get { return TextFields[0]; } }
    }

    internal class TileWide310x150PeekImage04 : TileWide310x150Base, ITileWide310x150PeekImage04
    {
        public TileWide310x150PeekImage04() : base(templateName: "TileWide310x150PeekImage04",  fallbackName: "TileWidePeekImage04", imageCount: 1, textCount: 1)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextBodyWrap { get { return TextFields[0]; } }
    }

    internal class TileWide310x150PeekImage05 : TileWide310x150Base, ITileWide310x150PeekImage05
    {
        public TileWide310x150PeekImage05() : base(templateName: "TileWide310x150PeekImage05",  fallbackName: "TileWidePeekImage05", imageCount: 2, textCount: 2)
        {
        }

        public INotificationContentImage ImageMain { get { return Images[0]; } }
        public INotificationContentImage ImageSecondary { get { return Images[1]; } }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBodyWrap { get { return TextFields[1]; } }
    }

    internal class TileWide310x150PeekImage06 : TileWide310x150Base, ITileWide310x150PeekImage06
    {
        public TileWide310x150PeekImage06() : base(templateName: "TileWide310x150PeekImage06",  fallbackName: "TileWidePeekImage06", imageCount: 2, textCount: 1)
        {
        }

        public INotificationContentImage ImageMain { get { return Images[0]; } }
        public INotificationContentImage ImageSecondary { get { return Images[1]; } }

        public INotificationContentText TextHeadingWrap { get { return TextFields[0]; } }
    }

    internal class TileWide310x150PeekImageAndText01 : TileWide310x150Base, ITileWide310x150PeekImageAndText01
    {
        public TileWide310x150PeekImageAndText01() : base(templateName: "TileWide310x150PeekImageAndText01",  fallbackName: "TileWidePeekImageAndText01", imageCount: 1, textCount: 1) 
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextBodyWrap { get { return TextFields[0]; } }
    }

    internal class TileWide310x150PeekImageAndText02 : TileWide310x150Base, ITileWide310x150PeekImageAndText02
    {
        public TileWide310x150PeekImageAndText02() : base(templateName: "TileWide310x150PeekImageAndText02",  fallbackName: "TileWidePeekImageAndText02", imageCount: 1, textCount: 5) 
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextBody1 { get { return TextFields[0]; } }
        public INotificationContentText TextBody2 { get { return TextFields[1]; } }
        public INotificationContentText TextBody3 { get { return TextFields[2]; } }
        public INotificationContentText TextBody4 { get { return TextFields[3]; } }
        public INotificationContentText TextBody5 { get { return TextFields[4]; } }
    }

    internal class TileWide310x150PeekImageCollection01 : TileWide310x150Base, ITileWide310x150PeekImageCollection01
    {
        public TileWide310x150PeekImageCollection01() : base(templateName: "TileWide310x150PeekImageCollection01",  fallbackName: "TileWidePeekImageCollection01", imageCount: 5, textCount: 2)
        {
        }

        public INotificationContentImage ImageMain { get { return Images[0]; } }
        public INotificationContentImage ImageSmallColumn1Row1 { get { return Images[1]; } }
        public INotificationContentImage ImageSmallColumn2Row1 { get { return Images[2]; } }
        public INotificationContentImage ImageSmallColumn1Row2 { get { return Images[3]; } }
        public INotificationContentImage ImageSmallColumn2Row2 { get { return Images[4]; } }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBodyWrap { get { return TextFields[1]; } }
    }

    internal class TileWide310x150PeekImageCollection02 : TileWide310x150Base, ITileWide310x150PeekImageCollection02
    {
        public TileWide310x150PeekImageCollection02() : base(templateName: "TileWide310x150PeekImageCollection02",  fallbackName: "TileWidePeekImageCollection02", imageCount: 5, textCount: 5)
        {
        }

        public INotificationContentImage ImageMain { get { return Images[0]; } }
        public INotificationContentImage ImageSmallColumn1Row1 { get { return Images[1]; } }
        public INotificationContentImage ImageSmallColumn2Row1 { get { return Images[2]; } }
        public INotificationContentImage ImageSmallColumn1Row2 { get { return Images[3]; } }
        public INotificationContentImage ImageSmallColumn2Row2 { get { return Images[4]; } }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBody1 { get { return TextFields[1]; } }
        public INotificationContentText TextBody2 { get { return TextFields[2]; } }
        public INotificationContentText TextBody3 { get { return TextFields[3]; } }
        public INotificationContentText TextBody4 { get { return TextFields[4]; } }
    }

    internal class TileWide310x150PeekImageCollection03 : TileWide310x150Base, ITileWide310x150PeekImageCollection03
    {
        public TileWide310x150PeekImageCollection03() : base(templateName: "TileWide310x150PeekImageCollection03",  fallbackName: "TileWidePeekImageCollection03", imageCount: 5, textCount: 1)
        {
        }

        public INotificationContentImage ImageMain { get { return Images[0]; } }
        public INotificationContentImage ImageSmallColumn1Row1 { get { return Images[1]; } }
        public INotificationContentImage ImageSmallColumn2Row1 { get { return Images[2]; } }
        public INotificationContentImage ImageSmallColumn1Row2 { get { return Images[3]; } }
        public INotificationContentImage ImageSmallColumn2Row2 { get { return Images[4]; } }

        public INotificationContentText TextHeadingWrap { get { return TextFields[0]; } }
    }

    internal class TileWide310x150PeekImageCollection04 : TileWide310x150Base, ITileWide310x150PeekImageCollection04
    {
        public TileWide310x150PeekImageCollection04() : base(templateName: "TileWide310x150PeekImageCollection04",  fallbackName: "TileWidePeekImageCollection04", imageCount: 5, textCount: 1)
        {
        }

        public INotificationContentImage ImageMain { get { return Images[0]; } }
        public INotificationContentImage ImageSmallColumn1Row1 { get { return Images[1]; } }
        public INotificationContentImage ImageSmallColumn2Row1 { get { return Images[2]; } }
        public INotificationContentImage ImageSmallColumn1Row2 { get { return Images[3]; } }
        public INotificationContentImage ImageSmallColumn2Row2 { get { return Images[4]; } }

        public INotificationContentText TextBodyWrap { get { return TextFields[0]; } }
    }

    internal class TileWide310x150PeekImageCollection05 : TileWide310x150Base, ITileWide310x150PeekImageCollection05
    {
        public TileWide310x150PeekImageCollection05() : base(templateName: "TileWide310x150PeekImageCollection05",  fallbackName: "TileWidePeekImageCollection05", imageCount: 6, textCount: 2)
        {
        }

        public INotificationContentImage ImageMain { get { return Images[0]; } }
        public INotificationContentImage ImageSmallColumn1Row1 { get { return Images[1]; } }
        public INotificationContentImage ImageSmallColumn2Row1 { get { return Images[2]; } }
        public INotificationContentImage ImageSmallColumn1Row2 { get { return Images[3]; } }
        public INotificationContentImage ImageSmallColumn2Row2 { get { return Images[4]; } }
        public INotificationContentImage ImageSecondary { get { return Images[5]; } }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBodyWrap { get { return TextFields[1]; } }
    }

    internal class TileWide310x150PeekImageCollection06 : TileWide310x150Base, ITileWide310x150PeekImageCollection06
    {
        public TileWide310x150PeekImageCollection06() : base(templateName: "TileWide310x150PeekImageCollection06",  fallbackName: "TileWidePeekImageCollection06", imageCount: 6, textCount: 1)
        {
        }

        public INotificationContentImage ImageMain { get { return Images[0]; } }
        public INotificationContentImage ImageSmallColumn1Row1 { get { return Images[1]; } }
        public INotificationContentImage ImageSmallColumn2Row1 { get { return Images[2]; } }
        public INotificationContentImage ImageSmallColumn1Row2 { get { return Images[3]; } }
        public INotificationContentImage ImageSmallColumn2Row2 { get { return Images[4]; } }
        public INotificationContentImage ImageSecondary { get { return Images[5]; } }

        public INotificationContentText TextHeadingWrap { get { return TextFields[0]; } }
    }

    internal class TileWide310x150SmallImageAndText01 : TileWide310x150Base, ITileWide310x150SmallImageAndText01
    {
        public TileWide310x150SmallImageAndText01() : base(templateName: "TileWide310x150SmallImageAndText01",  fallbackName: "TileWideSmallImageAndText01", imageCount: 1, textCount: 1)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextHeadingWrap { get { return TextFields[0]; } }
    }
    
    internal class TileWide310x150SmallImageAndText02 : TileWide310x150Base, ITileWide310x150SmallImageAndText02
    {
        public TileWide310x150SmallImageAndText02() : base(templateName: "TileWide310x150SmallImageAndText02",  fallbackName: "TileWideSmallImageAndText02", imageCount: 1, textCount: 5)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBody1 { get { return TextFields[1]; } }
        public INotificationContentText TextBody2 { get { return TextFields[2]; } }
        public INotificationContentText TextBody3 { get { return TextFields[3]; } }
        public INotificationContentText TextBody4 { get { return TextFields[4]; } }
    }

    internal class TileWide310x150SmallImageAndText03 : TileWide310x150Base, ITileWide310x150SmallImageAndText03
    {
        public TileWide310x150SmallImageAndText03() : base(templateName: "TileWide310x150SmallImageAndText03",  fallbackName: "TileWideSmallImageAndText03", imageCount: 1, textCount: 1)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextBodyWrap { get { return TextFields[0]; } }
    }

    internal class TileWide310x150SmallImageAndText04 : TileWide310x150Base, ITileWide310x150SmallImageAndText04
    {
        public TileWide310x150SmallImageAndText04() : base(templateName: "TileWide310x150SmallImageAndText04",  fallbackName: "TileWideSmallImageAndText04", imageCount: 1, textCount: 2)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBodyWrap { get { return TextFields[1]; } }
    }

    internal class TileWide310x150SmallImageAndText05 : TileWide310x150Base, ITileWide310x150SmallImageAndText05
    {
        public TileWide310x150SmallImageAndText05() : base(templateName: "TileWide310x150SmallImageAndText05",  fallbackName: "TileWideSmallImageAndText05", imageCount: 1, textCount: 2)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBodyWrap { get { return TextFields[1]; } }
    }

    internal class TileWide310x150Text01 : TileWide310x150Base, ITileWide310x150Text01
    {
        public TileWide310x150Text01() : base(templateName: "TileWide310x150Text01",  fallbackName: "TileWideText01", imageCount: 0, textCount: 5)
        {
        }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBody1 { get { return TextFields[1]; } }
        public INotificationContentText TextBody2 { get { return TextFields[2]; } }
        public INotificationContentText TextBody3 { get { return TextFields[3]; } }
        public INotificationContentText TextBody4 { get { return TextFields[4]; } }
    }

    internal class TileWide310x150Text02 : TileWide310x150Base, ITileWide310x150Text02
    {
        public TileWide310x150Text02() : base(templateName: "TileWide310x150Text02",  fallbackName: "TileWideText02", imageCount: 0, textCount: 9)
        {
        }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextColumn1Row1 { get { return TextFields[1]; } }
        public INotificationContentText TextColumn2Row1 { get { return TextFields[2]; } }
        public INotificationContentText TextColumn1Row2 { get { return TextFields[3]; } }
        public INotificationContentText TextColumn2Row2 { get { return TextFields[4]; } }
        public INotificationContentText TextColumn1Row3 { get { return TextFields[5]; } }
        public INotificationContentText TextColumn2Row3 { get { return TextFields[6]; } }
        public INotificationContentText TextColumn1Row4 { get { return TextFields[7]; } }
        public INotificationContentText TextColumn2Row4 { get { return TextFields[8]; } }
    }

    internal class TileWide310x150Text03 : TileWide310x150Base, ITileWide310x150Text03
    {
        public TileWide310x150Text03() : base(templateName: "TileWide310x150Text03",  fallbackName: "TileWideText03", imageCount: 0, textCount: 1)
        {
        }

        public INotificationContentText TextHeadingWrap { get { return TextFields[0]; } }
    }

    internal class TileWide310x150Text04 : TileWide310x150Base, ITileWide310x150Text04
    {
        public TileWide310x150Text04() : base(templateName: "TileWide310x150Text04",  fallbackName: "TileWideText04", imageCount: 0, textCount: 1)
        {
        }

        public INotificationContentText TextBodyWrap { get { return TextFields[0]; } }
    }

    internal class TileWide310x150Text05 : TileWide310x150Base, ITileWide310x150Text05
    {
        public TileWide310x150Text05() : base(templateName: "TileWide310x150Text05",  fallbackName: "TileWideText05", imageCount: 0, textCount: 5)
        {
        }

        public INotificationContentText TextBody1 { get { return TextFields[0]; } }
        public INotificationContentText TextBody2 { get { return TextFields[1]; } }
        public INotificationContentText TextBody3 { get { return TextFields[2]; } }
        public INotificationContentText TextBody4 { get { return TextFields[3]; } }
        public INotificationContentText TextBody5 { get { return TextFields[4]; } }
    }

    internal class TileWide310x150Text06 : TileWide310x150Base, ITileWide310x150Text06
    {
        public TileWide310x150Text06() : base(templateName: "TileWide310x150Text06",  fallbackName: "TileWideText06", imageCount: 0, textCount: 10)
        {
        }

        public INotificationContentText TextColumn1Row1 { get { return TextFields[0]; } }
        public INotificationContentText TextColumn2Row1 { get { return TextFields[1]; } }
        public INotificationContentText TextColumn1Row2 { get { return TextFields[2]; } }
        public INotificationContentText TextColumn2Row2 { get { return TextFields[3]; } }
        public INotificationContentText TextColumn1Row3 { get { return TextFields[4]; } }
        public INotificationContentText TextColumn2Row3 { get { return TextFields[5]; } }
        public INotificationContentText TextColumn1Row4 { get { return TextFields[6]; } }
        public INotificationContentText TextColumn2Row4 { get { return TextFields[7]; } }
        public INotificationContentText TextColumn1Row5 { get { return TextFields[8]; } }
        public INotificationContentText TextColumn2Row5 { get { return TextFields[9]; } }
    }

    internal class TileWide310x150Text07 : TileWide310x150Base, ITileWide310x150Text07
    {
        public TileWide310x150Text07() : base(templateName: "TileWide310x150Text07",  fallbackName: "TileWideText07", imageCount: 0, textCount: 9)
        {
        }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextShortColumn1Row1 { get { return TextFields[1]; } }
        public INotificationContentText TextColumn2Row1 { get { return TextFields[2]; } }
        public INotificationContentText TextShortColumn1Row2 { get { return TextFields[3]; } }
        public INotificationContentText TextColumn2Row2 { get { return TextFields[4]; } }
        public INotificationContentText TextShortColumn1Row3 { get { return TextFields[5]; } }
        public INotificationContentText TextColumn2Row3 { get { return TextFields[6]; } }
        public INotificationContentText TextShortColumn1Row4 { get { return TextFields[7]; } }
        public INotificationContentText TextColumn2Row4 { get { return TextFields[8]; } }
    }

    internal class TileWide310x150Text08 : TileWide310x150Base, ITileWide310x150Text08
    {
        public TileWide310x150Text08() : base(templateName: "TileWide310x150Text08",  fallbackName: "TileWideText08", imageCount: 0, textCount: 10)
        {
        }

        public INotificationContentText TextShortColumn1Row1 { get { return TextFields[0]; } }
        public INotificationContentText TextColumn2Row1 { get { return TextFields[1]; } }
        public INotificationContentText TextShortColumn1Row2 { get { return TextFields[2]; } }
        public INotificationContentText TextColumn2Row2 { get { return TextFields[3]; } }
        public INotificationContentText TextShortColumn1Row3 { get { return TextFields[4]; } }
        public INotificationContentText TextColumn2Row3 { get { return TextFields[5]; } }
        public INotificationContentText TextShortColumn1Row4 { get { return TextFields[6]; } }
        public INotificationContentText TextColumn2Row4 { get { return TextFields[7]; } }
        public INotificationContentText TextShortColumn1Row5 { get { return TextFields[8]; } }
        public INotificationContentText TextColumn2Row5 { get { return TextFields[9]; } }
    }

    internal class TileWide310x150Text09 : TileWide310x150Base, ITileWide310x150Text09
    {
        public TileWide310x150Text09() : base(templateName: "TileWide310x150Text09",  fallbackName: "TileWideText09", imageCount: 0, textCount: 2)
        {
        }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBodyWrap { get { return TextFields[1]; } }
    }

    internal class TileWide310x150Text10 : TileWide310x150Base, ITileWide310x150Text10
    {
        public TileWide310x150Text10() : base(templateName: "TileWide310x150Text10",  fallbackName: "TileWideText10", imageCount: 0, textCount: 9)
        {
        }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextPrefixColumn1Row1 { get { return TextFields[1]; } }
        public INotificationContentText TextColumn2Row1 { get { return TextFields[2]; } }
        public INotificationContentText TextPrefixColumn1Row2 { get { return TextFields[3]; } }
        public INotificationContentText TextColumn2Row2 { get { return TextFields[4]; } }
        public INotificationContentText TextPrefixColumn1Row3 { get { return TextFields[5]; } }
        public INotificationContentText TextColumn2Row3 { get { return TextFields[6]; } }
        public INotificationContentText TextPrefixColumn1Row4 { get { return TextFields[7]; } }
        public INotificationContentText TextColumn2Row4 { get { return TextFields[8]; } }
    }

    internal class TileWide310x150Text11 : TileWide310x150Base, ITileWide310x150Text11
    {
        public TileWide310x150Text11() : base(templateName: "TileWide310x150Text11",  fallbackName: "TileWideText11", imageCount: 0, textCount: 10)
        {
        }

        public INotificationContentText TextPrefixColumn1Row1 { get { return TextFields[0]; } }
        public INotificationContentText TextColumn2Row1 { get { return TextFields[1]; } }
        public INotificationContentText TextPrefixColumn1Row2 { get { return TextFields[2]; } }
        public INotificationContentText TextColumn2Row2 { get { return TextFields[3]; } }
        public INotificationContentText TextPrefixColumn1Row3 { get { return TextFields[4]; } }
        public INotificationContentText TextColumn2Row3 { get { return TextFields[5]; } }
        public INotificationContentText TextPrefixColumn1Row4 { get { return TextFields[6]; } }
        public INotificationContentText TextColumn2Row4 { get { return TextFields[7]; } }
        public INotificationContentText TextPrefixColumn1Row5 { get { return TextFields[8]; } }
        public INotificationContentText TextColumn2Row5 { get { return TextFields[9]; } }
    }

    internal class TileSquare310x310BlockAndText01 : TileSquare310x310Base, ITileSquare310x310BlockAndText01
    {
        public TileSquare310x310BlockAndText01()
            : base(templateName: "TileSquare310x310BlockAndText01", fallbackName: null, imageCount: 0, textCount: 9)
        {
        }

        public INotificationContentText TextHeadingWrap { get { return TextFields[0]; } }
        public INotificationContentText TextBody1 { get { return TextFields[1]; } }
        public INotificationContentText TextBody2 { get { return TextFields[2]; } }
        public INotificationContentText TextBody3 { get { return TextFields[3]; } }
        public INotificationContentText TextBody4 { get { return TextFields[4]; } }
        public INotificationContentText TextBody5 { get { return TextFields[5]; } }
        public INotificationContentText TextBody6 { get { return TextFields[6]; } }
        public INotificationContentText TextBlock { get { return TextFields[7]; } }
        public INotificationContentText TextSubBlock { get { return TextFields[8]; } }
    }

    internal class TileSquare310x310BlockAndText02 : TileSquare310x310Base, ITileSquare310x310BlockAndText02
    {
        public TileSquare310x310BlockAndText02()
            : base(templateName: "TileSquare310x310BlockAndText02", fallbackName: null, imageCount: 1, textCount: 7)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextBlock { get { return TextFields[0]; } }
        public INotificationContentText TextHeading1 { get { return TextFields[1]; } }
        public INotificationContentText TextHeading2 { get { return TextFields[2]; } }
        public INotificationContentText TextBody1 { get { return TextFields[3]; } }
        public INotificationContentText TextBody2 { get { return TextFields[4]; } }
        public INotificationContentText TextBody3 { get { return TextFields[5]; } }
        public INotificationContentText TextBody4 { get { return TextFields[6]; } }
    }

    internal class TileSquare310x310Image : TileSquare310x310Base, ITileSquare310x310Image
    {
        public TileSquare310x310Image()
            : base(templateName: "TileSquare310x310Image", fallbackName: null, imageCount: 1, textCount: 0)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }
    }

    internal class TileSquare310x310ImageAndText01 : TileSquare310x310Base, ITileSquare310x310ImageAndText01
    {
        public TileSquare310x310ImageAndText01()
            : base(templateName: "TileSquare310x310ImageAndText01", fallbackName: null, imageCount: 1, textCount: 1)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextCaptionWrap { get { return TextFields[0]; } }
    }

    internal class TileSquare310x310ImageAndText02 : TileSquare310x310Base, ITileSquare310x310ImageAndText02
    {
        public TileSquare310x310ImageAndText02()
            : base(templateName: "TileSquare310x310ImageAndText02", fallbackName: null, imageCount: 1, textCount: 2)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextCaption1 { get { return TextFields[0]; } }
        public INotificationContentText TextCaption2 { get { return TextFields[1]; } }
    }

    internal class TileSquare310x310ImageAndTextOverlay01 : TileSquare310x310Base, ITileSquare310x310ImageAndTextOverlay01
    {
        public TileSquare310x310ImageAndTextOverlay01()
            : base(templateName: "TileSquare310x310ImageAndTextOverlay01", fallbackName: null, imageCount: 1, textCount: 1)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextHeadingWrap { get { return TextFields[0]; } }
    }

    internal class TileSquare310x310ImageAndTextOverlay02 : TileSquare310x310Base, ITileSquare310x310ImageAndTextOverlay02
    {
        public TileSquare310x310ImageAndTextOverlay02()
            : base(templateName: "TileSquare310x310ImageAndTextOverlay02", fallbackName: null, imageCount: 1, textCount: 2)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextHeadingWrap { get { return TextFields[0]; } }
        public INotificationContentText TextBodyWrap { get { return TextFields[1]; } }
    }

    internal class TileSquare310x310ImageAndTextOverlay03 : TileSquare310x310Base, ITileSquare310x310ImageAndTextOverlay03
    {
        public TileSquare310x310ImageAndTextOverlay03()
            : base(templateName: "TileSquare310x310ImageAndTextOverlay03", fallbackName: null, imageCount: 1, textCount: 4)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }

        public INotificationContentText TextHeadingWrap { get { return TextFields[0]; } }
        public INotificationContentText TextBody1 { get { return TextFields[1]; } }
        public INotificationContentText TextBody2 { get { return TextFields[2]; } }
        public INotificationContentText TextBody3 { get { return TextFields[3]; } }
    }

    internal class TileSquare310x310ImageCollection : TileSquare310x310Base, ITileSquare310x310ImageCollection
    {
        public TileSquare310x310ImageCollection()
            : base(templateName: "TileSquare310x310ImageCollection", fallbackName: null, imageCount: 5, textCount: 0)
        {
        }

        public INotificationContentImage ImageMain { get { return Images[0]; } }
        public INotificationContentImage ImageSmall1 { get { return Images[1]; } }
        public INotificationContentImage ImageSmall2 { get { return Images[2]; } }
        public INotificationContentImage ImageSmall3 { get { return Images[3]; } }
        public INotificationContentImage ImageSmall4 { get { return Images[4]; } }
    }

    internal class TileSquare310x310ImageCollectionAndText01 : TileSquare310x310Base, ITileSquare310x310ImageCollectionAndText01
    {
        public TileSquare310x310ImageCollectionAndText01()
            : base(templateName: "TileSquare310x310ImageCollectionAndText01", fallbackName: null, imageCount: 5, textCount: 1)
        {
        }

        public INotificationContentImage ImageMain { get { return Images[0]; } }
        public INotificationContentImage ImageSmall1 { get { return Images[1]; } }
        public INotificationContentImage ImageSmall2 { get { return Images[2]; } }
        public INotificationContentImage ImageSmall3 { get { return Images[3]; } }
        public INotificationContentImage ImageSmall4 { get { return Images[4]; } }

        public INotificationContentText TextCaptionWrap { get { return TextFields[0]; } }
    }

    internal class TileSquare310x310ImageCollectionAndText02 : TileSquare310x310Base, ITileSquare310x310ImageCollectionAndText02
    {
        public TileSquare310x310ImageCollectionAndText02()
            : base(templateName: "TileSquare310x310ImageCollectionAndText02", fallbackName: null, imageCount: 5, textCount: 2)
        {
        }

        public INotificationContentImage ImageMain { get { return Images[0]; } }
        public INotificationContentImage ImageSmall1 { get { return Images[1]; } }
        public INotificationContentImage ImageSmall2 { get { return Images[2]; } }
        public INotificationContentImage ImageSmall3 { get { return Images[3]; } }
        public INotificationContentImage ImageSmall4 { get { return Images[4]; } }

        public INotificationContentText TextCaption1 { get { return TextFields[0]; } }
        public INotificationContentText TextCaption2 { get { return TextFields[1]; } }
    }

    internal class TileSquare310x310SmallImageAndText01 : TileSquare310x310Base, ITileSquare310x310SmallImageAndText01
    {
        public TileSquare310x310SmallImageAndText01()
            : base(templateName: "TileSquare310x310SmallImageAndText01", fallbackName: null, imageCount: 1, textCount: 3)
        {
        }

        public INotificationContentImage Image { get { return Images[0]; } }
        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBodyWrap { get { return TextFields[1]; } }
        public INotificationContentText TextBody { get { return TextFields[2]; } }
    }

    internal class TileSquare310x310SmallImagesAndTextList01 : TileSquare310x310Base, ITileSquare310x310SmallImagesAndTextList01
    {
        public TileSquare310x310SmallImagesAndTextList01()
            : base(templateName: "TileSquare310x310SmallImagesAndTextList01", fallbackName: null, imageCount: 3, textCount: 9)
        {
        }

        public INotificationContentImage Image1 { get { return Images[0]; } }
        public INotificationContentText TextHeading1 { get { return TextFields[0]; } }
        public INotificationContentText TextBodyGroup1Field1 { get { return TextFields[1]; } }
        public INotificationContentText TextBodyGroup1Field2 { get { return TextFields[2]; } }

        public INotificationContentImage Image2 { get { return Images[1]; } }
        public INotificationContentText TextHeading2 { get { return TextFields[3]; } }
        public INotificationContentText TextBodyGroup2Field1 { get { return TextFields[4]; } }
        public INotificationContentText TextBodyGroup2Field2 { get { return TextFields[5]; } }

        public INotificationContentImage Image3 { get { return Images[2]; } }
        public INotificationContentText TextHeading3 { get { return TextFields[6]; } }
        public INotificationContentText TextBodyGroup3Field1 { get { return TextFields[7]; } }
        public INotificationContentText TextBodyGroup3Field2 { get { return TextFields[8]; } }
    }

    internal class TileSquare310x310SmallImagesAndTextList02 : TileSquare310x310Base, ITileSquare310x310SmallImagesAndTextList02
    {
        public TileSquare310x310SmallImagesAndTextList02()
            : base(templateName: "TileSquare310x310SmallImagesAndTextList02", fallbackName: null, imageCount: 3, textCount: 3)
        {
        }

        public INotificationContentImage Image1 { get { return Images[0]; } }
        public INotificationContentText TextWrap1 { get { return TextFields[0]; } }

        public INotificationContentImage Image2 { get { return Images[1]; } }
        public INotificationContentText TextWrap2 { get { return TextFields[1]; } }

        public INotificationContentImage Image3 { get { return Images[2]; } }
        public INotificationContentText TextWrap3 { get { return TextFields[2]; } }
    }

    internal class TileSquare310x310SmallImagesAndTextList03 : TileSquare310x310Base, ITileSquare310x310SmallImagesAndTextList03
    {
        public TileSquare310x310SmallImagesAndTextList03()
            : base(templateName: "TileSquare310x310SmallImagesAndTextList03", fallbackName: null, imageCount: 3, textCount: 6)
        {
        }

        public INotificationContentImage Image1 { get { return Images[0]; } }
        public INotificationContentText TextHeading1 { get { return TextFields[0]; } }
        public INotificationContentText TextWrap1 { get { return TextFields[1]; } }

        public INotificationContentImage Image2 { get { return Images[1]; } }
        public INotificationContentText TextHeading2 { get { return TextFields[2]; } }
        public INotificationContentText TextWrap2 { get { return TextFields[3]; } }

        public INotificationContentImage Image3 { get { return Images[2]; } }
        public INotificationContentText TextHeading3 { get { return TextFields[4]; } }
        public INotificationContentText TextWrap3 { get { return TextFields[5]; } } 
    }

    internal class TileSquare310x310SmallImagesAndTextList04 : TileSquare310x310Base, ITileSquare310x310SmallImagesAndTextList04
    {
        public TileSquare310x310SmallImagesAndTextList04()
            : base(templateName: "TileSquare310x310SmallImagesAndTextList04", fallbackName: null, imageCount: 3, textCount: 6)
        {
        }

        public INotificationContentImage Image1 { get { return Images[0]; } }
        public INotificationContentText TextHeading1 { get { return TextFields[0]; } }
        public INotificationContentText TextWrap1 { get { return TextFields[1]; } }

        public INotificationContentImage Image2 { get { return Images[1]; } }
        public INotificationContentText TextHeading2 { get { return TextFields[2]; } }
        public INotificationContentText TextWrap2 { get { return TextFields[3]; } }

        public INotificationContentImage Image3 { get { return Images[2]; } }
        public INotificationContentText TextHeading3 { get { return TextFields[4]; } }
        public INotificationContentText TextWrap3 { get { return TextFields[5]; } }
    }

    internal class TileSquare310x310SmallImagesAndTextList05 : TileSquare310x310Base, ITileSquare310x310SmallImagesAndTextList05
    {
        public TileSquare310x310SmallImagesAndTextList05()
            : base(templateName: "TileSquare310x310SmallImagesAndTextList05", fallbackName: null, imageCount: 3, textCount: 7)
        {
        }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
 
        public INotificationContentImage Image1 { get { return Images[0]; } }
        public INotificationContentText TextGroup1Field1 { get { return TextFields[1]; } }
        public INotificationContentText TextGroup1Field2 { get { return TextFields[2]; } }

        public INotificationContentImage Image2 { get { return Images[1]; } }
        public INotificationContentText TextGroup2Field1 { get { return TextFields[3]; } }
        public INotificationContentText TextGroup2Field2 { get { return TextFields[4]; } }

        public INotificationContentImage Image3 { get { return Images[2]; } }
        public INotificationContentText TextGroup3Field1 { get { return TextFields[5]; } }
        public INotificationContentText TextGroup3Field2 { get { return TextFields[6]; } }
    }

    internal class TileSquare310x310Text01 : TileSquare310x310Base, ITileSquare310x310Text01
    {
        public TileSquare310x310Text01()
            : base(templateName: "TileSquare310x310Text01", fallbackName: null, imageCount: 0, textCount: 10)
        {
        }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBody1   { get { return TextFields[1]; } }
        public INotificationContentText TextBody2   { get { return TextFields[2]; } }
        public INotificationContentText TextBody3   { get { return TextFields[3]; } }
        public INotificationContentText TextBody4   { get { return TextFields[4]; } }
        public INotificationContentText TextBody5   { get { return TextFields[5]; } }
        public INotificationContentText TextBody6   { get { return TextFields[6]; } }
        public INotificationContentText TextBody7   { get { return TextFields[7]; } }
        public INotificationContentText TextBody8   { get { return TextFields[8]; } }
        public INotificationContentText TextBody9   { get { return TextFields[9]; } }
    }

    internal class TileSquare310x310Text02 : TileSquare310x310Base, ITileSquare310x310Text02
    {
        public TileSquare310x310Text02()
            : base(templateName: "TileSquare310x310Text02", fallbackName: null, imageCount: 0, textCount: 19)
        {
        }

        public INotificationContentText TextHeading     { get { return TextFields[0]; } }
        public INotificationContentText TextColumn1Row1 { get { return TextFields[1]; } }
        public INotificationContentText TextColumn2Row1 { get { return TextFields[2]; } }
        public INotificationContentText TextColumn1Row2 { get { return TextFields[3]; } }
        public INotificationContentText TextColumn2Row2 { get { return TextFields[4]; } }
        public INotificationContentText TextColumn1Row3 { get { return TextFields[5]; } }
        public INotificationContentText TextColumn2Row3 { get { return TextFields[6]; } }
        public INotificationContentText TextColumn1Row4 { get { return TextFields[7]; } }
        public INotificationContentText TextColumn2Row4 { get { return TextFields[8]; } }
        public INotificationContentText TextColumn1Row5 { get { return TextFields[9]; } }
        public INotificationContentText TextColumn2Row5 { get { return TextFields[10]; } }
        public INotificationContentText TextColumn1Row6 { get { return TextFields[11]; } }
        public INotificationContentText TextColumn2Row6 { get { return TextFields[12]; } }
        public INotificationContentText TextColumn1Row7 { get { return TextFields[13]; } }
        public INotificationContentText TextColumn2Row7 { get { return TextFields[14]; } }
        public INotificationContentText TextColumn1Row8 { get { return TextFields[15]; } }
        public INotificationContentText TextColumn2Row8 { get { return TextFields[16]; } }
        public INotificationContentText TextColumn1Row9 { get { return TextFields[17]; } }
        public INotificationContentText TextColumn2Row9 { get { return TextFields[18]; } }
    }

    internal class TileSquare310x310Text03 : TileSquare310x310Base, ITileSquare310x310Text03
    {
        public TileSquare310x310Text03()
            : base(templateName: "TileSquare310x310Text03", fallbackName: null, imageCount: 0, textCount: 11)
        {
        }

        public INotificationContentText TextBody1  { get  { return TextFields[0]; } }
        public INotificationContentText TextBody2  { get  { return TextFields[1]; } }
        public INotificationContentText TextBody3  { get  { return TextFields[2]; } }
        public INotificationContentText TextBody4  { get  { return TextFields[3]; } }
        public INotificationContentText TextBody5  { get  { return TextFields[4]; } }
        public INotificationContentText TextBody6  { get  { return TextFields[5]; } }
        public INotificationContentText TextBody7  { get  { return TextFields[6]; } }
        public INotificationContentText TextBody8  { get  { return TextFields[7]; } }
        public INotificationContentText TextBody9  { get  { return TextFields[8]; } }
        public INotificationContentText TextBody10 { get { return TextFields[9]; } }
        public INotificationContentText TextBody11 { get { return TextFields[10]; } }
    }

    internal class TileSquare310x310Text04 : TileSquare310x310Base, ITileSquare310x310Text04
    {
        public TileSquare310x310Text04()
            : base(templateName: "TileSquare310x310Text04", fallbackName: null, imageCount: 0, textCount: 22)
        {
        }

        public INotificationContentText TextColumn1Row1  { get { return TextFields[0]; } }
        public INotificationContentText TextColumn2Row1  { get { return TextFields[1]; } }
        public INotificationContentText TextColumn1Row2  { get { return TextFields[2]; } }
        public INotificationContentText TextColumn2Row2  { get { return TextFields[3]; } }
        public INotificationContentText TextColumn1Row3  { get { return TextFields[4]; } }
        public INotificationContentText TextColumn2Row3  { get { return TextFields[5]; } }
        public INotificationContentText TextColumn1Row4  { get { return TextFields[6]; } }
        public INotificationContentText TextColumn2Row4  { get { return TextFields[7]; } }
        public INotificationContentText TextColumn1Row5  { get { return TextFields[8]; } }
        public INotificationContentText TextColumn2Row5  { get { return TextFields[9];  } }
        public INotificationContentText TextColumn1Row6  { get { return TextFields[10]; } }
        public INotificationContentText TextColumn2Row6  { get { return TextFields[11]; } }
        public INotificationContentText TextColumn1Row7  { get { return TextFields[12]; } }
        public INotificationContentText TextColumn2Row7  { get { return TextFields[13]; } }
        public INotificationContentText TextColumn1Row8  { get { return TextFields[14]; } }
        public INotificationContentText TextColumn2Row8  { get { return TextFields[15]; } }
        public INotificationContentText TextColumn1Row9  { get { return TextFields[16]; } }
        public INotificationContentText TextColumn2Row9  { get { return TextFields[17]; } }
        public INotificationContentText TextColumn1Row10 { get { return TextFields[18]; } }
        public INotificationContentText TextColumn2Row10 { get { return TextFields[19]; } }
        public INotificationContentText TextColumn1Row11 { get { return TextFields[20]; } }
        public INotificationContentText TextColumn2Row11 { get { return TextFields[21]; } }
    }

    internal class TileSquare310x310Text05 : TileSquare310x310Base, ITileSquare310x310Text05
    {
        public TileSquare310x310Text05()
            : base(templateName: "TileSquare310x310Text05", fallbackName: null, imageCount: 0, textCount: 19)
        {
        }

        public INotificationContentText TextHeading     { get { return TextFields[0]; } }
        public INotificationContentText TextColumn1Row1 { get { return TextFields[1]; } }
        public INotificationContentText TextColumn2Row1 { get { return TextFields[2]; } }
        public INotificationContentText TextColumn1Row2 { get { return TextFields[3]; } }
        public INotificationContentText TextColumn2Row2 { get { return TextFields[4]; } }
        public INotificationContentText TextColumn1Row3 { get { return TextFields[5]; } }
        public INotificationContentText TextColumn2Row3 { get { return TextFields[6]; } }
        public INotificationContentText TextColumn1Row4 { get { return TextFields[7]; } }
        public INotificationContentText TextColumn2Row4 { get { return TextFields[8]; } }
        public INotificationContentText TextColumn1Row5 { get { return TextFields[9]; } }
        public INotificationContentText TextColumn2Row5 { get { return TextFields[10]; } }
        public INotificationContentText TextColumn1Row6 { get { return TextFields[11]; } }
        public INotificationContentText TextColumn2Row6 { get { return TextFields[12]; } }
        public INotificationContentText TextColumn1Row7 { get { return TextFields[13]; } }
        public INotificationContentText TextColumn2Row7 { get { return TextFields[14]; } }
        public INotificationContentText TextColumn1Row8 { get { return TextFields[15]; } }
        public INotificationContentText TextColumn2Row8 { get { return TextFields[16]; } }
        public INotificationContentText TextColumn1Row9 { get { return TextFields[17]; } }
        public INotificationContentText TextColumn2Row9 { get { return TextFields[18]; } }
    }

    internal class TileSquare310x310Text06 : TileSquare310x310Base, ITileSquare310x310Text06
    {
        public TileSquare310x310Text06()
            : base(templateName: "TileSquare310x310Text06", fallbackName: null, imageCount: 0, textCount: 22)
        {
        }

        public INotificationContentText TextColumn1Row1  { get { return TextFields[0]; } }
        public INotificationContentText TextColumn2Row1  { get { return TextFields[1]; } }
        public INotificationContentText TextColumn1Row2  { get { return TextFields[2]; } }
        public INotificationContentText TextColumn2Row2  { get { return TextFields[3]; } }
        public INotificationContentText TextColumn1Row3  { get { return TextFields[4]; } }
        public INotificationContentText TextColumn2Row3  { get { return TextFields[5]; } }
        public INotificationContentText TextColumn1Row4  { get { return TextFields[6]; } }
        public INotificationContentText TextColumn2Row4  { get { return TextFields[7]; } }
        public INotificationContentText TextColumn1Row5  { get { return TextFields[8]; } }
        public INotificationContentText TextColumn2Row5  { get { return TextFields[9];  } }
        public INotificationContentText TextColumn1Row6  { get { return TextFields[10]; } }
        public INotificationContentText TextColumn2Row6  { get { return TextFields[11]; } }
        public INotificationContentText TextColumn1Row7  { get { return TextFields[12]; } }
        public INotificationContentText TextColumn2Row7  { get { return TextFields[13]; } }
        public INotificationContentText TextColumn1Row8  { get { return TextFields[14]; } }
        public INotificationContentText TextColumn2Row8  { get { return TextFields[15]; } }
        public INotificationContentText TextColumn1Row9  { get { return TextFields[16]; } }
        public INotificationContentText TextColumn2Row9  { get { return TextFields[17]; } }
        public INotificationContentText TextColumn1Row10 { get { return TextFields[18]; } }
        public INotificationContentText TextColumn2Row10 { get { return TextFields[19]; } }
        public INotificationContentText TextColumn1Row11 { get { return TextFields[20]; } }
        public INotificationContentText TextColumn2Row11 { get { return TextFields[21]; } }
    }

    internal class TileSquare310x310Text07 : TileSquare310x310Base, ITileSquare310x310Text07
    {
        public TileSquare310x310Text07()
            : base(templateName: "TileSquare310x310Text07", fallbackName: null, imageCount: 0, textCount: 19)
        {
        }

        public INotificationContentText TextHeading     { get { return TextFields[0]; } }
        public INotificationContentText TextColumn1Row1 { get { return TextFields[1]; } }
        public INotificationContentText TextColumn2Row1 { get { return TextFields[2]; } }
        public INotificationContentText TextColumn1Row2 { get { return TextFields[3]; } }
        public INotificationContentText TextColumn2Row2 { get { return TextFields[4]; } }
        public INotificationContentText TextColumn1Row3 { get { return TextFields[5]; } }
        public INotificationContentText TextColumn2Row3 { get { return TextFields[6]; } }
        public INotificationContentText TextColumn1Row4 { get { return TextFields[7]; } }
        public INotificationContentText TextColumn2Row4 { get { return TextFields[8]; } }
        public INotificationContentText TextColumn1Row5 { get { return TextFields[9]; } }
        public INotificationContentText TextColumn2Row5 { get { return TextFields[10]; } }
        public INotificationContentText TextColumn1Row6 { get { return TextFields[11]; } }
        public INotificationContentText TextColumn2Row6 { get { return TextFields[12]; } }
        public INotificationContentText TextColumn1Row7 { get { return TextFields[13]; } }
        public INotificationContentText TextColumn2Row7 { get { return TextFields[14]; } }
        public INotificationContentText TextColumn1Row8 { get { return TextFields[15]; } }
        public INotificationContentText TextColumn2Row8 { get { return TextFields[16]; } }
        public INotificationContentText TextColumn1Row9 { get { return TextFields[17]; } }
        public INotificationContentText TextColumn2Row9 { get { return TextFields[18]; } }
    }

    internal class TileSquare310x310Text08 : TileSquare310x310Base, ITileSquare310x310Text08
    {
        public TileSquare310x310Text08()
            : base(templateName: "TileSquare310x310Text08", fallbackName: null, imageCount: 0, textCount: 22)
        {
        }

        public INotificationContentText TextColumn1Row1  { get { return TextFields[0]; } }
        public INotificationContentText TextColumn2Row1  { get { return TextFields[1]; } }
        public INotificationContentText TextColumn1Row2  { get { return TextFields[2]; } }
        public INotificationContentText TextColumn2Row2  { get { return TextFields[3]; } }
        public INotificationContentText TextColumn1Row3  { get { return TextFields[4]; } }
        public INotificationContentText TextColumn2Row3  { get { return TextFields[5]; } }
        public INotificationContentText TextColumn1Row4  { get { return TextFields[6]; } }
        public INotificationContentText TextColumn2Row4  { get { return TextFields[7]; } }
        public INotificationContentText TextColumn1Row5  { get { return TextFields[8]; } }
        public INotificationContentText TextColumn2Row5  { get { return TextFields[9];  } }
        public INotificationContentText TextColumn1Row6  { get { return TextFields[10]; } }
        public INotificationContentText TextColumn2Row6  { get { return TextFields[11]; } }
        public INotificationContentText TextColumn1Row7  { get { return TextFields[12]; } }
        public INotificationContentText TextColumn2Row7  { get { return TextFields[13]; } }
        public INotificationContentText TextColumn1Row8  { get { return TextFields[14]; } }
        public INotificationContentText TextColumn2Row8  { get { return TextFields[15]; } }
        public INotificationContentText TextColumn1Row9  { get { return TextFields[16]; } }
        public INotificationContentText TextColumn2Row9  { get { return TextFields[17]; } }
        public INotificationContentText TextColumn1Row10 { get { return TextFields[18]; } }
        public INotificationContentText TextColumn2Row10 { get { return TextFields[19]; } }
        public INotificationContentText TextColumn1Row11 { get { return TextFields[20]; } }
        public INotificationContentText TextColumn2Row11 { get { return TextFields[21]; } }
    }

    internal class TileSquare310x310Text09 : TileSquare310x310Base, ITileSquare310x310Text09
    {
        public TileSquare310x310Text09()
            : base(templateName: "TileSquare310x310Text09", fallbackName: null, imageCount: 0, textCount: 5)
        {
        }

        public INotificationContentText TextHeadingWrap { get { return TextFields[0]; } }
        public INotificationContentText TextHeading1 { get { return TextFields[1]; } }
        public INotificationContentText TextHeading2 { get { return TextFields[2]; } }
        public INotificationContentText TextBody1 { get { return TextFields[3]; } }
        public INotificationContentText TextBody2 { get { return TextFields[4]; } }
    }

    internal class TileSquare310x310TextList01 : TileSquare310x310Base, ITileSquare310x310TextList01
    {
        public TileSquare310x310TextList01()
            : base(templateName: "TileSquare310x310TextList01", fallbackName: null, imageCount: 0, textCount: 9)
        {
        }

        public INotificationContentText TextHeading1 { get { return TextFields[0]; } }
        public INotificationContentText TextBodyGroup1Field1 { get { return TextFields[1]; } }
        public INotificationContentText TextBodyGroup1Field2 { get { return TextFields[2]; } }

        public INotificationContentText TextHeading2 { get { return TextFields[3]; } }
        public INotificationContentText TextBodyGroup2Field1 { get { return TextFields[4]; } }
        public INotificationContentText TextBodyGroup2Field2 { get { return TextFields[5]; } }

        public INotificationContentText TextHeading3 { get { return TextFields[6]; } }
        public INotificationContentText TextBodyGroup3Field1 { get { return TextFields[7]; } }
        public INotificationContentText TextBodyGroup3Field2 { get { return TextFields[8]; } }
    }

    internal class TileSquare310x310TextList02 : TileSquare310x310Base, ITileSquare310x310TextList02
    {
        public TileSquare310x310TextList02()
            : base(templateName: "TileSquare310x310TextList02", fallbackName: null, imageCount: 0, textCount: 3)
        {
        }

        public INotificationContentText TextWrap1 { get { return TextFields[0]; } }

        public INotificationContentText TextWrap2 { get { return TextFields[1]; } }

        public INotificationContentText TextWrap3 { get { return TextFields[2]; } }
    }

    internal class TileSquare310x310TextList03 : TileSquare310x310Base, ITileSquare310x310TextList03
    {
        public TileSquare310x310TextList03()
            : base(templateName: "TileSquare310x310TextList03", fallbackName: null, imageCount: 0, textCount: 6)
        {
        }

        public INotificationContentText TextHeading1 { get { return TextFields[0]; } }
        public INotificationContentText TextWrap1 { get { return TextFields[1]; } }

        public INotificationContentText TextHeading2 { get { return TextFields[2]; } }
        public INotificationContentText TextWrap2 { get { return TextFields[3]; } }

        public INotificationContentText TextHeading3 { get { return TextFields[4]; } }
        public INotificationContentText TextWrap3 { get { return TextFields[5]; } }
    }

    internal class TileSquare99x99IconWithBadge : TileSquare99x99Base, ITileSquare99x99IconWithBadge
    {
        public TileSquare99x99IconWithBadge()
            : base(templateName: "TileSquare99x99IconWithBadge", fallbackName: null, imageCount: 1, textCount: 0)
        {
        }

        public INotificationContentImage ImageIcon { get { return Images[0]; } }
    }

    internal class TileSquare210x210IconWithBadge : TileSquare210x210Base, ITileSquare210x210IconWithBadge
    {
        public TileSquare210x210IconWithBadge()
            : base(templateName: "TileSquare210x210IconWithBadge", fallbackName: null, imageCount: 1, textCount: 0)
        {
        }

        public INotificationContentImage ImageIcon { get { return Images[0]; } }
    }

    internal class TileWide432x210IconWithBadgeAndText : TileWide432x210Base, ITileWide432x210IconWithBadgeAndText
    {
        public TileWide432x210IconWithBadgeAndText()
            : base(templateName: "TileWide432x210IconWithBadgeAndText", fallbackName: null, imageCount: 1, textCount: 3)
        {
        }

        public INotificationContentImage ImageIcon  { get { return Images[0]; } }

        public INotificationContentText TextHeading { get { return TextFields[0]; } }
        public INotificationContentText TextBody1   { get { return TextFields[1]; } }
        public INotificationContentText TextBody2   { get { return TextFields[2]; } }
    }

    /// <summary>
    /// A factory which creates tile content objects for all of the toast template types.
    /// </summary>
    public sealed class TileContentFactory
    {
        /// <summary>
        /// Creates a tile content object for the TileSquare150x150Block template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare150x150Block template.</returns>
        public static ITileSquare150x150Block CreateTileSquare150x150Block()
        {
            return new TileSquare150x150Block();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare150x150Image template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare150x150Image template.</returns>
        public static ITileSquare150x150Image CreateTileSquare150x150Image()
        {
            return new TileSquare150x150Image();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare150x150PeekImageAndText01 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare150x150PeekImageAndText01 template.</returns>
        public static ITileSquare150x150PeekImageAndText01 CreateTileSquare150x150PeekImageAndText01()
        {
            return new TileSquare150x150PeekImageAndText01();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare150x150PeekImageAndText02 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare150x150PeekImageAndText02 template.</returns>
        public static ITileSquare150x150PeekImageAndText02 CreateTileSquare150x150PeekImageAndText02()
        {
            return new TileSquare150x150PeekImageAndText02();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare150x150PeekImageAndText03 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare150x150PeekImageAndText03 template.</returns>
        public static ITileSquare150x150PeekImageAndText03 CreateTileSquare150x150PeekImageAndText03()
        {
            return new TileSquare150x150PeekImageAndText03();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare150x150PeekImageAndText04 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare150x150PeekImageAndText04 template.</returns>
        public static ITileSquare150x150PeekImageAndText04 CreateTileSquare150x150PeekImageAndText04()
        {
            return new TileSquare150x150PeekImageAndText04();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare150x150Text01 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare150x150Text01 template.</returns>
        public static ITileSquare150x150Text01 CreateTileSquare150x150Text01()
        {
            return new TileSquare150x150Text01();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare150x150Text02 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare150x150Text02 template.</returns>
        public static ITileSquare150x150Text02 CreateTileSquare150x150Text02()
        {
            return new TileSquare150x150Text02();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare150x150Text03 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare150x150Text03 template.</returns>
        public static ITileSquare150x150Text03 CreateTileSquare150x150Text03()
        {
            return new TileSquare150x150Text03();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare150x150Text04 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare150x150Text04 template.</returns>
        public static ITileSquare150x150Text04 CreateTileSquare150x150Text04()
        {
            return new TileSquare150x150Text04();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150BlockAndText01 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150BlockAndText01 template.</returns>
        public static ITileWide310x150BlockAndText01 CreateTileWide310x150BlockAndText01()
        {
            return new TileWide310x150BlockAndText01();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150BlockAndText02 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150BlockAndText02 template.</returns>
        public static ITileWide310x150BlockAndText02 CreateTileWide310x150BlockAndText02()
        {
            return new TileWide310x150BlockAndText02();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150Image template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150Image template.</returns>
        public static ITileWide310x150Image CreateTileWide310x150Image()
        {
            return new TileWide310x150Image();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150ImageAndText01 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150ImageAndText01 template.</returns>
        public static ITileWide310x150ImageAndText01 CreateTileWide310x150ImageAndText01()
        {
            return new TileWide310x150ImageAndText01();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150ImageAndText02 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150ImageAndText02 template.</returns>
        public static ITileWide310x150ImageAndText02 CreateTileWide310x150ImageAndText02()
        {
            return new TileWide310x150ImageAndText02();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150ImageCollection template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150ImageCollection template.</returns>
        public static ITileWide310x150ImageCollection CreateTileWide310x150ImageCollection()
        {
            return new TileWide310x150ImageCollection();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImage01 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImage01 template.</returns>
        public static ITileWide310x150PeekImage01 CreateTileWide310x150PeekImage01()
        {
            return new TileWide310x150PeekImage01();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImage02 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImage02 template.</returns>
        public static ITileWide310x150PeekImage02 CreateTileWide310x150PeekImage02()
        {
            return new TileWide310x150PeekImage02();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImage03 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImage03 template.</returns>
        public static ITileWide310x150PeekImage03 CreateTileWide310x150PeekImage03()
        {
            return new TileWide310x150PeekImage03();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImage04 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImage04 template.</returns>
        public static ITileWide310x150PeekImage04 CreateTileWide310x150PeekImage04()
        {
            return new TileWide310x150PeekImage04();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImage05 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImage05 template.</returns>
        public static ITileWide310x150PeekImage05 CreateTileWide310x150PeekImage05()
        {
            return new TileWide310x150PeekImage05();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImage06 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImage06 template.</returns>
        public static ITileWide310x150PeekImage06 CreateTileWide310x150PeekImage06()
        {
            return new TileWide310x150PeekImage06();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImageAndText01 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImageAndText01 template.</returns>
        public static ITileWide310x150PeekImageAndText01 CreateTileWide310x150PeekImageAndText01()
        {
            return new TileWide310x150PeekImageAndText01();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImageAndText02 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImageAndText02 template.</returns>
        public static ITileWide310x150PeekImageAndText02 CreateTileWide310x150PeekImageAndText02()
        {
            return new TileWide310x150PeekImageAndText02();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImageCollection01 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImageCollection01 template.</returns>
        public static ITileWide310x150PeekImageCollection01 CreateTileWide310x150PeekImageCollection01()
        {
            return new TileWide310x150PeekImageCollection01();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImageCollection02 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImageCollection02 template.</returns>
        public static ITileWide310x150PeekImageCollection02 CreateTileWide310x150PeekImageCollection02()
        {
            return new TileWide310x150PeekImageCollection02();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImageCollection03 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImageCollection03 template.</returns>
        public static ITileWide310x150PeekImageCollection03 CreateTileWide310x150PeekImageCollection03()
        {
            return new TileWide310x150PeekImageCollection03();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImageCollection04 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImageCollection04 template.</returns>
        public static ITileWide310x150PeekImageCollection04 CreateTileWide310x150PeekImageCollection04()
        {
            return new TileWide310x150PeekImageCollection04();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImageCollection05 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImageCollection05 template.</returns>
        public static ITileWide310x150PeekImageCollection05 CreateTileWide310x150PeekImageCollection05()
        {
            return new TileWide310x150PeekImageCollection05();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150PeekImageCollection06 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150PeekImageCollection06 template.</returns>
        public static ITileWide310x150PeekImageCollection06 CreateTileWide310x150PeekImageCollection06()
        {
            return new TileWide310x150PeekImageCollection06();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150SmallImageAndText01 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150SmallImageAndText01 template.</returns>
        public static ITileWide310x150SmallImageAndText01 CreateTileWide310x150SmallImageAndText01()
        {
            return new TileWide310x150SmallImageAndText01();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150SmallImageAndText02 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150SmallImageAndText02 template.</returns>
        public static ITileWide310x150SmallImageAndText02 CreateTileWide310x150SmallImageAndText02()
        {
            return new TileWide310x150SmallImageAndText02();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150SmallImageAndText03 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150SmallImageAndText03 template.</returns>

        public static ITileWide310x150SmallImageAndText03 CreateTileWide310x150SmallImageAndText03()
        {
            return new TileWide310x150SmallImageAndText03();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150SmallImageAndText04 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150SmallImageAndText04 template.</returns>
        public static ITileWide310x150SmallImageAndText04 CreateTileWide310x150SmallImageAndText04()
        {
            return new TileWide310x150SmallImageAndText04();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150SmallImageAndText05 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150SmallImageAndText05 template.</returns>
        public static ITileWide310x150SmallImageAndText05 CreateTileWide310x150SmallImageAndText05()
        {
            return new TileWide310x150SmallImageAndText05();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150Text01 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150Text01 template.</returns>
        public static ITileWide310x150Text01 CreateTileWide310x150Text01()
        {
            return new TileWide310x150Text01();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150Text02 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150Text02 template.</returns>
        public static ITileWide310x150Text02 CreateTileWide310x150Text02()
        {
            return new TileWide310x150Text02();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150Text03 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150Text03 template.</returns>
        public static ITileWide310x150Text03 CreateTileWide310x150Text03()
        {
            return new TileWide310x150Text03();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150Text04 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150Text04 template.</returns>
        public static ITileWide310x150Text04 CreateTileWide310x150Text04()
        {
            return new TileWide310x150Text04();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150Text05 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150Text05 template.</returns>
        public static ITileWide310x150Text05 CreateTileWide310x150Text05()
        {
            return new TileWide310x150Text05();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150Text06 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150Text06 template.</returns>
        public static ITileWide310x150Text06 CreateTileWide310x150Text06()
        {
            return new TileWide310x150Text06();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150Text07 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150Text07 template.</returns>
        public static ITileWide310x150Text07 CreateTileWide310x150Text07()
        {
            return new TileWide310x150Text07();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150Text08 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150Text08 template.</returns>
        public static ITileWide310x150Text08 CreateTileWide310x150Text08()
        {
            return new TileWide310x150Text08();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150Text09 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150Text09 template.</returns>
        public static ITileWide310x150Text09 CreateTileWide310x150Text09()
        {
            return new TileWide310x150Text09();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150Text10 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150Text10 template.</returns>
        public static ITileWide310x150Text10 CreateTileWide310x150Text10()
        {
            return new TileWide310x150Text10();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide310x150Text11 template.
        /// </summary>
        /// <returns>A tile content object for the TileWide310x150Text11 template.</returns>
        public static ITileWide310x150Text11 CreateTileWide310x150Text11()
        {
            return new TileWide310x150Text11();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310BlockAndText01 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310BlockAndText01 template.</returns>
        public static ITileSquare310x310BlockAndText01 CreateTileSquare310x310BlockAndText01()
        {
            return new TileSquare310x310BlockAndText01();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310BlockAndText02 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310BlockAndText02 template.</returns>
        public static ITileSquare310x310BlockAndText02 CreateTileSquare310x310BlockAndText02()
        {
            return new TileSquare310x310BlockAndText02();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310Image template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310Image template.</returns>
        public static ITileSquare310x310Image CreateTileSquare310x310Image()
        {
            return new TileSquare310x310Image();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310ImageAndText01 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310ImageAndText01 template.</returns>
        public static ITileSquare310x310ImageAndText01 CreateTileSquare310x310ImageAndText01()
        {
            return new TileSquare310x310ImageAndText01();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310ImageAndText02 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310ImageAndText02 template.</returns>
        public static ITileSquare310x310ImageAndText02 CreateTileSquare310x310ImageAndText02()
        {
            return new TileSquare310x310ImageAndText02();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310ImageAndTextOverlay01 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310ImageAndTextOverlay01 template.</returns>
        public static ITileSquare310x310ImageAndTextOverlay01 CreateTileSquare310x310ImageAndTextOverlay01()
        {
            return new TileSquare310x310ImageAndTextOverlay01();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310ImageAndTextOverlay02 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310ImageAndTextOverlay02 template.</returns>
        public static ITileSquare310x310ImageAndTextOverlay02 CreateTileSquare310x310ImageAndTextOverlay02()
        {
            return new TileSquare310x310ImageAndTextOverlay02();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310ImageAndTextOverlay03 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310ImageAndTextOverlay03 template.</returns>
        public static ITileSquare310x310ImageAndTextOverlay03 CreateTileSquare310x310ImageAndTextOverlay03()
        {
            return new TileSquare310x310ImageAndTextOverlay03();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310ImageCollection template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310ImageCollection template.</returns>
        public static ITileSquare310x310ImageCollection CreateTileSquare310x310ImageCollection()
        {
            return new TileSquare310x310ImageCollection();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310ImageCollectionAndText01 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310ImageCollectionAndText01 template.</returns>
        public static ITileSquare310x310ImageCollectionAndText01 CreateTileSquare310x310ImageCollectionAndText01()
        {
            return new TileSquare310x310ImageCollectionAndText01();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310ImageCollectionAndText02 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310ImageCollectionAndText02 template.</returns>
        public static ITileSquare310x310ImageCollectionAndText02 CreateTileSquare310x310ImageCollectionAndText02()
        {
            return new TileSquare310x310ImageCollectionAndText02();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310SmallImageAndText01 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310SmallImageAndText01 template.</returns>
        public static ITileSquare310x310SmallImageAndText01 CreateTileSquare310x310SmallImageAndText01()
        {
            return new TileSquare310x310SmallImageAndText01();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310SmallImagesAndTextList01 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310SmallImagesAndTextList01 template.</returns>
        public static ITileSquare310x310SmallImagesAndTextList01 CreateTileSquare310x310SmallImagesAndTextList01()
        {
            return new TileSquare310x310SmallImagesAndTextList01();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310SmallImagesAndTextList02 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310SmallImagesAndTextList02 template.</returns>
        public static ITileSquare310x310SmallImagesAndTextList02 CreateTileSquare310x310SmallImagesAndTextList02()
        {
            return new TileSquare310x310SmallImagesAndTextList02();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310SmallImagesAndTextList03 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310SmallImagesAndTextList03 template.</returns>
        public static ITileSquare310x310SmallImagesAndTextList03 CreateTileSquare310x310SmallImagesAndTextList03()
        {
            return new TileSquare310x310SmallImagesAndTextList03();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310SmallImagesAndTextList04 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310SmallImagesAndTextList04 template.</returns>
        public static ITileSquare310x310SmallImagesAndTextList04 CreateTileSquare310x310SmallImagesAndTextList04()
        {
            return new TileSquare310x310SmallImagesAndTextList04();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310SmallImagesAndTextList05 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310SmallImagesAndTextList05 template.</returns>
        public static ITileSquare310x310SmallImagesAndTextList05 CreateTileSquare310x310SmallImagesAndTextList05()
        {
            return new TileSquare310x310SmallImagesAndTextList05();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310Text01 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310Text01 template.</returns>
        public static ITileSquare310x310Text01 CreateTileSquare310x310Text01()
        {
            return new TileSquare310x310Text01();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310Text02 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310Text02 template.</returns>
        public static ITileSquare310x310Text02 CreateTileSquare310x310Text02()
        {
            return new TileSquare310x310Text02();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310Text03 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310Text03 template.</returns>
        public static ITileSquare310x310Text03 CreateTileSquare310x310Text03()
        {
            return new TileSquare310x310Text03();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310Text04 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310Text04 template.</returns>
        public static ITileSquare310x310Text04 CreateTileSquare310x310Text04()
        {
            return new TileSquare310x310Text04();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310Text05 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310Text05 template.</returns>
        public static ITileSquare310x310Text05 CreateTileSquare310x310Text05()
        {
            return new TileSquare310x310Text05();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310Text06 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310Text06 template.</returns>
        public static ITileSquare310x310Text06 CreateTileSquare310x310Text06()
        {
            return new TileSquare310x310Text06();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310Text07 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310Text07 template.</returns>
        public static ITileSquare310x310Text07 CreateTileSquare310x310Text07()
        {
            return new TileSquare310x310Text07();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310Text08 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310Text08 template.</returns>
        public static ITileSquare310x310Text08 CreateTileSquare310x310Text08()
        {
            return new TileSquare310x310Text08();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310Text09 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310Text09 template.</returns>
        public static ITileSquare310x310Text09 CreateTileSquare310x310Text09()
        {
            return new TileSquare310x310Text09();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310TextList01 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310TextList01 template.</returns>
        public static ITileSquare310x310TextList01 CreateTileSquare310x310TextList01()
        {
            return new TileSquare310x310TextList01();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310TextList02 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310TextList02 template.</returns>
        public static ITileSquare310x310TextList02 CreateTileSquare310x310TextList02()
        {
            return new TileSquare310x310TextList02();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare310x310TextList03 template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare310x310TextList03 template.</returns>
        public static ITileSquare310x310TextList03 CreateTileSquare310x310TextList03()
        {
            return new TileSquare310x310TextList03();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare99x99IconWithBadge template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare99x99IconWithBadge template.</returns>
        public static ITileSquare99x99IconWithBadge CreateTileSquare99x99IconWithBadge()
        {
            return new TileSquare99x99IconWithBadge();
        }

        /// <summary>
        /// Creates a tile content object for the TileSquare210x210IconWithBadge template.
        /// </summary>
        /// <returns>A tile content object for the TileSquare210x210IconWithBadge template.</returns>
        public static ITileSquare210x210IconWithBadge CreateTileSquare210x210IconWithBadge()
        {
            return new TileSquare210x210IconWithBadge();
        }

        /// <summary>
        /// Creates a tile content object for the TileWide432x210IconWithBadgeAndText template.
        /// </summary>
        /// <returns>A tile content object for the TileWide432x210IconWithBadgeAndText template.</returns>
        public static ITileWide432x210IconWithBadgeAndText CreateTileWide432x210IconWithBadgeAndText()
        {
            return new TileWide432x210IconWithBadgeAndText();
        }
    }
}
