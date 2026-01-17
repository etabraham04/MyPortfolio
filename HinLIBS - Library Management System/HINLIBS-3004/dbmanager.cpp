#include "dbmanager.h"

DBManager::DBManager()
{
    QString dbPath = "hinlibs.db";

    if (!QFileInfo::exists(dbPath))
    {
        // Fallback: Check application directory
        dbPath = QCoreApplication::applicationDirPath() + "/hinlibs.db";
        if (!QFileInfo::exists(dbPath)) {
             qDebug() << "CRITICAL ERROR: Database file not found at:" << dbPath;
             qDebug() << "Please ensure the database file exists before running.";
             qDebug() << "Checked paths: " << QDir::currentPath() << " and " << QCoreApplication::applicationDirPath();
             return;
        }
    }

    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection))
    {
        db = QSqlDatabase::database(QSqlDatabase::defaultConnection);
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbPath);
    }

    if (!db.open())
    {
        qDebug() << "ERROR: Cannot open database" << db.lastError().text();
    }
    else
    {
        qDebug() << "SUCCESS: Connected to database!";
        QSqlQuery query;
        if (!query.exec("PRAGMA foreign_keys = ON;"))
        {
            qDebug() << "WARNING: Failed to enable foreign keys:" << query.lastError().text();
        }
    }
}

bool DBManager::checkIn(CatalogueItem *item, Patron *patron)
{
    if (!item || !patron)
    {
        qDebug() << "Error, null inputs goven to checkIn";
        return false;
    }

    if (!db.transaction())
    {
        qDebug() << "Error: Failed to start database transaction";
        return false;
    }

    // extract data
    int isbn = item->get_isbn();
    QString username = QString::fromStdString(patron->get_username());
    QString today = QDate::currentDate().toString(Qt::ISODate);

    QSqlQuery query;
    query.prepare("UPDATE loans SET return_date = :today "
                  "WHERE isbn = :isbn AND username = :user AND return_date IS NULL");
    query.bindValue(":today", today);
    query.bindValue(":isbn", isbn);
    query.bindValue(":user", username);
    if (!query.exec())
    {
        qDebug() << "SQL Error updating loans table:" << query.lastError();
        db.rollback();
        return false;
    }
    else if (query.numRowsAffected() == 0)
    {
        qDebug() << "Transaction Aborted: Patron " << username << " does not have an active loan for " << isbn;
        db.rollback();
        return false;
    }

    if (!db.commit())
    {
        qDebug() << "Commit failed:" << db.lastError();
        db.rollback();
        return false;
    }

    return true;
}

bool DBManager::checkOut(CatalogueItem *item, Patron *patron)
{
    if (!item || !patron)
    {
        qDebug() << "Error, null inputs goven to checkOut";
        return false;
    }

    if (!db.transaction())
    {
        qDebug() << "Error: Failed to start database transaction";
        return false;
    }

    int isbn = item->get_isbn();
    QString username = QString::fromStdString(patron->get_username());
    QDate today = QDate::currentDate();
    QDate dueDate = today.addDays(14);

    QSqlQuery query;

    // Is item avaliabble for checkout?
    query.prepare("SELECT COUNT(*) FROM loans WHERE isbn = :isbn AND return_date IS NULL");
    query.bindValue(":isbn", isbn);
    if (!query.exec() || !query.next())
    {
        db.rollback();
        return false;
    }

    if (query.value(0).toInt() > 0)
    {
        qDebug() << "Checkout Failed: Item " << isbn << " is already checked out.";
        db.rollback();
        return false;
    }

    // does the  patronn have 3 loans already
    query.prepare("SELECT COUNT(*) FROM loans WHERE username = :user AND return_date IS NULL");
    query.bindValue(":user", username);
    if (!query.exec() || !query.next())
    {
        db.rollback();
        return false;
    }

    int currentLoans = query.value(0).toInt();
    if (currentLoans >= 3)
    {
        qDebug() << "Checkout Failed: Patron: " << username << " has reached the 3-item limit.";
        db.rollback();
        return false;
    }

    // if user has a hold.are they next? if so, deleete the hold from the table before creeating loan
    if (hasHolds(item))
    {
        int position = getHoldPosition(item, patron);

        if (position != 0)
        {
            qDebug() << "Checkout Failed: Item is reserved for another patron.";
            db.rollback();
            return false;
        }

        QSqlQuery deleteHold_query;
        deleteHold_query.prepare("DELETE FROM holds WHERE isbn = :isbn AND username = :user");
        deleteHold_query.bindValue(":isbn", isbn);
        deleteHold_query.bindValue(":user", username);
        if (!deleteHold_query.exec())
        {
            qDebug() << "Error removing hold during checkout.";
            db.rollback();
            return false;
        }
    }

    QSqlQuery addLoan_query;

    addLoan_query.prepare("INSERT INTO loans (isbn, username, loan_date, due_date) "
                          "VALUES (:isbn, :user, :date, :due)");
    addLoan_query.bindValue(":isbn", isbn);
    addLoan_query.bindValue(":user", username);
    addLoan_query.bindValue(":date", today.toString(Qt::ISODate));
    addLoan_query.bindValue(":due", dueDate.toString(Qt::ISODate));

    if (!addLoan_query.exec())
    {
        qDebug() << "Checkout SQL failed:" << addLoan_query.lastError();
        db.rollback();
        return false;
    }

    if (!db.commit())
    {
        qDebug() << "Commit failed:" << db.lastError();
        db.rollback();
        return false;
    }

    qDebug() << "Checkout Successful: " << username << " borrowed " << isbn;
    return true;
}

