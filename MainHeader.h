#include "dataStructures.h"

#define MAX_NO_OF_CLI_CONNC 10  // max no of clients to be connected. 
#define MAX_NO_OF_PRINTER_CONNC 10 // max no of printers to be connected. 
#define PORT_NO 9034            
#define PRINTER_PORT 9007
#define THRESHOLD_NO_OF_REQ 5



void  printSetOfPrinters(printer** setOfPrinters);
void executePrinting(allConnections* connectedNodes);

int count, totalConnectedPrinters, totalConnectedClients;

