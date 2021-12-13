#ifndef PLAYERS_MANAGER
#define PLAYERS_MANAGER

#include "library1.h"
#include "AVLTree.h"

class PlayerPointer;
class Group;
class GroupPointer;

class Player
{
	int id;
	int level;
	Group* group;

	friend class PlayerPointer;

public:
	AVLNode<PlayerPointer>* player_level; // pointer to Player's node in playersByLevel tree
	AVLNode<PlayerPointer>* group_player; // pointer to Player's node in its group's groupPlayers tree

	Player() = default;
	Player(int playerId, int level, Group* group) :
		id(playerId), level(level), group(group) {
		player_level = nullptr;
		group_player = nullptr;
	}
	~Player() {}

	Player& operator=(const Player& p) {
		this->id = p.getId();
		this->level = p.getLevel();
		this->group = p.getGroup();
		this->player_level = p.player_level;
		this->group_player = p.group_player;
		return *this;
	}

	bool operator<(int id) const{ return this->id < id; }
	bool operator<(const Player& p) const{ return this->id < p.id; }
	bool operator>(int id) const{ return this->id > id; }
	bool operator>(const Player& p) const{ return this->id > p.id; }
	bool operator==(int id) const{ return this->id == id; }
	bool operator==(const Player& p) const{ return this->id == p.id; }

	int getId() const{ return id; }
	int getLevel() const{ return level; }
	Group* getGroup() const { return group; }
	void updateGroup(Group* g) { group = g; }
};

class PlayerPointer{
public:
    Player* player; // pointer to Player data in playerTree
	~PlayerPointer() {}

	PlayerPointer& operator=(const PlayerPointer& pp) {
		this->player = pp.player;
		return *this;
	}
	
	bool operator<(int level) const { return player->level < level; }
	bool operator>(int level) const { return player->level > level; }
	bool operator==(int id) const { return player->id == id; }

	bool operator==(const PlayerPointer& pp) const{
	    return player->id == pp.player->id;
	}
	bool operator<(const PlayerPointer& pp) const{
		if (player->level == pp.player->level)
			return player->id > pp.player->id;
		return player->level < pp.player->level;
	}
	bool operator>(const PlayerPointer& pp) const{
		if (player->level == pp.player->level)
			return player->id < pp.player->id;
		return player->level > pp.player->level;
	}
};

class Group
{
	int id;
	int size;

	friend class GroupPointer;

public:
    PlayerPointer* highest_player;
    AVLTree<PlayerPointer>* groupPlayers; //sorted by level first, id second
	AVLNode<GroupPointer>* groupPointer;

	Group() = default;
	Group(int id) : id(id)
	{
		size = 0;
		highest_player = nullptr;
		groupPlayers = new AVLTree<PlayerPointer>();
		groupPointer = nullptr;
	}
	~Group() { 
		if (this->groupPlayers)
			delete groupPlayers;
		groupPlayers = NULL;
	}

	Group& operator=(const Group& g);

	bool operator<(int id) const{ return this->id < id; }
	bool operator<(const Group& g) const{ return this->id < g.id; }
	bool operator>(int id) const{ return this->id > id; }
	bool operator>(const Group& g) const{ return this->id > g.id; }
	bool operator==(int id) const{ return this->id == id; }
	bool operator==(const Group& g) const{ return this->id == g.id; }

	int getGroupId() const{ return id; }
	int getSize() const{ return size; }
	void increaseSize(){ size++; }
	void setSize(int new_size){ size = new_size; }
};

class GroupPointer
{
public:
	Group* group;
	~GroupPointer() {}
	GroupPointer& operator=(const GroupPointer& gp) {
		this->group = gp.group;
		return *this;
	}

	bool operator<(int id) const{ return group->id < id; }
	bool operator<(const GroupPointer& gp) const{ return group->id < gp.group->id; }
	bool operator>(int id) const{ return group->id > id; }
	bool operator>(const GroupPointer& gp) const{ return group->id > gp.group->id; }
	bool operator==(int id) const{ return group->id == id; }
	bool operator==(const GroupPointer& gp) const{ return group->id == gp.group->id; }
};


class PlayersManager
{
	AVLTree<Group>* groupTree;
	AVLTree<GroupPointer>* NonEmptyGroups;
	AVLTree<Player>* playersById; //sorted by id
	AVLTree<PlayerPointer>* playersByLevel; //sorted by level first, id second

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
