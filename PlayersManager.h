#ifndef PLAYERS_MANAGER
#define PLAYERS_MANAGER

#include "library1.h"
#include "AVLTree.h"

class Player
{
	int player_id;
	int player_group_id;
	int level;

public:
	AVLNode<PlayerLevel>* player_level;
	AVLNode<PlayerLevel>* group_player;

	Player();
	Player(int playerId, int groupId, int level);
	~Player();

	Player& operator=(const Player&);
	const bool operator<(int);
	const bool operator<(const Player&);
	const bool operator>(int);
	const bool operator>(const Player&);
	const bool operator==(int);
	const bool operator==(const Player&);

	int getId();
};

class Group
{
	int group_id;
	int group_size;
	PlayerLevel* highest_player;
	AVLTree<PlayerLevel>* groupPlayers;
};

class PlayerLevel
{
	int player_id;
	int level;
	AVLNode<Player>* player;
};

class GroupPointer
{
	int group_id;
	Group* group;
};


class PlayersManager
{
	AVLTree<Group>* groupTree;
	AVLTree<GroupPointer>* NonEmptyGroups;
	AVLTree<Player>* playerTree;
	AVLTree<PlayerLevel>* playerLevels;
};

#endif // PLAYERS_MANAGER
