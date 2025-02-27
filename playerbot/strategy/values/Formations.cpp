#include "botpch.h"
#include "../../playerbot.h"
#include "Formations.h"

#include "../../ServerFacade.h"
#include "Arrow.h"

using namespace ai;

WorldLocation Formation::NullLocation = WorldLocation();

bool IsSameLocation(WorldLocation const &a, WorldLocation const &b)
{
	return a.coord_x == b.coord_x && a.coord_y == b.coord_y && a.coord_z == b.coord_z && a.mapid == b.mapid;
}

bool Formation::IsNullLocation(WorldLocation const& loc)
{
	return IsSameLocation(loc, Formation::NullLocation);
}


WorldLocation MoveAheadFormation::GetLocation()
{
    Player* master = ai->GetGroupMaster();
    if (!master || master == bot)
        return WorldLocation();

    WorldLocation loc = GetLocationInternal();
    if (Formation::IsNullLocation(loc))
        return loc;

    float x = loc.coord_x;
    float y = loc.coord_y;
    float z = loc.coord_z;

    if (sServerFacade.isMoving(master)) {
        float ori = master->GetOrientation();
        float x1 = x + sPlayerbotAIConfig.tooCloseDistance * cos(ori);
        float y1 = y + sPlayerbotAIConfig.tooCloseDistance * sin(ori);
#ifdef MANGOSBOT_TWO
        float ground = master->GetMap()->GetHeight(master->GetPhaseMask(), x1, y1, z);
#else
        float ground = master->GetMap()->GetHeight(x1, y1, z);
#endif
        if (ground > INVALID_HEIGHT)
        {
            x = x1;
            y = y1;
        }
    }
#ifdef MANGOSBOT_TWO
    float ground = master->GetMap()->GetHeight(master->GetPhaseMask(), x, y, z);
#else
    float ground = master->GetMap()->GetHeight(x, y, z);
#endif
    if (ground <= INVALID_HEIGHT)
        return Formation::NullLocation;

    //z += CONTACT_DISTANCE;
    //bot->UpdateAllowedPositionZ(x, y, z);
    return WorldLocation(master->GetMapId(), x, y, z);
}

namespace ai
{
    class MeleeFormation : public FollowFormation
    {
    public:
        MeleeFormation(PlayerbotAI* ai) : FollowFormation(ai, "melee") {}
        virtual string GetTargetName() { return "master target"; }
    };

    class QueueFormation : public FollowFormation
    {
    public:
        QueueFormation(PlayerbotAI* ai) : FollowFormation(ai, "queue") {}
        virtual string GetTargetName() { return "line target"; }
    };

    class NearFormation : public MoveAheadFormation
    {
    public:
        NearFormation(PlayerbotAI* ai) : MoveAheadFormation(ai, "near") {}
        virtual WorldLocation GetLocationInternal()
        {
            Player* master = ai->GetGroupMaster();
            if (!master || master->GetMapId() != bot->GetMapId() || master->IsBeingTeleported())
                return WorldLocation();

            float range = sPlayerbotAIConfig.followDistance + master->GetObjectBoundingRadius();
            float angle = GetFollowAngle();
            float x = master->GetPositionX() + cos(angle) * range;
            float y = master->GetPositionY() + sin(angle) * range;
            float z = master->GetPositionZ();
#ifdef MANGOSBOT_TWO
            float ground = master->GetMap()->GetHeight(master->GetPhaseMask(), x, y, z);
#else
            float ground = master->GetMap()->GetHeight(x, y, z);
#endif
            //if (ground <= INVALID_HEIGHT)
            //    return Formation::NullLocation;

            // prevent going into terrain
            float ox, oy, oz;
            master->GetPosition(ox, oy, oz);
#ifdef MANGOSBOT_TWO
            master->GetMap()->GetHitPosition(ox, oy, oz + bot->GetCollisionHeight(), x, y, z, bot->GetPhaseMask(), -0.5f);
#else
            master->GetMap()->GetHitPosition(ox, oy, oz + bot->GetCollisionHeight(), x, y, z, -0.5f);
#endif

            if (!bot->IsFlying() && !bot->IsFreeFlying() && !bot->IsSwimming())
            {
                z += CONTACT_DISTANCE;
                bot->UpdateAllowedPositionZ(x, y, z);
            }
            return WorldLocation(master->GetMapId(), x, y, z);
        }

