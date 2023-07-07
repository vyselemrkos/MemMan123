#include <stdio.h>
#include <stdlib.h>

#define MAXMEMORY 1000
#define MINMEMORY 0

typedef struct node{
    int start;
    int end;
    struct node * next;
}node;

typedef enum BOOL{
    FALSE,
    TRUE
}BOOL;

void deleteTree(node * dugum)
{
    if(NULL!=dugum){
        deleteTree(dugum->next);
        free(dugum);
    }
}

void printTree(node * dugum)
{
    if(NULL!=dugum)
    {
        printf("%dK-%dK     ", dugum->start, dugum->end);
        printTree(dugum->next);
    }
    else printf("\n\n");
}

node * FindParent(node * head, int StartAddress, int size)
{
    node * iterator = head;
    node * returnvalue = NULL;
    while(NULL!=iterator)
    {
        if(StartAddress>=iterator->end)
        {
            returnvalue=iterator;
        }
        iterator=iterator->next;
    }
    return returnvalue;
}

node * FindChild(node * head, int StartAddress, int size)
{
    node * iterator = head;
    node * returnvalue = NULL;
    while(NULL!=iterator)
    {
        if(StartAddress+size<=iterator->start)
        {
            returnvalue=iterator;
            break;
        }
        iterator=iterator->next;
    }
    return returnvalue;
}

node * FindNode(node * head, int StartAddress, int size)
{
    node * returnvalue= NULL;
    node * iterator = head;
    while (NULL!=iterator)
    {
        if(iterator->start == StartAddress && iterator->end == StartAddress+size)
        {
            returnvalue = iterator;
        }
        iterator=iterator->next;
    }
    return returnvalue;
}

node * _WillBeUpdated(node * dugum, int StartAddress, int size) 
{
    node * returnvalue = NULL;
    if(NULL!=dugum)
    {
        returnvalue = _WillBeUpdated(dugum->next, StartAddress, size);
        if(StartAddress>=dugum->start && StartAddress<=dugum->end) //start içeride
        {
            returnvalue = dugum;
        }
        else if (StartAddress+size>=dugum->start && StartAddress+size<=dugum->end) //end içeride
        { 
            returnvalue = dugum;
        }
    }
    return returnvalue;
}

void bellekIadeEt(node ** head, int StartAddress, int size) //BellekSal
{
    if(NULL==head)
    {
        *head = (struct node *) malloc(sizeof(struct node));
        (*head)->start=StartAddress;
        (*head)->end=StartAddress+size;
        (*head)->next=NULL;
    }
    else
    {
        node * AddOrUpdate = _WillBeUpdated((*head), StartAddress, size);
        if(NULL!=AddOrUpdate)
        {
            if(AddOrUpdate->start>StartAddress){
                AddOrUpdate->start= StartAddress;
            }
            if(AddOrUpdate->end<StartAddress+size)
                AddOrUpdate->end= StartAddress+ size;
            if(NULL!=AddOrUpdate->next){
                if(AddOrUpdate->end==AddOrUpdate->next->start){
                    int a = AddOrUpdate->next->end;
                    bellekAlAdresli(head, AddOrUpdate->next->start, (AddOrUpdate->next->end-AddOrUpdate->next->start));
                    AddOrUpdate->end =a;
                }
            }
        }
        else
        {
            node * newnode = (struct node *)malloc(sizeof(struct node));
            newnode -> start = StartAddress;
            newnode -> end = StartAddress + size;

            node * Parent = FindParent((*head), StartAddress, size);
            node * Child = FindChild((*head), StartAddress, size);
            if(NULL!=Parent)
            {
                Parent->next = newnode;
                newnode -> next = Child;
            }
            else
            {
                (*head) = newnode;
                newnode->next= Child;
            }
            //new node must be added to right place
            //Find Parent
            //Find Child
        }
    }
    // merge if needed 
    printf("bellekIadeEt(&pList,%d*kb,%d*kb)\n", StartAddress, size);
    printf("--------------------------------------------------------------------------------\n");
    printTree((*head));
    //return head;
}

