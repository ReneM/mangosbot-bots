#pragma once

#include "GenericActions.h"
#include "NonCombatActions.h"
#include "EmoteAction.h"
#include "AddLootAction.h"
#include "LootAction.h"
#include "AddLootAction.h"
#include "LootRollAction.h"
#include "StayActions.h"
#include "FollowActions.h"
#include "ChangeStrategyAction.h"
#include "ChooseTargetActions.h"
#include "SuggestWhatToDoAction.h"
#include "PositionAction.h"
#include "AttackAction.h"
#include "CheckMailAction.h"
#include "CheckValuesAction.h"
#include "ChooseRpgTargetAction.h"
#include "ChooseTravelTargetAction.h"
#include "DelayAction.h"
#include "GiveItemAction.h"
#include "GreetAction.h"
#include "ImbueAction.h"
#include "MovementActions.h"
#include "MoveToRpgTargetAction.h"
#include "MoveToTravelTargetAction.h"
#include "OutfitAction.h"
#include "RevealGatheringItemAction.h"
#include "SayAction.h"
#include "OutfitAction.h"
#include "RandomBotUpdateAction.h"
#include "RemoveAuraAction.h"
#include "RpgAction.h"
#include "TravelAction.h"
#include "RtiAction.h"
#include "BattleGroundTactics.h"
#include "CheckMountStateAction.h"
#include "ChangeTalentsAction.h"
#include "AutoLearnSpellAction.h"
#include "XpGainAction.h"
#include "InviteToGroupAction.h"
#include "LeaveGroupAction.h"
#include "ReleaseSpiritAction.h"
#include "CombatActions.h"
#include "WorldBuffAction.h"
#include "CastCustomSpellAction.h"
#include "BattleGroundJoinAction.h"
#include "DestroyItemAction.h"
#include "ResetInstancesAction.h"
#include "BuyAction.h"
#include "GuildCreateActions.h"
#include "GuildManagementActions.h"
#include "GuildAcceptAction.h"
#include "RpgSubActions.h"
#include "VehicleActions.h"
#include "UseTrinketAction.h"
#include "BotStateActions.h"
#include "WaitForAttackAction.h"
#include "PullActions.h"

#include "OnyxiasLairDungeonActions.h"
#include "MoltenCoreDungeonActions.h"

