using Notifications.Helpers;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Notifications.Model
{
    public class ScenarioGroup : BindableBase, IScenario
    {
        private string _name;
        public string Name
        {
            get { return _name; }
        }

        public override string ToString()
        {
            return Name;
        }
        
        public List<IScenario> Children { get; private set; }

        public ScenarioGroup(string name, IEnumerable<IScenario> children)
        {
            _name = name;
            Children = new List<IScenario>(children);
        }

        public bool Contains(Type scenarioType)
        {
            return this.FindScenario(scenarioType) != null;
        }

        public Scenario FindScenario(Type scenarioType)
        {
            foreach (var child in Children)
            {
                Scenario found = child.FindScenario(scenarioType);
                if (found != null)
                    return found;
            }

            return null;
        }

        public ScenarioGroup FindParent(IScenario ofScenario)
        {
            foreach (var child in Children)
            {
                if (child == ofScenario)
                    return this;

                if (child is ScenarioGroup)
                {
                    ScenarioGroup recursiveFound = (child as ScenarioGroup).FindParent(ofScenario);
                    if (recursiveFound != null)
                        return recursiveFound;
                }
            }

            return null;


        }

        public List<ScenarioGroup> GetPathToScenario(Scenario scenario)
        {
            List<ScenarioGroup> answer = new List<ScenarioGroup>() { this };

            GetPathToScenarioHelper(scenario, answer);

            return answer;
        }

        private void GetPathToScenarioHelper(Scenario scenario, List<ScenarioGroup> listToAppendTo)
        {
            foreach (Scenario child in Children.OfType<Scenario>())
                if (child == scenario)
                {
                    return;
                }

            foreach (ScenarioGroup groupChild in Children.OfType<ScenarioGroup>())
            {
                if (groupChild.Contains(scenario.UIElementType))
                {
                    listToAppendTo.Add(groupChild);
                    groupChild.GetPathToScenarioHelper(scenario, listToAppendTo);
                }
            }
        }
    }
}
