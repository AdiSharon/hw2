#include "list_mtm1.h"
#include <stdlib.h>


struct list_t {
    ListElement element; //this is the iterartor
    CopyListElement copy_function;
    FreeListElement free_function;
    List previous;
    List next;
};


List listCreate(CopyListElement copyElement, FreeListElement freeElement){
    if (copyElement == NULL || freeElement == NULL){
        return NULL;
    }
    List new_list = malloc(sizeof(List));
    if(!new_list){
        return NULL;
    }
    new_list->copy_function = copyElement;
    new_list->free_function = freeElement;
    new_list->element = NULL;
    new_list->next = NULL;
    new_list->previous = NULL;
    return new_list;
}


List listCopy(List list){
    if(!list){
        return NULL;
    }
    List previous = NULL;
    List new = NULL;
    ListElement save_place = listGetCurrent(list);
    int count = 0;
    LIST_FOREACH(ListElement, list_element, list){
        new = listCreate(list->copy_function, list->free_function);
        if(new == NULL){
            listClear(previous);
            listDestroy(previous);
            return NULL;
        }
        if(list_element != save_place){
            count++;
        }
        new->element = list->copy_function(list_element);
        new->previous = previous;
        previous = new;
    }
    //return to original node in new list
    for (ListElement element = listGetFirst(new); count != 0; count-- ){
        element = listGetNext(new);
        if(element == NULL){
            listClear(new);
            listDestroy(new);
            return NULL;
        }
    }
    return new;
}


int listGetSize(List list){
    if(list == NULL){
        return -1;
    }
    int size = 0;
    LIST_FOREACH(ListElement, curr_element, list){
        size++;
    }
    return size;
}


ListElement listGetFirst(List list){
    if(list == NULL){
        return NULL;
    }
    if(list->element == NULL){
        return NULL;
    }
    while (list->previous){
        list = list->previous;
    }
    return list->element;
}


ListElement listGetNext(List list){
    if(list == NULL){
        return NULL;
    }
    if(list->element == NULL || list->next == NULL){
        return NULL;
    }
    list = list->next;
    return list->element;
}


ListElement listGetCurrent(List list){
    if(list == NULL){
        return NULL;
    }
    if(list->element == NULL){
        return NULL;
    }
    return list->element;
}


ListResult listInsertFirst(List list, ListElement element){
    if(list == NULL || element == NULL){
        return LIST_NULL_ARGUMENT;
    }
    ListElement save = list->element;
    listGetFirst(list); //sets list iterator to head
    List new_head = listCreate(list->copy_function, list->free_function);
    if(new_head == NULL){
        return LIST_OUT_OF_MEMORY;
    }
    new_head->element = list->copy_function(element);
    if(new_head->element == NULL){
        return LIST_OUT_OF_MEMORY;
    }
    list->previous = new_head;
    new_head->next = list;
    for (ListElement iterator = listGetFirst(list); iterator;
         iterator= listGetNext(list)){
        if(iterator == save){
            break;
        }
    }
    return LIST_SUCCESS;
}

/**
* Adds a new element to the list, the new element will be the last element
*
* @param list The list for which to add an element in its end
* @param element The element to insert. A copy of the element will be
* inserted as supplied by the copying function which is stored in the list
* @return
* LIST_NULL_ARGUMENT if a NULL was sent as list
* LIST_OUT_OF_MEMORY if an allocation failed (Meaning the function for copying
* an element failed)
* LIST_SUCCESS the element has been inserted successfully
*/
ListResult listInsertLast(List list, ListElement element){
    if(list == NULL || element == NULL){
        return LIST_NULL_ARGUMENT;
    }
    ListElement save = list->element;
    List new_end = listCreate(list->copy_function, list->free_function);
    if(new_end == NULL){
        return LIST_OUT_OF_MEMORY;
    }
    new_end->element = list->copy_function(element);
    if(new_end->element == NULL){
        return LIST_OUT_OF_MEMORY;
    }
    LIST_FOREACH(ListElement, i, list); //set iterator to list end
    list->next = new_end;
    new_end->previous = list;
    for (ListElement iterator = listGetFirst(list); iterator;
         iterator= listGetNext(list)){
        if(iterator == save){
            break;
        }
    }
    return LIST_SUCCESS;
}