namespace ai
{
    class ActionContext : public NamedObjectContext<Action>
    {
    public:
        ActionContext()
        {
            creators["mark rti"] = &ActionContext::mark_rti;
            creators["set return position"] = &ActionContext::set_return_position;
            creators["rpg"] = &ActionContext::rpg;
            creators["crpg"] = &ActionContext::crpg;
            creators["choose rpg target"] = &ActionContext::choose_rpg_target;
            creators["move to rpg target"] = &ActionContext::move_to_rpg_target;
			creators["travel"] = &ActionContext::travel;
			creators["choose travel target"] = &ActionContext::choose_travel_target;
			creators["move to travel target"] = &ActionContext::move_to_travel_target;
            creators["move out of collision"] = &ActionContext::move_out_of_collision;
            creators["move random"] = &ActionContext::move_random;
            creators["attack"] = &ActionContext::melee;
            creators["melee"] = &ActionContext::melee;
            creators["switch to melee"] = &ActionContext::switch_to_melee;
            creators["switch to ranged"] = &ActionContext::switch_to_ranged;
            creators["reach spell"] = &ActionContext::reach_spell;
            creators["reach melee"] = &ActionContext::reach_melee;
            creators["reach pull"] = &ActionContext::reach_pull;
            creators["reach party member to heal"] = &ActionContext::reach_party_member_to_heal;
            creators["flee"] = &ActionContext::flee;
            creators["flee with pet"] = &ActionContext::flee_with_pet;
            creators["wait for attack keep safe distance"] = &ActionContext::wait_for_attack_keep_safe_distance;
            creators["shoot"] = &ActionContext::shoot;
            creators["healthstone"] = &ActionContext::healthstone;
            creators["healing potion"] = &ActionContext::healing_potion;
            creators["mana potion"] = &ActionContext::mana_potion;
            creators["food"] = &ActionContext::food;
            creators["drink"] = &ActionContext::drink;
            creators["tank assist"] = &ActionContext::tank_assist;
            creators["dps assist"] = &ActionContext::dps_assist;
            creators["dps aoe"] = &ActionContext::dps_aoe;
            creators["attack rti target"] = &ActionContext::attack_rti_target;
            creators["loot"] = &ActionContext::loot;
            creators["add loot"] = &ActionContext::add_loot;
            creators["add gathering loot"] = &ActionContext::add_gathering_loot;
            creators["add all loot"] = &ActionContext::add_all_loot;
            creators["release loot"] = &ActionContext::release_loot;
            creators["auto loot roll"] = &ActionContext::auto_loot_roll;
            creators["shoot"] = &ActionContext::shoot;
            creators["follow"] = &ActionContext::follow;
            creators["flee to master"] = &ActionContext::flee_to_master;
            creators["runaway"] = &ActionContext::runaway;
            creators["stay"] = &ActionContext::stay;
            creators["sit"] = &ActionContext::sit;
            creators["attack anything"] = &ActionContext::attack_anything;
            creators["attack least hp target"] = &ActionContext::attack_least_hp_target;
            creators["attack enemy player"] = &ActionContext::attack_enemy_player;
            creators["pull my target"] = &ActionContext::pull_my_target;
            creators["pull start"] = &ActionContext::pull_start;
            creators["pull action"] = &ActionContext::pull_action;
            creators["pull end"] = &ActionContext::pull_end;
            creators["emote"] = &ActionContext::emote;
            creators["talk"] = &ActionContext::talk;
            creators["suggest what to do"] = &ActionContext::suggest_what_to_do;
            creators["suggest trade"] = &ActionContext::suggest_trade;
            creators["return"] = &ActionContext::_return;
            creators["move to loot"] = &ActionContext::move_to_loot;
            creators["open loot"] = &ActionContext::open_loot;
            creators["guard"] = &ActionContext::guard;
            creators["return to stay position"] = &ActionContext::return_to_stay_position;
            creators["move out of enemy contact"] = &ActionContext::move_out_of_enemy_contact;
            creators["set facing"] = &ActionContext::set_facing;
            creators["set behind"] = &ActionContext::set_behind;
            creators["attack duel opponent"] = &ActionContext::attack_duel_opponent;
            creators["select new target"] = &ActionContext::select_new_target;
            creators["check mail"] = &ActionContext::check_mail;
            creators["say"] = &ActionContext::say;
            creators["reveal gathering item"] = &ActionContext::reveal_gathering_item;
            creators["outfit"] = &ActionContext::outfit;
            creators["random bot update"] = &ActionContext::random_bot_update;
            creators["delay"] = &ActionContext::delay;
            creators["greet"] = &ActionContext::greet;
            creators["check values"] = &ActionContext::check_values;
            creators["ra"] = &ActionContext::ra;
            creators["apply poison"] = &ActionContext::apply_poison;
            creators["apply stone"] = &ActionContext::apply_stone;
            creators["apply oil"] = &ActionContext::apply_oil;
            creators["try emergency"] = &ActionContext::try_emergency;
            creators["give food"] = &ActionContext::give_food;
            creators["give water"] = &ActionContext::give_water;
            creators["mount"] = &ActionContext::mount;
            creators["auto talents"] = &ActionContext::auto_talents;
			creators["auto learn spell"] = &ActionContext::auto_learn_spell;
            creators["xp gain"] = &ActionContext::xp_gain;
            creators["invite nearby"] = &ActionContext::invite_nearby;
            creators["invite guild"] = &ActionContext::invite_guild;
            creators["leave far away"] = &ActionContext::leave_far_away;
            creators["move to dark portal"] = &ActionContext::move_to_dark_portal;
            creators["move from dark portal"] = &ActionContext::move_from_dark_portal;
            creators["use dark portal azeroth"] = &ActionContext::use_dark_portal_azeroth;
            creators["world buff"] = &ActionContext::world_buff;
            creators["hearthstone"] = &ActionContext::hearthstone;
            creators["cast random spell"] = &ActionContext::cast_random_spell;
            creators["free bg join"] = &ActionContext::free_bg_join;
            creators["use random recipe"] = &ActionContext::use_random_recipe;
            creators["use random quest item"] = &ActionContext::use_random_quest_item;
            creators["craft random item"] = &ActionContext::craft_random_item;
            creators["smart destroy item"] = &ActionContext::smart_destroy_item;
            creators["disenchant random item"] = &ActionContext::disenchant_random_item;
            creators["enchant random item"] = &ActionContext::enchant_random_item;
            creators["reset instances"] = &ActionContext::reset_instances;
            creators["buy petition"] = &ActionContext::buy_petition;
            creators["offer petition"] = &ActionContext::offer_petition;
            creators["offer petition nearby"] = &ActionContext::offer_petition_nearby;
            creators["turn in petition"] = &ActionContext::turn_in_petition;
            creators["buy tabard"] = &ActionContext::buy_tabard;
            creators["guild manage nearby"] = &ActionContext::guild_manage_nearby;
            creators["use trinket"] = &ActionContext::use_trinket;
            creators["goblin sapper"] = &ActionContext::goblin_sapper;
            creators["oil of immolation"] = &ActionContext::oil_of_immolation;
            creators["dark rune"] = &ActionContext::dark_rune;
            creators["adamantite grenade"] = &ActionContext::adamantite_grenade;
            creators["bg banner"] = &ActionContext::bg_banner;
            creators["use bandage"] = &ActionContext::use_bandage;

            // BG Tactics
            creators["bg tactics"] = &ActionContext::bg_tactics;
            creators["bg move to start"] = &ActionContext::bg_move_to_start;
            creators["bg move to objective"] = &ActionContext::bg_move_to_objective;
            creators["bg select objective"] = &ActionContext::bg_select_objective;
            creators["bg check objective"] = &ActionContext::bg_check_objective;
            creators["bg attack fc"] = &ActionContext::bg_attack_fc;
            creators["bg protect fc"] = &ActionContext::bg_protect_fc;
            creators["bg use buff"] = &ActionContext::bg_use_buff;
            creators["attack enemy flag carrier"] = &ActionContext::attack_enemy_fc;
            creators["bg check flag"] = &ActionContext::bg_check_flag;

            // lightwell
            creators["use lightwell"] = &ActionContext::use_lightwell;

            // Vehicles
            creators["enter vehicle"] = &ActionContext::enter_vehicle;
            creators["leave vehicle"] = &ActionContext::leave_vehicle;
            creators["hurl boulder"] = &ActionContext::hurl_boulder;
            creators["ram"] = &ActionContext::ram;
            creators["steam rush"] = &ActionContext::steam_rush;
            creators["steam blast"] = &ActionContext::steam_blast;
            creators["napalm"] = &ActionContext::napalm;
            creators["fire cannon"] = &ActionContext::fire_cannon;
            creators["incendiary rocket"] = &ActionContext::incendiary_rocket;
            creators["rocket blast"] = &ActionContext::rocket_blast;
            creators["blade salvo"] = &ActionContext::blade_salvo;
            creators["glaive throw"] = &ActionContext::glaive_throw;

            //Rpg
            creators["rpg stay"] = &ActionContext::rpg_stay;
            creators["rpg work"] = &ActionContext::rpg_work;
            creators["rpg emote"] = &ActionContext::rpg_emote;
            creators["rpg cancel"] = &ActionContext::rpg_cancel;
            creators["rpg taxi"] = &ActionContext::rpg_taxi;
            creators["rpg discover"] = &ActionContext::rpg_discover;
            creators["rpg start quest"] = &ActionContext::rpg_start_quest;
            creators["rpg end quest"] = &ActionContext::rpg_end_quest;
            creators["rpg buy"] = &ActionContext::rpg_buy;
            creators["rpg sell"] = &ActionContext::rpg_sell;
            creators["rpg ah sell"] = &ActionContext::rpg_ah_sell;
            creators["rpg ah buy"] = &ActionContext::rpg_ah_buy;
            creators["rpg get mail"] = &ActionContext::rpg_get_mail;
            creators["rpg repair"] = &ActionContext::rpg_repair;
            creators["rpg train"] = &ActionContext::rpg_train;
            creators["rpg heal"] = &ActionContext::rpg_heal;
            creators["rpg home bind"] = &ActionContext::rpg_home_bind;
            creators["rpg queue bg"] = &ActionContext::rpg_queue_bg;
            creators["rpg buy petition"] = &ActionContext::rpg_buy_petition;
            creators["rpg use"] = &ActionContext::rpg_use;
            creators["rpg spell"] = &ActionContext::rpg_spell;
            creators["rpg craft"] = &ActionContext::rpg_craft;
            creators["rpg trade useful"] = &ActionContext::rpg_trade_useful;
            creators["rpg duel"] = &ActionContext::rpg_duel;
            creators["rpg mount anim"] = &ActionContext::rpg_mount_anim;

            // Bot States
            creators["set combat state"] = &ActionContext::set_combat_state;
            creators["set non combat state"] = &ActionContext::set_non_combat_state;
            creators["set dead state"] = &ActionContext::set_dead_state;

            //racials
            creators["war stomp"] = &ActionContext::war_stomp;
            creators["berserking"] = &ActionContext::berserking;
            creators["blood fury"] = &ActionContext::blood_fury;
            creators["cannibalize"] = &ActionContext::cannibalize;
            creators["escape artist"] = &ActionContext::escape_artist;
            creators["shadowmeld"] = &ActionContext::shadowmeld;
            creators["stoneform"] = &ActionContext::stoneform;
            creators["perception"] = &ActionContext::perception;
            creators["will of the forsaken"] = &ActionContext::will_of_the_forsaken;
#ifndef MANGOSBOT_ZERO
            creators["mana tap"] = &ActionContext::mana_tap;
            creators["arcane torrent"] = &ActionContext::arcane_torrent;
            creators["gift of the naaru"] = &ActionContext::gift_of_the_naaru;
#endif
#ifdef MANGOSBOT_TWO
            creators["every_man_for_himself"] = &ActionContext::every_man_for_himself;
#endif    

            // Dungeon Actions
            creators["enable onyxia's lair strategy"] = &ActionContext::onyxias_lair_enable_strategy;
            creators["disable onyxia's lair strategy"] = &ActionContext::onyxias_lair_disable_strategy;
            creators["enable molten core strategy"] = &ActionContext::molten_core_enable_strategy;
            creators["disable molten core strategy"] = &ActionContext::molten_core_disable_strategy;

            // Dungeon Boss Actions
            creators["enable onyxia fight strategy"] = &ActionContext::onyxia_enable_fight_strategy;
            creators["disable onyxia fight strategy"] = &ActionContext::onyxia_disable_fight_strategy;

            creators["enable magmadar fight strategy"] = &ActionContext::magmadar_enable_fight_strategy;
            creators["disable magmadar fight strategy"] = &ActionContext::magmadar_disable_fight_strategy;
            creators["move away from magmadar lava bomb"] = &ActionContext::magmadar_move_away_from_lava_bomb;
        }

