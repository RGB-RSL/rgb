/* 
 * File:   Genetic.cpp
 * Author: Mehran Akhavan
 * 
 * Created on February 22, 2011, 4:07 PM
 */

#include "Genetic.h"
#include <stdlib.h>
#include <random>
#include <algorithm>
#define RANDOM_SEED 353

#define LOGLEVEL 8
using namespace std;
using namespace Types;

//bool matchingComparator(Genetic::Matching m1, Genetic::Matching m2)
//{
//    return m1.value > m2.value;
//}

Genetic::Genetic()
{
}

Genetic::Genetic(WorldGraph *worldGraph, WorldModel *world)
{
    this->worldGraph = worldGraph;
    this->world = world;
    firstTime = true;
}

Genetic::~Genetic()
{
}

int Genetic::getRescuedTime(std::vector<AmbulanceTeam*>& ambulances, Human* human)
{
    LOG(Main, 1) << "get rescued time begins" << endl;
    int timeToRescue = 0, buriedness = human->getBuriedness();
    int timesToReachHuman[ambulances.size()];
    for (int i = 0; i < ambulances.size(); i++)
    {
        LOG(Main, 1) << "ambulances[i]->getRepresentiveNodeIndex(): " << ambulances[i]->getRepresentiveNodeIndex() << endl;
        LOG(Main, 1) << "human->getRepresentiveNodeIndex(): " << human->getRepresentiveNodeIndex() << endl;
        timesToReachHuman[i] = worldGraph->getDistance(ambulances[i]->getRepresentiveNodeIndex(), human->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED) / AGENT_SPEED;
        LOG(Main, 1) << "ambulance " << ambulances[i]->getId() << " timesToReachCivilian " << human->getId() << " : " << timesToReachHuman[i] << " and dist is " << worldGraph->getDistance(ambulances[i]->getRepresentiveNodeIndex(), human->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED) << endl;
    }
    sort(timesToReachHuman, timesToReachHuman + ambulances.size());
    for (int i = 0; i < ambulances.size() && buriedness > 0; i++)
    {
        LOG(Main, 1) << "buriedness: " << buriedness << endl;
        buriedness -= i * timesToReachHuman[i];
        timeToRescue += timesToReachHuman[i];
        LOG(Main, 1) << "rescued Time: " << timeToRescue << endl;
        for (int j = i + 1; j < ambulances.size(); j++)
            timesToReachHuman[j] -= timesToReachHuman[i];
    }
    if (buriedness > 0)
    {
        if (buriedness % ambulances.size() == 0)
            timeToRescue += buriedness / ambulances.size();
        else
            timeToRescue += buriedness / ambulances.size() + 1;
    }
    LOG(Main, 1) << "timeTorecue: " << timeToRescue << endl;
    int minDist = MAX_INT;
    if (world->refuges.size() > 0)
    {
        for (int i = 0; i < world->refuges.size(); i++)
        {
            int dist = worldGraph->getDistance(world->refuges[i]->getRepresentiveNodeIndex(), human->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED);
            if (dist < minDist)
            {
                minDist = dist;
            }
        }
        timeToRescue += minDist / AGENT_SPEED;
    }
    //    LOG(Main, 1) << "END rescued Time: " << timeToRescue << endl;
    LOG(Main, 3) << "get rescued time ended" << endl;
    LOG(Main, 1) << "result: " << timeToRescue << endl;
    return timeToRescue;
}

