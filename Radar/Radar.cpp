/*

 * Radar.cpp
 *
 *  Created on: Jan 28, 2010
 *      Author: Mehran Akhavan
 */

#include <string.h>
#include <errno.h>


#include "Radar.h"
#include "../Utilities/Encodings.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../Utilities/Debugger.h"
#include "AbstractChannel.h"
#include "../Utilities/BoolStream.h"

#define LOGLEVEL 0
#define MIN_NEEDED_BANDWITH_FOR_ONE_CHANNEL_MODE 100
#define MIN_DIFFRECE_BANDWITH_BETWEEN_CENTER_DITRIBUTED_MODE 25
#define CENTERS_CYCLE_FOR_SPEAK 5
#define MAX_TIME_FOR_SEND_AGENT_FIRST_TIME 400

using namespace std;
using namespace Types;

/* TODO List 
        1. add the noise reducer
                1.1. send lost datas for centers
                1.2. send datas several times   (Done)
        2. if a platoon have buriedness, must send in radar (Done)
        3. say command (Done)
        4. test choose radar mode (Done)
        5. set maxByteForCenters (taghriban Done)
        6. send type of activity of agents on radar (Ignore)
 */

bool channelComparator(RadioChannel* c1, RadioChannel* c2)
{
    return c1->getBandWidth() > c2->getBandWidth();
}

Radar::Radar(WorldModel* world, WorldGraph *worldGraph, Command *command)
{
    this->world = world;
    this->command = command;
    this->worldGraph = worldGraph;
    init();
    for (int i = 0; i < (int) voiceChannels.size(); i++)
    {
        LOG(Main, 2) << "type: " << voiceChannels[i]->getType() << endl;
        LOG(Main, 2) << "channel num: " << voiceChannels[i]->getChannelNum() << endl;
        LOG(Main, 2) << "max size: " << voiceChannels[i]->getMaxSize() << endl;
        LOG(Main, 2) << "max num of message: " << voiceChannels[i]->getMaxNumOfMessage() << endl;
        LOG(Main, 2) << "range: " << voiceChannels[i]->getRange() << endl;
    }
    for (int i = 0; i < (int) radioChannels.size(); i++)
    {
        LOG(Main, 2) << "type: " << radioChannels[i]->getType() << endl;
        LOG(Main, 2) << "channel num: " << radioChannels[i]->getChannelNum() << endl;
        LOG(Main, 2) << "bandwidth: " << radioChannels[i]->getBandWidth() << endl;
    }
}

Radar::~Radar()
{
}

