// Copyright (c) Morten Nielsen. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using Windows.Foundation.Metadata;
using Windows.UI.Xaml;

namespace WindowsStateTriggers
{
    /// <summary>
    /// Enables a state if a type is present on the device
    /// </summary>
    /// <remarks>
    /// <para>
    /// Example: Checking for hardware back button availability:
    /// <code lang="xaml">
    ///     &lt;triggers:IsTypePresentTrigger TypeName="Windows.Phone.UI.Input.HardwareButtons" />
    /// </code>
    /// </para>
    /// </remarks>
public class IsTypePresentStateTrigger : StateTriggerBase
{
        /// <summary>
        /// Gets or sets the name of the type.
        /// </summary>
        /// <remarks>
        /// Example: <c>Windows.Phone.UI.Input.HardwareButtons</c>
        /// </remarks>
        public string TypeName
        {
            get { return (string)GetValue(TypeNameProperty); }
            set { SetValue(TypeNameProperty, value); }
        }

        /// <summary>
        /// Identifies the <see cref="TypeName"/> DependencyProperty
        /// </summary>
        public static readonly DependencyProperty TypeNameProperty =
            DependencyProperty.Register("TypeName", typeof(string), typeof(IsTypePresentStateTrigger),
            new PropertyMetadata("", OnTypeNamePropertyChanged));

        private static void OnTypeNamePropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var obj = (IsTypePresentStateTrigger)d;
            var val = (string)e.NewValue;

            if (string.IsNullOrWhiteSpace(val))
            {
                obj.SetTriggerValue(false);
            }
            else
            {
                var isTP = ApiInformation.IsTypePresent(val);
                obj.SetTriggerValue(isTP);
            }
        }
    }
}
