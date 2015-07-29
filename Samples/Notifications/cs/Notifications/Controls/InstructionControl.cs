using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace Notifications.Controls
{
    public class InstructionControl : ContentControl
    {
        public InstructionControl()
        {
            base.DefaultStyleKey = typeof(InstructionControl);
        }

        public static readonly DependencyProperty InstructionProperty = DependencyProperty.Register("Instruction", typeof(string), typeof(InstructionControl), new PropertyMetadata(null));

        public string Instruction
        {
            get { return GetValue(InstructionProperty) as string; }
            set { SetValue(InstructionProperty, value); }
        }
    }
}
