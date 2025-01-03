#include "defs.h"

/*
   Function:  spawnFlyer
   Purpose:   initializes a flyer and adds it to the flyer array
   Parameters:
      in:      flyer avatar, row, col, and strength
      out:     flyer array with new flyer added
      return:  none
*/
void spawnFlyer(EscapeType *escape, char avatar, int row, int col, int strength)
{
   FlyerType *flyer = malloc(sizeof(FlyerType));

   flyer->partInfo.avatar = avatar;
   flyer->partInfo.pos.row = row;
   flyer->partInfo.pos.col = col;
   flyer->strength = strength;

   addFlyer(&(escape->flyers), flyer);
}

/*
   Function:  addFlyer
   Purpose:   adds a flyer to the flyer array
   Parameters:
      in:      flyer to add
      out:     flyer array with new flyer added
      return:  none
*/
void addFlyer(FlyerArrayType *flyerArray, FlyerType *flyer)
{
   flyerArray->elements[flyerArray->size] = flyer;
   flyerArray->size++;
   return;
}

/*
   Function:  removeFlyer
   Purpose:   removes a flyer from the flyer array
   Parameters:
      in:      flyer to remove
      out:     flyer array with flyer removed
      return:  none
*/
void removeFlyer(EscapeType *escape, FlyerType *flyer)
{
   for (int i = 0; i < escape->flyers.size; i++)
   {
      if (escape->flyers.elements[i] == flyer)
      {
         free(flyer);
         i++;
         for (; i < escape->flyers.size; i++)
         {
            escape->flyers.elements[i - 1] = escape->flyers.elements[i];
         }
         escape->flyers.size--;
      }
   }
}

/*
   Function:  moveFlyer
   Purpose:   moves a flyer based on its avatar
   Parameters:
      in:      flyer to move
      out:     flyer position set to new calculated position
      return:  none
*/
void moveFlyer(FlyerType *flyer, EscapeType *escape)
{
   int rowMove;
   int colMove;

   if (flyer->partInfo.avatar == 'v')
   {
      rowMove = 1;
      colMove = randomInt(3) - 1;
   }
   else if (flyer->partInfo.avatar == '@')
   {
      int dir;

      computeHeroDir(escape, flyer, &dir);

      rowMove = randomInt(7) - 3;
      colMove = (randomInt(3) + 1) * dir;
   }

   setPos(&(flyer->partInfo.pos), rowMove, colMove);

   if (flyer->partInfo.pos.row == MAX_ROW - 1)
   {
      HeroType *hero = checkForCollision(&(flyer->partInfo.pos), escape);

      if (hero != NULL)
      {
         incurDamage(hero, flyer);
      }
   }
}

/*
   Function:  computeHeroDir
   Purpose:   computes the direction of the nearest hero for flying monkeys
   Parameters:
      in:      escape structure and flyer
      out:     direction of nearest hero (DIR_LEFT, DIR_RIGHT, DIR_SAME)
      return:  none
*/
void computeHeroDir(EscapeType *escape, FlyerType *flyer, int *dir)
{
   int min_distance = MAX_COL;

   for (int i = 0; i < escape->heroes.size; i++)
   {
      HeroType *hero = escape->heroes.elements[i];

      if (hero->dead == C_FALSE && heroIsSafe(hero) == C_FALSE)
      {
         int distance = hero->partInfo.pos.col - flyer->partInfo.pos.col;

         if (abs(distance) < abs(min_distance))
         {
            min_distance = distance;
         }
      }
   }

   if (min_distance < 0)
   {
      *dir = DIR_LEFT;
   }
   else if (min_distance > 0)
   {
      *dir = DIR_RIGHT;
   }
   else
   {
      *dir = DIR_SAME;
   }
}

/*
   Function:  flyerIsDone
   Purpose:   checks if a flyer has reached the bottom of the hollow
   Parameters:
     in:      flyer to check
     return:  C_TRUE if flyer is done, C_FALSE otherwise
*/
int flyerIsDone(FlyerType *flyer)
{
   if (flyer->partInfo.pos.row == MAX_ROW - 1)
   {
      return C_TRUE;
   }
   return C_FALSE;
}

/*
   Function:  incurDamage
   Purpose:   incurs damage to a hero
   Parameters:
     in:      hero, flyer
     out:     hero updated status/health
     return:  none
*/
void incurDamage(HeroType *hero, FlyerType *flyer)
{
   hero->health -= flyer->strength;

   if (hero->health <= 0)
   {
      hero->health = 0;
      hero->dead = C_TRUE;
      hero->partInfo.avatar = '+';
   }
}