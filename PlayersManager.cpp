#include "PlayersManager.h"


/* ------------------------------------------ Helper Functions ------------------------------------------ */

static AVLNode<PlayerPointer>* arrayToTreeAux(PlayerPointer** arr, int start, int end)
{
	if (end < start)
		return NULL;

	int mid = (start + end) / 2;

	AVLNode<PlayerPointer>* node = new AVLNode<PlayerPointer>(arr[mid]);
	(arr[mid])->player->group_player = node;
	
	node->setLChild(arrayToTreeAux(arr, start, mid - 1));
	node->setRChild(arrayToTreeAux(arr, mid + 1, end));

	node->calculateStats();

	return node;
}

static AVLTree<PlayerPointer>* arrayToTree(PlayerPointer** sortedArray, int size)
{
	AVLNode<PlayerPointer>* root = arrayToTreeAux(sortedArray, 0, size - 1);
	AVLTree<PlayerPointer>* tree = new AVLTree<PlayerPointer>(root, size);

	return tree;
}

static int mergeArrays(PlayerPointer** arr1, int size1, PlayerPointer** arr2, int size2, PlayerPointer** mergeArr)
{
	int i = 0, index1 = 0, index2 = 0;

	while (index1 < size1 && index2 < size2)
	{
		if (*arr1[index1] < *arr2[index2]) {
			mergeArr[i] = arr1[index1];
			index1++;
		}
		else if (*arr1[index1] > *arr2[index2]) {
			mergeArr[i] = arr2[index2];
			index2++;
		}
		i++;
	}

	while (index1 < size1)
	{
		mergeArr[i] = arr1[index1];
		index1++;
		i++;
	}

	while (index2 < size2)
	{
		mergeArr[i] = arr2[index2];
		index2++;
		i++;
	}

	return i;
}

static AVLTree<PlayerPointer>* mergeTrees(AVLTree<PlayerPointer>* tree1, AVLTree<PlayerPointer>* tree2)
{
	int size1 = tree1->getSize();
	int size2 = tree2->getSize();

	if (size1 > 0 && size2 > 0)
	{
		PlayerPointer** arr1 = tree1->orderedArray(size1);
		PlayerPointer** arr2 = tree2->orderedArray(size2);
		PlayerPointer** mergedArr = new PlayerPointer*[size1 + size2];
		int mergeSize = mergeArrays(arr1, size1, arr2, size2, mergedArr);
		AVLTree<PlayerPointer>* mergedTree = arrayToTree(mergedArr, mergeSize);

		delete[] arr1;
		delete[] arr2;
		delete[] mergedArr;

		return mergedTree;
	}

	else if (size1 > 0)
	{
		PlayerPointer** arr = tree1->orderedArray(size1);
		AVLTree<PlayerPointer>* tree = arrayToTree(arr, tree1->getSize());

		delete[] arr;
		return tree;
	}

	else if (size2 > 0)
	{
		PlayerPointer** arr = tree2->orderedArray(size2);
		AVLTree<PlayerPointer>* tree = arrayToTree(arr, tree2->getSize());

		delete[] arr;
		return tree;
	}

	return new AVLTree<PlayerPointer>();
}


/* ------------------------------------------ PlayersManager Functions ------------------------------------------ */


Group& Group::operator=(const Group& g)
{
    this->id = g.getGroupId();
    this->size = g.getSize();
    this->groupPointer = g.groupPointer;

    if (this->groupPlayers) {
        delete groupPlayers;
        this->groupPlayers = nullptr;
    }

    if (g.groupPlayers->getSize() > 0) {
        PlayerPointer** playerArr = g.groupPlayers->orderedArray(g.groupPlayers->getSize());
        this->groupPlayers = arrayToTree(playerArr, g.groupPlayers->getSize());
        delete[] playerArr;
        groupPlayers->updateHighest();
        this->highest_player = groupPlayers->getHighest();
    }
    else {
        this->groupPlayers = new AVLTree<PlayerPointer>();
        this->highest_player = nullptr;
    }
    return *this;
}


PlayersManager::PlayersManager()
{
	groupTree = new AVLTree<Group>();
	NonEmptyGroups = new AVLTree<GroupPointer>();
    playersById = new AVLTree<Player>();
    playersByLevel = new AVLTree<PlayerPointer>();
}

PlayersManager::~PlayersManager()
{
	delete groupTree;
	delete NonEmptyGroups;
	delete playersById;
	delete playersByLevel;
}

