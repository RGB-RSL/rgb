/*
 * File:   AmbulanceTeamAgent.cpp
 * Author: mehran
 *
 * Created on September 24, 2010, 3:55 PM
 */

#include "AmbulanceTeamAgent.h"
#include "../Utilities/Geometry/GeometryFunctions.h"
#include "../Utilities/Debugger.h"
#include "../WorldModel/Objects/Civilian.h"
#define LOGLEVEL 1

using namespace std;
using namespace Types;
using namespace Geometry;

bool idComparator(Human* h1, Human* h2)
{
    return h1 < h2;
}

AmbulanceTeamAgent::AmbulanceTeamAgent()
{
}

AmbulanceTeamAgent::~AmbulanceTeamAgent()
{
}

void AmbulanceTeamAgent::precomputation()
{
    Agent::precomputation();
    lastMotionless = NULL;
    dontShas[0] = dontShas[1] = dontShas[2] = dontShas[3] = false;
    targetForSearch = NULL;
}

bool humanComparator(Human* human1, Human* human2)
{
    if (human1->isCivilian() && human2->isCivilian())
        return ((Civilian*) human1)->getTimeToDeath() < ((Civilian*) human2)->getTimeToDeath();
    else if(human1->isCivilian() && human2->isPlatoon())
        return false;
    else if(human1->isPlatoon() && human2->isCivilian())
        return true;
    else
        return human1->getBuriedness() < human2->getBuriedness();
}

bool AmbulanceTeamAgent::isLoadingInjured(AmbulanceTeam* at)
{
    for (int i = 0; i < world->humans.size(); i++)
    {
        if (world->humans[i]->getPosition() == at->getId())
            return true;
    }
    return false;
}

bool AmbulanceTeamAgent::needEscape(AmbulanceTeam* at)
{
    if (self()->getDamage() / self()->getHp() > .02 || self()->getDamage() > 90 || (self()->getHp() < 3500 && self()->getDamage() > 0))
        return true;
    return false;
}

void AmbulanceTeamAgent::actBeforeRadar()
{
    //recognize captain
    AmbulanceTeam* captain = NULL;
    for (int i = 0; i < world->ambulanceTeams.size(); i++)
    {
        if (world->ambulanceTeams[i]->isAvailable)
        {
            captain = world->ambulanceTeams[i];
            break;
        }
    }
    if (captain == NULL)
        LOG(Main, 1) << "captain is NULLLLLLLL" << endl;
    else
        LOG(Main, 1) << "captain is not null" << endl;
    //if I am captain
    if (captain != NULL && self()->ambulanceTeamIndex == captain->ambulanceTeamIndex)
    {
        vector<AmbulanceTeam*> ambulances;
        for (int i = 0; i < world->ambulanceTeams.size(); i++)
        {
//            LOG(Main, 1) << "positon of " << world->ambulanceTeams[i]->getId() << " is " << world->ambulanceTeams[i]->getMotionlessObject()->getId() << endl;
//            LOG(Main, 1) << "representative node " << world->ambulanceTeams[i]->getRepresentiveNodeIndex() << endl;
            if (world->ambulanceTeams[i]->getBuriedness() <= 0 && world->ambulanceTeams[i]->getRepresentiveNodeIndex() != -1 && world->ambulanceTeams[i]->isAvailable)
                ambulances.push_back(world->ambulanceTeams[i]);
        }
        LOG(Main, 1) << "*************************************************" << endl;
//        for (int i = 0; i < world->civilians.size(); i++)
//        {
//            LOG(Main, 1) << "position " << world->civilians[i]->getMotionlessObject()->getId() << endl;
//            LOG(Main, 1) << "representative node: " << world->civilians[i]->getRepresentiveNodeIndex() << endl;
//        }
        if (world->getTime() == 1)
        {
            genetic = new Genetic(worldGraph, world);
        }
        setNeededHelpCiviliansAndAgents();
        LOG(Main, 1) << "Help needed civilinas size =  " << neededHelpHumans.size() << endl;
        LOG(Main, 1) << "All civilinas size =  " << world->civilians.size() << endl;
        for (int i = 0; i < neededHelpHumans.size(); i++)
            LOG(Main, 1) << "needed help: " << neededHelpHumans[i]->getId() << " last cylce updated " << neededHelpHumans[i]->getLastCycleUpdated() << endl;
        //        vector<AmbulanceTeam*> ambulances;
        //        for(int i = 0 ; i < world->ambulanceTeams.size(); i++)
        //        {
        //            AmbulanceTeam* at = world->ambulanceTeams[i];
        //            if(at->getBuriedness() > 0)
        //                continue;
        //            if(isLoadingInjured(at))
        //                continue;
        //            if(needEscape(at))
        //                continue;
        //            ambulances.push_back(at);
        //        }
        int geneticTime = 5;
        geneticTime += (int) (neededHelpHumans.size() / 2);

        if (neededHelpHumans.size() > 0 && world->getTime() % geneticTime == 0)
        {
            LOG(Main, 1) << "before starting genetic" << endl;
            //            genetic->start(neededHelpCivilians, ambulances);
            genetic->start(neededHelpHumans, ambulances);
        }

    }
}

