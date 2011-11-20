#include "initDataStruct.h"
#include "searchingAndSorting.h"
#include "manipulateDataStruct.h"
#include <stdio.h>
#include <stdlib.h>

void addTaskIntoPrinterJobQ(int noOfPages, int taskId,client* searchedClient, printer* availablePrinter){
  printerJobQNode* jobQNode;
  jobQNode = createNewPrinterJobQNode(noOfPages, taskId, searchedClient );

  if((availablePrinter->rare) == NULL)
    {
      availablePrinter -> front = jobQNode; 
      availablePrinter -> rare = jobQNode; 
    }
  else{
    jobQNode -> next = availablePrinter -> front;
    availablePrinter -> front = jobQNode;
  }
  availablePrinter -> noOfTaskAssigned += 1;
  availablePrinter -> status = 1;

  //checkFor Whether NoPages remaing need to be updated 
  //in this way or other way
  availablePrinter -> noOfPagesRemaining += noOfPages; 
  printf("Task ID ( %d ) added in printer( %s )job Q\n",taskId,availablePrinter -> ipAddr);
}

void  addTaskIntoClientJobQ(int noOfPages,int taskId,client* searchedClient,printer* availablePrinter){
  clientJobQNode* jobQNode =NULL;
  jobQNode = createNewClientJobQNode(noOfPages,taskId,availablePrinter);
  if(searchedClient -> startJob == NULL){
    searchedClient -> startJob = jobQNode;
    searchedClient -> lastJob = jobQNode;
  }
  else{
    searchedClient -> lastJob -> next = jobQNode; 
    searchedClient -> lastJob = jobQNode;
  }
  printf("Task ID ( %d ) added in client ( %s )jobs\n",taskId,searchedClient -> ipAddr);
}


void addTaskInWaitingQ(int taskId, int noOfPages, client* searchedClient,waitingQ* wtQ){
  waitingQNode* newNode=NULL;
  newNode = createWaitingQNode();
  initWaitingQNode(taskId,noOfPages,searchedClient,newNode);

  if(!(wtQ -> head)){
    wtQ -> head = newNode;
    wtQ -> tail = newNode;
    }
  else{
    wtQ -> tail -> next = newNode; 
    wtQ -> tail = newNode;
  }
  printf("Print Task ID ( %d) from client ( %s ) Added in Waiting Q\n",taskId,searchedClient->ipAddr);
}


client* removeFinishedTaskFromPrinterJobQ(printer* searchedPrinter){

  client* clientAllocated = NULL ;
  printerJobQNode* temp; 
  printerJobQNode* temp1; 
  int taskId = searchedPrinter -> rare -> taskId;

  //printf(" Task id to b removed - %d \n",searchedPrinter -> rare -> taskId);
 
  clientAllocated =  searchedPrinter -> rare -> clientAlloted;
  searchedPrinter -> noOfTaskAssigned -= 1;
  searchedPrinter -> noOfPagesRemaining -= searchedPrinter -> rare -> noOfPagesToBePrinted;
 
  if(searchedPrinter -> noOfTaskAssigned == 0){ 
    searchedPrinter -> status = 0;
  }

  temp = searchedPrinter -> rare ;

  if((searchedPrinter -> noOfTaskAssigned == 0) || searchedPrinter -> front == searchedPrinter -> rare)
    { 
      searchedPrinter -> front = NULL;
      searchedPrinter -> rare = NULL;
    }
  else{ 
    temp1 = searchedPrinter -> front;
    while(temp1 -> next != searchedPrinter -> rare)
      {
	temp1 = temp1 -> next;
      }
    searchedPrinter -> rare = temp1; //searchedPrinter -> rare -> next;
  }
 
  free(temp);
  printf("Finished Task ID ( %d ) From client( %s ) assigned to printer ( %s ) is removed printer Q\n",taskId, clientAllocated -> ipAddr,searchedPrinter -> ipAddr);
  
  return clientAllocated;
}

