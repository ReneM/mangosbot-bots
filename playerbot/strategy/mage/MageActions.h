#pragma once

#include "../../ServerFacade.h"
#include "../actions/GenericActions.h"

namespace ai
{
    BUFF_ACTION(CastFireWardAction, "fire ward");
    BUFF_ACTION(CastFrostWardAction, "frost ward");
    BUFF_ACTION(CastBlinkAction, "blink");
    SPELL_ACTION(CastIceLanceAction, "ice lance");

    class CastFireballAction : public CastSpellAction
    {
    public:
        CastFireballAction(PlayerbotAI* ai) : CastSpellAction(ai, "fireball") {}
    };

    class CastScorchAction : public CastSpellAction
    {
    public:
        CastScorchAction(PlayerbotAI* ai) : CastSpellAction(ai, "scorch") {}
        virtual bool isUseful() { return GetTarget() && !ai->HasAura("fire vulnerability", GetTarget(), true); }
    };

    class CastFireBlastAction : public CastSpellAction
    {
    public:
        CastFireBlastAction(PlayerbotAI* ai) : CastSpellAction(ai, "fire blast") {}
    };

    class CastArcaneBlastAction : public CastBuffSpellAction
    {
    public:
        CastArcaneBlastAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "arcane blast") {}
        virtual string GetTargetName() { return "current target"; }
    };

    class CastArcaneBarrageAction : public CastSpellAction
    {
    public:
        CastArcaneBarrageAction(PlayerbotAI* ai) : CastSpellAction(ai, "arcane barrage") {}
    };

    class CastArcaneMissilesAction : public CastSpellAction
    {
    public:
        CastArcaneMissilesAction(PlayerbotAI* ai) : CastSpellAction(ai, "arcane missiles") {}
    };

    class CastPyroblastAction : public CastSpellAction
    {
    public:
        CastPyroblastAction(PlayerbotAI* ai) : CastSpellAction(ai, "pyroblast") {}
    };

    class CastFlamestrikeAction : public CastSpellAction
    {
    public:
        CastFlamestrikeAction(PlayerbotAI* ai) : CastSpellAction(ai, "flamestrike") {}
    };

    SPELL_ACTION_U(CastFrostNovaAction, "frost nova", sServerFacade.IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", GetTargetName()), 10.0f));

	class CastFrostboltAction : public CastSpellAction
	{
	public:
		CastFrostboltAction(PlayerbotAI* ai) : CastSpellAction(ai, "frostbolt") {}
	};

	class CastBlizzardAction : public CastSpellAction
	{
	public:
		CastBlizzardAction(PlayerbotAI* ai) : CastSpellAction(ai, "blizzard") {}
        virtual ActionThreatType getThreatType() { return ActionThreatType::ACTION_THREAT_AOE; }
        virtual bool isUseful() { return CastSpellAction::isUseful() && ai->GetCombatStartTime() && (time(0) - ai->GetCombatStartTime()) > 10; }
	};

	class CastArcaneIntellectAction : public CastBuffSpellAction
    {
	public:
		CastArcaneIntellectAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "arcane intellect") {}
	};

	class CastArcaneIntellectOnPartyAction : public BuffOnPartyAction
    {
	public:
		CastArcaneIntellectOnPartyAction(PlayerbotAI* ai) : BuffOnPartyAction(ai, "arcane intellect") {}
	};

	class CastRemoveCurseAction : public CastCureSpellAction
    {
	public:
		CastRemoveCurseAction(PlayerbotAI* ai) : CastCureSpellAction(ai, "remove curse") {}
	};

	class CastRemoveLesserCurseAction : public CastCureSpellAction
    {
	public:
		CastRemoveLesserCurseAction(PlayerbotAI* ai) : CastCureSpellAction(ai, "remove lesser curse") {}
	};

	class CastIcyVeinsAction : public CastBuffSpellAction
    {
	public:
		CastIcyVeinsAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "icy veins") {}
	};

	class CastCombustionAction : public CastBuffSpellAction
    {
	public:
		CastCombustionAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "combustion") {}
	};

    BEGIN_SPELL_ACTION(CastCounterspellAction, "counterspell")
    END_SPELL_ACTION()

    class CastRemoveCurseOnPartyAction : public CurePartyMemberAction
    {
    public:
        CastRemoveCurseOnPartyAction(PlayerbotAI* ai) : CurePartyMemberAction(ai, "remove curse", DISPEL_CURSE) {}
    };

    class CastRemoveLesserCurseOnPartyAction : public CurePartyMemberAction
    {
    public:
        CastRemoveLesserCurseOnPartyAction(PlayerbotAI* ai) : CurePartyMemberAction(ai, "remove lesser curse", DISPEL_CURSE) {}
    };

	class CastConjureFoodAction : public CastBuffSpellAction
    {
	public:
		CastConjureFoodAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "conjure food") {}
	};

	class CastConjureWaterAction : public CastBuffSpellAction
    {
	public:
		CastConjureWaterAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "conjure water") {}
	};

	class CastIceBlockAction : public CastBuffSpellAction
    {
	public:
		CastIceBlockAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "ice block") {}
	};

    BUFF_ACTION(CastIceBarrierAction, "ice barrier");
    BUFF_ACTION(CastManaShieldAction, "mana shield");

    class CastMoltenArmorAction : public CastBuffSpellAction
    {
    public:
        CastMoltenArmorAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "molten armor") {}
    };

    class CastMageArmorAction : public CastBuffSpellAction
    {
    public:
        CastMageArmorAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "mage armor") {}
    };

    class CastIceArmorAction : public CastBuffSpellAction
    {
    public:
        CastIceArmorAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "ice armor") {}
    };

    class CastFrostArmorAction : public CastBuffSpellAction
    {
    public:
        CastFrostArmorAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "frost armor") {}
    };

    class CastPolymorphAction : public CastCrowdControlSpellAction
    {
    public:
        CastPolymorphAction(PlayerbotAI* ai) : CastCrowdControlSpellAction(ai, "polymorph") {}
        virtual bool Execute(Event& event)
        {
            vector<string> polySpells;
            polySpells.push_back("polymorph");
            if (bot->HasSpell(28271))
                polySpells.push_back("polymorph: turtle");
            if (bot->HasSpell(28272))
                polySpells.push_back("polymorph: pig");

            return ai->CastSpell(polySpells[urand(0, polySpells.size() - 1)], GetTarget());
        }
    };

	class CastSpellstealAction : public CastSpellAction
	{
	public:
		CastSpellstealAction(PlayerbotAI* ai) : CastSpellAction(ai, "spellsteal") {}
	};

	class CastLivingBombAction : public CastDebuffSpellAction
	{
	public:
	    CastLivingBombAction(PlayerbotAI* ai) : CastDebuffSpellAction(ai, "living bomb") {}
	};

	class CastDragonsBreathAction : public CastSpellAction
	{
	public:
	    CastDragonsBreathAction(PlayerbotAI* ai) : CastSpellAction(ai, "dragon's breath") {}
	};

	class CastBlastWaveAction : public CastSpellAction
	{
	public:
	    CastBlastWaveAction(PlayerbotAI* ai) : CastSpellAction(ai, "blast wave") {}
        virtual bool isUseful()
        {
            return sServerFacade.IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", GetTargetName()), 10.0f);
        }
	};

	class CastInvisibilityAction : public CastBuffSpellAction
	{
	public:
	    CastInvisibilityAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "invisibility") {}
	};

	class CastEvocationAction : public CastSpellAction
	{
	public:
	    CastEvocationAction(PlayerbotAI* ai) : CastSpellAction(ai, "evocation") {}
	    virtual string GetTargetName() { return "self target"; }
	};

    class CastCounterspellOnEnemyHealerAction : public CastSpellOnEnemyHealerAction
    {
    public:
	    CastCounterspellOnEnemyHealerAction(PlayerbotAI* ai) : CastSpellOnEnemyHealerAction(ai, "counterspell") {}
    };

    class CastArcanePowerAction : public CastBuffSpellAction
    {
    public:
        CastArcanePowerAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "arcane power") {}
    };

    class CastPresenceOfMindAction : public CastBuffSpellAction
    {
    public:
        CastPresenceOfMindAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "presence of mind") {}
    };

    SPELL_ACTION_U(CastArcaneExplosionAction, "arcane explosion", sServerFacade.IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", GetTargetName()), 10.0f));
    SPELL_ACTION(CastConeOfColdAction, "cone of cold");
    BUFF_ACTION(CastSummonWaterElementalAction, "summon water elemental");
}