    private:
        static Action* give_water(PlayerbotAI* ai) { return new GiveWaterAction(ai); }
        static Action* give_food(PlayerbotAI* ai) { return new GiveFoodAction(ai); }
        static Action* ra(PlayerbotAI* ai) { return new RemoveAuraAction(ai); }
        static Action* mark_rti(PlayerbotAI* ai) { return new MarkRtiAction(ai); }
        static Action* set_return_position(PlayerbotAI* ai) { return new SetReturnPositionAction(ai); }
        static Action* rpg(PlayerbotAI* ai) { return new RpgAction(ai); }
        static Action* crpg(PlayerbotAI* ai) { return new CRpgAction(ai); }
        static Action* choose_rpg_target(PlayerbotAI* ai) { return new ChooseRpgTargetAction(ai); }
        static Action* move_to_rpg_target(PlayerbotAI* ai) { return new MoveToRpgTargetAction(ai); }
        static Action* travel(PlayerbotAI* ai) { return new TravelAction(ai); }
        static Action* choose_travel_target(PlayerbotAI* ai) { return new ChooseTravelTargetAction(ai); }
        static Action* move_to_travel_target(PlayerbotAI* ai) { return new MoveToTravelTargetAction(ai); }
        static Action* move_out_of_collision(PlayerbotAI* ai) { return new MoveOutOfCollisionAction(ai); }
        static Action* move_random(PlayerbotAI* ai) { return new MoveRandomAction(ai); }
        static Action* check_values(PlayerbotAI* ai) { return new CheckValuesAction(ai); }
        static Action* greet(PlayerbotAI* ai) { return new GreetAction(ai); }
        static Action* apply_poison(PlayerbotAI* ai) { return new ImbueWithPoisonAction(ai); }
        static Action* apply_oil(PlayerbotAI* ai) { return new ImbueWithOilAction(ai); }
        static Action* try_emergency(PlayerbotAI* ai) { return new TryEmergencyAction(ai); }
        static Action* apply_stone(PlayerbotAI* ai) { return new ImbueWithStoneAction(ai); }
        static Action* check_mail(PlayerbotAI* ai) { return new CheckMailAction(ai); }
        static Action* select_new_target(PlayerbotAI* ai) { return new SelectNewTargetAction(ai); }
        static Action* attack_duel_opponent(PlayerbotAI* ai) { return new AttackDuelOpponentAction(ai); }
        static Action* guard(PlayerbotAI* ai) { return new GuardAction(ai); }
        static Action* return_to_stay_position(PlayerbotAI* ai) { return new ReturnToStayPositionAction(ai); }
        static Action* open_loot(PlayerbotAI* ai) { return new OpenLootAction(ai); }
        static Action* move_to_loot(PlayerbotAI* ai) { return new MoveToLootAction(ai); }
        static Action* _return(PlayerbotAI* ai) { return new ReturnAction(ai); }
        static Action* shoot(PlayerbotAI* ai) { return new CastShootAction(ai); }
        static Action* melee(PlayerbotAI* ai) { return new MeleeAction(ai); }
        static Action* switch_to_melee(PlayerbotAI* ai) { return new SwitchToMeleeAction(ai); }
        static Action* switch_to_ranged(PlayerbotAI* ai) { return new SwitchToRangedAction(ai); }
        static Action* reach_spell(PlayerbotAI* ai) { return new ReachSpellAction(ai); }
        static Action* reach_melee(PlayerbotAI* ai) { return new ReachMeleeAction(ai); }
        static Action* reach_pull(PlayerbotAI* ai) { return new ReachPullAction(ai); }
        static Action* reach_party_member_to_heal(PlayerbotAI* ai) { return new ReachPartyMemberToHealAction(ai); }
        static Action* flee_with_pet(PlayerbotAI* ai) { return new FleeWithPetAction(ai); }
        static Action* wait_for_attack_keep_safe_distance(PlayerbotAI* ai) { return new WaitForAttackKeepSafeDistanceAction(ai); }
        static Action* flee(PlayerbotAI* ai) { return new FleeAction(ai); }