void bellekAlAdresli(node ** head, int StartAddress, int size)
{
    node * iterator = (*head); 
    while (NULL!=iterator)
    {
        if (StartAddress>iterator->start && StartAddress<iterator->end && StartAddress+size>=iterator->start && StartAddress+size>=iterator->end)
        {
            // start arada
            iterator -> end = StartAddress;
            break;
        }
        else if (StartAddress<=iterator->start && StartAddress<=iterator->end && StartAddress+size>iterator->start && StartAddress+size<iterator->end)
        {
            // end arada
            iterator ->start = StartAddress + size;
            break;
        }
        else if (StartAddress>iterator->start && StartAddress<iterator->end && StartAddress+size>iterator->start && StartAddress+size<iterator->end)
        {
            // ikisi de arada
            iterator -> end = StartAddress;
            bellekIadeEt(&(*head), StartAddress+size, iterator->end-StartAddress+size);
            break;
        }
        else if(StartAddress==iterator->start && StartAddress+size==iterator->end)
        {
            node * thenode = FindNode((*head),StartAddress,size);
            node * parent = FindParent((*head),StartAddress,size);
            if(NULL!=parent)
            {
                parent->next=thenode->next;
            }
            else (*head) = thenode->next;
            break;
        }
        iterator = iterator -> next;
    }
    printf("bellekAlAdresli(root,%d*kb,%d*kb)\n", StartAddress, size);
    printf("--------------------------------------------------------------------------------\n");
    printTree((*head));
}

node * InsertBestFindParent(node * head)
{
    //return NULL if the best place is startaddress = 0
    //return head if the best place is startaddress = head->end
    //return a non null parent pointer if the best place is between two pointers and startaddress = parent->end
    node * iterator = head;
    node * parent = NULL;
    if(NULL==iterator->next) //only head
    { 
        if(head->start-MINMEMORY>MAXMEMORY-head->end)
        {
            return parent;
        }
        else return head;
    }
    else
    {
        int gap = iterator->start - MINMEMORY;
        while(NULL!=iterator->next) //at least there are two pointers
        {
            if(iterator->end - iterator->next->start>gap) // a new bigger gap is detected
            {
                gap = iterator->end - iterator->next->start;
                parent = iterator;
            }
            iterator=iterator->next;
        }
        //iterator pointing last element
        if(MAXMEMORY-iterator->end>gap)
        {
            parent = iterator;
        }
    }
    return parent;
}

void bellekAlAdressiz(node ** head, int size)
{
    if(!head)
    {
        *head = (struct node *)malloc(sizeof(struct node));
        (*head)->start=MINMEMORY;
        (*head)->end=size;
        (*head)->next=NULL;
    }
    else
    {
        node * parent = InsertBestFindParent(*head);
        if(NULL==parent)
        {
            bellekIadeEt(head, 0, size);
        }
        else if(parent==(*head))
        {
            bellekIadeEt(head, (*head)->end, size);
        }
        else
        {
            bellekIadeEt(head, parent->end,size);
        }
    }
    printf("bellekAlAdressiz(&pList,%d*kb)\n",size);
    printf("--------------------------------------------------------------------------------\n");
    printTree((*head));
}


