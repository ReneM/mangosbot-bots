#include "botpch.h"
#include "../../playerbot.h"
#include "UseItemAction.h"

#include "../../PlayerbotAIConfig.h"
#include "DBCStore.h"
#include "../../ServerFacade.h"
#include "../values/ItemUsageValue.h"
#include "../../TravelMgr.h"

using namespace ai;

bool UseItemAction::Execute(Event& event)
{
   string name = event.getParam();
   if (name.empty())
      name = getName();

   list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", name);
   list<ObjectGuid> gos = chat->parseGameobjects(name);

   if (gos.empty())
   {
      if (items.size() > 1)
      {
         list<Item*>::iterator i = items.begin();
         Item* itemTarget = *i++;
         Item* item = *i;
         if(item->IsPotion() || item->GetProto()->Class == ITEM_CLASS_CONSUMABLE)
             return UseItemAuto(item);
         else
             return UseItemOnItem(item, itemTarget);
      }
      else if (!items.empty())
         return UseItemAuto(*items.begin());
   }
   else
   {
      if (items.empty())
         return UseGameObject(*gos.begin());
      else
         return UseItemOnGameObject(*items.begin(), *gos.begin());
   }

    ai->TellError("No items (or game objects) available");
    return false;
}

bool UseItemAction::isPossible()
{
    return getName() == "use" || AI_VALUE2(uint32, "item count", getName()) > 0;
}

