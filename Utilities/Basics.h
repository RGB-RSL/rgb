
#ifndef __BASICS_H_
#define __BASICS_H_

#include <string>
#include <cmath>

#define RTD (180.0f / float(M_PI))
#define DTR (float(M_PI) / 180.0f)
#define pow2(x) ((x) * (x))

namespace basics
{
	extern void WarnLoadingTrue(std::string);
	extern void WarnLoadingFalse(std::string);

}

#endif //__BISICS_H_
