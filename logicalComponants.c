#include "logicalComponants.h"
#include <stdio.h>
#include "manipulateDataStruct.h"
#include "helperFunctions.h"
#include "searchingAndSorting.h"

int decodeMsg(char* msg){
int commandType= 0;
 sscanf(msg,"%d",&commandType);
 return commandType;
}


void processAndForwardMsgToClient(char* msg,printer* searchedPrinter,allConnections* connectedNodes,waitingQ* wtQ){
  //int status=5;
  int typeOfCommand;

  typeOfCommand = decodeMsg(msg);
  
  if(typeOfCommand == 1){ // Print Command
    printf("Print Complete\n");
    removeFinishedPrintTaskFromJobQsAndReplyToClient("Completed", searchedPrinter, connectedNodes,wtQ);
  }
  else if(typeOfCommand == 2 )
    {
      printf("Status Reply\n");
      forwardStatusToClient(msg,searchedPrinter);
    }
  else if(typeOfCommand == 3)
    {      
      printf("Print command Cancelled in %s\n",searchedPrinter->ipAddr);
      forwardCancelSuccessToOthrFuct(connectedNodes, searchedPrinter,wtQ);
      //cancelPrintTask();
    }
  else if(typeOfCommand == 4)
    {
      printf("printer Quit\n");
      //removeClientEntry();
    }
  //  return status;
}

int processAndCommandToPrinter(char* msg, client* searchedClient, allConnections* connectedNodes, waitingQ* wtQ){
  int status=5;
  int typeOfCommand;
  int printStatus;

  typeOfCommand = decodeMsg(msg);
  
  if(typeOfCommand == 1){ // Print Command
    printf("Print Command\n"); // check whether 3rd parameter is needed
    printStatus = addPrintTaskInJobQAndAssignToBestPrinter(msg, searchedClient, connectedNodes,wtQ);
  }
  else if(typeOfCommand == 2 )
    {
      printf("Status Command\n");
      findPrinterInJobQAndAskStatusToPrinter(msg, searchedClient,connectedNodes,wtQ);
    }
  else if(typeOfCommand == 3)
    {
      printf("Cancel Command\n");
      findPrintTaskInJobQAndSendCancelToPrinter(msg, searchedClient,connectedNodes,wtQ);
    }
  else if(typeOfCommand == 4)
    {
      printf("Client Quit\n");
      //removeClientEntry();
    }
  
  return status;
}

void analyzeAndProcessReceievedMsg(char* msg,int msgRecvdFd,allConnections* connectedNodes ,waitingQ* wtQ){
  int status=0;
  client* searchedClient  = NULL;
  printer* searchedPrinter= NULL;

  if((searchedPrinter = isMsgRecvdFrmPrinter(msgRecvdFd,connectedNodes -> setOfPrinters)) != NULL ){
    printf("Message Receieved from ( %s ) printer\n",searchedPrinter ->ipAddr);
    processAndForwardMsgToClient(msg,searchedPrinter, connectedNodes,wtQ);
    printf("Ananlyzed ---\n");
  }
  else if((searchedClient = isMsgRecvdFrmClient(msgRecvdFd,connectedNodes -> setOfClients)) != NULL){
    printf("Message Receieved from ( %s )client \n",searchedClient ->ipAddr);
    status = processAndCommandToPrinter(msg, searchedClient, connectedNodes,wtQ);
     if(status == 1)
       {
	 printf("success\n");
       }    
     else if(status == 2){
       printf("Job Q Full.Added in waiting Q\n");
     }
     //     else
     //printf("Error or Printer disconnecte\n");
   }
   else
      printf("Wrong File Idscripot or Closed FD\n");
} 

/*
void updateWaitingQAndOtherJobQs(waitingQ* wtQ,allConnections* connectedNodes){
  if(wtQ->head){
    removeTaskFromWtQAndAssignToBestPrinter(wtQ, connectedNodes);
  }
}
*/