        //racials      
        static Action* war_stomp(PlayerbotAI* ai) { return new CastWarStompAction(ai); }
        static Action* berserking(PlayerbotAI* ai) { return new CastBerserkingAction(ai); }
        static Action* blood_fury(PlayerbotAI* ai) { return new CastBloodFuryAction(ai); }
        static Action* cannibalize(PlayerbotAI* ai) { return new CastCannibalizeAction(ai); }
        static Action* escape_artist(PlayerbotAI* ai) { return new CastEscapeArtistAction(ai); }
        static Action* shadowmeld(PlayerbotAI* ai) { return new CastShadowmeldAction(ai); }
        static Action* stoneform(PlayerbotAI* ai) { return new CastStoneformAction(ai); }
        static Action* perception(PlayerbotAI* ai) { return new CastPerceptionAction(ai); }
        static Action* will_of_the_forsaken(PlayerbotAI* ai) { return new CastWillOfTheForsakenAction(ai); }
#ifndef MANGOSBOT_ZERO
        static Action* gift_of_the_naaru(PlayerbotAI* ai) { return new CastGiftOfTheNaaruAction(ai); }
        static Action* arcane_torrent(PlayerbotAI* ai) { return new CastArcaneTorrentAction(ai); }
        static Action* mana_tap(PlayerbotAI* ai) { return new CastManaTapAction(ai); }
#endif 
#ifdef MANGOSBOT_TWO
        static Action* every_man_for_himself(PlayerbotAI* ai) { return new CastEveryManforHimselfAction(ai); }
#endif 

