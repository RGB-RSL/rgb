/*
 * File:   AmbulanceTeamAgent.h
 * Author: mehran
 *
 * Created on September 24, 2010, 3:55 PM
 */

#ifndef _AMBULANCETEAMAGENT_H
#define	_AMBULANCETEAMAGENT_H
#include "Agent.h"
#include "map"
#include "Genetic.h"
class TimeToDeath
{
public:
    TimeToDeath(WorldModel *world, WorldGraph* worldGraph, int damagePerception, int hpPerception, int ignoreCommandUntil);
    void updateTimeToDeath();
private:
    WorldModel* world;
    WorldGraph* worldGraph;
    int damagePerception;
    int hpPerception;
    int ignoreCommandUntil;
    void guessTTD(Civilian* civ);
    std::map<Civilian*, int> firstDamage;
    int buriednessDamage;

};


class AmbulanceTeamAgent : public Agent
{
public:
    AmbulanceTeamAgent();
    virtual ~AmbulanceTeamAgent();
protected:
    void actBeforeRadar();
    void act();
    void precomputation();
    AmbulanceTeam* self();
private:
    bool isLoadingInjured(AmbulanceTeam* at);
    bool needEscape(AmbulanceTeam* at);
    bool needEscape();
    bool escape();
    void canGoRefuge();
    void unload();
    void setMyTarget();
    void setNeededHelpCiviliansAndAgents();
    void moveToRefuges();
    bool actGeneticResults();
    bool simpleDecision();
    void setValue(Human* human);
    int getNumOfNeededAmbulances(Human* human);
    bool canIGoRefuge;
    TimeToDeath* timeToDeath;
    Genetic* genetic;
    std::vector<Human*> neededHelpHumans;

    Building* targetForSearch;

    bool dontShasWhenIAmNotReachable();
    bool dontShasWhenShasingInAMotionless();
    bool dontShas[4];
    MotionlessObject* lastMotionless;
};

#endif	/* _AMBULANCETEAMAGENT_H */

