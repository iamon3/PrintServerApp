#include "helperFunctions.h"
#include "searchingAndSorting.h"
#include "manipulateDataStruct.h"
#include "communication.h"
#include <stdio.h>


int addPrintTaskInJobQAndAssignToBestPrinter(char* msg, client* searchedClient, allConnections* connectedNodes,waitingQ* wtQ){
  printer* availablePrinter= NULL;
  int noOfPages=0, commandType, taskId;
  int status = 0;
  //  unsigned int timeToWait;

  sscanf(msg,"%d %d %d",&commandType,&noOfPages,&taskId );

  if((availablePrinter = searchBestAvailablePrinter(connectedNodes -> setOfPrinters)) != NULL){
    addTaskIntoPrinterJobQ(noOfPages,taskId,searchedClient,availablePrinter);
    addTaskIntoClientJobQ(noOfPages,taskId,searchedClient,availablePrinter);
    //    timeToWait =(unsigned int) (noOfPages *availablePrinter->speed);
    //    status = sendPrintCommandToPrinter(timeToWait,availablePrinter->ipAddr,availablePrinter->printerFd );
    status = 1;
  }
  else {
    printf("All printers are buzy. Add TaskID ( %d ) from client ( %s ) in waiting Q\n",taskId, searchedClient->ipAddr);
    addTaskInWaitingQ(taskId,noOfPages,searchedClient,wtQ);
    status = 0;
  }
  return status;
}

void findPrintTaskInJobQAndSendCancelToPrinter(char* msg, client* searchedClient, allConnections* connectedNodes,waitingQ* wtQ){
  printer* assignedPrinter= NULL;
  int  commandType, taskId,status;

  sscanf(msg,"%d %d",&commandType,&taskId );

  if((assignedPrinter = searchAssignedPrinterForTaskInClientJobQ(taskId,searchedClient -> startJob)) != NULL){
    printf("Client %s task ID %d is in Printer %s job q\n",searchedClient -> ipAddr,taskId,assignedPrinter -> ipAddr);
    //    timeToWait =(unsigned int) (noOfPages *availablePrinter->speed);
    if(assignedPrinter -> rare -> taskId == taskId && assignedPrinter -> rare->flag == 1 )
      {
	printf("Client %s task ID %d is in Printer %s is in the process \n",searchedClient -> ipAddr,taskId,assignedPrinter -> ipAddr);
	status = sendCancelCommandToPrinter(assignedPrinter->ipAddr,assignedPrinter->printerFd );
      }
    else{
    printf("Client %s task ID %d is assigned to Printer %s but currently not in the process \n",searchedClient -> ipAddr,taskId,assignedPrinter -> ipAddr);
      removeCancelledPrintTaskFromJobQsAndReplyToClient(taskId, assignedPrinter, searchedClient);
    }
  }
  else {
    printf("Task %d  from client %s is not assigne to any printer. might be in waiting q \n",taskId, searchedClient -> ipAddr);
    removeCancelledTaskFromWaitingQ(wtQ, searchedClient, taskId);
    printf("Yet task ID( %d )from client ( %s ) is in waiting Q. No Printer assigned\n",taskId, searchedClient->ipAddr);
  }
}

void removeCancelledPrintTaskFromJobQsAndReplyToClient(int taskId, printer* assignedPrinter,client* searchedClient){
  removeCancelledTaskFromPrinterJobQ(taskId, assignedPrinter,searchedClient);
  removeCancelledTaskFromClientJobQ(taskId, searchedClient);
  sendFinishedPrintingTaskResponseToClient(searchedClient -> clientFd , taskId, "Cancelled", assignedPrinter -> ipAddr);
}

void removeFinishedPrintTaskFromJobQsAndReplyToClient(char* msg, printer* searchedPrinter, allConnections* connectedNodes,waitingQ* wtQ){
  client* clientAllocated=NULL;
  int taskIdFromPrinter,status;

  taskIdFromPrinter = searchedPrinter -> rare -> taskId;
  clientAllocated = removeFinishedTaskFromPrinterJobQ(searchedPrinter);
  status = removeFinishedTaskFromClientJobQ(taskIdFromPrinter,clientAllocated);
  sendFinishedPrintingTaskResponseToClient(clientAllocated -> clientFd , taskIdFromPrinter, msg , searchedPrinter -> ipAddr);
  updateWaitingQAndOtherJobQs(wtQ, connectedNodes,searchedPrinter);
}

void forwardCancelSuccessToOthrFuct(allConnections* connectedNodes,printer* searchedPrinter,waitingQ* wtQ){
  char msg[]="Cancelled\0"; 
  removeFinishedPrintTaskFromJobQsAndReplyToClient(msg, searchedPrinter,connectedNodes,wtQ);
}


void removeTaskFromWtQAndAssignToBestPrinter(waitingQ* wtQ,allConnections* connectedNodes,printer* searchedPrinter){
  printer* availablePrinter= NULL;
  availablePrinter= searchedPrinter;
  //if((availablePrinter = searchBestAvailablePrinter(connectedNodes -> setOfPrinters)) != NULL){
    addTaskIntoPrinterJobQ(wtQ->head->noOfPages,wtQ->head->taskId,wtQ->head->commandFromClient,availablePrinter);
    addTaskIntoClientJobQ(wtQ->head->noOfPages,wtQ->head->taskId,wtQ->head->commandFromClient,availablePrinter);
    removeTaskFromWaitingQ(wtQ,availablePrinter);
    /* }
  else{
    printf("Still all printers are buzy.TaskId (%d) from client( %s ) Can't be removed from Waiting Q\n",wtQ->head->taskId,wtQ->head->commandFromClient->ipAddr);
    }*/
}

void updateWaitingQAndOtherJobQs(waitingQ* wtQ,allConnections* connectedNodes,printer* searchedPrinter){
  if(wtQ->head){
    removeTaskFromWtQAndAssignToBestPrinter(wtQ, connectedNodes,searchedPrinter);
    printf("Updated.....\n");
  }
}


void forwardStatusToClient(char* msg,printer* searchedPrinter){
  int cmd, noOfPagesRemaining;
  sscanf(msg,"%d %d",&cmd,&noOfPagesRemaining);
  sendStatusMsgToClient(noOfPagesRemaining,searchedPrinter->rare->taskId,searchedPrinter->ipAddr,searchedPrinter -> rare -> clientAlloted->clientFd);
}

void findPrinterInJobQAndAskStatusToPrinter(char* msg,client* searchedClient,allConnections* connectedNodes,waitingQ* wtQ){
  int  commandType, taskId;
  printer* assignedPrinter=NULL;

  sscanf(msg,"%d %d",&commandType,&taskId );
  if((assignedPrinter = searchAssignedPrinterForTaskInClientJobQ(taskId,searchedClient -> startJob)) != NULL){
    if((assignedPrinter -> rare -> clientAlloted == searchedClient ) && (assignedPrinter -> rare -> taskId == taskId))
      askStatusMsgToPrinter(taskId,assignedPrinter -> printerFd,assignedPrinter -> ipAddr, searchedClient -> ipAddr);
  }
  else
    printf("Task %d from client %s is not assigned any printer. Might be in waiting Q \n",taskId, searchedClient->ipAddr);
}
