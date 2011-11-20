#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

struct printerJobQNode{
  int taskId;
  int flag;   // 1 - currently in process
  int noOfPagesToBePrinted;
  struct  client* clientAlloted;
  struct printerJobQNode* next;
};

struct printer{
  char ipAddr[64];
  int printerFd;
  int priority;
  int speed;         // Pages / second
  int noOfPagesRemaining;
  int status;         // buzy-1, idle-0
  int noOfTaskAssigned;
  struct printerJobQNode* rare;
  struct printerJobQNode* front;
};

struct clientJobQNode{
  int taskId;
  int noOfPagesToBePrinted;
  struct printer* allotedPrinter;
  struct clientJobQNode* next;  
};

struct client{
  char ipAddr[64];
  int clientFd;
  struct clientJobQNode* lastJob;
  struct clientJobQNode* startJob; // This can also kept as array incresing order 4 efficiency
};

struct allConnections{
  struct client** setOfClients;
  struct printer** setOfPrinters;
};

struct waitingQNode{
  int taskId;
  int noOfPages;
  struct client* commandFromClient;
  struct waitingQNode* next;
};

struct waitingQ{
  struct waitingQNode* head;
  struct waitingQNode* tail;
};

typedef struct client client;
typedef struct printer printer;
typedef struct clientJobQNode clientJobQNode;
typedef struct printerJobQNode printerJobQNode;
typedef struct allConnections allConnections;
typedef struct waitingQNode waitingQNode;
typedef struct waitingQ waitingQ;

#endif
