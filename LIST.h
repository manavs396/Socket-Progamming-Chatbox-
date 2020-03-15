
/// LIST.h cmpt 300 assignment1 

#pragma once
#include <stdlib.h>    
#include <stdio.h>  

#ifndef DEBUG
#define list_max 10
#define node_max 40
#endif

typedef struct node {
    void *data;
    struct node *next;
    struct node *prev;
    int bool_flag;
    struct list *list_sim;  
} node;

typedef struct list {
    node *head;
    node *tail;
    node *curr;
    int nodeCount;
    int bool_flag;
} list;


list *ListCreate();
int ListCount(const list *temp);
void *ListFirst(list *temp);
void *ListLast(list *temp);
void *ListNext(list *temp);
void *ListPrev(list *temp);
void *ListCurr(list *temp);
int ListAdd(list *temp, void *value);
int ListInsert(list *temp, void *value);
int ListAppend(list *temp, void *value);
int ListPrepend(list *temp, void *value);
void *ListRemove(list *temp);
void ListConcat(list *temp1, list *temp2);
void ListFree(list *temp, void (*itemFree)());
void *ListTrim(list *temp);
void *ListSearch(list *temp, int (*comparator)(), void *comparisonArg);