        virtual float GetMaxDistance() { return sPlayerbotAIConfig.followDistance; }
    };


    class ChaosFormation : public MoveAheadFormation
    {
    public:
        ChaosFormation(PlayerbotAI* ai) : MoveAheadFormation(ai, "chaos"), lastChangeTime(0) {}
        virtual WorldLocation GetLocationInternal()
        {
            Player* master = ai->GetGroupMaster();
            if (!master)
                return WorldLocation();

            float range = sPlayerbotAIConfig.followDistance;
			float angle = GetFollowAngle();

            time_t now = time(0);
            if (!lastChangeTime || now - lastChangeTime >= 3) {
                lastChangeTime = now;
                dx = (urand(0, 10) / 10.0 - 0.5) * sPlayerbotAIConfig.tooCloseDistance;
                dy = (urand(0, 10) / 10.0 - 0.5) * sPlayerbotAIConfig.tooCloseDistance;
                dr = sqrt(dx*dx + dy*dy);
            }

            float x = master->GetPositionX() + cos(angle) * range + dx;
            float y = master->GetPositionY() + sin(angle) * range + dy;
            float z = master->GetPositionZ();
#ifdef MANGOSBOT_TWO
            float ground = master->GetMap()->GetHeight(master->GetPhaseMask(), x, y, z);
#else
            float ground = master->GetMap()->GetHeight(x, y, z);
#endif
            //if (ground <= INVALID_HEIGHT)
            //    return Formation::NullLocation;

            if (!bot->IsFlying() && !bot->IsFreeFlying())
            {
                z += CONTACT_DISTANCE;
                bot->UpdateAllowedPositionZ(x, y, z);
            }
            return WorldLocation(master->GetMapId(), x, y, z);
        }

        virtual float GetMaxDistance() { return sPlayerbotAIConfig.followDistance + dr; }

    private:
        time_t lastChangeTime;
        float dx = 0, dy = 0, dr = 0;
    };

    class CircleFormation : public MoveFormation
    {
    public:
        CircleFormation(PlayerbotAI* ai) : MoveFormation(ai, "circle") {}
        virtual WorldLocation GetLocation()
        {
            float range = 2.0f;

            Unit* target = AI_VALUE(Unit*, "current target");
            Player* master = ai->GetGroupMaster();
            if (!target && target != bot)
                target = master;

            if (!target)
				return Formation::NullLocation;

            switch (bot->getClass())
            {
            case CLASS_HUNTER:
            case CLASS_MAGE:
            case CLASS_PRIEST:
            case CLASS_WARLOCK:
                range = ai->GetRange("flee");
                break;
            case CLASS_DRUID:
                if (!ai->IsTank(bot))
                    range = ai->GetRange("flee");
                break;
            case CLASS_SHAMAN:
                if (ai->IsHeal(bot))
                    range = ai->GetRange("flee");
                break;
            }

            float angle = GetFollowAngle();
            float x = target->GetPositionX() + cos(angle) * range;
            float y = target->GetPositionY() + sin(angle) * range;
            float z = target->GetPositionZ();
#ifdef MANGOSBOT_TWO
            float ground = target->GetMap()->GetHeight(target->GetPhaseMask(), x, y, z);
#else
            float ground = target->GetMap()->GetHeight(x, y, z);
#endif
            if (ground <= INVALID_HEIGHT)
                return Formation::NullLocation;

            if (!bot->IsFlying() && !bot->IsFreeFlying())
            {
                z += CONTACT_DISTANCE;
                bot->UpdateAllowedPositionZ(x, y, z);
            }
            return WorldLocation(bot->GetMapId(), x, y, z);
        }
    };

