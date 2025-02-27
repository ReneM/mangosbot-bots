#pragma once
#include "../Strategy.h"

namespace ai
{
    class LootNonCombatStrategy : public Strategy
    {
    public:
        LootNonCombatStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "loot"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "loot"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make bots look for, open and get items from nearby lootable objects.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "gather" }; }
#endif        
    public:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class GatherStrategy : public Strategy
    {
    public:
        GatherStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "gather"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "gather"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make bots look for and save nearby gathering nodes to loot later.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "reveal" }; }
#endif
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class RevealStrategy : public Strategy
    {
    public:
        RevealStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "reveal"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "reveal"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make bots point out nearby gathering nodes that they can open.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "gather" }; }
#endif
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class RollStrategy : public Strategy
    {
    public:
        RollStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "roll"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "roll"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make bots automatically roll on items.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "delayed roll"}; }
#endif
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class DelayedRollStrategy : public Strategy
    {
    public:
        DelayedRollStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "delayed roll"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "delayed roll"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make bots roll on item after the master rolls..";
        }
        virtual vector<string> GetRelatedStrategies() { return { "roll" }; }
#endif
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
