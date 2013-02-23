/*

 * Radar.h
 *
 *  Created on: Jan 28, 2010
 *      Author: navid

 */
#ifndef RADAR_H_
#define RADAR_H_

#include "../WorldModel/WorldModel.h"
#include "VoiceChannel.h"
#include "RadioChannel.h"
#include "../Decision/Command.h"
#include "../Utilities/BoolStream.h"
#include <vector>


typedef unsigned char byte;
class WorldModel;
class Radar
{
private:
	void init();
        Types::RadarMode chooseRadarMode();
	WorldModel *world;
	WorldGraph *worldGraph;
	Command *command;
        Types::RadarMode radarMode;
	std::vector<VoiceChannel *> voiceChannels;
	std::vector<RadioChannel *> radioChannels;
	int numOfSubscribeForPlatoons;
	int numOfSubscribeForCenters;
	int maxByteOnCivilianChannelsForEachAgent;
	int maxByteOnBlockadeChannelsForEachAgent;
	int maxByteOnFireChannelsForEachAgent;
	//channels for sending data:
	int myBlockadeChannel;
	int myCivilianChannel;
	int myFireChannel;
	void setCivilianData(BoolStream &msg, int &mySize);
	void setFireData(BoolStream &msg, int &mySize);
	void setBlockadeData(BoolStream &msg, int &mySize);
        void setPersonalInformation(BoolStream &msg, int &mySize);
        void setAgentFirstTime(BoolStream &msg, int &mySize);
        void setAmbulancesOrders(BoolStream &msg, int &mySize);

        void say();

	bool checkValidateMaxByte(int maxByte, std::vector<RadioChannel*> channels);
	bool checkValidateMaxByte(int maxByte, std::vector<RadioChannel*> channels, int ignoreBandwidth);
        //
        int maxSizeForCaptainOfAmbulances;
        //variables & functions for Distributation radar mode:
	void setChannelsJobs();
	std::vector<RadioChannel *> channelsForCivilianInUse;
	std::vector<RadioChannel *> channelsForFireInUse;
	std::vector<RadioChannel *> channelsForBlockadeInUse;
	std::vector<int> channelsForFireBrigades;
	std::vector<int> channelsForAmbulanceTeams;
	std::vector<int> channelsForPoliceForces;
        //variables & functions for One Channel radar mode:
        int maxByteOnChannelsForEachAgent;
        int myChannel;
        std::vector<int> channelsForHearOfPlatoons;
        //variables & functions for center radar mode:
        int maxByteForCenters;
        std::vector<int> channelsForHearOfCenters;
        int maxByteForEachAgentWhenCentersSpeak;
        int myChannelWhenCentersSpeak;
        Building* mainCenter;
        void setPosition(Human* human, int position, int side);
        
        enum MaxBitsOfSituations
        {
            MBOS_HEADER = 3,
            MBOS_ID = 32,
            MBOS_HP = 14,
            MBOS_DAMAGE = 14,
            MBOS_POSITION = 15,
            MBOS_BURIEDNESS = 7,
            MBOS_INDEX_OF_CIVILIANS = 9,
            MBOS_INDEX_OF_BUILDGINS_AND_ROADS = 12,
            MBOS_INDEX_OF_PLATOONS = 7,
            MBOS_INDEX_OF_CIVILIANS_IN_A_BUILDING = 4,
            MBOS_SIZE = 7,
            MBOS_TEMPERTURE = 10,
            MBOS_TIME = 10,
            MBOS_FIERYNESS = 2,
            MBOS_WATER_QUANTITY = 1,
        };
public:
	Radar(WorldModel *world, WorldGraph *worldGraph, Command *command);
        void subscribe();
	std::vector<byte> updateWorld();
	void shareSenseWorldAndSendOrders();
	void analyseMessage(std::vector<byte>& msg, int& offset, int sizeOfContent, int senderId, int channel);
	virtual ~Radar();
};

#endif

