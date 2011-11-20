//Communication
void sendFinishedPrintingTaskResponseToClient(int clientSocFd,int taskId, char* result, char* ipAddr);
int sendPrintCommandToPrinter(unsigned int timeToWait,char* ipAddre,int printerFd );
int sendCancelCommandToPrinter(char* ipAddr,int printerFd );
void getPrinterInfo(int printerFd,char* buffer);
void sendStatusMsgToClient(int noOfPagesRemaining,int taskId,char* printerIpAddr, int clientFd);
void askStatusMsgToPrinter(int taskId,int printerFd,char* printerIpAddr,char* clientIpAddr);
