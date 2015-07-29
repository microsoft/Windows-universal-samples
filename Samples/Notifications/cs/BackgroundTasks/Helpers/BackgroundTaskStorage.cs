using System.Collections.Generic;
using Windows.Foundation.Collections;
using Windows.Storage;

namespace BackgroundTasks.Helpers
{
    public static class BackgroundTaskStorage
    { 
        private static IPropertySet Values = ApplicationData.Current.LocalSettings.CreateContainer("BackgroundTaskStorage", ApplicationDataCreateDisposition.Always).Values;

        public static IPropertySet GetValues()
        {
            return Values;
        }

        public static void PutError(string message)
        {
            Values["error"] = message;
        }

        public static string GetError()
        {
            if (Values.ContainsKey("error"))
                return Values["error"] as string;

            return null;
        }

        public static void PutAnswer(object answer)
        {
            // Clear the message since it was successful
            PutError(null);

            Values["answer"] = answer;
        }

        public static object GetAnswer()
        {
            object obj;

            Values.TryGetValue("answer", out obj);

            return obj;
        }

        public static IDictionary<string, object> ConvertValueSetToDictionary(ValueSet valueSet)
        {
            Dictionary<string, object> converted = new Dictionary<string, object>();

            foreach (var value in valueSet)
            {
                converted[value.Key] = value.Value;
            }

            return converted;
        }

        public static ApplicationDataCompositeValue ConvertValueSetToApplicationDataCompositeValue(ValueSet valueSet)
        {
            ApplicationDataCompositeValue converted = new ApplicationDataCompositeValue();

            foreach (var value in valueSet)
            {
                converted[value.Key] = value.Value;
            }

            return converted;
        }
    }
}
