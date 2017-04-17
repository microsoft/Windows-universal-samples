using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.Storage;

namespace SDKTemplate
{
    public class Contact
    {
        public string FirstName { get; set; }
        public string LastName { get; set; }
        public string Company { get; set; }

        public string FullName { get { return string.Format("{0} {1}", FirstName, LastName); } }
        public string DisplayName { get { return string.Format("{0} ({1})", FullName, Company); } }
    }

    public static class ContactSampleDataSource
    {
        private static List<Contact> _contacts = new List<Contact>();

        public static async Task CreateContactSampleDataAsync()
        {
            // Don't need to do this more than once.
            if (_contacts.Count > 0)
            {
                return;
            }

            StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Assets/Contacts.txt"));
            IList<string> lines = await FileIO.ReadLinesAsync(file);

            // Verify that it hasn't been done by someone else in the meantime.
            if (_contacts.Count == 0)
            {
                for (int i = 0; i < lines.Count; i += 3)
                {
                    _contacts.Add(new Contact() { FirstName = lines[i], LastName = lines[i + 1], Company = lines[i + 2] });
                }
            }
            _contacts = _contacts.OrderBy(c => c.FullName).ToList();
        }

        /// <summary>
        /// Do a fuzzy search on all contacts and order results based on a pre-defined rule set
        /// </summary>
        /// <param name="query">The part of the name or company to look for</param>
        /// <returns>An ordered list of contacts that matches the query</returns>
        public static IEnumerable<Contact> GetMatchingContacts(string query)
        {
            return ContactSampleDataSource._contacts
                .Where(c => c.FirstName.IndexOf(query, StringComparison.CurrentCultureIgnoreCase) > -1 ||
                            c.LastName.IndexOf(query, StringComparison.CurrentCultureIgnoreCase) > -1 ||
                            c.Company.IndexOf(query, StringComparison.CurrentCultureIgnoreCase) > -1)
                .OrderByDescending(c => c.FirstName.StartsWith(query, StringComparison.CurrentCultureIgnoreCase))
                .ThenByDescending(c => c.LastName.StartsWith(query, StringComparison.CurrentCultureIgnoreCase))
                .ThenByDescending(c => c.Company.StartsWith(query, StringComparison.CurrentCultureIgnoreCase));
        }
    }
}
