#ifndef FICTION_H
#define FICTION_H

#include <string>
#include <book.h>

class Fiction : public Book {
public:
    Fiction(const std::string& t, const std::string& a, int y, int i,
            const std::string& f, const std::string& c,
            const std::string& s): Book(t, a, y, i, f, c, s) {};


};

#endif // FICTION_H
