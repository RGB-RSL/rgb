/* 
 * File:   Platoon.h
 * Author: mehran
 *
 * Created on October 7, 2010, 5:42 PM
 */

#ifndef _PLATOON_H
#define	_PLATOON_H

#include "Human.h"

class Platoon: public Human
{
public:
    static const int PLATOON_SIZE = 500;
    Platoon();
    Platoon(unsigned int Id);
    virtual ~Platoon();
    void setProperties(std::string type, int value);
    int platoonIndex;
    Types::CommandType myLastCommand;
private:

};

#endif	/* _PLATOON_H */

