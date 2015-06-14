using System;

namespace xBindSampleCS
{
    public class CSharpModel
    {
        public int IntField;
        public string StringField;

        public CSharpModel()
        {
            InitializeValues();
        }

        internal void InitializeValues()
        {
            StringField = "Simple string field";
            IntField = 2;
        }

        internal void UpdateValues()
        {
            this.IntField = IntField + 1 % 10;

            string ms = " " + DateTime.Now.Millisecond.ToString();
            this.StringField += ms;
        }

    }
}