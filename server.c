#include "MainHeader.h"
#include "dataStructures.h"
#include "initDataStruct.h"
#include "communication.h"
#include "logicalComponants.h"
#include <stdio.h>
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
#include  <signal.h>

void executePrinting(allConnections* connectedNodes){

  printer** setOfPrinters;
  int i,status;
  //  unsigned int timeToWait;
  int noOfPages;

  if(connectedNodes){
    setOfPrinters = connectedNodes -> setOfPrinters;
  }
  if(setOfPrinters){
    for(i=0;i<totalConnectedPrinters;i++){
      if(setOfPrinters[i] && setOfPrinters[i]->rare && (setOfPrinters[i]->rare->flag) == 0){
	setOfPrinters[i]->rare->flag = 1; //Currently in process
	//	timeToWait =(unsigned int) (((setOfPrinters[i] -> rare ->noOfPagesToBePrinted )/(setOfPrinters[i]->speed))*10);
	noOfPages = setOfPrinters[i] -> rare ->noOfPagesToBePrinted;
	status = sendPrintCommandToPrinter(noOfPages,setOfPrinters[i]->ipAddr,setOfPrinters[i]->printerFd );
	//printf("Status ");
      }
    }
  }
}

void  printServerManager(int server_soc_fd,int server_printer_soc_fd)
{
  int fd_max_val, i, len_byte, status;
  size_t addrlen;
  int new_soc_fd ;
  char buffer[32];
  fd_set original_fds,read_fds;
 
 
  struct sockaddr_in client_info, printer_info;
  allConnections* connectedNodes = NULL;
  waitingQ* wtQ= NULL;

  connectedNodes = createAllConnectionStruct();
  wtQ = createWaitingQ();
// client_info= *server_info;
//printer_info = *server_printer_info;

  FD_ZERO(&original_fds);   // structure used for select function handling.
  FD_ZERO(&read_fds);

  // add the server_soc_fd to the original_fds set
  FD_SET(server_soc_fd, &original_fds);
  FD_SET(server_printer_soc_fd, &original_fds);
  
 
  if(server_printer_soc_fd > server_soc_fd){
    fd_max_val = server_printer_soc_fd;
  }
  else
    fd_max_val = server_soc_fd;

  while(1)
    {
      executePrinting(connectedNodes);
      // updateWaitingQAndOtherJobQs(wtQ, connectedNodes);
      // here select pool for send ,recv,login,logout and handles appropietly.
      read_fds = original_fds; 
      if (select(fd_max_val+1, &read_fds, NULL, NULL, NULL) == -1) {
	perror("select");
	exit(1);
      }
      for(i = 0; i <= fd_max_val; i++) {   // run through the existing connections looking for data to read

	if(FD_ISSET(i, &read_fds)) { // we got one!!

	  if(i == server_soc_fd) {         // new client connectioned
            addrlen = sizeof(client_info);
            if((new_soc_fd = accept(server_soc_fd, (struct sockaddr *)&client_info, &addrlen)) == -1) {
	      perror("Client accept");
            } else { 
       	      status = initAndInsertInSetOfClients(new_soc_fd, &client_info,connectedNodes -> setOfClients); 
	      
	      FD_SET(new_soc_fd, &original_fds); // add to master set
	      if (new_soc_fd > fd_max_val) {     // keep track of the maximum
		fd_max_val = new_soc_fd;
	      }
	    }
	  }

	  else if(i == server_printer_soc_fd) {//New Printer connected
	    // handle new printer connections
            addrlen = sizeof(printer_info);
            if((new_soc_fd = accept(server_printer_soc_fd, (struct sockaddr *)&printer_info, &addrlen)) == -1) {
	      perror("Printer accept err\n");
            } else {
       	      status = initAndInsertInSetOfPrinters(new_soc_fd, &printer_info,connectedNodes -> setOfPrinters); 
	      FD_SET(new_soc_fd, &original_fds); // add to master set
	      if (new_soc_fd > fd_max_val) {     // keep track of the maximum
		    fd_max_val = new_soc_fd;
	      }
		
	    }
	  }

	  else{    // handle data from a connection
	    
	    if ((len_byte = recv(i, buffer, sizeof(buffer), 0)) <= 0)
	      {  // got error or connection closed by client
		if (len_byte == 0) {    		// connection closed
		   printf("client : exited abnormally \n");
		} else {
		  perror("recv");
		}
		close(i); // bye! free related data structures
		FD_CLR(i, &original_fds); // remove from master set
	      } 
	    else 
	      {
		printf("Connection Message :- %s\n",buffer);
		analyzeAndProcessReceievedMsg(buffer,i,connectedNodes,wtQ);
	      }
	    }
	}
      }
    } 
}
//////////////////////////////////////////////////////////////

void  printSetOfPrinters(printer** setOfPrinters){
  int i;
  for(i=0;i<totalConnectedPrinters;i++)
    {
      printf("Printer Ip:- %s Priority :- %d\n",setOfPrinters[i]->ipAddr, setOfPrinters[i]->priority);
    }
}

//////////////////////////////////////////////////////////////

int main()
{
  /*
   *main basically creates socket,binds it ,listen requests ,does some error handling.
   *at last calls   main_send_recv_manager(&client_info, server_soc_fd);
   */
  struct sockaddr_in server_info, server_printer_info;
  int server_soc_fd, client_soc_fd, server_printer_soc_fd, val, yes=1;

  if( (server_soc_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { 
    perror("socket client");
    exit(1);
    }
  if( (server_printer_soc_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { 
    perror("socket printer");
    exit(1);
    }


  if (setsockopt(server_soc_fd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
    { 
    perror("setsockopt client");
    exit(1);
    } 
  if (setsockopt(server_printer_soc_fd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
    { 
    perror("setsockopt printer");
    exit(1);
    } 



  server_info.sin_family = AF_INET;   // host byte order
  server_info.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
  server_info.sin_port = htons(PORT_NO); // short, network byte order
  memset( &(server_info.sin_zero), '\0', 8);  // zero the rest of the struct
  if( ( val=bind(server_soc_fd, (struct sockaddr*)&server_info, sizeof(server_info)) ) == -1 )
    {
    perror("Client bind");
    exit(1);
    }

  server_printer_info.sin_family = AF_INET;   // host byte order
  server_printer_info.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
  server_printer_info.sin_port = htons(PRINTER_PORT); // short, network byte order
  memset( &(server_printer_info.sin_zero), '\0', 8);  // zero the rest of the struct
  if( ( val=bind(server_printer_soc_fd, (struct sockaddr*)&server_printer_info, sizeof(server_printer_info)) ) == -1 )
    {
    perror("Printer bind");
    exit(1);
    }



  if( listen( server_soc_fd, MAX_NO_OF_CLI_CONNC ) == -1 ) 
    {
    perror("Client listen");
    exit(1);
    }
  printf("Waiting fr Client connection\n");
  if( listen( server_printer_soc_fd, MAX_NO_OF_CLI_CONNC ) == -1 ) 
    {
    perror("Pinter listen");
    exit(1);
    }
  printf("Waiting fr printer connection\n");

  //Handles all incoming Cleint and Printer Connections

  printServerManager(server_soc_fd, server_printer_soc_fd);

  close(client_soc_fd);
  close(server_printer_soc_fd);
  close(server_soc_fd);

  return 0;
}
