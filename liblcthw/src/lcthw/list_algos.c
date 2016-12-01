#include "dbg.h"
#include <lcthw/list.h>
#include <lcthw/list_algos.h>

List *List_bubble_sort(List *list, List_compare cmp)
{
    if (list->count < 2) return list;

    int n = list->count;
    while (n != 0) {
        int newn = 0;
        int i = 0;
        LIST_FOREACH(list, first, next, cur) {
            if (cur->next == NULL) break; // on last node

            if (cmp(cur->value, cur->next->value) > 0) {
                void *tmp = cur->value;
                cur->value = cur->next->value;
                cur->next->value = tmp;

                newn = i;
            }

            i++;
        }

        n = newn;
    }

    return list;
}

List *merge(List *left, List *right, List_compare cmp)
{
    List *result = List_create();

    while (left->count > 0 && right->count > 0) {
        if (cmp(left->first->value, right->first->value) < 0) {
            List_push(result, List_shift(left));
        }
        else {
            List_push(result, List_shift(right));
        }
    }

    while (left->count > 0) {
        List_push(result, List_shift(left));
    }

    while (right->count > 0) {
        List_push(result, List_shift(right));
    }
    
    return result;
}

List *List_merge_sort(List *list, List_compare cmp)
{
    List *result = NULL;

    if (list->count < 2)
        return list;

    List *left = List_create();
    List *right = List_create();

    int half = list->count / 2;
    LIST_FOREACH(list, first, next, cur) {
        if (half > 0) {
            List_push(left, cur->value);
        }
        else {
            List_push(right, cur->value);
        }

        half--;
    }

    List *sort_left = List_merge_sort(left, cmp);
    List *sort_right = List_merge_sort(right, cmp);

    if (sort_left != left)
        List_destroy(left);
    if (sort_right != right)
        List_destroy(right);

    result = merge(sort_left, sort_right, cmp);

    List_destroy(sort_left);
    List_destroy(sort_right);

    return result;
}
