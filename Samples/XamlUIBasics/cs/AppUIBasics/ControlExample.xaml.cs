//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Markup;

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

        public static readonly DependencyProperty ExampleHeightProperty = DependencyProperty.Register("ExampleHeight", typeof(GridLength), typeof(ControlExample), new PropertyMetadata(new GridLength(1, GridUnitType.Star)));
        public GridLength ExampleHeight
        {
            get { return (GridLength)GetValue(ExampleHeightProperty); }
            set { SetValue(ExampleHeightProperty, value); }
        }

        public new static readonly DependencyProperty HorizontalContentAlignmentProperty = DependencyProperty.Register("HorizontalContentAlignment", typeof(HorizontalAlignment), typeof(ControlExample), new PropertyMetadata(HorizontalAlignment.Left));
        public new HorizontalAlignment HorizontalContentAlignment
        {
            get { return (HorizontalAlignment)GetValue(HorizontalContentAlignmentProperty); }
            set { SetValue(HorizontalContentAlignmentProperty, value); }
        }

        public ControlExample()
        {
            this.InitializeComponent();
        }
    }
}