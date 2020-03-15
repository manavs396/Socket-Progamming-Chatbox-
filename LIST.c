
/// LIST.c CMPT 300 assignment 1
#pragma once


#include "LIST.h"

list list_array[list_max];  //intializing listarray
node node_array[node_max+2]; // intializing nodearray
void init(){
for(int i=0;i<list_max;i++){
    list_array[i].head=NULL;
    list_array[i].tail=NULL;
    list_array[i].curr=NULL;
    list_array[i].nodeCount=0;
    list_array[i].bool_flag=0;
    }
}

int list_last = 0; // tracking no. of list used
int count_node = 0;// tracking no. of used node


// intializing the nodearray elemets by using veradic tempelate
node node_array[node_max + 2] = {[0 ... node_max + 1].data=NULL,
       [0 ... node_max + 1].next=NULL, [0 ... node_max + 1].prev=NULL, [0 ...
        node_max + 1].list_sim=NULL,
        [0 ... node_max - 1].bool_flag=0, [node_max ...
        node_max + 1].bool_flag=1};



 // using temp_list as a data structure in the form of stack, using array based stack
list *temp_list[list_max] = {[0 ... list_max - 1]=NULL};
int num_templist = 0;

// helper function to check node is intialized 
int node_intialized(list *temp){
    int k=0;
    if(!temp || !temp->bool_flag || !temp->head != !temp->tail){
        k=-1;
    }
    return k;
}
// makes a new, empty list, and returns if there is a space of making new AS success. Returns a NULL pointer on failurre
list *ListCreate() {
    
    if(list_last < list_max){
    if (num_templist) 
    {
        list *returnList = temp_list[num_templist - 1];
        returnList->bool_flag = 1;
        temp_list[num_templist - 1] = NULL;
        num_templist--;
        return returnList;
    } else {
        list_array[list_last].bool_flag = 1;
        return &(list_array[list_last++]);
    }
}
else
return NULL;
}

//count no. of list 
int ListCount(const list *temp) {
    
    int k=node_intialized(temp);
    if (k==-1)
        return -1;
    return temp->nodeCount;
}

// returns a pointer to the first item in list and makes the first item the current item
void *ListFirst(list *temp) {
    int k=node_intialized(temp); //checking for unitilaization 
    if (k==-1)
        return NULL;

    temp->curr = temp->head;
    // checking curr out of bound around head 
    if(temp->head && temp->head->list_sim == temp){
       return temp->head->data;}
    else
      return NULL;
}


// returns a pointer to the last item in list and makes the last item the current one.
void *ListLast(list *temp) {
    int k=node_intialized(temp);
    if (k==-1)
        return NULL;

    temp->curr = temp->tail;
    // checking curr out of bound around tail
    if(temp->tail && temp->tail->list_sim == temp) 
    return temp->tail->data;
    else
    return NULL;
}

// advances list's current item by one, and returns a pointer to the new current item. If this operation advances the current item beyond the end of the list, a NULL pointer is returned
void *ListNext(list *temp) {
    int k=node_intialized(temp);
    // caution check 
    if (k==-1 || !temp->curr || !temp->curr->bool_flag ||
        ((temp->curr < node_array || temp->curr >= node_array + node_max + 2) && temp->curr))
        return NULL;

    if ((temp->head && temp->head->list_sim != temp) || (temp->tail && temp->tail->list_sim != temp) ||
        (temp->curr >= node_array && temp->curr < node_array + node_max &&
         temp->curr->list_sim != temp))
        return NULL;
    
    void *val_ret; // temp variable to return
 

    
      // checking out of bound for curr 
     if (temp->curr == node_array + node_max + 1) {
        val_ret = NULL;}
    else if (temp->curr == node_array + node_max) {
        temp->curr = temp->head;
        if (temp->head)
            val_ret = temp->head->data;
        else
            val_ret = NULL;
    }
    else if (temp->curr == node_array + node_max + 1) {
        val_ret = NULL;
    } else {
        temp->curr = temp->curr->next;
        val_ret = temp->curr->data;
    }
    return val_ret;
}

