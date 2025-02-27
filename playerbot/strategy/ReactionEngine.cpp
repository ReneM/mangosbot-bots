#include "../../botpch.h"
#include "../playerbot.h"

#include "ReactionEngine.h"
#include <iomanip>

using namespace ai;
using namespace std;

void Reaction::SetAction(Action* inAction)
{
    if (inAction)
    {
        // Set default duration (5 seconds)
        SetDuration(5000U);
        action = inAction;
    }
}

bool Reaction::Update(uint32 elapsed)
{
    // Update remaining duration
    duration = duration > elapsed ? duration - elapsed : 0;

    // TO DO: Check if the reaction got interrupted (stun, knockback, ...)
    // ...

    // Return true when the reaction has finished
    return !IsActive();
}

ReactionEngine::ReactionEngine(PlayerbotAI* ai, AiObjectContext* factory, BotState engineState) 
: Engine(ai, factory, engineState)
, aiReactionUpdateDelay(0U)
{
    
}

bool ReactionEngine::FindReaction()
{
    // Don't find a new reaction if the previous reaction is still running
    if(!IsReacting())
    {
        aiObjectContext->Update();

        // This will populate the queue to be processed with the reactions that can be triggered
        ProcessTriggers(false);

        ActionBasket* reactionItem = NULL;

        // Look for the best reaction (if any available)
        int iterations = 0;
        int iterationsPerTick = queue.Size() *  sPlayerbotAIConfig.iterationsPerTick;
        do 
        {
            // Get the best reaction in the queue (sorted by relevance)
            reactionItem = queue.Peek();
            if (reactionItem)
            {
                const bool skipReactionPrerequisites = reactionItem->isSkipPrerequisites();
                float reactionRelevance = reactionItem->getRelevance();
                const Event& reactionEvent = reactionItem->getEvent();

                // Extract the reaction from the queue (removed)
                ActionNode* reactionNode = queue.Pop(reactionItem);
                if (reactionNode)
                {
                    Action* reaction = InitializeAction(reactionNode);
                    if (reaction)
                    {
                        // Update the reaction relevance
                        reaction->setRelevance(reactionRelevance);

                        // Check if the reaction is useful
                        if (reaction->isUseful())
                        {
                            // Process the multipliers
                            for (list<Multiplier*>::iterator i = multipliers.begin(); i != multipliers.end(); i++)
                            {
                                reactionRelevance *= (*i)->GetValue(reaction);
                                reaction->setRelevance(reactionRelevance);
                                if (reactionRelevance <= 0.0f)
                                {
                                    // Multiplier made reaction useless
                                    break;
                                }
                            }

                            // Check if the reaction is possible
                            if ((reactionRelevance > 0.0f) && reaction->isPossible())
                            {
                                // Process prerequisites
                                if (!skipReactionPrerequisites)
                                {
                                    // Add the prerequisites to the queue with a slight higher relevance than this action to be processed in the next iteration
                                    if (MultiplyAndPush(reactionNode->getPrerequisites(), reactionRelevance + 0.02, false, reactionEvent, "prereq"))
                                    {
                                        // Add this reaction to the queue again to be processed after the prerequisite
                                        PushAgain(reactionNode, reactionRelevance + 0.01, reactionEvent);
                                        continue;
                                    }
                                }

                                // Reaction found
                                incomingReaction.SetAction(reaction);
                                incomingReaction.SetEvent(reactionEvent);
                                delete reactionNode;
                                break;
                            }
                            else
                            {
                                // Add the alternative reactions to the queue
                                MultiplyAndPush(reactionNode->getAlternatives(), reactionRelevance + 0.03, false, reactionEvent, "alt");
                            }
                        }
                    }

                    // Delete the reaction node
                    delete reactionNode;
                }
            }
        }
        while (reactionItem && ++iterations <= iterationsPerTick);

        // Remove the expired reactions
        queue.RemoveExpired();

        return incomingReaction.IsValid();
    }

    return false;
}

