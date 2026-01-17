#ifndef NONFICTION_H
#define NONFICTION_H

#include <string>
#include <book.h>

class NonFiction : public Book {
public:
    NonFiction(const std::string& t, const std::string& a, int y, int i,
               const std::string& f, const std::string& c,
               const std::string& s, int dNum)
    : Book(t, a, y, i, f, c, s), deweyDecimalNumber(dNum) {};

    int get_dewey_decimal() {return deweyDecimalNumber;}

   private:
       int deweyDecimalNumber;
};



#endif // NONFICTION_H