//backs up list's current item by one, and returns a pointer to the new current item. If this operation backs up the current item beyond the start of the list, a NULL pointer is returned.
void *ListPrev(list *temp) {
    
    int k=node_intialized(temp);
    if (k==-1 || !temp->curr || !temp->curr->bool_flag ||
        ((temp->curr < node_array || temp->curr >= node_array + node_max + 2) && temp->curr))
        return NULL;
    if ((temp->head && temp->head->list_sim != temp) || (temp->tail && temp->tail->list_sim != temp) ||
        (temp->curr >= node_array && temp->curr < node_array + node_max &&
         temp->curr->list_sim != temp))
        return NULL;

   
    void *val_ret;
    // check out of bound of curr around head
    
    if (temp->curr == temp->head) {
        temp->curr = node_array + node_max;
        val_ret = NULL;
    }
    else if (temp->curr == node_array + node_max + 1) {
        temp->curr = temp->tail;
        if (temp->tail)
            val_ret = temp->tail->data;
        else
            val_ret = NULL;
    }
    else if (temp->curr == node_array + node_max) {
        val_ret = NULL;
    } else {
        temp->curr = temp->curr->prev;
        val_ret = temp->curr->data;
    }
    return val_ret;
}

//  returns a pointer to the current item in list
void *ListCurr(list *temp) {
    if (temp && temp->bool_flag && !(!temp->head != !temp->tail) && temp->curr && temp->curr->bool_flag &&temp->curr >= node_array && temp->curr < node_array + node_max && temp->curr->list_sim == temp)
       return temp->curr->data; 
   else
    return NULL;
}

//dds the new item to list directly after the current item, and makes item the current item. If the current pointer is before the start of the list, the item is added at the start. If the current pointer is beyond the end of the list, the item is added at the end. Returns 0 on success, -1 on failure.
int ListAdd(list *temp, void *value) {

    int k=node_intialized(temp);
    if (k==-1)
        return -1;

    if ((temp->head && temp->head->list_sim != temp) || (temp->tail && temp->tail->list_sim != temp) ||
        (temp->curr >= node_array && temp->curr < node_array + node_max &&
         temp->curr->list_sim != temp))
        return NULL;

    //checking the empty node 
    if (count_node > node_max - 1)
        return -1;  
    node *ptr_node = node_array + count_node++;
    ptr_node->data = value;
    ptr_node->bool_flag = 1;
    ptr_node->list_sim = temp;

    
    if (!temp->head)    
      
    {
        temp->head = ptr_node;
        temp->tail = ptr_node;
        temp->curr = ptr_node;
    } 
      // chcking out of bound around head 
    else if (temp->curr == node_array + node_max) {
        temp->head->prev = ptr_node;
        ptr_node->next = temp->head;
        temp->head = ptr_node;
        temp->curr = ptr_node;
    }
        // checking out of bound around tail
    else if (temp->curr == node_array + node_max + 1) {
        temp->tail->next = ptr_node;
        ptr_node->prev = temp->tail;
        temp->tail = ptr_node;
        temp->curr = ptr_node;
    }
     else { 
        ptr_node->next = temp->curr->next; 
        ptr_node->prev = temp->curr;
        temp->curr->next = ptr_node;

        if (temp->curr == temp->tail)
            temp->tail = ptr_node; 
        else
            ptr_node->next->prev = ptr_node;

        temp->curr = ptr_node;
    }
    temp->nodeCount++;
    return 0;
}

