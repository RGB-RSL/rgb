#include "../../Utilities/Geometry/Segment.h"
#include "BuildingsBlock.h"
#include "../../Utilities/Geometry/Point.h"
#include <cmath>
#define LOGLEVEL 0
const int Max_Distance = 50000;
using namespace Geometry;
using namespace std;
//BuildingBlock::BuildingBlock(WorldModel* world)
//{
//	this->worldModel = world;
//}

BuildingBlock::BuildingBlock()
{
    color = -1;
    neededFirebBrigades = -1;
    value = 0;
}

void BuildingBlock::setID(int id)
{
    this->id = id;
}

int BuildingBlock::getID()
{
    return id;
}

void BuildingBlock::setTotalArea(int area)
{
    this->area = area;
}

int BuildingBlock::getTotalArea()
{
    return this->area;
}

void BuildingBlock::setLeftBottom(Building* a)
{
    this->left_bottom = a;
}

void BuildingBlock::setRightTop(Building* a)
{
    this->right_top = a;
}

void BuildingBlock::setCenter(Building * a)
{
    this->center = a;
}

Building * BuildingBlock::getLeftBottom()
{
    return this->left_bottom;
}

Building * BuildingBlock::getRightTop()
{
    return this->right_top;
}

Building * BuildingBlock::getCenter()
{
    return this->center;
}

void BuildingBlock::setFirePercent(int p)
{
    this->firePercent = p;
}

int BuildingBlock::getFirePercent()
{
    return this->firePercent;
}

void BuildingBlock::update()
{
    int meanX = 0, meanY = 0;
    int minLeftBottom = MAX_INT, maxRightTop = -1;
    Building* leftBottom = buildings[0];
    Building* rightTop = buildings[0];
    for(int i = 0 ;i < buildings.size(); i++)
    {
        if(buildings[i]->getX() + buildings[i]->getY() < minLeftBottom)
        {
            LOG(Main, 1) << "salam1" << endl;
            minLeftBottom = buildings[i]->getX() + buildings[i]->getY();
            leftBottom = buildings[i];
        }
        LOG(Main, 1) << "buildings[i]->getX() + buildings[i]->getY(): " << buildings[i]->getX() + buildings[i]->getY() << " maxRightTop " << maxRightTop << endl;
        if(buildings[i]->getX() + buildings[i]->getY() > maxRightTop)
        {
            LOG(Main, 1) << "salam2" << endl;
            maxRightTop = buildings[i]->getX() + buildings[i]->getY();
            rightTop = buildings[i];
        }
        meanX += buildings[i]->getX();
        meanY += buildings[i]->getY();
    }
    setLeftBottom(leftBottom);
    setRightTop(rightTop);

    meanX /= buildings.size();
    meanY /= buildings.size();
    Point p(meanX, meanY);
    int min = MAX_INT;
    Building* center = buildings[0];
    for(int i = 0 ; i < buildings.size(); i++)
    {
        if(buildings[i]->getPos().distFromPoint(p) < min)
        {
            min = buildings[i]->getPos().distFromPoint(p);
            center = buildings[i];
        }
    }
    setCenter(center);

}