        static Action* emote(PlayerbotAI* ai) { return new EmoteAction(ai); }
        static Action* talk(PlayerbotAI* ai) { return new TalkAction(ai); }
        static Action* suggest_what_to_do(PlayerbotAI* ai) { return new SuggestWhatToDoAction(ai); }
        static Action* suggest_trade(PlayerbotAI* ai) { return new SuggestTradeAction(ai); }
        static Action* attack_anything(PlayerbotAI* ai) { return new AttackAnythingAction(ai); }
        static Action* attack_least_hp_target(PlayerbotAI* ai) { return new AttackLeastHpTargetAction(ai); }
        static Action* attack_enemy_player(PlayerbotAI* ai) { return new AttackEnemyPlayerAction(ai); }
        static Action* pull_my_target(PlayerbotAI* ai) { return new PullMyTargetAction(ai); }
        static Action* pull_start(PlayerbotAI* ai) { return new PullStartAction(ai); }
        static Action* pull_action(PlayerbotAI* ai) { return new PullAction(ai); }
        static Action* pull_end(PlayerbotAI* ai) { return new PullEndAction(ai); }
        static Action* stay(PlayerbotAI* ai) { return new StayAction(ai); }
        static Action* sit(PlayerbotAI* ai) { return new SitAction(ai); }
        static Action* runaway(PlayerbotAI* ai) { return new RunAwayAction(ai); }
        static Action* follow(PlayerbotAI* ai) { return new FollowAction(ai); }
        static Action* flee_to_master(PlayerbotAI* ai) { return new FleeToMasterAction(ai); }
        static Action* add_gathering_loot(PlayerbotAI* ai) { return new AddGatheringLootAction(ai); }
        static Action* add_loot(PlayerbotAI* ai) { return new AddLootAction(ai); }
        static Action* add_all_loot(PlayerbotAI* ai) { return new AddAllLootAction(ai); }
        static Action* loot(PlayerbotAI* ai) { return new LootAction(ai); }
        static Action* release_loot(PlayerbotAI* ai) { return new ReleaseLootAction(ai); }
        static Action* auto_loot_roll(PlayerbotAI* ai) { return new AutoLootRollAction(ai); }
        static Action* dps_assist(PlayerbotAI* ai) { return new DpsAssistAction(ai); }
        static Action* dps_aoe(PlayerbotAI* ai) { return new DpsAoeAction(ai); }
        static Action* attack_rti_target(PlayerbotAI* ai) { return new AttackRtiTargetAction(ai); }
        static Action* tank_assist(PlayerbotAI* ai) { return new TankAssistAction(ai); }
        static Action* drink(PlayerbotAI* ai) { return new DrinkAction(ai); }
        static Action* food(PlayerbotAI* ai) { return new EatAction(ai); }
        static Action* mana_potion(PlayerbotAI* ai) { return new UseManaPotion(ai); }
        static Action* healing_potion(PlayerbotAI* ai) { return new UseHealingPotion(ai); }
        static Action* healthstone(PlayerbotAI* ai) { return new UseItemAction(ai, "healthstone"); }
        static Action* move_out_of_enemy_contact(PlayerbotAI* ai) { return new MoveOutOfEnemyContactAction(ai); }
        static Action* set_facing(PlayerbotAI* ai) { return new SetFacingTargetAction(ai); }
        static Action* set_behind(PlayerbotAI* ai) { return new SetBehindTargetAction(ai); }
        static Action* say(PlayerbotAI* ai) { return new SayAction(ai); }
        static Action* reveal_gathering_item(PlayerbotAI* ai) { return new RevealGatheringItemAction(ai); }
        static Action* outfit(PlayerbotAI* ai) { return new OutfitAction(ai); }
        static Action* random_bot_update(PlayerbotAI* ai) { return new RandomBotUpdateAction(ai); }
        static Action* delay(PlayerbotAI* ai) { return new DelayAction(ai); }
        static Action* mount(PlayerbotAI *ai) { return new CastSpellAction(ai, "mount"); }
        static Action* auto_talents(PlayerbotAI* ai) { return new AutoSetTalentsAction(ai); }
		static Action* auto_learn_spell(PlayerbotAI* ai) { return new AutoLearnSpellAction(ai); }
        static Action* xp_gain(PlayerbotAI* ai) { return new XpGainAction(ai); }
        static Action* invite_nearby(PlayerbotAI* ai) { return new InviteNearbyToGroupAction(ai); }
        static Action* invite_guild(PlayerbotAI* ai) { return new InviteGuildToGroupAction(ai); }
        static Action* leave_far_away(PlayerbotAI* ai) { return new LeaveFarAwayAction(ai); }
        static Action* move_to_dark_portal(PlayerbotAI* ai) { return new MoveToDarkPortalAction(ai); }
        static Action* use_dark_portal_azeroth(PlayerbotAI* ai) { return new DarkPortalAzerothAction(ai); }
        static Action* move_from_dark_portal(PlayerbotAI* ai) { return new MoveFromDarkPortalAction(ai); }
        static Action* world_buff(PlayerbotAI* ai) { return new WorldBuffAction(ai); }
        static Action* hearthstone(PlayerbotAI* ai) { return new UseHearthStone(ai); }
        static Action* cast_random_spell(PlayerbotAI* ai) { return new CastRandomSpellAction(ai); }
        static Action* free_bg_join(PlayerbotAI* ai) { return new FreeBGJoinAction(ai); }
        static Action* use_random_recipe(PlayerbotAI* ai) { return new UseRandomRecipe(ai); }
        static Action* use_random_quest_item(PlayerbotAI* ai) { return new UseRandomQuestItem(ai); }
        static Action* craft_random_item(PlayerbotAI* ai) { return new CraftRandomItemAction(ai); }
        static Action* smart_destroy_item(PlayerbotAI* ai) { return new SmartDestroyItemAction(ai); }
        static Action* disenchant_random_item(PlayerbotAI* ai) { return new DisEnchantRandomItemAction(ai); }
        static Action* enchant_random_item(PlayerbotAI* ai) { return new EnchantRandomItemAction(ai); }
        static Action* reset_instances(PlayerbotAI* ai) { return new ResetInstancesAction(ai); }
        static Action* buy_petition(PlayerbotAI* ai) { return new BuyPetitionAction(ai); }
        static Action* offer_petition(PlayerbotAI* ai) { return new PetitionOfferAction(ai); }
        static Action* offer_petition_nearby(PlayerbotAI* ai) { return new PetitionOfferNearbyAction(ai); }
        static Action* turn_in_petition(PlayerbotAI* ai) { return new PetitionTurnInAction(ai); }
        static Action* buy_tabard(PlayerbotAI* ai) { return new BuyTabardAction(ai); }
        static Action* guild_manage_nearby(PlayerbotAI* ai) { return new GuildManageNearbyAction(ai); }
        static Action* use_trinket(PlayerbotAI* ai) { return new UseTrinketAction(ai); }

