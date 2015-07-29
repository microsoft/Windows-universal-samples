using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

namespace xDeferLoadStrategy
{
    public class TitledImage : Control
    {
        public static readonly DependencyProperty HeaderProperty = DependencyProperty.Register("Header", typeof(string), typeof(TitledImage), new PropertyMetadata(null));
        public string Header
        {
            get { return (string)GetValue(HeaderProperty); }
            set { SetValue(HeaderProperty, value); }
        }

        void HeaderChanged(DependencyObject sender, DependencyProperty prop)
        {
            string header = (string)sender.GetValue(prop);
            //Double check to make sure that we don't needlessly realize the header presenter
            if (!string.IsNullOrEmpty(Header) && _header == null)
            {
                _header = (ContentPresenter)GetTemplateChild("HeaderPresenter"); //This will realize the element
            }
        }

        public static readonly DependencyProperty SourceProperty = DependencyProperty.Register("Source", typeof(ImageSource), typeof(TitledImage), new PropertyMetadata(null));
        public ImageSource Source
        {
            get { return (BitmapSource)GetValue(SourceProperty); }
            set { SetValue(SourceProperty, value); }
        }

        private ContentPresenter _header;

        public TitledImage()
        {
            this.DefaultStyleKey = typeof(TitledImage);

            //We need to register the callback in case they change the header at runtime
            this.RegisterPropertyChangedCallback(HeaderProperty, HeaderChanged);
        }

        protected override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            //Check to see if there's a header, and realize the element if there is one
            if(!string.IsNullOrEmpty(Header))
            {
                _header = (ContentPresenter)GetTemplateChild("HeaderPresenter"); //This will realize the element
            }
        }
    }
}
