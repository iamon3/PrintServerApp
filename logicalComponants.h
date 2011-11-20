//logicalComponants
#include "dataStructures.h"
int decodeMsg(char* msg);
void processAndForwardMsgToClient(char* msg,printer* searchedPrinter,allConnections* connectedNodes,waitingQ* wtQ);
int processAndCommandToPrinter(char* msg, client* searchedClient, allConnections* connectedNodes, waitingQ* wtQ);
void analyzeAndProcessReceievedMsg(char* buffer,int megRecvedFd, allConnections* connectedNodes,waitingQ* wtQ);
//void updateWaitingQAndOtherJobQs(waitingQ* wtQ,allConnections* connectedNodes);
