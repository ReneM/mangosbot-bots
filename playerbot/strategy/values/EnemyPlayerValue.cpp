#include "botpch.h"
#include "../../playerbot.h"
#include "EnemyPlayerValue.h"
#include "TargetValue.h"

using namespace ai;
using namespace std;

list<ObjectGuid> EnemyPlayersValue::Calculate()
{
    list<ObjectGuid> result;
    if (ai->AllowActivity(ALL_ACTIVITY))
    {
        if (bot->IsInWorld() && !bot->IsBeingTeleported())
        {
            // Check if we only need one attacker
            bool getOne = false;
            if (!qualifier.empty())
            {
                getOne = stoi(qualifier);
            }

            if (getOne)
            {
                // Try to get one enemy target
                result = AI_VALUE2(list<ObjectGuid>, "possible attack targets", 1);
                ApplyFilter(result, getOne);
            }

            // If the one enemy player failed, retry with multiple possible attack targets
            if (result.empty())
            {
                result = AI_VALUE(list<ObjectGuid>, "possible attack targets");
                ApplyFilter(result, getOne);
            }
        }
    }

    return result;
}

bool EnemyPlayersValue::IsValid(Unit* target, Player* player)
{
    if (target)
    {
        // If the target is a player
        Player* enemyPlayer = dynamic_cast<Player*>(target);
        if (enemyPlayer)
        {
            // If the target is friendly to the player
            if (sServerFacade.IsFriendlyTo(target, player))
            {
                return false;
            }

            /*
            // Check if too far away (Do we need this?)
            const float maxPvPDistance = GetMaxAttackDistance(player);
            const bool inCannon = player->GetPlayerbotAI() && player->GetPlayerbotAI()->IsInVehicle(false, true);
            uint32 const pvpDistance = (inCannon || player->GetHealth() > enemyPlayer->GetHealth()) ? maxPvPDistance : 20.0f;
            if (!player->IsWithinDist(enemyPlayer, pvpDistance, false))
            {
                return false;
            }
            */

            return true;
        }
    }

    return false;
}

void EnemyPlayersValue::ApplyFilter(list<ObjectGuid>& targets, bool getOne)
{
    list<ObjectGuid> filteredTargets;
    for (const ObjectGuid& targetGuid : targets)
    {
        Unit* target = ai->GetUnit(targetGuid);
        if (IsValid(target, bot))
        {
            filteredTargets.push_back(target->GetObjectGuid());

            if (getOne)
            {
                break;
            }
        }
    }

    targets = filteredTargets;
}

bool HasEnemyPlayersValue::Calculate()
{
    return !context->GetValue<list<ObjectGuid>>("enemy player targets", 1)->Get().empty();
}

Unit* EnemyPlayerValue::Calculate()
{
    // Prioritize the duel opponent
    if(bot->duel && bot->duel->opponent && !sServerFacade.IsFriendlyTo(bot->duel->opponent, bot))
    {
        return bot->duel->opponent;
    }

    Unit* bestEnemyPlayer = nullptr;
    float bestEnemyPlayerDistance = 999999999.0f;
    uint32 bestEnemyPlayerHealth = 99999999;

    const bool isMelee = !ai->IsRanged(bot);

    list<ObjectGuid> enemyPlayers = AI_VALUE(list<ObjectGuid>, "enemy player targets");
    for (const ObjectGuid& targetGuid : enemyPlayers)
    {
        Unit* target = ai->GetUnit(targetGuid);
        if(target)
        {
            // Prioritize an enemy player if it has a battleground flag
            if ((bot->GetTeam() == HORDE && target->HasAura(23333)) ||
                (bot->GetTeam() == ALLIANCE && target->HasAura(23335)))
            {
                bestEnemyPlayer = target;
                break;
            }

            if (isMelee)
            {
                // Score best enemy player based on lowest distance
                const float distanceToEnemyPlayer = target->GetDistance(bot, false);
                if (distanceToEnemyPlayer < bestEnemyPlayerDistance)
                {
                    bestEnemyPlayerDistance = distanceToEnemyPlayer;
                    bestEnemyPlayer = target;
                }
            }
            else
            {
                // Score best enemy player based on lowest health
                const uint32 enemyPlayerHealth = target->GetHealth();
                if (enemyPlayerHealth < bestEnemyPlayerHealth)
                {
                    bestEnemyPlayerHealth = enemyPlayerHealth;
                    bestEnemyPlayer = target;
                }
            }
        }
    }

    return bestEnemyPlayer;
}


float EnemyPlayerValue::GetMaxAttackDistance(Player* bot)
{
    if (!bot->GetBattleGround())
        return 60.0f;

    if (bot->InBattleGround())
    {
        BattleGround* bg = bot->GetBattleGround();
        if (!bg)
            return 40.0f;

        BattleGroundTypeId bgType = bg->GetTypeId();

#ifdef MANGOSBOT_TWO
        if (bgType == BATTLEGROUND_RB)
            bgType = bg->GetTypeId(true);

        if (bgType == BATTLEGROUND_IC)
        {
            if (bot->GetPlayerbotAI()->IsInVehicle(false, true))
                return 120.0f;
        }
#endif
        if (bgType == BATTLEGROUND_AV)
        {
            bool strifeTime = bg->GetStartTime() < (uint32)(20 * MINUTE * IN_MILLISECONDS);
            return strifeTime ? 40.0f : 10.0f;
        }
    }

    return 40.0f;
}
