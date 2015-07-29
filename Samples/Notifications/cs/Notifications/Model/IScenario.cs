using System;

namespace Notifications.Model
{
    public interface IScenario
    {
        string Name { get; }

        bool Contains(Type scenarioType);

        Scenario FindScenario(Type scenarioType);
    }
}
