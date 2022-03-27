#include "list.h"
#include <stdio.h>

static List lists[LIST_MAX_NUM_HEADS];
static bool max_list[LIST_MAX_NUM_HEADS];
static Node nodes[LIST_MAX_NUM_NODES];
static bool max_nodes[LIST_MAX_NUM_NODES];

// Makes a new, empty list, and returns its reference on success.
// Returns a NULL pointer on failure.
List* List_create() {
    List* current = NULL;
    //find the empty list
    for(int i = 0; i < LIST_MAX_NUM_HEADS; i++){
        if(!max_list[i]){
            max_list[i]=true;
            current = &lists[i];
            current->index = i;
            break;
        }
    }
    //initialize the list
    if(current){
        current->current_item = LIST_OOB_START;
        current->total = 0;
        current->head = NULL;
        current->now = NULL;
        current->tail = NULL;
    }
    return current;
}

// Returns the number of items in pList.
int List_count(List* pList){
    return pList->total;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_first(List* pList){
    //set current be head
    pList->now = pList->head;

    if( pList->head == NULL){
        pList->current_item = LIST_OOB_START;
        return NULL;
    }
    return pList->now->item;
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_last(List* pList) {
    // set current be tail
    pList->now = pList->tail;

    if (pList->tail == NULL) {
        pList->current_item = LIST_OOB_END;
        return NULL;
    }
    return pList->now->item;
}

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer
// is returned and the current item is set to be beyond end of pList.
void* List_next(List* pList){

    if(pList->now!= NULL){ // find whether the current is  tail
        pList->now=pList->now->next;
        if(pList->now == NULL){
            pList->current_item = LIST_OOB_END;
            return NULL;
        }
        return pList->now->item;
    }

    if(pList->now == NULL){ // find whether the current is head
        if(pList->current_item == LIST_OOB_START){
            pList->now = pList->head;
            return pList->now->item;
        }
        else{
            return NULL;
        }
    }
    return pList->now->item;
}

// Backs up pList's current item by one, and returns a pointer to the new current item.
// If this operation backs up the current item beyond the start of the pList, a NULL pointer
// is returned and the current item is set to be before the start of pList.
void* List_prev(List* pList){

    if(pList->now != NULL){ // find whether the current is head
        pList->now = pList->now->pre;
        if(pList->now == NULL){
            pList->current_item = LIST_OOB_START;
            return NULL;
        }
        return pList->now->item;
    }

    if(pList->now == NULL){ // find whether the current is tail
        if(pList->current_item == LIST_OOB_END){
            pList->now = pList->tail;
            return pList->now->item;
        }
        else{
            return NULL;
        }
    }
    return pList->now->item;
}

// Returns a pointer to the current item in pList.
void* List_curr(List* pList){
    if(pList->now != NULL){
        return pList->now->item;
    }
    return NULL;
}

static Node* node_create(void* pItem){
    Node* current = NULL;
    //find the empty node array
    for(int i = 0; i < LIST_MAX_NUM_NODES; i++){
        if(!max_nodes[i]){
            current = &nodes[i];
            max_nodes[i]=true;
            current->index = i;
            break;
        }
    }
    //initialize the node
    if(current){
        current->item = pItem;
        current->next = NULL;
        current->pre = NULL;
    }
    return current;
}

static int list_add(List* pList, void* pItem){

    Node* node = node_create(pItem);
    if(!node){
        return -1;
    }

    if(pList->total == 0){ //for empty list
        pList->tail = node;
        pList->head = node;
    }
    else if(pList->now == NULL){ // for current beyond the list
        if(pList->current_item == LIST_OOB_START){
            pList->head->pre = node;
            node->next = pList->head;
            node->pre = NULL;
            pList->head = node;
        }
        else if(pList->current_item == LIST_OOB_END){
            node->pre = pList->tail;
            pList->tail->next = node;
            pList->tail = node;
            node->next = NULL;
        }
    }
    else if(pList->tail == pList->now){ // for current at the tail
        pList->tail->next = node;
        node->pre = pList->tail;
        node->next = NULL;
        pList->tail = pList->tail->next;
    }
    else{ // for current in the middle
        node->next = pList->now->next;
        node->pre = pList->now;
        pList->now->next->pre = node;
        pList->now->next = pList->now->next->pre;
    }
    pList->now = node;
    pList->total++;
    return 0;
}
// Adds the new item to pList directly after the current item, and makes item the current item.
// If the current pointer is before the start of the pList, the item is added at the start. If
// the current pointer is beyond the end of the pList, the item is added at the end.
// Returns 0 on success, -1 on failure.
int List_insert_after(List* pList, void* pItem){
    return list_add(pList,pItem);
}

// Adds item to pList directly before the current item, and makes the new item the current one.
// If the current pointer is before the start of the pList, the item is added at the start.
// If the current pointer is beyond the end of the pList, the item is added at the end.
// Returns 0 on success, -1 on failure.
int List_insert_before(List* pList, void* pItem){
    List_prev(pList);
    return list_add(pList,pItem);
}

// Adds item to the end of pList, and makes the new item the current one.
// Returns 0 on success, -1 on failure.
int List_append(List* pList, void* pItem){
    List_last(pList);
    return list_add(pList,pItem);
}

// Adds item to the front of pList, and makes the new item the current one.
// Returns 0 on success, -1 on failure.
int List_prepend(List* pList, void* pItem){
    List_first(pList);
    List_prev(pList);
    return list_add(pList,pItem);
}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void* List_remove(List* pList){
    if(pList->now == NULL ){
        return NULL;
    }

    Node* pre = pList->now->pre;
    Node* next = pList->now->next;

    if (next != NULL) {
        next->pre = pre;
    }
    else {// determine whether the current is tail
        pList->tail = pre;
    }
    if (pre != NULL) {
        pre->next = next;
    }
    else { // determine whether the current is head
        pList->head = next;
    }

    void* item = pList->now->item;
    max_nodes[pList->now->index] = false; // free the node
    pList->now = next;

    if (pList->now == NULL) { // determine whether it is the last one
        pList->current_item = LIST_OOB_END;
    }

    pList->total--;
    return item;
}

// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void* List_trim(List* pList){
    if(pList == NULL){
        return NULL;
    }

    List_last(pList);
    return List_remove(pList);
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1.
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List* pList1, List* pList2){

    if (pList1->tail == NULL) { // if pList1 is empty
        pList1->total = pList2->total;
        pList1->tail = pList2->tail;
        pList1->head = pList2->head;
    }
    else if (pList2->total > 0){ //if pList2 is not empty
        pList1->tail->next = pList2->head;
        pList2->head->pre = pList1->tail;
        pList1->tail = pList2->tail;
        pList1->total += pList2->total;
    }

    max_list[pList2->index] = false;//free the pList2
}

// Delete pList. pItemFreeFn is a pointer to a routine that frees an item.
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are
// available for future operations.
void List_free(List* pList, FREE_FN pItemFreeFn){
    Node* copy = pList->head;

    while(copy!=NULL){ // free the node one by one
        if(pItemFreeFn){
            pItemFreeFn(copy->item);
        }
        max_nodes[copy->index] = false;
        copy = copy->next;
    }

    max_list[pList->index] = false; // then free the list
}

// Search pList, starting at the current item, until the end is reached or a match is found.
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match,
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator.
//
// If a match is found, the current pointer is left at the matched item and the pointer to
// that item is returned. If no match is found, the current pointer is left beyond the end of
// the list and a NULL pointer is returned.
//
// If the current pointer is before the start of the pList, then start searching from
// the first node in the list (if any).
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){

    Node* copy = pList->now;

    if(pList->now == NULL){ // if null or oob, return null
        if(pList->current_item == LIST_OOB_START){
            copy = pList->head;
        }
    }

    while(copy != NULL){ // find the node from beginning
        if(copy->item == pComparisonArg){
            return copy->item;
        }
        copy = copy->next;
    }
    return NULL;
}

