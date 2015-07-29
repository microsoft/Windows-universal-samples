// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

using System;
using System.Collections.ObjectModel;
using System.Runtime.InteropServices;
using System.Text;
#if !WINRT_NOT_PRESENT
using Windows.Data.Xml.Dom;
#endif

namespace NotificationsExtensions
{
    internal sealed class NotificationContentText : INotificationContentText
    {
        internal NotificationContentText() { }

        public string Text
        {
            get { return m_Text; }
            set { m_Text = value; }
        }

        public string Lang
        {
            get { return m_Lang; }
            set { m_Lang = value; }
        }

        private string m_Text;
        private string m_Lang;
    }

    internal sealed class NotificationContentImage : INotificationContentImage
    {
        internal NotificationContentImage() { }

        public string Src
        {
            get { return m_Src; }
            set { m_Src = value; }
        }

        public string Alt
        {
            get { return m_Alt; }
            set { m_Alt = value; }
        }

        public bool AddImageQuery
        {
            get
            {
                if (m_AddImageQueryNullable == null || m_AddImageQueryNullable.Value == false)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            set { m_AddImageQueryNullable = value; }
        }

        public bool? AddImageQueryNullable
        {
            get { return m_AddImageQueryNullable; }
            set { m_AddImageQueryNullable = value; }
        }

        private string m_Src;
        private string m_Alt;
        private bool? m_AddImageQueryNullable;
    }

    internal static class Util
    {
        public const int NOTIFICATION_CONTENT_VERSION = 1;

        public static string HttpEncode(string value)
        {
            return value.Replace("&", "&amp;").Replace("<", "&lt;").Replace(">", "&gt;").Replace("\"", "&quot;").Replace("'", "&apos;");
        }
    }

