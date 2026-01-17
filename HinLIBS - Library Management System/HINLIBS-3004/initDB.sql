-- Drop all tables in reverse order of dependencies
DROP TABLE IF EXISTS holds;
DROP TABLE IF EXISTS loans;
DROP TABLE IF EXISTS videogames;
DROP TABLE IF EXISTS movies;
DROP TABLE IF EXISTS magazines;
DROP TABLE IF EXISTS nonfiction_books;
DROP TABLE IF EXISTS books;
DROP TABLE IF EXISTS items;
DROP TABLE IF EXISTS users;


-- Create Users table
CREATE TABLE IF NOT EXISTS users (
    username VARCHAR(50) PRIMARY KEY,
    password VARCHAR(50),
    user_type VARCHAR(20),
    email VARCHAR(100) UNIQUE,
    card_number INTEGER
);

-- Create Items table (base table for all catalogue items)
CREATE TABLE IF NOT EXISTS items (
    isbn INTEGER PRIMARY KEY,
    title VARCHAR(200),
    author VARCHAR(100),
    publication_year INTEGER,
    format VARCHAR(50),
    condition VARCHAR(50),
    circulation_status VARCHAR(50),
    item_type VARCHAR(50)
);

-- Create Books table
CREATE TABLE IF NOT EXISTS books (
    isbn INTEGER PRIMARY KEY,
    book_type VARCHAR(50),
    FOREIGN KEY(isbn) REFERENCES items(isbn)
);

-- Create NonFiction books table
CREATE TABLE IF NOT EXISTS nonfiction_books (
    isbn INTEGER PRIMARY KEY,
    dewey_decimal INTEGER,
    FOREIGN KEY(isbn) REFERENCES books(isbn)
);

-- Create Magazines table
CREATE TABLE IF NOT EXISTS magazines (
    isbn INTEGER PRIMARY KEY,
    issue_number INTEGER,
    publication_date VARCHAR(50),
    FOREIGN KEY(isbn) REFERENCES items(isbn)
);

-- Create Movies table
CREATE TABLE IF NOT EXISTS movies (
    isbn INTEGER PRIMARY KEY,
    genre VARCHAR(50),
    rating VARCHAR(10),
    FOREIGN KEY(isbn) REFERENCES items(isbn)
);

-- Create VideoGames table
CREATE TABLE IF NOT EXISTS videogames (
    isbn INTEGER PRIMARY KEY,
    genre VARCHAR(50),
    rating VARCHAR(10),
    FOREIGN KEY(isbn) REFERENCES items(isbn)
);

-- Create Loans table
CREATE TABLE IF NOT EXISTS loans (
    loan_id INTEGER PRIMARY KEY AUTOINCREMENT,
    isbn INTEGER,
    username VARCHAR(50),
    loan_date DATE,
    due_date DATE,
    return_date DATE,
    FOREIGN KEY(isbn) REFERENCES items(isbn),
    FOREIGN KEY(username) REFERENCES users(username)
);

-- Create Holds table
CREATE TABLE IF NOT EXISTS holds (
    hold_id INTEGER PRIMARY KEY AUTOINCREMENT,
    isbn INTEGER,
    username VARCHAR(50),
    hold_date DATE,
    position INTEGER,
    FOREIGN KEY(isbn) REFERENCES items(isbn),
    FOREIGN KEY(username) REFERENCES users(username)
);

--- insert initial data
-- Insert Users
INSERT INTO users (username, password, user_type, email, card_number) VALUES ('admin1', 'oozmakappa', 'Admin', NULL, NULL);
INSERT INTO users (username, password, user_type, email, card_number) VALUES ('librarian1', 'oozmakappa', 'Librarian', NULL, NULL);
INSERT INTO users (username, password, user_type, email, card_number) VALUES 
('Mike', '0001', 'Patron', 'mike@email.com', 1),
('Sulley', '0002', 'Patron', 'sulley@email.com', 2),
('DonCarleton', '0003', 'Patron', 'doncarleton@email.com', 3),
('TerryTerri', '0004', 'Patron', 'TerryTerri@email.com', 4),
('ScottSquishy', '0005', 'Patron', 'ScottSquishy@email.com', 5);

-- Insert Fiction Books
INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (123456789, 'The Great Gatsby', 'F. Scott Fitzgerald', 1925, 'Hardcover', 'Good', 'Available', 'Book');
INSERT INTO books (isbn, book_type) VALUES (123456789, 'Fiction');

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (987654321, '1984', 'George Orwell', 1949, 'Paperback', 'Fair', 'Available', 'Book');
INSERT INTO books (isbn, book_type) VALUES (987654321, 'Fiction');

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (112233445, 'To Kill a Mockingbird', 'Harper Lee', 1960, 'Hardcover', 'New', 'Available', 'Book');
INSERT INTO books (isbn, book_type) VALUES (112233445, 'Fiction');

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (223344556, 'The Catcher in the Rye', 'J.D. Salinger', 1951, 'Paperback', 'Good', 'Available', 'Book');
INSERT INTO books (isbn, book_type) VALUES (223344556, 'Fiction');

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (667788990, 'Moby-Dick', 'Herman Melville', 1851, 'Paperback', 'Poor', 'Available', 'Book');
INSERT INTO books (isbn, book_type) VALUES (667788990, 'Fiction');