    class LineFormation : public MoveAheadFormation
    {
    public:
        LineFormation(PlayerbotAI* ai) : MoveAheadFormation(ai, "line") {}
        virtual WorldLocation GetLocationInternal()
        {
            Group* group = bot->GetGroup();
            if (!group)
                return Formation::NullLocation;

            float range = 2.0f;

            Player* master = ai->GetGroupMaster();
            if (!master)
                return Formation::NullLocation;

            float x = master->GetPositionX();
            float y = master->GetPositionY();
            float z = master->GetPositionZ();
            float orientation = master->GetOrientation();

            vector<Player*> players;
            for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
            {
                Player* member = gref->getSource();
                if (!member || bot->GetMapId() != member->GetMapId()) continue;
                if (member != master)
                    players.push_back(member);
            }

            players.insert(players.begin() + players.size() / 2, master);

            return MoveLine(players, 0.0f, x, y, z, orientation, range);
        }
    };

    class ShieldFormation : public MoveFormation
    {
    public:
        ShieldFormation(PlayerbotAI* ai) : MoveFormation(ai, "shield") {}
        virtual WorldLocation GetLocation()
        {
            Group* group = bot->GetGroup();
            if (!group)
                return Formation::NullLocation;

            float range = sPlayerbotAIConfig.followDistance;

            Player* master = ai->GetGroupMaster();
            if (!master)
                return Formation::NullLocation;

            float x = master->GetPositionX();
            float y = master->GetPositionY();
            float z = master->GetPositionZ();
            float orientation = master->GetOrientation();

            vector<Player*> tanks;
            vector<Player*> dps;
            for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
            {
                Player* member = gref->getSource();
                if (!member || bot->GetMapId() != member->GetMapId()) continue;
                if (member != master)
                {
                    if (ai->IsTank(member))
                        tanks.push_back(member);
                    else
                        dps.push_back(member);
                }
            }

            if (ai->IsTank(master))
                tanks.insert(tanks.begin() + (tanks.size() + 1) / 2, master);
            else
                dps.insert(dps.begin() + (dps.size() + 1) / 2, master);

            if (ai->IsTank(bot) && ai->IsTank(master))
            {
                return MoveLine(tanks, 0.0f, x, y, z, orientation, range);
            }
            if (!ai->IsTank(bot) && !ai->IsTank(master))
            {
                return MoveLine(dps, 0.0f, x, y, z, orientation, range);
            }
            if (ai->IsTank(bot) && !ai->IsTank(master))
            {
                float diff = tanks.size() % 2 == 0 ? -sPlayerbotAIConfig.tooCloseDistance / 2.0f : 0.0f;
                return MoveLine(tanks, diff, x + cos(orientation) * range, y + sin(orientation) * range, z, orientation, range);
            }
            if (!ai->IsTank(bot) && ai->IsTank(master))
            {
                float diff = dps.size() % 2 == 0 ? -sPlayerbotAIConfig.tooCloseDistance / 2.0f : 0.0f;
                return MoveLine(dps, diff, x - cos(orientation) * range, y - sin(orientation) * range, z, orientation, range);
            }
            return Formation::NullLocation;
        }
    };

