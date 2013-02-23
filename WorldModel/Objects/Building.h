/*
 * Building.h
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Navid Saleh
 */

#ifndef __BUILDING_H_
#define __BUILDING_H_

#include "MotionlessObject.h"
#include "Civilian.h"
#include "Road.h"
#include <vector>
#include "../../Utilities/Types/Types.h"

class Building : public MotionlessObject
{
private:

    struct InSightPoint
    {
        int roadIndex;
        int index;
        InSightPoint();
        InSightPoint(int, int);
    };
public:
    Building(unsigned int Id);
    virtual ~Building();
    bool isBurning();
    bool isPutOut();
    bool isUnburned();
    unsigned int getAreaGround() const;
    unsigned int getAreaTotal() const;
    Types::BuildingCode getBuildingCode() const;
    unsigned short int getFieryness() const;
    unsigned short int getFloors() const;
    //    std::vector<Edge> getEdges() const;
    unsigned int getBrokenness() const;
    unsigned int getAttributes() const;
    int getTemperature() const;
    int getImportance() const;
    std::vector<InSightPoint>& getInSightPoints();
    void setAreaGround(unsigned int AreaGround);
    void setAreaTotal(unsigned int AreaTotal);
    void setBuildingCode(Types::BuildingCode buildingCode);
    void setFieryness(unsigned short int fieryness);
    void setFloors(unsigned short int floors);
    void setBrokenness(unsigned int brokenness);
    void setProperties(std::string type, int value);
    void setImportance(int value);
    void setAttributes(unsigned int value);
    void setTemperature(unsigned int value);
    void addInSightPoint(int roadIndex, int index);
    int blockNum;
    int neededFireBrigades;
    bool occupied;
    int getValue();
    void setValue(int value);


    std::vector<Building *> closerThan50B;
    std::vector<Building *> closerThan40B;
    std::vector<Building *> closerThan30B;
    std::vector<Building *> closerThan20B;
    std::vector<MotionlessObject *> closerThan30M;
    std::vector<MotionlessObject *> closerThan40M;
    std::vector<MotionlessObject *> closerThan50M;
    std::vector<Road *> closerThan100R;
    std::vector<Civilian *> civilians;

    int buildingIndex;
    int value;
    bool wasBurning;
private:
    unsigned int attributes;
    unsigned short int fieryness;
    unsigned short int floors;
    unsigned int AreaTotal;
    unsigned int AreaGround;
    unsigned int brokenness;
    int temperature;
    int importance;
    Types::BuildingCode buildingCode;
    std::vector<InSightPoint> inSightPoints;
};

#endif