StatusType PlayersManager::AddGroup(int GroupID)
{
    if (GroupID <= 0) return INVALID_INPUT;
    
    if (groupTree->findData(GroupID))
        return FAILURE;

    Group newGroup = Group(GroupID);
    TreeResult insertResult = groupTree->insertNode(&newGroup, nullptr);
    if (insertResult == TreeResult::NODE_ALREADY_EXISTS)
        return FAILURE;
    if (insertResult == TreeResult::OUT_OF_MEMORY)
        return ALLOCATION_ERROR;

    return SUCCESS;
}

static StatusType addPlayerToGroup(Player* new_player, Group* group, 
    AVLTree<Player>* playersById, AVLTree<PlayerPointer>* playersByLevel, AVLTree<GroupPointer>* NonEmptyGroups) 
{    
    PlayerPointer new_player_ptr = PlayerPointer();
    new_player_ptr.player = new_player;

    TreeResult res = playersByLevel->insertNode(&new_player_ptr, &new_player->player_level);
    if (res == TreeResult::OUT_OF_MEMORY) {
        return ALLOCATION_ERROR;
    }

    TreeResult res1 = group->groupPlayers->insertNode(&new_player_ptr, &new_player->group_player);
    if (res1 == TreeResult::OUT_OF_MEMORY) {
        return ALLOCATION_ERROR;
    }
    group->highest_player = group->groupPlayers->getHighest();
    group->increaseSize();

    if (group->getSize() == 1) {
        GroupPointer new_nonEmptyGroup = GroupPointer();
        new_nonEmptyGroup.group = group;
        
        TreeResult res2 = NonEmptyGroups->insertNode(&new_nonEmptyGroup, &group->groupPointer);
        if (res2 == TreeResult::OUT_OF_MEMORY) {
            return ALLOCATION_ERROR;
        }
    }

    AVLNode<Player>* player_node;
    TreeResult res3 = playersById->insertNode(new_player, &player_node);
    if (res3 == TreeResult::OUT_OF_MEMORY) {
        return ALLOCATION_ERROR;
    }

    new_player->player_level->getData()->player = player_node->getData();
    new_player->group_player->getData()->player = player_node->getData();

    return SUCCESS;
}

StatusType PlayersManager::AddPlayer(int PlayerID, int GroupID, int Level) 
{
    if(PlayerID <= 0 || GroupID <= 0 || Level < 0){
        return INVALID_INPUT;
    }
    Group* group = groupTree->findData(GroupID);
    
    if(!group) return FAILURE;
    if (playersById->findData(PlayerID)) return FAILURE;

    Player new_player = Player(PlayerID, Level, group);
    
    return addPlayerToGroup(&new_player, group, playersById, playersByLevel, NonEmptyGroups);
}

StatusType PlayersManager::RemovePlayer(int PlayerID)
{
    if (PlayerID <= 0) return INVALID_INPUT;

    Player* player = playersById->findData(PlayerID);
    if (player == NULL) return FAILURE;

    // delete player from playersByLevel
    AVLNode<PlayerPointer>* level_swapped;
    playersByLevel->deleteByPointer(player->player_level, &level_swapped);
    if (level_swapped) {
        level_swapped->getData()->player->player_level = level_swapped;
    }

    // delete player from groupPlayers
    Group* playerGroup = player->getGroup();

    AVLNode<PlayerPointer>* group_swapped;
    playerGroup->groupPlayers->deleteByPointer(player->group_player, &group_swapped);
    if (group_swapped) {
        group_swapped->getData()->player->group_player = group_swapped;
    }
    playerGroup->highest_player = playerGroup->groupPlayers->getHighest();
    playerGroup->setSize(playerGroup->groupPlayers->getSize());

    // if player's group has no more players, delete the group from NonEmptyGroups
    if (playerGroup->getSize() == 0) {
        AVLNode<GroupPointer>* group_swapped;
        NonEmptyGroups->deleteByPointer(playerGroup->groupPointer, &group_swapped);
        if (group_swapped) {
            group_swapped->getData()->group->groupPointer = group_swapped;
        }
        playerGroup->groupPointer = nullptr;
    }

    // delete player from playersById
    playersById->deleteNode(PlayerID);
    
    return SUCCESS;
}

