# HinLIBS - Library Management System 📚

**HinLIBS** is a comprehensive Library Management System designed to streamline library operations for both patrons and librarians. Built with **C++** and **Qt**, this application provides a robust and user-friendly interface for managing catalogues, user accounts, and circulation.

## 🚀 Key Features

*   **Role-Based Access Control**: Distinct interfaces and permissions for **Patrons** and **Librarians**.
*   **Catalogue Management**: Searchable database of books, movies, magazines, and more.
*   **User Accounts**: Patron account management with borrowing history and status tracking.
*   **Database Integration**: Powered by **SQLite** for reliable local data storage.
*   **Modern UI**: Clean, page-based navigation system using Qt Widgets.

## 🛠️ Technology Stack

*   **Language**: C++
*   **Framework**: Qt (Widgets)
*   **Database**: SQLite
*   **Build System**: QMake

## 📖 Application Overview

The application utilizes a central `MainWindow` to navigate between different functional pages:

### 1. 🔐 User Authentication (`loginpage.cpp`)
Secure entry point. Users log in to access features tailored to their role.

### 2. 🏠 Welcome Dashboard (`welcomepage.cpp`)
A personalized landing page that greets the user and provides quick access to main functions.

### 3. 🧭 Navigation System
A persistent navigation bar allowing easy travel between:
*   **Browse Catalogue**: View and search the library's collection.
*   **My Account**: Check loans and account status.
*   **Librarian Tools**: (Admin only) Add items, manage patrons, and oversee circulation.

### 4. 📚 Catalogue & Item View (`browseCatalogue.cpp`, `viewItem.cpp`)
*   **Patrons**: View available items.
*   **Librarians**: View full inventory, including items removed from circulation.
*   Detailed item views show Title, Author, Format, and Availability.

---

## 🔧 Setup & Installation

1.  **Requirements**:
    *   C++ Compiler (GCC/Clang/MSVC)
    *   Qt 5 or Qt 6 SDK
    *   Qt Creator IDE

2.  **Build Instructions**:
    *   Open `HINLIBS-3004.pro` in Qt Creator.
    *   Configure the project using your preferred Kit.
    *   Run **Build** (Ctrl+B).
    *   The `hinlibs.db` database will be automatically copied to the build directory.

3.  **Running the App**:
    *   Press **Run** (Ctrl+R) in Qt Creator.

