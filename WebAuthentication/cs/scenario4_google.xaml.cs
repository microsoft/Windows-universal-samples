using SDKTemplate;
using System;
using Windows.Security.Authentication.Web;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace WebAuthentication
{
    public sealed partial class Scenario4_Google : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario4_Google()
        {
            this.InitializeComponent();
        }

        private void OutputToken(String TokenUri)
        {
            GoogleReturnedToken.Text = TokenUri;
        }

        private async void Launch_Click(object sender, RoutedEventArgs e)
        {
            if (GoogleClientID.Text == "")
            {
                rootPage.NotifyUser("Please enter an Client ID.", NotifyType.StatusMessage);
            }
            else if (GoogleCallbackUrl.Text == "")
            {
                rootPage.NotifyUser("Please enter an Callback URL.", NotifyType.StatusMessage);
            }

            try
            {
                String GoogleURL = "https://accounts.google.com/o/oauth2/auth?client_id=" + Uri.EscapeDataString(GoogleClientID.Text) + "&redirect_uri=" + Uri.EscapeDataString(GoogleCallbackUrl.Text) + "&response_type=code&scope=" + Uri.EscapeDataString("http://picasaweb.google.com/data");

                Uri StartUri = new Uri(GoogleURL);
                // When using the desktop flow, the success code is displayed in the html title of this end uri
                Uri EndUri = new Uri("https://accounts.google.com/o/oauth2/approval?");

                rootPage.NotifyUser("Navigating to: " + GoogleURL, NotifyType.StatusMessage);

                WebAuthenticationResult WebAuthenticationResult = await WebAuthenticationBroker.AuthenticateAsync(WebAuthenticationOptions.UseTitle, StartUri, EndUri);
                if (WebAuthenticationResult.ResponseStatus == WebAuthenticationStatus.Success)
                {
                    OutputToken(WebAuthenticationResult.ResponseData.ToString());
                }
                else if (WebAuthenticationResult.ResponseStatus == WebAuthenticationStatus.ErrorHttp)
                {
                    OutputToken("HTTP Error returned by AuthenticateAsync() : " + WebAuthenticationResult.ResponseErrorDetail.ToString());
                }
                else
                {
                    OutputToken("Error returned by AuthenticateAsync() : " + WebAuthenticationResult.ResponseStatus.ToString());
                }
            }
            catch (Exception Error)
            {
                rootPage.NotifyUser(Error.Message, NotifyType.ErrorMessage);
            }
        }
    }
}
