#ifndef lcthw_List_algos_h
#define lcthw_List_algos_h

#ifndef lcthw_List_h
#include <lcthw/list.h>
#endif

typedef int (*List_compare) (ListNode *item1, ListNode *item2);

List *List_bubble_sort(List *list, List_compare cmp);
List *List_merge_sort(List *list, List_compare cmp);

#endif
