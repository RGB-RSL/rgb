/* 
 * File:   PoliceOfficeAgent.h
 * Author: mehran
 *
 * Created on September 24, 2010, 3:59 PM
 */

#ifndef _POLICEOFFICEAGENT_H
#define	_POLICEOFFICEAGENT_H
#include "Agent.h"

class PoliceOfficeAgent: public Agent
{
public:
    PoliceOfficeAgent();
    virtual ~PoliceOfficeAgent();
protected:
    void actBeforeRadar();
    void act();
    void precomputation();
private:

};

#endif	/* _POLICEOFFICEAGENT_H */

