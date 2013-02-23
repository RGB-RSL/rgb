/* 
 * File:   FireStationAgent.h
 * Author: mehran
 *
 * Created on September 24, 2010, 3:58 PM
 */

#ifndef _FIRESTATIONAGENT_H
#define	_FIRESTATIONAGENT_H
#include "Agent.h"

class FireStationAgent: public Agent
{
public:
    FireStationAgent();
    virtual ~FireStationAgent();
protected:
    void actBeforeRadar();
    void act();
    void precomputation();
private:

};

#endif	/* _FIRESTATIONAGENT_H */

