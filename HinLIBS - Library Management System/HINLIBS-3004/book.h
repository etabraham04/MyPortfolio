#ifndef BOOK_H
#define BOOK_H

#include <string>
#include "catalogueItem.h"


class Book : public CatalogueItem{

public:

    Book(const std::string& t, const std::string& a, int y, int i,
         const std::string& f, const std::string& c,
         const std::string& s)
        : CatalogueItem(t, a , y, i, f, c, s){}

    virtual ~Book() = default;




};

#endif // BOOK_H
