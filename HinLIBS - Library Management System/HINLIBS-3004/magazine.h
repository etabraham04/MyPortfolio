#ifndef MAGAZINE_H
#define MAGAZINE_H

#include <string>
#include <catalogueItem.h>

class Magazine : public CatalogueItem {
public:
    Magazine(const std::string& t, const std::string& a, int y, int i,
             const std::string& f, const std::string& c,
             const std::string& s,
             int issueNum, const std::string& pubDate)
        : CatalogueItem(t, a, y, i, f, c, s), issueNumber(issueNum), publicationDate(pubDate) {};

    int get_issue_number() { return issueNumber; }
    std::string get_publication_date() { return publicationDate; }
private:
    int issueNumber;
    std::string publicationDate;
};

#endif // MAGAZINE_H
