#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include "linked_list.h"

/* create a linked list and manages it */
linked_list* linked_list_create() {
	linked_list* list = (linked_list*)malloc(sizeof(linked_list));
	if (list == NULL) {
		printf("ERROR : Not enough memory");
		exit(1);
	}
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;

	return list;
}

//release the linked list from memory
void linked_list_release(linked_list* list) {
	while (list->size > 0)
		linked_list_removeFirst(list);

	free(list);
}

/** create a node */
linked_list_node* linked_list_create_node (void* item) {
	linked_list_node* node = (linked_list_node*) malloc (sizeof(linked_list_node));
	if (node == NULL) {
		printf("ERROR : Not enough memory");
		exit(1);
	}
	node->item = item;
	node->prev = NULL;
	node->next = NULL;
	return node;
}

/** add item to any position */
void linked_list_add (linked_list* list, void* item, int position) {
	linked_list_node *node = NULL, *newNode = NULL;
	int i;

	// index out of list size
	assert(position > list->size);

	// add to head
	if (position == 0) {
		linked_list_addFirst(list, item);
	} else if (position == list->size) {
		// add to tail
		linked_list_addLast(list, item);
	} else {
		// insert between head and tail

		node = list->head;
		i = 0;
		// loop until the position
		while (i < position) {
			node = node->next;
			i++;
		}
		// insert new node to position
		newNode = linked_list_create_node(item);
		linked_list_insertBefore(list, node, newNode);
		list->size++;
	}
}
/** add item to head */
void linked_list_addFirst (linked_list* list, void* item) {
	linked_list_node* newNode = linked_list_create_node(item);
	linked_list_node* head = list->head;
	// list is empty
	if (head == NULL)
		list->head = newNode;
	else { // has item(s)
		linked_list_node* last = list->tail;
		if (last == NULL) // only head node
			last = head;
		newNode->next = head;
		head->prev = newNode;
		list->head = newNode;
		list->tail = last;
	}

	list->size++;
}
/** add item to tail */
void linked_list_addLast (linked_list* list, void* item) {
	linked_list_node* newNode = linked_list_create_node(item);
	linked_list_node* head = list->head;
	linked_list_node* tail = list->tail;
	// list is empty
	if (head == NULL)
		list->head = newNode;
	else { // has item(s)
		linked_list_node* lastNode = tail;
		if (tail == NULL) // only head node
			lastNode = head;
		lastNode->next = newNode;
		newNode->prev = lastNode;
		list->tail = newNode;
	}
	list->size++;
}

/** insert one node before another,
 * newNdoe, node and node->prev should not be null.
 */
void linked_list_insertBefore (linked_list* list, linked_list_node* node, linked_list_node* newNode) {
	linked_list_node* prev = node->prev;

	node->prev = newNode;
	newNode->next = node;
	prev->next = newNode;
	newNode->prev = prev;
}
/** get item from specific position */
void* linked_list_get (linked_list* list, int position) {
	// list is empty
	assert(list->size > 0);
	assert(position < list->size);

	// get head item
	if (position == 0) {
		return linked_list_getFirst(list);
	} else if (position+1 == list->size) {
		// get tail item
		return linked_list_getLast(list);
	} else {
		linked_list_node* node = list->head;
		int i = 0;
		// loop until position
		while (i < position) {
			node = node->next;
			i++;
		}
		return node->item;
	}
}
/** get item from head  */
void* linked_list_getFirst (linked_list* list) {
	// list is empty
	assert(list->size > 0);
	return list->head->item;
}
/** get item from tail */
void* linked_list_getLast (linked_list* list) {
	// list is empty
	assert(list->size > 0);
	// only head node
	if (list->size == 1) {
		return linked_list_getFirst(list);
	}
	return list->tail->item;
}


/** get the position of the item. */
int linked_list_find(linked_list* list, void* item) {
	linked_list_node *p;
	int i;

	// list is empty
	assert(list->size > 0);

	p = list->head;
	i = 0;
	while (p != NULL && p->item != item) {
		++i;
		p = p->next;
	}

	if (i == list->size)
		return -1;

	return i;
}

/** remove item */
void linked_list_remove_item(linked_list* list, void* item) {
	int pos = linked_list_find(list, item);
	linked_list_remove(list, pos);
}

/** get item and remove it from any position */
void* linked_list_remove (linked_list* list, int position) {
	// list is empty
	assert(list->size > 0);
	assert(position < list->size);

	// remove from head
	if (position == 0) {
		return linked_list_removeFirst(list);
	} else if (position+1 == list->size) {
		// remove from tail
		return linked_list_removeLast(list);
	} else {
		linked_list_node* node = list->head;
		linked_list_node* prev;
		linked_list_node* next;
		int i = 0;
		void* item;
		// loop until position
		while (i < position) {
			node = node->next;
			i++;
		}
		item = node->item;
		// remove node from list
		prev = node->prev;
		next = node->next;
		prev->next = next;
		next->prev = prev;
		free(node);
		list->size--;
		return item;
	}
}
/** get and remove item from head */
void* linked_list_removeFirst (linked_list* list) {
	linked_list_node* head = list->head;
	linked_list_node* next;
	void* item;
	// list is empty
	assert(list->size > 0);
	assert(head != NULL);

	item = head->item;
	next = head->next;
	list->head = next;
	if (next != NULL) // has next item
		next->prev = NULL;
	free(head);
	list->size--;
	if (list->size <= 1) // empty or only head node
		list->tail = NULL;
	return item;
}
/** get and remove item from tail  */
void* linked_list_removeLast (linked_list* list) {
	// list is empty
	assert(list->size > 0);
	if (list->size == 1) { // only head node
		return linked_list_removeFirst(list);
	} else {
		linked_list_node* tail = list->tail;
		linked_list_node* prev = tail->prev;
		void* item = tail->item;
        free(tail);
		prev->next = NULL;
		if (list->size > 1)
			list->tail = prev;
		list->size--;
		if (list->size <= 1) // empty or only head node
			list->tail = NULL;
		return item;
	}
}