        // item helpers
        static Action* goblin_sapper(PlayerbotAI* ai) { return new CastGoblinSappersAction(ai); }
        static Action* oil_of_immolation(PlayerbotAI* ai) { return new CastOilOfImmolationAction(ai); }
        static Action* dark_rune(PlayerbotAI* ai) { return new DarkRuneAction(ai); }
        static Action* adamantite_grenade(PlayerbotAI* ai) { return new UseAdamantiteGrenadeAction(ai); }
        static Action* bg_banner(PlayerbotAI* ai) { return new UseBgBannerAction(ai); }
        static Action* use_bandage(PlayerbotAI* ai) { return new UseBandageAction(ai); }
        
        // BG Tactics
        static Action* bg_tactics(PlayerbotAI* ai) { return new BGTactics(ai); }
        static Action* bg_move_to_start(PlayerbotAI* ai) { return new BGTactics(ai, "move to start"); }
        static Action* bg_move_to_objective(PlayerbotAI* ai) { return new BGTactics(ai, "move to objective"); }
        static Action* bg_select_objective(PlayerbotAI* ai) { return new BGTactics(ai, "select objective"); }
        static Action* bg_check_objective(PlayerbotAI* ai) { return new BGTactics(ai, "check objective"); }
        static Action* bg_attack_fc(PlayerbotAI* ai) { return new BGTactics(ai, "attack fc"); }
        static Action* bg_protect_fc(PlayerbotAI* ai) { return new BGTactics(ai, "protect fc"); }
        static Action* attack_enemy_fc(PlayerbotAI* ai) { return new AttackEnemyFlagCarrierAction(ai); }
        static Action* bg_use_buff(PlayerbotAI* ai) { return new BGTactics(ai, "use buff"); }
        static Action* bg_check_flag(PlayerbotAI* ai) { return new BGTactics(ai, "check flag"); }   

