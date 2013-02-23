/* 
 * File:   AmbulanceCenterAgent.h
 * Author: mehran
 *
 * Created on September 24, 2010, 3:56 PM
 */

#ifndef _AMBULANCECENTERAGENT_H
#define	_AMBULANCECENTERAGENT_H
#include "Agent.h"

class AmbulanceCenterAgent: public Agent
{
public:
	AmbulanceCenterAgent();
	virtual ~AmbulanceCenterAgent();
protected:
    void actBeforeRadar();
    void act();
    void precomputation();
    AmbulanceCenter* self();
private:

};

#endif	/* _AMBULANCECENTERAGENT_H */

