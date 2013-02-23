/* 
 * File:   Search.h
 * Author: mehran
 *
 * Created on January 19, 2011, 10:25 AM
 */

#ifndef _SEARCH_H
#define	_SEARCH_H

#include "../WorldModel/WorldModel.h"
#include "../WorldGraph/WorldGraph.h"
class Search
{
public:
    Search();
    Search(WorldModel *world, WorldGraph *worldGraph);
    Building* searchNearBuilding();
    void init(MotionlessObject* lastDuties[]);
    Building* actPoliceSearch(bool canIGoRefuge);
    Building* actAmbulanceAndFireBrigadeSearch(bool canGoRefuge);
    Building* updateCiviliansAndZone(bool canIGoRefuge);
    virtual ~Search();
private:
    struct Zone{
        std::vector<BuildingBlock* > buildingBlocks;
        int policeForceIndex;
    };
    void creatZones();
    void assignPoliceForcesToZones(MotionlessObject* lastDuties[]);
    Zone* myZone;
    std::vector<Zone* > zones;
    std::vector<int > eachPoliceZone;
    WorldModel *world;
    WorldGraph *worldGraph;

};

#endif	/* _SEARCH_H */

