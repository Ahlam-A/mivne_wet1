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
		PlayerPointer** arr1 = tree1->orderedArray();
		PlayerPointer** arr2 = tree2->orderedArray();
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
		PlayerPointer** arr = tree1->orderedArray();
		AVLTree<PlayerPointer>* tree = arrayToTree(arr, tree1->getSize());

		delete[] arr;
		return tree;
	}

	else if (size2 > 0)
	{
		PlayerPointer** arr = tree2->orderedArray();
		AVLTree<PlayerPointer>* tree = arrayToTree(arr, tree2->getSize());

		delete[] arr;
		return tree;
	}

	return new AVLTree<PlayerPointer>();
}


/* ------------------------------------------ PlayersManager Functions ------------------------------------------ */

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
    
    Group newGroup = Group(GroupID);
    TreeResult insertResult = groupTree->insertNode(&newGroup, nullptr);
    
    if (insertResult == TreeResult::NODE_ALREADY_EXISTS)
        return FAILURE;
    if (insertResult == TreeResult::OUT_OF_MEMORY)
        return ALLOCATION_ERROR;

    return SUCCESS;
}

static StatusType addPlayerToGroup(Player* new_player, Group* group, AVLTree<GroupPointer>* NonEmptyGroups) 
{
    PlayerPointer new_player_ptr = PlayerPointer();
    new_player_ptr.player = new_player;
    
    AVLNode<PlayerPointer>* new_player_node_ptr = nullptr;
    TreeResult res1 = group->groupPlayers->insertNode(&new_player_ptr, new_player_node_ptr);
    if (res1 == TreeResult::OUT_OF_MEMORY) {
        return ALLOCATION_ERROR;
    }
    new_player->group_player = new_player_node_ptr;
    if (group->getSize() == 0) {
        GroupPointer new_nonEmptyGroup = GroupPointer();
        new_nonEmptyGroup.group = group;
        
        TreeResult res2 = NonEmptyGroups->insertNode(&new_nonEmptyGroup, group->groupPointer);
        if (res2 == TreeResult::OUT_OF_MEMORY) {
            return ALLOCATION_ERROR;
        }
        group->highest_player = &new_player_ptr;
    }
    else if (group->highest_player->player->getLevel() < new_player->getLevel()) {
        group->highest_player = &new_player_ptr;
    }
    group->increaseSize();

    return SUCCESS;
}

StatusType PlayersManager::AddPlayer(int PlayerID, int GroupID, int Level) {
    if(PlayerID <= 0 || GroupID <= 0 || Level < 0){
        return INVALID_INPUT;
    }
    Group* group = groupTree->findData(GroupID);
    if(!group){
        return FAILURE;
    }
    AVLNode<Player>* new_player_node = nullptr;
    Player new_player(PlayerID, Level, group);
    TreeResult res1 = playersById->insertNode(&new_player, new_player_node);
    if(res1 == TreeResult::NODE_ALREADY_EXISTS){
        return FAILURE;
    }
    if(res1 == TreeResult::OUT_OF_MEMORY){
        return ALLOCATION_ERROR;
    }
    AVLNode<PlayerPointer>* new_player_node_ptr1 = nullptr;
    PlayerPointer new_player_ptr = PlayerPointer();
    new_player_ptr.player = &new_player;
    TreeResult res2 = playersByLevel->insertNode(&new_player_ptr, new_player_node_ptr1);
    if(res2 == TreeResult::OUT_OF_MEMORY){
        return ALLOCATION_ERROR;
    }
    new_player_node->getData()->player_level = new_player_node_ptr1;
    
    return addPlayerToGroup(&new_player, group, NonEmptyGroups);
}