void Radar::init()
{
    int selfIndex = 0;
    if (world->self->isPlatoon())
        selfIndex = ((Platoon*) (world->self))->platoonIndex;
    this->numOfSubscribeForCenters = atoi(world->config[Encodings::getConfigType(CT_CENTER_MAX_CHANNELS)].c_str());
    this->numOfSubscribeForPlatoons = atoi(world->config[Encodings::getConfigType(CT_PLATOON_MAX_CHANNELS)].c_str());
    //set channels
    int numOfChannels = atoi(world->config[Encodings::getConfigType(CT_NUM_OF_CHANNELS)].c_str());
    for (int i = 0; i < numOfChannels; i++)
    {
        std::stringstream num;
        num << i;
        string typeKey = Encodings::getConfigType(CT_COMMUNICATION_PREFIX) + num.str() + Encodings::getConfigType(CT_TYPE_SUFFIX);
        //        strcpy(typeKey, Encodings::getConfigType(CT_TYPE_SUFFIX));
        if (world->config[typeKey] == Encodings::getConfigType(CT_TYPE_RADAR))
        {
            LOG(Main, 1) << "type of channel " << i << " is radio" << endl;
            string bandwidthKey = Encodings::getConfigType(CT_COMMUNICATION_PREFIX) + num.str() + Encodings::getConfigType(CT_BANDWIDTH_SUFIX);
            int bandwidth = atoi(world->config[bandwidthKey].c_str());
            RadioChannel *newChannel = new RadioChannel("radio", i, bandwidth);
            this->radioChannels.push_back(newChannel);
        }
        else if (world->config[typeKey] == Encodings::getConfigType(CT_TYPE_VOICE))
        {
            LOG(Main, 1) << "type of channel " << i << " is voice" << endl;
            string maxSizeKey = Encodings::getConfigType(CT_COMMUNICATION_PREFIX) + num.str() + Encodings::getConfigType(CT_MESSAGE_SIZE_SUFFIX);
            int maxSize = atoi(world->config[maxSizeKey].c_str());
            string maxNumOfMessageKey = Encodings::getConfigType(CT_COMMUNICATION_PREFIX) + num.str() + Encodings::getConfigType(CT_MESSAGE_MAX_SUFFIX);
            int maxNumOfMessage = atoi(world->config[maxNumOfMessageKey].c_str());
            string rangeKey = Encodings::getConfigType(CT_COMMUNICATION_PREFIX) + num.str() + Encodings::getConfigType(CT_VOICE_RANGE);
            int range = atoi(world->config[rangeKey].c_str());
            VoiceChannel *newChannel = new VoiceChannel("Voice", i, maxSize, maxNumOfMessage, range);
            this->voiceChannels.push_back(newChannel);
        }
        else
            LOG(Main, 1) << "Unknown type of channel " << i << " :" << world->config[typeKey] << endl;
    }
    //sort radio channels as their bandwith
    sort(radioChannels.begin(), radioChannels.end(), channelComparator);
    //set Radar Mode
    radarMode = chooseRadarMode();
    switch (radarMode)
    {
    case RM_DISTRIBUTE:
    {
        LOG(Main, 1) << "Radar mode = Distributate" << endl;
        //set channels jobs & channels in use
        setChannelsJobs();
        int temp = world->ambulanceTeams.size() * MBOS_INDEX_OF_BUILDGINS_AND_ROADS + MBOS_HEADER + MBOS_POSITION + MBOS_INDEX_OF_CIVILIANS_IN_A_BUILDING * world->ambulanceTeams.size();
        if (temp % 8 == 0)
            temp /= 8;
        else
            temp = temp / 8 + 1;
        if (temp > channelsForCivilianInUse[0]->getBandWidth())
            maxSizeForCaptainOfAmbulances = 0;
        else
            maxSizeForCaptainOfAmbulances = temp;
        LOG(Main, 1) << "maxSizeForCaptainOfAmbulances: " << maxSizeForCaptainOfAmbulances << endl;
        //set bandwidth for each agent
        int counter = 1;
        while (checkValidateMaxByte(counter, channelsForBlockadeInUse))
            counter++;
        maxByteOnBlockadeChannelsForEachAgent = counter - 1;
        LOG(Main, 1) << "maxByteOnBlockadeChannelsForEachAgent: " << maxByteOnBlockadeChannelsForEachAgent << endl;

        counter = 1;
        while (checkValidateMaxByte(counter, channelsForCivilianInUse, maxSizeForCaptainOfAmbulances))
            counter++;
        maxByteOnCivilianChannelsForEachAgent = counter - 1;
        LOG(Main, 1) << "maxByteOnCivilianChannelsForEachAgent: " << maxByteOnCivilianChannelsForEachAgent << endl;

        counter = 1;
        while (checkValidateMaxByte(counter, channelsForFireInUse))
            counter++;
        maxByteOnFireChannelsForEachAgent = counter - 1;
        LOG(Main, 1) << "maxByteOnFireChannelsForEachAgent: " << maxByteOnFireChannelsForEachAgent << endl;

        //set send channel for each agent
        int bandwidth = 0;
        for (int i = 0; i < (int) channelsForBlockadeInUse.size(); i++)
        {
            bandwidth += channelsForBlockadeInUse[i]->getBandWidth();
            if (selfIndex * maxByteOnBlockadeChannelsForEachAgent < bandwidth)
            {
                myBlockadeChannel = channelsForBlockadeInUse[i]->getChannelNum();
                break;
            }
        }
        LOG(Main, 1) << "myBlockadeChannel: " << myBlockadeChannel << endl;

        bandwidth = 0;
        for (int i = 0; i < (int) channelsForCivilianInUse.size(); i++)
        {
            bandwidth += channelsForCivilianInUse[i]->getBandWidth();
            if (selfIndex * maxByteOnCivilianChannelsForEachAgent + maxSizeForCaptainOfAmbulances < bandwidth)
            {
                myCivilianChannel = channelsForCivilianInUse[i]->getChannelNum();
                break;
            }
        }
        LOG(Main, 1) << "myCivilianChannel " << myCivilianChannel << endl;

        bandwidth = 0;
        for (int i = 0; i < (int) channelsForFireInUse.size(); i++)
        {
            bandwidth += channelsForFireInUse[i]->getBandWidth();
            if (selfIndex * maxByteOnFireChannelsForEachAgent < bandwidth)
            {
                myFireChannel = channelsForFireInUse[i]->getChannelNum();
                break;
            }
        }
        LOG(Main, 1) << "myFireChannel: " << myFireChannel << endl;
        break;
    }
    case RM_ONE_CHANNEL:
    {
        LOG(Main, 1) << "Radar mode = one channel" << endl;
        vector<RadioChannel*> channelsInUse;
        for (int i = 0; i < numOfSubscribeForPlatoons && i < (int) radioChannels.size(); i++)
        {
            channelsInUse.push_back(radioChannels[i]);
            channelsForHearOfPlatoons.push_back(radioChannels[i]->getChannelNum());
        }
        LOG(Main, 1) << "channels in use.size: " << channelsInUse.size() << endl;
        LOG(Main, 1) << "channels for hear of platoons: ";
        for (int i = 0; i < channelsForHearOfPlatoons.size(); i++)
            LOG(Main, 1) << " " << channelsForHearOfPlatoons[i];
        LOG(Main, 1) << endl;

        int temp = world->ambulanceTeams.size() * MBOS_INDEX_OF_BUILDGINS_AND_ROADS + MBOS_HEADER + MBOS_POSITION + MBOS_INDEX_OF_CIVILIANS_IN_A_BUILDING * world->ambulanceTeams.size();
        if (temp % 8 == 0)
            temp /= 8;
        else
            temp = temp / 8 + 1;
        if (temp > channelsInUse[0]->getBandWidth())
            maxSizeForCaptainOfAmbulances = 0;
        else
            maxSizeForCaptainOfAmbulances = temp;
        LOG(Main, 1) << "maxSizeForCaptainOfAmbulances: " << maxSizeForCaptainOfAmbulances << endl;

        int counter = 1;
        while (checkValidateMaxByte(counter, channelsInUse, maxSizeForCaptainOfAmbulances))
            counter++;
        maxByteOnChannelsForEachAgent = counter - 1;
        LOG(Main, 1) << "maxByteOnChannelsForEachAgent: " << maxByteOnChannelsForEachAgent << endl;
        int bandwidth = 0;
        for (int i = 0; i < (int) channelsInUse.size(); i++)
        {
            bandwidth += channelsInUse[i]->getBandWidth();
            if (selfIndex * maxByteOnChannelsForEachAgent + maxSizeForCaptainOfAmbulances < bandwidth)
            {
                myChannel = channelsInUse[i]->getChannelNum();
                break;
            }
        }
        LOG(Main, 1) << "my channel: " << myChannel << endl;
        break;
    }
    case RM_CENTER:
    {
        LOG(Main, 1) << "Radar mode = center" << endl;
        vector<RadioChannel*> channelsInUse;
        for (int i = 0; i < numOfSubscribeForCenters + numOfSubscribeForPlatoons && i < radioChannels.size(); i++)
            channelsInUse.push_back(radioChannels[i]);
        LOG(Main, 1) << "channels in use.size: " << channelsInUse.size() << endl;
        for (int i = 0; i < channelsInUse.size() && i < numOfSubscribeForPlatoons; i++)
            channelsForHearOfPlatoons.push_back(channelsInUse[i]->getChannelNum());
        for (int i = numOfSubscribeForPlatoons; i < channelsInUse.size(); i++)
            channelsForHearOfCenters.push_back(channelsInUse[i]->getChannelNum());

        LOG(Main, 1) << "channels for hear of platoons: ";
        for (int i = 0; i < channelsForHearOfPlatoons.size(); i++)
            LOG(Main, 1) << " " << channelsForHearOfPlatoons[i];
        LOG(Main, 1) << endl;

        LOG(Main, 1) << "channels for hear of centers: ";
        for (int i = 0; i < channelsForHearOfCenters.size(); i++)
            LOG(Main, 1) << " " << channelsForHearOfCenters[i];
        LOG(Main, 1) << endl;

        int temp = world->ambulanceTeams.size() * MBOS_INDEX_OF_BUILDGINS_AND_ROADS + MBOS_HEADER + MBOS_POSITION + MBOS_INDEX_OF_CIVILIANS_IN_A_BUILDING * world->ambulanceTeams.size();
        if (temp % 8 == 0)
            temp /= 8;
        else
            temp = temp / 8 + 1;
        if (temp > channelsInUse[0]->getBandWidth())
            maxSizeForCaptainOfAmbulances = 0;
        else
            maxSizeForCaptainOfAmbulances = temp;
        LOG(Main, 1) << "maxSizeForCaptainOfAmbulances: " << maxSizeForCaptainOfAmbulances << endl;

        int counter = 1;
        while (checkValidateMaxByte(++counter, channelsInUse, maxSizeForCaptainOfAmbulances));
        maxByteOnChannelsForEachAgent = counter - 1;
        LOG(Main, 1) << "maxByteOnChannelsForEachAgent: " << maxByteOnChannelsForEachAgent << endl;
        int bandwidth = 0;
        for (int i = 0; i < (int) channelsInUse.size(); i++)
        {
            bandwidth += channelsInUse[i]->getBandWidth();
            if (selfIndex * maxByteOnChannelsForEachAgent + maxSizeForCaptainOfAmbulances < bandwidth)
            {
                myChannel = channelsInUse[i]->getChannelNum();
                break;
            }
        }
        LOG(Main, 1) << "my channel: " << myChannel << endl;
        int maxChannelsInUse = 0;
        bandwidth = 0;
        for (int i = 0; i < (int) channelsInUse.size(); i++)
        {
            bandwidth += channelsInUse[i]->getBandWidth();
            if ((world->platoons.size() - 1) * maxByteOnChannelsForEachAgent < bandwidth)
            {
                maxChannelsInUse = i + 1;
                break;
            }
        }
        LOG(Main, 1) << "maxChannelsInUse: " << maxChannelsInUse << endl;
        //TODO set maxByteForCenters
        int temp2 = (world->platoons.size() * 20);
        maxByteForCenters = min(radioChannels[0]->getBandWidth(), temp2);
        //            int howManyTimesOfMaxByte = world->platoons.size() / ;
        //            if(maxByteOnFireChannelsForEachAgent > NORMAL_NEEDED_BANDWITH)
        //                howManyTimesOfMaxByte = 4;

        counter = 1;
        while (checkValidateMaxByte(counter, channelsInUse, maxByteForCenters))
            counter++;
        maxByteForEachAgentWhenCentersSpeak = counter - 1;
        LOG(Main, 1) << "maxByteForEachAgentWhenCentersSpeak: " << maxByteForEachAgentWhenCentersSpeak << endl;
        bandwidth = -1 * maxByteForCenters;
        for (int i = 0; i < (int) channelsInUse.size(); i++)
        {
            bandwidth += channelsInUse[i]->getBandWidth();
            if (selfIndex * maxByteForEachAgentWhenCentersSpeak < bandwidth)
            {
                myChannelWhenCentersSpeak = channelsInUse[i]->getChannelNum();
                break;
            }
        }

        //set main center
        if (world->fireStations.size() > 0)
            mainCenter = world->fireStations[0];
        else if (world->policeOffices.size() > 0)
            mainCenter = world->policeOffices[0];
        else if (world->ambulanceCenters.size() > 0)
            mainCenter = world->ambulanceCenters[0];
        LOG(Main, 1) << "my channel when centers speak: " << myChannelWhenCentersSpeak << endl;
        break;
    }
    case RM_COMMUNICATIONLESS:
    {
        break;
    }
    }
}

RadarMode Radar::chooseRadarMode()
{
    LOG(Main, 1) << "starting choose radar mode" << endl;
    if ((int) radioChannels.size() < 1 || numOfSubscribeForPlatoons == 0 || radioChannels[0]->getBandWidth() == 0)
        return RM_COMMUNICATIONLESS;
    if (radioChannels.size() < 3 || numOfSubscribeForPlatoons == 1)
        return RM_ONE_CHANNEL;
    else
        return RM_DISTRIBUTE;
//    if (numOfSubscribeForPlatoons == 1)
//    {
//        for (int i = 0; i < numOfSubscribeForPlatoons && i < radioChannels.size(); i++)
//            bandwidthInOneChannelMode = radioChannels[i]->getBandWidth() / world->platoons.size();
//    }
//    LOG(Main, 1) << "guess bandwidth on one channel mode " << bandwidthInOneChannelMode << endl;
//    int bandwidthInCenterMode = 0;
//    if (radioChannels.size() > numOfSubscribeForPlatoons && (world->ambulanceCenters.size() > 0 || world->fireStations.size() > 0 || world->policeOffices.size() > 0))
//    {
//        vector<RadioChannel*> channelsInUse;
//        for (int i = 0; i < numOfSubscribeForCenters + numOfSubscribeForPlatoons && i < (int) radioChannels.size(); i++)
//            channelsInUse.push_back(radioChannels[i]);
//        int counter = 1;
//        while (checkValidateMaxByte(++counter, channelsInUse));
//        counter--;
//        LOG(Main, 1) << "guessed bytes for each platoons in center mode: " << counter << endl;
//        bandwidthInCenterMode = counter;
//    }
//    if (numOfSubscribeForPlatoons == 1)
//    {
//        if (bandwidthInCenterMode - bandwidthInOneChannelMode > MIN_DIFFRECE_BANDWITH_BETWEEN_CENTER_DITRIBUTED_MODE)
//            return RM_CENTER;
//        else
//            return RM_ONE_CHANNEL;
//    }
//    //    return RM_DISTRIBUTE;
//    int bandwidthInDistributedMode = 0;
//    // this variable indicade the channels in which data will be sent
//    int inUseOfDisturbutedMode = (int) ((numOfSubscribeForPlatoons - 1) / 2) + 1;
//    inUseOfDisturbutedMode *= 3;
//    for (int i = 0; i < inUseOfDisturbutedMode && i < (int) radioChannels.size(); i++)
//        bandwidthInDistributedMode += radioChannels[i]->getBandWidth() / world->platoons.size();
//    LOG(Main, 1) << "guessed bytes for each platoons in distributed mode: " << bandwidthInDistributedMode << endl;

    //choose between distributed & one channel
//    if (bandwidthInCenterMode - bandwidthInDistributedMode > MIN_DIFFRECE_BANDWITH_BETWEEN_CENTER_DITRIBUTED_MODE)
//        return RM_CENTER;
//    return RM_DISTRIBUTE;

}

