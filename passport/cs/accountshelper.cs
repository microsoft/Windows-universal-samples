using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Serialization;
using Windows.Storage;

namespace SDKTemplate
{
    public class AccountsHelper
    {
        private const string USER_LIST_FILE_NAME = "accountlist.txt";

        /// <summary>
        /// Gets the account list file and deserializes it from XML to a list of accounts object.
        /// </summary>
        /// <returns>List of account objects</returns>
        static public async Task<List<Account>> LoadAccountList()
        {
            try
            {
                StorageFile accountsFile = await ApplicationData.Current.LocalFolder.GetFileAsync(USER_LIST_FILE_NAME);
                string accountsXml = await Windows.Storage.FileIO.ReadTextAsync(accountsFile);
                return AccountsHelper.DeserializeXmlToAccountList(accountsXml);
            }
            catch (FileNotFoundException e)
            {
                List<Account> emptyAccountList = new List<Account>();
                return emptyAccountList;
            }
        }

        /// <summary>
        /// Takes a list of accounts and create an account list file. (Replacing the old one)
        /// </summary>
        /// <param name="accountList">List object of accounts</param>
        static public async void SaveAccountList(List<Account> accountList)
        {
            string accountsXml = SerializeAccountListToXml(accountList);

            try
            {
                StorageFile accountsFile = await ApplicationData.Current.LocalFolder.GetFileAsync(USER_LIST_FILE_NAME);
                await Windows.Storage.FileIO.WriteTextAsync(accountsFile, accountsXml);
            }
            catch (FileNotFoundException e)
            {
                StorageFile accountsFile = await ApplicationData.Current.LocalFolder.CreateFileAsync(USER_LIST_FILE_NAME);
                await Windows.Storage.FileIO.WriteTextAsync(accountsFile, accountsXml);
            }
        }

        /// <summary>
        /// Takes a list of accounts and returns an XML formatted string representing the list
        /// </summary>
        /// <param name="list">List object of accounts</param>
        /// <returns>XML formatted list of accounts</returns>
        static public string SerializeAccountListToXml(List<Account> list)
        {
            XmlSerializer xmlizer = new XmlSerializer(typeof(List<Account>));
            StringWriter writer = new StringWriter();
            xmlizer.Serialize(writer, list);
            return writer.ToString();            
        }

        /// <summary>
        /// Takes an XML formatted string representing a list of accounts and returns a list object of accounts
        /// </summary>
        /// <param name="listAsXml">XML formatted list of accounts</param>
        /// <returns>List object of accounts</returns>
        static public List<Account> DeserializeXmlToAccountList(string listAsXml)
        {
            XmlSerializer xmlizer = new XmlSerializer(typeof(List<Account>));
            List<Account> accounts = new List<Account>();
            TextReader textreader = new StreamReader(new MemoryStream(Encoding.UTF8.GetBytes(listAsXml)));
            accounts = (xmlizer.Deserialize(textreader)) as List<Account>;
            return accounts;
        }
    }
}
