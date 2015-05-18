using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ScreenCasting.Util
{
    class ApiInformation
    {
        public static bool IsTypePresent(string typeName)
        {
            try
            {
                Type.GetType(typeName);
                return true;
            }
            catch {
                return false;
            }
        }
    }
}
