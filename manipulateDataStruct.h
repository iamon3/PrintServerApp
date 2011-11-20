//DataStructure Manipulate
#include "dataStructures.h"
void addTaskIntoPrinterJobQ(int noOfPages, int taskId,client* searchedClient,printer* availablePrinter);
void  addTaskIntoClientJobQ(int noOfPages,int taskId,client* searchedClient,printer* availablePrinter);
void addTaskInWaitingQ(int taskId, int noOfPages, client* searchedClient,waitingQ* wtQ);

client* removeFinishedTaskFromPrinterJobQ(printer* searchedPrinter);
int removeFinishedTaskFromClientJobQ(int taskIdFromPrinter, client* clientAllocated);
void removeTaskFromWaitingQ(waitingQ* wtQ,printer* assigndPrinter);

void removeCancelledTaskFromPrinterJobQ(int taskId,printer* assignedPrinter,client* searchedClient);
void removeCancelledTaskFromClientJobQ(int taskId, client* searchedClient);
void removeCancelledTaskFromWaitingQ(waitingQ* wtQ, client* searchedClient, int taskId);
