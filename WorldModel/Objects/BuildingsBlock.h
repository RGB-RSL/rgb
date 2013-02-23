/*
 * File:   BuildingsBlock.h
 * Author: Alimohammad Foroutannezhad
 *
 * Created on September 21, 2010, 5:59 PM
 */

#ifndef _BUILDINGSBLOCK_H
#define	_BUILDINGSBLOCK_H

#include <Building.h>
#include <iostream>
#include <vector>

#include "Civilian.h"

class WorldModel;

class BuildingBlock {
public:

    BuildingBlock();

    void setID(int id);
    int getID();
    void setTotalArea(int area);
    int getTotalArea();
    void setFirePercent(int p);
    int getFirePercent();
    void update();

    void setLeftBottom(Building * a);
    void setRightTop(Building * a);
    void setCenter(Building * a);

    Building * getLeftBottom();
    Building * getRightTop();
    Building * getCenter();

    std::vector<Building *> buildings;
    std::vector<Building *> buildingsInFire;
    std::vector<Civilian *> civilians;

    int buildingBlockIndex;
    int color;
    int neededFirebBrigades;
    int value;
private:

    WorldModel *worldModel;
    int id;
    int area;

    Building * left_bottom;
    Building * right_top;
    Building * center;

    int firePercent;
};


#endif	/* _BUILDINGSBLOCK_H */
