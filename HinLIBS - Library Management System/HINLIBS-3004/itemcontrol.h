#ifndef ITEMCONTROL_H
#define ITEMCONTROL_H

#include "catalogueItem.h"
#include "patron.h"

class itemcontrol
{
public:
    void checkIn(CatalogueItem* item, Patron* patron);
    void checkOut(CatalogueItem* item, Patron* patron);
};

#endif // ITEMCONTROL_H