void buildTree()
{
    node * pList=NULL;
    //bellekIadeEt(&head, 55,300); // 55 355
    //bellekIadeEt(&head, 360,50); // 360 410
    //bellekIadeEt(&head, 10,35); // 10 35
    //bellekAlAdressiz(&head, 1);

   //bellekIadeEt(&pList,900,5);
   //bellekIadeEt(&pList,26,3);
   //bellekAlAdresli(&pList,26,3);
   //bellekIadeEt(&pList,3,1);
   //bellekIadeEt(&pList,17,1);
   //bellekIadeEt(&pList,920,10);
   //bellekIadeEt(&pList,16,1);
   //bellekIadeEt(&pList,9,1);
   //bellekIadeEt(&pList,8,1);
   //bellekIadeEt(&pList,19,1);
   //bellekIadeEt(&pList,18,1);
   //bellekIadeEt(&pList,24,2);
   //bellekIadeEt(&pList,1,2);
   //bellekIadeEt(&pList,29,1);
   //bellekAlAdresli(&pList,2,1);
   //bellekAlAdresli(&pList,24,2);
   //bellekAlAdresli(&pList,18,1);
    bellekIadeEt(&pList,900,5);
    bellekIadeEt(&pList,26,3);
    bellekAlAdresli(&pList,26,3);
    bellekIadeEt(&pList,3,1);
    bellekIadeEt(&pList,17,1);
    bellekIadeEt(&pList,920,10);
    bellekIadeEt(&pList,16,1);
    bellekIadeEt(&pList,9,1);
    bellekIadeEt(&pList,8,1);
    bellekIadeEt(&pList,19,1);
    bellekIadeEt(&pList,18,1);
    bellekIadeEt(&pList,24,2);
    bellekIadeEt(&pList,1,2);
    bellekIadeEt(&pList,29,1);
    bellekAlAdresli(&pList,2,1);
    bellekAlAdresli(&pList,24,2);
    bellekAlAdresli(&pList,18,1);
    bellekAlAdresli(&pList,19,1);
    bellekAlAdresli(&pList,8,1);
    bellekAlAdresli(&pList,9,1);
    bellekAlAdresli(&pList,16,1);
    bellekAlAdresli(&pList,17,1);
    bellekIadeEt(&pList,28,1);
    bellekIadeEt(&pList,17,1);
    bellekIadeEt(&pList,16,1);
    bellekIadeEt(&pList,899,1);
    bellekIadeEt(&pList,27,1);
    bellekIadeEt(&pList,18,9);
    bellekAlAdresli(&pList,15,5);
    bellekIadeEt(&pList,30,50);
    bellekIadeEt(&pList,17,1);
    bellekAlAdresli(&pList,19,2);
    bellekAlAdressiz(&pList,1);
    bellekIadeEt(&pList,90,17);
    bellekIadeEt(&pList,18,2);
    bellekAlAdressiz(&pList,6);
    bellekIadeEt(&pList,180,20);
    bellekIadeEt(&pList,250,20);
    bellekIadeEt(&pList,300,20);
    bellekAlAdresli(&pList,29,3);
    bellekAlAdressiz(&pList,8);
    bellekAlAdresli(&pList,303,17);
    bellekAlAdressiz(&pList,2);
    bellekAlAdresli(&pList,92,15);
    bellekIadeEt(&pList,303,20);
    bellekAlAdressiz(&pList,1);
    bellekIadeEt(&pList,92,20);
    bellekAlAdressiz(&pList,25);
    bellekAlAdressiz(&pList,15);
    bellekAlAdressiz(&pList,22);
    bellekAlAdressiz(&pList,22);
    bellekAlAdressiz(&pList,22);
    bellekIadeEt(&pList,25,7);
    bellekAlAdresli(&pList,195,5);
    bellekAlAdressiz(&pList,1);
    bellekAlAdressiz(&pList,1);
    bellekIadeEt(&pList,300,20);
    bellekAlAdressiz(&pList,7);
    bellekAlAdressiz(&pList,20);
    bellekAlAdressiz(&pList,20);
    bellekAlAdressiz(&pList,6);
    bellekAlAdressiz(&pList,2);
    bellekAlAdressiz(&pList,2);
    bellekIadeEt(&pList,400,40);
    bellekIadeEt(&pList,40,40);
    bellekIadeEt(&pList,100,40);
    bellekAlAdresli(&pList,100,5);
    bellekAlAdresli(&pList,430,10);
    bellekAlAdresli(&pList,70,10);
    bellekIadeEt(&pList,430,40);
    bellekIadeEt(&pList,70,20);
    bellekIadeEt(&pList,950,40);
    bellekAlAdresli(&pList,420,5);
    bellekAlAdresli(&pList,950,5);
    bellekIadeEt(&pList,95,5);
    bellekIadeEt(&pList,330,40);
    bellekIadeEt(&pList,480,40);
    bellekIadeEt(&pList,90,4);
    bellekIadeEt(&pList,140,40);
    bellekIadeEt(&pList,990,40);
    bellekIadeEt(&pList,94,1);
    bellekIadeEt(&pList,370,30);
    bellekIadeEt(&pList,520,435);
    bellekAlAdresli(&pList,40,10);
    bellekAlAdresli(&pList,322,10);
    bellekAlAdresli(&pList,480,10);
    bellekAlAdresli(&pList,490,540);
    bellekAlAdresli(&pList,50,50);
    bellekAlAdresli(&pList,110,20);
    bellekAlAdresli(&pList,330,10);
    bellekAlAdresli(&pList,350,10);
    bellekAlAdresli(&pList,430,10);


    //deleteTree(head);
}

int main()
{
    buildTree();
    return 0;
}