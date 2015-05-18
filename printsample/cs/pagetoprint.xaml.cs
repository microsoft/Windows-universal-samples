using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace PrintSample
{
    /// <summary>
    /// Page content to send to the printer
    /// </summary>
    public sealed partial class PageToPrint : Page
    {
        public RichTextBlock TextContentBlock { get; set; }

        public PageToPrint()
        {
            this.InitializeComponent();
            TextContentBlock = TextContent;
        }
    }
}
