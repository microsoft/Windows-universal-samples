using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.ApplicationModel.Resources;
namespace AppResourceClassLibrary
{
    public static class LocalizedNamesLibrary
    {
        static ResourceLoader  resourceLoader = null;

        public static string LibraryName { 
            get 
            {
                String name;
                GetLibraryName("string1", out name);
                return name;
            } 
        }

        private static void GetLibraryName(string resourceName, out string resourceValue)
        {
            if (resourceLoader == null)
            {
                resourceLoader = ResourceLoader.GetForCurrentView("AppResourceClassLibrary/Resources");
            }
            resourceValue = resourceLoader.GetString(resourceName);
        }

    }
}
