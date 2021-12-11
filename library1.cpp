#include "PlayersManager.h"


void* Init()
{
	return new PlayersManager();
}

StatusType AddGroup(void* DS, int GroupID)
{
	if (DS == NULL)
		return INVALID_INPUT;
	return ((PlayersManager*)DS)->AddGroup(GroupID);
}

StatusType AddPlayer(void* DS, int PlayerID, int GroupID, int Level)
{
	if (DS == NULL)
		return INVALID_INPUT;
	return ((PlayersManager*)DS)->AddPlayer(PlayerID, GroupID, Level);
}

StatusType RemovePlayer(void* DS, int PlayerID)
{
	if (DS == NULL)
		return INVALID_INPUT;
	return ((PlayersManager*)DS)->RemovePlayer(PlayerID);
}

StatusType ReplaceGroup(void* DS, int GroupID, int ReplacementID)
{
	if (DS == NULL)
		return INVALID_INPUT;
	return ((PlayersManager*)DS)->ReplaceGroup(GroupID, ReplacementID);
}

StatusType IncreaseLevel(void* DS, int PlayerID, int LevelIncrease)
{
	if (DS == NULL)
		return INVALID_INPUT;
	return ((PlayersManager*)DS)->IncreaseLevel(PlayerID, LevelIncrease);
}

StatusType GetHighestLevel(void* DS, int GroupID, int* PlayerID)
{
	if (DS == NULL)
		return INVALID_INPUT;
	return ((PlayersManager*)DS)->GetHighestLevel(GroupID, PlayerID);
}

StatusType GetAllPlayersByLevel(void* DS, int GroupID, int** Players, int* numOfPlayers)
{
	if (DS == NULL)
		return INVALID_INPUT;
	return ((PlayersManager*)DS)->GetAllPlayersByLevel(GroupID, Players, numOfPlayers);
}

StatusType GetGroupsHighestLevel(void* DS, int numOfGroups, int** Players)
{
	if (DS == NULL)
		return INVALID_INPUT;
	return ((PlayersManager*)DS)->GetGroupsHighestLevel(numOfGroups, Players);
}

void Quit(void** DS)
{
	if (DS == NULL || *DS == NULL)
		return;
	delete* ((PlayersManager**)DS);
	*DS = NULL;
}
