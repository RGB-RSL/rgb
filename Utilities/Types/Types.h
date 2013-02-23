/*
 * Types.h
 *
 *  Created on: Feb 6, 2010
 *      Author: Naivd Saleh
 */

#ifndef TYPES_H_
#define TYPES_H_
#include <string>
#include <float.h>
const int AGENT_SPEED = 30000;
typedef unsigned char byte;
const int MAX_INT = -1u / 2;
const int MAX_HP = 10000;
const int fireDamage = 12;
const float buriednessDamage = .1;
const double MAX_DOUBLE = DBL_MAX;

namespace Types
{

	enum EntityType
	{
		ET_FIRE_BRIGADE,
		ET_FIRE_STATION,
		ET_AMBULANCE_TEAM,
		ET_AMBULANCE_CENTER,
		ET_POLICE_FORCE,
		ET_POLICE_OFFICE,
		ET_CIVILIAN,
		ET_WORLD,
		ET_BULDING,
		ET_REFUGE,
		ET_ROAD,
		ET_BLOCKADE
	};

	enum MessageType
	{
		MT_AK_CONNECT,
		MT_AK_ACKNOWLEDGE,
		MT_KA_CONNECT_OK,
		MT_KA_CONNECT_ERROR,
		MT_KA_SENSE,
		MT_KA_HEAR,
		MT_AK_Clear,
		MT_AK_Extinguish,
		MT_AK_Load,
		MT_AK_Move,
		MT_AK_Rescue,
		MT_AK_Rest,
		MT_AK_Say,
		MT_AK_Speak,
		MT_AK_Subscribe,
		MT_AK_Tell,
		MT_AK_Unload
	};

	enum PropertyType
	{
		PT_START_TIME,
		PT_LONGTITUDE,
		PT_LATITUDE,
		PT_WIND_FORCE,
		PT_WIND_DIRECTION,
		PT_X,
		PT_Y,
		PT_BLOCKADES,
		PT_REPAIR_COST,
		PT_FLOORS,
		PT_BUILDING_ATTRIBUTES,
		PT_IGNITION,
		PT_FIERYNESS,
		PT_BROKENNESS,
		PT_BUILDING_CODE,
		PT_BUILDING_AREA_GROUND,
		PT_BUILDING_AREA_TOTAL,
		PT_APEXES,
		PT_EDGES,
		PT_POSITION,
		PT_DIRECTION,
		PT_POSITION_HISTORY,
		PT_STAMINA,
		PT_HP,
		PT_DAMAGE,
		PT_BURIEDNESS,
		PT_TRAVEL_DISTANCE,
		PT_WATER_QUANTITY,
		PT_TEMPERATURE,
		PT_IMPORTANCE,
		PT_TAIL,
		PT_HEAD,
		PT_LENGTH,
		PT_WIDTH
	};

	enum AgentType
	{
		AT_FIRE_BRIGADE,
		AT_FIRE_STATION,
		AT_AMBULANCE_TEAM,
		AT_AMBULANCE_CENTER,
		AT_POLICE_FORCE,
		AT_POLICE_OFFICE

	};

	enum ConfigType
	{
		CT_COMMUNICATION_PREFIX,
		CT_NUM_OF_CHANNELS,
		CT_PLATOON_MAX_CHANNELS,
		CT_CENTER_MAX_CHANNELS,
		CT_TYPE_SUFFIX,
		CT_NOISE_SUFFIX,
		CT_TYPE_VOICE,
		CT_TYPE_RADAR,
		CT_NOISE_TYPE_DROPOUT,
		CT_NOISE_TYPE_STATIC,
		CT_VOICE_RANGE,
		CT_BANDWIDTH_SUFIX,
		CT_MESSAGE_SIZE_SUFFIX,
		CT_MESSAGE_MAX_SUFFIX,
		CT_IGNORE_UNTIL,
		CT_STARTUP_CONNECT_TIME,
		CT_THINK_TIME,
                CT_DAMAGE_PERCEPTION,
                CT_HP_PERCEPTION,
                CT_REPAIR_RATE,
                CT_CLEAR_DISTANCE,
                CT_SIHGT_RANGE,
	};

        enum CommandType
        {
            CT_MOVE,
            CT_EXTINGUISH,
            CT_RESCUE,
            CT_CLEAR,
            CT_REST,
            CT_LOAD,
            CT_UNLOAD
        };

	enum ChannelJob
	{
		CJ_GENERALL,
		CJ_CIVILIAN,
		CJ_FIRE,
		CJ_BLOCKADE
	};

	enum MessageProtocol
	{
//            MP_AGENT_FIRST_TIME = 1,
//            MP_AGENT_NORMAL = 0x02,
//            MP_CIVILIAN_SPOTTED = 1,
            MP_END = 0,
            MP_CIVILIAN_NORMAL = 1,
            MP_CIVILIAN_FERT = 2,
            MP_BUILDING = 3,
            MP_ROAD_BLOCKADE_DATA = 4,
            MP_AGENT_FIRST_TIME = 5,
            MP_AMBULANCE_ORDER = 6
	};

	enum MessageSize
	{
		MS_BUILDING = 9,
		MS_CIVILIAN_NORMAL = 21,
		MS_CIVILIAN_FERT = 5,
		MS_BLOCKADE = 100
	};

	enum BuildingCode
	{
		WOODEN = 0,
		STEEL_FRAME = 1,
		REINFORCED_CONCRETE = 2
	};

	enum WayMode
	{
		ONE_WAY_MODE = 0,
		TWO_WAY_MODE = 1
	};

	enum PassingMode
	{
		PM_UNKNOWN = 0,
		PM_PASSABLE = 1,
		PM_NOT_PASSABLE = 2
	};

	enum GraphMode
	{
		GM_WITHOUT_BLOCKADE = 0,
		GM_DEFAULT_BLOCKED = 1,
		GM_DEFAULT_NOT_BLOCKED = 2
	};

	enum RadarMode
	{
		RM_ONE_CHANNEL,
		RM_DISTRIBUTE,
		RM_CENTER,
		RM_COMMUNICATIONLESS
	};

	template<typename _A, typename _B>
	struct SameType
	{
		static const bool value = false;
	};

	template<typename _A>
	struct SameType<_A, _A>
	{
		static const bool value = true;
	};

	template<typename _A, typename _B>
	bool sameType()
	{
		return SameType<_A, _B>::value;
	}

}

#endif /* TYPES_H_ */
