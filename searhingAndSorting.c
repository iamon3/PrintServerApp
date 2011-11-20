#include "searchingAndSorting.h"
#include "MainHeader.h"
#include <stdio.h>

clientJobQNode *searchPrevFinishedTaskToBeRemovedFromClientJobQ(int taskId, clientJobQNode* head){
  clientJobQNode* prev=NULL;

  if(head -> taskId == taskId)
    return NULL;
  else{
    while(head->next && head ->next->taskId != taskId ){
      head = head ->next;
    }
    if(head ->next->taskId == taskId) 
      prev = head;  
  }
  return prev;
}

printer* isMsgRecvdFrmPrinter(int msgRecvdFd,printer** setOfPrinters)
{
  int i;
  for(i=0; i<totalConnectedPrinters; i++){
    if(msgRecvdFd == setOfPrinters[i]->printerFd){
      return setOfPrinters[i];
    }
  }
  return NULL;
}


client* isMsgRecvdFrmClient(int msgRecvdFd,client** setOfClients)
{
  int i;
  for(i=0; i<totalConnectedClients; i++){
    if(msgRecvdFd == setOfClients[i]->clientFd)
      return setOfClients[i]; 
  }
  return NULL;
}


printer* searchBestAvailablePrinter(printer** setOfPrinters){
  int i;
  for(i=0; i<totalConnectedPrinters; i++){
    if((setOfPrinters[i]->noOfTaskAssigned) < THRESHOLD_NO_OF_REQ )
      return setOfPrinters[i];
  }
  return NULL;
}

printer* searchAssignedPrinterForTaskInClientJobQ(int taskId,clientJobQNode* currentJobQNode){
  while( currentJobQNode != NULL ){
    if(currentJobQNode -> taskId == taskId)
      return currentJobQNode -> allotedPrinter;
  }
  printf("Task Id Not Found \n");
  return NULL;
}

printerJobQNode* serachCancelledPrintTaskInJobQ(int taskId,printer* assignedPrinter,client* searchedClient){
 printerJobQNode* prev = NULL;
  printerJobQNode* head=NULL;

  head = assignedPrinter -> front;
  if((head->clientAlloted == searchedClient) && (head -> taskId == taskId))
    return NULL;
  else{
    while(head->next && (head->next->clientAlloted != searchedClient) && head ->next->taskId != taskId ){
      head = head ->next;
    }
    if((head->next->clientAlloted == searchedClient) && head ->next->taskId == taskId) 
      prev = head;  
  }
  return prev;//

}
//////////////sorting

void sortSetOfPrinters(printer** setOfPrinters){
  int i,j;
  printer* temp;

  for(i=1;i<totalConnectedPrinters;i++){
    temp = setOfPrinters[i];
    j = i-1;
    while(j>=0 && (temp->priority < setOfPrinters[j]->priority)){
      setOfPrinters[j+1] = setOfPrinters[j];
      // printf("%d %d \n",i,j);
      j = j-1;
    }
    while(j>=0 && (temp->priority == setOfPrinters[j]->priority) && (temp->speed > setOfPrinters[j]->speed) ){
      setOfPrinters[j+1] = setOfPrinters[j];
      // printf("%d %d \n",i,j);
      j = j-1;
    }

    setOfPrinters[j+1] = temp;
  }

  printSetOfPrinters(setOfPrinters);
}


waitingQNode* searchPrevTaskOfCancelTaskFromWaitingQ(waitingQ* wtQ ,client* searchedClient, int taskId){
  waitingQNode* prev = NULL;
  waitingQNode* head=NULL;

  head = wtQ -> head;
  if((head->commandFromClient == searchedClient) && (head -> taskId == taskId))
    return NULL;
  else{
    while(head->next && head ->next->taskId != taskId ){
      head = head ->next;
    }
    if(head ->next->taskId == taskId) 
      prev = head;  
  }
  return prev;
}
