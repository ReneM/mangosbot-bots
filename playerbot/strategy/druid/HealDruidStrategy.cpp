#include "botpch.h"
#include "../../playerbot.h"
#include "DruidMultipliers.h"
#include "HealDruidStrategy.h"

using namespace ai;

class HealDruidStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    HealDruidStrategyActionNodeFactory()
    {
    }
private:
};

HealDruidStrategy::HealDruidStrategy(PlayerbotAI* ai) : GenericDruidStrategy(ai)
{
    actionNodeFactories.Add(new HealDruidStrategyActionNodeFactory());
}

void HealDruidStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    GenericDruidStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "tree form",
        NextAction::array(0, new NextAction("tree form", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "medium health",
        NextAction::array(0, new NextAction("regrowth", ACTION_MEDIUM_HEAL + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "party member medium health",
        NextAction::array(0, new NextAction("regrowth on party", ACTION_MEDIUM_HEAL + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "almost full health",
        NextAction::array(0, new NextAction("rejuvenation", ACTION_LIGHT_HEAL + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "party member almost full health",
        NextAction::array(0, new NextAction("rejuvenation on party", ACTION_LIGHT_HEAL + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "medium aoe heal",
        NextAction::array(0, new NextAction("tranquility", ACTION_MEDIUM_HEAL + 3), NULL)));

    triggers.push_back(new TriggerNode(
        "party member to heal out of spell range",
        NextAction::array(0, new NextAction("reach party member to heal", ACTION_EMERGENCY + 1), NULL)));
}
