#pragma once

#include "../Action.h"

namespace ai
{
    class TellTargetAction : public Action
    {
    public:
        TellTargetAction(PlayerbotAI* ai) : Action(ai, "tell target") {}
        virtual bool Execute(Event& event);
    };

    class TellAttackersAction : public Action
    {
    public:
        TellAttackersAction(PlayerbotAI* ai) : Action(ai, "tell attackers") {}
        virtual bool Execute(Event& event);
    };

    class TellPossibleAttackTargetsAction : public Action
    {
    public:
        TellPossibleAttackTargetsAction(PlayerbotAI* ai) : Action(ai, "tell possible attack targets") {}
        virtual bool Execute(Event& event);
    };
}
