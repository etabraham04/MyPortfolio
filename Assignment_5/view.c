#include "defs.h"

/*
   Function:  viewEscape
   Purpose:   views the escape from server communication
   Parameters:
     in:      ip addressof server
     out:     none
     return:  none
*/
void viewEscape(char *ip)
{
    int vSocket;

    setupClientSocket(&vSocket, ip);

    char buffer[MAX_BUFF];

    do
    {
        int bytesRcv = rcvData(vSocket, buffer);
        buffer[bytesRcv] = 0;

        if (strcmp(buffer, "quit") == 0)
        {
            break;
        }

        printf("%s", buffer);

    } while (1);

    close(vSocket);

    return;
}