    class FarFormation : public FollowFormation
    {
    public:
        FarFormation(PlayerbotAI* ai) : FollowFormation(ai, "far") {}
        virtual WorldLocation GetLocation()
        {
            float range = sPlayerbotAIConfig.farDistance;
            float followRange = sPlayerbotAIConfig.followDistance;

            Player* master = ai->GetGroupMaster();
            if (!master || master == bot)
                return Formation::NullLocation;

            if (sServerFacade.GetDistance2d(bot, master) <= range)
                return Formation::NullLocation;

            float angle = master->GetAngle(bot);
            float followAngle = GetFollowAngle();

            float x = master->GetPositionX() + cos(angle) * range + cos(followAngle) * followRange;
            float y = master->GetPositionY() + sin(angle) * range + sin(followAngle) * followRange;
            float z = master->GetPositionZ();
#ifdef MANGOSBOT_TWO
            float ground = master->GetMap()->GetHeight(master->GetPhaseMask(), x, y, z);
#else
            float ground = master->GetMap()->GetHeight(x, y, z);
#endif
            if (ground <= INVALID_HEIGHT)
            {
                float minDist = 0, minX = 0, minY = 0;
                for (double angle = 0.0f; angle <= 2 * M_PI; angle += M_PI / 16.0f)
                {
                    x = master->GetPositionX() + cos(angle) * range + cos(followAngle) * followRange;
                    y = master->GetPositionY() + sin(angle) * range + sin(followAngle) * followRange;
                    float dist = sServerFacade.GetDistance2d(bot, x, y);
#ifdef MANGOSBOT_TWO
                    float ground = master->GetMap()->GetHeight(master->GetPhaseMask(), x, y, z);
#else
                    float ground = master->GetMap()->GetHeight(x, y, z);
#endif
                    if (ground > INVALID_HEIGHT && (!minDist || minDist > dist))
                    {
                        minDist = dist;
                        minX = x;
                        minY = y;
                    }
                }
                if (minDist)
                {
                    if (!bot->IsFlying() && !bot->IsFreeFlying())
                    {
                        z += CONTACT_DISTANCE;
                        bot->UpdateAllowedPositionZ(minX, minY, z);
                    }
                    return WorldLocation(bot->GetMapId(), minX, minY, z);
                }

                return Formation::NullLocation;
            }

            // prevent going into terrain
            float ox, oy, oz;
            master->GetPosition(ox, oy, oz);
#ifdef MANGOSBOT_TWO
            master->GetMap()->GetHitPosition(ox, oy, oz + bot->GetCollisionHeight(), x, y, z, bot->GetPhaseMask(), -0.5f);
#else
            master->GetMap()->GetHitPosition(ox, oy, oz + bot->GetCollisionHeight(), x, y, z, -0.5f);
#endif
            if (!bot->IsFlying() && !bot->IsFreeFlying())
            {
                z += CONTACT_DISTANCE;
                bot->UpdateAllowedPositionZ(x, y, z);
            }
            return WorldLocation(bot->GetMapId(), x, y, z);
        }
    };
};

float Formation::GetFollowAngle()
{
    Player* master = ai->GetGroupMaster();
    Group* group = bot->GetGroup();
    PlayerbotAI* ai = bot->GetPlayerbotAI();
    int index = 1, total = 1;
    if (!group && master && !master->GetPlayerbotAI() && master->GetPlayerbotMgr())
    {
        for (PlayerBotMap::const_iterator i = master->GetPlayerbotMgr()->GetPlayerBotsBegin(); i != master->GetPlayerbotMgr()->GetPlayerBotsEnd(); ++i)
        {
            if (i->second == bot) index = total;
            total++;
        }
    }
    else if (group)
    {
        vector<Player*> roster;
        for (GroupReference *ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->getSource();
            if (!member || !sServerFacade.IsAlive(member) || bot->GetMapId() != member->GetMapId()) continue;
            if (member && member != master && !ai->IsTank(member) && !ai->IsHeal(member))
            {
                roster.insert(roster.begin() + roster.size() / 2, member);
            }
        }
        for (GroupReference *ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->getSource();
            if (!member || !sServerFacade.IsAlive(member) || bot->GetMapId() != member->GetMapId()) continue;
            if (member && member != master && ai->IsHeal(member))
            {
                roster.insert(roster.begin() + roster.size() / 2, member);
            }
        }
        bool left = true;
        for (GroupReference *ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->getSource();
            if (!member || !sServerFacade.IsAlive(member) || bot->GetMapId() != member->GetMapId()) continue;
            if (member && member != master && ai->IsTank(member))
            {
                if (left) roster.push_back(member); else roster.insert(roster.begin(), member);
                left = !left;
            }
        }

        for (vector<Player*>::iterator i = roster.begin(); i != roster.end(); ++i)
        {
            if (*i == bot) break;
            index++;
        }
        total = roster.size() + 1;
    }
    float start = (master ? master->GetOrientation() : 0.0f);
    return start + (0.125f + 1.75f * index / total + (total == 2 ? 0.125f : 0.0f)) * M_PI;
}

FormationValue::FormationValue(PlayerbotAI* ai) : ManualSetValue<Formation*>(ai, new NearFormation(ai), "formation")
{
}

string FormationValue::Save()
{
    return value ? value->getName() : "?";
}