void Radar::setChannelsJobs()
{
    //	sort(voiceChannels.begin(), voiceChannels.end(), channelCompare);

    int counterOfCivilian = 0;
    int counterOfFire = 0;
    int counterOfBlockade = 0;
    for (int i = 0; i < (int) radioChannels.size(); i++)
    {

        if (i % 3 == 0)
        {
            radioChannels[i]->setChannelJob(CJ_CIVILIAN);
            counterOfCivilian++;
        }
        else if (i % 3 == 1)
        {
            radioChannels[i]->setChannelJob(CJ_FIRE);
            counterOfFire++;
        }
        else
        {
            radioChannels[i]->setChannelJob(CJ_BLOCKADE);
            counterOfBlockade++;
        }
        LOG(Main, 1) << "radio channel number " << i << " is " << radioChannels[i]->getChannelNum() << endl;
    }
    //set max channel (for each job) each agent can subscribe
    int maxHear = 0;
    if (numOfSubscribeForPlatoons % 2 == 0)
        maxHear = numOfSubscribeForPlatoons / 2;
    else
        maxHear = numOfSubscribeForPlatoons / 2 + 1;

    //set numbre of the channels in use
    int numOfChannelForBlockadeInUse = min(maxHear, counterOfBlockade);
    int numOfChannelForCivilianInUse = min(maxHear, counterOfCivilian);
    int numOfChannelForFireInUse = min(maxHear, counterOfFire);

    for (int i = 0; i < (int) radioChannels.size(); i++)
    {
        if (radioChannels[i]->getChannelJob() == CJ_CIVILIAN && numOfChannelForCivilianInUse != 0)
        {
            channelsForCivilianInUse.push_back(radioChannels[i]);
            numOfChannelForCivilianInUse--;
        }
        if (radioChannels[i]->getChannelJob() == CJ_BLOCKADE && numOfChannelForBlockadeInUse != 0)
        {
            channelsForBlockadeInUse.push_back(radioChannels[i]);
            numOfChannelForBlockadeInUse--;
        }
        if (radioChannels[i]->getChannelJob() == CJ_FIRE && numOfChannelForFireInUse != 0)
        {
            channelsForFireInUse.push_back(radioChannels[i]);
            numOfChannelForFireInUse--;
        }
    }

    //  ********************LOG***********************
    LOG(Main, 1) << "blockade channels: ";
    for (int i = 0; i < (int) channelsForBlockadeInUse.size(); i++)
        LOG(Main, 1) << channelsForBlockadeInUse[i]->getChannelNum() << " ";
    LOG(Main, 1) << endl;

    LOG(Main, 1) << "civilian channels: ";
    for (int i = 0; i < (int) channelsForCivilianInUse.size(); i++)
        LOG(Main, 1) << channelsForCivilianInUse[i]->getChannelNum() << " ";
    LOG(Main, 1) << endl;

    LOG(Main, 1) << "fire channels: ";
    for (int i = 0; i < (int) channelsForFireInUse.size(); i++)
        LOG(Main, 1) << channelsForFireInUse[i]->getChannelNum() << " ";
    LOG(Main, 1) << endl;
    //  ********************LOG***********************

    //set channels that each agent should subscribe

    for (int i = 1; i <= numOfSubscribeForPlatoons; i++)
    {
        if (i % 2 == 1)
        {
            if ((int) channelsForFireInUse.size() > i / 2)
                channelsForFireBrigades.push_back(channelsForFireInUse[i / 2]->getChannelNum());
            if ((int) channelsForCivilianInUse.size() > i / 2)
                channelsForAmbulanceTeams.push_back(channelsForCivilianInUse[i / 2]->getChannelNum());
            if ((int) channelsForBlockadeInUse.size() > i / 2)
                channelsForPoliceForces.push_back(channelsForBlockadeInUse[i / 2]->getChannelNum());
        }
        else
        {
            if ((int) channelsForBlockadeInUse.size() > i / 2 - 1)
            {
                channelsForFireBrigades.push_back(channelsForBlockadeInUse[i / 2 - 1]->getChannelNum());
                channelsForAmbulanceTeams.push_back(channelsForBlockadeInUse[i / 2 - 1]->getChannelNum());
            }
            if ((int) channelsForCivilianInUse.size() > i / 2 - 1)
                channelsForPoliceForces.push_back(channelsForCivilianInUse[i / 2 - 1]->getChannelNum());
        }
    }

    //  ********************LOG***********************
    LOG(Main, 1) << "channelsForAmbulanceTeams: ";
    for (int i = 0; i < (int) channelsForAmbulanceTeams.size(); i++)
        LOG(Main, 1) << channelsForAmbulanceTeams[i] << " ";
    LOG(Main, 1) << endl;

    LOG(Main, 1) << "channelsForFireBrigades: ";
    for (int i = 0; i < (int) channelsForFireBrigades.size(); i++)
        LOG(Main, 1) << channelsForFireBrigades[i] << " ";
    LOG(Main, 1) << endl;

    LOG(Main, 1) << "channelsForPoliceForces: ";
    for (int i = 0; i < (int) channelsForPoliceForces.size(); i++)
        LOG(Main, 1) << channelsForPoliceForces[i] << " ";
    LOG(Main, 1) << endl;
    //  ********************LOG***********************
}

bool lastCycleSentInRadarComparator(RCRObject* o1, RCRObject* o2)
{
    return o1->getLastCycleSentInRadar() > o2->getLastCycleSentInRadar();
}

void Radar::setFireData(BoolStream& msg, int &mySize)
{

    vector<Building*> buildingsForSend;
    for (unsigned int i = 0; i < world->buildings.size(); i++)
    {
        if (world->self->isHuman())
        {
            if ((world->buildings[i]->getLastCycleUpdatedBySense() > world->buildings[i]->getLastCycleSentInRadar() && !(world->buildings[i]->getLastCycleUpdatedBySense() < world->buildings[i]->getLastCycleUpdated()) && (world->buildings[i]->getTemperature() > 40 || world->buildings[i]->getFieryness() == 8 || world->buildings[i]->wasBurning)) || world->buildings[i]->getLastCycleSentInRadar() == world->getTime())
            {
                LOG(Main, 7) << "buildings for send added " << world->buildings[i]->getId() << endl;
                buildingsForSend.push_back(world->buildings[i]);
            }
        }
        else
        {
            if (world->buildings[i]->getLastCycleUpdated() > world->getTime() - CENTERS_CYCLE_FOR_SPEAK && (world->buildings[i]->getTemperature() > 40 || world->buildings[i]->getFieryness() == 8))
                buildingsForSend.push_back(world->buildings[i]);

        }
    }
    sort(buildingsForSend.begin(), buildingsForSend.end(), lastCycleSentInRadarComparator);

    LOG(Main, 1) << "buildings for send num: " << buildingsForSend.size() << endl;
    for (int i = 0; i < buildingsForSend.size(); i++)
        LOG(Main, 1) << "biulding with id " << buildingsForSend[i]->getId() << endl;
    for (unsigned int i = 0; i < buildingsForSend.size(); i++)
    {
        if (mySize < MBOS_HEADER + MBOS_INDEX_OF_BUILDGINS_AND_ROADS + MBOS_TEMPERTURE + MBOS_FIERYNESS)
            break;
        //        Encodings::writeByte(msg, MP_BUILDING);
        msg.push_back(bits(MP_BUILDING, MBOS_HEADER));
        msg.push_back(bits(buildingsForSend[i]->buildingIndex, MBOS_INDEX_OF_BUILDGINS_AND_ROADS));
        msg.push_back(bits(buildingsForSend[i]->getTemperature(), MBOS_TEMPERTURE));
        //0 = building isn't burning but have high temperture, 1 = building is burning, 2 = building is completely burnt
        if (buildingsForSend[i]->isBurning())
            msg.push_back(bits(1, MBOS_FIERYNESS));
        else if (buildingsForSend[i]->getFieryness() == 8)
            msg.push_back(bits(2, MBOS_FIERYNESS));
        else
            msg.push_back(bits(0, MBOS_FIERYNESS));


        mySize -= MBOS_HEADER + MBOS_INDEX_OF_BUILDGINS_AND_ROADS + MBOS_TEMPERTURE + MBOS_FIERYNESS;
        buildingsForSend[i]->setLastCycleSentInRadar(world->getTime());
    }
}

//void Radar::setBlockadeChannelData(vector<byte>& msg, int &mySize)
//{
//	if(mySize >= 4)
//		Encodings::writeUnsignedInt(msg, ((Human*)world->self)->getPosition());
//	mySize -= 4;
//}

