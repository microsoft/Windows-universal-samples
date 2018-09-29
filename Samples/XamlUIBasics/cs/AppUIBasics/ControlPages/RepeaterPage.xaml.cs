using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class RepeaterPage : ItemsPageBase
    {
        private Random random = new Random();
        private int MaxLength = 425;
        private bool isHorizontal = false;

        public ObservableCollection<Bar> BarItems;
        public RepeaterPage()
        {
            this.InitializeComponent();
            InitializeData();
            repeater2.ItemsSource = Enumerable.Range(0, 500);
        }

        private void InitializeData()
        {
            if (BarItems == null)
            {
                BarItems = new ObservableCollection<Bar>();
            }
            BarItems.Add(new Bar(300, this.MaxLength));
            BarItems.Add(new Bar(25, this.MaxLength));
            BarItems.Add(new Bar(175, this.MaxLength));
        }

        private void AddBtn_Click(object sender, RoutedEventArgs e)
        {
            BarItems.Add(new Bar(random.Next(this.MaxLength), this.MaxLength));
            DeleteBtn.IsEnabled = true;
        }

        private void DeleteBtn_Click(object sender, RoutedEventArgs e)
        {
            if (BarItems.Count > 0)
            {
                BarItems.RemoveAt(0);
                if (BarItems.Count == 0)
                {
                    DeleteBtn.IsEnabled = false;
                }
            }
        }

        private void OrientationBtn_Click(object sender, RoutedEventArgs e)
        {
            string layoutKey = String.Empty, elementGeneratorKey = String.Empty, itemTemplateKey = String.Empty;

            if (isHorizontal)
            {
                layoutKey = "VerticalStackLayout";
                elementGeneratorKey = "HorizontalElementGenerator";
                itemTemplateKey = "HorizontalBarTemplate";
            }
            else
            {
                layoutKey = "HorizontalStackLayout";
                elementGeneratorKey = "VerticalElementGenerator";
                itemTemplateKey = "VerticalBarTemplate";
            }

            repeater.Layout = Resources[layoutKey] as Microsoft.UI.Xaml.Controls.VirtualizingLayoutBase;
            repeater.ViewGenerator = Resources[elementGeneratorKey] as Microsoft.UI.Xaml.Controls.ViewGenerator;
            repeater.ItemsSource = BarItems;

            layout.Text = layoutKey;
            elementGenerator.Text = itemTemplateKey;

            isHorizontal = !isHorizontal;
        }

        private void LayoutBtn_Click(object sender, RoutedEventArgs e)
        {
            string layoutKey = ((FrameworkElement)sender).Tag as string;

            repeater2.Layout = Resources[layoutKey] as Microsoft.UI.Xaml.Controls.VirtualizingLayoutBase;

            layout2.Text = layoutKey;
        }

        private void RadioBtn_Click(object sender, RoutedEventArgs e)
        {
            string elementGeneratorKey = String.Empty, itemTemplateKey = String.Empty;
            var layoutKey = ((FrameworkElement)sender).Tag as string;

            if (layoutKey.Equals(nameof(this.VerticalStackLayout))) // we used x:Name in the resources which both acts as the x:Key value and creates a member field by the same name
            {
                layout.Text = layoutKey;
                elementGeneratorKey = "HorizontalElementGenerator";
                itemTemplateKey = "HorizontalBarTemplate";

                repeater.MaxWidth = MaxLength + 12;
            }
            else if (layoutKey.Equals(nameof(this.HorizontalStackLayout)))
            {
                layout.Text = layoutKey;
                elementGeneratorKey = "VerticalElementGenerator";
                itemTemplateKey = "VerticalBarTemplate";

                repeater.MaxWidth = 6000;
            }
            else if (layoutKey.Equals(nameof(this.UniformGridLayout)))
            {
                layout.Text = layoutKey;
                elementGeneratorKey = "CircularElementGenerator";
                itemTemplateKey = "CircularTemplate";

                repeater.MaxWidth = 540;
            }

            repeater.Layout = Resources[layoutKey] as Microsoft.UI.Xaml.Controls.VirtualizingLayoutBase;
            repeater.ViewGenerator = Resources[elementGeneratorKey] as Microsoft.UI.Xaml.Controls.ViewGenerator;
            repeater.ItemsSource = BarItems;

            elementGenerator.Text = itemTemplateKey;
        }

        private void GridViewGenerator_SelectTemplateKey(Microsoft.UI.Xaml.Controls.RecyclingViewGenerator sender, Microsoft.UI.Xaml.Controls.SelectTemplateEventArgs args)
        {
            args.TemplateKey = ((int)args.DataContext) % 2 == 0 ? nameof(this.NormalItem) : nameof(this.AccentItem);
        }
    }

    public class Bar
    {
        public Bar(double length, int max)
        {
            Length = length;
            MaxLength = max;

            Height = length / 4;
            MaxHeight = max / 4;

            Diameter = length / 6;
            MaxDiameter = max / 6;
        }
        public double Length { get; set; }
        public int MaxLength { get; set; }

        public double Height { get; set; }
        public double MaxHeight { get; set; }

        public double Diameter { get; set; }
        public double MaxDiameter { get; set; }
    }
}
