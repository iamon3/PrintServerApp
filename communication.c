#include "communication.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>


void sendFinishedPrintingTaskResponseToClient(int clientSocFd,int taskId, char* result, char* ipAddr){
  char response[32];
  memset(response,0,sizeof(response));
  sprintf(response,"%d %s %s",taskId,result, ipAddr );
  if(send(clientSocFd, response, 32, 0) == -1 )
    {
      perror("sending err\n");
    }
  printf("Printing Fininshed msg sent to client from %s\n",ipAddr);
}

int sendPrintCommandToPrinter(unsigned int timeToWait,char* ipAddr,int printerFd ){
  int status = 1;
  char command[32];
  printf("\nSending Message\n");
  memset(command, 0,sizeof(command));
  sprintf(command,"1 %u",timeToWait);
  if(send(printerFd, command, 32, 0) == -1 ){
    perror("send error\n");
    status = -1;
  }
  printf("Print Command sent to printer %s\n",ipAddr);
  return status;
}

int sendCancelCommandToPrinter(char* ipAddr,int printerFd ){
  int status = 1;
  char command[32];
  printf("\nSending Cancel command to %s\n",ipAddr);
  memset(command, 0,sizeof(command));
  sprintf(command,"3");
  if(send(printerFd, command, 32, 0) == -1 ){
    perror("send error\n");
    status = -1;
  }
  printf("Cancel Command sent to printer %s\n",ipAddr);
  return status;
}



void getPrinterInfo(int printerFd, char* buffer){
  int len_byte;
 
  memset(buffer, 0,sizeof(buffer));
  if ((len_byte = recv(printerFd, buffer, sizeof(buffer), 0)) <= 0)
    {  // got error or connection closed by client
      if (len_byte == 0) {    		// connection closed
	printf("printer : exited abnormally \n");
      } else {
	perror("recv");
      }
    }   
}

void sendStatusMsgToClient(int noOfPagesRemaining,int taskId,char* printerIpAddr, int clientSocFd){
 char response[128];
  memset(response,0,sizeof(response));
  sprintf(response,"%d Pg completed -%d %s",taskId,noOfPagesRemaining, printerIpAddr );
  if(send(clientSocFd, response, 32, 0) == -1 )
    {
      perror("sending err\n");
    }
  printf("Status msg sent to client from printer %s\n",printerIpAddr);
}

void askStatusMsgToPrinter(int taskId,int printerFd,char* printerIpAddr,char* clientIpAddr){
 char msg[32];
  memset(msg,0,sizeof(msg));
  sprintf(msg,"2");
  if(send(printerFd, msg, 32, 0) == -1 )
    {
      perror("sending err\n");
    }
  printf("Status msg of task Id %d from client ( %s ) is asked printer ( %s )\n",taskId,clientIpAddr,printerIpAddr);
}
