#pragma once
#include "../Strategy.h"

namespace ai
{
    class MoltenCoreDungeonStrategy : public Strategy
    {
    public:
        MoltenCoreDungeonStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "molten core"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class MagmadarFightStrategy : public Strategy
    {
    public:
        MagmadarFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "magmadar"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
    };
}