void Radar::setCivilianData(BoolStream& msg, int &mySize)
{
    vector<Civilian*> civiliansForSend;
    for (unsigned int i = 0; i < world->civilians.size(); i++)
    {
        if (world->self->isHuman())
        {
            if ((world->civilians[i]->getLastCycleUpdatedBySense() > world->civilians[i]->getLastCycleSentInRadar() && !(world->civilians[i]->getLastCycleUpdatedBySense() < world->civilians[i]->getLastCycleUpdated())) || world->civilians[i]->getLastCycleSentInRadar() == world->getTime())
                civiliansForSend.push_back(world->civilians[i]);
        }
        else
        {
            if (world->civilians[i]->getLastCycleUpdated() > world->getTime() - CENTERS_CYCLE_FOR_SPEAK)
                civiliansForSend.push_back(world->civilians[i]);
        }
    }
    sort(civiliansForSend.begin(), civiliansForSend.end(), lastCycleSentInRadarComparator);
    LOG(Main, 1) << "civilian for send num: " << civiliansForSend.size() << endl;
    for (int i = 0; i < civiliansForSend.size(); i++)
        LOG(Main, 1) << "civilian with id: " << civiliansForSend[i]->getId() << endl;
    for (unsigned int i = 0; i < civiliansForSend.size(); i++)
    {
        Civilian* civ = civiliansForSend[i];
        if (civ->getHp() <= 1 || civ->getMotionlessObject()->isRefuge() && civ->isAvailable)
        {
            if (mySize < MBOS_HEADER + MBOS_ID)
                break;
            LOG(Main, 1) << "civilan fert " << civ->getId() << endl;
            msg.push_back(bits(MP_CIVILIAN_FERT, MBOS_HEADER));
            msg.push_back(bits(civ->getId(), MBOS_ID));
            mySize -= MBOS_HEADER + MBOS_ID;
            civiliansForSend[i]->setLastCycleSentInRadar(world->getTime());
        }
        else
        {
            if (mySize < MBOS_HEADER + MBOS_ID + MBOS_HP + MBOS_DAMAGE + MBOS_BURIEDNESS + MBOS_POSITION)
                continue;
            LOG(Main, 1) << "civilian normal " << civ->getId() << endl;
            msg.push_back(bits(MP_CIVILIAN_NORMAL, MBOS_HEADER));
            msg.push_back(bits(civ->getId(), MBOS_ID));
            msg.push_back(bits(civ->getHp(), MBOS_HP));
            msg.push_back(bits(civ->getDamage(), MBOS_DAMAGE));
            msg.push_back(bits(civ->getBuriedness(), MBOS_BURIEDNESS));
            if (civ->getRepresentiveNodeIndex() == -1)
            {
                msg.push_back(bits(civ->getWithoutBlockadeRepresentiveNodeIndex(), MBOS_POSITION - 2));
                msg.push_back(bits(2 + (worldGraph->getNodes()[civ->getWithoutBlockadeRepresentiveNodeIndex()]->getFirstMotionlessIndex() == civ->getMotionlessObject()->motionlessIndex), 2));
            }
            else
            {
                msg.push_back(bits(civ->getRepresentiveNodeIndex(), MBOS_POSITION - 2));
                msg.push_back(bits(worldGraph->getNodes()[civ->getRepresentiveNodeIndex()]->getFirstMotionlessIndex() == civ->getMotionlessObject()->motionlessIndex, 2));
            }
            mySize -= MBOS_HEADER + MBOS_ID + MBOS_HP + MBOS_DAMAGE + MBOS_BURIEDNESS + MBOS_POSITION;
            civiliansForSend[i]->setLastCycleSentInRadar(world->getTime());
        }
    }
}

void Radar::setBlockadeData(BoolStream& msg, int& mySize)
{
    for (int i = 0; i < (int) world->roads.size(); i++)
    {
        if (world->roads[i]->getHasToBeSent() == world->getTime() || (world->getTime() == 4 && world->roads[i]->getHasToBeSent() == 1))
        {
            int bSize = world->roads[i]->getRelativeEdgesIndexes().size();
            //			LOG(Main, 1) << "road: " << world->roads[i]->getId() << ' ' << bSize << endl;
            if (mySize >= MBOS_INDEX_OF_BUILDGINS_AND_ROADS + MBOS_HEADER + bSize)
            {
                msg.push_back(bits(MP_ROAD_BLOCKADE_DATA, MBOS_HEADER));
                msg.push_back(bits(world->roads[i]->roadIndex, MBOS_INDEX_OF_BUILDGINS_AND_ROADS));
                LOG(Main, 1) << "information of road " << world->roads[i]->getId() << " sent." << endl;
                //				LOG(Main, 1) << "number of relative edges: " << world->roads[i]->getRelativeEdgesIndexes().size() << endl;
                for (int j = 0; j < world->roads[i]->getRelativeEdgesIndexes().size(); j++)
                {
                    //					LOG(Main, 1) << (worldGraph->getEdges()[world->roads[i]->getRelativeEdgesIndexes()[j]]->getPassingMode() == PM_PASSABLE);
                    msg.push_back(worldGraph->getEdges()[world->roads[i]->getRelativeEdgesIndexes()[j]]->getPassingMode() == PM_PASSABLE);
                }
                //				LOG(Main, 1) << endl;
                mySize -= MBOS_INDEX_OF_BUILDGINS_AND_ROADS + MBOS_HEADER + bSize;
            }
        }
    }
}

void Radar::setPersonalInformation(BoolStream& msg, int& mySize)
{
    if (world->self->isPlatoon())
    {
        if (mySize >= MBOS_POSITION)
        {
            if (((Platoon*) world->self)->getRepresentiveNodeIndex() == -1)
            {
                msg.push_back(bits(((Platoon*) world->self)->getWithoutBlockadeRepresentiveNodeIndex(), MBOS_POSITION - 2));
                msg.push_back(bits(2 + (worldGraph->getNodes()[((Platoon*) world->self)->getWithoutBlockadeRepresentiveNodeIndex()]->getFirstMotionlessIndex() == ((Platoon*) world->self)->getMotionlessObject()->motionlessIndex), 2));
            }
            else
            {
                msg.push_back(bits(((Platoon*) world->self)->getRepresentiveNodeIndex(), MBOS_POSITION - 2));
                msg.push_back(bits(worldGraph->getNodes()[((Platoon*) world->self)->getRepresentiveNodeIndex()]->getFirstMotionlessIndex() == ((Platoon*) world->self)->getMotionlessObject()->motionlessIndex, 2));
            }

            //            msg.push_back(bits(((Platoon*) world->self)->getMotionlessObject()->motionlessIndex, MBOS_POSITION));
            mySize -= MBOS_POSITION;
        }
        if (world->self->isFireBrigade())
        {
            if (mySize >= MBOS_WATER_QUANTITY)
            {
                if (((FireBrigade*) world->self)->getWaterQuantity() > 0)
                    msg.push_back(bits(1, MBOS_WATER_QUANTITY));
                else
                    msg.push_back(bits(0, MBOS_WATER_QUANTITY));

                mySize -= MBOS_WATER_QUANTITY;
            }
        }
    }
    else
    {
        vector<Platoon*> platoonsForSend;
        for (int i = 0; i < world->platoons.size(); i++)
        {
            LOG(Main, 1) << "last Cycle updated: " << world->platoons[i]->getLastCycleUpdated() << endl;
            if (world->platoons[i]->getLastCycleUpdated() > world->getTime() - CENTERS_CYCLE_FOR_SPEAK)
                platoonsForSend.push_back(world->platoons[i]);
        }
        if (mySize >= MBOS_SIZE)
        {
            msg.push_back(bits(platoonsForSend.size(), MBOS_SIZE));
            mySize -= MBOS_SIZE;
        }
        for (int i = 0; i < platoonsForSend.size(); i++)
        {
            if (mySize >= MBOS_POSITION + MBOS_INDEX_OF_PLATOONS)
            {
                msg.push_back(bits(platoonsForSend[i]->platoonIndex, MBOS_INDEX_OF_PLATOONS));

                if (platoonsForSend[i]->getRepresentiveNodeIndex() == -1)
                {
                    msg.push_back(bits(platoonsForSend[i]->getWithoutBlockadeRepresentiveNodeIndex(), MBOS_POSITION - 2));
                    msg.push_back(bits(2 + (worldGraph->getNodes()[platoonsForSend[i]->getWithoutBlockadeRepresentiveNodeIndex()]->getFirstMotionlessIndex() == platoonsForSend[i]->getMotionlessObject()->motionlessIndex), 2));
                }
                else
                {
                    msg.push_back(bits(platoonsForSend[i]->getRepresentiveNodeIndex(), MBOS_POSITION - 2));
                    msg.push_back(bits(worldGraph->getNodes()[platoonsForSend[i]->getRepresentiveNodeIndex()]->getFirstMotionlessIndex() == platoonsForSend[i]->getMotionlessObject()->motionlessIndex, 2));
                }
                mySize -= MBOS_POSITION + MBOS_INDEX_OF_PLATOONS;
            }
        }

    }
}