std::vector<Loan *> DBManager::getActiveLoans(std::string username)
{
    std::vector<Loan *> activeLoans;

    if (username.empty())
    {
        qDebug() << "Error: Empty username passed to getActiveLoans";
        return activeLoans;
    }
    QSqlQuery query;
    query.prepare("SELECT * FROM loans WHERE username = :user AND return_date IS NULL");
    query.bindValue(":user", QString::fromStdString(username));

    if (!query.exec())
    {
        qDebug() << "Failed to fetch loans for user " << QString::fromStdString(username) << ":" << query.lastError();
        return activeLoans;
    }

    while (query.next())
    {
        QString dueDate = query.value("due_date").toString();
        int isbn = query.value("isbn").toInt();

        Loan *loan = new Loan();
        loan->setLoanStatus(LoanStatus::Loaned);
        loan->setDueDate(dueDate.toStdString());
        loan->setIsbn(isbn);
        activeLoans.push_back(loan);
    }

    return activeLoans;
}

bool DBManager::applyHold(CatalogueItem *item, Patron *patron)
{
    // check input
    if (!item || !patron)
    {
        qDebug() << "Error: Null inputs passed to applyHold";
        return false;
    }
    if (!db.transaction())
    {
        qDebug() << "Error: Failed to start transaction";
        return false;
    }

    // extract data
    int isbn = item->get_isbn();
    QString username = QString::fromStdString(patron->get_username());
    QString today = QDate::currentDate().toString(Qt::ISODate);

    // check first if item  is available
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM loans WHERE isbn = :isbn AND return_date IS NULL");
    query.bindValue(":isbn", isbn);

    if (!query.exec() || !query.next())
    {
        db.rollback();
        return false;
    }

    if (query.value(0).toInt() == 0)
    {
        qDebug() << "Hold Failed: Item " << isbn << " is currently Available. Please borrow it directly.";
        db.rollback();
        return false;
    }

    // check if the patron  already has a hold on this book
    query.prepare("SELECT COUNT(*) FROM holds WHERE isbn = :isbn AND username = :user");
    query.bindValue(":isbn", isbn);
    query.bindValue(":user", username);
    if (!query.exec() || !query.next())
    {
        db.rollback();
        return false;
    }
    if (query.value(0).toInt() > 0)
    {
        qDebug() << "Hold Failed: Patron " << username << " already has a hold on this item.";
        db.rollback();
        return false;
    }

    //  else create hold tuple with correct position
    query.prepare("SELECT MAX(position) FROM holds WHERE isbn = :isbn");
    query.bindValue(":isbn", isbn);
    if (!query.exec() || !query.next())
    {
        db.rollback();
        return false;
    }

    int nextPosition = 0;
    if (!query.value(0).isNull())
    {
        nextPosition = query.value(0).toInt() + 1;
    }

    QSqlQuery insertHold_query;
    insertHold_query.prepare("INSERT INTO holds (isbn, username, hold_date, position) "
                             "VALUES (:isbn, :user, :date, :pos)");
    insertHold_query.bindValue(":isbn", isbn);
    insertHold_query.bindValue(":user", username);
    insertHold_query.bindValue(":date", today);
    insertHold_query.bindValue(":pos", nextPosition);

    if (!insertHold_query.exec())
    {
        qDebug() << "Apply Hold SQL failed:" << insertHold_query.lastError();
        db.rollback();
        return false;
    }

    if (!db.commit())
    {
        qDebug() << "Commit failed:" << db.lastError();
        db.rollback();
        return false;
    }

    qDebug() << "Hold Applied: " << username << " is pos #" << nextPosition << " for item " << isbn;
    return true;
}

