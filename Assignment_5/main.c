#include "defs.h"

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    runEscape();
  }
  else if (argc == 2)
  {
    viewEscape(argv[1]);
  }
  return (0);
}

/*
  Function:  randomInt
  Purpose:   returns a random number in the range [0,max)
  Parameters:
    in:      maximum of range
    return:  randomly generated number
*/
int randomInt(int max)
{
  return (rand() % max);
}
