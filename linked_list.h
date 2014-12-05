#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

/**
 * The node struct,
 * contains item and the pointers that point to previous node/next node.
 */
typedef struct linked_list_node_t {
	void* item;
	// previous node
	struct linked_list_node_t* prev;
	// next node
	struct linked_list_node_t* next;
} linked_list_node;

/**
 * The linked_list struct, contains the pointers that
 * point to first node and last node, the size of the linked_list,
 * and the function pointers.
 */
typedef struct linked_list_t {
	linked_list_node* head;
	linked_list_node* tail;

	// size of this linked_list
	int size;
} linked_list;

/**
 * ISO C forbids conversion of object pointer to function pointer type, since it the standard does not define the actual behavior in such cases.
 * in order to use a generic linked list to carry function pointers, we wrap them in generic structs containing generic function pointers.
 */
typedef struct opaque_fpointer_t
{
	void(*fp)(void);
} opaque_fpointer_t;

/** create a linked_list */
linked_list* linked_list_create();

/** release a linked list */
void linked_list_release(linked_list* list);

/** add item to any position */
void linked_list_add(linked_list* list, void* item, int position);

/** add item to head */
void linked_list_addFirst(linked_list* list, void* item);

/** add item to tail */
void linked_list_addLast(linked_list* list, void* item);

/** insert one node before another, * newNdoe, node and node->prev should not be null.  */
void linked_list_insertBefore(linked_list* list, linked_list_node* cur_node, linked_list_node* new_node);

/** get item from specific position */
void* linked_list_get(linked_list* list, int position);

/** get item from head */
void* linked_list_getFirst(linked_list* list);

/** get item from tail */
void* linked_list_getLast(linked_list* list);

/** get the position of the item. */
int linked_list_find(linked_list* list, void* item);

/** remove item */
void linked_list_remove_item(linked_list* list, void* item);

/** get item and remove it from any position */
void* linked_list_remove(linked_list* list, int position);

/** get and remove item from head */
void* linked_list_removeFirst(linked_list* list);

/** get and remove item from tail */
void* linked_list_removeLast(linked_list* list);

/** display the items in the list */
// void display(linked_list* list);

#endif

