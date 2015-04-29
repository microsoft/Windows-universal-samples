using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Input.Inking;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class InkCanvasPage : Page
    {
        private InkPresenter _inkPresenter;

        public InkCanvasPage()
        {
            this.InitializeComponent();

            _inkPresenter = Control1.InkPresenter;
            _inkPresenter.InputDeviceTypes =
                CoreInputDeviceTypes.Mouse | CoreInputDeviceTypes.Pen | CoreInputDeviceTypes.Touch;

            UpdatePen();
        }

        private void penColor_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            UpdatePen();
        }

        private void strokeSize_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            UpdatePen();
        }

        private void drawAsHighlighter_CheckedChanged(object sender, RoutedEventArgs e)
        {
            UpdatePen();

        }

        private void penTipShape_Toggled(object sender, RoutedEventArgs e)
        {
            UpdatePen();

        }

        private void UpdatePen()
        {
            if (_inkPresenter != null)
            {
                var defaultAttributes = _inkPresenter.CopyDefaultDrawingAttributes();

                switch(penColor.SelectedValue.ToString())
                {
                    case "Black":
                        defaultAttributes.Color = Colors.Black;
                        break;
                    case "Red":
                        defaultAttributes.Color = Colors.Red;
                        break;
                    case "Blue":
                        defaultAttributes.Color = Colors.Blue;
                        break;
                    case "Green":
                        defaultAttributes.Color = Colors.Green;
                        break;
                }

                defaultAttributes.Size = new Size(strokeSize.Value, strokeSize.Value);
                defaultAttributes.DrawAsHighlighter = drawAsHighlighter.IsChecked.Value;
                defaultAttributes.PenTip = penTipShape.IsOn ? PenTipShape.Circle : PenTipShape.Rectangle;

                _inkPresenter.UpdateDefaultDrawingAttributes(defaultAttributes);
            }
        }

        private void clearAll_Click(object sender, RoutedEventArgs e)
        {
            _inkPresenter.StrokeContainer.Clear();
        }
    }
}
