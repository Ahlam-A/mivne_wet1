#ifndef PLAYERS_MANAGER
#define PLAYERS_MANAGER

#include "library1.h"
#include "AVLTree.h"

class Player
{
	int id;
	int group_id;
	int level;

	friend PlayerPointer;

public:
	AVLNode<PlayerPointer>* player_level; // pointer to Player's node in playerLevels tree
	AVLNode<PlayerPointer>* group_player; // pointer to Player's node in its group's groupPlayers tree

	Player() = default;
	Player(int playerId, int groupId, int level) :
		id(playerId), group_id(groupId), level(level) {}
	~Player() = default;

	Player& operator=(const Player&);

	const bool operator<(int id) { return this->id < id; }
	const bool operator<(const Player& p) { return this->id < p.id; }
	const bool operator>(int id) { return this->id > id; }
	const bool operator>(const Player& p) { return this->id > p.id; }
	const bool operator==(int id) { return this->id == id; }
	const bool operator==(const Player& p) { return this->id == p.id; }

	int getId() { return id; }
	int getLevel() { return level; }
};

class PlayerPointer
{
public:
	Player* player; // pointer to Player data in playerTree

	PlayerPointer& operator=(const PlayerPointer& pp); 
	
	//const bool operator<(int pp) { return player->level < pp; }
	//const bool operator>(int pp);
	//const bool operator==(int pp);

	const bool operator<(const PlayerPointer& pp) {
		if (player->level == pp.player->level)
			return player->id > pp.player->id;
		return player->level < pp.player->level;
	}
	const bool operator>(const PlayerPointer& pp) {
		if (player->level == pp.player->level)
			return player->id < pp.player->id;
		return player->level > pp.player->level;
	}
	//const bool operator==(const PlayerPointer& pp);

};

class Group
{
	int id;
	int size;
	PlayerPointer* highest_player;
	AVLTree<PlayerPointer>* groupPlayers; //sorted by level first, id second

	friend GroupPointer;

public:

	Group() = default;
	Group(int id) : id(id) 
	{
		size = 0;
		highest_player = nullptr;
		groupPlayers = new AVLTree<PlayerPointer>();
	}

	~Group() { delete groupPlayers; }

	Group& operator=(const Group&);

	const bool operator<(int id) { return this->id < id; }
	const bool operator<(const Group& g) { return this->id < g.id; }
	const bool operator>(int id) { return this->id > id; }
	const bool operator>(const Group& g) { return this->id > g.id; }
	const bool operator==(int id) { return this->id == id; }
	const bool operator==(const Group& g) { return this->id == g.id; }

};


class GroupPointer
{
public:
	Group* group;

	GroupPointer& operator=(const GroupPointer& gp);

	const bool operator<(int id) { return group->id < id; }
	const bool operator<(const GroupPointer& gp) { return group->id < gp.group->id; }
	const bool operator>(int id) { return group->id > id; }
	const bool operator>(const GroupPointer& gp) { return group->id > gp.group->id; }
	const bool operator==(int id) { return group->id == id; }
	const bool operator==(const GroupPointer& gp) { return group->id == gp.group->id; }
};


class PlayersManager
{
	AVLTree<Group>* groupTree;
	AVLTree<GroupPointer>* NonEmptyGroups;
	AVLTree<Player>* playerTree; //sorted by id
	AVLTree<PlayerPointer>* playerLevels; //sorted by level first, id second

public:

	PlayersManager();
	~PlayersManager();

	StatusType AddGroup(int GroupID);
	StatusType AddPlayer(int PlayerID, int GroupID, int Level);
	StatusType RemovePlayer(int PlayerID);
	StatusType ReplaceGroup(int GroupID, int ReplacementID);
	StatusType IncreaseLevel(int PlayerID, int LevelIncrease);
	StatusType GetHighestLevel(int GroupID, int* PlayerID);
	StatusType GetAllPlayersByLevel(int GroupID, int** Players, int* numOfPlayers);
	StatusType GetGroupsHighestLevel(int numOfGroups, int** Players);
};

#endif // PLAYERS_MANAGER
