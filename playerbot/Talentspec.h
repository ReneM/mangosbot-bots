#pragma once

#include "Player.h"
//#include "playerbot.h"

struct TalentEntry;
struct TalentTabEntry;

class TalentSpec {
    public:
#define SORT_BY_DEFAULT 0
#define SORT_BY_POINTS_TREE 1

#define ABSOLUTE_DIST 0
#define SUBSTRACT_OLD_NEW 1
#define SUBSTRACT_NEW_OLD -1
#define ADDED_POINTS 2
#define REMOVED_POINTS -2

        struct TalentListEntry
        {
            int entry;
            int rank;
            int maxRank;
            TalentEntry const* talentInfo;
            TalentTabEntry const* talentTabInfo;
            int tabPage() { return talentTabInfo->TalentTabID == 41 ? 1 : talentTabInfo->tabpage; }
        };

        uint32 points = 0;
        std::vector<TalentListEntry> talents;

        TalentSpec() {};
        TalentSpec(uint32 classMask) { GetTalents(classMask); }
        TalentSpec(Player* bot) { GetTalents(bot->getClassMask()); ReadTalents(bot); }
        TalentSpec(TalentSpec* base, string link) { talents = base->talents; ReadTalents(link); }
        TalentSpec(Player* bot, string link) { GetTalents(bot->getClassMask()); ReadTalents(link); }

        bool CheckTalentLink(string link, ostringstream* out);
        virtual bool CheckTalents(int maxPoints, ostringstream* out);
        void CropTalents(uint32 level);
        void ShiftTalents(TalentSpec* oldTalents, uint32 level);
        void ApplyTalents(Player* bot, ostringstream* out);

        int GetTalentPoints(std::vector<TalentListEntry>& talents, int tabpage = -1);
        int GetTalentPoints(int tabpage = -1) { return GetTalentPoints(talents, tabpage); };
        bool isEarlierVersionOf(TalentSpec& newSpec);

        string GetTalentLink();
        int highestTree();
        string formatSpec(Player* bot);
    protected:
        uint32 LeveltoPoints(uint32 level) const;
        uint32 PointstoLevel(int points) const;
        void GetTalents(uint32 classMask);
        void SortTalents(std::vector<TalentListEntry>& talents, int sortBy);
        void SortTalents(int sortBy) { SortTalents(talents, sortBy); }

        void ReadTalents(Player* bot);
        void ReadTalents(string link);

        std::vector<TalentListEntry> GetTalentTree(int tabpage);
        std::vector<TalentListEntry> SubTalentList(std::vector<TalentListEntry>& oldList, std::vector<TalentListEntry>& newList, int reverse);
    };

    class TalentPath {
    public:
        TalentPath(int pathId, string pathName, int pathProbability) { id = pathId; name = pathName; probability = pathProbability; };
        int id =0;
        string name = "";
        int probability = 100;
        std::vector<TalentSpec> talentSpec;
    };

    class ClassSpecs {
    public:
        ClassSpecs() {};
        ClassSpecs(uint32 specsClassMask) {classMask = specsClassMask;  baseSpec = TalentSpec(specsClassMask);
        }
        uint32 classMask = 0;
        TalentSpec baseSpec;

        std::vector<TalentPath> talentPath;
    };
