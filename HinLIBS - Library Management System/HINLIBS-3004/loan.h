#ifndef LOAN_H
#define LOAN_H

#include <string>


enum class LoanStatus { Available, Loaned};

class Loan {
public:
    Loan(): loanStatus(LoanStatus::Available), dueDate("None"), isbn(-1) {};
    std::string getDueDate() const { return dueDate; }
    LoanStatus getLoanStatus() const { return loanStatus; }
    int getIsbn() const { return isbn; }
    void setLoanStatus(const LoanStatus& s) { loanStatus = s; }
    void setDueDate(const std::string& d) { dueDate = d; }
    void setIsbn(const int i) { isbn = i; }
private:
    LoanStatus loanStatus;
    std::string dueDate;
    int isbn;
};

#endif // LOAN_H
