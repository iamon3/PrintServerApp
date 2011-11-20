#include "dataStructures.h"
#include <netinet/in.h>
// Initialization
client* createNewClient();
allConnections*  createAllConnectionStruct();
printer * createNewPrinter();
printerJobQNode* createNewPrinterJobQNode(int noOfPages, int taskId,client* searchedClient);
clientJobQNode* createNewClientJobQNode(int noOfPages,int taskId, printer * availablePrinter);
int initAndInsertInSetOfClients(int newClientFd, struct sockaddr_in* client_info,client** setOfClients);
int initAndInsertInSetOfPrinters(int newPrinterSocFd, struct sockaddr_in* printerInfo,printer** setOfPrinters);
void initNewClient(client* newClient,int newClientFd,struct sockaddr_in* newClientInfo );
void initNewPrinter(printer* newPrinter, int newPrinterSocFd, struct sockaddr_in* printerInfo,int priority, int speed);
waitingQ* createWaitingQ();
waitingQNode* createWaitingQNode();
void initWaitingQNode(int taskId, int noOfPages, client* searchedClient, waitingQNode* newNode);
