#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#define DEBUG(format, ...) \
    printf("[ %s:%d ] " format "", __FUNCTION__, __LINE__, ##__VA_ARGS__);
//#define DEBUG(format, ...)
//#define DEBUG(format, ...) printf("   "format" File:%s, Line:%d, Function:%s
//\n", ##__VA_ARGS__, __FILE__, __LINE__ , __FUNCTION__);


static bool _create_element(list_ele_t **new, char *s)
{
    /* 檢查 new 是否為 NULL，如果是 return false */
    *new = malloc(sizeof(list_ele_t));
    if (!*new)
        return false;
    (*new)->next = NULL;
    (*new)->value = 0;


    size_t str_len = strlen(s) + 1;
    (*new)->value = (char *) malloc(str_len * sizeof(char));
    if (!((*new)->value)) {
        free(*new);
        return false;
    }
    // snprintf( (*new)->value, str_len, "%s", s);
    memcpy((*new)->value, s, str_len);
    return true;
}


/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    /*
       man malloc: If size is 0, then malloc() returns either NULL...
       檢查 q 是否為 NULL，如果是 return NULL
    */
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;

    /* 初始化 head & tail 指向 NULL */
    q->head = q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;

    list_ele_t *e = q->head;
    while (e) {
        list_ele_t *next = e->next;
        free(e->value);
        free(e);
        e = next;
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;

    /* 檢查 _create_element 是否成功 */
    list_ele_t *newh;
    if (!_create_element(&newh, s)) {
        return false;
    }

    /* 將新增的 newh insert 到最前面 */
    newh->next = q->head;
    q->head = newh;

    /* 只有當 tail == NULL 才會紀錄 */
    if (!q->tail) {
        q->tail = newh;
    }
    ++(q->size);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;

    /* 檢查 _create_element 是否成功 */
    list_ele_t *newt;
    if (!_create_element(&newt, s)) {
        return false;
    }

    /*
     q head is     NULL : 將 head & tail 都指向 newt
     q head is not NULL : 將目前 tail->next 指向 newt
    */
    newt->next = NULL;
    if (!q->head) {
        q->head = newt;
        q->tail = newt;
    } else {
        q->tail->next = newt;
        q->tail = newt;
    }
    ++(q->size);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head)
        return false;

    if (sp) {
        size_t val_len = strlen(q->head->value);
        size_t realcpy = (bufsize - 1) > val_len ? val_len : (bufsize - 1);
        // snprintf( sp, bufsize, "%s", q->head->value);
        memcpy(sp, q->head->value, realcpy);
        *(sp + realcpy) = '\0';
    }

    list_ele_t *e = q->head;
    q->head = e->next;
    free(e->value);
    free(e);
    --(q->size);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q)
        return 0;
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
/*
static list_ele_t* reverse_list(list_ele_t* head) {
    if( !head || !head->next ) return head;
    list_ele_t* p       = reverse_list(head->next);
    head->next->next    = head;
    head->next          = NULL;
    return p;
}
*/

void q_reverse(queue_t *q)
{
    if (!q || !(q->head) || !(q->head->next))
        return;
    // q->tail = q->head;
    // q->head = reverse_list(q->head);

    list_ele_t *current = q->head;
    list_ele_t *next = current->next;
    list_ele_t *tmp = NULL;
    q->tail = q->head;
    q->tail->next = NULL;
    while (next) {
        tmp = next->next;
        next->next = current;
        current = next;
        next = tmp;
    }
    q->head = current;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
#if 1
static list_ele_t *merge_list(list_ele_t *list1, list_ele_t *list2)
{
    if (list1 == NULL)
        return list2;
    if (list2 == NULL)
        return list1;
    if (strcasecmp(list1->value, list2->value) < 0) {
        list1->next = merge_list(list1->next, list2);
        return list1;
    } else {
        list2->next = merge_list(list1, list2->next);
        return list2;
    }
}

static list_ele_t *find_mid(list_ele_t *head)
{
    list_ele_t *slow = head;
    list_ele_t *fast = head;
    list_ele_t *prev = NULL;
    while (fast && fast->next) {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }
    prev->next = NULL;
    return slow;
}

static list_ele_t *merge_sort(list_ele_t *head)
{
    if (head->next == NULL)
        return head;
    list_ele_t *mid = find_mid(head);
    list_ele_t *list1 = merge_sort(head);
    list_ele_t *list2 = merge_sort(mid);
    return merge_list(list1, list2);
}

void q_sort(queue_t *q)
{
    if (!q || !(q->head))
        return;
    q->head = merge_sort(q->head);
    while (q->tail->next) {
        q->tail = q->tail->next;
    }
}
#endif