bool DBManager::cancelHold(CatalogueItem *item, Patron *patron)
{
    if (!item || !patron)
    {
        qDebug() << "Error: Null inputs passed to cancelHold";
        return false;
    }

    if (!db.transaction())
    {
        qDebug() << "Error: Failed to start transaction";
        return false;
    }

    // extract data
    int isbn = item->get_isbn();
    QString username = QString::fromStdString(patron->get_username());

    // get position of hold to be removed
    QSqlQuery query;
    query.prepare("SELECT position FROM holds WHERE isbn = :isbn AND username = :user");
    query.bindValue(":isbn", isbn);
    query.bindValue(":user", username);

    if (!query.exec())
    {
        qDebug() << "Error finding hold to cancel:" << query.lastError();
        db.rollback();
        return false;
    }

    if (!query.next())
    {
        qDebug() << "Cancel Failed: Patron " << username << " does not have a hold on item " << isbn;
        db.rollback();
        return false;
    }

    int positionToRemove = query.value("position").toInt();

    // delete the hold
    QSqlQuery delete_query;
    delete_query.prepare("DELETE FROM holds WHERE isbn = :isbn AND username = :user");
    delete_query.bindValue(":isbn", isbn);
    delete_query.bindValue(":user", username);

    if (!delete_query.exec())
    {
        qDebug() << "Error deleting hold:" << delete_query.lastError();
        db.rollback();
        return false;
    }

    // shift positions of holds behind the cancelled hold
    QSqlQuery shift_query;
    shift_query.prepare("UPDATE holds SET position = position - 1 "
                        "WHERE isbn = :isbn AND position > :removedPos");

    shift_query.bindValue(":isbn", isbn);
    shift_query.bindValue(":removedPos", positionToRemove);

    if (!shift_query.exec())
    {
        qDebug() << "Error shifting hold positions:" << shift_query.lastError();
        db.rollback();
        return false;
    }

    if (!db.commit())
    {
        qDebug() << "Commit failed:" << db.lastError();
        db.rollback();
        return false;
    }

    qDebug() << "Hold Cancelled for " << username << ". Queue shifted successfully.";
    return true;
}

std::vector<Patron *> DBManager::getItemHolds(CatalogueItem *item)
{
    std::vector<Patron *> holdQueue;

    if (!item)
    {
        qDebug() << "Error: Null item passed to getItemHolds";
        return holdQueue;
    }

    int isbn = item->get_isbn();

    QSqlQuery query;
    query.prepare("SELECT u.username, u.password, u.email, u.card_number, h.position "
                  "FROM holds h "
                  "JOIN users u ON h.username = u.username "
                  "WHERE h.isbn = :isbn "
                  "ORDER BY h.position ASC");
    query.bindValue(":isbn", isbn);

    if (!query.exec())
    {
        qDebug() << "Failed to fetch holds for item:" << query.lastError();
        return holdQueue;
    }

    while (query.next())
    {
        std::string username = query.value("username").toString().toStdString();
        std::string password = query.value("password").toString().toStdString();
        std::string email = query.value("email").toString().toStdString();
        int cardNumber = query.value("card_number").toInt();

        Patron *patron = new Patron(username, password, email, cardNumber);
        holdQueue.push_back(patron);
    }
    return holdQueue;
}

std::vector<CatalogueItem *> DBManager::getPatronHolds(Patron *patron)
{
    std::vector<CatalogueItem *> items_on_hold;

    if (!patron)
    {
        qDebug() << "Error: Null patron passed to getPatronHolds";
        return items_on_hold;
    }

    QSqlQuery query;
    query.prepare("SELECT i.*, h.position, "
                  "b.book_type, n.dewey_decimal, "
                  "mag.issue_number, mag.publication_date, "
                  "mov.genre AS movie_genre, mov.rating AS movie_rating, "
                  "vg.genre AS game_genre, vg.rating AS game_rating "
                  "FROM holds h "
                  "JOIN items i ON h.isbn = i.isbn "
                  "LEFT JOIN books b ON i.isbn = b.isbn "
                  "LEFT JOIN nonfiction_books n ON i.isbn = n.isbn "
                  "LEFT JOIN magazines mag ON i.isbn = mag.isbn "
                  "LEFT JOIN movies mov ON i.isbn = mov.isbn "
                  "LEFT JOIN videogames vg ON i.isbn = vg.isbn "
                  "WHERE h.username = :user "
                  "ORDER BY h.hold_date ASC");

    query.bindValue(":user", QString::fromStdString(patron->get_username()));

    if (!query.exec())
    {
        qDebug() << "Failed to fetch holds for user:" << query.lastError();
        return items_on_hold;
    }

    while (query.next())
    {
        // CatalogueItem base stuff
        int isbn = query.value("isbn").toInt();
        std::string title = query.value("title").toString().toStdString();
        std::string author = query.value("author").toString().toStdString();
        int year = query.value("publication_year").toInt();
        std::string format = query.value("format").toString().toStdString();
        std::string condition = query.value("condition").toString().toStdString();
        std::string status = query.value("circulation_status").toString().toStdString();

        QString itemType = query.value("item_type").toString();
        CatalogueItem *newItem = nullptr;

        if (itemType == "Book")
        {
            QString bookType = query.value("book_type").toString();
            if (bookType == "NonFiction")
            {
                int dewey = query.value("dewey_decimal").toInt();
                newItem = new NonFiction(title, author, year, isbn, format, condition, status, dewey);
            }
            else
            {
                newItem = new Fiction(title, author, year, isbn, format, condition, status);
            }
        }
        else if (itemType == "Magazine")
        {
            int issue = query.value("issue_number").toInt();
            std::string pubDate = query.value("publication_date").toString().toStdString();
            newItem = new Magazine(title, author, year, isbn, format, condition, status, issue, pubDate);
        }
        else if (itemType == "Movie")
        {
            std::string genre = query.value("movie_genre").toString().toStdString();
            std::string rating = query.value("movie_rating").toString().toStdString();
            newItem = new Movie(title, author, year, isbn, format, condition, status, genre, rating);
        }
        else if (itemType == "VideoGame")
        {
            std::string genre = query.value("game_genre").toString().toStdString();
            std::string rating = query.value("game_rating").toString().toStdString();
            newItem = new VideoGame(title, author, year, isbn, format, condition, status, genre, rating);
        }
        else
        {
            qDebug() << "Warning: Unknown item type " << itemType << " in holds list.";
            continue;
        }

        if (newItem)
        {
            items_on_hold.push_back(newItem);
        }
    }

    return items_on_hold;
}

