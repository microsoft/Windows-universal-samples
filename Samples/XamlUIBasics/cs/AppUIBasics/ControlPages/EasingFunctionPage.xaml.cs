using System;
using System.Collections.Generic;
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
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class EasingFunctionPage : Page
    {
        private List<Tuple<string, EasingFunctionBase>> EasingFunctions { get; } = new List<Tuple<string, EasingFunctionBase>>()
            {
            new Tuple<string, EasingFunctionBase>("BackEase", new BackEase()),
            new Tuple<string, EasingFunctionBase>("BounceEase", new BounceEase()),
            new Tuple<string, EasingFunctionBase>("CircleEase", new CircleEase()),
            new Tuple<string, EasingFunctionBase>("CubicEase", new CubicEase()),
            new Tuple<string, EasingFunctionBase>("ElasticEase", new ElasticEase()),
            new Tuple<string, EasingFunctionBase>("ExponentialEase", new ExponentialEase()),
            new Tuple<string, EasingFunctionBase>("PowerEase", new PowerEase()),
            new Tuple<string, EasingFunctionBase>("QuadraticEase", new QuadraticEase()),
            new Tuple<string, EasingFunctionBase>("QuarticEase", new QuarticEase()),
            new Tuple<string, EasingFunctionBase>("QuinticEase", new QuinticEase()),
            new Tuple<string, EasingFunctionBase>("SineEase", new SineEase())
            };

        public EasingFunctionPage()
        {
            this.InitializeComponent();
        }

        private void Button1_Click(object sender, RoutedEventArgs e)
        {
            Storyboard1.Children[0].SetValue(DoubleAnimation.FromProperty, Translation1.X);
            Storyboard1.Children[0].SetValue(DoubleAnimation.ToProperty, Translation1.X > 0 ? 0 : 200);
            Storyboard1.Begin();
        }

        private void Button2_Click(object sender, RoutedEventArgs e)
        {
            Storyboard2.Children[0].SetValue(DoubleAnimation.FromProperty, Translation2.X);
            Storyboard2.Children[0].SetValue(DoubleAnimation.ToProperty, Translation2.X > 0 ? 0 : 200);
            Storyboard2.Begin();
        }

        private void Button3_Click(object sender, RoutedEventArgs e)
        {
            Storyboard3.Children[0].SetValue(DoubleAnimation.FromProperty, Translation3.X);
            Storyboard3.Children[0].SetValue(DoubleAnimation.ToProperty, Translation3.X > 0 ? 0 : 200);
            Storyboard3.Begin();
        }

        private void Button4_Click(object sender, RoutedEventArgs e)
        {
            var easingFunction = EasingComboBox.SelectedValue as EasingFunctionBase;
            easingFunction.EasingMode = GetEaseValue();
            (Storyboard4.Children[0] as DoubleAnimation).EasingFunction = easingFunction;

            Storyboard4.Children[0].SetValue(DoubleAnimation.FromProperty, Translation4.X);
            Storyboard4.Children[0].SetValue(DoubleAnimation.ToProperty, Translation4.X > 0 ? 0 : 200);
            Storyboard4.Begin();
        }

        EasingMode GetEaseValue()
        {
            if (easeOutRB.IsChecked == true) return EasingMode.EaseOut;
            else if (easeInRB.IsChecked == true) return EasingMode.EaseIn;
            else return EasingMode.EaseInOut;
        }

        private void EasingComboBox_Loaded(object sender, RoutedEventArgs e)
        {
            EasingComboBox.SelectedIndex = 0; 
        }
    }
}
