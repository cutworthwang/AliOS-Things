/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _SYS_QUEUE_H_
#define _SYS_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define QMD_SAVELINK(name, link)
#define TRASHIT(x)

/*
 * Singly-linked List declarations.
 */
#define SLIST_HEAD(name, type)  \
    struct name {   \
        struct type *slh_first; /* first element */ \
    }

#define SLIST_HEAD_INITIALIZER(head)    \
    { NULL }

#define SLIST_ENTRY(type)   \
    struct {    \
        struct type *sle_next;  /* next element */  \
    }

/*
 * Singly-linked List functions.
 */
#define SLIST_EMPTY(head)   ((head)->slh_first == NULL)

#define SLIST_FIRST(head)   ((head)->slh_first)

#define SLIST_FOREACH(var, head, field) \
    for ((var) = SLIST_FIRST((head));   \
            (var);  \
            (var) = SLIST_NEXT((var), field))

#define SLIST_FOREACH_SAFE(var, head, field, tvar)  \
    for ((var) = SLIST_FIRST((head));   \
            (var) && ((tvar) = SLIST_NEXT((var), field), 1);    \
            (var) = (tvar))

#define SLIST_FOREACH_PREVPTR(var, varp, head, field)   \
    for ((varp) = &SLIST_FIRST((head)); \
            ((var) = *(varp)) != NULL;  \
            (varp) = &SLIST_NEXT((var), field))

#define SLIST_INIT(head) do {   \
        SLIST_FIRST((head)) = NULL; \
    } while (0)

#define SLIST_INSERT_AFTER(slistelm, elm, field) do {   \
        SLIST_NEXT((elm), field) = SLIST_NEXT((slistelm), field);   \
        SLIST_NEXT((slistelm), field) = (elm);  \
    } while (0)

#define SLIST_INSERT_HEAD(head, elm, field) do {    \
        SLIST_NEXT((elm), field) = SLIST_FIRST((head)); \
        SLIST_FIRST((head)) = (elm);    \
    } while (0)

#define SLIST_NEXT(elm, field)  ((elm)->field.sle_next)

#define SLIST_REMOVE(head, elm, type, field) do {   \
        QMD_SAVELINK(oldnext, (elm)->field.sle_next);   \
        if (SLIST_FIRST((head)) == (elm)) { \
            SLIST_REMOVE_HEAD((head), field);   \
        }   \
        else {  \
            struct type *curelm = SLIST_FIRST((head));  \
            while (SLIST_NEXT(curelm, field) != (elm))  \
                curelm = SLIST_NEXT(curelm, field); \
            SLIST_REMOVE_AFTER(curelm, field);  \
        }   \
        TRASHIT(*oldnext);  \
    } while (0)

#define SLIST_REMOVE_AFTER(elm, field) do { \
        SLIST_NEXT(elm, field) =    \
                SLIST_NEXT(SLIST_NEXT(elm, field), field);  \
    } while (0)

#define SLIST_REMOVE_HEAD(head, field) do { \
        SLIST_FIRST((head)) = SLIST_NEXT(SLIST_FIRST((head)), field);   \
    } while (0)

/*
 * Singly-linked Tail queue declarations.
 */
#define STAILQ_HEAD(name, type) \
    struct name {   \
        struct type *stqh_first;    /* first element */ \
        struct type **stqh_last;    /* addr of last next element */ \
    }

#define STAILQ_HEAD_INITIALIZER(head)   \
    { NULL, &(head).stqh_first }

#define STAILQ_ENTRY(type)  \
    struct {    \
        struct type *stqe_next; /* next element */  \
    }

/*
 * Singly-linked Tail queue functions.
 */
#define STAILQ_CONCAT(head1, head2) do {    \
        if (!STAILQ_EMPTY((head2))) {   \
            *(head1)->stqh_last = (head2)->stqh_first;  \
            (head1)->stqh_last = (head2)->stqh_last;    \
            STAILQ_INIT((head2));   \
        }   \
    } while (0)

#define STAILQ_EMPTY(head)  ((head)->stqh_first == NULL)

#define STAILQ_FIRST(head)  ((head)->stqh_first)

#define STAILQ_FOREACH(var, head, field)    \
    for((var) = STAILQ_FIRST((head));   \
            (var);  \
            (var) = STAILQ_NEXT((var), field))


