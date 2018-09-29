using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace AppUIBasics.ControlPages
{
    public sealed partial class ToggleSplitButtonPage : Page
    {
        private MarkerType _type = MarkerType.Bullet;
        public ToggleSplitButtonPage()
        {
            this.InitializeComponent();
        }

        private void myListButton_Click(Microsoft.UI.Xaml.Controls.SplitButton sender, Microsoft.UI.Xaml.Controls.SplitButtonClickEventArgs args)
        {
            // TODO: correct if statement when platform bug is fixed
            if ((sender as Microsoft.UI.Xaml.Controls.ToggleSplitButton).IsChecked)
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

            myListButton.IsChecked = true;
            myListButton.Flyout.Hide();
            myRichEditBox.Focus(FocusState.Keyboard);
        }
    }
}
