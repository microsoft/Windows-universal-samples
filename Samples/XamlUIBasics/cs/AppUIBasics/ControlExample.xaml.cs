//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using ColorCode;
using ColorCode.Common;
using System;
using System.Diagnostics;
using Windows.Foundation.Metadata;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Markup;
using Windows.UI.Xaml.Media;

namespace AppUIBasics
{
    [ContentProperty(Name="Example")]
    public sealed partial class ControlExample : UserControl
    {
        public static readonly DependencyProperty HeaderTextProperty = DependencyProperty.Register("HeaderText", typeof(string), typeof(ControlExample), new PropertyMetadata(null));
        public string HeaderText
        {
            get { return (string)GetValue(HeaderTextProperty); }
            set { SetValue(HeaderTextProperty, value); }
        }

        public static readonly DependencyProperty ExampleProperty = DependencyProperty.Register("Example", typeof(object), typeof(ControlExample), new PropertyMetadata(null));
        public object Example
        {
            get { return GetValue(ExampleProperty); }
            set { SetValue(ExampleProperty, value); }
        }

        public static readonly DependencyProperty OptionsProperty = DependencyProperty.Register("Options", typeof(object), typeof(ControlExample), new PropertyMetadata(null));
        public object Options
        {
            get { return GetValue(OptionsProperty); }
            set { SetValue(OptionsProperty, value); }
        }

        public static readonly DependencyProperty XamlProperty = DependencyProperty.Register("Xaml", typeof(object), typeof(ControlExample), new PropertyMetadata(null));
        public object Xaml
        {
            get { return GetValue(XamlProperty); }
            set { SetValue(XamlProperty, value); }
        }

        public static readonly DependencyProperty XamlSourceProperty = DependencyProperty.Register("XamlSource", typeof(object), typeof(ControlExample), new PropertyMetadata(null));
        public Uri XamlSource
        {
            get { return (Uri)GetValue(XamlSourceProperty); }
            set { SetValue(XamlSourceProperty, value); }
        }

        public static readonly DependencyProperty CSharpSourceProperty = DependencyProperty.Register("CSharpSource", typeof(object), typeof(ControlExample), new PropertyMetadata(null));
        public Uri CSharpSource
        {
            get { return (Uri)GetValue(CSharpSourceProperty); }
            set { SetValue(CSharpSourceProperty, value); }
        }

        public static readonly DependencyProperty ExampleHeightProperty = DependencyProperty.Register("ExampleHeight", typeof(GridLength), typeof(ControlExample), new PropertyMetadata(new GridLength(1, GridUnitType.Star)));
        public GridLength ExampleHeight
        {
            get { return (GridLength)GetValue(ExampleHeightProperty); }
            set { SetValue(ExampleHeightProperty, value); }
        }

        public static readonly DependencyProperty WebViewHeightProperty = DependencyProperty.Register("WebViewHeight", typeof(Int32), typeof(ControlExample), new PropertyMetadata(400));
        public Int32 WebViewHeight
        {
            get { return (Int32)GetValue(WebViewHeightProperty); }
            set { SetValue(WebViewHeightProperty, value); }
        }

        public static readonly DependencyProperty WebViewWidthProperty = DependencyProperty.Register("WebViewWidth", typeof(Int32), typeof(ControlExample), new PropertyMetadata(800));
        public Int32 WebViewWidth
        {
            get { return (Int32)GetValue(WebViewWidthProperty); }
            set { SetValue(WebViewWidthProperty, value); }
        }

        public new static readonly DependencyProperty HorizontalContentAlignmentProperty = DependencyProperty.Register("HorizontalContentAlignment", typeof(HorizontalAlignment), typeof(ControlExample), new PropertyMetadata(HorizontalAlignment.Left));
        public new HorizontalAlignment HorizontalContentAlignment
        {
            get { return (HorizontalAlignment)GetValue(HorizontalContentAlignmentProperty); }
            set { SetValue(HorizontalContentAlignmentProperty, value); }
        }

        public static readonly DependencyProperty MinimumUniversalAPIContractProperty = DependencyProperty.Register("MinimumUniversalAPIContract", typeof(int), typeof(ControlExample), new PropertyMetadata(null));
        public int MinimumUniversalAPIContract
        {
            get { return (int)GetValue(MinimumUniversalAPIContractProperty); }
            set { SetValue(MinimumUniversalAPIContractProperty, value); }
        }

        public ControlExample()
        {
            this.InitializeComponent();
        }

