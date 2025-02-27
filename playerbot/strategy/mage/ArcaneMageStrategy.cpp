#include "botpch.h"
#include "../../playerbot.h"
#include "MageMultipliers.h"
#include "ArcaneMageStrategy.h"

using namespace ai;

class ArcaneMageStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    ArcaneMageStrategyActionNodeFactory()
    {
        creators["arcane blast"] = &arcane_blast;
        creators["arcane barrage"] = &arcane_barrage;
        creators["arcane missiles"] = &arcane_missiles;
    }
private:
    static ActionNode* arcane_blast(PlayerbotAI* ai)
    {
        return new ActionNode ("arcane blast",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("arcane missiles"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* arcane_barrage(PlayerbotAI* ai)
    {
        return new ActionNode ("arcane barrage",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("arcane missiles"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* arcane_missiles(PlayerbotAI* ai)
    {
        return new ActionNode ("arcane missiles",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("shoot"), NULL),
            /*C*/ NULL);
    }
};

ArcaneMageStrategy::ArcaneMageStrategy(PlayerbotAI* ai) : GenericMageStrategy(ai)
{
    actionNodeFactories.Add(new ArcaneMageStrategyActionNodeFactory());
}

NextAction** ArcaneMageStrategy::GetDefaultCombatActions()
{
    return NextAction::array(0, new NextAction("arcane barrage", 10.0f), NULL);
}

void ArcaneMageStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    GenericMageStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "arcane blast",
        NextAction::array(0, new NextAction("arcane blast", 15.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "missile barrage",
        NextAction::array(0, new NextAction("arcane missiles", 15.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "mana shield",
        NextAction::array(0, new NextAction("mana shield", 20.0f), NULL)));
}

void ArcaneMageAoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "enemy too close for spell",
        NextAction::array(0, new NextAction("arcane explosion", 60.0f), NULL)));
}
