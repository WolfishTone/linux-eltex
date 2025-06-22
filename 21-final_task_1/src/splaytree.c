#include <splayTree.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>


int comparator(int key1, int key2, char* val1, char* val2)
{
	if(key1 == key2)
	{
		return strncmp(val1, val2, 4);
	}
	return key1 > key2? 1: -1;
}

struct SplayTree* createNode (uint32_t key, char* value, struct SplayTree* parent) // создание элемента
{
	struct SplayTree* newNode= malloc(sizeof(struct SplayTree));
	if(newNode)
	{
		newNode->key= key;
		newNode->value= value;
		newNode->count= 0;
		newNode->right= 0;
		newNode->left=0;
		newNode->parent= parent;
	}
	return newNode;
}

struct SplayTree* minNode(struct SplayTree* tree)
{
	struct SplayTree* buf= 0;
	while(tree)
	{
		buf= tree;
		tree=tree->left;
	}
	return buf;
}

struct SplayTree* maxNode(struct SplayTree* tree)
{
	struct SplayTree* buf= 0;
	while(tree)
	{
		buf= tree;
		tree=tree->left;
	}
	return buf;
}

struct SplayTree* zig(struct SplayTree* x, struct SplayTree* p) // поворот вправо
{
	p->left= x->right;
	if(p->left)
		p->left->parent= p;
	x->right= p;
	x->parent= p->parent;
	p->parent= x;
	if(x->parent && (p == x->parent->left))
		x->parent->left= x;
	if(x->parent && p == x->parent->right)
		x->parent->right= x;

	return x; // новый самый верхний элемент
}

struct SplayTree* zag(struct SplayTree* x, struct SplayTree* p) // поворот влево
{
	p->right= x->left;
	if(p->right)
		p->right->parent= p;
	x->left= p;
	x->parent= p->parent;
	p->parent= x;
	if(x->parent && p == x->parent->left)
		x->parent->left= x;
	if(x->parent && p == x->parent->right)
		x->parent->right= x;
	

	return x; // новый самый верхний элемент
}

struct SplayTree* splay(struct SplayTree* x)
{
	if(!x)
		return 0;
	struct SplayTree* p= x->parent;
	if(!p)// узел уже и так корень
		return x;
	else
	{
		struct SplayTree* g= p->parent;
		if(g)
		{
			if(g->left== p)
			{
				if(p->left== x) //зиг-зиг
				{
					p= zig(p, g);
					x= zig(x, p);
				}
				if(p->right== x) // заг-зиг
				{
					x= zag(x, p);
					x= zig(x, g);
				}
			}
			else if(g->right== p)
			{
				if(p->left== x) //зиг-заг
				{
					x= zig(x, p);
					x= zag(x, g);
				}
				if(p->right== x) // заг-заг
				{
					p= zag(p, g);
					x= zag(x, p);
				}
			}
		}
		else if(p->left== x) // если g нет, то зиг или заг
			x= zig(x, p);
		else if(p->right== x)
			x= zag(x, p);
	}
	x= splay(x);
	return x;
}

struct SplayTree* SplayTreeInsert(struct SplayTree* tree, uint32_t key, char* value)
{
	if (!tree)
		return createNode(key, value, 0);
	
	struct SplayTree* parent;
	while(tree)
	{
		if(comparator(key, tree->key, value, tree->value) == 0)
			return splay(tree);	
		
		parent= tree;
		if(comparator(key, tree->key, value, tree->value) > 0)
			tree = tree->right;
		else
			tree = tree->left;
	}
	struct SplayTree* newNode= createNode(key, value, parent);
	if(comparator(key, parent->key, value, parent->value) > 0)
		parent->right= newNode;
	else
		parent->left= newNode;
	return splay(newNode);
}

struct SplayTree* SplayTreeLookup(struct SplayTree* tree, uint32_t key, char* value)
{
	while(tree)
	{
		if(comparator(key, tree->key, value, tree->value) == 0)
			return splay(tree); //делаем корнем найденый элемент
		
		if(comparator(key, tree->key, value, tree->value) < 0)
			tree= tree->left;
		else
			tree= tree->right;
	}
	return tree;
}

struct SplayTree* SplayTreeMerge(struct SplayTree* tree, struct SplayTree* tree1) // объединение двух деревьев
{
	if(!tree)
		return tree1? tree1: 0;
	if(!tree1)
		return tree? tree: 0;
		
