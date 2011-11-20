#include "initDataStruct.h"
#include "MainHeader.h"
#include "searchingAndSorting.h"
#include "communication.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

printerJobQNode* createNewPrinterJobQNode(int noOfPages, int taskId,client* searchedClient ){
  printerJobQNode* newNode = (printerJobQNode*)calloc(1,sizeof(printerJobQNode));
  newNode -> clientAlloted = searchedClient;
  newNode -> taskId = taskId;
  newNode -> flag = 0;
  newNode -> noOfPagesToBePrinted = noOfPages;
  newNode -> next = NULL;
  return newNode;
}


clientJobQNode* createNewClientJobQNode(int noOfPages,int taskId, printer * availablePrinter){

  clientJobQNode* newNode =NULL;
  newNode = (clientJobQNode*) calloc(1,sizeof(clientJobQNode));
  newNode -> taskId = taskId;
  newNode -> noOfPagesToBePrinted = noOfPages;
  newNode -> allotedPrinter = availablePrinter;
  newNode -> next = NULL;
  return newNode;
}

client* createNewClient(){
  client* newClient=NULL;
  newClient = (client*)calloc(1,sizeof(client));
  return newClient;
}


printer* createNewPrinter(){
  printer* newPrinter = NULL;
  newPrinter = (printer *) calloc(1 , sizeof(printer));
  return newPrinter;
}


allConnections* createAllConnectionStruct()
{
  allConnections* connectedNodes = NULL;
  connectedNodes = (allConnections*) calloc(1,sizeof(allConnections));
  connectedNodes-> setOfClients = (client**)calloc(MAX_NO_OF_CLI_CONNC,sizeof(client*));
  connectedNodes-> setOfPrinters = (printer**)calloc(MAX_NO_OF_PRINTER_CONNC  , sizeof(printer*));
  return connectedNodes;
}


void initNewPrinter(printer* newPrinter, int newPrinterSocFd, struct sockaddr_in* printerInfo,int priority, int speed){

  char buff[64];

  memset(buff,0,strlen(buff));
  inet_ntop((*printerInfo).sin_family,(struct sockaddr *) printerInfo , buff, sizeof(buff));
  printf("Server get connection from Printer ( %s )\n", buff);

  strcpy( newPrinter -> ipAddr, buff);
  newPrinter -> printerFd = newPrinterSocFd;
  newPrinter -> priority = priority; 
  newPrinter -> speed = speed ;
  newPrinter -> noOfPagesRemaining = 0;
  newPrinter -> status = 0;
  newPrinter -> noOfTaskAssigned = 0;
  newPrinter -> rare = NULL;
  newPrinter -> front = NULL;
  printf("Printer ( %s ) initialized\n",newPrinter->ipAddr);
}


void initNewClient(client* newClient,int newClientFd,struct sockaddr_in* newClientInfo ){
  char buff[64];
  // int length_byts, status = -1;

  memset(buff,0,strlen(buff));
  inet_ntop((*newClientInfo).sin_family,(struct sockaddr *) newClientInfo , buff, sizeof(buff));
  printf("Server get connection from client:- %s\n", buff);

  strcpy(newClient-> ipAddr , buff); 
  newClient -> clientFd = newClientFd;
  newClient -> startJob = NULL;
  newClient -> lastJob = NULL;
}


int initAndInsertInSetOfPrinters(int newPrinterSocFd, struct sockaddr_in* printerInfo, printer** setOfPrinters) {

  int status;
  printer*  newPrinter;
  int priority, speed;
  char pinfo[128];

  getPrinterInfo(newPrinterSocFd,pinfo);
  sscanf(pinfo,"%d %d",&priority,&speed);

  newPrinter = createNewPrinter();   
  initNewPrinter(newPrinter, newPrinterSocFd, printerInfo,priority,speed);
  setOfPrinters[totalConnectedPrinters++]=newPrinter;

  if(totalConnectedPrinters > 1)
    sortSetOfPrinters(setOfPrinters);
  return status;
}


int initAndInsertInSetOfClients(int newClientFd, struct sockaddr_in* newClientInfo ,client** setOfClients){
  client* newClient = NULL;
  newClient = createNewClient();
  initNewClient(newClient,newClientFd,newClientInfo );
  setOfClients[totalConnectedClients++] = newClient;
  return 1;
}

waitingQ* createWaitingQ(){
  waitingQ* new = NULL;
  new = (waitingQ*)calloc(1,sizeof(waitingQ));
  new-> head = NULL;
  new -> tail =NULL;
  return new;
}

waitingQNode* createWaitingQNode(){
  waitingQNode* new =NULL;
  new  = (waitingQNode*) calloc(1,sizeof(waitingQNode*));
  return new;
}

void initWaitingQNode(int taskId, int noOfPages, client* searchedClient, waitingQNode* newNode){
  newNode -> taskId = taskId;
  newNode -> noOfPages = noOfPages;
  newNode -> commandFromClient = searchedClient;
  newNode -> next = NULL;
}
