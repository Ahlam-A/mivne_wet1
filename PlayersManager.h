#ifndef PLAYERS_MANAGER
#define PLAYERS_MANAGER

#include "library1.h"
#include "AVLTree.h"

class GroupPlayer
{
	int player_id;
	int level;

};

class Group
{
	int group_id;
	int group_size;
	GroupPlayer* highest_player;
	AVLTree<GroupPlayer>* groupPlayers;
};

class GroupPointer
{
	int group_id;
	Group* group;
};

class Player
{

};


class PlayersManager
{
	AVLTree<Group>* groupTree;
	AVLTree<GroupPointer>* NonEmptyGroups;
	AVLTree<Player>* playerTree;
};

#endif // PLAYERS_MANAGER
