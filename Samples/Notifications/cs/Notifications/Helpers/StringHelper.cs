using System.Collections.Generic;
using System.Text;

namespace BackgroundTasks.Helpers
{
    public static class StringHelper
    {
        public static string ToString<K, V>(this IEnumerable<KeyValuePair<K, V>> valueSet)
        {
            if (valueSet == null)
                return null;

            StringBuilder builder = new StringBuilder();

            foreach (var value in valueSet)
            {
                builder.Append(value.Key);
                builder.Append(": ");
                builder.Append(value.Value);

                builder.Append('\n');
            }

            if (builder.Length > 0)
                builder.Length--;

            return builder.ToString();
        }
    }
}