void AmbulanceTeamAgent::act()
{
    LOG(Main, 1) << "I am AmbulanceTeam" << endl;
    LOG(Main, 1) << "my Position: " << self()->getMotionlessObject()->getId() << endl;
    LOG(Main, 1) << "getWithoutBlockadeRepresentiveNodeIndex: " << self()->getWithoutBlockadeRepresentiveNodeIndex() << endl;
    if (world->getTime() < ignoreCommandsUntil)
    {
        if (world->getTime() == 1)
        {
            //            genetic = new Genetic(worldGraph, world);
            timeToDeath = new TimeToDeath(world,worldGraph, damagePerception, hpPerception, ignoreCommandsUntil);
        }
        return;
    }
//    for (int i = 0; i < worldGraph->getEdges().size(); i++)
//    {
//        Edge *edge = worldGraph->getEdges()[i];
//        switch (edge->getPassingMode())
//        {
//        case PM_PASSABLE:
//            setColor(0);
//            break;
//        case PM_NOT_PASSABLE:
//            setColor(4);
//            break;
//        case PM_UNKNOWN:
//            setColor(1);
//            break;
//        }
//        LOG(Main, 1) << Segment(worldGraph->getNodes()[edge->getFirstNodeIndex()]->getRepresentivePoint(), worldGraph->getNodes()[edge->getSecondNodeIndex()]->getRepresentivePoint()) << ' ';
//    }
//    LOG(Main, 1) << endl;
    // can I go to any refgue
    canGoRefuge();
    //escape
    if (escape())
        return;
    LOG(Main, 1) << "after escape" << endl;
    //unload
    if (self()->isLoadingInjured())
    {
        unload();
        return;
    }
    LOG(Main, 1) << "after unload" << endl;
    if (dontShasWhenIAmNotReachable())
        return;
    if (dontShasWhenShasingInAMotionless())
        return;
    //set humanTarget NULL if needed
    timeToDeath->updateTimeToDeath();
    //    choose help needed civilians
    setNeededHelpCiviliansAndAgents();
    LOG(Main, 1) << "after update TTD" << endl;
    setMyTarget();
    LOG(Main, 1) << "after setMyTarget" << endl;
    if (actGeneticResults())
        return;
    LOG(Main, 1) << "after actGenetic" << endl;
    LOG(Main, 1) << "Help needed civilinas size =  " << neededHelpHumans.size() << endl;
    LOG(Main, 1) << "All civilinas size =  " << world->civilians.size() << endl;
    for (int i = 0; i < neededHelpHumans.size(); i++)
        LOG(Main, 1) << "needed help: " << neededHelpHumans[i]->getId() << " last cylce updated " << neededHelpHumans[i]->getLastCycleUpdated() << endl;
    if (neededHelpHumans.size() > 0)
    {
        LOG(Main, 1) << "neededHelpCivilians.size(): " << neededHelpHumans.size() << endl;
        //set time to death
        //        LOG(Main, 1) << "before update time to death" << endl;
        //        timeToDeath->updateTimeToDeath();
        if (simpleDecision())
            return;
    }

    //SEARCH
    LOG(Main, 1) << "Searching..." << endl;
    if (targetForSearch != NULL && (self()->getMotionlessObject()->motionlessIndex == targetForSearch->motionlessIndex || !worldGraph->isReachable(self()->getRepresentiveNodeIndex(), targetForSearch->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED)))
        targetForSearch = NULL;
    if (targetForSearch == NULL)
        targetForSearch = search->actAmbulanceAndFireBrigadeSearch(canIGoRefuge);
    if (targetForSearch != NULL)
    {
        LOG(Main, 1) << "search target " << targetForSearch->getId() << endl;
        command->moveToMotionless(targetForSearch->motionlessIndex, GM_DEFAULT_NOT_BLOCKED);
        return;
    }
    if(!canIGoRefuge && self()->getMotionlessObject()->isBuilding())
        for(int i = 0 ; i < world->roads.size(); i++)
            if(worldGraph->isReachable(self()->getRepresentiveNodeIndex(), world->roads[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
            {
                command->moveToMotionless(world->roads[i]->motionlessIndex, GM_DEFAULT_NOT_BLOCKED);
                return;
            }
    moveToRefuges();
}

bool AmbulanceTeamAgent::actGeneticResults()
{
    if (self()->buildingTarget != NULL)
    {
        if (self()->buildingTarget->iHaveBeenHere && self()->humanTarget == NULL)
            return false;
        MotionlessObject* myGeneticTarget = self()->buildingTarget;
        LOG(Main, 1) << "my genetic target: " << myGeneticTarget->getId() << endl;
        if (self()->getMotionlessObject()->motionlessIndex != myGeneticTarget->motionlessIndex)
        {
            LOG(Main, 1) << "move to myTarget " << myGeneticTarget->getId() << endl;
            command->moveToMotionless(myGeneticTarget->motionlessIndex, GM_DEFAULT_NOT_BLOCKED);
            return true;
        }
        else
        {
            if (self()->humanTarget == NULL || !self()->humanTarget->isAvailable)
            {
                //                for(int i = 0 ; i < world->humans.size(); i++)
                //                {
                //                    if(world->humans[i]->isCivilian() || world->humans[i]->getBuriedness() > 0)
                //                        if(world->humans[i]->getMotionlessObject() == myGeneticTarget)
                //                            self()->humanTarget = world->humans[i];
                //                }
                return false;
            }
            else
            {
                LOG(Main, 1) << "my human target(in actGenetic) " << self()->humanTarget->getId() << endl;
                if (self()->humanTarget->getBuriedness() > 0)
                {
                    command->rescue(*self()->humanTarget);
                    return true;
                }
                else
                {
                    if (self()->humanTarget->isCivilian())
                    {
                        //who load the civilian
                        int minId = self()->getId();
                        for (int i = 0; i < world->ambulanceTeams.size(); i++)
                        {
                            if (/*world->ambulanceTeams[i]->buildingTarget == self()->buildingTarget &&*/ world->ambulanceTeams[i]->getId() < minId && world->ambulanceTeams[i]->getMotionlessObject()->motionlessIndex == self()->buildingTarget->motionlessIndex)
                                minId = world->ambulanceTeams[i]->getId();
                        }
                        if (self()->getId() == minId)
                        {
                            command->load(*self()->humanTarget);
                            return true;
                        }
                        else
                        {
                            self()->humanTarget->isAvailable = false;
                            self()->buildingTarget = NULL;
                            self()->humanTarget = NULL;
                        }
                    }
                    else
                    {
                        self()->buildingTarget = NULL;
                        self()->humanTarget = NULL;
                    }
                }
            }
        }
    }
    return false;
}

bool AmbulanceTeamAgent::simpleDecision()
{
    LOG(Main, 1) << "Simple Decision begins" << endl;
    //    for (int i = 0; i < neededHelpHumans.size(); i++)
    //        setValue(neededHelpHumans[i]);
    sort(neededHelpHumans.begin(), neededHelpHumans.end(), humanComparator);
    Human* myTarget;
    set<int> busyAgents;
    for (int i = 0; i < neededHelpHumans.size(); i++)
    {
        int neededAmbulances = getNumOfNeededAmbulances(neededHelpHumans[i]);
        LOG(Main, 1) << "neededAmbulances for " << neededHelpHumans[i]->getId() << " is " << neededAmbulances << endl;
        for (int j = 0; j < neededAmbulances; j++)
        {
            int minDist = MAX_INT;
            AmbulanceTeam* at = NULL;
            for (int k = 0; k < world->ambulanceTeams.size(); k++)
            {
                if (busyAgents.find(world->ambulanceTeams[k]->ambulanceTeamIndex) == busyAgents.end() && worldGraph->isReachable(world->ambulanceTeams[k]->getRepresentiveNodeIndex(), neededHelpHumans[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
                {
                    int dist = worldGraph->getDistance(world->ambulanceTeams[k]->getRepresentiveNodeIndex(), neededHelpHumans[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED);
                    //                    LOG(Main, 1) << "dist  " << world->ambulanceTeams[k]->getId() << " from civ is " << dist << endl;
                    if (dist < minDist)
                    {
                        //                        LOG(Main, 1) << "in if" << endl;
                        minDist = dist;
                        at = world->ambulanceTeams[k];
                        //                        LOG(Main, 1) << "at->getId " << at->getId() << endl;
                    }
                }
            }
            if (at != NULL)
            {
                if (at->ambulanceTeamIndex == self()->ambulanceTeamIndex)
                {
                    myTarget = neededHelpHumans[i];
                    break;
                }
                else
                {
                    LOG(Main, 1) << "ambulance " << at->getId() << " goes to " << neededHelpHumans[i]->getId() << endl;
                    busyAgents.insert(at->ambulanceTeamIndex);
                }
            }
        }
        if (myTarget != NULL)
            break;
    }

    if (myTarget != NULL)
    {
        LOG(Main, 1) << "my Target: " << myTarget->getId() << " its postion is " << myTarget->getMotionlessObject()->getId() << endl;
        if (self()->getMotionlessObject() != myTarget->getMotionlessObject())
        {
            LOG(Main, 1) << "move to myTarget " << myTarget->getId() << endl;
            command->moveToMotionless(myTarget->getMotionlessObject()->motionlessIndex, GM_DEFAULT_NOT_BLOCKED);
            return true;
        }
        else if (myTarget->getBuriedness() > 0)
        {
            LOG(Main, 1) << "I am rescuing my target " << endl;
            command->rescue(*myTarget);
            return true;
        }
        else
        {
            if (myTarget->isCivilian())
            {
                int minId = self()->getId();
                for (int i = 0; i < world->ambulanceTeams.size(); i++)
                    if (world->ambulanceTeams[i]->getMotionlessObject() == myTarget->getMotionlessObject() && world->ambulanceTeams[i]->getId() < minId)
                        minId = world->ambulanceTeams[i]->getId();
                if (minId == self()->getId())
                {
                    LOG(Main, 1) << "I am loading my target" << endl;
                    command->load(*myTarget);
                    return true;
                }
                else
                {
                    myTarget->isAvailable = false;
                }
            }
        }
    }
    return false;
}

int AmbulanceTeamAgent::getNumOfNeededAmbulances(Human* human)
{
    if (human->isCivilian())
    {
        int counter = 1;
        Civilian* civ = ((Civilian*) human);
        while (human->getBuriedness() / counter > civ->getTimeToDeath() && counter < world->ambulanceTeams.size())
            counter++;
        counter++;
        return counter;
    }
    else
    {
        int counter = 5;
        while (counter > world->ambulanceTeams.size())
            counter--;
        return counter;
    }
}

void AmbulanceTeamAgent::setValue(Human* human)
{
    human->value = 0;
    if (human->isCivilian())
    {
        Civilian* civ = (Civilian*) human;
        human->value -= civ->getTimeToDeath();
    }
    int rescueTime = 0;
    rescueTime = worldGraph->getDistance(self()->getRepresentiveNodeIndex(), human->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED) / AGENT_SPEED;
    rescueTime += human->getBuriedness();
    int minDist = MAX_INT / 2;
    for (int i = 0; i < world->refuges.size(); i++)
    {
        int dist = worldGraph->getDistance(world->refuges[0]->getRepresentiveNodeIndex(), human->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED);
        if (dist < minDist)
            minDist = dist;
    }
    rescueTime += minDist / AGENT_SPEED;
    human->value -= rescueTime;
    if (human->isPlatoon())
        human->value += 100;

}

void AmbulanceTeamAgent::setNeededHelpCiviliansAndAgents()
{
    neededHelpHumans.clear();
    for (int i = 0; i < world->civilians.size(); i++)
    {
        Civilian* civ = world->civilians[i];
        if (civ->getMotionlessObject()->isRefuge())
            continue;
        if (civ->getHp() < 1)
            continue;
        if (!civ->isAvailable)
            continue;
        if (civ->getMotionlessObject()->isRoad())
            continue;
        if(world->refuges.size() == 0 && civ->getTimeToDeath() > 200)
        {
            LOG(Main, 1) << "5" << endl;
            continue;
	}
        if (civ->getMotionlessObject()->isBuilding())
            if (((Building*) civ->getMotionlessObject())->getTemperature() > 100)
                continue;
        neededHelpHumans.push_back(civ);
    }
    for (int i = 0; i < world->platoons.size(); i++)
    {
        Platoon* platoon = world->platoons[i];
        if (!platoon->isAvailable)
            continue;
        if (platoon->getBuriedness() > 0)
            neededHelpHumans.push_back(platoon);
    }
}

void AmbulanceTeamAgent::setMyTarget()
{
    vector<Human*> myNominatedTarget;
    LOG(Main, 1) << "setMyTarget begins" <<endl;
    if (self()->buildingTarget != NULL)
    {
        LOG(Main, 1) << "in first if" <<endl;
        if (self()->buildingTarget->isRefuge())
        {
            self()->buildingTarget = NULL;
            self()->humanTarget = NULL;
			return;
        }
        if(!canIGoRefuge && self()->buildingTarget->isBurning())
        {
            self()->buildingTarget = NULL;
            self()->humanTarget = NULL;
            return;
        }
    }
    LOG(Main, 1) << "before second if" <<endl;
    if (self()->buildingTarget != NULL)
    {
        LOG(Main, 1) << "before is reachable" << endl;
        if(!worldGraph->isReachable(self()->buildingTarget->getRepresentiveNodeIndex(), self()->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
        {
            self()->buildingTarget = NULL;
            self()->humanTarget = NULL;
            return;
        }
        LOG(Main, 1) << "before for; neede help size: " << neededHelpHumans.size()  << endl;
        for (int i = 0; i < neededHelpHumans.size(); i++)
            if (neededHelpHumans[i]->getMotionlessObject()->motionlessIndex == self()->buildingTarget->motionlessIndex)
            {
//                if(world->humans[i]->isCivilian() || (world->humans[i]->isPlatoon() && world->humans[i]->getBuriedness() > 0))
//                {
                    myNominatedTarget.push_back(neededHelpHumans[i]);
//                }
            }
        LOG(Main, 1) << "myTargetIndex " << self()->targetIndex << endl;
        sort(myNominatedTarget.begin(), myNominatedTarget.end(), idComparator);
        if(myNominatedTarget.size() > self()->targetIndex)
        {
            LOG(Main, 1) << "myHumanTarget " << myNominatedTarget[self()->targetIndex]->getId() << endl;
            self()->humanTarget = myNominatedTarget[self()->targetIndex];
        }
        else if(self()->buildingTarget->iHaveBeenHere)
        {
            self()->buildingTarget = NULL;
            self()->humanTarget = NULL;
        }
    }
    else
        return;
//    self()->humanTarget = NULL;
//    if (myNominatedTarget.size() > 0)
//    {
//        for (set<Human*>::iterator it = myNominatedTarget.begin(); it != myNominatedTarget.end(); it++)
//        {
//            Human* human = *it;
//            if (!(human->isAvailable))
//            {
//                myNominatedTarget.erase(it);
//                continue;
//            }
//            if (human->isPlatoon() && human->getBuriedness() <= 0)
//            {
//                myNominatedTarget.erase(it);
//                continue;
//            }
//            if (self()->humanTarget == NULL)
//                self()->humanTarget = human;
//            else if (self()->humanTarget->isCivilian())
//                self()->humanTarget = human;
//        }
//    }
//    else if (self()->buildingTarget->iHaveBeenHere)
//    {
//        self()->buildingTarget = NULL;
//        self()->humanTarget = NULL;
//    }

}

void AmbulanceTeamAgent::unload()
{
    LOG(Main, 1) << "isLoaded" << endl;
    if(canIGoRefuge)
    {
        if (self()->getMotionlessObject()->isRefuge())
            command->unload();
        else
            moveToRefuges();
    } else if(self()->getRepresentiveNodeIndex() != -1)
    {
        int minDist = MAX_INT;
        Road* minRoad = NULL;
        for(int i = 0 ; i <  world->roads.size(); i++)
        {
            if(worldGraph->isReachable(self()->getRepresentiveNodeIndex(), world->roads[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
            {
                int dist = worldGraph->getAirDistance(self()->getRepresentiveNodeIndex(), world->roads[i]->getRepresentiveNodeIndex());
                if(dist < minDist)
                {
                    minDist = dist;
                    minRoad = world->roads[i];
                }
            }
        }
        if(minRoad != NULL)
        {
            if(self()->getMotionlessObject()->motionlessIndex == minRoad->motionlessIndex)
            {
                command->unload();
            } else
                command->moveToMotionless(minRoad->motionlessIndex, GM_DEFAULT_NOT_BLOCKED);
        } else
        {
            command->unload();
        }
    }
}

void AmbulanceTeamAgent::canGoRefuge()
{
    canIGoRefuge = false;
    for (int i = 0; i < world->refuges.size(); i++)
        if (worldGraph->isReachable(self()->getRepresentiveNodeIndex(), world->refuges[i]->getRepresentiveNodeIndex(), GM_DEFAULT_BLOCKED))
        {
            canIGoRefuge = true;
            break;
        }
}

bool AmbulanceTeamAgent::needEscape()
{
    if (self()->getDamage() / self()->getHp() > .02 || self()->getDamage() > 90 || (self()->getHp() < 3500 && self()->getDamage() > 0))
        return true;
    return false;
}

bool AmbulanceTeamAgent::escape()
{
    if (needEscape())
    {
        if (canIGoRefuge)
        {
            moveToRefuges();
            return true;
        }
    }
    return false;
}

bool AmbulanceTeamAgent::dontShasWhenIAmNotReachable()
{
    if (self()->getRepresentiveNodeIndex() == -1)
    {
        //        Polygon* polygon = &self()->getMotionlessObject()->getShape();
        //        Point middle = polygon->getMidPoint();
        //        int random = rand() % polygon->size();
        //        Point p2 = polygon->getVertex(random);
        //        Point randomPoint = Segment(middle, p2).getMiddlePoint();
        int randomNeighbour = rand() % self()->getMotionlessObject()->getNeighbours().size();
        vector<int> path;
        path.push_back(self()->getMotionlessObject()->getId());
        path.push_back(world->motionlessObjects[self()->getMotionlessObject()->getNeighbours()[randomNeighbour]]->getId());
        LOG(Main, 1) << "dontShasWhenIAmIsNotReachable " << world->motionlessObjects[self()->getMotionlessObject()->getNeighbours()[randomNeighbour]]->getId() << endl;
        command->move(path, -1, -1);
    }
    else
        return false;
}

bool AmbulanceTeamAgent::dontShasWhenShasingInAMotionless()
{
    if (lastMotionless != NULL && self()->myLastCommand == CT_MOVE && lastMotionless == self()->getMotionlessObject())
    {
        if (!dontShas[0])
            dontShas[0] = true;
        else if (!dontShas[1])
            dontShas[1] = true;
        else if (!dontShas[2])
            dontShas[2] = true;
        else if (!dontShas[3])
            dontShas[3] = true;
    }
    else
    {
        dontShas[0] = dontShas[1] = dontShas[2] = dontShas[3] = false;
    }
    if (dontShas[0] && dontShas[1] && dontShas[2] && dontShas[3])
    {
        dontShas[0] = dontShas[1] = dontShas[2] = dontShas[3] = false;
        command->moveToPoint(self()->getMotionlessObject()->motionlessIndex, self()->getMotionlessObject()->getPos(), GM_DEFAULT_NOT_BLOCKED);
        return true;
    }
    lastMotionless = self()->getMotionlessObject();
    return false;
}

void AmbulanceTeamAgent::moveToRefuges()
{
    LOG(Main, 1) << "move to refuges" << endl;
    int minDist = MAX_INT;
    Refuge* ref;
    for (int i = 0; i < world->refuges.size(); i++)
    {
        int dist = worldGraph->getDistance(self()->getRepresentiveNodeIndex(), world->refuges[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED);
        if (dist < minDist)
        {
            minDist = dist;
            ref = world->refuges[i];
        }
    }
    if (minDist != MAX_INT)
        command->moveToMotionless(ref->motionlessIndex, GM_DEFAULT_NOT_BLOCKED);
}

AmbulanceTeam* AmbulanceTeamAgent::self()
{
    return (AmbulanceTeam*) world->self;
}

TimeToDeath::TimeToDeath(WorldModel* world, WorldGraph* worldGraph,  int damagePerception, int hpPerception, int ignoreCommandUntil)
{
    this->world = world;
    this->worldGraph = worldGraph;
    this->damagePerception = damagePerception;
    this->hpPerception = hpPerception;
    this->ignoreCommandUntil = ignoreCommandUntil;
    this->buriednessDamage = .01;
}

void TimeToDeath::updateTimeToDeath()
{
    LOG(Main, 2) << "update TTD begins" << endl;
    for (int i = 0; i < world->civilians.size(); i++)
    {
        Civilian* civ = world->civilians[i];
        LOG(Main, 2) << "id: " << civ->getId() << " availablity: " << civ->isAvailable << " isRefuge " << civ->getMotionlessObject()->isRefuge() << " hp: " << civ->getHp() << " damage: " << civ->getDamage() << endl;
        if (!civ->isAvailable || civ->getMotionlessObject()->isRefuge() || civ->getHp() <= 1)
            continue;
        if (civ->getLastCycleUpdated() == world->getTime())
        {
            LOG(Main, 2) << "civilian updated in current cycle" << endl;
            int damage = civ->getDamage(), hp = civ->getHp();
            if (damage == 0)
                damage = damagePerception / 4;
            if (damage == 0)
                damage = 1;
            bool flag = true;
            int counter = 4;
            int limit = 1;
            while (flag && damage < civ->getDamage() + damagePerception / 2 && damage > civ->getDamage() - damagePerception / 2 && limit < 10)
            {
                int time = world->getTime();
                int tempDamage = damage;
                float tempHp = MAX_HP;
                while (time > ignoreCommandUntil)
                {
                    tempHp -= tempDamage;
                    tempDamage -= buriednessDamage * civ->getBuriedness();
                    time--;
                }
                //                int tempHp = MAX_HP - damage * (world->getTime() - ignoreCommandUntil);
                if (tempHp > hp + hpPerception / 2)
                    damage += damagePerception / counter;
                else if (tempHp < hp - hpPerception / 2)
                    damage -= damagePerception / counter;
                else
                    flag = false;
                counter *= 2;
                limit++;
                if (counter > 20)
                    LOG(Main, 2) << "ajaba" << endl;
            }
            if (damage == 0)
                damage = 5;
            LOG(Main, 2) << "damage= " << damage << endl;
//            int tolerance = 8;
//            if(civ->getMotionlessObject()->isBuilding())
//            {
//                LOG(Main, 1) << "is in Buiding" << endl;
//                Building* b = (Building*) civ->getMotionlessObject();
//                int minDist = MAX_INT;
//                Building* minBuilding = NULL;
//                for(int j = 0 ; j < world->buildings.size(); j++)
//                {
//                    if(world->buildings[j]->isBurning())
//                    {
//                        int dist = worldGraph->getAirDistance(civ->getRepresentiveNodeIndex(), world->buildings[j]->getRepresentiveNodeIndex());
//                        if(dist < minDist)
//                        {
//                            minDist = dist;
//                            minBuilding = world->buildings[j];
//                        }
//                    }
//                }
//                LOG(Main, 1) << "min dist " << minDist << endl;
//                tolerance = minDist / 10000;
//                LOG(Main, 1) << "tolerance " << tolerance << endl;
//
//            }
            civ->setTimeToDeath(hp / damage - 8);
            LOG(Main, 2) << "TTD = " << civ->getTimeToDeath() << endl;
        }
        else
        {
            civ->setTimeToDeath(civ->getTimeToDeath() - 1);
            LOG(Main, 2) << "Time to death = " << civ->getTimeToDeath();
        }
    }
}

void TimeToDeath::guessTTD(Civilian* civ)
{
    civ->setTimeToDeath(100);
}
