/* 
 * File:   Genetic.h
 * Author: Mehran Akhavan
 *
 * Created on February 22, 2011, 4:07 PM
 */

#ifndef _GENETIC_H
#define	_GENETIC_H
#include "../WorldModel/WorldModel.h"
class Genetic
{
private:
    WorldGraph *worldGraph;
    WorldModel *world;
    std::vector<Human*> humans;
    std::vector<AmbulanceTeam*> ambulances;
    std::vector< std::vector<int> > ambulancesIsReachableTo;
public:
    struct Matching
    {
    public:
        std::vector<int> matches;
        std::vector<std::vector<AmbulanceTeam*> > ambulancesWhichMatched;
        int value;
        Matching operator=(Matching matching);
        bool operator==(Matching matching);
        bool operator()(Genetic::Matching m1, Genetic::Matching m2);
    };
    Genetic();
    Genetic(WorldGraph* worldGraph, WorldModel* world);
    void start(const std::vector<Human*> &humans,const std::vector<AmbulanceTeam*> &ambulances, int numberOfgenerations = 1000, int numberOfSamples = 300, int limit = 600);
    virtual ~Genetic();
private:
    int getRescuedTime(std::vector<AmbulanceTeam*>& ambulances,Human *human);
    int getRescuedTime2(std::vector<AmbulanceTeam*>& ambulances,Human *human);
    int getDistanceFromFire(Human* human);
    void creatLastBestMatching(Genetic::Matching& matching, int limit);
    void creatRandomly(Genetic::Matching& matching, int limit);
    bool isValid(Genetic::Matching& matching);
    void setValue(Genetic::Matching& matching);
    void setValue2(Genetic::Matching& matching);
    Genetic::Matching creatChild(const Genetic::Matching& parent1, const Genetic::Matching& parent2, bool type, int limit);
    Genetic::Matching lastBestMatching;
    bool firstTime;

};

#endif	/* _GENETIC_H */

