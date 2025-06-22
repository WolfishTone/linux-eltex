#pragma once
#include<inttypes.h>

#define GR_BACK "\033[42m\033[30m"
#define DEF_COL "\033[0m"

struct SplayTree
{
	uint32_t key; // port
	char* value; // ip
	int count; // sended message count
	struct SplayTree* left;
	struct SplayTree* right;
	struct SplayTree* parent;
};


struct SplayTree* SplayTreeInsert(struct SplayTree* tree, uint32_t key, char* value);

struct SplayTree* SplayTreeLookup(struct SplayTree* tree, uint32_t key, char* value);

struct SplayTree* SplayTreeDelete(struct SplayTree* tree, uint32_t key, char* value);

struct SplayTree* SplayTreeMerge(struct SplayTree* tree, struct SplayTree* tree1);

void SplayTreeSplit(struct SplayTree** tree, struct SplayTree** tree1, uint32_t key, char* value);
void SplayTreeFree(struct SplayTree* tree);


void SplayTreePrint(struct SplayTree* tree, int layer);