// dds item to list directly before the current item, and makes the new item the current one. If the current pointer is before the start of the list, the item is added at the start. If the current pointer is beyond the end of the list, the item is added at the end. Returns 0 on success, -1 on failure.
int ListInsert(list *temp, void *value) {
    
    int k=node_intialized(temp);
    if (k==-1)
        return -1;

    if ((temp->head && temp->head->list_sim != temp) || (temp->tail && temp->tail->list_sim != temp) ||
        (temp->curr >= node_array && temp->curr < node_array + node_max &&
         temp->curr->list_sim != temp))
        return NULL;

    // checking for empty node
    if (count_node > node_max - 1)
        return -1;
    node *ptr_node = node_array + count_node++;
    ptr_node->data = value;
    ptr_node->bool_flag = 1;
    ptr_node->list_sim = temp;


    if (!temp->head)   //if empty list
      
    {
        temp->head = ptr_node;
        temp->tail = ptr_node;
        temp->curr = ptr_node;
    } 
    // if out of bound around head
    else if (temp->curr == node_array + node_max) {
        temp->head->prev = ptr_node;
        ptr_node->next = temp->head;
        temp->head = ptr_node;
        temp->curr = ptr_node;
    }
    // if out of bound around tail
    else if (temp->curr == node_array + node_max + 1) {
        temp->tail->next = ptr_node;
        ptr_node->prev = temp->tail;
        temp->tail = ptr_node;
        temp->curr = ptr_node;
    } 

    else { 
        ptr_node->prev = temp->curr->prev; 
        ptr_node->next = temp->curr;
        temp->curr->prev = ptr_node;

        if (temp->curr == temp->head)
            temp->head = ptr_node; 
        else
            ptr_node->prev->next = ptr_node;
        temp->curr = ptr_node;
    }
    temp->nodeCount++;
    return 0;
}



// adds item to the end of list, and makes the new item the current one. Returns 0 on success, -1 on failure.
int ListAppend(list *temp, void *value) {

    int k=node_intialized(temp);
    if (k==-1)
        return -1;

    if ((temp->head && temp->head->list_sim != temp) || (temp->tail && temp->tail->list_sim != temp) ||
        (temp->curr >= node_array && temp->curr < node_array + node_max &&
         temp->curr->list_sim != temp))
        return NULL;

    // checking for empty node
    if (count_node > node_max - 1)
        return -1;

    node *ptr_node = node_array + count_node++;
    ptr_node->data = value;
    ptr_node->bool_flag = 1;
    ptr_node->list_sim = temp;

    //if it is empty list, everything will point to node pointer
    if (!temp->head)    
        
    {
        temp->head = ptr_node;
        temp->tail = ptr_node;
        temp->curr = ptr_node;
    } else {
        ptr_node->prev = temp->tail;
        temp->tail->next = ptr_node;
        temp->tail = ptr_node; 
        temp->curr = ptr_node;
    }
    temp->nodeCount++;
    return 0;
}

// adds item to the front of list, and makes the new item the current one. Returns 0 on success, -1 on failure.
int ListPrepend(list *temp, void *value) {
    
    int k=node_intialized(temp);
    if (k==-1)
        return -1;
    if ((temp->head && temp->head->list_sim != temp) || (temp->tail && temp->tail->list_sim != temp) ||
        (temp->curr >= node_array && temp->curr < node_array + node_max &&
         temp->curr->list_sim != temp))
        return NULL;


    // checking for empty node
    if (count_node > node_max - 1)
        return -1;

    node *ptr_node = node_array + count_node++;
    ptr_node->data = value;
    ptr_node->bool_flag = 1;
    ptr_node->list_sim = temp;

    // if list is empty, everything will point to nodeptr
    if (!temp->head)    
        
    {
        temp->head = ptr_node;
        temp->tail = ptr_node;
        temp->curr = ptr_node;
    } else {
        ptr_node->next = temp->head;
        temp->head->prev = ptr_node;
        temp->head = ptr_node; 
        temp->curr = ptr_node;
    }
    temp->nodeCount++;
    return 0;
}

