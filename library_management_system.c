
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BOOKS 100
#define MAX_USERS 100
#define MAX_BORROWED 10

typedef struct {
    int bserial_number;
    int isbn_number;
    char bname[50];
    int edition;
    char author[50];
    char is_reserved[6];
} Book;

typedef struct {
    char name[50];
    char rollNumber[20];
    char email[50];
    char password[20];
    char haveBooks[MAX_BORROWED][50];
} Student;

Book library[MAX_BOOKS];
int book_count = 0;

void clearInputBuffer();
void loadBooks();
void saveBooks();

void adminMenu();
void userMenu(Student* user);

void addBook();
void removeBook();
void modifyBook();
void displayBooks();
void cancelMembership();

void borrowBook(Student* user);
void returnBook(Student* user);
void searchBook();
void signupUser();
int loginUser(Student* user);
void saveUserData(Student* user);

int main() {
    loadBooks();
    int choice;
    while (1) {
        printf("\n==== Library Management System ===="
               "\n1. Admin Login\n2. User Login\n3. Signup\n4. Exit\nChoice: ");
        scanf("%d", &choice);
        clearInputBuffer();

        if (choice == 1) {
            char email[50], pass[20];
            printf("Enter Admin Email: "); fgets(email, 50, stdin);
            email[strcspn(email, "\n")] = '\0';
            printf("Enter Password: "); fgets(pass, 20, stdin);
            pass[strcspn(pass, "\n")] = '\0';

            if ((strcmp(email, "admin1@gmail.com") == 0 || strcmp(email, "admin2@gmail.com") == 0) &&
                (strcmp(pass, "password1") == 0 || strcmp(pass, "password2") == 0)) {
                printf("\nAdmin login successful.\n");
                adminMenu();
            } else {
                printf("Invalid credentials.\n");
            }
        } else if (choice == 2) {
            Student user;
            if (loginUser(&user)) {
                printf("\nUser login successful.\n");
                userMenu(&user);
            }
        } else if (choice == 3) {
            signupUser();
        } else if (choice == 4) {
            printf("Exiting...\n");
            break;
        } else {
            printf("Invalid option.\n");
        }
    }
    return 0;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void loadBooks() {
    FILE *fp = fopen("LibraryBooks.txt", "r");
    if (fp == NULL) return;
    while (fscanf(fp, "%d\n%d\n%[^\n]\n%d\n%[^\n]\n%[^\n]\n",
                  &library[book_count].bserial_number,
                  &library[book_count].isbn_number,
                  library[book_count].bname,
                  &library[book_count].edition,
                  library[book_count].author,
                  library[book_count].is_reserved) == 6) {
        book_count++;
    }
    fclose(fp);
}

void saveBooks() {
    FILE *fp = fopen("LibraryBooks.txt", "w");
    for (int i = 0; i < book_count; i++) {
        fprintf(fp, "%d\n%d\n%s\n%d\n%s\n%s\n",
                library[i].bserial_number,
                library[i].isbn_number,
                library[i].bname,
                library[i].edition,
                library[i].author,
                library[i].is_reserved);
    }
    fclose(fp);
}

void adminMenu() {
    int choice;
    do {
        printf("\n-- Admin Menu --\n1. Add Book\n2. Remove Book\n3. Modify Book\n4. Display Books\n5. Cancel Membership\n6. Back to Main\nChoice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        switch (choice) {
            case 1: addBook(); break;
            case 2: removeBook(); break;
            case 3: modifyBook(); break;
            case 4: displayBooks(); break;
            case 5: cancelMembership(); break;
            case 6: return;
            default: printf("Invalid choice.\n");
        }
    } while (1);
}

void addBook() {
    if (book_count >= MAX_BOOKS) {
        printf("Library full.\n"); return;
    }
    Book newBook;
    printf("Enter Serial Number: "); scanf("%d", &newBook.bserial_number);
    printf("Enter ISBN: "); scanf("%d", &newBook.isbn_number);
    clearInputBuffer();
    for (int i = 0; i < book_count; i++) {
        if (library[i].bserial_number == newBook.bserial_number) {
            printf("Book with this serial number already exists.\n");
            return;
        }
    }
    printf("Enter Name: "); fgets(newBook.bname, 50, stdin); newBook.bname[strcspn(newBook.bname, "\n")] = '\0';
    printf("Enter Edition: "); scanf("%d", &newBook.edition);
    clearInputBuffer();
    printf("Enter Author: "); fgets(newBook.author, 50, stdin); newBook.author[strcspn(newBook.author, "\n")] = '\0';
    strcpy(newBook.is_reserved, "no");

    library[book_count++] = newBook;
    saveBooks();
    printf("Book added.\n");
}

void removeBook() {
    char name[50];
    printf("Enter book name to remove: ");
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = '\0';
    for (int i = 0; i < book_count; i++) {
        if (strcmp(library[i].bname, name) == 0) {
            for (int j = i; j < book_count - 1; j++) {
                library[j] = library[j + 1];
            }
            book_count--;
            saveBooks();
            printf("Book removed.\n");
            return;
        }
    }
    printf("Book not found.\n");
}

void modifyBook() {
    char name[50], newName[50];
    printf("Enter book name to modify: ");
    fgets(name, 50, stdin); name[strcspn(name, "\n")] = '\0';
    for (int i = 0; i < book_count; i++) {
        if (strcmp(library[i].bname, name) == 0) {
            printf("Enter new name: ");
            fgets(newName, 50, stdin); newName[strcspn(newName, "\n")] = '\0';
            strcpy(library[i].bname, newName);
            saveBooks();
            printf("Book modified.\n");
            return;
        }
    }
    printf("Book not found.\n");
}

void displayBooks() {
    for (int i = 0; i < book_count; i++) {
        printf("\n[%d] %s | ISBN: %d | Author: %s | Edition: %d | Reserved: %s\n",
               i + 1, library[i].bname, library[i].isbn_number, library[i].author,
               library[i].edition, library[i].is_reserved);
    }
}

void borrowBook(Student* user) {
    displayBooks();
    int index;
    printf("Enter book index to borrow: ");
    scanf("%d", &index);
    clearInputBuffer();
    index--;
    if (index >= 0 && index < book_count && strcmp(library[index].is_reserved, "no") == 0) {
        for (int i = 0; i < MAX_BORROWED; i++) {
            if (user->haveBooks[i][0] == '\0') {
                strcpy(user->haveBooks[i], library[index].bname);
                strcpy(library[index].is_reserved, "yes");
                saveBooks();
                saveUserData(user);
                printf("Book borrowed.\n");
                return;
            }
        }
        printf("Borrow limit reached.\n");
    } else {
        printf("Invalid index or book already reserved.\n");
    }
}

void returnBook(Student* user) {
    for (int i = 0; i < MAX_BORROWED; i++) {
        if (user->haveBooks[i][0] != '\0') {
            printf("[%d] %s\n", i + 1, user->haveBooks[i]);
        }
    }
    int index;
    printf("Enter index of book to return: ");
    scanf("%d", &index);
    clearInputBuffer();
    index--;
    if (index >= 0 && index < MAX_BORROWED && user->haveBooks[index][0] != '\0') {
        for (int i = 0; i < book_count; i++) {
            if (strcmp(library[i].bname, user->haveBooks[index]) == 0) {
                strcpy(library[i].is_reserved, "no");
                break;
            }
        }
        user->haveBooks[index][0] = '\0';
        saveBooks();
        saveUserData(user);
        printf("Book returned.\n");
    } else {
        printf("Invalid index.\n");
    }
}

void searchBook() {
    char name[50];
    printf("Enter book name to search: ");
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = '\0';
    for (int i = 0; i < book_count; i++) {
        if (strstr(library[i].bname, name)) {
            printf("\n[%d] %s | ISBN: %d | Author: %s | Edition: %d | Reserved: %s\n",
                   i + 1, library[i].bname, library[i].isbn_number, library[i].author,
                   library[i].edition, library[i].is_reserved);
        }
    }
}

void signupUser() {
    Student user;
    clearInputBuffer();
    printf("Enter name: "); fgets(user.name, 50, stdin); user.name[strcspn(user.name, "\n")] = '\0';
    printf("Enter roll number: "); fgets(user.rollNumber, 20, stdin); user.rollNumber[strcspn(user.rollNumber, "\n")] = '\0';
    printf("Enter email: "); fgets(user.email, 50, stdin); user.email[strcspn(user.email, "\n")] = '\0';
    printf("Enter password: "); fgets(user.password, 20, stdin); user.password[strcspn(user.password, "\n")] = '\0';

    char filename[100];
    snprintf(filename, sizeof(filename), "%s.txt", user.email);
    FILE *check = fopen(filename, "r");
    if (check) {
        fclose(check);
        printf("User already exists.\n");
        return;
    }

    for (int i = 0; i < MAX_BORROWED; i++) user.haveBooks[i][0] = '\0';
    saveUserData(&user);
    printf("User registered.\n");
}

int loginUser(Student* user) {
    char email[50], pass[20];
    clearInputBuffer();
    printf("Enter email: "); fgets(email, 50, stdin); email[strcspn(email, "\n")] = '\0';
    printf("Enter password: "); fgets(pass, 20, stdin); pass[strcspn(pass, "\n")] = '\0';
    char filename[100];
    snprintf(filename, sizeof(filename), "%s.txt", email);
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("User not found.\n");
        return 0;
    }
    fscanf(fp, "%[^\n]\n%[^\n]\n%[^\n]\n%[^\n]\n", user->name, user->rollNumber, user->email, user->password);
    for (int i = 0; i < MAX_BORROWED; i++) user->haveBooks[i][0] = '\0';
    for (int i = 0; i < MAX_BORROWED && fgets(user->haveBooks[i], 50, fp); i++) {
        user->haveBooks[i][strcspn(user->haveBooks[i], "\n")] = '\0';
    }
    fclose(fp);
    return (strcmp(user->password, pass) == 0);
}

void saveUserData(Student* user) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s.txt", user->email);
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "%s\n%s\n%s\n%s\n", user->name, user->rollNumber, user->email, user->password);
    for (int i = 0; i < MAX_BORROWED; i++) {
        if (user->haveBooks[i][0] != '\0') fprintf(fp, "%s\n", user->haveBooks[i]);
    }
    fclose(fp);
}

void cancelMembership() {
    char email[50];
    printf("Enter email to delete account: ");
    fgets(email, 50, stdin);
    email[strcspn(email, "\n")] = '\0';
    char filename[100];
    snprintf(filename, sizeof(filename), "%s.txt", email);
    if (remove(filename) == 0) {
        printf("Membership canceled.\n");
    } else {
        printf("Error deleting account.\n");
    }
}

void userMenu(Student* user) {
    int choice;
    do {
        printf("\n-- User Menu --\n1. Borrow Book\n2. Return Book\n3. Search Book\n4. Display Books\n5. Logout\nChoice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        switch (choice) {
            case 1: borrowBook(user); break;
            case 2: returnBook(user); break;
            case 3: searchBook(); break;
            case 4: displayBooks(); break;
            case 5: return;
            default: printf("Invalid choice.\n");
        }
    } while (1);
}
