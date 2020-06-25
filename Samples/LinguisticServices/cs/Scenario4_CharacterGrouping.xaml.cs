using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Globalization;
using Windows.Globalization.Collation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4_CharacterGrouping : Page
    {
        MainPage rootPage = MainPage.Current;
        CharacterGroupings ActiveGroupings = null;
        Dictionary<string, UIElement> groups = new Dictionary<string, UIElement>();
        static readonly string GlobeString = "\U0001F310";

        public Scenario4_CharacterGrouping()
        {
            this.InitializeComponent();
        }

        void AddGroup(string label, UIElement element)
        {
            groups.Add(label, element);
            GroupingsGrid.Items.Add(element);
        }

        public void GetGroups_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);
            if (LanguageText.Text == "Default")
            {
                ActiveGroupings = new CharacterGroupings();
            }
            else if (Windows.Globalization.Language.IsWellFormed(LanguageText.Text))
            {
                ActiveGroupings = new CharacterGroupings(LanguageText.Text);
            }
            else
            {
                rootPage.NotifyUser("Invalid language tag.", NotifyType.ErrorMessage);
                return;
            }

            GroupingsGrid.Items.Clear();
            groups.Clear();
            foreach (CharacterGrouping grouping in ActiveGroupings)
            {
                // Save the "other" group for the end.
                if (!string.IsNullOrEmpty(grouping.Label))
                {
                    AddGroup(grouping.Label, new TextBlock { Text = grouping.Label });
                }
            }
            AddGroup(GlobeString, new SymbolIcon { Symbol = Symbol.Globe });
        }

        public void AssignGroup_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);
            if (ActiveGroupings == null)
            {
                rootPage.NotifyUser("Select a language first.", NotifyType.ErrorMessage);
                return;
            }
            if (String.IsNullOrEmpty(CandidateText.Text))
            {
                rootPage.NotifyUser("Enter text to assign to a group.", NotifyType.ErrorMessage);
                return;
            }
            string label = ActiveGroupings.Lookup(CandidateText.Text);
            if (!groups.TryGetValue(label, out UIElement element))
            {
                label = GlobeString;
                element = groups[GlobeString];
            }
            GroupingsGrid.SelectedItem = element;
            GroupingResult.Text = $"Group \"{label}\"";
            element.StartBringIntoView();
        }
    }
}