void Radar::setAgentFirstTime(BoolStream& msg, int& mySize)
{
    if (world->self->isPlatoon())
    {
        if (mySize >= MBOS_HEADER + MBOS_BURIEDNESS + MBOS_INDEX_OF_PLATOONS)
        {
            LOG(Main, 1) << "platoon buriedness for send is " << ((Platoon*) world->self)->getBuriedness() << endl;
            msg.push_back(bits(MP_AGENT_FIRST_TIME, MBOS_HEADER));
            msg.push_back(bits(((Platoon*) world->self)->platoonIndex, MBOS_INDEX_OF_PLATOONS));
            msg.push_back(bits(((Platoon*) world->self)->getBuriedness(), MBOS_BURIEDNESS));
            mySize -= MBOS_HEADER + MBOS_BURIEDNESS + MBOS_INDEX_OF_PLATOONS;
        }
    }
    else
    {
        vector<Platoon*> platoonsForSend;
        for (int i = 0; i < world->platoons.size(); i++)
        {
            if (world->platoons[i]->getLastCycleUpdated() > world->getTime() - CENTERS_CYCLE_FOR_SPEAK)
                platoonsForSend.push_back(world->platoons[i]);
        }
        for (int i = 0; i < platoonsForSend.size(); i++)
        {
            if (mySize >= MBOS_BURIEDNESS + MBOS_INDEX_OF_PLATOONS + MBOS_HEADER)
            {
                LOG(Main, 1) << "Agent first time " << platoonsForSend[i]->getId() << endl;
                msg.push_back(bits(MP_AGENT_FIRST_TIME, MBOS_HEADER));
                msg.push_back(bits(platoonsForSend[i]->platoonIndex, MBOS_INDEX_OF_PLATOONS));
                msg.push_back(bits(platoonsForSend[i]->getBuriedness(), MBOS_BURIEDNESS));
                mySize -= MBOS_BURIEDNESS + MBOS_INDEX_OF_PLATOONS;
            }

        }
    }
}

void Radar::setAmbulancesOrders(BoolStream& msg, int& mySize)
{
    if (mySize >= MBOS_HEADER + MBOS_INDEX_OF_BUILDGINS_AND_ROADS * world->ambulanceTeams.size() + MBOS_INDEX_OF_CIVILIANS_IN_A_BUILDING * world->ambulanceTeams.size())
    {
        msg.push_back(bits(MP_AMBULANCE_ORDER, MBOS_HEADER));
        for (int i = 0; i < world->ambulanceTeams.size(); i++)
        {
            if (world->ambulanceTeams[i]->humanTarget != NULL)
            {
                if (world->ambulanceTeams[i]->humanTarget->getMotionlessObject()->isBuilding())
                {
                    msg.push_back(bits(((Building*) world->ambulanceTeams[i]->humanTarget->getMotionlessObject())->buildingIndex, MBOS_INDEX_OF_BUILDGINS_AND_ROADS));
                    LOG(Main, 1) << "building target in radar: " <<  ((Building*) world->ambulanceTeams[i]->humanTarget->getMotionlessObject())->getId() << endl;
                    int targetIndex = world->ambulanceTeams[i]->targetIndex;
                    if(targetIndex > pow(2, (double)MBOS_INDEX_OF_CIVILIANS_IN_A_BUILDING) - 1)
                        targetIndex = pow(2, (double)MBOS_INDEX_OF_CIVILIANS_IN_A_BUILDING) - 1;
                    LOG(Main, 1) << "target index in radar: " << targetIndex << endl;
                    msg.push_back(bits(targetIndex, MBOS_INDEX_OF_CIVILIANS_IN_A_BUILDING));
                }
                else
                    msg.push_back(bits(world->buildings.size(), MBOS_INDEX_OF_BUILDGINS_AND_ROADS));
            }
            else
            {
                msg.push_back(bits(world->buildings.size(), MBOS_INDEX_OF_BUILDGINS_AND_ROADS));
            }
        }
    }
}

