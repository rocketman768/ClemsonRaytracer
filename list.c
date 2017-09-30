#include "ray.h"

list_t* list_init()
{
    list_t* ret = (list_t*)malloc(sizeof(list_t));
    ret->head = NULL;
    return ret;
}

void list_add(list_t* list, obj_t* obj)
{
    if (list->head == NULL)
    {
        obj->next = NULL;
        list->head = obj;
    }
    else
    {
        obj->next = list->head;
        list->head = obj;
    }
}