int DBManager::getHoldPosition(CatalogueItem *item, Patron *patron)
{
    if (!item)
    {
        qDebug() << "Error: Null item passed to getHoldPosition";
        return -1;
    }
    if (!patron)
    {
        qDebug() << "Error: Null patron passed to getHoldPosition";
        return -1;
    }

    // get data
    int isbn = item->get_isbn();
    QString username = QString::fromStdString(patron->get_username());

    QSqlQuery query;
    query.prepare("SELECT position FROM holds WHERE isbn = :isbn AND username = :username");
    query.bindValue(":isbn", isbn);
    query.bindValue(":username", username);

    if (!query.exec())
    {
        qDebug() << "Failed to get holds for book: " << isbn;
        return -1;
    }

    if (!query.next())
    {
        qDebug() << "No record of a hold found with username, isbn: " << isbn << ", " << username;
        return -1;
    }

    return query.value("position").toInt();
}

bool DBManager::hasHolds(CatalogueItem *item)
{
    if (!item)
    {
        qDebug() << "Error: Null item passed to hasHolds";
        return false;
    }

    // get isbn
    auto isbn = item->get_isbn();

    QSqlQuery query;
    query.prepare("SELECT 1 FROM holds WHERE isbn = :isbn");
    query.bindValue(":isbn", isbn);

    if (!query.exec())
    {
        qDebug() << "Failed to get holds for book: " << isbn;
        return false;
    }

    return query.next();
};

bool DBManager::readyForPickup(CatalogueItem *item, Patron *patron)
{
    int holdPosition = getHoldPosition(item, patron);
    auto currentLoan = getLoan(item);
    // Crash guard: getLoan may return nullptr on query failure
    if (!currentLoan) {
        qDebug() << "readyForPickup: currentLoan is null; treating as not ready";
        return false;
    }

    bool isItemAvailable = (currentLoan->getLoanStatus() == LoanStatus::Available);

    delete currentLoan; // free loan object allocated in getLoan

    bool ready = (holdPosition == 0) && isItemAvailable;
    return ready;
}

bool DBManager::addUser(Patron *patron)
{
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, user_type, email, card_number) "
                  "VALUES (:username, :password, :type, :email, :card)");

    // bind values
    query.bindValue(":username", QString::fromStdString(patron->get_username()));
    query.bindValue(":password", QString::fromStdString(patron->get_password()));
    query.bindValue(":type", QString::fromStdString(patron->getUserType()));
    query.bindValue(":email", QString::fromStdString(patron->get_email()));
    query.bindValue(":card", patron->get_card_number());

    if (!query.exec())
    {
        qDebug() << "addUser error:" << query.lastError();
        return false;
    }

    return true;
}

std::vector<User *> DBManager::getAllUsers()
{
    QSqlQuery query;
    query.prepare("SELECT * FROM users");

    std::vector<User *> users;

    if (!query.exec())
    {
        qDebug() << "ERROR: getAllUsers query.exec() failed";
        return users;
    }

    while (query.next())
    {
        QString type = query.value("user_type").toString();
        QString username = query.value("username").toString();
        QString password = query.value("password").toString();

        if (type == "Patron")
        {
            QString email = query.value("email").toString(); // empty if NULL
            QVariant qcard = query.value("card_number");
            int card = qcard.isNull() ? -1 : qcard.toInt();

            users.push_back(new Patron(username.toStdString(),
                                       password.toStdString(),
                                       email.toStdString(),
                                       card));
        }
        else if (type == "Librarian")
        {
            users.push_back(new Librarian(username.toStdString(), password.toStdString()));
        }
        else if (type == "Admin")
        {
            users.push_back(new Admin(username.toStdString(), password.toStdString()));
        }
    }
    return users;
}

User *DBManager::getUser(std::string username)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :username");
    query.bindValue(":username", QString::fromStdString(username));

    if (!query.exec())
    {
        qDebug() << "ERROR: getUser query.exec() failed for" << QString::fromStdString(username)
                 << ":" << query.lastError().text();
        return nullptr;
    }

    if (!query.next())
    {
        qDebug() << "INFO: No user found for username:" << QString::fromStdString(username);
        return nullptr;
    }

    QString type = query.value("user_type").toString();
    QString password = query.value("password").toString();

    // Use QString comparisons (case-sensitive). Adjust if you want case-insensitive.
    if (type == "Patron")
    {
        QString email = query.value("email").toString(); // empty if NULL
        QVariant qcard = query.value("card_number");
        int card = qcard.isNull() ? -1 : qcard.toInt(); // -1 indicates missing card_number

        return new Patron(username, password.toStdString(), email.toStdString(), card);
    }
    else if (type == "Librarian")
    {
        return new Librarian(username, password.toStdString());
    }
    else if (type == "Admin")
    {
        return new Admin(username, password.toStdString());
    }

    qDebug() << "ERROR: Unknown user_type for" << QString::fromStdString(username) << ":" << type;
    return nullptr;
}

