#include "MainHeader.h"
#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <errno.h>
#include  <string.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <netdb.h>
#include  <arpa/inet.h>
#include  <sys/wait.h>
#include  <sys/time.h>
#include  <signal.h>
#include  <pthread.h>
#include <ncurses.h>

/////////////////////////////////////////////

#define STDIN 0  // file descriptor for standard input

/////////////////////////////////////////////

typedef struct threadParam{ 
  int* flag;
  int noOfPages;
  // unsigned int wt;
}threadParam;

long int perPageTime;
int printerSocFd, priority, speed;

/////////////////////////////////////////////

void printerManager();
void decodeMsgAndProcess(char* msg,int*  );
void doPrinting(int , int*);
void* printPages(void* tp);
//void cancelPrinting(int printerSocFd ,pthread_t* threadId );
//void sendFinishedPriningTaskResposeToClient(int clientSocFd,int taskId, char* result, char* ipAddr);
void sendPrinterInfoToServer();

///////Print command //////////////////////////////

void* printPages(void* tpm)
{
  char msg[32];
  threadParam* tp;
  tp = (threadParam*)tpm;

  int noOfPages ;
  int *flag;
  long int totalTimeReq=0;
  noOfPages = tp -> noOfPages;
  flag = tp -> flag;
  totalTimeReq = noOfPages * perPageTime;

  printf("Staring printing no of pages = %d\n",noOfPages);

  while((totalTimeReq)>= 0){
    napms(perPageTime);
    totalTimeReq -= perPageTime;
    if(*flag == 2){
      printf("Printing complete\n");
      memset(msg,0,sizeof(msg));
      sprintf(msg,"2 %d",(int)(noOfPages - (totalTimeReq / perPageTime)));
      if(send(printerSocFd, msg, 32, 0) == -1 )
	{
	  perror("sending err\n");
	}
      *flag = 0;
    }
    if(*flag == 3 ){
      break;
    }
  }
  if(*flag == 1){
    printf("Printing complete\n");
    memset(msg,0,sizeof(msg));
    sprintf(msg,"1 1");
    if(send(printerSocFd, msg, 32, 0) == -1 )
      {
	perror("sending err\n");
      }
    *flag = 0;
  }
  if(*flag == 3){
    printf("Printing Cancelled when %d pgaes  are completed to be printed\n",(int)(noOfPages - (totalTimeReq / perPageTime) ));
    memset(msg,0,sizeof(msg));
    sprintf(msg,"3 1");
    if(send(printerSocFd, msg, 32, 0) == -1 )
      {
	perror("sending err\n");
      }
    *flag = 0;
  }
  return NULL;
}

/////////////////////////////////////////////////////////

void doPrinting(int noOfPages, int* flag){
  pthread_t threadId;
  threadParam* tp;
  tp = (threadParam *)calloc(1,sizeof(threadParam));
  tp -> noOfPages = noOfPages;
  tp -> flag = flag;
  printf("Printing starting \n");
  pthread_create(&threadId, NULL, &printPages, tp);
}

/////////////////////////////////////////////////////////


void decodeMsgAndProcess(char* msg, int* flag){
  int command,noOfPages;
  
  sscanf(msg,"%d %d",&command,&noOfPages);
  
  if(command == 1)
    {
      printf("Print command.\n");
      *flag = 1;
      doPrinting(noOfPages,flag);
    }
  else if(command == 2)
    {
      printf("Status command.\n");
      *flag = 2;
    }
  else if(command == 3)
    {
      printf("Cancel command.\n");
      *flag = 3;
      //cancelPrinting(printerSocFd,threadId);
    }
  else 
    printf("Unrecognized command\n");

}
/////////////////////////////////////////////////////////

void sendPrinterInfoToServer(){
  char msg[128];
  printf("Sending Printer Info to Server\n");
  memset(msg,0,sizeof(msg));
  sprintf(msg,"%d %d",priority, speed);
  if(send(printerSocFd, msg, 128, 0) == -1 )
    {
      perror("sending err\n");
    }
}

/////////////////////////////////////////////////////////

void printerManager(){

  int len_byte;
  char msg[32];
  int flag=0;
 
  while(1){
    memset(msg,0,sizeof(msg));
    if ((len_byte = recv(printerSocFd, msg, sizeof(msg), 0)) <= 0)
      {  // got error or connection closed by server
	if (len_byte == 0) {    		// connection closed
	  printf("Server : exited abnormally \n");
	  exit(1);
	} else {
	  perror("recv error\n");
	}
      }
    else{
      printf("Server passed message:- %s\n",msg);
      decodeMsgAndProcess(msg,&flag);
    }
  } 
  
}

////////////////////Main Function//////////////////////////////

int main()
{

  struct sockaddr_in printer_info;
  struct hostent *he;

  printf("Enter Printer Priority (1 is max ,2 2nd Max..)e.g. 2 :-\n");
  scanf("%d",&priority);
  printf("Enter Printer Speed(Pages/Sec) e.g 10:-\n");
  scanf("%d",&speed);

  perPageTime = 1000/speed;

  if((he=gethostbyname( "localhost" ) )== NULL) 
    {
    perror("gethostbyname");
    exit(1);
    }
  if( (printerSocFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror("socket");
      exit(1);
    }
  
  printer_info.sin_family = AF_INET;   // host byte order
  printer_info.sin_port = htons(PRINTER_PORT); // short, network byte order
  printer_info.sin_addr = *((struct in_addr *)he->h_addr);
  memset( &(printer_info.sin_zero), 0, 8);  // zero the rest of the struct
  if(connect(printerSocFd, (struct sockaddr *)&printer_info, sizeof(printer_info)) == -1) 
    {
      perror("connect");
      exit(1);
    }
  printf("Printer connected to the chat server......\n");

  sendPrinterInfoToServer();

  printerManager();

  close(printerSocFd);

  return 0;
}

////////////////////Main Function//////////////////////////////