// Return current item and take it out of list. Make the next item the current one.
void *ListRemove(list *temp) {
    
    int k=node_intialized(temp);
    if (k==-1 || count_node > node_max)
        return NULL;


    // if head and tail is not in the list 
    if ((temp->head && temp->head->list_sim != temp) || (temp->tail && temp->tail->list_sim != temp) ||
        (temp->curr >= node_array && temp->curr < node_array + node_max &&
         temp->curr->list_sim != temp))
        return NULL;

    int tail_end;
    if(temp->curr == temp->tail) 
     tail_end=1;
     else
     tail_end=0;


    int head_end;
     if(temp->curr == temp->head)
         head_end=1; 
         else
         head_end= 0;
    
    node deletionNode = *temp->curr;
    
    if (tail_end&&head_end)   
    {
        temp->head = NULL;
        temp->tail = NULL;
    } else if (tail_end) {
        temp->curr->prev->next = NULL;
        temp->tail = temp->curr->prev;
    } else if (head_end) {
        temp->curr->next->prev = temp->head;
        temp->head = temp->head->next;
        temp->head->prev = NULL;
    } // deleting in the middle of the list
    else {
        temp->curr->prev->next = temp->curr->next;
        temp->curr->next->prev = temp->curr->prev;
    }

    node *prev_node = node_array + count_node - 1;
    if (temp->curr != prev_node && prev_node->bool_flag) {
      
        *temp->curr = *prev_node;
        list *list_chan = prev_node->list_sim;

    if (list_chan->head == prev_node)
        list_chan->head = temp->curr;
    if (list_chan->tail == prev_node)
        list_chan->tail = temp->curr;
    if (list_chan->curr == prev_node)
        list_chan->curr = temp->curr;
    if (prev_node->prev)
        prev_node->prev->next = temp->curr;
    if (prev_node->next)
        prev_node->next->prev = temp->curr;
    }
    prev_node->bool_flag = 0;
    prev_node->data = NULL;
    prev_node->next = NULL;
    prev_node->prev = NULL;
    prev_node->list_sim = NULL;
    count_node--;
    temp->nodeCount--;

    if (tail_end)  
        temp->curr = deletionNode.prev;
    else if (head_end)
        temp->curr = temp->head;  
    return deletionNode.data;
}
// adds list2 to the end of list1. The current pointer is set to the current pointer of list1. List2 no longer exists after the operation.
void ListConcat(list *temp1, list *temp2) {
    
    int k=node_intialized(temp1);
    if (k==-1 || count_node > node_max)
        return;
    int j=node_intialized(temp2);
    if (j==-1)
        return;
    if ((temp1->head && temp1->head->list_sim != temp1) || (temp1->tail && temp1->tail->list_sim != temp1))
        return;
    if ((temp2->head && temp2->head->list_sim != temp2) || (temp2->tail && temp2->tail->list_sim != temp2))
        return;

    temp1->nodeCount += temp2->nodeCount;
    if (temp1->head && temp2->head) 
    {
        temp1->tail->next = temp2->head;
        temp2->head->prev = temp1->tail;
        temp1->tail = temp2->tail;
    } else if (temp2->head)
    {
        temp1->head = temp2->head;
        temp1->tail = temp2->tail;
    }
    

    node *tempNodePtr = temp2->head;
    while (tempNodePtr) {
#ifdef DEBUG
        if (tempNodePtr->list_sim != temp2)
            printf("\nError: node with data %d did not matches to temp2!, real value(list with head data) = %d\n",
                   *(int *) tempNodePtr->data, *(int *) tempNodePtr->list_sim->head->data);
#endif //DEBUG
        tempNodePtr->list_sim = temp1;
        tempNodePtr = tempNodePtr->next;
    }

  
    if (temp2 != list_array + list_last - 1) {
        temp_list[num_templist++] = temp2;
    } else {
        list_last--; 
    }
    temp2->bool_flag = 0;
    temp2->tail = NULL;
    temp2->nodeCount = 0;
    temp2->curr = NULL;
    temp2->head = NULL;
}