std::vector<User *> DBManager::getUsersContaining(std::string str)
{

    std::vector<User *> matchingUsers;
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username LIKE :pattern");
    query.bindValue(":pattern", "%" + QString::fromStdString(str) + "%");

    if (!query.exec())
    {
        qDebug() << "ERROR: getUsersContaining query.exec() failed for pattern"
                 << "%" + QString::fromStdString(str) + "%" << ":" << query.lastError().text();
        return matchingUsers;
    }

    while (query.next())
    {
        std::string username = query.value("username").toString().toStdString();
        std::string password = query.value("password").toString().toStdString();
        std::string type = query.value("user_type").toString().toStdString();

        User *user = nullptr;

        if (type == "Patron")
        {
            std::string email = query.value("email").toString().toStdString();
            int cardNum = query.value("card_number").toInt();

            user = new Patron(username, password, email, cardNum);
        }
        else if (type == "Librarian")
        {
            user = new Librarian(username, password);
        }
        else if (type == "Admin")
        {
            user = new Admin(username, password);
        }
        else
        {
            qDebug() << "Warning: Unknown user type found:" << QString::fromStdString(type);
            continue; // skip
        }

        if (user)
        {
            matchingUsers.push_back(user);
        }
    }
    return matchingUsers;
}

std::string determineItemType(CatalogueItem *item)
{
    if (dynamic_cast<Fiction *>(item))
    {
        return "Fiction";
    }
    else if (dynamic_cast<NonFiction *>(item))
    {
        return "NonFiction";
    }
    else if (dynamic_cast<Magazine *>(item))
    {
        return "Magazine";
    }
    else if (dynamic_cast<Movie *>(item))
    {
        return "Movie";
    }
    else if (dynamic_cast<VideoGame *>(item))
    {
        return "VideoGame";
    }
    return "Unknown";
}

bool DBManager::addItem(CatalogueItem *item)
{
    // figure out what type item is:
    auto type = determineItemType(item);

    if (type == "Unknown")
    {
        qDebug() << "ERROR: Could not get item type";
        return false;
    }

    if (!db.transaction())
    {
        qDebug() << "ERROR: Failed to start transaction:" << db.lastError().text();
        return false;
    }

    // add general catalogue item info
    QSqlQuery query;
    query.prepare("INSERT INTO items(isbn, title, author, publication_year, format, condition, circulation_status, item_type) "
                  "VALUES (:isbn, :title, :author, :publication_year, :format, :condition, :circulation_status, :item_type)");

    query.bindValue(":isbn", item->get_isbn());
    query.bindValue(":title", QString::fromStdString(item->get_title()));
    query.bindValue(":author", QString::fromStdString(item->get_author()));
    query.bindValue(":publication_year", item->get_publication_year());
    query.bindValue(":format", QString::fromStdString(item->get_format()));
    query.bindValue(":condition", QString::fromStdString(item->get_condition()));
    query.bindValue(":circulation_status", QString::fromStdString(item->get_circulationStatus()));

    // Determine the correct item_type for the 'items' table based on the C++ type
    if (type == "Fiction" || type == "NonFiction")
    {
        query.bindValue(":item_type", "Book");
    }
    else
    {
        query.bindValue(":item_type", QString::fromStdString(type));
    }

    if (!query.exec())
    {
        qDebug() << "Failed to insert into items table:" << query.lastError();
        db.rollback();
        return false;
    }

    bool success = true;

    if (type == "Fiction")
    {
        Fiction *fiction = dynamic_cast<Fiction *>(item);
        if (fiction)
        {
            query.prepare("INSERT INTO books(isbn, book_type) VALUES (:isbn, 'Fiction')");
            query.bindValue(":isbn", fiction->get_isbn());
            if (!query.exec())
            {
                qDebug() << "Failed to insert into books table:" << query.lastError();
                success = false;
            }
        }
    }
    else if (type == "NonFiction")
    {
        NonFiction *nonfiction = dynamic_cast<NonFiction *>(item);
        if (nonfiction)
        {
            query.prepare("INSERT INTO books(isbn, book_type) VALUES (:isbn, 'NonFiction')");
            query.bindValue(":isbn", nonfiction->get_isbn());
            if (!query.exec())
            {
                qDebug() << "Failed to insert into books table:" << query.lastError();
                success = false;
            }

            query.prepare("INSERT INTO nonfiction_books(isbn, dewey_decimal) VALUES (:isbn, :dewey_decimal)");
            query.bindValue(":isbn", nonfiction->get_isbn());
            query.bindValue(":dewey_decimal", nonfiction->get_dewey_decimal());
            if (!query.exec())
            {
                qDebug() << "Failed to insert into nonfiction_books table:" << query.lastError();
                success = false;
            }
        }
    }
    else if (type == "Magazine")
    {
        Magazine *magazine = dynamic_cast<Magazine *>(item);
        if (magazine)
        {
            query.prepare("INSERT INTO magazines(isbn, issue_number, publication_date) VALUES (:isbn, :issue_number, :publication_date)");
            query.bindValue(":isbn", magazine->get_isbn());
            query.bindValue(":issue_number", magazine->get_issue_number());
            query.bindValue(":publication_date", QString::fromStdString(magazine->get_publication_date()));
            if (!query.exec())
            {
                qDebug() << "Failed to insert into magazines table:" << query.lastError();
                success = false;
            }
        }
    }
    else if (type == "Movie")
    {
        Movie *movie = dynamic_cast<Movie *>(item);
        if (movie)
        {
            query.prepare("INSERT INTO movies(isbn, genre, rating) VALUES (:isbn, :genre, :rating)");
            query.bindValue(":isbn", movie->get_isbn());
            query.bindValue(":genre", QString::fromStdString(movie->get_genre()));
            query.bindValue(":rating", QString::fromStdString(movie->get_rating()));
            if (!query.exec())
            {
                qDebug() << "Failed to insert into movies table:" << query.lastError();
                success = false;
            }
        }
    }
    else if (type == "VideoGame")
    {
        VideoGame *videogame = dynamic_cast<VideoGame *>(item);
        if (videogame)
        {
            query.prepare("INSERT INTO videogames(isbn, genre, rating) VALUES (:isbn, :genre, :rating)");
            query.bindValue(":isbn", videogame->get_isbn());
            query.bindValue(":genre", QString::fromStdString(videogame->get_genre()));
            query.bindValue(":rating", QString::fromStdString(videogame->get_rating()));
            if (!query.exec())
            {
                qDebug() << "Failed to insert into videogames table:" << query.lastError();
                success = false;
            }
        }
    }

    if (!success)
    {
        db.rollback();
        return false;
    }

    if (!db.commit())
    {
        qDebug() << "ERROR: Failed to commit transaction:" << db.lastError().text();
        db.rollback();
        return false;
    }

    return true;
}

