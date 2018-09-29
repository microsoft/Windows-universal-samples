using Windows.Foundation.Metadata;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;

namespace AppUIBasics.SamplePages
{
    public sealed partial class SamplePage2 : Page
    {
        public SamplePage2()
        {
            this.InitializeComponent();
        }

        public void PrepareConnectedAnimation(ConnectedAnimationConfiguration config)
        {
            var anim = ConnectedAnimationService.GetForCurrentView().PrepareToAnimate("BackwardConnectedAnimation", DestinationElement);

            if (config != null && ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))
            {
                anim.Configuration = config;
            }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            var anim = ConnectedAnimationService.GetForCurrentView().GetAnimation("ForwardConnectedAnimation");
            if (anim != null)
            {
                AddContentPanelAnimations();
                anim.TryStart(DestinationElement);
            }
        }

        private void AddContentPanelAnimations()
        {
            ContentPanel.Transitions = new TransitionCollection();
            ContentPanel.Transitions.Add(new EntranceThemeTransition());
        }
    }
}
