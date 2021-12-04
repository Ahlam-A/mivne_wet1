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

static AVLTree<PlayerPointer>* arrayToTree(PlayerPointer** sortedLabelArray, int size)
{
	AVLNode<PlayerPointer>* root = arrayToTreeAux(sortedLabelArray, 0, size - 1);
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

static AVLTree<PlayerPointer>* mergeLabelTrees(AVLTree<PlayerPointer>* tree1, AVLTree<PlayerPointer>* tree2)
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

StatusType PlayersManager::AddPlayer(int PlayerID, int GroupID, int Level) {
    if(PlayerID <= 0 || GroupID <= 0 || Level < 0){
        return INVALID_INPUT;
    }
    Group* group = groupTree->findData(GroupID);
    if(!group || playersById->findData(PlayerID)){
        return FAILURE;
    }
    AVLNode<Player>* new_player_node = nullptr;
    Player new_player(PlayerID, GroupID, Level);
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
    new_player.player_level = new_player_node_ptr1;
    AVLNode<PlayerPointer>* new_player_node_ptr2 = nullptr;
    TreeResult res3 = group->groupPlayers->insertNode(&new_player_ptr, new_player_node_ptr2);
    if(res3 == TreeResult::OUT_OF_MEMORY){
        return ALLOCATION_ERROR;
    }
    new_player.group_player = new_player_node_ptr2;
    if(!group->getSize()){
        GroupPointer new_nonEmptyGroup = GroupPointer();
        new_nonEmptyGroup.group = group;
        NonEmptyGroups->insertNode(&new_nonEmptyGroup, nullptr);
    }
    group->increaseSize();
    return SUCCESS;
}