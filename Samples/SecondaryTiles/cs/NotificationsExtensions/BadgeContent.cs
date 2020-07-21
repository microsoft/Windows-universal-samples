// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

using System;
#if !WINRT_NOT_PRESENT
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;
#endif

namespace NotificationsExtensions.BadgeContent
{
    /// <summary>
    /// Notification content object to display a glyph on a tile's badge.
    /// </summary>
    public sealed class BadgeGlyphNotificationContent : IBadgeNotificationContent
    {
        /// <summary>
        /// Default constructor to create a glyph badge content object.
        /// </summary>
        public BadgeGlyphNotificationContent()
        {
        }

        /// <summary>
        /// Constructor to create a glyph badge content object with a glyph.
        /// </summary>
        /// <param name="glyph">The glyph to be displayed on the badge.</param>
        public BadgeGlyphNotificationContent(GlyphValue glyph)
        {
            m_Glyph = glyph;
        }

        /// <summary>
        /// The glyph to be displayed on the badge.
        /// </summary>
        public GlyphValue Glyph
        {
            get { return m_Glyph; }
            set
            {
                if (!Enum.IsDefined(typeof(GlyphValue), value))
                {
                    throw new ArgumentOutOfRangeException("value");
                }
                m_Glyph = value;
            }
        }

        /// <summary>
        /// Retrieves the notification Xml content as a string.
        /// </summary>
        /// <returns>The notification Xml content as a string.</returns>
        public string GetContent()
        {
            if (!Enum.IsDefined(typeof(GlyphValue), m_Glyph))
            {
                throw new NotificationContentValidationException("The badge glyph property was left unset.");
            }

            string glyphString = m_Glyph.ToString();
            // lower case the first character of the enum value to match the Xml schema
            glyphString = String.Format("{0}{1}", Char.ToLowerInvariant(glyphString[0]), glyphString.Substring(1));
            return String.Format("<badge version='{0}' value='{1}'/>", Util.NOTIFICATION_CONTENT_VERSION, glyphString);
        }

        /// <summary>
        /// Retrieves the notification Xml content as a string.
        /// </summary>
        /// <returns>The notification Xml content as a string.</returns>
        public override string ToString()
        {
            return GetContent();
        }
        
#if !WINRT_NOT_PRESENT
        /// <summary>
        /// Retrieves the notification Xml content as a WinRT Xml document.
        /// </summary>
        /// <returns>The notification Xml content as a WinRT Xml document.</returns>
        public XmlDocument GetXml()
        {
            XmlDocument xml = new XmlDocument();
            xml.LoadXml(GetContent());
            return xml;
        }

        /// <summary>
        /// Creates a WinRT BadgeNotification object based on the content.
        /// </summary>
        /// <returns>A WinRT BadgeNotification object based on the content.</returns>
        public BadgeNotification CreateNotification()
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.LoadXml(GetContent());
            return new BadgeNotification(xmlDoc);
        }
#endif

        private GlyphValue m_Glyph = (GlyphValue)(-1);
    }

    /// <summary>
    /// Notification content object to display a number on a tile's badge.
    /// </summary>
    public sealed class BadgeNumericNotificationContent : IBadgeNotificationContent
    {
        /// <summary>
        /// Default constructor to create a numeric badge content object.
        /// </summary>
        public BadgeNumericNotificationContent()
        {
        }

        /// <summary>
        /// Constructor to create a numeric badge content object with a number.
        /// </summary>
        /// <param name="number">
        /// The number that will appear on the badge.  If the number is 0, the badge
        /// will be removed.  The largest value that will appear on the badge is 99.
        /// Numbers greater than 99 are allowed, but will be displayed as "99+".
        /// </param>
        public BadgeNumericNotificationContent(uint number)
        {
            m_Number = number;
        }

        /// <summary>
        /// The number that will appear on the badge.  If the number is 0, the badge
        /// will be removed.  The largest value that will appear on the badge is 99.
        /// Numbers greater than 99 are allowed, but will be displayed as "99+".
        /// </summary>
        public uint Number
        {
            get { return m_Number; }
            set { m_Number = value; }
        }

        /// <summary>
        /// Retrieves the notification Xml content as a string.
        /// </summary>
        /// <returns>The notification Xml content as a string.</returns>
        public string GetContent()
        {
            return String.Format("<badge version='{0}' value='{1}'/>", Util.NOTIFICATION_CONTENT_VERSION, m_Number);
        }

        /// <summary>
        /// Retrieves the notification Xml content as a string.
        /// </summary>
        /// <returns>The notification Xml content as a string.</returns>
        public override string ToString()
        {
            return GetContent();
        }

#if !WINRT_NOT_PRESENT
        /// <summary>
        /// Retrieves the notification Xml content as a WinRT Xml document.
        /// </summary>
        /// <returns>The notification Xml content as a WinRT Xml document.</returns>
        public XmlDocument GetXml()
        {
            XmlDocument xml = new XmlDocument();
            xml.LoadXml(GetContent());
            return xml;
        }

        /// <summary>
        /// Creates a WinRT BadgeNotification object based on the content.
        /// </summary>
        /// <returns>A WinRT BadgeNotification object based on the content.</returns>
        public BadgeNotification CreateNotification()
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.LoadXml(GetContent());
            return new BadgeNotification(xmlDoc);
        }
#endif

        private uint m_Number = 0;
    }
}