void removeCancelledTaskFromPrinterJobQ(int taskId,printer* assignedPrinter,client* searchedClient){
  printerJobQNode* prev;
  printerJobQNode* temp;
  prev = serachCancelledPrintTaskInJobQ(taskId, assignedPrinter,searchedClient);
  if(prev == NULL){
    temp = assignedPrinter -> front;
    assignedPrinter -> front =  assignedPrinter -> front -> next;
  }
  else{
    temp = prev -> next;
    if(temp == assignedPrinter -> rare){
      assignedPrinter -> rare = prev;
    }
    prev -> next =  temp -> next;
  }
  free(temp);
  printf("Cancelled task ID ( %d ) is removed from Printer ( %s ) Job Q\n",taskId,assignedPrinter -> ipAddr);
}


int removeFinishedTaskFromClientJobQ(int taskIdFromPrinter, client* clientAllocated){

  int taskId;
  clientJobQNode* prev;
  clientJobQNode* temp ;
 
  if((clientAllocated -> startJob) == (clientAllocated -> lastJob)){
    temp = clientAllocated -> startJob;
    clientAllocated -> startJob = NULL;
    clientAllocated -> lastJob = NULL;
  }
  else{
    prev = searchPrevFinishedTaskToBeRemovedFromClientJobQ(taskIdFromPrinter ,clientAllocated->startJob);

    if(prev == NULL){
      temp = clientAllocated -> startJob;
      clientAllocated -> startJob = clientAllocated -> startJob ->next;
    }
    else{
      temp = prev -> next;
      prev -> next = temp->next;
      if(temp == clientAllocated -> lastJob){
      clientAllocated -> lastJob = prev;
    }
   }
  }
 
  taskId = temp->taskId;
  prev =NULL;
  free(temp);
  printf("Finished task ID ( %d ) from client(  %s ) removed from its job Q\n",taskId,clientAllocated -> ipAddr);
  return taskId;
}

void removeCancelledTaskFromClientJobQ(int taskId, client* searchedClient){
  int status ;
  status = removeFinishedTaskFromClientJobQ( taskId, searchedClient);
}


void removeTaskFromWaitingQ(waitingQ* wtQ,printer* assignedPrinter){
  waitingQNode* temp ;

  temp = wtQ->head;
  if((wtQ -> head) == (wtQ -> tail)){
    wtQ -> head = NULL;
    wtQ -> tail = NULL;
  }
  else{
    wtQ -> head = temp -> next;
  }
  printf("Task( %d )of client( %s ) removed from waiting Q n assigned to printer( %s )\n",temp->taskId,temp->commandFromClient->ipAddr,assignedPrinter -> ipAddr);

  temp -> commandFromClient = NULL;
  temp -> next = NULL;
  temp=NULL;
  //  free(temp);
  printf("Memory freeddd ....\n");
}

void removeCancelledTaskFromWaitingQ(waitingQ* wtQ, client* searchedClient, int taskId){

  waitingQNode* prev;
  waitingQNode* temp ;
 
  if((wtQ -> head) == (wtQ -> tail) && (wtQ -> head -> commandFromClient) && (wtQ -> head -> taskId == taskId)){
    temp = wtQ -> head;
    wtQ -> head = NULL;
    wtQ -> tail = NULL;
  }
  else{
    prev = searchPrevTaskOfCancelTaskFromWaitingQ(wtQ ,searchedClient, taskId);

    if(prev == NULL){
      temp = wtQ -> head;
      wtQ -> head = wtQ ->head ->next;
    }
    else{
      temp = prev -> next;
      prev -> next = temp->next;
      if(temp == wtQ -> tail){
      wtQ -> tail = prev;
    }
   }
  }
  prev =NULL;
  free(temp);
  printf("Cancelled task ID ( %d ) from client(  %s ) removed from Waiting Q\n",taskId,searchedClient -> ipAddr);
}

