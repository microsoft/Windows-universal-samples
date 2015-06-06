using Windows.Foundation;
using Windows.UI.Xaml.Controls;

namespace PrintSample
{
    /// <summary>
    /// Used in scenario 5 to display a custom print preview unavailable page
    /// </summary>
    public sealed partial class PreviewUnavailable : Page
    {
        public PreviewUnavailable()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Preview unavailable page constructor
        /// </summary>
        /// <param name="paperSize">The printing page paper size</param>
        /// <param name="printableSize">The usable/"printable" area on the page</param>
        public PreviewUnavailable(Size paperSize, Size printableSize)
            : this()
        {
            Page.Width = paperSize.Width;
            Page.Height = paperSize.Height;

            PrintablePage.Width = printableSize.Width;
            PrintablePage.Height = printableSize.Height;
        }
    }
}