ListResult listInsertBeforeCurrent(List list, ListElement element){
    if(list == NULL ){
        return LIST_NULL_ARGUMENT;
    }
    if(element == NULL){
        return LIST_INVALID_CURRENT;
    }
    List new_node = listCreate(list->copy_function, list->free_function);
    if(!new_node){
        return LIST_OUT_OF_MEMORY;
    }
    new_node->element = list->copy_function(element);
    if(!new_node->element){
        return LIST_OUT_OF_MEMORY;
    }
    List previous = list->previous;
    if(previous != NULL){
        previous->next = new_node;
    }
    new_node->previous = previous;
    list->previous = new_node;
    new_node->next = list;
    return LIST_SUCCESS;
}


ListResult listInsertAfterCurrent(List list, ListElement element){
    if(list == NULL ){
        return LIST_NULL_ARGUMENT;
    }
    if(element == NULL){
        return LIST_INVALID_CURRENT;
    }
    List new_node = listCreate(list->copy_function, list->free_function);
    if(!new_node){
        return LIST_OUT_OF_MEMORY;
    }
    new_node->element = list->copy_function(element);
    if(!new_node->element){
        return LIST_OUT_OF_MEMORY;
    }
    List next = list->next;
    if(next != NULL){
        next->previous = new_node;
    }
    new_node->next = next;
    new_node->previous = list;
    list->next = new_node;
    return LIST_SUCCESS;
}

ListResult listRemoveCurrent(List list){
    if (list == NULL){
        return LIST_NULL_ARGUMENT;
    }
    if (list->element == NULL){
        return LIST_INVALID_CURRENT;
    }
    (list->previous)->next = list->next;
    (list->next)->previous = list->previous;
    list->free_function(listGetCurrent(list));
    list->free_function = NULL;
    list->copy_function = NULL;
    list->next = NULL;
    list->previous = NULL;
    free(list);
    return LIST_SUCCESS;
}


ListResult listSort(List list, CompareListElements compareElement){
    if(list == NULL || compareElement == NULL){
        return LIST_NULL_ARGUMENT;
    }
    bool sorted = false;
    while(!sorted){
        sorted = true;
        listGetFirst(list); //move iterator to head
        for (ListElement iterator = listGetCurrent(list->next); iterator;
             iterator = listGetNext(list)){//loop starts from 2nd
            if(compareElement((list->previous)->element, iterator) < 0){ //1 > 2
                sorted = false;
                ListResult result = listInsertBeforeCurrent(list, iterator);
                if (result != LIST_SUCCESS){
                    return result;
                }
                result = listRemoveCurrent(iterator);
                if(result != LIST_SUCCESS){
                    return result;
                }
            }
        }
    }
    return LIST_SUCCESS;
}


List listFilter(List list, FilterListElement filterElement, ListFilterKey key){
    if(list == NULL || filterElement == NULL){
        return NULL;
    }
    List filtered = listCreate(list->copy_function, list->free_function);
    if(!filtered){
        return NULL;
    }
    ListResult result;
    LIST_FOREACH(ListElement, iterator, list){
        if(filterElement(iterator, key)){
            result = listInsertAfterCurrent(filtered, iterator);
            if(result != LIST_SUCCESS){
                listClear(filtered);
                listDestroy(filtered);
                return NULL;
            }
        }
    }
    return filtered;
}


ListResult listClear(List list){
    if(list == NULL){
        return LIST_NULL_ARGUMENT;
    }
    LIST_FOREACH(ListElement, iterator, list){
        list->free_function(iterator);
        list->element = NULL;
    }
    return LIST_SUCCESS;
}


void listDestroy(List list){
    if(list == NULL){
        return;
    }
    listClear(list);
    LIST_FOREACH(ListElement, iterator, list){
        listRemoveCurrent(list);
    }
}