void Radar::shareSenseWorldAndSendOrders()
{
    LOG(Main, 1) << "start shareSense" << endl;
    BoolStream msg;
    switch (radarMode)
    {
    case RM_DISTRIBUTE:
    {
        if (world->self->isHuman())
        {
            int mySize = maxByteOnCivilianChannelsForEachAgent * 8;
            int prevSize = maxByteOnCivilianChannelsForEachAgent;
            for (int i = 0; i < 3 && mySize > 0; i++)
            {
                msg.clear();
                msg.setSize(maxByteOnCivilianChannelsForEachAgent);
                setPersonalInformation(msg, mySize);
                if (world->getTime() < MAX_TIME_FOR_SEND_AGENT_FIRST_TIME)
                    setAgentFirstTime(msg, mySize);
                setCivilianData(msg, mySize);
                if (mySize % 8 == 0)
                {
                    msg.setSize(prevSize - mySize / 8 + 1);
                    prevSize = mySize / 8 + 1;
                }
                else
                {
                    msg.setSize(prevSize - (int) (mySize) / 8);
                    prevSize = mySize / 8;
                }
                if (mySize % 8 != 0)
                    mySize -= (mySize % 8);
                LOG(Main, 1) << "msg for send " << msg.getVal(" ") << endl;
                command->speak(myCivilianChannel, msg.getValue());
            }
            //captain orders:
            if (world->self->isAmbulanceTeam() && maxSizeForCaptainOfAmbulances > 0)
            {
                //recognize captain
                AmbulanceTeam* captain = NULL;
                for (int i = 0; i < world->ambulanceTeams.size(); i++)
                    if (world->ambulanceTeams[i]->isAvailable)
                    {
                        captain = world->ambulanceTeams[i];
                        break;
                    }
                //if I am captain
                if (captain != NULL && ((AmbulanceTeam*) world->self)->ambulanceTeamIndex == captain->ambulanceTeamIndex)
                {
                    LOG(Main, 2) << "Captain Orders" << endl;
                    int captainSize = maxSizeForCaptainOfAmbulances * 8;
                    LOG(Main, 2) << "Captain size: " << captainSize << endl;
                    msg.clear();
                    msg.setSize(maxSizeForCaptainOfAmbulances);
                    setPersonalInformation(msg, captainSize);
                    setAmbulancesOrders(msg, captainSize);
                    LOG(Main, 2) << "msg for send orders: " << msg.getVal(" ") << endl;
                    command->speak(channelsForCivilianInUse[0]->getChannelNum(), msg.getValue());
                    for (int i = 0; i < 3 && mySize >= maxSizeForCaptainOfAmbulances * 8; i++)
                    {
                        LOG(Main, 2) << "Send order another time: " << i << "on channels: " << myCivilianChannel << endl;
                        command->speak(myCivilianChannel, msg.getValue());
                        mySize -= maxSizeForCaptainOfAmbulances * 8;
                    }
                }
            }

            mySize = maxByteOnFireChannelsForEachAgent * 8;
            prevSize = maxByteOnFireChannelsForEachAgent;
            for (int i = 0; i < 3 && mySize > 0; i++)
            {
                msg.clear();
                msg.setSize(maxByteOnFireChannelsForEachAgent);
                setPersonalInformation(msg, mySize);
                if (world->getTime() < MAX_TIME_FOR_SEND_AGENT_FIRST_TIME)
                    setAgentFirstTime(msg, mySize);
                setFireData(msg, mySize);
                if (mySize % 8 == 0)
                {
                    msg.setSize(prevSize - mySize / 8 + 1);
                    prevSize = mySize / 8 + 1;
                }
                else
                {
                    msg.setSize(prevSize - (int) (mySize) / 8);
                    prevSize = mySize / 8;
                }
                if (mySize % 8 != 0)
                    mySize -= (mySize % 8);
                LOG(Main, 1) << "msg for send " << msg.getVal(" ") << endl;
                command->speak(myFireChannel, msg.getValue());
            }
            //            if(false)
            //            {
            mySize = maxByteOnBlockadeChannelsForEachAgent * 8;
            prevSize = maxByteOnBlockadeChannelsForEachAgent;
            for (int i = 0; i < 3 && mySize > 0; i++)
            {
                msg.clear();
                msg.setSize(maxByteOnBlockadeChannelsForEachAgent);
                setPersonalInformation(msg, mySize);
                if (world->getTime() < MAX_TIME_FOR_SEND_AGENT_FIRST_TIME)
                    setAgentFirstTime(msg, mySize);
                setBlockadeData(msg, mySize);
                if (mySize % 8 == 0)
                {
                    msg.setSize(prevSize - mySize / 8 + 1);
                    prevSize = mySize / 8 + 1;
                }
                else
                {
                    msg.setSize(prevSize - (int) (mySize) / 8);
                    prevSize = mySize / 8;
                }
                if (mySize % 8 != 0)
                    mySize -= (mySize % 8);
                LOG(Main, 1) << "msg for send " << msg.getVal(" ") << endl;
                command->speak(myBlockadeChannel, msg.getValue());
            }
            //            }
        }
        break;
    }
    case RM_ONE_CHANNEL:
    {
        if (world->self->isHuman())
        {
            int mySize = maxByteOnChannelsForEachAgent * 8;
            LOG(Main, 6) << "my origin size: " << mySize << endl;
            int prevSize = maxByteOnChannelsForEachAgent;
            LOG(Main, 6) << "original prev size: " << prevSize << " " << maxByteOnChannelsForEachAgent << endl;
            for (int i = 0; i < 3 && mySize > 0; i++)
            {
                LOG(Main, 1) << "mySize: " << mySize << endl;

                msg.clear();
                msg.setSize(maxByteOnChannelsForEachAgent);
                setPersonalInformation(msg, mySize);
                LOG(Main, 6) << "size: " << mySize << endl;
                if (world->getTime() < MAX_TIME_FOR_SEND_AGENT_FIRST_TIME)
                    setAgentFirstTime(msg, mySize);
                LOG(Main, 6) << "size: " << mySize << endl;
                setCivilianData(msg, mySize);
                LOG(Main, 6) << "after set civilian data" << endl;
                LOG(Main, 6) << "size: " << mySize << endl;

                setFireData(msg, mySize);
                LOG(Main, 6) << "after set fire data" << endl;
                setBlockadeData(msg, mySize);
                LOG(Main, 1) << "msg for send " << msg.getVal(" ") << endl;
                //                    if (i == 0)
                //                    {
                LOG(Main, 6) << "size: " << mySize << endl;
                LOG(Main, 6) << "prev size: " << prevSize << endl;
                if (mySize % 8 == 0)
                {
                    //                            msg.setSize(maxByteOnChannelsForEachAgent - mySize / 8 + 1);
                    msg.setSize(prevSize - mySize / 8 + 1);
                    prevSize = mySize / 8 + 1;
                }
                else
                {
                    //                            msg.setSize(maxByteOnChannelsForEachAgent - (int) (mySize / 8));
                    msg.setSize(prevSize - (int) (mySize / 8));
                    prevSize = mySize / 8;
                }
                //                    } else
                //                    {
                //                        msg.setSize(prevSize);
                //                    }
                if (mySize % 8 != 0)
                    mySize -= (mySize % 8);
                command->speak(myChannel, msg.getValue());
            }
            //captain orders:
            if (world->self->isAmbulanceTeam() && maxSizeForCaptainOfAmbulances > 0)
            {
                //recognize captain
                AmbulanceTeam* captain = NULL;
                for (int i = 0; i < world->ambulanceTeams.size(); i++)
                    if (world->ambulanceTeams[i]->isAvailable)
                    {
                        captain = world->ambulanceTeams[i];
                        break;
                    }
                //if I am captain
                if (captain != NULL && ((AmbulanceTeam*) world->self)->ambulanceTeamIndex == captain->ambulanceTeamIndex)
                {
                    LOG(Main, 2) << "Captain Orders" << endl;
                    int captainSize = maxSizeForCaptainOfAmbulances * 8;
                    LOG(Main, 2) << "Captain size: " << captainSize << endl;
                    msg.clear();
                    msg.setSize(maxSizeForCaptainOfAmbulances);
                    setPersonalInformation(msg, captainSize);
                    setAmbulancesOrders(msg, captainSize);
                    LOG(Main, 2) << "msg for send orders: " << msg.getVal(" ") << endl;
                    command->speak(radioChannels[0]->getChannelNum(), msg.getValue());
                    for (int i = 0; i < 3 && mySize >= maxSizeForCaptainOfAmbulances * 8; i++)
                    {
                        LOG(Main, 2) << "Send order another time: " << i << "on channels: " << radioChannels[0]->getChannelNum() << endl;
                        command->speak(radioChannels[0]->getChannelNum(), msg.getValue());
                        mySize -= maxSizeForCaptainOfAmbulances * 8;
                    }
                }
            }
        }
        break;
    }
    case RM_CENTER:
    {
        if (world->getTime() % CENTERS_CYCLE_FOR_SPEAK == 0)
        {
            if (world->self->isHuman())
            {
                int mySize = maxByteForEachAgentWhenCentersSpeak * 8;
                int prevSize = maxByteForEachAgentWhenCentersSpeak;
                for (int i = 0; i < 3 && mySize > 0; i++)
                {
                    msg.clear();
                    msg.setSize(maxByteForEachAgentWhenCentersSpeak);
                    setPersonalInformation(msg, mySize);
                    if (world->getTime() < MAX_TIME_FOR_SEND_AGENT_FIRST_TIME)
                        setAgentFirstTime(msg, mySize);
                    setFireData(msg, mySize);
                    //                    command->speak(myChannelWhenCentersSpeak, msg.getValue());

                    setCivilianData(msg, mySize);
                    setBlockadeData(msg, mySize);
                    if (mySize % 8 == 0)
                    {
                        msg.setSize(prevSize - mySize / 8 + 1);
                        prevSize = mySize / 8 + 1;
                    }
                    else
                    {
                        msg.setSize(prevSize - (int) (mySize) / 8);
                        prevSize = mySize / 8;
                    }

                    if (mySize % 8 != 0)
                        mySize -= (mySize % 8);
                    LOG(Main, 1) << "msg for send " << msg.getVal(" ") << " on channel: " << myChannelWhenCentersSpeak << endl;
                    command->speak(myChannelWhenCentersSpeak, msg.getValue());
                }
            }
            else if (world->self->getId() == mainCenter->getId())
            {
                //TODO set & send the datas which centers must send
                int mySize = maxByteForCenters * 8;
                int prevSize = maxByteForCenters;
                for (int i = 0; i < 3 && mySize > 0; i++)
                {
                    msg.clear();
                    msg.setSize(maxByteForCenters);
                    setPersonalInformation(msg, mySize);
                    if (world->getTime() < MAX_TIME_FOR_SEND_AGENT_FIRST_TIME)
                        setAgentFirstTime(msg, mySize);
                    setFireData(msg, mySize);
                    //                    command->speak(myChannelWhenCentersSpeak, msg.getValue());

                    setCivilianData(msg, mySize);
                    setBlockadeData(msg, mySize);
                    if (mySize % 8 == 0)
                    {
                        msg.setSize(prevSize - mySize / 8 + 1);
                        prevSize = mySize / 8 + 1;
                    }
                    else
                    {
                        msg.setSize(prevSize - (int) (mySize) / 8);
                        prevSize = mySize / 8;
                    }
                    if (mySize % 8 != 0)
                        mySize -= (mySize % 8);
                    LOG(Main, 1) << "msg for send " << msg.getVal(" ") << endl;
                    command->speak(radioChannels[0]->getChannelNum(), msg.getValue());
                }
            }
        }
        else
        {
            if (world->self->isHuman())
            {
                int mySize = maxByteOnChannelsForEachAgent * 8;
                int prevSize = maxByteOnChannelsForEachAgent;
                for (int i = 0; i < 3 && mySize > 0; i++)
                {
                    msg.clear();
                    msg.setSize(maxByteOnChannelsForEachAgent);
                    setPersonalInformation(msg, mySize);
                    if (world->getTime() < MAX_TIME_FOR_SEND_AGENT_FIRST_TIME)
                        setAgentFirstTime(msg, mySize);
                    setCivilianData(msg, mySize);

                    setFireData(msg, mySize);
                    setBlockadeData(msg, mySize);
                    if (mySize % 8 == 0)
                    {
                        msg.setSize(prevSize - mySize / 8 + 1);
                        prevSize = mySize / 8 + 1;
                    }
                    else
                    {
                        msg.setSize(prevSize - (int) (mySize) / 8);
                        prevSize = mySize / 8;
                    }
                    if (mySize % 8 != 0)
                        mySize -= (mySize % 8);
                    LOG(Main, 1) << "msg for send " << msg.getVal(" ") << " on channel: " << myChannel << endl;
                    command->speak(myChannel, msg.getValue());
                }
                //captain orders:
                if (world->self->isAmbulanceTeam() && maxSizeForCaptainOfAmbulances > 0)
                {
                    //recognize captain
                    AmbulanceTeam* captain = NULL;
                    for (int i = 0; i < world->ambulanceTeams.size(); i++)
                        if (world->ambulanceTeams[i]->isAvailable)
                        {
                            captain = world->ambulanceTeams[i];
                            break;
                        }
                    //if I am captain
                    if (captain != NULL && ((AmbulanceTeam*) world->self)->ambulanceTeamIndex == captain->ambulanceTeamIndex)
                    {
                        LOG(Main, 2) << "Captain Orders" << endl;
                        int captainSize = maxSizeForCaptainOfAmbulances * 8;
                        LOG(Main, 2) << "Captain size: " << captainSize << endl;
                        msg.clear();
                        msg.setSize(maxSizeForCaptainOfAmbulances);
                        setPersonalInformation(msg, captainSize);
                        setAmbulancesOrders(msg, captainSize);
                        LOG(Main, 2) << "msg for send orders: " << msg.getVal(" ") << endl;
                        command->speak(channelsForHearOfPlatoons[0], msg.getValue());
                    }
                }
            }
        }

        break;
    }
    case RM_COMMUNICATIONLESS:
    {
        break;
    }
    }

    if (voiceChannels.size() > 0 && world->self->isPlatoon())
        say();
    LOG(Main, 1) << "end of shareSense" << endl;
}