StatusType PlayersManager::ReplaceGroup(int GroupID, int ReplacementID) 
{
    if(GroupID <= 0 || ReplacementID <= 0 || GroupID == ReplacementID){
        return INVALID_INPUT;
    }
    Group* group1 = groupTree->findData(GroupID);
    Group* group2 = groupTree->findData(ReplacementID);
    
    if (!group1 || !group2) {
        return FAILURE;
    }
    
    if (group1->getSize() == 0) {
        groupTree->deleteNode(GroupID);
        return SUCCESS;
    }
    
    if (group2->getSize() == 0) 
    {
        GroupPointer group2_ptr = GroupPointer();
        group2_ptr.group = group2;

        TreeResult res = NonEmptyGroups->insertNode(&group2_ptr, &group2->groupPointer);
        if (res == TreeResult::OUT_OF_MEMORY) {
            return ALLOCATION_ERROR;
        }
    }

    AVLTree<PlayerPointer>* mergedTree = mergeTrees(group1->groupPlayers, group2->groupPlayers);
    PlayerPointer** mergedArr = mergedTree->orderedArray(mergedTree->getSize());
    for (int i = 0; i < mergedTree->getSize(); ++i) {
        mergedArr[i]->player->updateGroup(group2);
    }
    delete[] mergedArr;

    delete group2->groupPlayers;
    group2->groupPlayers = mergedTree;
    group2->setSize(mergedTree->getSize());
    group2->highest_player = group2->groupPlayers->getHighest();
    
    AVLNode<GroupPointer>* group_swapped;
    NonEmptyGroups->deleteByPointer(group1->groupPointer, &group_swapped);
    if (group_swapped) {
        group_swapped->getData()->group->groupPointer = group_swapped;
    }
    groupTree->deleteNode(GroupID);

    return SUCCESS;
}

StatusType PlayersManager::IncreaseLevel(int PlayerID, int LevelIncrease)
{
    if (PlayerID <= 0 || LevelIncrease <= 0)
        return INVALID_INPUT;

    Player* player = playersById->findData(PlayerID);
    if (player == NULL) return FAILURE;

    Group* player_group = player->getGroup();
    int player_level = player->getLevel();
    Player updated_player = Player(PlayerID, player_level + LevelIncrease, player_group);

    RemovePlayer(PlayerID);
    
    return addPlayerToGroup(&updated_player, player_group, playersById, playersByLevel, NonEmptyGroups);

}

StatusType PlayersManager::GetHighestLevel(int GroupID, int *PlayerID) {
    if(GroupID == 0 || !PlayerID){
        return INVALID_INPUT;
    }
    if(GroupID < 0){
        if (playersById->getSize() == 0) {
            *PlayerID = -1;
            return SUCCESS;
        }
        *PlayerID = playersByLevel->getHighest()->player->getId();
        return SUCCESS;
    }
    Group* group = groupTree->findData(GroupID);
    if(!group){
        return FAILURE;
    }
    if(group->getSize() == 0){
        *PlayerID = -1;
        return SUCCESS;
    }
    *PlayerID = group->highest_player->player->getId();
    return SUCCESS;
}

static int* getPlayersByLevel(int numOfPlayers, AVLTree<PlayerPointer>* playersTree)
{
    int* players = new int[numOfPlayers];

    PlayerPointer** player_pointers = playersTree->orderedArray(numOfPlayers);

    int i = numOfPlayers - 1;
    int j = 0;
    while (i >= 0 && j < numOfPlayers) {
        players[j] = player_pointers[i]->player->getId();
        i--;
        j++;
    }
    delete[] player_pointers;

    return players;
}

StatusType PlayersManager::GetAllPlayersByLevel(int GroupID, int** Players, int* numOfPlayers)
{
    if (GroupID == 0 || !Players || !numOfPlayers)
        return INVALID_INPUT;
    
    try 
    {
        if (GroupID > 0) 
        {
            Group* group = groupTree->findData(GroupID);
            if (group == NULL) return FAILURE;

            *numOfPlayers = group->groupPlayers->getSize();
            *Players = getPlayersByLevel(*numOfPlayers, group->groupPlayers);
        }

        else if (GroupID < 0) 
        {
            *numOfPlayers = playersByLevel->getSize();
            *Players = getPlayersByLevel(*numOfPlayers, playersByLevel);
        }
    }
    catch (bad_alloc&) {
        return ALLOCATION_ERROR;
    }

    return SUCCESS;
}

StatusType PlayersManager::GetGroupsHighestLevel(int numOfGroups, int **Players) 
{
    if(numOfGroups < 1 || !Players){
        return INVALID_INPUT;
    }
    if(numOfGroups > NonEmptyGroups->getSize()){
        return FAILURE;
    }
    try 
    {
        GroupPointer** arr = NonEmptyGroups->orderedArray(numOfGroups);
    
        int* highestPlayers = new int[numOfGroups];

        for (int i = 0; i < numOfGroups; i++) {
            highestPlayers[i] = arr[i]->group->highest_player->player->getId();
        }
        delete[] arr;
        
        *Players = highestPlayers;
    }
    catch (bad_alloc&) {
        return ALLOCATION_ERROR;
    }
    
    return SUCCESS;
}
