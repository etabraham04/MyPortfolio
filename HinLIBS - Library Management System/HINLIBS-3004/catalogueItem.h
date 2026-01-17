#ifndef CATALOGUEITEM_H
#define CATALOGUEITEM_H

#include <string>
#include <iostream>
#include <deque>
#include <algorithm>

class CatalogueItem {
public:
    CatalogueItem(const std::string& t, const std::string& a, int y, int i,
                  const std::string& f, const std::string& c,
                  const std::string& s)
        : title(t), author(a), publicationYear(y), isbn(i),
        format(f), condition(c), circulationStatus(s) {}
    CatalogueItem(const CatalogueItem&) = delete; // prevent a item from being copied, only one should exist
    CatalogueItem& operator=(const CatalogueItem&) = delete;

    // member functions
    virtual void print() const {
            std::cout << "Item: " << title << std::endl;
        }
    virtual const std::string& get_title() const { return title; }
    virtual const int& get_publication_year() const { return publicationYear; }
    virtual const std::string& get_circulationStatus() const { return circulationStatus; }
    // setter added to allow librarian to toggle circulation status in testing **
    virtual void set_circulationStatus(const std::string& status) { circulationStatus = status; }

    virtual const std::string& get_author() const { return author; }
    virtual const std::string& get_format() const { return format; }
    virtual const std::string& get_condition() const { return condition; }

    int get_isbn() const { return isbn; }

    virtual ~CatalogueItem() = default;

protected:
    std::string title;
    std::string author;
    int publicationYear;
    int isbn;
    std::string format;
    std::string condition;
    std::string circulationStatus;
};

#endif // CATALOGUEITEM_H