StatusType PlayersManager::RemovePlayer(int PlayerID)
{
    if (PlayerID <= 0) return INVALID_INPUT;

    Player* player = playersById->findData(PlayerID);
    if (player == NULL) return FAILURE;

    // delete player from playersByLevel
    playersByLevel->deleteByPointer(player->player_level);

    // delete player from groupPlayers
    Group* playerGroup = player->getGroup();
    playerGroup->groupPlayers->deleteByPointer(player->group_player);
    playerGroup->highest_player = playerGroup->groupPlayers->getHighest();

    // if player's group has no more players, delete the group from NonEmptyGroups
    if (playerGroup->groupPlayers->getSize() == 0) {
        NonEmptyGroups->deleteByPointer(playerGroup->groupPointer);
        playerGroup->groupPointer = nullptr;
    }

    // delete player from playersById
    playersById->deleteNode(PlayerID);
    
    return SUCCESS;
}

StatusType PlayersManager::ReplaceGroup(int GroupID, int ReplacementID) {
    if(GroupID <= 0 || ReplacementID <= 0 || GroupID == ReplacementID){
        return INVALID_INPUT;
    }
    Group* group1 = groupTree->findData(GroupID);
    Group* group2 = groupTree->findData(ReplacementID);
    if(!group1 || !group2){
        return FAILURE;
    }
    if(!group1->getSize()){
        groupTree->deleteNode(GroupID);
        return SUCCESS;
    }
    int highest_level_player_id = group1->highest_player->player->getId();
    int highest_level_id2, highest_level1, highest_level2;
    highest_level1 = group1->highest_player->player->getLevel();
    if(!group2->getSize()){
        GroupPointer new_group_ptr = GroupPointer();
        new_group_ptr.group = group2;
        NonEmptyGroups->insertNode(&new_group_ptr, nullptr);
    }else{
        highest_level_id2 = group2->highest_player->player->getId();
        highest_level2 = group2->highest_player->player->getLevel();
        if(highest_level1 < highest_level2 || (highest_level1 == highest_level2 && highest_level_player_id > highest_level_id2)){
            highest_level_player_id = highest_level_id2;
        }
    }
    AVLTree<PlayerPointer>* tmp = mergeTrees(group1->groupPlayers, group2->groupPlayers);
    PlayerPointer** mergedArr = new PlayerPointer*[group1->getSize() + group2->getSize()];
    tmp->inorder(tmp->getRoot(), mergedArr, group1->getSize() + group2->getSize());
    for (int i = 0; i < group1->getSize() + group2->getSize(); ++i) {
        mergedArr[i]->player->updateGroup(group2);
    }
    delete[] mergedArr;
    delete group1->groupPlayers;
    delete group2->groupPlayers;
    group2->groupPlayers = tmp;
    group2->setSize(group1->getSize() + group2->getSize());
    groupTree->deleteNode(GroupID);
    NonEmptyGroups->deleteNode(GroupID);
    PlayerPointer new_highest_level_player = PlayerPointer();
    new_highest_level_player.player = playersById->findData(highest_level_player_id);
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
    
    return addPlayerToGroup(&updated_player, player_group, NonEmptyGroups);

}

StatusType PlayersManager::GetHighestLevel(int GroupID, int *PlayerID) {
    if(GroupID == 0 || !PlayerID){
        return INVALID_INPUT;
    }
    if(GroupID < 0){
        *PlayerID = playersByLevel->getHighest()->player->getId();
        return SUCCESS;
    }
    Group* group = groupTree->findData(GroupID);
    if(!group){
        return FAILURE;
    }
    if(!group->getSize()){
        *PlayerID = -1;
        return SUCCESS;
    }
    *PlayerID = group->highest_player->player->getId();
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
    GroupPointer** arr = new GroupPointer*[numOfGroups];
    if(!arr){
        return ALLOCATION_ERROR;
    }
    NonEmptyGroups->inorder(NonEmptyGroups->getRoot(), arr, numOfGroups);
    for (int i = 0; i < numOfGroups; ++i) {
        *Players[i] = arr[i]->group->highest_player->player->getId();
    }
    return SUCCESS;
}