int Genetic::getRescuedTime2(std::vector<AmbulanceTeam*>& ambulances, Human* human)
{
    LOG(Main, 8) << "get rescued time begins" << endl;
    if (human == NULL)
        LOG(Main, 8) << "ajab" << endl;
    LOG(Main, 8) << "ambulances.size " << ambulances.size() << endl;
    int timeToRescue = 0, buriedness = human->getBuriedness();
    LOG(Main, 8) << "2" << endl;
    int timesToReachHuman[ambulances.size()];
    LOG(Main, 8) << "3" << endl;
    for (int i = 0; i < ambulances.size(); i++)
    {
        LOG(Main, 8) << "ambulances[i]->getRepresentiveNodeIndex(): " << ambulances[i]->getRepresentiveNodeIndex() << endl;
        LOG(Main, 8) << "human->getRepresentiveNodeIndex(): " << human->getRepresentiveNodeIndex() << endl;
        timesToReachHuman[i] = worldGraph->getDistance(ambulances[i]->getRepresentiveNodeIndex(), human->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED) / AGENT_SPEED;
        LOG(Main, 8) << "ambulance " << ambulances[i]->getId() << " timesToReachCivilian " << human->getId() << " : " << timesToReachHuman[i] << " and dist is " << worldGraph->getDistance(ambulances[i]->getRepresentiveNodeIndex(), human->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED) << endl;
    }
    sort(timesToReachHuman, timesToReachHuman + ambulances.size());
    for (int i = 0; i < ambulances.size() && buriedness > 0; i++)
    {
        LOG(Main, 1) << "buriedness: " << buriedness << endl;
        buriedness -= i * timesToReachHuman[i];
        timeToRescue += timesToReachHuman[i];
        LOG(Main, 1) << "rescued Time: " << timeToRescue << endl;
        for (int j = i + 1; j < ambulances.size(); j++)
            timesToReachHuman[j] -= timesToReachHuman[i];
    }
    if (buriedness > 0)
    {
        if (buriedness % ambulances.size() == 0)
            timeToRescue += buriedness / ambulances.size();
        else
            timeToRescue += buriedness / ambulances.size() + 1;
    }
    LOG(Main, 8) << "timeTorecue: " << timeToRescue << endl;
    int minDist = MAX_INT;
    for (int i = 0; i < world->refuges.size(); i++)
    {
        int dist = worldGraph->getDistance(world->refuges[i]->getRepresentiveNodeIndex(), human->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED);
        if (dist < minDist)
        {
            minDist = dist;
        }
    }
    timeToRescue += minDist / AGENT_SPEED;
    //    LOG(Main, 1) << "END rescued Time: " << timeToRescue << endl;
    LOG(Main, 1) << "get rescued time ended" << endl;
    LOG(Main, 8) << "result: " << timeToRescue << endl;
    return timeToRescue;
}

