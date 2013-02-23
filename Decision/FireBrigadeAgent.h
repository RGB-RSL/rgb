/* 
 * File:   FireBrigadeAgent.h
 * Author: mehran
 *
 * Created on September 24, 2010, 3:57 PM
 */

#ifndef _FIREBRIGADEAGENT_H
#define	_FIREBRIGADEAGENT_H
#include "Agent.h"

using namespace std;
using namespace Geometry;

class FireBrigadeAgent : public Agent
{
public:
    FireBrigadeAgent();
    virtual ~FireBrigadeAgent();
protected:
    void actBeforeRadar();
    void act();
    void precomputation();
    FireBrigade* self();
private:
    int maxSightRange;
    int maxFireTank;
    int maxWaterForExtinguish;
    int maxExtinguishDistance;

    bool needEscape(FireBrigade* fb);
    bool needEscape();
    bool escape();
    void canGoRefuge();
    void moveToRefuges();

    bool needToGoToRefuge();
    bool stayInRefuge();

    Building * mySearchTarget;

    bool canIGoRefuge;
    bool dontShasWhenIAmNotReachable();
    bool dontShasWhenShasingInAMotionless();
    bool dontShas[4];
    MotionlessObject* lastMotionless;


    bool isNeighbor(Building * a, Building * b);
    void createBuildingBlock();
    void dfs(BuildingBlock* buildingBlock, int color, Building* building);

    bool isHere(std::vector<Road *> a, int b);

    void updateBuildingBlocks();

    void setBuildingBlocksInFire();
    bool isThereUnburnedBuildingBlockNear(BuildingBlock* bb);
    void setNeededFireBrigades();
    void sortBuildingBlocks();
    void matchMaker();
    void matchToNearestFire();
    void chooseMyBuilding();
    bool positioning();
    void extinguishPlan();
    bool isOneOfBuldingBlocksInfire(Building* b);
    std::set<FireBrigade*> myTeammates;
    std::vector<BuildingBlock* > buildingBlocksInFire;
    std::set<BuildingBlock* > burntBuildingBlocks;
    BuildingBlock* myBuildingBlock;
    Building* myBuilding;
    bool needChoosePositon();
    void eraseMyTargets();
    bool isReachableToBuildingBlock(FireBrigade* fb, BuildingBlock* bb);
    bool isInAImportantBuildingBlocks(Building* b);
    bool isReachableBuilding(FireBrigade* fb, Building* b);

    Building* targetForSearch;
    bool isNearMatching;

    //	vector<Building *> sortBuildingsInBuildingBlock(BuildingBlock * target);

};

#endif	/* _FIREBRIGADEAGENT_H */

