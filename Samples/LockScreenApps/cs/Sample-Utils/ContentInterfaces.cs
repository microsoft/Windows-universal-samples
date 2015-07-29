// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#if !WINRT_NOT_PRESENT
using Windows.UI.Notifications;
using Windows.Data.Xml.Dom;
#endif

namespace NotificationsExtensions
{
    /// <summary>
    /// Base notification content interface to retrieve notification Xml as a string.
    /// </summary>
    public interface INotificationContent
    {
        /// <summary>
        /// Retrieves the notification Xml content as a string.
        /// </summary>
        /// <returns>The notification Xml content as a string.</returns>
        string GetContent();

#if !WINRT_NOT_PRESENT
        /// <summary>
        /// Retrieves the notification Xml content as a WinRT Xml document.
        /// </summary>
        /// <returns>The notification Xml content as a WinRT Xml document.</returns>
        XmlDocument GetXml();
#endif
    }

    /// <summary>
    /// A type contained by the tile and toast notification content objects that
    /// represents a text field in the template.
    /// </summary>
    public interface INotificationContentText
    {
        /// <summary>
        /// The text value that will be shown in the text field.
        /// </summary>
        string Text { get; set; }

        /// <summary>
        /// The language of the text field.  This proprety overrides the language provided in the
        /// containing notification object.  The language should be specified using the
        /// abbreviated language code as defined by BCP 47.
        /// </summary>
        string Lang { get; set; }
    }

    /// <summary>
    /// A type contained by the tile and toast notification content objects that
    /// represents an image in a template.
    /// </summary>
    public interface INotificationContentImage
    {
        /// <summary>
        /// The location of the image.  Relative image paths use the BaseUri provided in the containing
        /// notification object.  If no BaseUri is provided, paths are relative to ms-appx:///.
        /// Only png and jpg images are supported.  Images must be 1024x1024 pixels or less, and smaller than
        /// 200 kB in size.
        /// </summary>
        string Src { get; set; }

        /// <summary>
        /// Alt text that describes the image.
        /// </summary>
        string Alt { get; set; }

        /// <summary>
        /// Controls if query strings that denote the client configuration of contrast, scale, and language setting should be appended to the Src
        /// If true, Windows will append query strings onto the Src
        /// If false, Windows will not append query strings onto the Src
        /// Query string details:
        /// Parameter: ms-contrast
        ///     Values: standard, black, white
        /// Parameter: ms-scale
        ///     Values: 80, 100, 140, 180
        /// Parameter: ms-lang
        ///     Values: The BCP 47 language tag set in the notification xml, or if omitted, the current preferred language of the user
        /// </summary>
        bool AddImageQuery { get; set; }
    }

    namespace TileContent
    {
        /// <summary>
        /// Base tile notification content interface.
        /// </summary>
        public interface ITileNotificationContent : INotificationContent
        {
            /// <summary>
            /// Whether strict validation should be applied when the Xml or notification object is created,
            /// and when some of the properties are assigned.
            /// </summary>
            bool StrictValidation { get; set; }

            /// <summary>
            /// The language of the content being displayed.  The language should be specified using the
            /// abbreviated language code as defined by BCP 47.
            /// </summary>
            string Lang { get; set; }

            /// <summary>
            /// The BaseUri that should be used for image locations.  Relative image locations use this
            /// field as their base Uri.  The BaseUri must begin with http://, https://, ms-appx:///, or
            /// ms-appdata:///local/.
            /// </summary>
            string BaseUri { get; set; }

            /// <summary>
            /// Determines the application branding when tile notification content is displayed on the tile.
            /// </summary>
            TileBranding Branding { get; set; }

            /// <summary>
            /// Controls if query strings that denote the client configuration of contrast, scale, and language setting should be appended to the Src
            /// If true, Windows will append query strings onto images that exist in this template
            /// If false (the default), Windows will not append query strings onto images that exist in this template
            /// Query string details:
            /// Parameter: ms-contrast
            ///     Values: standard, black, white
            /// Parameter: ms-scale
            ///     Values: 80, 100, 140, 180
            /// Parameter: ms-lang
            ///     Values: The BCP 47 language tag set in the notification xml, or if omitted, the current preferred language of the user
            /// </summary>
            bool AddImageQuery { get; set; }

            /// <summary>
            /// Used by the system to do semantic deduplication of content with the same contentId.
            /// </summary>
            string ContentId { get; set; }

#if !WINRT_NOT_PRESENT
            /// <summary>
            /// Creates a WinRT TileNotification object based on the content.
            /// </summary>
            /// <returns>The WinRT TileNotification object</returns>
            TileNotification CreateNotification();
#endif
        }

        /// <summary>
        /// Base square tile notification content interface.
        /// </summary>
        public interface ISquare150x150TileNotificationContent : ITileNotificationContent
        {
        }

        /// <summary>
        /// Base wide tile notification content interface.
        /// </summary>
        public interface IWide310x150TileNotificationContent : ITileNotificationContent
        {
            /// <summary>
            /// Corresponding square tile notification content should be a part of every wide tile notification.
            /// </summary>
            ISquare150x150TileNotificationContent Square150x150Content { get; set; }

            /// <summary>
            /// Whether square tile notification content needs to be added to pass
            /// validation.  Square150x150 content is required by default.
            /// </summary>
            bool RequireSquare150x150Content { get; set; }
        }

        /// <summary>
        /// Base large tile notification content interface.
        /// </summary>
        public interface ISquare310x310TileNotificationContent : ITileNotificationContent
        {
            /// <summary>
            /// Corresponding wide tile notification content should be a part of every large tile notification.
            /// </summary>
            IWide310x150TileNotificationContent Wide310x150Content { get; set; }

            /// <summary>
            /// Whether wide tile notification content needs to be added to pass
            /// validation.  Wide310x150 content is required by default.
            /// </summary>
            bool RequireWide310x150Content { get; set; }
        }