void Genetic::start(const std::vector<Human*>& humans, const std::vector<AmbulanceTeam*>& ambulances, int numberOfgenerations, int numberOfSamples, int limit)
{
    srand(RANDOM_SEED);
    this->humans = humans;
    this->ambulances = ambulances;
    set<Matching, Matching> samples;
    LOG(Main, 8) << "Genetic BEGINS" << endl;
    LOG(Main, 3) << "Humans.size: " << humans.size() << endl;
    ambulancesIsReachableTo.clear();
    ambulancesIsReachableTo.resize(ambulances.size());
    for (int i = 0; i < ambulances.size(); i++)
    {
        for (int j = 0; j < humans.size(); j++)
        {
            if (worldGraph->isReachable(ambulances[i]->getMotionlessObject()->getRepresentiveNodeIndex(), humans[j]->getMotionlessObject()->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
            {
//                LOG(Main, 8) << ambulances[i]->getId() << " and " << humans[j]->getId() << endl;
                ambulancesIsReachableTo[i].push_back(j);
            }
        }
    }
//    LOG(Main, 8) << "*************************************************************" << endl;
//    for (int i = 0; i < ambulancesIsReachableTo.size(); i++)
//    {
//        LOG(Main, 8) << "reachable size " << ambulancesIsReachableTo[i].size() << endl;
//        for (int j = 0; j < ambulancesIsReachableTo[i].size(); j++)
//            LOG(Main, 8) << "ambulance " << ambulances[i]->getId() << " is reachable to " << humans[ambulancesIsReachableTo[i][j]]->getId() << endl;
//    }
    //    for(int i = 0 ; i < numberOfSamples; i++)
    if (!firstTime)
    {
        Matching lastBestMatching2;
        creatLastBestMatching(lastBestMatching2, limit);
        setValue(lastBestMatching2);
        LOG(Main, 8) << "lastBestMatching2 created" << endl;
        samples.insert(lastBestMatching2);
    }
    LOG(Main, 8) << "samples creating begins" << endl;
    int counter = 0;
    while (samples.size() < numberOfSamples && counter < limit)
    {
        //        samples[i].init( civilians, ambulances);
        //        samples[i].creatRandomly();
        Matching m;
        creatRandomly(m, limit);
        setValue(m);
        samples.insert(m);
        counter++;
    }

    LOG(Main, 8) << "samples creating ends " << samples.size() << endl;
    //    sort(samples, samples + numberOfSamples, matchingComparator);
    //
    for (set<Matching>::iterator it = samples.begin(); it != samples.end(); it++)
    {
        LOG(Main, 6) << "**********************************" << endl;
        for (int j = 0; j < (*it).matches.size(); j++)
        {
            LOG(Main, 6) << "Ambulance with id: " << ambulances[j]->getId() << " matched to " << humans[(*it).matches[j]]->getId() << endl;
        }
        LOG(Main, 6) << "Value of matching: " << (*it).value << endl;
    }
    LOG(Main, 1) << "before for of creat childes" << endl;
    int counter3 = 0;
    for (int i = 0; i < numberOfgenerations && samples.size() > 3; i++)
    {
        counter3++;
        int rand1 = rand() % (samples.size() - 2), rand2;
        set<Matching>::iterator it = samples.begin();
        for (int i = rand1; i > 0; i--)
        {
            it++;
        }
        const Genetic::Matching parent1 = (*it);
        do
        {
            rand2 = rand() % (samples.size() - 2);
        }
        while (rand1 == rand2);

        set<Matching>::iterator it2 = samples.begin();
        for (int i = rand2; i > 0; i--)
        {
            it2++;
        }
        const Genetic::Matching parent2 = (*it2);

        LOG(Main, 5) << "Baba: " << endl;
        for (int j = 0; j < parent1.matches.size(); j++)
        {
            LOG(Main, 5) << "Ambulance with id: " << ambulances[j]->getId() << " matched to " << humans[parent1.matches[j]]->getId() << endl;
        }
        LOG(Main, 5) << "Maman: " << endl;
        for (int j = 0; j < parent2.matches.size(); j++)
        {
            LOG(Main, 5) << "Ambulance with id: " << ambulances[j]->getId() << " matched to " << humans[parent2.matches[j]]->getId() << endl;
        }

        //        LOG(Main, 8) << "before creat pesar" << endl;
        samples.erase(--samples.end());
        Matching child1 = creatChild(parent1, parent2, true, limit);
        LOG(Main, 6) << "after creat child" << endl;
        setValue(child1);
        LOG(Main, 7) << "before insert child" << endl;
        samples.insert(child1);
        //        LOG(Main, 8) << "pesar is borned!" << endl;
        LOG(Main, 3) << "///////////////////////////////////// " << endl;
        //        LOG(Main, 3) << "Bache1: " << endl;
        //        for(int j = 0; j < samples[numberOfSamples - 1].matches.size() ; j++)
        //        {
        //            LOG(Main, 3) << "Ambulance with id: " << ambulances[j]->getId() << " matched to " << civilians[samples[numberOfSamples - 1].matches[j]]->getId() << endl;
        //        }
        samples.erase(--samples.end());
        //        LOG(Main, 8) << "before creat dokhtar" << endl;
        Matching child2 = creatChild(parent1, parent2, false, limit);
        //        LOG(Main, 8) << "dokhtar is borned!" << endl;
        setValue(child2);
        LOG(Main, 7) << "before insert child" << endl;
        samples.insert(child2);
        LOG(Main, 7) << "after insert child" << endl;
        LOG(Main, 7) << "samples.size = " << samples.size() << endl;
        //        LOG(Main, 3) << "Bache2: " << endl;
        //        for(int j = 0; j < samples[numberOfSamples - 2].matches.size() ; j++)
        //        {
        //            LOG(Main, 3) << "Ambulance with id: " << ambulances[j]->getId() << " matched to " << civilians[samples[numberOfSamples - 2].matches[j]]->getId() << endl;
        //        }
        //        LOG(Main, 5) << "before sort" << endl;
        //        sort(samples, samples + numberOfSamples, matchingComparator);
        //        LOG(Main, 5) << "after sort" << endl;
    }
    LOG(Main, 8) << "GENETIC ENDS" << endl;
    LOG(Main, 8) << "num of generations " << counter3 << endl;
    Matching bestMatching = *samples.find(*samples.begin());
    for (int i = 0; i < bestMatching.matches.size(); i++)
    {
        if (bestMatching.matches[i] != -1)
        {
            ambulances[i]->humanTarget = humans[bestMatching.matches[i]];
            ambulances[i]->buildingTarget = (Building*) humans[bestMatching.matches[i]]->getMotionlessObject();
            LOG(Main, 7) << "Ambulance with id: " << ambulances[i]->getId() << " matched to " << humans[bestMatching.matches[i]]->getId() << endl;
            LOG(Main, 8) << "line" << ambulances[i]->getMotionlessObject()->getPos() << humans[bestMatching.matches[i]]->getMotionlessObject()->getPos() << " 2; ";
            int counter2 = 0;
            for (int j = 0; j < humans.size(); j++)
            {
                if (humans[j]->getMotionlessObject()->motionlessIndex == humans[bestMatching.matches[i]]->getMotionlessObject()->motionlessIndex)
                {
                    if (humans[j]->getId() < humans[bestMatching.matches[i]]->getId())
                        counter2++;
                }
            }
            ambulances[i]->targetIndex = counter2;
            LOG(Main, 7) << "targetINdex= " << ambulances[i]->targetIndex << endl;
        }
        else
        {

            ambulances[i]->humanTarget = NULL;
            ambulances[i]->buildingTarget = NULL;
        }
    }
    LOG(Main, 8) << endl;
    setValue2(bestMatching);
    //    setValue2(bestMatching);
    //    LOG(Main, 8) << "best last matching" << endl;
    //    if(!firstTime)
    //        setValue2(lastBestMatching);
    //    lastBestMatching = bestMatching;
    firstTime = false;
}

void Genetic::creatLastBestMatching(Genetic::Matching& matching, int limit)
{

    matching.matches.resize(ambulances.size());
    fill(matching.matches.begin(), matching.matches.end(), -1);
    matching.ambulancesWhichMatched.resize(humans.size());
    for (int i = 0; i < ambulances.size(); i++)
    {
        Human* human = ambulances[i]->humanTarget;
        if (human != NULL)
        {
            for (int j = 0; j < humans.size(); j++)
            {
                if (humans[j]->humanIndex == human->humanIndex && worldGraph->isReachable(humans[j]->getMotionlessObject()->getRepresentiveNodeIndex(), ambulances[i]->getMotionlessObject()->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
                {
                    matching.matches[i] = j;
                    matching.ambulancesWhichMatched[j].push_back(ambulances[i]);
                }
            }
        }
    }
    LOG(Main, 2) << "after first for" << endl;
    int counter = 0;
    do
    {
        for (int i = 0; i < ambulances.size(); i++)
        {
            if (matching.matches[i] == -1)
            {
                if (ambulancesIsReachableTo[i].size() == 0)
                    continue;
                //                int rand2 = rand() % humans.size();
                int rand2 = rand() % ambulancesIsReachableTo[i].size();
                //                if(!worldGraph->isReachable(ambulances[i]->getRepresentiveNodeIndex(), humans[rand2]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
                //                    continue;
                matching.matches[i] = ambulancesIsReachableTo[i][rand2];
                matching.ambulancesWhichMatched[ambulancesIsReachableTo[i][rand2]].push_back(ambulances[i]);
            }
        }
        LOG(Main, 2) << "coutner: " << counter << endl;
        for (int i = 0; i < humans.size(); i++)
        {
            if (humans[i]->isCivilian())
            {
                if (matching.ambulancesWhichMatched[i].size() > 0 && getRescuedTime(matching.ambulancesWhichMatched[i], humans[i]) > ((Civilian*) humans[i])->getTimeToDeath())
                {
                    matching.ambulancesWhichMatched[i].clear();
                    for (int j = 0; j < matching.matches.size(); j++)
                    {
                        if (matching.matches[j] == i)
                            matching.matches[j] = -1;
                    }
                }
            }
        }
        counter++;
    }

    while (!isValid(matching) && counter < limit);
}

void Genetic::creatRandomly(Genetic::Matching& matching, int limit)
{
    matching.matches.resize(ambulances.size());
    //    int numOfAmbulancesWhichMatched[civs.size()];
    fill(matching.matches.begin(), matching.matches.end(), -1);
    //    fill(numOfAmbulancesWhichMatched, numOfAmbulancesWhichMatched + civs.size(), 0);
    int counter = 0;
    matching.ambulancesWhichMatched.resize(humans.size());
    do
    {
        for (int i = 0; i < ambulances.size(); i++)
        {
            if (matching.matches[i] == -1)
            {
                if (ambulancesIsReachableTo[i].size() == 0)
                    continue;
                //                int rand2 = rand() % humans.size();
                int rand2 = rand() % ambulancesIsReachableTo[i].size();
                //                if(!worldGraph->isReachable(ambulances[i]->getRepresentiveNodeIndex(), humans[rand2]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
                //                    continue;
                matching.matches[i] = ambulancesIsReachableTo[i][rand2];
                matching.ambulancesWhichMatched[ambulancesIsReachableTo[i][rand2]].push_back(ambulances[i]);
            }
        }
        for (int i = 0; i < humans.size(); i++)
        {
            if (matching.ambulancesWhichMatched[i].size() > 0)
            {
                if (humans[i]->isCivilian())
                {
                    if (getRescuedTime(matching.ambulancesWhichMatched[i], humans[i]) > ((Civilian*) humans[i])->getTimeToDeath())
                    {
                        matching.ambulancesWhichMatched[i].clear();
                        for (int j = 0; j < matching.matches.size(); j++)
                        {
                            if (matching.matches[j] == i)
                                matching.matches[j] = -1;
                        }
                    }
                }
            }
        }
        counter++;
    }

    while (!isValid(matching) && counter < limit);
    //    LOG(Main , 8) << "counter: " << counter << endl;
}

Genetic::Matching Genetic::creatChild(const Genetic::Matching& parent1, const Genetic::Matching& parent2, bool type, int limit)
{
    Genetic::Matching child;
    child.ambulancesWhichMatched.resize(humans.size());
    child.matches.resize(ambulances.size());
    fill(child.matches.begin(), child.matches.end(), -1);
    //    LOG(Main, 8) << "1" << endl;
    for (int i = 0; i < ambulances.size(); i++)
    {
        if (type)
        {
            if (i % 2 == 0)
            {
                if (parent1.matches[i] == -1)
                    continue;
                child.matches[i] = parent1.matches[i];
                child.ambulancesWhichMatched[parent1.matches[i]].push_back(ambulances[i]);
            }
            else
            {
                if (parent2.matches[i] == -1)
                    continue;
                child.matches[i] = parent2.matches[i];
                child.ambulancesWhichMatched[parent2.matches[i]].push_back(ambulances[i]);
            }
        }
        else
        {
            if (i % 2 == 0)
            {
                if (parent2.matches[i] == -1)
                    continue;
                child.matches[i] = parent2.matches[i];
                child.ambulancesWhichMatched[parent2.matches[i]].push_back(ambulances[i]);
            }
            else
            {
                if (parent1.matches[i] == -1)
                    continue;
                child.matches[i] = parent1.matches[i];
                child.ambulancesWhichMatched[parent1.matches[i]].push_back(ambulances[i]);
            }
        }
    }
    //    LOG(Main, 8) << "2" << endl;
    for (int i = 0; i < humans.size(); i++)
    {
        if (humans[i]->isCivilian())
        {
            if (child.ambulancesWhichMatched[i].size() > 0 && getRescuedTime(child.ambulancesWhichMatched[i], humans[i]) > ((Civilian*) humans[i])->getTimeToDeath())
            {
                child.ambulancesWhichMatched[i].clear();
                for (int j = 0; j < child.matches.size(); j++)
                {
                    if (child.matches[j] == i)
                        child.matches[j] = -1;
                }
            }
        }
    }
    //    LOG(Main, 8) << "3" << endl;
    for (int j = 0; j < child.matches.size(); j++)
    {
        LOG(Main, 6) << "matching " << child.matches[j] << endl;
    }
    if (isValid(child))
        return child;
    LOG(Main, 7) << "after first isValid" << endl;

    //    LOG(Main, 8) << "4 " << endl;
    for (int i = 0; i < ambulances.size(); i++)
    {
        if (child.matches[i] == -1)
        {
            if (type)
            {
                if (i % 2 == 0)
                {
                    if (parent2.matches[i] == -1)
                        continue;
                    child.matches[i] = parent2.matches[i];
                    child.ambulancesWhichMatched[parent2.matches[i]].push_back(ambulances[i]);
                }
                else
                {
                    if (parent1.matches[i] == -1)
                        continue;
                    child.matches[i] = parent1.matches[i];
                    child.ambulancesWhichMatched[parent1.matches[i]].push_back(ambulances[i]);
                }
            }
            else
            {
                if (i % 2 == 0)
                {
                    if (parent1.matches[i] == -1)
                        continue;
                    child.matches[i] = parent1.matches[i];
                    child.ambulancesWhichMatched[parent1.matches[i]].push_back(ambulances[i]);
                }
                else
                {
                    if (parent2.matches[i] == -1)
                        continue;
                    child.matches[i] = parent2.matches[i];
                    child.ambulancesWhichMatched[parent2.matches[i]].push_back(ambulances[i]);
                }
            }
        }
    }
    //    LOG(Main, 8) << "5 " << endl;
    for (int i = 0; i < humans.size(); i++)
    {
        if (humans[i]->isCivilian())
        {
            if (child.ambulancesWhichMatched[i].size() > 0 && getRescuedTime(child.ambulancesWhichMatched[i], humans[i]) > ((Civilian*) humans[i])->getTimeToDeath())
            {
                child.ambulancesWhichMatched[i].clear();
                for (int j = 0; j < child.matches.size(); j++)
                {
                    if (child.matches[j] == i)
                        child.matches[j] = -1;
                }
            }
        }
    }
    //    LOG(Main, 8) << "6 " << endl;
    if (isValid(child))
        return child;
    LOG(Main, 7) << "after second isValid" << endl;

    int counter = 0;
    do
    {
        for (int i = 0; i < ambulances.size(); i++)
        {
            if (child.matches[i] == -1)
            {
                if (ambulancesIsReachableTo[i].size() == 0)
                    continue;
                //                int rand2 = rand() % humans.size();
                int rand2 = rand() % ambulancesIsReachableTo[i].size();
                //                if(!worldGraph->isReachable(ambulances[i]->getRepresentiveNodeIndex(), humans[rand2]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
                //                    continue;
                child.matches[i] = ambulancesIsReachableTo[i][rand2];
                child.ambulancesWhichMatched[ambulancesIsReachableTo[i][rand2]].push_back(ambulances[i]);
            }
        }
        for (int i = 0; i < humans.size(); i++)
        {
            if (humans[i]->isCivilian())
            {
                if (child.ambulancesWhichMatched[i].size() > 0 && getRescuedTime(child.ambulancesWhichMatched[i], humans[i]) > ((Civilian*) humans[i])->getTimeToDeath())
                {
                    child.ambulancesWhichMatched[i].clear();
                    for (int j = 0; j < child.matches.size(); j++)
                    {
                        if (child.matches[j] == i)
                            child.matches[j] = -1;
                    }
                }
            }
        }
        counter++;

    }

    while (!isValid(child) && counter < limit);
    //    LOG(Main, 8) << "counter: " << counter << endl;
    return child;
}

bool Genetic::isValid(Genetic::Matching& matching)
{
    for (int i = 0; i < matching.matches.size(); i++)

        if (matching.matches[i] == -1 && ambulancesIsReachableTo[i].size() != 0)
            return false;
    return true;
}

void Genetic::setValue(Genetic::Matching& matching)
{
    LOG(Main, 2) << "start valuing!!!!!" << endl;
    matching.value = 0;
    set<int> marked;
    for (int i = 0; i < matching.matches.size(); i++)
    {
        if (matching.matches[i] != -1)
        {
            if (marked.find(matching.matches[i]) == marked.end())
            {
                matching.value++;
                if (humans[matching.matches[i]]->isPlatoon())
                    matching.value += 3;
                marked.insert(matching.matches[i]);
            }
        }
    }
    matching.value *= 1000;
    LOG(Main, 2) << "Value1 (number of humans in matching): " << matching.value << endl;
    int meanTTD = 0, counter = 0, meanDist = 0, counter2 = 0;
    for (int i = 0; i < humans.size(); i++)
    {
        if (matching.ambulancesWhichMatched[i].size() > 0)
        {
            matching.value -= getRescuedTime(matching.ambulancesWhichMatched[i], humans[i]) * matching.ambulancesWhichMatched[i].size() * 20;
            int distFromFire = getDistanceFromFire(humans[i]);
            if(distFromFire != MAX_INT)
                matching.value += getDistanceFromFire(humans[i]) / 40;
            for(int k = 0 ; k < matching.ambulancesWhichMatched[i].size(); k++)
            {
                meanDist += worldGraph->getDistance(matching.ambulancesWhichMatched[i][k]->getRepresentiveNodeIndex(), humans[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED);
                counter2++;
            }
            LOG(Main, 2) << "Value2 (RescuedTime): " << matching.value << endl;
            if (humans[i]->isCivilian())
                meanTTD += ((Civilian*) humans[i])->getTimeToDeath();
            counter++;
        }
    }

    if (counter != 0)
        meanTTD /= counter;
    matching.value -= meanTTD * 6;
    if (counter2 != 0)
        meanDist /= counter2;
    matching.value -= meanDist / 40;
    LOG(Main, 2) << "Value3 (TTD): " << matching.value << endl;
}

void Genetic::setValue2(Genetic::Matching& matching)
{
    LOG(Main, 8) << "start valuing!!!!!" << endl;
    matching.value = 0;
    set<int> marked;
    for (int i = 0; i < matching.matches.size(); i++)
    {
        if (matching.matches[i] != -1)
        {
            if (marked.find(matching.matches[i]) == marked.end())
            {
                matching.value++;
                if (humans[matching.matches[i]]->isPlatoon())
                    matching.value += 3;
                marked.insert(matching.matches[i]);
            }
        }
    }
    matching.value *= 1000;
    LOG(Main, 8) << "Value1 (number of humans in matching): " << matching.value << endl;
    int meanTTD = 0, counter = 0, meanDist = 0, counter2 = 0;
    for (int i = 0; i < humans.size(); i++)
    {
        if (matching.ambulancesWhichMatched[i].size() > 0)
        {
            matching.value -= getRescuedTime(matching.ambulancesWhichMatched[i], humans[i]) * matching.ambulancesWhichMatched[i].size() * 20;
            int distFromFire = getDistanceFromFire(humans[i]);
            LOG(Main, 8) << "Value2 (RescuedTime): " << matching.value << endl;
            if(distFromFire != MAX_INT)
                matching.value -= getDistanceFromFire(humans[i]) / 30000;
            for(int k = 0 ; k < matching.ambulancesWhichMatched[i].size(); k++)
            {
                meanDist += worldGraph->getDistance(matching.ambulancesWhichMatched[i][k]->getRepresentiveNodeIndex(), humans[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED);
                counter2++;
            }
            LOG(Main, 8) << "Value3 (Dist form fire): " << matching.value << endl;
            if (humans[i]->isCivilian())
                meanTTD += ((Civilian*) humans[i])->getTimeToDeath();
            counter++;
        }
    }

    if (counter != 0)
        meanTTD /= counter;
    matching.value -= meanTTD * 6;
    LOG(Main, 8) << "Value4 (TTD): " << matching.value << endl;
    if (counter2 != 0)
        meanDist /= counter2;
    LOG(Main , 8 ) << "meanDist is " << meanDist << endl;
    matching.value -= meanDist / 40;
    LOG(Main, 8) << "Value5 (dist): " << matching.value << endl;
}

int Genetic::getDistanceFromFire(Human* human)
{
    int minDist = MAX_INT;
    for(int i = 0 ; i < world->buildingsInFire.size(); i++)
    {
        int dist = human->getPos().distFromPoint(world->buildingsInFire[i]->getPos());
        if(dist < minDist)
        {
            minDist = dist;
        }
    }
    return minDist;
}

Genetic::Matching Genetic::Matching::operator =(Matching matching)
{

    this->matches.resize(matching.matches.size());
    this->matches = matching.matches;
    this->ambulancesWhichMatched.resize(matching.ambulancesWhichMatched.size());
    this->ambulancesWhichMatched = matching.ambulancesWhichMatched;
    this->value = matching.value;
    return *this;
}

bool Genetic::Matching::operator ==(Matching matching)
{
    if (this->matches == matching.matches &&
            this->ambulancesWhichMatched == matching.ambulancesWhichMatched)
        return true;

    else
        return false;

}

bool Genetic::Matching::operator() (Genetic::Matching m1, Genetic::Matching m2)
{
    return m1.value > m2.value;
}