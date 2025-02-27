#pragma once

#include "Action.h"
#include "Queue.h"
#include "Trigger.h"
#include "Multiplier.h"
#include "AiObjectContext.h"
#include "Strategy.h"
#include "BotState.h"

namespace ai
{
    class ActionExecutionListener
    {
    public:
        virtual bool Before(Action* action, const Event& event) = 0;
        virtual bool AllowExecution(Action* action, const Event& event) = 0;
        virtual void After(Action* action, bool executed, const Event& event) = 0;
        virtual bool OverrideResult(Action* action, bool executed, const Event& event) = 0;
        virtual ~ActionExecutionListener() {};
    };

    // -----------------------------------------------------------------------------------------------------------------------

    class ActionExecutionListeners : public ActionExecutionListener
    {
    public:
        virtual ~ActionExecutionListeners();

    // ActionExecutionListener
    public:
        virtual bool Before(Action* action, const Event& event);
        virtual bool AllowExecution(Action* action, const Event& event);
        virtual void After(Action* action, bool executed, const Event& event);
        virtual bool OverrideResult(Action* action, bool executed, const Event& event);

    public:
        void Add(ActionExecutionListener* listener)
        {
            listeners.push_back(listener);
        }
        void Remove(ActionExecutionListener* listener)
        {
            listeners.remove(listener);
        }

    private:
        std::list<ActionExecutionListener*> listeners;
    };

    // -----------------------------------------------------------------------------------------------------------------------

    enum ActionResult
    {
        ACTION_RESULT_UNKNOWN,
        ACTION_RESULT_OK,
        ACTION_RESULT_IMPOSSIBLE,
        ACTION_RESULT_USELESS,
        ACTION_RESULT_FAILED
    };

    class Engine : public PlayerbotAIAware
    {
    public:
        Engine(PlayerbotAI* ai, AiObjectContext *factory, BotState state);

	    void Init();
        void addStrategy(string name);
		void addStrategies(string first, ...);
        bool removeStrategy(string name, bool init = true);
        bool HasStrategy(string name);
        Strategy* GetStrategy(string name) const;
        void removeAllStrategies();
        void toggleStrategy(string name);
        std::string ListStrategies();
        list<string> GetStrategies();
		bool ContainsStrategy(StrategyType type);
		void ChangeStrategy(string names, string engineType = "");
		string GetLastAction() { return lastAction; }

    public:
	    virtual bool DoNextAction(Unit*, int depth = 0, bool minimal = false);
	    ActionResult ExecuteAction(string name, Event& event, string qualifier = "");
        bool CanExecuteAction(string name, string qualifier = "", bool isPossible = true, bool isUseful = true);

    public:
        void AddActionExecutionListener(ActionExecutionListener* listener)
        {
            actionExecutionListeners.Add(listener);
        }
        void removeActionExecutionListener(ActionExecutionListener* listener)
        {
            actionExecutionListeners.Remove(listener);
        }

    public:
	    virtual ~Engine(void);

    protected:
        bool MultiplyAndPush(NextAction** actions, float forceRelevance, bool skipPrerequisites, const Event& event, const char* pushType);
        void Reset();
        void ProcessTriggers(bool minimal);
        void PushDefaultActions();
        void PushAgain(ActionNode* actionNode, float relevance, const Event& event);
        ActionNode* CreateActionNode(string name);
        virtual Action* InitializeAction(ActionNode* actionNode);
        virtual bool ListenAndExecute(Action* action, Event& event);

    private:
        void LogAction(const char* format, ...);
        void LogValues();

    protected:
	    Queue queue;
	    std::list<TriggerNode*> triggers;
        std::list<Multiplier*> multipliers;
        AiObjectContext* aiObjectContext;
        std::map<string, Strategy*> strategies;
        float lastRelevance;
        std::string lastAction;
        ActionExecutionListeners actionExecutionListeners;
        BotState state;

    public:
		bool testMode;
        bool initMode = true;
    };
}
