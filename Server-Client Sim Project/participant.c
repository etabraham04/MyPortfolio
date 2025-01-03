#include "defs.h"

/*
   Function:  setPos
   Purpose:   sets the position of a participant
   Parameters:
     in:      row and col to set participant
     out:     position of participant set
     return:  none
*/
void setPos(PositionType *pos, int row, int col)
{
    pos->row += row;
    pos->col += col;

    if (pos->row < 0)
    {
        pos->row = 0;
    }

    else if (pos->row >= MAX_ROW)
    {
        pos->row = MAX_ROW - 1;
    }

    if (pos->col < 0)
    {
        pos->col = 0;
    }

    else if (pos->col >= MAX_COL)
    {
        pos->col = MAX_COL - 1;
    }
}

/*
   Function:  checkForCollision
   Purpose:   checks if a hero collides with a flyer
   Parameters:
     in:      position of flyer, escape struct
     out:     none
     return:  hero that collided with flyer or NULL if no collision
*/
HeroType *checkForCollision(PositionType *pos, EscapeType *escape)
{
    for (int i = 0; i < escape->heroes.size; i++)
    {
        HeroType *hero = escape->heroes.elements[i];

        // if hero is alive and not safe, check for collision
        if (hero->dead == C_FALSE && heroIsSafe(hero) == C_FALSE)
        {
            if (pos->col == hero->partInfo.pos.col)
            {
                return hero;
            }
        }
    }
    return NULL;
}