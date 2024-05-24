#include "defs.h"

/*
   Function:  outputHollow
   Purpose:   outputs the hollow by serializing and printing it, and sending it to the view
   Parameters:
     in:      escape
     out:     none
     return:  none
*/
void outputHollow(EscapeType *escape)
{
   char hollow[MAX_BUFF];

   serializeHollow(escape, hollow);

   printf("%s", hollow);

   sendData(escape->viewSocket, hollow);
}

/*
   Function:  initHollow
   Purpose:   initializes a 2D hollow with characters in position
   Parameters:
     in:      escape
     out:     2D hollow with characters in position
     return:  none
*/
void initHollow(EscapeType *escape, char hollow[][MAX_COL])
{
   for (int row = 0; row < MAX_ROW; row++)
   {
      for (int col = 0; col < MAX_COL; col++)
      {
         hollow[row][col] = ' ';
      }
   }

   for (int i = 0; i < escape->flyers.size; i++)
   {
      FlyerType *flyer = escape->flyers.elements[i];

      hollow[flyer->partInfo.pos.row][flyer->partInfo.pos.col] = flyer->partInfo.avatar;
   }

   for (int i = 0; i < escape->heroes.size; i++)
   {
      HeroType *hero = escape->heroes.elements[i];

      hollow[hero->partInfo.pos.row][hero->partInfo.pos.col] = hero->partInfo.avatar;
   }
}

/*
   Function:  serializeHollow
   Purpose:   serializes the hollow into a string
   Parameters:
     in:      escape
     out:     hollow string
     return:  none
*/
void serializeHollow(EscapeType *escape, char *hollow)
{
   char hollowGrid[MAX_ROW][MAX_COL];

   initHollow(escape, hollowGrid);

   int i = 0;
   int heroIndex = 0;

   for (int i = 0; i < escape->heroes.size; i++)
   {
      HeroType *hero = escape->heroes.elements[i];

      char healthStr[MAX_BUFF];
      sprintf(healthStr, "%s's Health: %d", hero->name, hero->health);
   }

   for (int col = 0; col < MAX_COL + 2; col++)
   {
      hollow[i++] = '=';
   }
   hollow[i++] = '\n';

   for (int row = 0; row < MAX_ROW; row++)
   {
      hollow[i++] = (row != MAX_ROW - 1 ? '|' : '=');

      for (int col = 0; col < MAX_COL; col++)
      {
         hollow[i++] = hollowGrid[row][col];
      }

      hollow[i++] = (row != MAX_ROW - 1 ? '|' : '=');

      if (heroIndex < escape->heroes.size)
      {
         HeroType *hero = escape->heroes.elements[heroIndex++];

         char healthStr[MAX_BUFF];
         int len = sprintf(healthStr, "\t%s's Health: %d", hero->name, hero->health);

         for (int j = 0; j < len; j++)
         {
            hollow[i++] = healthStr[j];
         }
      }

      hollow[i++] = '\n';
   }

   for (int col = 0; col < MAX_COL + 2; col++)
   {
      hollow[i++] = '=';
   }
   hollow[i++] = '\n';
   hollow[i++] = '\0';
}