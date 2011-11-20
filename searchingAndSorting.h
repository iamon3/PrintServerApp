#include "dataStructures.h"
//Sorting and Serching
void sortSetOfPrinters(printer** setOfPrinters);
printer * isMsgRecvdFrmPrinter(int msgRecvdFd,printer** setOfPrinters);
client* isMsgRecvdFrmClient(int msgRecvFd,client** setOfClients);
printer * searchBestAvailablePrinter(printer** setOfPrinters);
clientJobQNode *searchPrevFinishedTaskToBeRemovedFromClientJobQ(int taskId, clientJobQNode* head);
printer* searchAssignedPrinterForTaskInClientJobQ(int taskId,clientJobQNode* currentJobQNode);
printerJobQNode* serachCancelledPrintTaskInJobQ(int taskId,printer* assignedPrinter,client* searchedClient);
waitingQNode* searchPrevTaskOfCancelTaskFromWaitingQ(waitingQ* wtQ ,client* searchedClient, int taskId);