        // lightwell
        static Action* use_lightwell(PlayerbotAI* ai) { return new UseLightwellAction(ai); }

        // Vehicles
        static Action* enter_vehicle(PlayerbotAI* ai) { return new EnterVehicleAction(ai); }
        static Action* leave_vehicle(PlayerbotAI* ai) { return new LeaveVehicleAction(ai); }
        static Action* hurl_boulder(PlayerbotAI* ai) { return new CastHurlBoulderAction(ai); }
        static Action* ram(PlayerbotAI* ai) { return new CastRamAction(ai); }
        static Action* steam_blast(PlayerbotAI* ai) { return new CastSteamBlastAction(ai); }
        static Action* steam_rush(PlayerbotAI* ai) { return new CastSteamRushAction(ai); }
        static Action* napalm(PlayerbotAI* ai) { return new CastNapalmAction(ai); }
        static Action* fire_cannon(PlayerbotAI* ai) { return new CastFireCannonAction(ai); }
        static Action* incendiary_rocket(PlayerbotAI* ai) { return new CastIncendiaryRocketAction(ai); }
        static Action* rocket_blast(PlayerbotAI* ai) { return new CastRocketBlastAction(ai); }
        static Action* glaive_throw(PlayerbotAI* ai) { return new CastGlaiveThrowAction(ai); }
        static Action* blade_salvo(PlayerbotAI* ai) { return new CastBladeSalvoAction(ai); }