void Radar::say()
{
    LOG(Main, 1) << "start saying" << endl;
    BoolStream msg;
    int time = world->getTime();
    int mySize = voiceChannels[0]->getMaxSize() * 8;
    msg.setSize(voiceChannels[0]->getMaxSize());
    while (time > 0 && mySize > 0)
    {
        LOG(Main, -1) << "time: " << time << endl;
        for (int i = 0; i < world->civilians.size(); i++)
        {
            Civilian *civ = world->civilians[i];
            if (civ->getLastCycleUpdatedBySense() == time)
            {
                if (mySize < MBOS_HEADER + MBOS_ID + MBOS_HP + MBOS_DAMAGE + MBOS_BURIEDNESS + MBOS_POSITION + MBOS_TIME)
                    break;
                msg.push_back(bits(MP_CIVILIAN_NORMAL, MBOS_HEADER));
                msg.push_back(bits(civ->getId(), MBOS_ID));
                msg.push_back(bits(civ->getHp(), MBOS_HP));
                msg.push_back(bits(civ->getDamage(), MBOS_DAMAGE));
                msg.push_back(bits(civ->getBuriedness(), MBOS_BURIEDNESS));
                if (civ->getRepresentiveNodeIndex() == -1)
                {
                    msg.push_back(bits(civ->getWithoutBlockadeRepresentiveNodeIndex(), MBOS_POSITION - 2));
                    msg.push_back(bits(2 + (worldGraph->getNodes()[civ->getWithoutBlockadeRepresentiveNodeIndex()]->getFirstMotionlessIndex() == civ->getMotionlessObject()->motionlessIndex), 2));
                }
                else
                {
                    msg.push_back(bits(civ->getRepresentiveNodeIndex(), MBOS_POSITION - 2));
                    msg.push_back(bits(worldGraph->getNodes()[civ->getRepresentiveNodeIndex()]->getFirstMotionlessIndex() == civ->getMotionlessObject()->motionlessIndex, 2));
                }
                msg.push_back(bits(civ->getLastCycleUpdated(), MBOS_TIME));
                mySize -= MBOS_HEADER + MBOS_ID + MBOS_HP + MBOS_DAMAGE + MBOS_BURIEDNESS + MBOS_POSITION + MBOS_TIME;
            }
        }
        for (int i = 0; i < world->buildings.size(); i++)
        {
            Building *building = world->buildings[i];
            if (building->getLastCycleUpdatedBySense() == time && building->getTemperature() > 0)
            {
                LOG(Main, -1) << "in the if" << endl;
                if (mySize < MBOS_HEADER + MBOS_INDEX_OF_BUILDGINS_AND_ROADS + MBOS_TEMPERTURE + MBOS_FIERYNESS + MBOS_TIME)
                    break;
                msg.push_back(bits(MP_BUILDING, MBOS_HEADER));
                msg.push_back(bits(building->buildingIndex, MBOS_INDEX_OF_BUILDGINS_AND_ROADS));
                msg.push_back(bits(building->getTemperature(), MBOS_TEMPERTURE));
                if (building->isBurning())
                    msg.push_back(bits(1, MBOS_FIERYNESS));
                else if (building->getFieryness() == 8)
                    msg.push_back(bits(2, MBOS_FIERYNESS));
                else
                    msg.push_back(bits(0, MBOS_FIERYNESS));
                msg.push_back(bits(building->getLastCycleUpdated(), MBOS_TIME));
                mySize -= MBOS_HEADER + MBOS_INDEX_OF_BUILDGINS_AND_ROADS + MBOS_TEMPERTURE + MBOS_FIERYNESS + MBOS_TIME;
            }
        }
        for (int i = 0; i < (int) world->roads.size(); i++)
        {
            if (world->roads[i]->getHasToBeSent() == time)
            {
                int bSize = world->roads[i]->getRelativeEdgesIndexes().size();
                //				LOG(Main, 1) << "road: " << world->roads[i]->getId() << ' ' << bSize << endl;
                if (mySize >= MBOS_INDEX_OF_BUILDGINS_AND_ROADS + MBOS_HEADER + bSize)
                {
                    msg.push_back(bits(MP_ROAD_BLOCKADE_DATA, MBOS_HEADER));
                    msg.push_back(bits(world->roads[i]->roadIndex, MBOS_INDEX_OF_BUILDGINS_AND_ROADS));
                    LOG(Main, 1) << "information of road " << world->roads[i]->getId() << " sent." << endl;
                    LOG(Main, 1) << "has to be sent " << world->roads[i]->getHasToBeSent() << endl;
                    //					LOG(Main, 1) << "number of relative edges: " << world->roads[i]->getRelativeEdgesIndexes().size() << endl;
                    for (int j = 0; j < world->roads[i]->getRelativeEdgesIndexes().size(); j++)
                    {
                        //						LOG(Main, 1) << (worldGraph->getEdges()[world->roads[i]->getRelativeEdgesIndexes()[j]]->getPassingMode() == PM_PASSABLE);
                        msg.push_back(worldGraph->getEdges()[world->roads[i]->getRelativeEdgesIndexes()[j]]->getPassingMode() == PM_PASSABLE);
                    }
                    //					LOG(Main, 1) << endl;
                    mySize -= MBOS_INDEX_OF_BUILDGINS_AND_ROADS + MBOS_HEADER + bSize;
                }
            }
        }
        time--;
    }
    LOG(Main, -1) << "msg for send " << msg.getVal(" ") << endl;
    command->speak(voiceChannels[0]->getChannelNum(), msg.getValue());
}

void Radar::subscribe()
{
    switch (radarMode)
    {
    case RM_DISTRIBUTE:
    {
        if (world->self->isAmbulanceTeam())
        {
            command->subscribe(channelsForAmbulanceTeams);
            LOG(Main, 1) << "I am subscribing ";
            for (int i = 0; i < channelsForAmbulanceTeams.size(); i++)
                LOG(Main, 1) << channelsForAmbulanceTeams[i] << " ";
            LOG(Main, 1) << endl;
        }
        else if (world->self->isFireBrigade())
            command->subscribe(channelsForFireBrigades);
        else if (world->self->isPoliceForce())
            command->subscribe(channelsForPoliceForces);
        break;
    }
    case RM_ONE_CHANNEL:
    {
        if (world->self->isHuman())
        {
            command->subscribe(channelsForHearOfPlatoons);
            LOG(Main, 1) << "I am subscribing ";
            for (int i = 0; i < channelsForHearOfPlatoons.size(); i++)
                LOG(Main, 1) << channelsForHearOfPlatoons[i] << " ";
            LOG(Main, 1) << endl;
        }
        break;
    }
    case RM_CENTER:
    {
        if (world->self->isHuman())
            command->subscribe(channelsForHearOfPlatoons);
        else
        {
            command->subscribe(channelsForHearOfCenters);
            LOG(Main, 1) << "I am subscribing ";
            for (int i = 0; i < channelsForHearOfCenters.size(); i++)
                LOG(Main, 1) << channelsForHearOfCenters[i] << " ";
            LOG(Main, 1) << endl;
        }
        break;
    }
    case RM_COMMUNICATIONLESS:
    {
        break;
    }
    }
}

bool Radar::checkValidateMaxByte(int maxByte, vector<RadioChannel*> channels)
{
    int counter = 0;
    for (int i = 0; i < (int) channels.size(); i++)
        counter += channels[i]->getBandWidth() / maxByte;
    if (counter >= (int) world->platoons.size())
        return true;
    return false;
}

bool Radar::checkValidateMaxByte(int maxByte, vector<RadioChannel*> channels, int ignoreBandwidth)
{
    int counter = 0;
    for (int i = 0; i < (int) channels.size(); i++)
    {
        if (!i)
            counter += (channels[i]->getBandWidth() - ignoreBandwidth) / maxByte;
        else
            counter += channels[i]->getBandWidth() / maxByte;
    }
    if (counter >= (int) world->platoons.size())
        return true;
    return false;
}

