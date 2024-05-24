#include "defs.h"

/*
  Function:  runEscape
  Purpose:   runs the escape game
  Parameters:
    in:      none
    out:     the hollow printed out and streamed to client
    return:  none
*/
void runEscape()
{
   EscapeType escape;
   initEscape(&escape);

   do
   {
      int randSpawn = randomInt(100);

      if (randSpawn < BIRD_SPAWN_RATE)
      {
         int row = randomInt(4);
         int col = randomInt(MAX_COL);
         int strength = (randomInt(3) + 3);

         spawnFlyer(&escape, 'v', row, col, strength);
      }

      if (randSpawn < MONKEY_SPAWN_RATE)
      {
         int row = randomInt(14);
         int col = randomInt(MAX_COL);
         int strength = (randomInt(4) + 8);

         spawnFlyer(&escape, '@', row, col, strength);
      }

      for (int i = 0; i < escape.heroes.size; i++)
      {

         if (escape.heroes.elements[i]->dead == C_FALSE && heroIsSafe(escape.heroes.elements[i]) == C_FALSE)
         {
            moveHero(escape.heroes.elements[i]);
         }
      }

      for (int i = 0; i < escape.flyers.size; i++)
      {
         if (flyerIsDone(escape.flyers.elements[i]) == C_TRUE)
         {
            removeFlyer(&escape, escape.flyers.elements[i]);
         }
         else
         {
            moveFlyer(escape.flyers.elements[i], &escape);
         }
      }

      usleep(OUTPUT_INTERVAL);
      outputHollow(&escape);

   } while (escapeIsOver(&escape) == C_FALSE);

   usleep(OUTPUT_INTERVAL);
   handleEscapeResult(&escape);

   usleep(OUTPUT_INTERVAL);
   cleanupEscape(&escape);

   return;
}

/*
  Function:  escapeIsOver
  Purpose:   checks if the escape is over
  Parameters:
    in:      pointer to escape structure
    return:  C_TRUE if escape is over, C_FALSE otherwise
*/
int escapeIsOver(EscapeType *escape)
{
   for (int i = 0; i < escape->heroes.size; i++)
   {
      HeroType *hero = escape->heroes.elements[i];

      if (hero->dead == C_FALSE)
      {
         if (heroIsSafe(hero) == C_FALSE)
         {
            return C_FALSE;
         }
      }
   }
   return C_TRUE;
};

/*
  Function:  initEscape
  Purpose:   initializes the escape
  Parameters:
    in:      none
    out:     the escape structure initialized
    return:  none
*/
void initEscape(EscapeType *escape)
{
   srand(time(NULL));

   escape->heroes.size = 0;
   escape->flyers.size = 0;

   HeroType *timmy, *harold;

   initHero(&timmy, 'T', MAX_HEALTH, "Timmy");
   initHero(&harold, 'H', MAX_HEALTH, "Harold");

   addHero(&(escape->heroes), timmy);
   addHero(&(escape->heroes), harold);

   setupServerSocket(&(escape->listenSocket));
   acceptConnection(escape->listenSocket, &(escape->viewSocket));

   return;
}

/*
  Function:  handleEscapeResult
  Purpose:   handles the escape result (who died, who escaped) and streams/prints data out
  Parameters:
    in:      escape structure
    out:     none
    return:  none
*/
void handleEscapeResult(EscapeType *escape)
{
   for (int i = 0; i < escape->heroes.size; i++)
   {
      HeroType *hero = escape->heroes.elements[i];
      char str_output[MAX_BUFF];

      if (hero->dead == C_TRUE)
      {
         sprintf(str_output, "%s died!\n", hero->name);
      }
      else
      {
         sprintf(str_output, "%s escaped!\n", hero->name);
      }
      printf("%s", str_output);
      sendData(escape->viewSocket, str_output);
   }
}

/*
  Function:  cleanupEscape
  Purpose:   cleans up the escape (frees memory, closes sockets)
  Parameters:
    in:      escape structure
    out:     message to client to quit
    return:  none
*/
void cleanupEscape(EscapeType *escape)
{
   for (int i = 0; i < escape->heroes.size; i++)
   {
      free(escape->heroes.elements[i]);
   }

   for (int i = 0; i < escape->flyers.size; i++)
   {
      free(escape->flyers.elements[i]);
   }

   free(escape->heroes.elements);

   sendData(escape->viewSocket, "quit");

   close(escape->listenSocket);
   close(escape->viewSocket);
}