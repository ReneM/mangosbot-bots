#pragma once

#include "../Action.h"
#include "InventoryAction.h"

namespace ai
{
    class DestroyItemAction : public InventoryAction {
    public:
        DestroyItemAction(PlayerbotAI* ai, string name = "destroy") : InventoryAction(ai, name) {}
        virtual bool Execute(Event& event);
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "destroy"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This command will make the bot destroy a certain item.\n"
                "Usage: destroy [itemlink]\n";
        }
        virtual vector<string> GetUsedActions() { return {}; }
        virtual vector<string> GetUsedValues() { return { "force item usage" }; }
#endif 
    protected:
        void DestroyItem(FindItemVisitor* visitor);
    };

    class SmartDestroyItemAction : public DestroyItemAction {
    public:
        SmartDestroyItemAction(PlayerbotAI* ai) : DestroyItemAction(ai, "smart destroy") {}
        virtual bool Execute(Event& event);
        virtual bool isUseful() { return !ai->HasActivePlayerMaster(); }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "smart destroy"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This command will make the bot destroy a certain item.\n"
                "Usage: destroy [itemlink]\n";
        }
        virtual vector<string> GetUsedActions() { return {"destroy"}; }
        virtual vector<string> GetUsedValues() { return { "bag space", "force item usage" }; }
#endif 
    };

}