std::vector<CatalogueItem *> DBManager::getAllItems()
{
    QSqlQuery query;
    query.prepare("SELECT * FROM items");

    std::vector<CatalogueItem *> items;

    if (!query.exec())
    {
        qDebug() << "ERROR: Failed to get all items:" << query.lastError();
    }

    while (query.next())
    {
        QString type = query.value("item_type").toString();
        int isbn = query.value("isbn").toInt();
        std::string title = query.value("title").toString().toStdString();
        std::string author = query.value("author").toString().toStdString();
        int publication_year = query.value("publication_year").toInt();
        std::string format = query.value("format").toString().toStdString();
        std::string condition = query.value("condition").toString().toStdString();
        std::string circulation_status = query.value("circulation_status").toString().toStdString();

        if (type == "Book")
        {
            QSqlQuery book_query;
            book_query.prepare("SELECT book_type FROM books WHERE isbn = :isbn");
            book_query.bindValue(":isbn", isbn);
            if (!book_query.exec())
            {
                qDebug() << "Failed to get Book type" << book_query.lastError();
            }

            if (!book_query.next())
            {
                qDebug() << "Data Integrity Error: Item is marked as Book but not found in books table. ISBN:" << isbn;
                continue;
            }

            QString book_type = book_query.value(0).toString();
            if (book_type == "Fiction")
            {
                items.push_back(new Fiction(title,
                                            author,
                                            publication_year,
                                            isbn,
                                            format,
                                            condition,
                                            circulation_status));
            }
            else if (book_type == "NonFiction")
            {
                QSqlQuery nf_query;
                nf_query.prepare("SELECT dewey_decimal FROM nonfiction_books WHERE isbn = :isbn");
                nf_query.bindValue(":isbn", isbn);
                if (!nf_query.exec())
                {
                    qDebug() << "Failed to get nonfiction deweydecimal" << nf_query.lastError();
                }

                if (!nf_query.next())
                {
                    qDebug() << "Data Integrity Error: Item is NonFiction but not found in nonfiction_books table. ISBN:" << isbn;
                    continue;
                }

                int deweyDecimal = nf_query.value(0).toInt();
                items.push_back(new NonFiction(title,
                                               author,
                                               publication_year,
                                               isbn,
                                               format,
                                               condition,
                                               circulation_status,
                                               deweyDecimal));
            }
        }
        else if (type == "Magazine")
        {
            QSqlQuery mag_query;
            mag_query.prepare("SELECT * FROM magazines WHERE isbn = :isbn");
            mag_query.bindValue(":isbn", isbn);
            if (!mag_query.exec())
            {
                qDebug() << "Failed to get magazine info" << mag_query.lastError();
            }

            if (!mag_query.next())
            {
                qDebug() << "Data Integrity Error: Item is Magazine but not found in magazines table. ISBN:" << isbn;
                continue;
            }

            int issue_number = mag_query.value("issue_number").toInt();
            std::string publication_date = mag_query.value("publication_date").toString().toStdString();

            items.push_back(new Magazine(title,
                                         author,
                                         publication_year,
                                         isbn,
                                         format,
                                         condition,
                                         circulation_status,
                                         issue_number,
                                         publication_date));
        }
        else if (type == "Movie")
        {
            QSqlQuery movie_query;
            movie_query.prepare("SELECT * FROM movies WHERE isbn = :isbn");
            movie_query.bindValue(":isbn", isbn);
            if (!movie_query.exec())
            {
                qDebug() << "Failed to get movie info" << movie_query.lastError();
            }

            if (!movie_query.next())
            {
                qDebug() << "Data Integrity Error: Item is Movie but not found in movies table. ISBN:" << isbn;
                continue;
            }

            std::string genre = movie_query.value("genre").toString().toStdString();
            std::string rating = movie_query.value("rating").toString().toStdString();

            items.push_back(new Movie(title,
                                      author,
                                      publication_year,
                                      isbn,
                                      format,
                                      condition,
                                      circulation_status,
                                      genre,
                                      rating));
        }
        else if (type == "VideoGame")
        {
            QSqlQuery vg_query;
            vg_query.prepare("SELECT * FROM videogames WHERE isbn = :isbn");
            vg_query.bindValue(":isbn", isbn);
            if (!vg_query.exec())
            {
                qDebug() << "Failed to get videogame info" << vg_query.lastError();
            }

            if (!vg_query.next())
            {
                qDebug() << "Data Integrity Error: Item is VideoGame but not found in videogames table. ISBN:" << isbn;
                continue;
            }

            std::string genre = vg_query.value("genre").toString().toStdString();
            std::string rating = vg_query.value("rating").toString().toStdString();

            items.push_back(new VideoGame(title,
                                          author,
                                          publication_year,
                                          isbn,
                                          format,
                                          condition,
                                          circulation_status,
                                          genre,
                                          rating));
        }
    }
    return items;
}

