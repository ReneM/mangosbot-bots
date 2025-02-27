#include "botpch.h"
#include "../../playerbot.h"
#include "PartyMemberToHeal.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

using namespace ai;

class IsTargetOfHealingSpell : public SpellEntryPredicate
{
public:
    virtual bool Check(SpellEntry const* spell) {
        for (int i=0; i<3; i++) {
            if (spell->Effect[i] == SPELL_EFFECT_HEAL ||
                spell->Effect[i] == SPELL_EFFECT_HEAL_MAX_HEALTH ||
                spell->Effect[i] == SPELL_EFFECT_HEAL_MECHANICAL)
                return true;
        }
        return false;
    }

};

bool compareByHealth(const Unit *u1, const Unit *u2)
{
    return u1->GetHealthPercent() < u2->GetHealthPercent();
}

bool compareByMissingHealth(const Unit* u1, const Unit* u2)
{
    uint32 hp1 = u1->GetHealth();
    uint32 hpmax1 = u1->GetMaxHealth();
    uint32 hp2 = u2->GetHealth();
    uint32 hpmax2 = u2->GetMaxHealth();


    return (hpmax1 - hp1) > (hpmax2 - hp2);
}

Unit* PartyMemberToHeal::Calculate()
{

    IsTargetOfHealingSpell predicate;

    vector<Unit*> needHeals;
    vector<Unit*> tankTargets;

    if (bot->GetSelectionGuid())
    {
        Unit* target = ai->GetUnit(bot->GetSelectionGuid());
        if (target && target->GetObjectGuid() != bot->GetObjectGuid() && sServerFacade.IsFriendlyTo(bot, target) && 
#ifdef MANGOS
            target->HealthBelowPct(100))
#endif
#ifdef CMANGOS
            target->GetHealthPercent() < 100)
#endif
            if (Check(target))
                needHeals.push_back(target);
    }

    Group* group = bot->GetGroup();
    if (!group && needHeals.empty())
        return NULL;

    if (group)
    {
        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* player = gref->getSource();
            if (!Check(player) || !sServerFacade.IsAlive(player))
                continue;

            bool isTank = ai->IsTank(player);

            // do not heal dueling members
            if (player->duel && player->duel->opponent)
                continue;

            uint8 health = player->GetHealthPercent();
            if ((isTank || health < sPlayerbotAIConfig.almostFullHealth) && health < sPlayerbotAIConfig.almostFullHealth || (!isTank && !IsTargetOfSpellCast(player, predicate)))
                needHeals.push_back(player);

            Pet* pet = player->GetPet();
            if (pet && CanHealPet(pet))
            {
                health = pet->GetHealthPercent();
                if (health < sPlayerbotAIConfig.almostFullHealth || !IsTargetOfSpellCast(player, predicate))
                    needHeals.push_back(pet);
            }

            if (isTank && bot->IsInGroup(player))
                tankTargets.push_back(player);
        }
    }
    if (needHeals.empty() && tankTargets.empty())
        return NULL;

    if (needHeals.empty() && !tankTargets.empty())
        needHeals = tankTargets;

    sort(needHeals.begin(), needHeals.end(), compareByMissingHealth);

    int healerIndex = 0;
    if (group)
    {
        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* player = gref->getSource();
            if (!player) continue;
            if (player == bot) break;
            if (ai->IsHeal(player) && player->GetPlayerbotAI())
            {
                float percent = (float)player->GetPower(POWER_MANA) / (float)player->GetMaxPower(POWER_MANA) * 100.0;
                if (percent > sPlayerbotAIConfig.lowMana)
                    healerIndex++;
            }
        }
    }
    else
        healerIndex = 1;

    healerIndex = healerIndex % needHeals.size();
    return needHeals[healerIndex];
}

bool PartyMemberToHeal::CanHealPet(Pet* pet)
{
    return MINI_PET != pet->getPetType();
}

bool PartyMemberToHeal::Check(Unit* player)
{
    bool isBg = bot->InBattleGround();

    float maxDist = isBg ? ai->GetRange("spell") : (player->IsPlayer() && ai->IsTank((Player*)player)) ? 60.0f : 50.0f;

    if (!player)
        return false;

    if (player->GetObjectGuid() == bot->GetObjectGuid())
        return false;

    if (player->GetMapId() != bot->GetMapId())
        return false;

    if (!player->IsInWorld())
        return false;
                                                     
    if (sServerFacade.GetDistance2d(bot, player) > maxDist)
        return false;

    return true;
}

Unit* PartyMemberToProtect::Calculate()
{
    Group* group = bot->GetGroup();
    if (!group)
        return NULL;

    vector<Unit*> needProtect;

    list<ObjectGuid> attackers = ai->GetAiObjectContext()->GetValue<list<ObjectGuid>>("possible attack targets")->Get();
    for (list<ObjectGuid>::iterator i = attackers.begin(); i != attackers.end(); ++i)
    {
        Unit* unit = ai->GetUnit(*i);
        if (!unit)
            continue;

        bool isRanged = false;

        if (unit->AI())
        {
            if (unit->AI()->IsRangedUnit())
                isRanged = true;
        }

        Unit* pVictim = unit->GetVictim();
        if (!pVictim || !pVictim->IsPlayer())
            continue;

        if (pVictim == bot)
            continue;

        if (sServerFacade.GetDistance2d(pVictim, bot) > 30.0f)
            continue;

        float attackDistance = isRanged ? 30.0f : 10.0f;
        if (sServerFacade.GetDistance2d(pVictim, unit) > attackDistance)
            continue;

        if (ai->IsTank((Player*)pVictim) && pVictim->GetHealthPercent() > 10)
            continue;
        else if (pVictim->GetHealthPercent() > 30)
            continue;

        if (find(needProtect.begin(), needProtect.end(), pVictim) == needProtect.end())
        needProtect.push_back(pVictim);
    }

    if (needProtect.empty())
        return NULL;

    sort(needProtect.begin(), needProtect.end(), compareByHealth);

    return needProtect[0];
}
