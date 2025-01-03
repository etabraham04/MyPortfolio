#include "defs.h"

/*
   Function:   initHero
   Purpose:   initializes a hero
   Parameters:
     in:      hero avatar, health, and name
     out:     hero
     return:  none
*/
void initHero(HeroType **hero, char avatar, int health, char *name)
{
   *hero = malloc(sizeof(HeroType));

   (*hero)->partInfo.avatar = avatar;
   (*hero)->partInfo.pos.col = 0;
   (*hero)->partInfo.pos.row = MAX_ROW;

   (*hero)->health = health;

   strcpy((*hero)->name, name);

   (*hero)->dead = C_FALSE;
}

/*
   Function:  addHero
   Purpose:   adds a hero to the hero array
   Parameters:
     in:      hero to add
     out:     hero array with new hero added
     return:  none
*/
void addHero(HeroArrayType *heroArray, HeroType *hero)
{
   HeroArrayType *tmpArr = malloc(sizeof(HeroArrayType));

   tmpArr->elements = calloc(sizeof(HeroType), heroArray->size + 1);

   for (int i = 0; i < heroArray->size; i++)
   {
      tmpArr->elements[i] = heroArray->elements[i];
   }

   tmpArr->elements[heroArray->size] = hero;
   tmpArr->size = heroArray->size + 1;

   if (heroArray->size > 0)
   {
      free(heroArray->elements);
   }

   *heroArray = *tmpArr;

   free(tmpArr);
}

/*
   Function:  moveHero
   Purpose:   moves a hero
   Parameters:
     in:      none
     out:     hero moved
     return:  none
*/
void moveHero(HeroType *hero)
{
   int randInt = randomInt(100);
   int colMove = 0;

   if (hero->partInfo.avatar == 'T')
   {
      if (randInt < 50)
      {
         colMove += 2;
      }
      else if (randInt < 80)
      {
         colMove -= 1;
      }
      else if (randInt < 100)
      {
         colMove += 1;
      }
   }

   else if (hero->partInfo.avatar == 'H')
   {
      if (randInt < 20)
      {
         colMove += 0;
      }
      else if (randInt < 30)
      {
         colMove += 5;
      }
      else if (randInt < 40)
      {
         colMove -= 4;
      }
      else if (randInt < 80)
      {
         colMove += 3;
      }
      else if (randInt < 100)
      {
         colMove -= 2;
      }
   }

   if (hero->partInfo.pos.col + colMove >= MAX_COL)
   {
      hero->partInfo.avatar = ' ';
   }

   setPos(&(hero->partInfo.pos), MAX_ROW - 1, colMove);
}

/*
   Function:  heroIsSafe
   Purpose:   checks if a hero is safe
   Parameters:
     in:      hero
     out:     none
     return:  C_TRUE if hero is safe, C_FALSE otherwise
*/
int heroIsSafe(HeroType *hero)
{
   if (hero->partInfo.avatar == ' ')
   {
      return C_TRUE;
   }

   return C_FALSE;
}