CatalogueItem* DBManager::getItemByIsbn(int isbn)
{
    QSqlQuery query;
    query.prepare("SELECT i.*, b.book_type, n.dewey_decimal, "
                  "mag.issue_number, mag.publication_date, "
                  "mov.genre AS movie_genre, mov.rating AS movie_rating, "
                  "vg.genre AS game_genre, vg.rating AS game_rating "
                  "FROM items i "
                  "LEFT JOIN books b ON i.isbn = b.isbn "
                  "LEFT JOIN nonfiction_books n ON i.isbn = n.isbn "
                  "LEFT JOIN magazines mag ON i.isbn = mag.isbn "
                  "LEFT JOIN movies mov ON i.isbn = mov.isbn "
                  "LEFT JOIN videogames vg ON i.isbn = vg.isbn "
                  "WHERE i.isbn = :isbn");
    query.bindValue(":isbn", isbn);

    if (!query.exec()) {
        qDebug() << "ERROR: getItemByIsbn query failed:" << query.lastError();
        return nullptr;
    }

    if (!query.next()) {
        return nullptr;
    }

    // Base fields
    std::string title = query.value("title").toString().toStdString();
    std::string author = query.value("author").toString().toStdString();
    int year = query.value("publication_year").toInt();
    std::string format = query.value("format").toString().toStdString();
    std::string condition = query.value("condition").toString().toStdString();
    std::string status = query.value("circulation_status").toString().toStdString();
    QString itemType = query.value("item_type").toString();

    if (itemType == "Book") {
        QString bookType = query.value("book_type").toString();
        if (bookType == "NonFiction") {
            int dewey = query.value("dewey_decimal").toInt();
            return new NonFiction(title, author, year, isbn, format, condition, status, dewey);
        } else {
            return new Fiction(title, author, year, isbn, format, condition, status);
        }
    } else if (itemType == "Magazine") {
        int issue = query.value("issue_number").toInt();
        std::string pubDate = query.value("publication_date").toString().toStdString();
        return new Magazine(title, author, year, isbn, format, condition, status, issue, pubDate);
    } else if (itemType == "Movie") {
        std::string genre = query.value("movie_genre").toString().toStdString();
        std::string rating = query.value("movie_rating").toString().toStdString();
        return new Movie(title, author, year, isbn, format, condition, status, genre, rating);
    } else if (itemType == "VideoGame") {
        std::string genre = query.value("game_genre").toString().toStdString();
        std::string rating = query.value("game_rating").toString().toStdString();
        return new VideoGame(title, author, year, isbn, format, condition, status, genre, rating);
    }

    return nullptr;
}

bool DBManager::updateItemStatus(CatalogueItem *item, const std::string &newStatus)
{
    // get isbn
    auto isbn = item->get_isbn();

    QSqlQuery query;
    query.prepare("UPDATE items SET circulation_status = :status WHERE isbn = :isbn");

    query.bindValue(":status", QString::fromStdString(newStatus));
    query.bindValue(":isbn", isbn);

    if (!query.exec())
    {
        qDebug() << "Failed to update status for ISBN " << isbn << ": " << query.lastError();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qDebug() << "Error: No item found with ISBN " << isbn;
        return false;
    }

    qDebug() << "Successfully updated ISBN" << isbn << "to status:" << QString::fromStdString(newStatus);
    return true;
}