	struct SplayTree* min= minNode(tree);
	struct SplayTree* min1= minNode(tree1);
	
	tree= splay(min);
	tree1= splay(min1);
	
	if(min->key < min1->key)
	{
		tree1->left= tree;
		tree->parent= tree1;
		return tree1;
	}
	tree->left= tree1;
	tree1->parent= tree;
	return tree;
}

void SplayTreeSplit(struct SplayTree** tree, struct SplayTree** tree1, uint32_t key, char* value)	 // разделение дерева на два
{
	struct SplayTree* tre1= 0;
	struct SplayTree* tre = SplayTreeLookup(*tree, key, value);
	if(tre)
	{
		if(!tre->left)
		{
			tre1= tre->right;
			tre->right= 0;
		}
		else
		{
			tre1= tre->left;
			tre->left= 0;
		}
		if(tre1)
			tre1->parent= 0;	
		*tree= tre;
		*tree1= tre1;
	}
}

struct SplayTree* SplayTreeDelete(struct SplayTree* tree, uint32_t key, char *value)
{
	if(!tree)
		return 0;
		
	struct SplayTree* tree1=0;	
	SplayTreeSplit(&tree, &tree1, key, value);
	
	if(comparator(key, tree->key, value, tree->value) == 0)
		tree= (!tree->left)?tree->right: tree->left;
	else if(tree1)
		tree1= (!tree1->left)?tree1->right: tree1->left;
		
	return  SplayTreeMerge(tree, tree1);
}

void SplayTreeFree(struct SplayTree* tree)
{
	if(tree)
	{
		SplayTreeFree(tree->left);
		SplayTreeFree(tree->right);
		free(tree);
	}
}

void SplayTreePrint(struct SplayTree* tree, int layer) // n+v
{
	if (tree!= 0)
	{
		SplayTreePrint(tree->right, layer+1);
		for(int i= 0; i< layer; i++)
			printf("\t   ");
		printf("%s k=%d v=%u.%u.%u.%u c=%d", GR_BACK, tree->key, tree->value[0], 
			tree->value[1], tree->value[2], tree->value[3], tree->count);
		if(tree->left)
			printf(" l=%d ", tree->left->key);
		if(tree->right)
			printf(" r=%d ", tree->right->key);
		if(tree->parent)
			printf(" p=%d ", tree->parent->key);
		printf("%s\n", DEF_COL);
		SplayTreePrint(tree->left, layer+1);
	}
}



/*int main()
{
	struct SplayTree* clients_tree = 0;

	
	char ip[4];
	ip[0] = 127; ip[1] = 0; ip[2] = 0; ip[3] = 1;
	clients_tree = SplayTreeInsert(clients_tree, 777, ip);
	SplayTreePrint(clients_tree, 0);

	char ip1[4];
	ip1[0] = 127; ip1[1] = 0; ip1[2] = 0; ip1[3] = 1;
	clients_tree = SplayTreeInsert(clients_tree, 7771, ip1);
	SplayTreePrint(clients_tree, 0);

	clients_tree = SplayTreeInsert(clients_tree, 777, ip);
	SplayTreePrint(clients_tree, 0);
	printf("=================================\n\n");

	clients_tree = SplayTreeInsert(clients_tree, 7771, ip1);
	SplayTreePrint(clients_tree, 0);
	printf("=================================\n\n");

	clients_tree = SplayTreeInsert(clients_tree, 7771, ip1);
	SplayTreePrint(clients_tree, 0);
	printf("=================================\n\n");

	clients_tree = SplayTreeInsert(clients_tree, 777, ip);
	SplayTreePrint(clients_tree, 0);
	printf("=================================\n\n");

	clients_tree = SplayTreeInsert(clients_tree, 777, ip);
	SplayTreePrint(clients_tree, 0);
	printf("=================================\n\n");

	clients_tree = SplayTreeInsert(clients_tree, 7771, ip1);
	SplayTreePrint(clients_tree, 0);
	printf("=================================\n\n");

	clients_tree = SplayTreeInsert(clients_tree, 7771, ip1);
	SplayTreePrint(clients_tree, 0);
	printf("=================================\n\n");

	clients_tree = SplayTreeInsert(clients_tree, 777, ip);
	SplayTreePrint(clients_tree, 0);
	printf("=================================\n\n");


	SplayTreeFree(clients_tree);
	clients_tree = 0;
	SplayTreePrint(clients_tree, 0);

	return 0;
}*/
