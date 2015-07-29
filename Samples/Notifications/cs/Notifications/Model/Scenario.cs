using Notifications.Helpers;
using System;

namespace Notifications.Model
{
    public class Scenario : BindableBase, IScenario
    {
        /// <summary>
        /// Simply returns the PageUri, since that's guaranteed to be unique
        /// </summary>
        public string Id
        {
            get { return UIElementType.FullName; }
        }

        private string _name;
        public string Name
        {
            get { return _name; }
        }

        public Type UIElementType { get; private set; }

        public Scenario(string name, Type uiElementType)
        {
            _name = name;
            UIElementType = uiElementType;
        }

        public override string ToString()
        {
            return Name + " - " + Id;
        }

        public bool Contains(Type scenarioType)
        {
            return FindScenario(scenarioType) != null;
        }

        public Scenario FindScenario(Type scenarioType)
        {
            if (UIElementType == scenarioType)
                return this;

            return null;
        }
    }
}