Loan *DBManager::getLoan(CatalogueItem *item)
{
    // Get isbn of item
    auto isbn = item->get_isbn();

    QSqlQuery query;

    query.prepare("SELECT * FROM loans WHERE isbn = :isbn AND return_date IS NULL");
    query.bindValue(":isbn", isbn);

    if (!query.exec())
    {
        qDebug() << "Failed to get loan info" << query.lastError();
        return nullptr; // Better to return nullptr than an empty object on error
    }

    if (!query.next())
    {
        qDebug() << "There is no active loan record for isbn: " << isbn;
        return new Loan();
    }

    // Extract data
    auto due_date = query.value("due_date").toString().toStdString();
    auto borrower = query.value("username").toString().toStdString(); // Important for tracking

    auto loan = new Loan();
    loan->setLoanStatus(LoanStatus::Loaned);
    loan->setDueDate(due_date);

    return loan;
}

std::vector<CatalogueItem *> DBManager::getLoanedItems(Patron *patron)
{
    std::vector<CatalogueItem *> loanedItems;

    if (!patron)
    {
        qDebug() << "Error: Null patron passed to getLoanedItems";
        return loanedItems;
    }

    QSqlQuery query;
    query.prepare("SELECT i.*, "
                  "b.book_type, n.dewey_decimal, "
                  "mag.issue_number, mag.publication_date, "
                  "mov.genre AS movie_genre, mov.rating AS movie_rating, "
                  "vg.genre AS game_genre, vg.rating AS game_rating "
                  "FROM loans l "
                  "JOIN items i ON l.isbn = i.isbn "
                  "LEFT JOIN books b ON i.isbn = b.isbn "
                  "LEFT JOIN nonfiction_books n ON i.isbn = n.isbn "
                  "LEFT JOIN magazines mag ON i.isbn = mag.isbn "
                  "LEFT JOIN movies mov ON i.isbn = mov.isbn "
                  "LEFT JOIN videogames vg ON i.isbn = vg.isbn "
                  "WHERE l.username = :user AND l.return_date IS NULL");

    query.bindValue(":user", QString::fromStdString(patron->get_username()));

    if (!query.exec())
    {
        qDebug() << "Failed to fetch loaned items for user:" << query.lastError();
        return loanedItems;
    }

    while (query.next())
    {
        // CatalogueItem general info
        int isbn = query.value("isbn").toInt();
        std::string title = query.value("title").toString().toStdString();
        std::string author = query.value("author").toString().toStdString();
        int year = query.value("publication_year").toInt();
        std::string format = query.value("format").toString().toStdString();
        std::string condition = query.value("condition").toString().toStdString();
        std::string status = query.value("circulation_status").toString().toStdString();

        QString itemType = query.value("item_type").toString();
        CatalogueItem *newItem = nullptr;

        // Factory logic
        if (itemType == "Book")
        {
            QString bookType = query.value("book_type").toString();
            if (bookType == "NonFiction")
            {
                int dewey = query.value("dewey_decimal").toInt();
                newItem = new NonFiction(title, author, year, isbn, format, condition, status, dewey);
            }
            else
            {
                newItem = new Fiction(title, author, year, isbn, format, condition, status);
            }
        }
        else if (itemType == "Magazine")
        {
            int issue = query.value("issue_number").toInt();
            std::string pubDate = query.value("publication_date").toString().toStdString();
            newItem = new Magazine(title, author, year, isbn, format, condition, status, issue, pubDate);
        }
        else if (itemType == "Movie")
        {
            std::string genre = query.value("movie_genre").toString().toStdString();
            std::string rating = query.value("movie_rating").toString().toStdString();
            newItem = new Movie(title, author, year, isbn, format, condition, status, genre, rating);
        }
        else if (itemType == "VideoGame")
        {
            std::string genre = query.value("game_genre").toString().toStdString();
            std::string rating = query.value("game_rating").toString().toStdString();
            newItem = new VideoGame(title, author, year, isbn, format, condition, status, genre, rating);
        }
        else
        {
            qDebug() << "Warning: Unknown item type " << itemType << " for ISBN " << isbn;
            continue;
        }

        if (newItem)
        {
            loanedItems.push_back(newItem);
        }
    }

    return loanedItems;
}



std::string DBManager::getItemStatus(CatalogueItem* item)
{
    if (!item) return "Unknown";

    QSqlQuery query;
    query.prepare("SELECT circulation_status FROM items WHERE isbn = :isbn");
    query.bindValue(":isbn", item->get_isbn());

    if (!query.exec() || !query.next())
        return "Unknown";

    return query.value(0).toString().toStdString();
}


std::string DBManager::getLoanBorrower(CatalogueItem* item)
{
    if (!item) return "";

    QSqlQuery query;
    query.prepare("SELECT username FROM loans WHERE isbn = :isbn AND return_date IS NULL");
    query.bindValue(":isbn", item->get_isbn());

    if (!query.exec() || !query.next())
        return "";

    return query.value(0).toString().toStdString();
}
