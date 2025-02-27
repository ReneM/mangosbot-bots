#pragma once

#include "../Action.h"
#include "QueryItemUsageAction.h"
#include "LootAction.h"

namespace ai
{
    class LootStartRollAction : public Action {
    public:
        LootStartRollAction(PlayerbotAI* ai, string name = "loot start roll") : Action(ai, name) {}
        virtual bool Execute(Event& event);

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "loot start roll"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This is a WorldPacket action which will stores rollable item to be rolled on later.\n";
        }
        virtual vector<string> GetUsedActions() { return {}; }
        virtual vector<string> GetUsedValues() { return { "active rolls" }; }
#endif 
    };

    class RollAction : public QueryItemUsageAction {
    public:
        RollAction(PlayerbotAI* ai, string name = "roll") : QueryItemUsageAction(ai, name) {}
        virtual bool Execute(Event& event);
        virtual bool isPossible();
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "roll"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This will make the bot roll a certain way on a specific item.\n"
                "Usage: roll <roll type> [itemlink].\n"
                "Types are need, greed, pass and auto.\n"
                "When no itemlink is provided bots will roll on all current rollable loot.\n"
                "Examples:\n"
                "roll greed [item]\n"
                "roll auto\n"
                "See also strategies [h:strategy|roll] and [h:strategy|delayed roll].\n";
        }
        virtual vector<string> GetUsedActions() { return {}; }
        virtual vector<string> GetUsedValues() { return { "item usage", "force item usage"}; }
#endif 
    protected:
        virtual ItemPrototype const* GetRollItem(ObjectGuid lootGuid, uint32 slot);
        virtual RollVote CalculateRollVote(ItemPrototype const* proto);
        virtual bool RollOnItemInSlot(RollVote type, ObjectGuid lootGuid, uint32 slot);
    };

    class LootRollAction : public RollAction {
    public:
        LootRollAction(PlayerbotAI* ai, string name = "loot roll") : RollAction(ai, name) {}
        virtual bool Execute(Event& event);
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "delayed loot roll"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This action will make the bot roll on an item the master just rolled on.\n"
                "The type of roll will be automatically determined based on if the bot finds the item usefull.";
        }
        virtual vector<string> GetUsedActions() { return { "roll" }; }
        virtual vector<string> GetUsedValues() { return { "item usage", "force item usage" }; }
#endif 
    };

    class AutoLootRollAction : public RollAction {
    public:
        AutoLootRollAction(PlayerbotAI* ai, string name = "auto loot roll") : RollAction(ai, name) {}
        virtual bool Execute(Event& event);
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "auto loot roll"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This action will make the bot roll on one item that it hasn't rolled for yet.\n"
                "The type of roll will be automatically determined based on if the bot finds the item usefull.";
        }
        virtual vector<string> GetUsedActions() { return { "roll" }; }
        virtual vector<string> GetUsedValues() { return { "item usage", "force item usage" }; }
#endif 
    };
}