        /// <summary>
        /// Base Windows Phone small tile notification content interface.
        /// </summary>
        public interface ISquare99x99TileNotificationContent : ITileNotificationContent
        {
        }

        /// <summary>
        /// Base Windows Phone medium tile notification content interface.
        /// </summary>
        public interface ISquare210x210TileNotificationContent : ITileNotificationContent
        {
            /// <summary>
            /// Corresponding small tile notification content should be a part of every medium tile notification.
            /// </summary>
            ISquare99x99TileNotificationContent Square99x99Content { get; set; }

            /// <summary>
            /// Whether small tile notification content needs to be added to pass
            /// validation.  Square99x99 content is required by default.
            /// </summary>
            bool RequireSquare99x99Content { get; set; }
        }

        /// <summary>
        /// Base Windows Phone large tile notification content interface.
        /// </summary>
        public interface IWide432x210TileNotificationContent : ITileNotificationContent
        {
            /// <summary>
            /// Corresponding medium tile notification content should be a part of every large tile notification.
            /// </summary>
            ISquare210x210TileNotificationContent Square210x210Content { get; set; }

            /// <summary>
            /// Whether medium tile notification content needs to be added to pass
            /// validation.  Square210x210 content is required by default.
            /// </summary>
            bool RequireSquare210x210Content { get; set; }
        }

        /// <summary>
        /// A square tile template that displays two text captions.
        /// </summary>
        public interface ITileSquare150x150Block : ISquare150x150TileNotificationContent
        {
            /// <summary>
            /// A large block text field.
            /// </summary>
            INotificationContentText TextBlock { get; }

            /// <summary>
            /// The description under the large block text field.
            /// </summary>
            INotificationContentText TextSubBlock { get; }
        }

