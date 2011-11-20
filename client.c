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
#include <sys/time.h>
#include  <signal.h>

#define STDIN 0  // file descriptor for standard input
#define MAX_NO_OF_CLI_CONNC 9
#define PORT_NO 9034


////////////////////////////////////////////////

void createClientMenu(int* taskId);
int  printAllTasksInProcess(int maxTaskId, char* option);
void send_and_receive_manager(int client_soc_fd);
void printClientMenu();
int checkAndCreateClientCommand(int *taskId,char* buffer);
void createPrintCommand(int* taskId,char* command);
void createStatusCommand(int taskId,char* command);
void createCancelCommand(int taskId, char* command);

////////////////////////////////////////////////

int printAllTasksInProcess(int maxTaskId, char* option){
  int i=0,taskId=-1;
  printf("Pleas select the Task ID you want to get %s from <Task Id, Printer ID, Status>\n",option);
  for(i=0;i<=maxTaskId;i++){
    printf("<%d, Pid, Stat>\n",i);
  }

  while(taskId > maxTaskId || taskId<0){
    printf("Enter Task Id :- ");
    scanf("%d",&taskId);
    printf("\n");
  }
  return taskId;
}


void createClientCommand(int* taskId)
{
  int option=-1, noOfPages = 0,cancelTaskId, statusTaskId;

  while(option == -1){
  printf("Hello Client Select your option \n 1 Print\n 2 Cancel\n 3 Status\n 4 Quit\n option :-\n");
  scanf("%d",&option);
  if(*taskId == -1 && (option==2 || option==3)){
    printf("You have not given any task for printing. First give task\n");
    option = -1;
  }
  else if(option == 1){
    printf("Enter No Of pages to be printed :- \n");
    scanf("%d",&noOfPages);
    printf("Thank you.Printing task is in progress.\nYour Task ID :- %d\n",++(*taskId));
  }
  else if(option == 2){
   cancelTaskId  = printAllTasksInProcess(*taskId,"cancelled");
  }
  else if(option == 3){
    statusTaskId = printAllTasksInProcess(*taskId,"status");
  }
  else
    printf("You entered wrong option\n");
  } 
}

/////////////////////////////////////////////////////

void  printClientMenu(){
  printf("----------- SELECT OPTION -------------------\n");
  printf("Hello Client.Please select command from follwoing menu\n");
  printf(" 1 Print\n 2 Status\n 3 Cancel\n 4 Quit\n");
  //printf(" 1 Print\n 4 Quit\n");
 printf("----------- SELECT OPTION -------------------\n");
 printf("Your Option :- \n");
}

void createPrintCommand(int* taskId, char* command){

  int pageNum=0;
  //  char *command;
  //command = (char*)calloc(32,sizeof(char));

  while(pageNum <= 0){
    printf("Enter No Of pages greater than 0 to be printed :- \n");
    scanf("%d",&pageNum);
    // pageNum = atoi(noOfPages);
    if(pageNum <= 0)
      printf("You Entered Wrong Input\n");
    else 
      sprintf(command,"1 %d %d",pageNum,++(*taskId));   
    }
  printf("Thank you.Printing task is in progress.\nYour Task ID :- %d\n",(*taskId));
}


void createStatusCommand(int taskId,char* command){
  sprintf(command,"2 %d", taskId);
}

void createCancelCommand(int taskId, char* command){
  sprintf(command,"3 %d", taskId);
}

int checkAndCreateClientCommand(int *taskId,char* command){
  int option,status = 0,tid;
 
  fscanf(stdin,"%d",&option);

  if(*taskId == -1 && (option==2 || option==3)){
    printf("You have not given any task for printing. First give task\n");
    printClientMenu();
  }

  else if(option == 1){
    createPrintCommand(taskId, command);
    status = 1;
  }

  else if(option == 2){
    printf("Enter Task ID of which status is required :- \n");
    scanf("%d",&tid);
    createStatusCommand(tid,command);
    status = 1;
  }

  else if(option == 3){
    printf("Enter Task ID To Be Cancelled :- \n");
    scanf("%d",&tid);
    createCancelCommand(tid,command);
    status = 1;
  }
  else if(option == 4){
    status = -1;
  }
  else{
    printf("You entered wrong option\n");
    printClientMenu();
    status = -1;
  }
  return status; 
}

void send_and_receive_manager(int clientSocFd)
{
  /*
   * it manages the prcedure of send ,receive and error handling.
   * for syncro it uses select funftion where two fd's are clientSocFd and STDIN.
   */
 
  int i,no_of_bytes,taskId = -1, status;
  char buffer1[128];
  char *buffer = (char*)calloc(64,sizeof(char)) ;
  fd_set readfds, original;

  printClientMenu();

  FD_ZERO(&original); // variables and macros for select
  FD_ZERO(&readfds);
  FD_SET( STDIN, &original);
  FD_SET(clientSocFd, &original);

  //while(1){
  while(status != -1){
    readfds = original;
    // waits until read request form one of the two fd's are clientSocFd or STDIN comes 
    //it's done infinitely
    if(select(clientSocFd+1, &readfds, NULL, NULL, NULL) == -1){
      perror("select error");
      exit(1);
    }

    for(i=0; i<= clientSocFd ; i++){

      if (FD_ISSET( i, &readfds)){ 
	if(i == STDIN){//  in send...........
	  memset(buffer, 0, strlen(buffer));
	  status = checkAndCreateClientCommand(&taskId,buffer);
       	  if( status== 1 && send(clientSocFd, buffer, 32, 0) == -1 ){
	    perror("send");
	  }
	} 

	else{  //  in recv...........
	  memset(buffer1, 0, strlen(buffer1));
	  if( (no_of_bytes = recv(clientSocFd, buffer1, 128, 0)) == -1 )
	    {
	      perror("recv");
	      printf("exited abnomally\n");
	    }
	  if(no_of_bytes==0)
	    {
	      printf("exited nomally or connection cut by server (TRY AFT SOME TIME)\n");
	      exit(1);
	    }
	  // printf("Received some message:-\n %s \n", buffer1);
	  printf("#################### RESULT #################################\n");
	  printf("#                                                           #\n");
	  printf("# Task ID ->    %s     <- Printer IP   #\n", buffer1);
	  printf("#                                                           #\n");
	  printf("#################### RESULT #################################\n");
	  //updateTaskIds(buffer1);  need to be written
	}
      }
    }
    printClientMenu();
  }
  close(clientSocFd);
  return;
}

////////////////////////////////////////////////////

int main()
{
  /*                                                                                                                          *main basically creates socket , connect and does some error handling.                                                     *at last calls   send_and_receive_manager( clientSocFd );
   */
  int client_soc_fd ;
  struct sockaddr_in client_info;
  struct hostent *he;
  if((he=gethostbyname( "localhost" ) )== NULL) 
    {
    perror("gethostbyname");
    exit(1);
    }
  if( (client_soc_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror("socket");
      exit(1);
    }
  
  client_info.sin_family = AF_INET;   // host byte order
  client_info.sin_port = htons(PORT_NO); // short, network byte order
  client_info.sin_addr = *((struct in_addr *)he->h_addr);
  memset( &(client_info.sin_zero), 0, 8);  // zero the rest of the struct
  if(connect(client_soc_fd, (struct sockaddr *)&client_info, sizeof(client_info)) == -1) 
    {
      perror("connect");
      exit(1);
    }
  printf("Client connected to the chat server......\n");

  send_and_receive_manager( client_soc_fd);

  return 0;
}
