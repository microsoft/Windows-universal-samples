using System;
using Windows.Foundation.Collections;
using Windows.Storage;

namespace BackgroundTasks.Helpers
{
    public static class Settings
    {
        private static IPropertySet currentSettings = ApplicationData.Current.LocalSettings.CreateContainer("Settings", ApplicationDataCreateDisposition.Always).Values;

        public static IPropertySet Current
        {
            get
            {
                return currentSettings;
            }
        }

        private static readonly string SCENARIO_TO_OPEN_WHEN_LAUNCHED_OR_ACTIVATED = "ScenarioToOpenWhenLaunchedOrActivated";

        public static Type ScenarioToOpenWhenLaunchedOrActivated
        {
            get
            {
                if (Current.ContainsKey(SCENARIO_TO_OPEN_WHEN_LAUNCHED_OR_ACTIVATED))
                {
                    string s = Current[SCENARIO_TO_OPEN_WHEN_LAUNCHED_OR_ACTIVATED] as string;
                    return Type.GetType(s);
                }
                
                return null;
            }

            set
            {
                if (value == null)
                    Current.Remove(SCENARIO_TO_OPEN_WHEN_LAUNCHED_OR_ACTIVATED);

                else
                    Current[SCENARIO_TO_OPEN_WHEN_LAUNCHED_OR_ACTIVATED] = value.AssemblyQualifiedName;
            }
        }
    }
}
