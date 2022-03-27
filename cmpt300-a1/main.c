#include "list.h"
#include <stdio.h>
#include "list.c"

void Free_item(void* pItem)
{
    pItem = NULL;
}

bool Search_item(void* pItem, void* pComparisonArg)
{
    if (pItem == pComparisonArg)
    {
        return true;
    }
    return false;
}

void print_list(List* pList)
{
    Node* current = pList->head;
    printf("Print list: ");
    while (current != NULL && current->item != NULL)
    {
        printf("%d \t", *(int*)current->item);
        current = current->next;
    }
    printf("\n");
}

int main(void)
{
    int arr[20];

    List* pList1 = List_create();
    List* pList2 = List_create();

    for (int i = 0; i < 15; i++){
        arr[i] = i;
        List_insert_after(pList1, &arr[i]);
        List_insert_before(pList1, &arr[i]);
        List_prepend(pList2, &arr[i]);
        List_append(pList2, &arr[i]);
    }


    print_list(pList1);
    print_list(pList2);

    List_concat(pList1, pList2);
    print_list(pList1);

    List_last(pList1);
    List_remove(pList1);
    print_list(pList1);

    List_prev(pList1);
    List_remove(pList1);
    print_list(pList1);

    List_first(pList1);
    List_remove(pList1);
    print_list(pList1);

    List_next(pList1);
    List_remove(pList1);
    print_list(pList1);

    List_trim(pList1);
    print_list(pList1);

    int a = 10;
    int b = 6;

    int* word1 = &a;
    int* word2 = &b;

    List_search(pList1, Search_item, word1);
    List_search(pList1, Search_item, word2);

    List_free(pList1, Free_item);


    return 0;
}
