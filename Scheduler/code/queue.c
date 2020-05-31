#include "headers.h"

typedef struct node {
struct node * next;
PCB data;
} node;

typedef struct node1 {
struct node1 * next;
float data;
} node1;

typedef struct nqueue
{
   node1 *front,*rear;
   int count;
}nqueue;


typedef struct queue
{
   node *front,*rear;
   int count;
}queue;

nqueue* createNQueue()
{
    
    struct nqueue* q = (nqueue*)malloc(sizeof(nqueue)); 
    q->front = q->rear = NULL; 
    q->count =0;
    return q; 
};


queue* createQueue()
{
    
    struct queue* q = (queue*)malloc(sizeof(queue)); 
    q->front = q->rear = NULL; 
    q->count =0;
    return q; 
};
void enqueueN(nqueue* q, float no)
{
    q->count++;
    node1* temp = (node1*)malloc(sizeof(node1));
    temp->data=no;
    temp->next = NULL;
    if(q->rear==NULL)
    {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next=temp;
    q->rear = temp;
};

void enqueue(queue* q, PCB newPCB)
{
    
    q->count++;
    node* temp = (node*)malloc(sizeof(node));
    temp->data=newPCB;
    temp->next = NULL;
    if(q->rear==NULL)
    {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next=temp;
    q->rear = temp;
};
void priorityEnqueue(queue* q, PCB newPCB)
{
    q->count++;
    node* temp = (node*)malloc(sizeof(node));
    temp->data=newPCB;
    temp->next = NULL;
    //if empty queue
    if(q->rear==NULL)
    {
        q->front = q->rear = temp;
        return;
    }
    //else
    node* p = (node*)malloc(sizeof(node));
    //pointer to front
    p=q->front;
    if(temp->data.priority < p->data.priority)
    {
        temp->next=q->front;
        q->front = temp;
        return;
    }
    //get to the position wherer last item with priority equal to mine
    while((p->next)&&(p->next->data.priority < temp->data.priority))
    {

        p=p->next;
    }

    temp->next=p->next;
    p->next=temp;
    return;
};

void priorityTEnqueue(queue* q, PCB newPCB)
{
    //if(newPCB.runTime <= 0 )
    // return;

    q->count++;
    node* temp = (node*)malloc(sizeof(node));
    temp->data=newPCB;
    temp->next = NULL;
    //if empty queue
    if(q->rear==NULL)
    {
        q->front = q->rear = temp;
        return;
    }
    //else
    node* p = (node*)malloc(sizeof(node));
    //pointer to front
    p=q->front;
    if(temp->data.remainingTime < p->data.remainingTime)
    {
        temp->next=q->front;
        q->front = temp;
        return;
    }
    //get to the position wherer last item with priority equal to mine
    while((p->next)&&(p->next->data.remainingTime < temp->data.remainingTime))
    {

        p=p->next;
    }

    temp->next=p->next;
    p->next=temp;
    return;
};
node* dequeue( queue* q) 
{  
    if(q->count==0)
        return NULL;

        q->count--;
    if (q->front == NULL) 
        return NULL; 
  
    struct node* temp = q->front; 
    q->front = q->front->next; 
    node * temp1 = temp;
    free(temp);
    if (q->front == NULL) 
        q->rear = NULL; 
  
    return temp1; 
} 


float dequeueN( nqueue* q) 
{  
    if(q->count==0)
        return 0;

        q->count--;
    if (q->front == NULL) 
        return 0; 
  
    struct node1* temp = q->front; 
    q->front = q->front->next; 
    float temp1 = temp->data;
    free(temp);
    if (q->front == NULL) 
        q->rear = NULL; 
  
    return temp1; 
} 
node* findNode(queue * q, int pid) {
    node* crntNode = q->front;
    while (crntNode != NULL) {
        if(crntNode->data.processID == pid) {
            return crntNode;
        }
        crntNode = crntNode->next;
    }
    return NULL;
}