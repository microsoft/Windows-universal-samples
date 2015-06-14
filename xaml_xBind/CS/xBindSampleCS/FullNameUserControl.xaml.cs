using xBindSampleModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace xBindSampleCS
{
    public sealed partial class FullNameUserControl : UserControl
    {
        public FullNameUserControl()
        {
            this.InitializeComponent();
        }

        // The following was possible with the xBind prototype
        // public Employee Person;

        public IEmployee Person
        {
            get { return (IEmployee)GetValue(PersonProperty); }
            set { SetValue(PersonProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Person.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty PersonProperty =
            DependencyProperty.Register("Person", typeof(IEmployee), typeof(FullNameUserControl), new PropertyMetadata(null));

    }
}
