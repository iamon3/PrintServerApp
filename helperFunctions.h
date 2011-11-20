#include "dataStructures.h"
//HelperFunctions

int  addPrintTaskInJobQAndAssignToBestPrinter(char* msg, client* searchedClient, allConnections* connectedNodes,waitingQ* wtQ);
void forwardCancelSuccessToOthrFuct(allConnections* connectedNodes,printer* searchedPrinter,waitingQ* wtQ);

void findPrintTaskInJobQAndSendCancelToPrinter(char* msg, client* searchedClient, allConnections* connectedNodes,waitingQ* wtQ);

void removeTaskFromWtQAndAssignToBestPrinter(waitingQ* wt,allConnections* connectedNodes,printer* searchedPrinter);
void removeTaskFromWaitingQ(waitingQ* wtQ,printer* assignPrinter);
void removeFinishedPrintTaskFromJobQsAndReplyToClient( char* msg,printer* searchedPrinter, allConnections* connectedNodes,waitingQ* wtQ);
void removeCancelledPrintTaskFromJobQsAndReplyToClient(int taskId, printer* assignedPrinter,client* searchedClient);

void updateWaitingQAndOtherJobQs(waitingQ* wtQ,allConnections* connectedNodes,printer* searchedPrinter);
void forwardStatusToClient(char* msg,printer* searchedPrinter);
void findPrinterInJobQAndAskStatusToPrinter(char* msg,client* searchedClient,allConnections* connectedNodes,waitingQ* wtQ);
