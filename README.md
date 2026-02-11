# 📚 Library Management System

## 📋 Overview

This project is a fully functional Library Management System developed in C for Unix-based environments. It is a multi-file, modular system designed to manage books, members, and circulation records using persistent binary storage.
The system handles complete library operations including adding and editing books, managing members, issuing and returning books, automatic overdue detection, and detailed reporting of circulation history. All records are stored in binary database files and remain available even after the program exits.
The project is structured for scalability, maintainability, and real-world logic implementation.

---

## ✨ Features

### 🔐 Authentication System
- Secure login system using username and hashed password.
- Password stored as SHA-512 crypt hash (not plain text).
- Login validation before accessing system operations.

### 📘 Book Management
- Add new books with auto-generated unique Book ID.
- Edit existing book details including title, author, publisher, subject, language, edition, and quantity.
- Delete book records with safety checks (cannot delete if issued).
- Automatic shelf number generation.
- Search books using case-insensitive and partial matching.
- View books with filtering by subject, author, publisher, language, year, and edition.
- Pagination support for large datasets.

### 👤 Member Management
- Add new members with auto-generated unique Member ID.
- Edit member details and manage status (active / blocked).
- Search members by ID, name, or phone number (partial and case-insensitive).
- View members by type, borrow count, status, or borrowing activity.
- Pagination for all result listings.

### 🔄 Circulation Management
- Issue books to members with borrow limit enforcement.
- Return issued books with automatic record updates.
- Renew issued books with due date extension.
- Automatic due date calculation (30 days from issue date).
- Automatic blocking of members if any issued book exceeds due date.
- Automatic unblocking once all overdue books are returned or renewed.

### 📊 Issued Book Reporting
Option to view issued book records with member details:
1. Issued and returned records (includes kept days calculation).
2. Issued but not returned (due date not exceeded).
3. Issued but not returned (due date exceeded).

Reports include:
- Book ID and title
- Member ID and name
- Issue date
- Due date
- Return date (if applicable)
- Calculated duration (kept days or exceeded days)
Records are displayed from latest to oldest and support pagination.

### 💾 Persistent Storage
All data is stored in binary database files:
- Books database
- Members database
- Issues database
- ID counter database
Records remain intact between executions.

### 📝 Logging System
The system maintains operation logs for:
- Book operations
- Member operations
- Circulation activities
Each log entry records operation type and timestamp.

---

## ⚙️ Compilation

Requirements:
`GCC compiler`
`make`
`Unix-based environment`

Build:
```sh
make
```

Run:
```sh
./library
```

Clean Build Files:
```sh
make clean
```

---

## 🔑 Default Login
```text
Username: Admin@123
Password: Pass#code123
```
Password stored securely as a hashed value inside the source code. Password can be change later.

---

## 📸 Images (Screenshots)

![Demo Screenshot 1](https://github.com/code5x/library_management_system/blob/main/images/screenshot_001.jpg)
![Demo Screenshot 2](https://github.com/code5x/library_management_system/blob/main/images/screenshot_002.jpg)
![Demo Screenshot 3](https://github.com/code5x/library_management_system/blob/main/images/screenshot_003.jpg)

---

## 📌 Important Notes

- If the Book structure is modified (for example adding/removing new fields), existing database files may become incompatible and should be recreated.
- ID values are auto-incremented and never reused, even if records are deleted.
- Member status is automatically derived from issue records and overdue conditions.
- Pagination is implemented for large datasets to maintain clean terminal output.
- The system reads issue records in reverse order to show the most recent activity first.

---