bool UseItemAction::UseGameObject(ObjectGuid guid)
{
    GameObject* go = ai->GetGameObject(guid);
    if (!go || !sServerFacade.isSpawned(go)
#ifdef CMANGOS
        || go->IsInUse()
#endif
        || go->GetGoState() != GO_STATE_READY)
        return false;

   go->Use(bot);
   ostringstream out; out << "Using " << chat->formatGameobject(go);
   ai->TellMasterNoFacing(out.str(), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
   return true;
}

bool UseItemAction::UseItemAuto(Item* item)
{
    uint8 bagIndex = item->GetBagSlot();
    uint8 slot = item->GetSlot();
    uint8 spell_index = 0;
    uint8 cast_count = 1;
    uint32 spellId = 0;
    ObjectGuid item_guid = item->GetObjectGuid();
#ifdef MANGOSBOT_ZERO
    uint16 targetFlag = TARGET_FLAG_SELF;
#else
    uint32 targetFlag = TARGET_FLAG_SELF;
#endif
    uint32 glyphIndex = 0;
    uint8 unk_flags = 0;

    ItemPrototype const* proto = item->GetProto();
    bool isDrink = proto->Spells[0].SpellCategory == 59;
    bool isFood = proto->Spells[0].SpellCategory == 11;

    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        // wrong triggering type
        if (proto->Spells[i].SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
            continue;

        if (proto->Spells[i].SpellId > 0)
        {
            spell_index = i;
        }
    }

    //Temporary fix for starting quests:
    if (uint32 questid = item->GetProto()->StartQuest)
    {
        Quest const* qInfo = sObjectMgr.GetQuestTemplate(questid);
        if (qInfo)
        {
            WorldPacket packet(CMSG_QUESTGIVER_ACCEPT_QUEST, 8 + 4 + 4);
            packet << item_guid;
            packet << questid;
            packet << uint32(0);
            bot->GetSession()->HandleQuestgiverAcceptQuestOpcode(packet);
            ostringstream out; out << "Got quest " << chat->formatQuest(qInfo);
            ai->TellMasterNoFacing(out.str(), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
            return true;
        }
    }

#ifdef MANGOSBOT_ZERO
    WorldPacket packet(CMSG_USE_ITEM);
    packet << bagIndex << slot << spell_index;
#endif
#ifdef MANGOSBOT_ONE
    WorldPacket packet(CMSG_USE_ITEM);
    packet << bagIndex << slot << spell_index << cast_count << item_guid;
#endif
#ifdef MANGOSBOT_TWO
    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        if (item->GetProto()->Spells[i].SpellId > 0)
        {
            spellId = item->GetProto()->Spells[i].SpellId;
            break;
        }
    }

    WorldPacket packet(CMSG_USE_ITEM, 1 + 1 + 1 + 4 + 8 + 4 + 1 + 8 + 1);
    packet << bagIndex << slot << cast_count << spellId << item_guid << glyphIndex << unk_flags;
#endif

    packet << targetFlag;
    packet.appendPackGUID(bot->GetObjectGuid());

    SetDuration(sPlayerbotAIConfig.globalCoolDown);
    if (proto->Class == ITEM_CLASS_CONSUMABLE && (proto->SubClass == ITEM_SUBCLASS_FOOD || proto->SubClass == ITEM_SUBCLASS_CONSUMABLE) &&
        (isFood || isDrink))
    {
        if (sServerFacade.IsInCombat(bot))
            return false;

        bot->addUnitState(UNIT_STAND_STATE_SIT);
        ai->InterruptSpell();

        float hp = bot->GetHealthPercent();
        float mp = bot->GetPower(POWER_MANA) * 100.0f / bot->GetMaxPower(POWER_MANA);
        float p;
        if (isDrink && isFood)
        {
            p = min(hp, mp);
            TellConsumableUse(item, "Feasting", p);
        }
        else if (isDrink)
        {
            p = mp;
            TellConsumableUse(item, "Drinking", p);
        }
        else if (isFood)
        {
            p = hp;
            TellConsumableUse(item, "Eating", p);
        }

        if(!bot->IsInCombat())
        {
            const float multiplier = bot->InBattleGround() ? 20000.0f : 27000.0f;
            const float duration = multiplier * ((100 - p) / 100.0f);
            SetDuration(duration);
        }
    }

    bot->GetSession()->HandleUseItemOpcode(packet);
    return true;

   //return UseItem(item, ObjectGuid(), nullptr);
}

bool UseItemAction::UseItemOnGameObject(Item* item, ObjectGuid go)
{
   return UseItem(item, go, nullptr);
}

bool UseItemAction::UseItemOnItem(Item* item, Item* itemTarget)
{
   return UseItem(item, ObjectGuid(), itemTarget);
}

bool UseItemAction::UseItem(Item* item, ObjectGuid goGuid, Item* itemTarget, Unit* unitTarget)
{
   if (bot->CanUseItem(item) != EQUIP_ERR_OK)
      return false;

   if (bot->IsNonMeleeSpellCasted(true))
      return false;

   uint8 bagIndex = item->GetBagSlot();
   uint8 slot = item->GetSlot();
   uint8 spell_index = 0;
   uint8 cast_count = 1;
   uint32 spellId = 0;
#ifndef MANGOSBOT_TWO
   ObjectGuid item_guid = item->GetObjectGuid();
#else
   ObjectGuid item_guid = item->GetObjectGuid();
#endif
   uint32 glyphIndex = 0;
   uint8 unk_flags = 0;

#ifdef MANGOSBOT_ZERO
   uint16 targetFlag = TARGET_FLAG_SELF;
#else
   uint32 targetFlag = TARGET_FLAG_SELF;
#endif

   if (itemTarget)
   {
       for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
       {
           if (item->GetProto()->Spells[i].SpellId > 0)
           {
               spellId = item->GetProto()->Spells[i].SpellId;
               spell_index = i;
               break;
           }
       }
   }

#ifdef MANGOSBOT_ZERO
   WorldPacket packet(CMSG_USE_ITEM);
   packet << bagIndex << slot << spell_index;
#endif
#ifdef MANGOSBOT_ONE
   WorldPacket packet(CMSG_USE_ITEM);
   packet << bagIndex << slot << spell_index << cast_count << item_guid;
#endif
#ifdef MANGOSBOT_TWO
   for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
   {
       if (item->GetProto()->Spells[i].SpellId > 0)
       {
           spellId = item->GetProto()->Spells[i].SpellId;
           break;
       }
   }

   WorldPacket packet(CMSG_USE_ITEM, 1 + 1 + 1 + 4 + 8 + 4 + 1 + 8 + 1);
   packet << bagIndex << slot << cast_count << spellId << item_guid << glyphIndex << unk_flags;
#endif

   bool targetSelected = false;
   ostringstream out; out << "Using " << chat->formatItem(item->GetProto());
   if ((int)item->GetProto()->Stackable > 1)
   {
      uint32 count = item->GetCount();
      if (count > 1)
         out << " (" << count << " available) ";
      else
         out << " (the last one!)";
   }

   if (goGuid)
   {
      GameObject* go = ai->GetGameObject(goGuid);
      if (!go || !sServerFacade.isSpawned(go))
         return false;

#ifdef MANGOS
        targetFlag = TARGET_FLAG_OBJECT;
#endif
#ifdef CMANGOS
        targetFlag = TARGET_FLAG_GAMEOBJECT;
#endif
        packet << targetFlag;
        packet.appendPackGUID(goGuid.GetRawValue());
        out << " on " << chat->formatGameobject(go);
        targetSelected = true;
    }

   if (itemTarget)
   {
#ifndef MANGOSBOT_ZERO
      if (item->GetProto()->Class == ITEM_CLASS_GEM)
      {
         bool fit = SocketItem(itemTarget, item) || SocketItem(itemTarget, item, true);
         if (!fit)
            ai->TellMaster("Socket does not fit", PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
         return fit;
      }
      else
      {
#endif
      targetFlag = TARGET_FLAG_ITEM;
      packet << targetFlag;
      packet.appendPackGUID(itemTarget->GetObjectGuid());
      out << " on " << chat->formatItem(itemTarget->GetProto());
      targetSelected = true;
#ifndef MANGOSBOT_ZERO
      }
#endif
   }

   Player* master = GetMaster();
   if (!targetSelected && item->GetProto()->Class != ITEM_CLASS_CONSUMABLE && master && ai->HasActivePlayerMaster() && !selfOnly)
   {
      ObjectGuid masterSelection = master->GetSelectionGuid();
      if (masterSelection)
      {
         Unit* unit = ai->GetUnit(masterSelection);
         if (unit && item->IsTargetValidForItemUse(unit))
         {
            targetFlag = TARGET_FLAG_UNIT;
            packet << targetFlag << masterSelection.WriteAsPacked();
            out << " on " << unit->GetName();
            targetSelected = true;
         }
      }
   }

   if (!targetSelected && item->GetProto()->Class != ITEM_CLASS_CONSUMABLE && unitTarget)
   {
       if (item->IsTargetValidForItemUse(unitTarget))
       {
           targetFlag = TARGET_FLAG_UNIT;
           packet << targetFlag << unitTarget->GetObjectGuid().WriteAsPacked();
           out << " on " << unitTarget->GetName();
           targetSelected = true;
       }
   }

   if (unitTarget)
   {
       uint32 spellid = 0;
       for (int i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
       {
           if (item->GetProto()->Spells[i].SpellTrigger == ITEM_SPELLTRIGGER_ON_USE)
           {
               spellid = item->GetProto()->Spells[i].SpellId;
               spell_index = i;
               break;
           }
       }

       if (SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellid))
       {
           if (!bot->IsSpellReady(spellid, item->GetProto()))
               return false;

           if (spellInfo->Targets & TARGET_FLAG_DEST_LOCATION)
           {
               targetFlag = TARGET_FLAG_DEST_LOCATION;
               Position pos = unitTarget->GetPosition();
               SpellCastTargets targets;
               targets.setDestination(pos.x, pos.y, pos.z);
               targets.m_targetMask = TARGET_FLAG_DEST_LOCATION;

               const auto spell = new Spell(bot, spellInfo, false, bot->GetObjectGuid());
               spell->m_targets = targets;
               if (spell->CheckRange(true) != SPELL_CAST_OK)
               {
                   delete spell;
                   return false;
               }
               delete spell;
#ifdef MANGOSBOT_ZERO
               bot->CastItemUseSpell(item, targets, spell_index);
#endif
#ifdef MANGOSBOT_ONE
               bot->CastItemUseSpell(item, targets, 1, spell_index);
#endif
#ifdef MANGOSBOT_TWO
               bot->CastItemUseSpell(item, targets, 1, 0, spellid);
#endif

               return true;
           }
       }
   }

   if (uint32 questid = item->GetProto()->StartQuest)
   {
      Quest const* qInfo = sObjectMgr.GetQuestTemplate(questid);
      if (qInfo)
      {
         WorldPacket packet(CMSG_QUESTGIVER_ACCEPT_QUEST, 8 + 4 + 4);
         packet << item_guid;
         packet << questid;
         packet << uint32(0);
         bot->GetSession()->HandleQuestgiverAcceptQuestOpcode(packet);
         ostringstream out; out << "Got quest " << chat->formatQuest(qInfo);
         ai->TellMasterNoFacing(out.str(), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
         return true;
      }
   }

   //bot->clearUnitState(UNIT_STAT_CHASE);
   //bot->clearUnitState(UNIT_STAT_FOLLOW);

   if (sServerFacade.isMoving(bot))
   {
       ai->StopMoving();
       SetDuration(sPlayerbotAIConfig.globalCoolDown);
      return false;
   }

   for (int i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
   {
      spellId = item->GetProto()->Spells[i].SpellId;
      if (!spellId)
         continue;

      if (!ai->CanCastSpell(spellId, bot, 0, false))
         continue;

      const SpellEntry* const pSpellInfo = sServerFacade.LookupSpellInfo(spellId);
      if (pSpellInfo->Targets & TARGET_FLAG_ITEM)
      {
         Item* itemForSpell = AI_VALUE2(Item*, "item for spell", spellId);
         if (!itemForSpell)
            continue;

         if (itemForSpell->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT))
            continue;

         if (bot->GetTrader())
         {
            if (selfOnly)
               return false;

            targetFlag = TARGET_FLAG_TRADE_ITEM;
            packet << targetFlag << (uint8)1 << (uint64)TRADE_SLOT_NONTRADED;
            targetSelected = true;
            out << " on traded item";
         }
         else
         {
            targetFlag = TARGET_FLAG_ITEM;
            packet << targetFlag;
            packet.appendPackGUID(itemForSpell->GetObjectGuid());
            targetSelected = true;
            out << " on " << chat->formatItem(itemForSpell->GetProto());
         }

         Spell *spell = new Spell(bot, pSpellInfo, false);
         ai->WaitForSpellCast(spell);
         delete spell;
      }
      break;
   }

   if (!targetSelected)
   {
      targetFlag = TARGET_FLAG_SELF;
      packet << targetFlag;
      packet.appendPackGUID(bot->GetObjectGuid());
      targetSelected = true;
      out << " on self";
   }

   if (!spellId)
       return false;

   SetDuration(sPlayerbotAIConfig.globalCoolDown);
   ai->TellMasterNoFacing(out.str(), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);

   bot->GetSession()->HandleUseItemOpcode(packet);
   return true;
}

void UseItemAction::TellConsumableUse(Item* item, string action, float percent)
{
    ostringstream out;
    out << action << " " << chat->formatItem(item->GetProto());
    if ((int)item->GetProto()->Stackable > 1) out << "/x" << item->GetCount();
    out << " (" << round(percent) << "%)";
    ai->TellMasterNoFacing(out.str(), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
}

#ifndef MANGOSBOT_ZERO
bool UseItemAction::SocketItem(Item* item, Item* gem, bool replace)
{
   WorldPacket* const packet = new WorldPacket(CMSG_SOCKET_GEMS);
   *packet << item->GetObjectGuid();

   bool fits = false;
   for (uint32 enchant_slot = SOCK_ENCHANTMENT_SLOT; enchant_slot < SOCK_ENCHANTMENT_SLOT + MAX_GEM_SOCKETS; ++enchant_slot)
   {
      uint8 SocketColor = item->GetProto()->Socket[enchant_slot - SOCK_ENCHANTMENT_SLOT].Color;
      GemPropertiesEntry const* gemProperty = sGemPropertiesStore.LookupEntry(gem->GetProto()->GemProperties);
      if (gemProperty && (gemProperty->color & SocketColor))
      {
         if (fits)
         {
            *packet << ObjectGuid();
            continue;
         }

         uint32 enchant_id = item->GetEnchantmentId(EnchantmentSlot(enchant_slot));
         if (!enchant_id)
         {
            *packet << gem->GetObjectGuid();
            fits = true;
            continue;
         }

         SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
         if (!enchantEntry || !enchantEntry->GemID)
         {
            *packet << gem->GetObjectGuid();
            fits = true;
            continue;
         }

         if (replace && enchantEntry->GemID != gem->GetProto()->ItemId)
         {
            *packet << gem->GetObjectGuid();
            fits = true;
            continue;
         }

      }

      *packet << ObjectGuid();
   }

   if (fits)
   {
      ostringstream out; out << "Socketing " << chat->formatItem(item->GetProto());
      out << " with " << chat->formatItem(gem->GetProto());
      ai->TellMaster(out, PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);

      bot->GetSession()->HandleSocketOpcode(*packet);
   }
   return fits;
}
#endif

bool UseItemIdAction::Execute(Event& event)
{
    return CastItemSpell(GetItemId(), GetTarget());
}

bool UseItemIdAction::isPossible()
{
    if (uint32 itemId = GetItemId())
    {
        if (HasSpellCooldown(itemId))
            return false;

        if (!bot->HasItemCount(itemId, 1) && !ai->HasCheat(BotCheatMask::item))
            return false;
    }

    return true;
}

bool UseItemIdAction::HasSpellCooldown(const uint32 itemId)
{
    ItemPrototype const* proto = sObjectMgr.GetItemPrototype(itemId);

    if (!proto)
        return false;

    uint32 spellId = 0;
    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        if (proto->Spells[i].SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
        {
            continue;
        }

        if (proto->Spells[i].SpellId > 0)
        {
            if (!sServerFacade.IsSpellReady(bot, proto->Spells[i].SpellId))
                return true;

            if (!sServerFacade.IsSpellReady(bot, proto->Spells[i].SpellId, itemId))
                return true;
        }
    }

    return false;
}

bool UseItemIdAction::CastItemSpell(uint32 itemId, Unit* target)
{
    ItemPrototype const* proto = sObjectMgr.GetItemPrototype(itemId);

    if (!proto)
        return false;

    Item* item = nullptr;

    if (!ai->HasCheat(BotCheatMask::item)) //If bot has no item cheat it needs an item to cast.
    {
        list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", chat->formatQItem(itemId));

        if (items.empty())
            return false;

        item = items.front();
    }

    SpellCastTargets targets;
    if (target)
    {
        targets.setUnitTarget(target);
        targets.setDestination(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
    }
    else
        targets.m_targetMask = TARGET_FLAG_SELF;
        
    // use triggered flag only for items with many spell casts and for not first cast
    int count = 0;

    for (int i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        _Spell const& spellData = proto->Spells[i];

        // no spell
        if (!spellData.SpellId)
            continue;

        // wrong triggering type
#ifdef MANGOSBOT_ZERO
        if (spellData.SpellTrigger != ITEM_SPELLTRIGGER_ON_USE && spellData.SpellTrigger != ITEM_SPELLTRIGGER_ON_NO_DELAY_USE)
#else
        if (spellData.SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
#endif
            continue;

        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellData.SpellId);
        if (!spellInfo)
        {
            sLog.outError("Player::CastItemUseSpell: Item (Entry: %u) in have wrong spell id %u, ignoring", proto->ItemId, spellData.SpellId);
            continue;
        }

        Spell* spell = new Spell(bot, spellInfo, (count > 0) ? TRIGGERED_OLD_TRIGGERED : TRIGGERED_NONE);

        if (item)
        {
            spell->SetCastItem(item);
            item->SetUsedInSpell(true);
        }

        spell->m_clientCast = true;

        bool result = (spell->SpellStart(&targets) == SPELL_CAST_OK);

        if (!result)
            return false;

        bot->AddCooldown(*spellInfo, proto, false);

        ++count;
    }

    return count;
}

bool UseSpellItemAction::isUseful()
{
   return AI_VALUE2(bool, "spell cast useful", getName());
}

bool UseHearthStone::Execute(Event& event)
{
    if (bot->IsMoving())
    {
        ai->StopMoving();
    }

    if (bot->IsMounted())
    {
        if (bot->IsFlying() && WorldPosition(bot).currentHeight() > 10.0f)
            return false;

        WorldPacket emptyPacket;
        bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
    }

    const bool used = UseItemAction::Execute(event);
    if (used)
    {
        RESET_AI_VALUE(bool, "combat::self target");
        RESET_AI_VALUE(WorldPosition, "current position");
        SetDuration(10000U); // 10s
    }

    return used;
}

bool UseRandomRecipe::isUseful()
{
   return !bot->IsInCombat() && !ai->HasActivePlayerMaster() && !bot->InBattleGround();
}

bool UseRandomRecipe::Execute(Event& event)
{
    list<Item*> recipes = AI_VALUE2(list<Item*>, "inventory items", "recipe");   
    string recipeName = "";
    for (auto& recipe : recipes)
    {
        recipeName = recipe->GetProto()->Name1;
    }

    if (recipeName.empty())
        return false;

    Event useItemEvent = Event(name, recipeName);
    const bool used = UseItemAction::Execute(useItemEvent);
    if (used)
    {
        SetDuration(3000U); // 3s
    }

    return used;
}

bool UseRandomQuestItem::isUseful()
{
    return !ai->HasActivePlayerMaster() && !bot->InBattleGround() && !bot->IsTaxiFlying();
}

bool UseRandomQuestItem::Execute(Event& event)
{
    Unit* unitTarget = nullptr;
    ObjectGuid goTarget = ObjectGuid();

    list<Item*> questItems = AI_VALUE2(list<Item*>, "inventory items", "quest");
    if (questItems.empty())
        return false;

    Item* item = nullptr;
    for (uint8 i = 0; i< 5;i++)
    {
        auto itr = questItems.begin();
        std::advance(itr, urand(0, questItems.size()- 1));
        Item* questItem = *itr;

        ItemPrototype const* proto = questItem->GetProto();

        if (proto->StartQuest)
        {
            Quest const* qInfo = sObjectMgr.GetQuestTemplate(proto->StartQuest);
            if (bot->CanTakeQuest(qInfo, false))
            {
                item = questItem;
                break;
            }
        }

        uint32 spellId = proto->Spells[0].SpellId;
        if (spellId)
        {
            SpellEntry const* spellInfo = sServerFacade.LookupSpellInfo(spellId);

            list<ObjectGuid> npcs = AI_VALUE(list<ObjectGuid>, ("nearest npcs"));
            for (auto& npc : npcs)
            {
                Unit* unit = ai->GetUnit(npc);
                if (questItem->IsTargetValidForItemUse(unit) || ai->CanCastSpell(spellId, unit, 0, false))
                {
                    item = questItem;
                    unitTarget = unit;
                    break;
                }
            }

            list<ObjectGuid> gos = AI_VALUE(list<ObjectGuid>, ("nearest game objects"));
            for (auto& go : gos)
            {
                GameObject* gameObject = ai->GetGameObject(go);
                GameObjectInfo const* goInfo = gameObject->GetGOInfo();
                if (!goInfo->GetLockId())
                    continue;

                LockEntry const* lock = sLockStore.LookupEntry(goInfo->GetLockId());

                for (uint8 i = 0; i < MAX_LOCK_CASE; ++i)
                {
                    if (!lock->Type[i])
                        continue;
                    if (lock->Type[i] != LOCK_KEY_ITEM)
                        continue;

                    if (lock->Index[i] == proto->ItemId)
                    {
                        item = questItem;
                        goTarget = go;
                        unitTarget = nullptr;
                        break;
                    }
                }               
            }
        }
    }

    if (!item)
        return false;

    if (!goTarget && !unitTarget)
        return false;

    const bool used = UseItem(item, goTarget, nullptr, unitTarget);
    if (used)
    {
        SetDuration(sPlayerbotAIConfig.globalCoolDown);
    }

    return used;
}