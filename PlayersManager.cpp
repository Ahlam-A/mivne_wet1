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
	playerTree = new AVLTree<Player>();
	playerLevels = new AVLTree<PlayerPointer>();
}

PlayersManager::~PlayersManager()
{
	delete groupTree;
	delete NonEmptyGroups;
	delete playerTree;
	delete playerLevels;
}