-- Insert NonFiction Books
INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (123987456, 'Sapiens: A Brief History of Humankind', 'Yuval Noah Harari', 2011, 'Hardcover', 'Excellent', 'Available', 'Book');
INSERT INTO books (isbn, book_type) VALUES (123987456, 'NonFiction');
INSERT INTO nonfiction_books (isbn, dewey_decimal) VALUES (123987456, 909);

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (456789123, 'Educated', 'Tara Westover', 2018, 'Paperback', 'Good', 'Available', 'Book');
INSERT INTO books (isbn, book_type) VALUES (456789123, 'NonFiction');
INSERT INTO nonfiction_books (isbn, dewey_decimal) VALUES (456789123, 305);

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (789456123, 'The Immortal Life of Henrietta Lacks', 'Rebecca Skloot', 2010, 'Paperback', 'Fair', 'Available', 'Book');
INSERT INTO books (isbn, book_type) VALUES (789456123, 'NonFiction');
INSERT INTO nonfiction_books (isbn, dewey_decimal) VALUES (789456123, 616);

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (321654987, 'A Brief History of Time', 'Stephen Hawking', 1988, 'Hardcover', 'Good', 'Available', 'Book');
INSERT INTO books (isbn, book_type) VALUES (321654987, 'NonFiction');
INSERT INTO nonfiction_books (isbn, dewey_decimal) VALUES (321654987, 523);

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (135792468, 'The Art of War', 'Sun Tzu', -500, 'Paperback', 'Excellent', 'Available', 'Book');
INSERT INTO books (isbn, book_type) VALUES (135792468, 'NonFiction');
INSERT INTO nonfiction_books (isbn, dewey_decimal) VALUES (135792468, 355);

-- Insert Magazines
INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (112233446, 'National Geographic', 'National Geographic Society', 2023, 'Magazine', 'New', 'Available', 'Magazine');
INSERT INTO magazines (isbn, issue_number, publication_date) VALUES (112233446, 2023, 'January 2023');

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (223344557, 'TIME', 'TIME Magazine', 2023, 'Magazine', 'Good', 'Available', 'Magazine');
INSERT INTO magazines (isbn, issue_number, publication_date) VALUES (223344557, 1123, 'February 2023');

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (334455667, 'The Economist', 'The Economist Group', 2023, 'Magazine', 'Fair', 'Available', 'Magazine');
INSERT INTO magazines (isbn, issue_number, publication_date) VALUES (334455667, 3421, 'March 2023');

-- Insert Movies
INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (456789124, 'Inception', 'Christopher Nolan', 2010, 'Blu-ray', 'Excellent', 'Available', 'Movie');
INSERT INTO movies (isbn, genre, rating) VALUES (456789124, 'Sci-Fi', 'PG-13');

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (112233447, 'The Godfather', 'Francis Ford Coppola', 1972, 'DVD', 'Good', 'Available', 'Movie');
INSERT INTO movies (isbn, genre, rating) VALUES (112233447, 'Crime', 'R');

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (667788991, 'The Dark Knight', 'Christopher Nolan', 2008, 'Blu-ray', 'Good', 'Available', 'Movie');
INSERT INTO movies (isbn, genre, rating) VALUES (667788991, 'Action', 'PG-13');

-- Insert VideoGames
INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (123456790, 'The Last of Us', 'Naughty Dog', 2013, 'PlayStation 3', 'New', 'Available', 'VideoGame');
INSERT INTO videogames (isbn, genre, rating) VALUES (123456790, 'Action-Adventure', 'M');

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (987654322, 'The Witcher 3: Wild Hunt', 'CD Projekt', 2015, 'PC', 'Good', 'Available', 'VideoGame');
INSERT INTO videogames (isbn, genre, rating) VALUES (987654322, 'RPG', 'M');

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (456789125, 'Minecraft', 'Mojang', 2011, 'PC', 'Excellent', 'Available', 'VideoGame');
INSERT INTO videogames (isbn, genre, rating) VALUES (456789125, 'Sandbox', 'E10+');

INSERT INTO items (isbn, title, author, publication_year, format, condition, circulation_status, item_type) VALUES (112233448, 'Super Mario Odyssey', 'Nintendo', 2017, 'Nintendo Switch', 'Good', 'Available', 'VideoGame');
INSERT INTO videogames (isbn, genre, rating) VALUES (112233448, 'Platformer', 'E');