void Radar::analyseMessage(vector<byte>& radarMsg, int& offset, int sizeOfContent, int senderId, int channel)
{
    LOG(Main, 1) << "radar message begins" << endl;
    BoolStream msg(sizeOfContent + 1);
    msg.setValue(vector<byte > (radarMsg.begin() + offset, radarMsg.begin() + offset + sizeOfContent));
    //convert size of content from byte to bit
    sizeOfContent *= 8;
    LOG(Main, 1) << "msg: " << msg.getVal(" ") << endl;
    bool isRadio = true;
    for (int i = 0; i < voiceChannels.size(); i++)
        if (voiceChannels[i]->getChannelNum() == channel)
            isRadio = false;
    if (!isRadio)
        LOG(Main, 1) << "is voice" << endl;
    if (sizeOfContent > 8)
    {

        if (isRadio)
        {
            if (world->objects[senderId]->isPlatoon())
            {
                if (sizeOfContent > 0)
                {
                    int pos = msg.pop_bits(MBOS_POSITION - 2).getA();
                    int side = msg.pop_bits(2).getA();
                    if (world->objects[senderId]->isFireBrigade())
                    {
                        int waterQuantity = msg.pop_bits(MBOS_WATER_QUANTITY).getA();
                        LOG(Main, 1) << "water quantity: " << waterQuantity << endl;
                        ((FireBrigade*) world->objects[senderId])->setHasWaterValue(waterQuantity);
                        sizeOfContent -= MBOS_WATER_QUANTITY;
                    }
                    setPosition((Human*) world->objects[senderId], pos, side);
                }
            }
            else
            {
                LOG(Main, 1) << "sender was center" << endl;
                if (sizeOfContent > 0)
                {
                    int numOfPositions = msg.pop_bits((MBOS_SIZE)).getA();
                    sizeOfContent -= MBOS_SIZE;
                    LOG(Main, 1) << "numOfPositions: " << numOfPositions << endl;
                    while (numOfPositions > 0 && sizeOfContent >= MBOS_POSITION + MBOS_INDEX_OF_PLATOONS)
                    {
                        int index = msg.pop_bits(MBOS_INDEX_OF_PLATOONS).getA();
                        int pos = msg.pop_bits(MBOS_POSITION - 2).getA();
                        int side = msg.pop_bits(2).getA();
                        setPosition(world->platoons[index], pos, side);
                        world->platoons[index]->setLastCycleUpdated(world->getTime());
                        numOfPositions--;
                        sizeOfContent -= MBOS_INDEX_OF_PLATOONS + MBOS_POSITION;
                    }
                }
            }
        }
        else if (world->objects[senderId] == NULL || !world->objects[senderId]->isPlatoon())
        {
            LOG(Main, 1) << "A civilian is screaming!!!" << endl;
            return;
        }
        while (sizeOfContent > 3)
        {
            int header = msg.pop_bits(MBOS_HEADER).getA();
            LOG(Main, 1) << "header: " << header << endl;
            switch (header)
            {
            case MP_BUILDING:
            {
                int index = msg.pop_bits(MBOS_INDEX_OF_BUILDGINS_AND_ROADS).getA();
                LOG(Main, 1) << "id: " << world->buildings[index]->getId() << endl;
                int temperture = msg.pop_bits(MBOS_TEMPERTURE).getA();
                LOG(Main, 1) << "temperture: " << temperture << endl;
                int fieryness = msg.pop_bits(MBOS_FIERYNESS).getA();
                LOG(Main, 1) << "fieryness: " << fieryness << endl;
                int updatedTime;
                if (!isRadio)
                {
                    updatedTime = msg.pop_bits(MBOS_TIME).getA();
                    if (updatedTime < world->buildings[index]->getLastCycleUpdated())
                        break;
                }
                if (world->buildings[index]->getLastCycleUpdatedBySense() != world->getTime())
                {
                    world->buildings[index]->setTemperature(temperture);
                    if (fieryness == 0)
                        world->buildings[index]->setFieryness(0);
                    else if (fieryness == 1)
                        world->buildings[index]->setFieryness(1);
                    else if (fieryness == 2)
                        world->buildings[index]->setFieryness(8);

                    if (!isRadio)
                        world->buildings[index]->setLastCycleUpdated(updatedTime);
                    else
                        world->buildings[index]->setLastCycleUpdated(world->getTime());
                }
                sizeOfContent -= MBOS_INDEX_OF_BUILDGINS_AND_ROADS + MBOS_TEMPERTURE;
                break;
            }
            case MP_CIVILIAN_NORMAL:
            {
                int id = msg.pop_bits(MBOS_ID).getA();
                LOG(Main, 1) << "ID: " << id << endl;
                int hp = msg.pop_bits(MBOS_HP).getA();
                LOG(Main, 1) << "HP: " << hp << endl;
                int damage = msg.pop_bits(MBOS_DAMAGE).getA();
                LOG(Main, 1) << "damage: " << damage << endl;
                int buriedness = msg.pop_bits(MBOS_BURIEDNESS).getA();
                LOG(Main, 1) << "buriedness: " << buriedness << endl;
                int position = msg.pop_bits(MBOS_POSITION - 2).getA();
                int side = msg.pop_bits(2).getA();
                int updatedTime;
                if (!isRadio)
                {
                    updatedTime = msg.pop_bits(MBOS_TIME).getA();
                }
                if (world->objects[id] == NULL)
                {
                    Civilian* civ = new Civilian(id);
                    civ->setHp(hp);
                    civ->setDamage(damage);
                    civ->setBuriedness(buriedness);

                    setPosition(civ, position, side);
                    //                        civ->setPosition(world->motionlessObjects[position]->getId());

                    civ->setLastCycleUpdated(world->getTime());
                    //                        civ->setEstimateDamage(damage);
                    //                        civ->setEstimateHp(hp);
                    civ->setSpotedTime(world->getTime());
                    civ->humanIndex = world->humans.size() - 1;
                    if (civ->getBuriedness() == 0)
                        civ->isAvailable = false;
                    world->civilians.push_back(civ);
                    world->humans.push_back(civ);
                    world->objects[id] = civ;
                }
                else
                {
                    Civilian* civ = (Civilian*) (world->objects[id]);
                    if (!isRadio)
                    {
                        //                            LOG(Main, 7) << "civ->getLastCycleUpdated(): " << civ->getLastCycleUpdated() << " updatedTime: " << updatedTime << endl;
                        if (updatedTime <= civ->getLastCycleUpdated())
                            break;
                    }
                    if (civ->getLastCycleUpdatedBySense() != world->getTime())
                    {
                        civ->setHp(hp);
                        civ->setDamage(damage);
                        civ->setBuriedness(buriedness);

                        setPosition(civ, position, side);
                        //                            civ->setPosition(world->motionlessObjects[position]->getId());
                        if (!isRadio)
                            civ->setLastCycleUpdated(updatedTime);
                        else
                            civ->setLastCycleUpdated(world->getTime());
                        civ->setEstimateDamage(damage);
                        civ->setEstimateHp(hp);

                    }
                }
                sizeOfContent -= MBOS_ID + MBOS_HP + MBOS_DAMAGE + MBOS_BURIEDNESS + MBOS_POSITION;
                break;
            }
            case MP_CIVILIAN_FERT:
            {
                int id = msg.pop_bits(MBOS_ID).getA();
                if (world->objects[id] != NULL)
                {
                    Civilian* civ = (Civilian*) world->objects[id];
                    civ->isAvailable = false;
                }
                break;
            }
            case MP_AGENT_FIRST_TIME:
            {
                LOG(Main, 1) << "Agent_First_Time" << endl;
                int platoonIndex = msg.pop_bits(MBOS_INDEX_OF_PLATOONS).getA();
                int buriedness = msg.pop_bits(MBOS_BURIEDNESS).getA();
                if (world->platoons[platoonIndex]->getLastCycleUpdatedBySense() != world->getTime())
                {
                    LOG(Main, 1) << "buriedness: " << buriedness << endl;
                    world->platoons[platoonIndex]->setBuriedness(buriedness);
                    world->platoons[platoonIndex]->setLastCycleUpdated(world->getTime());
                    sizeOfContent -= MBOS_INDEX_OF_PLATOONS + MBOS_BURIEDNESS;
                }
                break;

            }
            case MP_AMBULANCE_ORDER:
            {
                LOG(Main, 11) << "recieve orders" << endl;
                for (int i = 0; i < world->ambulanceTeams.size(); i++)
                {
                    LOG(Main, 11) << "ambulance id: " << world->ambulanceTeams[i]->getId() << endl;
                    int targetId = msg.pop_bits(MBOS_INDEX_OF_BUILDGINS_AND_ROADS).getA();
                    if (targetId == world->buildings.size())
                        continue;
                    LOG(Main, 11) << "building target ID: " << world->buildings[targetId]->getId() << endl;
                    int humanIndex = msg.pop_bits(MBOS_INDEX_OF_CIVILIANS_IN_A_BUILDING).getA();
                    world->ambulanceTeams[i]->targetIndex = humanIndex;
                    LOG(Main, 11) << "human index " << humanIndex << endl;
                    world->ambulanceTeams[i]->buildingTarget = world->buildings[targetId];
                }
                sizeOfContent -= world->ambulanceTeams.size() * MBOS_INDEX_OF_BUILDGINS_AND_ROADS+ world->ambulanceTeams.size() * MBOS_INDEX_OF_CIVILIANS_IN_A_BUILDING;
                break;
            }
            case MP_ROAD_BLOCKADE_DATA:
            {
                int roadIndex = msg.pop_bits(MBOS_INDEX_OF_BUILDGINS_AND_ROADS).getA();
                LOG(Main, 1) << "Rescieved information about road " << world->roads[roadIndex]->getId() << endl;
                LOG(Main, 1) << "number of relative edges: " << world->roads[roadIndex]->getRelativeEdgesIndexes().size() << endl;
                for (int i = 0; i < (int) world->roads[roadIndex]->getRelativeEdgesIndexes().size(); i++)
                {
                    bool pm = msg.pop_bool();
                    worldGraph->getEdges()[world->roads[roadIndex]->getRelativeEdgesIndexes()[i]]->setPassingMode(pm ? PM_PASSABLE : PM_NOT_PASSABLE);
                    LOG(Main, 1) << pm;
                }
                LOG(Main, 1) << endl;
                sizeOfContent -= MBOS_INDEX_OF_BUILDGINS_AND_ROADS + (int) world->roads[roadIndex]->getRelativeEdgesIndexes().size();
                break;
            }
            case MP_END:
            {
                sizeOfContent = 0;
                break;
            }
            }
        }

    }
    else
    {
        LOG(Main, 1) << "this message had noise and I didn't recieve anything" << endl;
    }
    LOG(Main, 2) << "analyze radar message ended" << endl;
}

void Radar::setPosition(Human* human, int position, int side)
{
    LOG(Main, 9) << "human: " << human->getId() << " pos: " << position << " side: " << side << endl;
    if (side < 2)
    {
        human->setRepresentiveNodeIndex(position);
    }
    else
    {
        human->setRepresentiveNodeIndex(-1);
        human->setWithoutBlockadeRepresentiveNodeIndex(position);
        side -= 2;
    }
    if (side == 1)
    {
        human->setPosition(world->motionlessObjects[worldGraph->getNodes()[position]->getFirstMotionlessIndex()]->getId());
    }
    else
    {
        human->setPosition(world->motionlessObjects[worldGraph->getNodes()[position]->getSecondMotionlessIndex()]->getId());
    }
    LOG(Main, 1) << "human " << human->getId() << " pos : " << human->getPosition() << endl;
}