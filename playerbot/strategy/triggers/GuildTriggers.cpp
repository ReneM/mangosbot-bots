#pragma once
#include "botpch.h"
#include "../../playerbot.h"
#include "GuildTriggers.h"
#include "ServerFacade.h"
#include "../values/BudgetValues.h"
#include "../../TravelMgr.h"

using namespace ai;

bool BuyTabardTrigger::IsActive()
{
	if (!bot->GetGuildId())
		return false;

	if(context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling())
		return false;

	bool inCity = false;
	AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(sServerFacade.GetAreaId(bot));
	if (areaEntry)
	{
		if (areaEntry->zone)
			areaEntry = GetAreaEntryByAreaID(areaEntry->zone);

		if (areaEntry && areaEntry->flags & AREA_FLAG_CAPITAL)
			inCity = true;
	}

	if (!inCity)
		return false;

	if(AI_VALUE2(uint32, "item count", chat->formatQItem(5976)))
		return false;
		
    if(AI_VALUE2(uint32, "free money for", uint32(NeedMoneyFor::guild)) < 10000)
		return false;

	return true;
};

bool LeaveLargeGuildTrigger::IsActive()
{
	if (!bot->GetGuildId())
		return false;

	if (ai->IsRealPlayer())
		return false;

	if (ai->IsAlt())
		return false;

	if (ai->IsInRealGuild())
		return false;

	GuilderType type = ai->GetGuilderType();

	Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());

	Player* leader = sObjectMgr.GetPlayer(guild->GetLeaderGuid());

	//Only leave the guild if we know the leader is not a real player.
	if (!leader || !leader->GetPlayerbotAI() || leader->GetPlayerbotAI()->IsRealPlayer())
		return false;

	if (type == GuilderType::SOLO && guild->GetLeaderGuid() != bot->GetObjectGuid())
		return true;

	uint32 members = guild->GetMemberSize();
	uint32 maxMembers = uint8(type);

	return members > maxMembers;
}