        private void rootGrid_Loaded(object sender, RoutedEventArgs e)
        {
            if ((ushort)MinimumUniversalAPIContract == 0) return;
            if (!(ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", (ushort)MinimumUniversalAPIContract)))
            {
                ErrorTextBlock.Visibility = Visibility.Visible;
            }
        }

        private Uri GetDerivedSource(Uri rawSource)
        {
            Uri derivedSource = null;

            // Get the full path of the source string
            string concatString = "";
            for (int i = 2; i < rawSource.Segments.Length; i++)
            {
                concatString += rawSource.Segments[i];
            }
            derivedSource = new Uri(new Uri("ms-appx:///ControlPagesSampleCode/"), concatString);

            return derivedSource;
        }

        private enum SyntaxHighlightLanguage { Xml, CSharp };

        private void XamlPresenter_Loaded(object sender, RoutedEventArgs e)
        {
            // Convert existing samples that use the Xaml property
            if (Xaml != null && (Xaml as RichTextBlock) != null)
            {
                var sampleString = GetStringFromRichTextBlock((Xaml as RichTextBlock));

                FormatAndRenderSampleFromString(sampleString, (sender as ContentPresenter), SyntaxHighlightLanguage.Xml);
            }
            else
            {
                FormatAndRenderSampleFromFile(XamlSource, (sender as ContentPresenter), SyntaxHighlightLanguage.Xml);
            }
        }

        private void CSharpPresenter_Loaded(object sender, RoutedEventArgs e)
        {
            FormatAndRenderSampleFromFile(CSharpSource, (sender as ContentPresenter), SyntaxHighlightLanguage.CSharp);
        }

        private async void FormatAndRenderSampleFromFile(Uri source, ContentPresenter presenter, SyntaxHighlightLanguage highlightLanguage)
        {
            if (source != null && source.AbsolutePath.EndsWith("txt"))
            {
                string sampleString = "";

                Uri derivedSource = GetDerivedSource(source);
                var file = await StorageFile.GetFileFromApplicationUriAsync(derivedSource);
                sampleString = await FileIO.ReadTextAsync(file);

                FormatAndRenderSampleFromString(sampleString, presenter, highlightLanguage);
            }
            else
            {
                presenter.Visibility = Visibility.Collapsed;
            }
        }

        private void FormatAndRenderSampleFromString(String sampleString, ContentPresenter presenter, SyntaxHighlightLanguage HighlightLanguage)
        {
            var language = (HighlightLanguage == SyntaxHighlightLanguage.CSharp) ? Languages.CSharp : Languages.Xml;

            var sampleCodeRTB = new RichTextBlock();
            sampleCodeRTB.FontFamily = new FontFamily("Consolas");

            var formatter = GenerateRichTextFormatter();
            formatter.FormatRichTextBlock(sampleString, language, sampleCodeRTB);
            presenter.Content = sampleCodeRTB;
        }

        private RichTextBlockFormatter GenerateRichTextFormatter()
        {
            var formatter = new RichTextBlockFormatter(App.ActualTheme);

            if (App.ActualTheme == ElementTheme.Dark)
            {
                UpdateFormatterDarkThemeColors(formatter);
            }

            return formatter;
        }

        private void UpdateFormatterDarkThemeColors(RichTextBlockFormatter formatter)
        {
            // Replace the default dark theme resources with ones that more closely align to VS Code dark theme.
            formatter.Styles.Remove(formatter.Styles[ScopeName.XmlAttribute]);
            formatter.Styles.Remove(formatter.Styles[ScopeName.XmlAttributeQuotes]);
            formatter.Styles.Remove(formatter.Styles[ScopeName.XmlAttributeValue]);
            formatter.Styles.Remove(formatter.Styles[ScopeName.HtmlComment]);
            formatter.Styles.Remove(formatter.Styles[ScopeName.XmlDelimiter]);
            formatter.Styles.Remove(formatter.Styles[ScopeName.XmlName]);

            formatter.Styles.Add(new ColorCode.Styling.Style(ScopeName.XmlAttribute)
            {
                Foreground = "#FF87CEFA",
                ReferenceName = "xmlAttribute"
            });
            formatter.Styles.Add(new ColorCode.Styling.Style(ScopeName.XmlAttributeQuotes)
            {
                Foreground = "#FFFFA07A",
                ReferenceName = "xmlAttributeQuotes"
            });
            formatter.Styles.Add(new ColorCode.Styling.Style(ScopeName.XmlAttributeValue)
            {
                Foreground = "#FFFFA07A",
                ReferenceName = "xmlAttributeValue"
            });
            formatter.Styles.Add(new ColorCode.Styling.Style(ScopeName.HtmlComment)
            {
                Foreground = "#FF6B8E23",
                ReferenceName = "htmlComment"
            });
            formatter.Styles.Add(new ColorCode.Styling.Style(ScopeName.XmlDelimiter)
            {
                Foreground = "#FF808080",
                ReferenceName = "xmlDelimiter"
            });
            formatter.Styles.Add(new ColorCode.Styling.Style(ScopeName.XmlName)
            {
                Foreground = "#FF5F82E8",
                ReferenceName = "xmlName"
            });
        }

        private void SampleCode_ActualThemeChanged(FrameworkElement sender, object args)
        {
            // If the theme has changed after the user has already opened the app (ie. via settings), then the new locally set theme will overwrite the colors that are set during Loaded.
            // Therefore we need to re-format the REB to use the correct colors. 

            XamlPresenter_Loaded(null, null);
        }

        private string GetStringFromRichTextBlock(RichTextBlock RTB)
        {
            var text = "";
            var blocks = RTB.Blocks;
            foreach (Block b in blocks)
            {
                var inlines = (b as Paragraph).Inlines;

                // Convert each TextIndent into a series of spaces.
                for (int i = 0; i < (b as Paragraph).TextIndent / 4; i++)
                {
                    text += " ";
                }

                foreach (Inline i in inlines)
                {
                    if (i as Run != null)
                    {
                        text += (i as Run).Text;
                    }
                    else
                    {
                        foreach (Inline i2 in (i as Span).Inlines)
                        {
                            if (i2 as Run != null)
                            {
                                text += (i2 as Run).Text;
                            }
                        }
                    }
                }

                text += "\r\n";
            }
            return text;
        }
    }
}