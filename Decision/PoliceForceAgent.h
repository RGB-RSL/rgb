/* 
 * File:   PoliceForceAgent.h
 * Author: Alimohammad
 *
 * Created on September 24, 2010, 3:59 PM
 */

#ifndef _POLICEFORCEAGENT_H
#define	_POLICEFORCEAGENT_H
#include "Agent.h"
#include <string>
#include "../Utilities/Geometry/Point.h"
//struct firstTribesComparator{
//    bool operator()(const std::vector<MotionlessObject*> &v1, const std::vector<MotionlessObject*> &v2) const
//    {
//        return v1.size() > v2.size();
//    }
//};
class PoliceForceAgent: public Agent
{
public:
    PoliceForceAgent();
    virtual ~PoliceForceAgent();
protected:
    void actBeforeRadar();
    void act();
    void precomputation();
    PoliceForce* self();
private:
    int maxClearDistance;
    int repairRate;
    bool canIGoRefuge;
    std::vector<int> myFirstTribesIndex;
    std::vector<std::set<MotionlessObject *> > firstTribes;
    std::vector<BuildingBlock* > secondaryTribes;
    Building *targetForSearch;
    Building *targetForUpdate;
    Geometry::Point lastPosition;
    void setFirstTribes(MotionlessObject* target, std::set<MotionlessObject*> &currentTargets, std::set<MotionlessObject*> &addedTargets, std::vector<MotionlessObject*> &allFirstTargets);
    void setSecondaryTribes();
    BuildingBlock* chooseMySecondaryTribe();
    bool needEscape();
    bool escape();
    void setBuildingBlocksInFire();
    void canGoRefuge();
    void moveToRefuges();
    void matchPolicesToFirstTargets(MotionlessObject* lastDuties[]);
    void updateMyFirstTribe();
    bool clearMyFirstTribe(int tribeIndex);
    bool clearMySecondTribe(BuildingBlock* tribe);
    bool clear(MotionlessObject *target);

    bool isNeighbor(Building * a, Building * b);
    void createBuildingBlock();
    void dfs(BuildingBlock* buildingBlock, int color, Building* building);

    bool isHere(std::vector<Road *> a, int b);
    bool isCloserThan(Building * a, Building * b);

    bool dontShasWhenIAmNotReachable();
    bool dontShasWhenShasingInAPoint();
    bool dontShasWhenShasingInAMotionless();
    bool dontShas[4];
    MotionlessObject* lastMotionless;

    std::set<PoliceForce*> helpNeededPolices;
    void savePolices();
    bool saveCivilians();

    void precompute();
};

#endif	/* _POLICEFORCEAGENT_H */