        /// <summary>
        /// A square tile template that displays an image.
        /// </summary>
        public interface ITileSquare150x150Image : ISquare150x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }
        }

        /// <summary>
        /// A square tile template that displays an image, then transitions to show
        /// four text fields.
        /// </summary>
        public interface ITileSquare150x150PeekImageAndText01 : ISquare150x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }
        }

        /// <summary>
        /// A square tile template that displays an image, then transitions to show
        /// two text fields.
        /// </summary>
        public interface ITileSquare150x150PeekImageAndText02 : ISquare150x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A square tile template that displays an image, then transitions to show
        /// four text fields.
        /// </summary>
        public interface ITileSquare150x150PeekImageAndText03 : ISquare150x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }
            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }
        }

        /// <summary>
        /// A square tile template that displays an image, then transitions to
        /// show a text field.
        /// </summary>
        public interface ITileSquare150x150PeekImageAndText04 : ISquare150x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A square tile template that displays four text fields.
        /// </summary>
        public interface ITileSquare150x150Text01 : ISquare150x150TileNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }
        }

        /// <summary>
        /// A square tile template that displays two text fields.
        /// </summary>
        public interface ITileSquare150x150Text02 : ISquare150x150TileNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A square tile template that displays four text fields.
        /// </summary>
        public interface ITileSquare150x150Text03 : ISquare150x150TileNotificationContent
        {
            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }
        }

        /// <summary>
        /// A square tile template that displays a text field.
        /// </summary>
        public interface ITileSquare150x150Text04 : ISquare150x150TileNotificationContent
        {
            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A square tile template that displays six text fields.
        /// </summary>
        public interface ITileWide310x150BlockAndText01 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }

            /// <summary>
            /// A large block text field.
            /// </summary>
            INotificationContentText TextBlock { get; }

            /// <summary>
            /// The description under the large block text field.
            /// </summary>
            INotificationContentText TextSubBlock { get; }
        }

        /// <summary>
        /// A square tile template that displays three text fields.
        /// </summary>
        public interface ITileWide310x150BlockAndText02 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }

            /// <summary>
            /// A large block text field.
            /// </summary>
            INotificationContentText TextBlock { get; }

            /// <summary>
            /// The description under the large block text field.
            /// </summary>
            INotificationContentText TextSubBlock { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image.
        /// </summary>
        public interface ITileWide310x150Image : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image and a text caption.
        /// </summary>
        public interface ITileWide310x150ImageAndText01 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A caption for the image.
            /// </summary>
            INotificationContentText TextCaptionWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image and two text captions.
        /// </summary>
        public interface ITileWide310x150ImageAndText02 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// The first caption for the image.
            /// </summary>
            INotificationContentText TextCaption1 { get; }

            /// <summary>
            /// The second caption for the image.
            /// </summary>
            INotificationContentText TextCaption2 { get; }
        }

        /// <summary>
        /// A wide tile template that displays five images - one main image,
        /// and four square images in a grid.
        /// </summary>
        public interface ITileWide310x150ImageCollection : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage ImageMain { get; }

            /// <summary>
            /// A small square image on the tile.
            /// </summary>
            INotificationContentImage ImageSmallColumn1Row1 { get; }

            /// <summary>
            /// A small square image on the tile.
            /// </summary>
            INotificationContentImage ImageSmallColumn2Row1 { get; }

            /// <summary>
            /// A small square image on the tile.
            /// </summary>
            INotificationContentImage ImageSmallColumn1Row2 { get; }

            /// <summary>
            /// A small square image on the tile.
            /// </summary>
            INotificationContentImage ImageSmallColumn2Row2 { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image, then transitions to show
        /// two text fields.
        /// </summary>
        public interface ITileWide310x150PeekImage01 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image, then transitions to show
        /// five text fields.
        /// </summary>
        public interface ITileWide310x150PeekImage02 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image, then transitions to show
        /// a text field.
        /// </summary>
        public interface ITileWide310x150PeekImage03 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image, then transitions to show
        /// a text field.
        /// </summary>
        public interface ITileWide310x150PeekImage04 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image, then transitions to show
        /// another image and two text fields.
        /// </summary>
        public interface ITileWide310x150PeekImage05 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage ImageMain { get; }

            /// <summary>
            /// The secondary image on the tile.
            /// </summary>
            INotificationContentImage ImageSecondary { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image, then transitions to show
        /// another image and a text field.
        /// </summary>
        public interface ITileWide310x150PeekImage06 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage ImageMain { get; }

            /// <summary>
            /// The secondary image on the tile.
            /// </summary>
            INotificationContentImage ImageSecondary { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image and a portion of a text field,
        /// then transitions to show all of the text field.
        /// </summary>
        public interface ITileWide310x150PeekImageAndText01 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image and a text field,
        /// then transitions to show the text field and four other text fields.
        /// </summary>
        public interface ITileWide310x150PeekImageAndText02 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody5 { get; }
        }

        /// <summary>
        /// A wide tile template that displays five images - one main image,
        /// and four square images in a grid, then transitions to show two
        /// text fields.
        /// </summary>
        public interface ITileWide310x150PeekImageCollection01 : ITileWide310x150ImageCollection
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays five images - one main image,
        /// and four square images in a grid, then transitions to show five
        /// text fields.
        /// </summary>
        public interface ITileWide310x150PeekImageCollection02 : ITileWide310x150ImageCollection
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }
        }

        /// <summary>
        /// A wide tile template that displays five images - one main image,
        /// and four square images in a grid, then transitions to show a
        /// text field.
        /// </summary>
        public interface ITileWide310x150PeekImageCollection03 : ITileWide310x150ImageCollection
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays five images - one main image,
        /// and four square images in a grid, then transitions to show a
        /// text field.
        /// </summary>
        public interface ITileWide310x150PeekImageCollection04 : ITileWide310x150ImageCollection
        {
            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays five images - one main image,
        /// and four square images in a grid, then transitions to show an image
        /// and two text fields.
        /// </summary>
        public interface ITileWide310x150PeekImageCollection05 : ITileWide310x150ImageCollection
        {
            /// <summary>
            /// The secondary image on the tile.
            /// </summary>
            INotificationContentImage ImageSecondary { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays five images - one main image,
        /// and four square images in a grid, then transitions to show an image
        /// and a text field.
        /// </summary>
        public interface ITileWide310x150PeekImageCollection06 : ITileWide310x150ImageCollection
        {
            /// <summary>
            /// The secondary image on the tile.
            /// </summary>
            INotificationContentImage ImageSecondary { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image and a text field.
        /// </summary>
        public interface ITileWide310x150SmallImageAndText01 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image and 5 text fields.
        /// </summary>
        public interface ITileWide310x150SmallImageAndText02 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image and a text field.
        /// </summary>
        public interface ITileWide310x150SmallImageAndText03 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays an image and two text fields.
        /// </summary>
        public interface ITileWide310x150SmallImageAndText04 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays a poster image and two text fields.
        /// </summary>
        public interface ITileWide310x150SmallImageAndText05 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays five text fields.
        /// </summary>
        public interface ITileWide310x150Text01 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }
        }

        /// <summary>
        /// A wide tile template that displays nine text fields - a heading and two columns
        /// of four text fields.
        /// </summary>
        public interface ITileWide310x150Text02 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row4 { get; }
        }

        /// <summary>
        /// A wide tile template that displays a text field.
        /// </summary>
        public interface ITileWide310x150Text03 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays a text field.
        /// </summary>
        public interface ITileWide310x150Text04 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays five text fields.
        /// </summary>
        public interface ITileWide310x150Text05 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody5 { get; }
        }

        /// <summary>
        /// A wide tile template that displays ten text fields - two columns
        /// of five text fields.
        /// </summary>
        public interface ITileWide310x150Text06 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row5 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row5 { get; }
        }

        /// <summary>
        /// A wide tile template that displays nine text fields - a heading and two columns
        /// of four text fields.
        /// </summary>
        public interface ITileWide310x150Text07 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextShortColumn1Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextShortColumn1Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextShortColumn1Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextShortColumn1Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row4 { get; }
        }

        /// <summary>
        /// A wide tile template that displays ten text fields - two columns
        /// of five text fields.
        /// </summary>
        public interface ITileWide310x150Text08 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextShortColumn1Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextShortColumn1Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextShortColumn1Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextShortColumn1Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextShortColumn1Row5 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row5 { get; }
        }

        /// <summary>
        /// A wide tile template that displays two text fields.
        /// </summary>
        public interface ITileWide310x150Text09 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A wide tile template that displays nine text fields - a heading and two columns
        /// of four text fields.
        /// </summary>
        public interface ITileWide310x150Text10 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextPrefixColumn1Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextPrefixColumn1Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextPrefixColumn1Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextPrefixColumn1Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row4 { get; }
        }

        /// <summary>
        /// A wide tile template that displays ten text fields - two columns
        /// of five text fields.
        /// </summary>
        public interface ITileWide310x150Text11 : IWide310x150TileNotificationContent
        {
            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextPrefixColumn1Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextPrefixColumn1Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextPrefixColumn1Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextPrefixColumn1Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextPrefixColumn1Row5 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row5 { get; }
        }

        /// <summary>
        /// A large tile template that displays a heading,
        /// six text fields grouped into three units,
        /// and two more text fields.
        /// </summary>
        public interface ITileSquare310x310BlockAndText01 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody5 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody6 { get; }

            /// <summary>
            /// A large block text field.
            /// </summary>
            INotificationContentText TextBlock { get; }

            /// <summary>
            /// The description under the large block text field.
            /// </summary>
            INotificationContentText TextSubBlock { get; }
        }

        /// <summary>
        /// A large tile template that displays seven text fields with one background image.
        /// </summary>
        public interface ITileSquare310x310BlockAndText02 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The background image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A large block text field.
            /// </summary>
            INotificationContentText TextBlock { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading1 { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }
        }

        /// <summary>
        /// A large square tile template that displays an image.
        /// </summary>
        public interface ITileSquare310x310Image : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }
        }

        /// <summary>
        /// A large square tile template that displays an image and a text caption.
        /// </summary>
        public interface ITileSquare310x310ImageAndText01 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A caption for the image.
            /// </summary>
            INotificationContentText TextCaptionWrap { get; }
        }

        /// <summary>
        /// A large square tile template that displays an image and two text captions.
        /// </summary>
        public interface ITileSquare310x310ImageAndText02 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// The first caption for the image.
            /// </summary>
            INotificationContentText TextCaption1 { get; }

            /// <summary>
            /// The second caption for the image.
            /// </summary>
            INotificationContentText TextCaption2 { get; }
        }

        /// <summary>
        /// A large square tile template that displays a heading with a background image.
        /// </summary>
        public interface ITileSquare310x310ImageAndTextOverlay01 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A caption for the image.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }
        }

        /// <summary>
        /// A large square tile template that displays a heading and a text field with a background image.
        /// </summary>
        public interface ITileSquare310x310ImageAndTextOverlay02 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A caption for the image.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A large square tile template that displays a heading and three text fields with a background image.
        /// </summary>
        public interface ITileSquare310x310ImageAndTextOverlay03 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A caption for the image.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }
        }

        /// <summary>
        /// A large square tile template that displays five images - one main image,
        /// and four smaller images in a row across the top.
        /// </summary>
        public interface ITileSquare310x310ImageCollection : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage ImageMain { get; }

            /// <summary>
            /// A small image on the tile.
            /// </summary>
            INotificationContentImage ImageSmall1 { get; }

            /// <summary>
            /// A small square image on the tile.
            /// </summary>
            INotificationContentImage ImageSmall2 { get; }

            /// <summary>
            /// A small square image on the tile.
            /// </summary>
            INotificationContentImage ImageSmall3 { get; }

            /// <summary>
            /// A small image on the tile.
            /// </summary>
            INotificationContentImage ImageSmall4 { get; }
        }

        /// <summary>
        /// A large square tile template that displays five images - one main image,
        /// four smaller images in a row across the top, and a text caption.
        /// </summary>
        public interface ITileSquare310x310ImageCollectionAndText01 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage ImageMain { get; }

            /// <summary>
            /// A small image on the tile.
            /// </summary>
            INotificationContentImage ImageSmall1 { get; }

            /// <summary>
            /// A small square image on the tile.
            /// </summary>
            INotificationContentImage ImageSmall2 { get; }

            /// <summary>
            /// A small square image on the tile.
            /// </summary>
            INotificationContentImage ImageSmall3 { get; }

            /// <summary>
            /// A small image on the tile.
            /// </summary>
            INotificationContentImage ImageSmall4 { get; }

            /// <summary>
            /// A caption for the image.
            /// </summary>
            INotificationContentText TextCaptionWrap { get; }
        }

        /// <summary>
        /// A large square tile template that displays five images - one main image,
        /// four smaller images in a row across the top, and two text captions.
        /// </summary>
        public interface ITileSquare310x310ImageCollectionAndText02 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The main image on the tile.
            /// </summary>
            INotificationContentImage ImageMain { get; }

            /// <summary>
            /// A small image on the tile.
            /// </summary>
            INotificationContentImage ImageSmall1 { get; }

            /// <summary>
            /// A small square image on the tile.
            /// </summary>
            INotificationContentImage ImageSmall2 { get; }

            /// <summary>
            /// A small square image on the tile.
            /// </summary>
            INotificationContentImage ImageSmall3 { get; }

            /// <summary>
            /// A small image on the tile.
            /// </summary>
            INotificationContentImage ImageSmall4 { get; }

            /// <summary>
            /// A caption for the image.
            /// </summary>
            INotificationContentText TextCaption1 { get; }

            /// <summary>
            /// A caption for the image.
            /// </summary>
            INotificationContentText TextCaption2 { get; }
        }

        /// <summary>
        /// A large square tile template that displays an image, a headline, a text field that can wrap to two lines,
        /// and a text field.
        /// </summary>
        public interface ITileSquare310x310SmallImageAndText01 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The image at the top of the tile.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// The headline text.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// The middle text field that wraps to two lines.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }

            /// <summary>
            /// The lower text field.
            /// </summary>
            INotificationContentText TextBody { get; }
        }

        /// <summary>
        /// A large square tile template that displays a list of three groups, each group having an image,
        /// a heading, and two text fields.
        /// </summary>
        public interface ITileSquare310x310SmallImagesAndTextList01 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The image for the first group in the list.
            /// </summary>
            INotificationContentImage Image1 { get; }

            /// <summary>
            /// The heading for the first group in the list.
            /// </summary>
            INotificationContentText TextHeading1 { get; }

            /// <summary>
            /// The first text field for the first group in the list.
            /// </summary>
            INotificationContentText TextBodyGroup1Field1 { get; }

            /// <summary>
            /// The second text field for the first group in the list.
            /// </summary>
            INotificationContentText TextBodyGroup1Field2 { get; }

            /// <summary>
            /// The image for the second group in the list.
            /// </summary>
            INotificationContentImage Image2 { get; }

            /// <summary>
            /// The heading for the second group in the list.
            /// </summary>
            INotificationContentText TextHeading2 { get; }

            /// <summary>
            /// The first text field for the second group in the list.
            /// </summary>
            INotificationContentText TextBodyGroup2Field1 { get; }

            /// <summary>
            /// The second text field for the second group in the list.
            /// </summary>
            INotificationContentText TextBodyGroup2Field2 { get; }

            /// <summary>
            /// The image for the third group in the list.
            /// </summary>
            INotificationContentImage Image3 { get; }

            /// <summary>
            /// The heading for the third group in the list.
            /// </summary>
            INotificationContentText TextHeading3 { get; }

            /// <summary>
            /// The first text field for the third group in the list.
            /// </summary>
            INotificationContentText TextBodyGroup3Field1 { get; }

            /// <summary>
            /// The second text field for the third group in the list.
            /// </summary>
            INotificationContentText TextBodyGroup3Field2 { get; }
        }

        /// <summary>
        /// A large square tile template that displays a list of three groups, each group having an image,
        /// and a text field that can wrap to a total of three lines.
        /// </summary>
        public interface ITileSquare310x310SmallImagesAndTextList02 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The image for the first group in the list.
            /// </summary>
            INotificationContentImage Image1 { get; }

            /// <summary>
            /// The text field for the first group in the list.
            /// </summary>
            INotificationContentText TextWrap1 { get; }

            /// <summary>
            /// The image for the second group in the list.
            /// </summary>
            INotificationContentImage Image2 { get; }

            /// <summary>
            /// The text field for the second group in the list.
            /// </summary>
            INotificationContentText TextWrap2 { get; }

            /// <summary>
            /// The image for the third group in the list.
            /// </summary>
            INotificationContentImage Image3 { get; }

            /// <summary>
            /// The  text field for the third group in the list.
            /// </summary>
            INotificationContentText TextWrap3 { get; }
        }

        /// <summary>
        /// A large square tile template that displays a list of three groups, each group having an image,
        /// a heading, and one text field, which wraps to two lines.
        /// </summary>
        public interface ITileSquare310x310SmallImagesAndTextList03 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The image for the first group in the list.
            /// </summary>
            INotificationContentImage Image1 { get; }

            /// <summary>
            /// The heading for the first group in the list.
            /// </summary>
            INotificationContentText TextHeading1 { get; }

            /// <summary>
            /// The first text field for the first group in the list.
            /// </summary>
            INotificationContentText TextWrap1 { get; }

            /// <summary>
            /// The image for the second group in the list.
            /// </summary>
            INotificationContentImage Image2 { get; }

            /// <summary>
            /// The heading for the second group in the list.
            /// </summary>
            INotificationContentText TextHeading2 { get; }

            /// <summary>
            /// The first text field for the second group in the list.
            /// </summary>
            INotificationContentText TextWrap2 { get; }

            /// <summary>
            /// The image for the third group in the list.
            /// </summary>
            INotificationContentImage Image3 { get; }

            /// <summary>
            /// The heading for the third group in the list.
            /// </summary>
            INotificationContentText TextHeading3 { get; }

            /// <summary>
            /// The first text field for the third group in the list.
            /// </summary>
            INotificationContentText TextWrap3 { get; }
        }

        /// <summary>
        /// A large square tile template that displays a list of three groups, each group having
        /// a heading, and one text field, which wraps to two lines followed by an image.
        /// </summary>
        public interface ITileSquare310x310SmallImagesAndTextList04 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The image for the first group in the list.
            /// </summary>
            INotificationContentImage Image1 { get; }

            /// <summary>
            /// The heading for the first group in the list.
            /// </summary>
            INotificationContentText TextHeading1 { get; }

            /// <summary>
            /// The first text field for the first group in the list.
            /// </summary>
            INotificationContentText TextWrap1 { get; }

            /// <summary>
            /// The image for the second group in the list.
            /// </summary>
            INotificationContentImage Image2 { get; }

            /// <summary>
            /// The heading for the second group in the list.
            /// </summary>
            INotificationContentText TextHeading2 { get; }

            /// <summary>
            /// The first text field for the second group in the list.
            /// </summary>
            INotificationContentText TextWrap2 { get; }

            /// <summary>
            /// The image for the third group in the list.
            /// </summary>
            INotificationContentImage Image3 { get; }

            /// <summary>
            /// The heading for the third group in the list.
            /// </summary>
            INotificationContentText TextHeading3 { get; }

            /// <summary>
            /// The first text field for the third group in the list.
            /// </summary>
            INotificationContentText TextWrap3 { get; }
        }

        /// <summary>
        /// A large square tile template that displays a headline and a list of three groups, each group having
        /// an image and two text fields.
        /// </summary>
        public interface ITileSquare310x310SmallImagesAndTextList05 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The headline text.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// The image for the first group in the list.
            /// </summary>
            INotificationContentImage Image1 { get; }

            /// <summary>
            /// The first text field for the first group in the list.
            /// </summary>
            INotificationContentText TextGroup1Field1 { get; }

            /// <summary>
            /// The second text field for the first group in the list.
            /// </summary>
            INotificationContentText TextGroup1Field2 { get; }

            /// <summary>
            /// The image for the second group in the list.
            /// </summary>
            INotificationContentImage Image2 { get; }

            /// <summary>
            /// The first text field for the second group in the list.
            /// </summary>
            INotificationContentText TextGroup2Field1 { get; }

            /// <summary>
            /// The second text field for the second group in the list.
            /// </summary>
            INotificationContentText TextGroup2Field2 { get; }

            /// <summary>
            /// The image for the third group in the list.
            /// </summary>
            INotificationContentImage Image3 { get; }

            /// <summary>
            /// The first text field for the third group in the list.
            /// </summary>
            INotificationContentText TextGroup3Field1 { get; }

            /// <summary>
            /// The second text field for the third group in the list.
            /// </summary>
            INotificationContentText TextGroup3Field2 { get; }
        }

        /// <summary>
        /// A large square tile template that displays a heading and nine text fields.
        /// </summary>
        public interface ITileSquare310x310Text01 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody5 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody6 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody7 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody8 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody9 { get; }
        }

        /// <summary>
        /// A wide tile template that displays nineteen text fields - a heading and two columns
        /// of nine text fields.
        /// </summary>
        public interface ITileSquare310x310Text02 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row5 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row5 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row6 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row6 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row7 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row7 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row8 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row8 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row9 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row9 { get; }
        }

        /// <summary>
        /// A large square tile template that displays eleven text fields.
        /// </summary>
        public interface ITileSquare310x310Text03 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody3 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody4 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody5 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody6 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody7 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody8 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody9 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody10 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody11 { get; }
        }

        /// <summary>
        /// A wide tile template that displays 22 text fields - two columns
        /// of 11 text fields.
        /// </summary>
        public interface ITileSquare310x310Text04 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row1 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row2 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row3 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row4 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row5 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row5 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row6 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row6 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row7 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row7 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row8 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row8 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row9 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row9 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row10 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row10 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn1Row11 { get; }

            /// <summary>
            /// A text field displayed in a column and row.
            /// </summary>
            INotificationContentText TextColumn2Row11 { get; }
        }

        /// <summary>
        /// A wide tile template that displays nineteen text fields - a heading and two columns
        /// of nine text fields. Column 1 is narrower than column 2.
        /// </summary>
        public interface ITileSquare310x310Text05 : ITileSquare310x310Text02
        {
            // ITileSquare310x310Text05 has the same properties as ITileSquare310x310Text02
        }

        /// <summary>
        /// A wide tile template that displays 22 text fields - two columns
        /// of eleven text fields. Column 1 is narrower than column 2.
        /// </summary>
        public interface ITileSquare310x310Text06 : ITileSquare310x310Text04
        {
            // ITileSquare310x310Text06 has the same properties as ITileSquare310x310Text04
        }

        /// <summary>
        /// A wide tile template that displays nineteen text fields - a heading and two columns
        /// of nine text fields. Column 1 is very narrow.
        /// </summary>
        public interface ITileSquare310x310Text07 : ITileSquare310x310Text02
        {
            // ITileSquare310x310Text07 has the same properties as ITileSquare310x310Text02
        }

        /// <summary>
        /// A wide tile template that displays 22 text fields - two columns
        /// of eleven text fields. Column 1 is very narrow.
        /// </summary>
        public interface ITileSquare310x310Text08 : ITileSquare310x310Text04
        {
            // ITileSquare310x310Text08 has the same properties as ITileSquare310x310Text04
        }

        /// <summary>
        /// A large square tile template that displays a heading which wraps to two lines,
        /// two more heading-sized text fields, and two text fields.
        /// </summary>
        public interface ITileSquare310x310Text09 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading1 { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading2 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }
        }

        /// <summary>
        /// A large square tile template that displays a list of three groups, each group having
        /// a heading, and two text fields.
        /// </summary>
        public interface ITileSquare310x310TextList01 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The heading for the first group in the list.
            /// </summary>
            INotificationContentText TextHeading1 { get; }

            /// <summary>
            /// The first text field for the first group in the list.
            /// </summary>
            INotificationContentText TextBodyGroup1Field1 { get; }

            /// <summary>
            /// The second text field for the first group in the list.
            /// </summary>
            INotificationContentText TextBodyGroup1Field2 { get; }

            /// <summary>
            /// The heading for the second group in the list.
            /// </summary>
            INotificationContentText TextHeading2 { get; }

            /// <summary>
            /// The first text field for the second group in the list.
            /// </summary>
            INotificationContentText TextBodyGroup2Field1 { get; }

            /// <summary>
            /// The second text field for the second group in the list.
            /// </summary>
            INotificationContentText TextBodyGroup2Field2 { get; }

            /// <summary>
            /// The heading for the third group in the list.
            /// </summary>
            INotificationContentText TextHeading3 { get; }

            /// <summary>
            /// The first text field for the third group in the list.
            /// </summary>
            INotificationContentText TextBodyGroup3Field1 { get; }

            /// <summary>
            /// The second text field for the third group in the list.
            /// </summary>
            INotificationContentText TextBodyGroup3Field2 { get; }
        }

        /// <summary>
        /// A large square tile template that displays a list of three text fields that can wrap to a total of three lines.
        /// </summary>
        public interface ITileSquare310x310TextList02 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The text field for the first group in the list.
            /// </summary>
            INotificationContentText TextWrap1 { get; }

            /// <summary>
            /// The text field for the second group in the list.
            /// </summary>
            INotificationContentText TextWrap2 { get; }

            /// <summary>
            /// The  text field for the third group in the list.
            /// </summary>
            INotificationContentText TextWrap3 { get; }
        }

        /// <summary>
        /// A large square tile template that displays a list of three groups, each group having
        /// a heading, and one text field, which wraps to two lines.
        /// </summary>
        public interface ITileSquare310x310TextList03 : ISquare310x310TileNotificationContent
        {
            /// <summary>
            /// The heading for the first group in the list.
            /// </summary>
            INotificationContentText TextHeading1 { get; }

            /// <summary>
            /// The first text field for the first group in the list.
            /// </summary>
            INotificationContentText TextWrap1 { get; }

            /// <summary>
            /// The heading for the second group in the list.
            /// </summary>
            INotificationContentText TextHeading2 { get; }

            /// <summary>
            /// The first text field for the second group in the list.
            /// </summary>
            INotificationContentText TextWrap2 { get; }

            /// <summary>
            /// The heading for the third group in the list.
            /// </summary>
            INotificationContentText TextHeading3 { get; }

            /// <summary>
            /// The first text field for the third group in the list.
            /// </summary>
            INotificationContentText TextWrap3 { get; }
        }

        /// <summary>
        /// A Windows Phone small tile template that displays a monochrome icon with a badge.
        /// </summary>
        public interface ITileSquare99x99IconWithBadge : ISquare99x99TileNotificationContent
        {
            /// <summary>
            /// The image for the icon.
            /// </summary>
            INotificationContentImage ImageIcon { get; }
        }

        /// <summary>
        /// A Windows Phone medium tile template that displays a monochrome icon with a badge.
        /// </summary>
        public interface ITileSquare210x210IconWithBadge : ISquare210x210TileNotificationContent
        {
            /// <summary>
            /// The image for the icon.
            /// </summary>
            INotificationContentImage ImageIcon { get; }
        }

        /// <summary>
        /// A Windows Phone large tile template that displays a monochrome icon with a badge and three lines of text.
        /// </summary>
        public interface ITileWide432x210IconWithBadgeAndText : IWide432x210TileNotificationContent
        {
            /// <summary>
            /// The image for the icon.
            /// </summary>
            INotificationContentImage ImageIcon { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }
        }

        /// <summary>
        /// The types of behavior that can be used for application branding when
        /// tile notification content is displayed on the tile.
        /// </summary>
        public enum TileBranding
        {
            /// <summary>
            /// No application branding will be displayed on the tile content.
            /// </summary>
            None = 0,
            /// <summary>
            /// The application logo will be displayed with the tile content.
            /// </summary>
            Logo,
            /// <summary>
            /// The application name will be displayed with the tile content.
            /// </summary>
            Name
        }
    }

    namespace ToastContent
    {
        /// <summary>
        /// Type representing the toast notification audio properties which is contained within
        /// a toast notification content object.
        /// </summary>
        public interface IToastAudio
        {
            /// <summary>
            /// The audio content that should be played when the toast is shown.
            /// </summary>
            ToastAudioContent Content { get; set; }

            /// <summary>
            /// Whether the audio should loop.  If this property is set to true, the toast audio content
            /// must be a looping sound.
            /// </summary>
            bool Loop { get; set; }
        }

        /// <summary>
        /// Type representing the incoming call command properties which is contained within
        /// a toast notification content object.
        /// </summary>
        public interface IIncomingCallCommands
        {
            /// <summary>
            /// Whether the toast shows the video button.
            /// </summary>
            bool ShowVideoCommand { get; set; }

            /// <summary>
            /// The argument passed to the app when the video command is invoked.
            /// </summary>
            string VideoArgument { get; set; }

            /// <summary>
            /// Whether the toast shows the voice button.
            /// </summary>
            bool ShowVoiceCommand { get; set; }

            /// <summary>
            /// The argument passed to the app when the voice command is invoked.
            /// </summary>
            string VoiceArgument { get; set; }

            /// <summary>
            /// Whether the toast shows the decline button.
            /// </summary>
            bool ShowDeclineCommand { get; set; }

            /// <summary>
            /// The argument passed to the app when the decline command is invoked.
            /// </summary>
            string DeclineArgument { get; set; }
        }

        /// <summary>
        /// Type representing the alarm command properties which is contained within
        /// a toast notification content object.
        /// </summary>
        public interface IAlarmCommands
        {
            /// <summary>
            /// Whether the toast shows the snooze button.
            /// </summary>
            bool ShowSnoozeCommand { get; set; }

            /// <summary>
            /// The argument passed to the app when the snooze command is invoked.
            /// </summary>
            string SnoozeArgument { get; set; }

            /// <summary>
            /// Whether the toast shows the dismiss button.
            /// </summary>
            bool ShowDismissCommand { get; set; }

            /// <summary>
            /// The argument passed to the app when the dismiss command is invoked.
            /// </summary>
            string DismissArgument { get; set; }
        }

        /// <summary>
        /// Base toast notification content interface.
        /// </summary>
        public interface IToastNotificationContent : INotificationContent
        {
            /// <summary>
            /// Whether strict validation should be applied when the Xml or notification object is created,
            /// and when some of the properties are assigned.
            /// </summary>
            bool StrictValidation { get; set; }

            /// <summary>
            /// The language of the content being displayed.  The language should be specified using the
            /// abbreviated language code as defined by BCP 47.
            /// </summary>
            string Lang { get; set; }

            /// <summary>
            /// The BaseUri that should be used for image locations.  Relative image locations use this
            /// field as their base Uri.  The BaseUri must begin with http://, https://, ms-appx:///, or
            /// ms-appdata:///local/.
            /// </summary>
            string BaseUri { get; set; }

            /// <summary>
            /// Controls if query strings that denote the client configuration of contrast, scale, and language setting should be appended to the Src
            /// If true, Windows will append query strings onto images that exist in this template
            /// If false (the default, Windows will not append query strings onto images that exist in this template
            /// Query string details:
            /// Parameter: ms-contrast
            ///     Values: standard, black, white
            /// Parameter: ms-scale
            ///     Values: 80, 100, 140, 180
            /// Parameter: ms-lang
            ///     Values: The BCP 47 language tag set in the notification xml, or if omitted, the current preferred language of the user
            /// </summary>
            bool AddImageQuery { get; set; }

            /// <summary>
            /// The launch parameter passed into the Windows Store app when the toast is activated.
            /// </summary>
            string Launch { get; set; }

            /// <summary>
            /// The audio that should be played when the toast is displayed.
            /// </summary>
            IToastAudio Audio { get; }

            /// <summary>
            /// The length that the toast should be displayed on screen.
            /// </summary>
            ToastDuration Duration { get; set; }

            /// <summary>
            /// IncomingCall action buttons will be displayed on the toast if one of the option flags (ShowVideoCommand/ShowVoiceCommand/ShowDeclineCommand) is set to true.
            /// To enable IncomingCall toasts for an app, ensure that the Lock Screen Call extension is enabled in the application manifest.
            /// </summary>
            IIncomingCallCommands IncomingCallCommands { get; }

            /// <summary>
            /// Alarm action buttons will be displayed on the toast if one of the option flags (ShowSnoozeCommand/ShowDismissCommand) is set to true.
            /// To enable Alarm toasts for an app, ensure the app declares itself as Alarm capable.
            /// The app needs to be set as the Primary Alarm on the PC Settings page (only one app can exist as the alarm app at a given time).
            /// </summary>
            IAlarmCommands AlarmCommands { get; }

#if !WINRT_NOT_PRESENT
            /// <summary>
            /// Creates a WinRT ToastNotification object based on the content.
            /// </summary>
            /// <returns>A WinRT ToastNotification object based on the content.</returns>
            ToastNotification CreateNotification();
#endif
        }

        /// <summary>
        /// The audio options that can be played while the toast is on screen.
        /// </summary>
        public enum ToastAudioContent
        {
            /// <summary>
            /// The default toast audio sound.
            /// </summary>
            Default = 0,
            /// <summary>
            /// Audio that corresponds to new mail arriving.
            /// </summary>
            Mail,
            /// <summary>
            /// Audio that corresponds to a new SMS message arriving.
            /// </summary>
            SMS,
            /// <summary>
            /// Audio that corresponds to a new IM arriving.
            /// </summary>
            IM,
            /// <summary>
            /// Audio that corresponds to a reminder.
            /// </summary>
            Reminder,
            /// <summary>
            /// The default looping sound.  Audio that corresponds to a call.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingCall,
            /// <summary>
            /// Audio that corresponds to a call.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingCall2,
            /// <summary>
            /// Audio that corresponds to a call.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingCall3,
            /// <summary>
            /// Audio that corresponds to a call.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingCall4,
            /// <summary>
            /// Audio that corresponds to a call.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingCall5,
            /// <summary>
            /// Audio that corresponds to a call.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingCall6,
            /// <summary>
            /// Audio that corresponds to a call.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingCall7,
            /// <summary>
            /// Audio that corresponds to a call.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingCall8,
            /// <summary>
            /// Audio that corresponds to a call.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingCall9,
            /// <summary>
            /// Audio that corresponds to a call.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingCall10,
            /// <summary>
            /// Audio that corresponds to an alarm.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingAlarm,
            /// <summary>
            /// Audio that corresponds to an alarm.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingAlarm2,
            /// <summary>
            /// Audio that corresponds to an alarm.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingAlarm3,
            /// <summary>
            /// Audio that corresponds to an alarm.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingAlarm4,
            /// <summary>
            /// Audio that corresponds to an alarm.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingAlarm5,
            /// <summary>
            /// Audio that corresponds to an alarm.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingAlarm6,
            /// <summary>
            /// Audio that corresponds to an alarm.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingAlarm7,
            /// <summary>
            /// Audio that corresponds to an alarm.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingAlarm8,
            /// <summary>
            /// Audio that corresponds to an alarm.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingAlarm9,
            /// <summary>
            /// Audio that corresponds to an alarm.
            /// Only valid for toasts that are have the duration set to "Long".
            /// </summary>
            LoopingAlarm10,
            /// <summary>
            /// No audio should be played when the toast is displayed.
            /// </summary>
            Silent
        }

        /// <summary>
        /// The duration the toast should be displayed on screen.
        /// </summary>
        public enum ToastDuration
        {
            /// <summary>
            /// Default behavior.  The toast will be on screen for a short amount of time.
            /// </summary>
            Short = 0,
            /// <summary>
            /// The toast will be on screen for a longer amount of time.
            /// </summary>
            Long
        }

        /// <summary>
        /// A toast template that displays an image and a text field.
        /// </summary>
        public interface IToastImageAndText01 : IToastNotificationContent
        {
            /// <summary>
            /// The main image on the toast.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A toast template that displays an image and two text fields.
        /// </summary>
        public interface IToastImageAndText02 : IToastNotificationContent
        {
            /// <summary>
            /// The main image on the toast.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A toast template that displays an image and two text fields.
        /// </summary>
        public interface IToastImageAndText03 : IToastNotificationContent
        {
            /// <summary>
            /// The main image on the toast.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody { get; }
        }

        /// <summary>
        /// A toast template that displays an image and three text fields.
        /// </summary>
        public interface IToastImageAndText04 : IToastNotificationContent
        {
            /// <summary>
            /// The main image on the toast.
            /// </summary>
            INotificationContentImage Image { get; }

            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }
        }

        /// <summary>
        /// A toast template that displays a text fields.
        /// </summary>
        public interface IToastText01 : IToastNotificationContent
        {
            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A toast template that displays two text fields.
        /// </summary>
        public interface IToastText02 : IToastNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBodyWrap { get; }
        }

        /// <summary>
        /// A toast template that displays two text fields.
        /// </summary>
        public interface IToastText03 : IToastNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeadingWrap { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody { get; }
        }

        /// <summary>
        /// A toast template that displays three text fields.
        /// </summary>
        public interface IToastText04 : IToastNotificationContent
        {
            /// <summary>
            /// A heading text field.
            /// </summary>
            INotificationContentText TextHeading { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody1 { get; }

            /// <summary>
            /// A body text field.
            /// </summary>
            INotificationContentText TextBody2 { get; }
        }
    }

    namespace BadgeContent
    {
        /// <summary>
        /// Base badge notification content interface.
        /// </summary>
        public interface IBadgeNotificationContent : INotificationContent
        {
#if !WINRT_NOT_PRESENT
            /// <summary>
            /// Creates a WinRT BadgeNotification object based on the content.
            /// </summary>
            /// <returns>A WinRT BadgeNotification object based on the content.</returns>
            BadgeNotification CreateNotification();
#endif
        }

        /// <summary>
        /// The types of glyphs that can be placed on a badge.
        /// </summary>
        public enum GlyphValue
        {
            /// <summary>
            /// No glyph.  If there is a numeric badge, or a glyph currently on the badge,
            /// it will be removed.
            /// </summary>
            None = 0,
            /// <summary>
            /// A glyph representing application activity.
            /// </summary>
            Activity,
            /// <summary>
            /// A glyph representing an alert.
            /// </summary>
            Alert,
            /// <summary>
            /// A glyph representing availability status.
            /// </summary>
            Available,
            /// <summary>
            /// A glyph representing away status
            /// </summary>
            Away,
            /// <summary>
            /// A glyph representing busy status.
            /// </summary>
            Busy,
            /// <summary>
            /// A glyph representing that a new message is available.
            /// </summary>
            NewMessage,
            /// <summary>
            /// A glyph representing that media is paused.
            /// </summary>
            Paused,
            /// <summary>
            /// A glyph representing that media is playing.
            /// </summary>
            Playing,
            /// <summary>
            /// A glyph representing unavailable status.
            /// </summary>
            Unavailable,
            /// <summary>
            /// A glyph representing an error.
            /// </summary>
            Error,
            /// <summary>
            /// A glyph representing attention status.
            /// </summary>
            Attention,
            /// <summary>
            /// A glyph representing an alarm.
            /// </summary>
            Alarm
        }
    }
}