        //Rpg
        static Action* rpg_stay(PlayerbotAI* ai) { return new RpgStayAction(ai); }
        static Action* rpg_work(PlayerbotAI* ai) { return new RpgWorkAction(ai); }
        static Action* rpg_emote(PlayerbotAI* ai) { return new RpgEmoteAction(ai); }
        static Action* rpg_cancel(PlayerbotAI* ai) { return new RpgCancelAction(ai); }
        static Action* rpg_taxi(PlayerbotAI* ai) { return new RpgTaxiAction(ai); }
        static Action* rpg_discover(PlayerbotAI* ai) { return new RpgDiscoverAction(ai); }
        static Action* rpg_start_quest(PlayerbotAI* ai) { return new RpgStartQuestAction(ai); }
        static Action* rpg_end_quest(PlayerbotAI* ai) { return new RpgEndQuestAction(ai); }
        static Action* rpg_buy(PlayerbotAI* ai) { return new RpgBuyAction(ai); }
        static Action* rpg_sell(PlayerbotAI* ai) { return new RpgSellAction(ai); }
        static Action* rpg_ah_sell(PlayerbotAI* ai) { return new RpgAHSellAction(ai); }
        static Action* rpg_ah_buy(PlayerbotAI* ai) { return new RpgAHBuyAction(ai); }
        static Action* rpg_get_mail(PlayerbotAI* ai) { return new RpgGetMailAction(ai); }
        static Action* rpg_repair(PlayerbotAI* ai) { return new RpgRepairAction(ai); }
        static Action* rpg_train(PlayerbotAI* ai) { return new RpgTrainAction(ai); }
        static Action* rpg_heal(PlayerbotAI* ai) { return new RpgHealAction(ai); }
        static Action* rpg_home_bind(PlayerbotAI* ai) { return new RpgHomeBindAction(ai); }
        static Action* rpg_queue_bg(PlayerbotAI* ai) { return new RpgQueueBgAction(ai); }
        static Action* rpg_buy_petition(PlayerbotAI* ai) { return new RpgBuyPetitionAction(ai); }
        static Action* rpg_use(PlayerbotAI* ai) { return new RpgUseAction(ai); }
        static Action* rpg_spell(PlayerbotAI* ai) { return new RpgSpellAction(ai); }
        static Action* rpg_craft(PlayerbotAI* ai) { return new RpgCraftAction(ai); }
        static Action* rpg_trade_useful(PlayerbotAI* ai) { return new RpgTradeUsefulAction(ai); }
        static Action* rpg_duel(PlayerbotAI* ai) { return new RpgDuelAction(ai); }
        static Action* rpg_mount_anim(PlayerbotAI* ai) { return new RpgMountAnimAction(ai); }

        // Bot States
        static Action* set_combat_state(PlayerbotAI* ai) { return new SetCombatStateAction(ai); }
        static Action* set_non_combat_state(PlayerbotAI* ai) { return new SetNonCombatStateAction(ai); }
        static Action* set_dead_state(PlayerbotAI* ai) { return new SetDeadStateAction(ai); }

        // Dungeon Actions
        static Action* onyxias_lair_enable_strategy(PlayerbotAI* ai) { return new OnyxiasLairEnableDungeonStrategyAction(ai); }
        static Action* onyxias_lair_disable_strategy(PlayerbotAI* ai) { return new OnyxiasLairDisableDungeonStrategyAction(ai); }
        static Action* molten_core_enable_strategy(PlayerbotAI* ai) { return new MoltenCoreEnableDungeonStrategyAction(ai); }
        static Action* molten_core_disable_strategy(PlayerbotAI* ai) { return new MoltenCoreDisableDungeonStrategyAction(ai); }

        // Dungeon Boss Actions
        static Action* onyxia_enable_fight_strategy(PlayerbotAI* ai) { return new OnyxiaEnableFightStrategyAction(ai); }
        static Action* onyxia_disable_fight_strategy(PlayerbotAI* ai) { return new OnyxiaDisableFightStrategyAction(ai); }

        static Action* magmadar_enable_fight_strategy(PlayerbotAI* ai) { return new MagmadarEnableFightStrategyAction(ai); }
        static Action* magmadar_disable_fight_strategy(PlayerbotAI* ai) { return new MagmadarDisableFightStrategyAction(ai); }
        static Action* magmadar_move_away_from_lava_bomb(PlayerbotAI* ai) { return new MagmadarMoveAwayFromLavaBombAction(ai); }
    };
};
