#include "defs.h"

/*
  Purpose:  initializes a listen socket on a server
      out:  listen socket
*/
void setupServerSocket(int *s)
{
  struct sockaddr_in serverAddr;
  int currPort = MIN_PORT;
  int i;

  // Create listen socket
  *s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (*s < 0)
  {
    printf("Could not open socket\n");
    exit(-1);
  }

  // Setup server address
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons((unsigned short)currPort);

  // Bind listen socket

  i = bind(*s, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  while (i < 0 && currPort <= MAX_PORT)
  {
    serverAddr.sin_port = htons((unsigned short)++currPort);
    i = bind(*s, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  }

  if (i < 0)
  {
    printf("Could not bind socket\n");
    exit(-1);
  }

  // Listen on socket
  i = listen(*s, 5);
  if (i < 0)
  {
    printf("Could not setup listening on socket\n");
    exit(-1);
  }
}

/*
  Purpose:  blocks until a connection request comes in on the listen
            socket, and accepts the connection on client socket
       in:  listen socket
      out:  socket connected to the client
*/
void acceptConnection(int lSock, int *cSock)
{
  struct sockaddr_in clientAddr;
  socklen_t addrSize;

  // Wait for connection request
  addrSize = sizeof(clientAddr);

  *cSock = accept(lSock, (struct sockaddr *)&clientAddr, &addrSize);
  if (*cSock < 0)
  {
    printf("Could not accept the connection\n");
    exit(-1);
  }
}

/*
  Purpose:  initializes a socket on the client, and connects to the
            server at given IP address
       in:  IP address of server
      out:  socket connected to the server
*/
void setupClientSocket(int *s, char *ip)
{
  struct sockaddr_in addr;
  int i;
  int currPort = MIN_PORT;

  // Create socket
  *s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (*s < 0)
  {
    printf("Could not open socket\n");
    exit(-1);
  }

  // Setup address
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port = htons((unsigned short)currPort);

  // Connect to server

  i = connect(*s, (struct sockaddr *)&addr, sizeof(addr));

  while (i < 0 && currPort <= MAX_PORT)
  {
    addr.sin_port = htons((unsigned short)++currPort);
    i = connect(*s, (struct sockaddr *)&addr, sizeof(addr));
  }

  if (i < 0)
  {
    printf("Client could not connect\n");
    exit(-1);
  }
}

/*
  Purpose:  sends data over connected TCP/IP socket
       in:  connected socket
       in:  buffer of data to be sent
*/
void sendData(int s, char *buffer)
{
  send(s, buffer, strlen(buffer), 0);
}

/*
  Purpose:  block and receive data over connected TCP/IP socket
       in:  connected socket
      out:  buffer of data that was received
*/
int rcvData(int s, char *buffer)
{
  char tmpBuff[MAX_BUFF];
  int bytesRcv;

  // Read message from client
  bytesRcv = recv(s, tmpBuff, sizeof(tmpBuff), 0);
  tmpBuff[bytesRcv] = 0;
  strcpy(buffer, tmpBuff);

  return (bytesRcv);
}