bool FormationValue::Load(string formation)
{

    if (formation == "melee")
    {
        if (value) delete value;
        value = new MeleeFormation(ai);
    }
    else if (formation == "queue")
    {
        if (value) delete value;
        value = new QueueFormation(ai);
    }
    else if (formation == "chaos")
    {
        if (value) delete value;
        value = new ChaosFormation(ai);
    }
    else if (formation == "circle")
    {
        if (value) delete value;
        value = new CircleFormation(ai);
    }
    else if (formation == "line")
    {
        if (value) delete value;
        value = new LineFormation(ai);
    }
    else if (formation == "shield")
    {
        if (value) delete value;
        value = new ShieldFormation(ai);
    }
    else if (formation == "arrow")
    {
        if (value) delete value;
        value = new ArrowFormation(ai);
    }
    else if (formation == "near" || formation == "default")
    {
        if (value) delete value;
        value = new NearFormation(ai);
    }
    else if (formation == "far")
    {
        if (value) delete value;
        value = new FarFormation(ai);
    }
    else return false;

    return true;
}


bool SetFormationAction::Execute(Event& event)
{
    string formation = event.getParam();

    FormationValue* value = (FormationValue*)context->GetValue<Formation*>("formation");
    if (formation == "?" || formation.empty())
    {
        ostringstream str; str << "Formation: |cff00ff00" << value->Get()->getName();
        ai->TellMaster(str);
        return true;
    }

    if (formation == "show")
    {
        WorldLocation loc = value->Get()->GetLocation();
        if (!Formation::IsNullLocation(loc))
            ai->Ping(loc.coord_x, loc.coord_y);

        return true;
    }

    if (!value->Load(formation))
    {
        ostringstream str; str << "Invalid formation: |cffff0000" << formation;
        ai->TellMaster(str);
        ai->TellMaster("Please set to any of:|cffffffff near (default), queue, chaos, circle, line, shield, arrow, melee, far");
        return false;
    }

    ostringstream str; str << "Formation set to: " << formation;
    ai->TellMaster(str);
    return true;
}


WorldLocation MoveFormation::MoveLine(vector<Player*> line, float diff, float cx, float cy, float cz, float orientation, float range)
{
    if (line.size() < 5)
    {
        return MoveSingleLine(line, diff, cx, cy, cz, orientation, range);
    }

    int lines = ceil((double)line.size() / 5.0);
    for (int i = 0; i < lines; i++)
    {
        float radius = range * i;
        float x = cx + cos(orientation) * radius;
        float y = cy + sin(orientation) * radius;
        vector<Player*> singleLine;
        for (int j = 0; j < 5 && !line.empty(); j++)
        {
            singleLine.push_back(line[line.size() - 1]);
            line.pop_back();
        }

        WorldLocation loc = MoveSingleLine(singleLine, diff, x, y,cz, orientation, range);
        if (!Formation::IsNullLocation(loc))
            return loc;
    }

    return Formation::NullLocation;
}

WorldLocation MoveFormation::MoveSingleLine(vector<Player*> line, float diff, float cx, float cy, float cz, float orientation, float range)
{
    float count = line.size();
    float angle = orientation - M_PI / 2.0f;
    float x = cx + cos(angle) * (range * floor(count / 2.0f) + diff);
    float y = cy + sin(angle) * (range * floor(count / 2.0f) + diff);

    int index = 0;
    for (vector<Player*>::iterator i = line.begin(); i != line.end(); i++)
    {
        Player* member = *i;

        if (member == bot)
        {
            float angle = orientation + M_PI / 2.0f;
            float radius = range * index;

            float lx = x + cos(angle) * radius;
            float ly = y + sin(angle) * radius;
            float lz = cz;
#ifdef MANGOSBOT_TWO
            float ground = bot->GetMap()->GetHeight(bot->GetPhaseMask(), lx, ly, lz);
#else
            float ground = bot->GetMap()->GetHeight(lx, ly, lz);
#endif
            if (ground <= INVALID_HEIGHT)
                return Formation::NullLocation;

            if (!bot->IsFlying() && !bot->IsFreeFlying())
            {
                lz += CONTACT_DISTANCE;
                bot->UpdateAllowedPositionZ(lx, ly, lz);
            }
            return WorldLocation(bot->GetMapId(), lx, ly, lz);
        }

        index++;
    }

    return Formation::NullLocation;
}