#define STAILQ_FOREACH_SAFE(var, head, field, tvar) \
    for ((var) = STAILQ_FIRST((head));  \
            (var) && ((tvar) = STAILQ_NEXT((var), field), 1);   \
            (var) = (tvar))

#define STAILQ_INIT(head) do {  \
        STAILQ_FIRST((head)) = NULL;    \
        (head)->stqh_last = &STAILQ_FIRST((head));  \
    } while (0)

#define STAILQ_INSERT_AFTER(head, tqelm, elm, field) do {   \
        if ((STAILQ_NEXT((elm), field) = STAILQ_NEXT((tqelm), field)) == NULL)\
            (head)->stqh_last = &STAILQ_NEXT((elm), field); \
        STAILQ_NEXT((tqelm), field) = (elm);    \
    } while (0)

#define STAILQ_INSERT_HEAD(head, elm, field) do {   \
        if ((STAILQ_NEXT((elm), field) = STAILQ_FIRST((head))) == NULL) \
            (head)->stqh_last = &STAILQ_NEXT((elm), field); \
        STAILQ_FIRST((head)) = (elm);   \
    } while (0)

#define STAILQ_INSERT_TAIL(head, elm, field) do {   \
        STAILQ_NEXT((elm), field) = NULL;   \
        *(head)->stqh_last = (elm); \
        (head)->stqh_last = &STAILQ_NEXT((elm), field); \
    } while (0)

#define STAILQ_LAST(head, type, field)  \
    (STAILQ_EMPTY((head))?   \
     NULL : \
     ((struct type *)(void *)\
      ((char *)((head)->stqh_last) - __offsetof(struct type, field))))

#define STAILQ_NEXT(elm, field) ((elm)->field.stqe_next)

#define STAILQ_REMOVE(head, elm, type, field) do {  \
        QMD_SAVELINK(oldnext, (elm)->field.stqe_next);  \
        if (STAILQ_FIRST((head)) == (elm)) {    \
            STAILQ_REMOVE_HEAD((head), field);  \
        }   \
        else {  \
            struct type *curelm = STAILQ_FIRST((head)); \
            while (STAILQ_NEXT(curelm, field) != (elm)) \
                curelm = STAILQ_NEXT(curelm, field);    \
            STAILQ_REMOVE_AFTER(head, curelm, field);   \
        }   \
        TRASHIT(*oldnext);  \
    } while (0)

#define STAILQ_REMOVE_HEAD(head, field) do {    \
        if ((STAILQ_FIRST((head)) = \
                STAILQ_NEXT(STAILQ_FIRST((head)), field)) == NULL)  \
            (head)->stqh_last = &STAILQ_FIRST((head));  \
    } while (0)

#define STAILQ_REMOVE_AFTER(head, elm, field) do {  \
        if ((STAILQ_NEXT(elm, field) =  \
                STAILQ_NEXT(STAILQ_NEXT(elm, field), field)) == NULL)   \
            (head)->stqh_last = &STAILQ_NEXT((elm), field); \
    } while (0)

#define STAILQ_SWAP(head1, head2, type) do {    \
        struct type *swap_first = STAILQ_FIRST(head1);  \
        struct type **swap_last = (head1)->stqh_last;   \
        STAILQ_FIRST(head1) = STAILQ_FIRST(head2);  \
        (head1)->stqh_last = (head2)->stqh_last;    \
        STAILQ_FIRST(head2) = swap_first;   \
        (head2)->stqh_last = swap_last; \
        if (STAILQ_EMPTY(head1))    \
            (head1)->stqh_last = &STAILQ_FIRST(head1);  \
        if (STAILQ_EMPTY(head2))    \
            (head2)->stqh_last = &STAILQ_FIRST(head2);  \
    } while (0)

#define STAILQ_INSERT_CHAIN_HEAD(head, elm_chead, elm_ctail, field) do {   \
        if ((STAILQ_NEXT(elm_ctail, field) = STAILQ_FIRST(head)) == NULL ) { \
            (head)->stqh_last = &STAILQ_NEXT(elm_ctail, field);            \
        }                                                                      \
        STAILQ_FIRST(head) = (elm_chead);                                    \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* !_SYS_QUEUE_H_ */