    /// <summary>
    /// Base class for the notification content creation helper classes.
    /// </summary>
#if !WINRT_NOT_PRESENT
    internal abstract class NotificationBase
#else
    abstract partial class NotificationBase
#endif
    {
        protected NotificationBase(string templateName, string fallbackName, int imageCount, int textCount)
        {
            m_TemplateName = templateName;
            m_FallbackName = fallbackName;

            m_Images = new NotificationContentImage[imageCount];
            for (int i = 0; i < m_Images.Length; i++)
            {
                m_Images[i] = new NotificationContentImage();
            }

            m_TextFields = new INotificationContentText[textCount];
            for (int i = 0; i < m_TextFields.Length; i++)
            {
                m_TextFields[i] = new NotificationContentText();
            }
        }

        public bool StrictValidation
        {
            get { return m_StrictValidation; }
            set { m_StrictValidation = value; }
        }
        public abstract string GetContent();

        public override string ToString()
        {
            return GetContent();
        }

#if !WINRT_NOT_PRESENT
        public XmlDocument GetXml()
        {
            XmlDocument xml = new XmlDocument();
            xml.LoadXml(GetContent());
            return xml;
        }
#endif

        /// <summary>
        /// Retrieves the list of images that can be manipulated on the notification content.
        /// </summary>
        public INotificationContentImage[] Images
        {
            get { return m_Images; }
        }

        /// <summary>
        /// Retrieves the list of text fields that can be manipulated on the notification content.
        /// </summary>
        public INotificationContentText[] TextFields
        {
            get { return m_TextFields; }
        }

        /// <summary>
        /// The base Uri path that should be used for all image references in the notification.
        /// </summary>
        public string BaseUri
        {
            get { return m_BaseUri; }
            set
            {
                if (this.StrictValidation && !String.IsNullOrEmpty(value))
                {
                    Uri uri;
                    try
                    {
                        uri = new Uri(value);
                    }
                    catch (Exception e)
                    {
                        throw new ArgumentException("Invalid URI. Use a valid URI or turn off StrictValidation", e);
                    }
                    if (!(uri.Scheme.Equals("http", StringComparison.OrdinalIgnoreCase)
                        || uri.Scheme.Equals("https", StringComparison.OrdinalIgnoreCase)
                        || uri.Scheme.Equals("ms-appx", StringComparison.OrdinalIgnoreCase)
                        || (uri.Scheme.Equals("ms-appdata", StringComparison.OrdinalIgnoreCase)
                            && (String.IsNullOrEmpty(uri.Authority)) // check to make sure the Uri isn't ms-appdata://foo/local
                            && (uri.AbsolutePath.StartsWith("/local/") || uri.AbsolutePath.StartsWith("local/"))))) // both ms-appdata:local/ and ms-appdata:/local/ are valid
                    {
                        throw new ArgumentException("The BaseUri must begin with http://, https://, ms-appx:///, or ms-appdata:///local/.", "value");
                    }
                }
                m_BaseUri = value;
            }
        }

        public string Lang
        {
            get { return m_Lang; }
            set { m_Lang = value; }
        }

        public bool AddImageQuery
        {
            get
            {
                if (m_AddImageQueryNullable == null || m_AddImageQueryNullable.Value == false)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            set { m_AddImageQueryNullable = value; }
        }

        public bool? AddImageQueryNullable
        {
            get { return m_AddImageQueryNullable; }
            set { m_AddImageQueryNullable = value; }
        }

        protected string SerializeProperties(string globalLang, string globalBaseUri, bool globalAddImageQuery)
        {
            globalLang = (globalLang != null) ? globalLang : String.Empty;
            globalBaseUri = String.IsNullOrWhiteSpace(globalBaseUri) ? null : globalBaseUri;

            StringBuilder builder = new StringBuilder(String.Empty);
            for (int i = 0; i < m_Images.Length; i++)
            {
                if (!String.IsNullOrEmpty(m_Images[i].Src))
                {
                    string escapedSrc = Util.HttpEncode(m_Images[i].Src);
                    if (!String.IsNullOrWhiteSpace(m_Images[i].Alt))
                    {
                        string escapedAlt = Util.HttpEncode(m_Images[i].Alt);
                        if (m_Images[i].AddImageQueryNullable == null || m_Images[i].AddImageQueryNullable == globalAddImageQuery)
                        {
                            builder.AppendFormat("<image id='{0}' src='{1}' alt='{2}'/>", i + 1, escapedSrc, escapedAlt);
                        }
                        else
                        {
                            builder.AppendFormat("<image addImageQuery='{0}' id='{1}' src='{2}' alt='{3}'/>", m_Images[i].AddImageQuery.ToString().ToLowerInvariant(), i + 1, escapedSrc, escapedAlt);
                        }
                    }
                    else
                    {
                        if (m_Images[i].AddImageQueryNullable == null || m_Images[i].AddImageQueryNullable == globalAddImageQuery)
                        {
                            builder.AppendFormat("<image id='{0}' src='{1}'/>", i + 1, escapedSrc);
                        }
                        else
                        {
                            builder.AppendFormat("<image addImageQuery='{0}' id='{1}' src='{2}'/>", m_Images[i].AddImageQuery.ToString().ToLowerInvariant(), i + 1, escapedSrc);
                        }
                    }
                }
            }

            for (int i = 0; i < m_TextFields.Length; i++)
            {
                if (!String.IsNullOrWhiteSpace(m_TextFields[i].Text))
                {
                    string escapedValue = Util.HttpEncode(m_TextFields[i].Text);
                    if (!String.IsNullOrWhiteSpace(m_TextFields[i].Lang) && !m_TextFields[i].Lang.Equals(globalLang))
                    {
                        string escapedLang = Util.HttpEncode(m_TextFields[i].Lang);
                        builder.AppendFormat("<text id='{0}' lang='{1}'>{2}</text>", i + 1, escapedLang, escapedValue);
                    }
                    else
                    {
                        builder.AppendFormat("<text id='{0}'>{1}</text>", i + 1, escapedValue);
                    }
                }
            }

            return builder.ToString();
        }

        public string TemplateName { get { return m_TemplateName; } }
        public string FallbackName { get { return m_FallbackName; } }

        private bool m_StrictValidation = true;
        private NotificationContentImage[] m_Images;
        private INotificationContentText[] m_TextFields;

        private string m_Lang;
        private string m_BaseUri;
        private string m_TemplateName;
        private string m_FallbackName;
        private bool? m_AddImageQueryNullable;
    }

    /// <summary>
    /// Exception returned when invalid notification content is provided.
    /// </summary>
    internal sealed class NotificationContentValidationException : COMException
    {
        public NotificationContentValidationException(string message)
            : base(message, unchecked((int)0x80070057))
        {
        }
    }
}