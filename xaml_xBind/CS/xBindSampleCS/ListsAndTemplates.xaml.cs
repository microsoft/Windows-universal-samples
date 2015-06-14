using System.Collections.Generic;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using xBindSampleModel;


namespace xBindSampleCS
{
    public sealed partial class ListsAndTemplates : Page
    {
        public DataModel Model { get; set; }

        public List<IEmployee> Employees = new List<IEmployee>();

        public ListsAndTemplates()
        {
            this.Model = new DataModel();
            this.InitializeComponent();
            InitializeValues();
        }

        void InitializeValues()
        {
            this.Model.InitializeValues();
            foreach (IEmployee e in Model.ManagerProp.ReportsList)
            {
                if (this.Employees != null)
                {
                    this.Employees.Add(e);
                }
            }
        }

        private void UpdateValuesClick(object sender, RoutedEventArgs e)
        {
            this.Model.UpdateValues();
            if (this.Employees != null)
            {
                IEmployee temp = Employees[0];
                Employees.RemoveAt(0);
                Employees.Add(temp);
            }
        }

        private void ResetValuesClick(object sender, RoutedEventArgs e)
        {
            this.InitializeValues();
        }

    }
}
