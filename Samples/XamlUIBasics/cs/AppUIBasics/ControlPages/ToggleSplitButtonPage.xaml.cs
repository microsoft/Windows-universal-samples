using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ToggleSplitButtonPage : Page
    {
        private MarkerType _type = MarkerType.Bullet;
        public ToggleSplitButtonPage()
        {
            this.InitializeComponent();
        }

        private void myListButton_Click(SplitButton sender, SplitButtonClickEventArgs args)
        {
            // TODO: correct if statement when platform bug is fixed
            if ((sender as ToggleSplitButton).IsChecked)
            {
                //remove bulleted list
                myRichEditBox.Document.Selection.ParagraphFormat.ListType = MarkerType.None;
            }
            else
            {
                //add bulleted list
                myRichEditBox.Document.Selection.ParagraphFormat.ListType = _type;
            }            
        }

        private void BulletButton_Click(object sender, RoutedEventArgs e)
        {
            Button clickedBullet = (Button)sender;
            SymbolIcon symbol = (SymbolIcon)clickedBullet.Content;

            if (symbol.Symbol == Symbol.List)
            {
                _type = MarkerType.Bullet;
                mySymbolIcon.Symbol = Symbol.List;
            }
            else if (symbol.Symbol == Symbol.Bullets)
            {
                _type = MarkerType.UppercaseRoman;
                mySymbolIcon.Symbol = Symbol.Bullets;
            }
            myRichEditBox.Document.Selection.ParagraphFormat.ListType = _type;

            //mySymbolIcon = symbol;

            myListButton.IsChecked = true;
            myListButton.Flyout.Hide();
            myRichEditBox.Focus(FocusState.Keyboard);
        }
    }
}