//delete list. itemFree is a pointer to a routine that frees an item. It should be invoked (within ListFree) as: (*itemFree)(itemToBeFreed)
void ListFree(list *temp, void (*itemFree)()) {
    int k=node_intialized(temp);
      // if no. of node is greater than node capacity
    if (k==-1 || count_node > node_max)
        return;
    
    if ((temp->head && temp->head->list_sim != temp) || (temp->tail && temp->tail->list_sim != temp))
        return;

    
    temp->curr=temp->head;   
    int temp4=0;
    while(temp->head&&!temp4)
    {
        if(temp->head==temp->tail)    
            temp4=1;
        if(temp->head->data)
            (*itemFree)(temp->head->data);
        ListRemove(temp);   
    }

    
    if (temp == list_array + list_last - 1) {
        list_last--;}
    else
        temp_list[num_templist++] = temp;

    temp->bool_flag = 0;
    temp->tail = NULL;
    temp->nodeCount = 0;
    temp->curr = NULL;
    temp->head = NULL;
}
// Return last item and take it out of list. Make the new last item the current one
void *ListTrim(list *temp) {
    
    int k=node_intialized(temp);
    if (k==-1||!temp->head)
        return NULL;
    
    if ((temp->head && temp->head->list_sim != temp) || (temp->tail && temp->tail->list_sim != temp) ||
        (temp->curr >= node_array && temp->curr < node_array + node_max &&
         temp->curr->list_sim != temp))
        return NULL;
    
    if(temp->tail->next)
        return NULL;

    temp->curr=temp->tail;
    return ListRemove(temp);
}

//searches list starting at the current item until the end is reached or a match is found. In this context, a match is determined by the comparator parameter. This parameter is a pointer to a routine that takes as its first argument an item pointer, and as its second argument comparisonArg. Comparator returns 0 if the item and comparisonArg don't match, or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator. If a match is found, the current pointer is left at the matched item and the pointer to that item is returned. If no match is found, the current pointer is left beyond the end of the list and a NULL pointer is returned
void *ListSearch(list *temp, int (*comparator)(), void *comparisonArg) {
    
    int k=node_intialized(temp);
    if (k==-1)
        return NULL;

    if ((temp->head && temp->head->list_sim != temp) || (temp->tail && temp->tail->list_sim != temp) ||
        (temp->curr >= node_array && temp->curr < node_array + node_max &&
         temp->curr->list_sim != temp))
        return NULL;

 
    temp->curr=temp->head;
    while(temp->curr)
    {
       
        if(temp->curr->data && (*comparator)(temp->curr->data,comparisonArg))
        {
            return temp->curr->data;
        }
        temp->curr=temp->curr->next;
    }
    // not found
    temp->curr=node_array + node_max + 1;
    return NULL;
}
// helper function used to print nodes
void listPrint(list *temp) {
    
    if (!temp->bool_flag)
        printf("Error activation of list not happen!\n");
     puts("\n");
    if (!temp) {
        printf("Error listPrint() shows NULL !\n");
        return;
    }
    node *tempPtr = temp->head;
    printf("start Printing: \n");
    int i = 0;
    while (tempPtr) {
        if (tempPtr == temp->tail && tempPtr->next)
            printf("\ncarefull: node %d is matched tail, but this->next!=NULL!!\n", i);
        printf("number %d: ", i);

        if (!tempPtr->list_sim)
            printf("\ncarefull;: node %d have no matched column \n", i);
        else if (tempPtr->list_sim != temp)
            printf("\ncarefull: node %d did not match list_sim to current list matches list with head data = %d\n", i,
                   *(int *) tempPtr->list_sim->head->data);

        if (!tempPtr->bool_flag)
            printf("\ncarefull: node %d bool_flag is 0!!\n", i);

        if (tempPtr->data)
            printf("%d\n", *(int *) (tempPtr->data));
        tempPtr = tempPtr->next;
        i++;
    }
    printf("DONE\n");
    node *tempPtr2 = temp->curr;


    if (tempPtr2 >= node_array && tempPtr2 < node_array + node_max &&
        tempPtr2) {
        int T = 0;
        while (tempPtr2 && tempPtr2 != temp->head) {
            tempPtr2 = tempPtr2->prev;
            T++;
        }
        if (temp->curr && temp->curr->data)
            printf("list->curr is currently at #%d (value: %d)\n", T, *(int *) temp->curr->data);
        else
            printf("list->curr is currently at #%d (data column is NULL)\n", T);
    } else if (tempPtr2 == node_array + node_max)
        printf("list->curr is now out of bounds around HEAD \n");
    else if (tempPtr2 == node_array + node_max + 1)
        printf("list->curr is now out of bounds around TAIL \n");
    else if (!tempPtr2)
        printf("list->curr is NULL\n");
    else
        printf(" now!! list->curr is invalid:  %d\n", (int) tempPtr2);
}

