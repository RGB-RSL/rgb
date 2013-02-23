/*
 * Command.h
 *
 *  Created on: Feb 8, 2010
 *      Author: Alimohammad
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include "../WorldModel/Objects/Civilian.h"
#include "../WorldModel/Objects/Building.h"
#include "../WorldModel/Objects/Road.h"
#include <vector>
#include "../WorldModel/WorldModel.h"
#include "../Connection/OnlineConnection.h"
#include "../Connection/Message.h"
#include "../Utilities/BoolStream.h"

class WorldGraph;

struct Command
{
public:
	Command(WorldGraph* worldGraph, WorldModel * world, AbstractConnection * connection);
	virtual ~Command();

	//Commands
	void load(Human & target);
	void load(Human &target, int time);

	void unload();
	void unload(int time);

	void rescue(const Human & target);
	void rescue(const Human &target, int time);\

	void rest();
	void rest(int time);

	void extinguish(const Building &target, int water);
	void extinguish(const Building &target, int water, int time);

	void clear(Blockade & target);
	void clear(Blockade &target, int time);

	void move(std::vector<int> &path);
	void move(std::vector<int> &path, int time);
	void move(std::vector<int> &path, int destinationX, int destinationY);
	void move(std::vector<int> &path, int destinationX, int destinationY, int time);

	void speak(int channel, std::vector<byte> data);

	void subscribe(std::vector<int> channels);

	void moveToMotionless(int motionlessIndex, Types::GraphMode graphMode);
	void moveToHuman(int humanIndex, Types::GraphMode graphMode);
	void moveToPoint(int motionlessIndex, Geometry::Point p, Types::GraphMode graphMode);


	std::vector<byte> getMessage();
private:
	std::vector<int> getPathToNode(int nodeIndex, Types::GraphMode graphMode);
	Message cm;
	std::vector<byte> message;
	WorldModel *world;
	AbstractConnection *connection;
	WorldGraph *worldGraph;
};



#endif /* COMMANDER_H_ */