bool ReactionEngine::StartReaction()
{
    bool reactionExecuted = false;
    if (incomingReaction.IsValid())
    {
        // Execute the incoming reaction
        reactionExecuted = ListenAndExecute(incomingReaction.GetAction(), incomingReaction.GetEvent());
        if (reactionExecuted)
        {
            // Move the incoming reaction to the ongoing reaction
            ongoingReaction = incomingReaction;
        }

        // Remove the incoming reaction
        incomingReaction.Reset();
    }

    return reactionExecuted;
}

void ReactionEngine::StopReaction()
{
    ongoingReaction.Reset();
    aiReactionUpdateDelay = 0U;

    // TO DO: Interrupt if the action is still running
    // ...
}

bool ReactionEngine::Update(uint32 elapsed, bool minimal, bool& reactionFound, bool& reactionFinished)
{
    aiReactionUpdateDelay = aiReactionUpdateDelay > elapsed ? aiReactionUpdateDelay - elapsed : 0U;

    reactionFound = false;
    reactionFinished = false;

    // Can update reaction?
    if (CanUpdateAIReaction())
    {
        if (IsReacting())
        {
            if (ongoingReaction.Update(elapsed))
            {
                StopReaction();
                reactionFinished = true;
            }
        }
        else
        {
            if (HasIncomingReaction())
            {
                // Start the incoming reaction
                StartReaction();
            }
            else
            {
                // Look for an available reaction
                if (FindReaction())
                {
                    reactionFound = true;
                }
            }
        }

        // Only add a reaction update delay if no reaction is pending or currently running
        if (!HasIncomingReaction() && !IsReacting())
        {
            if (aiReactionUpdateDelay < sPlayerbotAIConfig.reactDelay)
                aiReactionUpdateDelay = minimal ? sPlayerbotAIConfig.reactDelay * 10 : sPlayerbotAIConfig.reactDelay;
        }
    }

    // Return true if a reaction is pending or currently running
    return HasIncomingReaction() || IsReacting();
}

bool ReactionEngine::ListenAndExecute(Action* action, Event& event)
{
    bool actionExecuted = false;
    if (actionExecutionListeners.Before(action, event))
    {
        actionExecuted = actionExecutionListeners.AllowExecution(action, event) ? action->Execute(event) : true;
    }

    if (ai->HasStrategy("debug", BotState::BOT_STATE_NON_COMBAT))
    {
        ostringstream out;
        out << "do: ";
        out << action->getName();
        if (actionExecuted)
            out << " 1 (";
        else
            out << " 0 (";

        out << std::fixed << std::setprecision(2);
        out << action->getRelevance() << ")";

        if (!event.getSource().empty())
            out << " [" << event.getSource() << "]";

        out << " [reaction]";

        if(actionExecuted)
            out << " (duration: " << ((float)incomingReaction.GetDuration() / IN_MILLISECONDS) << "s)";

        ai->TellMasterNoFacing(out);
    }

    actionExecuted = actionExecutionListeners.OverrideResult(action, actionExecuted, event);
    actionExecutionListeners.After(action, actionExecuted, event);
    return actionExecuted;
}

ai::Action* ReactionEngine::InitializeAction(ActionNode* actionNode)
{
    Action* action = actionNode->getAction();
    if (!action)
    {
        action = aiObjectContext->GetAction(actionNode->getName());
        actionNode->setAction(action);
    }

    if (action != nullptr)
    {
        action->SetReaction(true);
    }

    return action;
}

void ReactionEngine::SetReactionDuration(const Action* action, uint32 duration)
{
    if (action && (IsReacting() || HasIncomingReaction()))
    {
        if (ongoingReaction.GetAction() == action)
        {
            ongoingReaction.SetDuration(duration);
        }
        else if (incomingReaction.GetAction() == action)
        {
            incomingReaction.SetDuration(duration);
        }
    }
}

void ReactionEngine::Reset()
{
    ongoingReaction.Reset();
    incomingReaction.Reset();
    aiReactionUpdateDelay = 0U;
}

bool ReactionEngine::CanUpdateAIReaction() const
{
    Player* bot = ai->GetBot();
    return (aiReactionUpdateDelay < 100U) && 
            bot->IsInWorld() &&
           !bot->IsBeingTeleported() &&
           !bot->IsTaxiFlying();
}
