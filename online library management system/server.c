#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
//imt2022530
#define MAX_BOOK_TITLE_LENGTH 50

void handle_client(int nsd);
void process_admin(int nsd);
void process_member(int nsd);

int main() {
    struct sockaddr_in serv, cli;
    int sd, nsd;
    int len = sizeof(cli);

    // Create socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Server address setup
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(5000);

    // Bind socket to address
    if (bind(sd, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
        perror("Bind failed");
        close(sd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sd, 5) == -1) {
        perror("Listen failed");
        close(sd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        nsd = accept(sd, (struct sockaddr *)&cli, &len);
        if (nsd < 0) {
            perror("Accept failed");
            continue;
        }

        if (fork() == 0) { // Child process
            close(sd); // Close the listening socket in the child process
            handle_client(nsd);
            close(nsd);
            exit(0);
        }

        close(nsd); // Close the socket in the parent process
    }

    close(sd);
    return 0;
}

void handle_client(int nsd) {
    char buf[100];
    int ret;

    // Receive user type from client
    ret = read(nsd, buf, sizeof(buf) - 1);
    if (ret == -1) {
        perror("Read error");
        return;
    }
    buf[ret] = '\0';
    char user_type[20];
    strcpy(user_type, buf);

    // Receive username from client
    ret = read(nsd, buf, sizeof(buf) - 1);
    if (ret == -1) {
        perror("Read error");
        return;
    }
    buf[ret] = '\0';
    printf("Type received from client: %s\n",buf);
    char username[20];
    strcpy(username, buf);

    // Receive password from client
    ret = read(nsd, buf, sizeof(buf) - 1);
    if (ret == -1) {
        perror("Read error");
        return;
    }
    buf[ret] = '\0';
    char password[20];
    strcpy(password, buf);

    // Open the appropriate file based on user type
    FILE *file = NULL;
    if (strcmp(user_type, "admin") == 0) {
        file = fopen("admins.txt", "r+");
    } else if (strcmp(user_type, "member") == 0) {
        file = fopen("members.txt", "r+");
    } else {
        perror("Invalid user type!\n");
        return;
    }

    if (file == NULL) {
        perror("Error opening file!\n");
        return;
    }

    int found = 0;
    char stored_username[20], stored_password[20];
    while (fscanf(file, "%s %s", stored_username, stored_password) != EOF) {
        if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0) {
            found = 1;
            char success_message[100];
            sprintf(success_message, "%s authentication successful!\n", user_type);
            //printf("admin has opened");
            write(nsd, success_message, strlen(success_message));
            break;
        }
    }

    if (!found) {
        fprintf(file, "%s %s\n", username, password);
        char success_message[100];
        sprintf(success_message, "New %s added successfully!\n", user_type);
        printf("new admin has been added");
        write(nsd, success_message, strlen(success_message));
    }

    fclose(file);

    if (strcmp(user_type, "admin") == 0) {
        process_admin(nsd);
    } else {
        process_member(nsd);
    }
}

void process_admin(int nsd) {
    char buf[100], choice[10];
    int ret;
    FILE *file = NULL;

    do {
        // Receive the choice from the client
        ret = read(nsd, buf, sizeof(buf) - 1);
        if (ret == -1) {
            perror("Read error");
            return;
        }
        buf[ret] = '\0';
        strcpy(choice, buf);

        if (strcmp(choice, "1") == 0) {
            file = fopen("members.txt", "a");
            if (file == NULL) {
                perror("Error opening members file!\n");
                return;
            }

            // Receive username to be added
            ret = read(nsd, buf, sizeof(buf) - 1);
            if (ret == -1) {
                perror("Read error");
                fclose(file);
                return;
            }
            buf[ret] = '\0';
            printf("Username added: %s\n", buf);
            fprintf(file, "%s\n", buf);
            fclose(file);

            // Success message to the client
            write(nsd, "Member added successfully!\n", strlen("Member added successfully!\n"));
            printf("member is added");
        } else if (strcmp(choice, "2") == 0) {
            // Delete member
            FILE *members = fopen("members.txt", "r");
            if (members == NULL) {
                perror("Error opening members file!\n");
                return;
            }

            // Receive username to be deleted
            ret = read(nsd, buf, sizeof(buf) - 1);
            if (ret == -1) {
                perror("Read error");
                fclose(members);
                return;
            }
            buf[ret] = '\0';
            char delete_username[20];
            strcpy(delete_username, buf);

            FILE *temp = fopen("temp.txt", "w");
            if (temp == NULL) {
                perror("Error creating temporary file!\n");
                fclose(members);
                return;
            }

            int member_found = 0;
            char temp_member[20];
            while (fgets(temp_member, sizeof(temp_member), members)) {
                temp_member[strcspn(temp_member, "\n")] = '\0'; // Remove newline character
                if (strcmp(temp_member, delete_username) != 0) {
                    fputs(temp_member, temp);
                    fputs("\n", temp); // Add newline after each username
                } else {
                    member_found = 1;
                }
            }

            fclose(members);
            fclose(temp);
            remove("members.txt");
            rename("temp.txt", "members.txt");

            if (member_found) {
                write(nsd, "Member deleted successfully!\n", strlen("Member deleted successfully!\n"));
                printf("member deleted");
            } else {
                write(nsd, "Member not found!\n", strlen("Member not found!\n"));
                printf("member not found");
            }
        } else if (strcmp(choice, "3") == 0) {
            // Add book
            file = fopen("books.txt", "a");
            if (file == NULL) {
                perror("Error opening books file!\n");
                return;
            }

            // Receive book title to add
            ret = read(nsd, buf, sizeof(buf) - 1);
            if (ret == -1) {
                perror("Read error");
                fclose(file);
                return;
            }
            buf[ret] = '\0';
            printf("book added:%s\n",buf);
            fprintf(file, "%s\n", buf);
            fclose(file);

            // Success message to the client
            write(nsd, "Book added successfully!\n", strlen("Book added successfully!\n"));
        } else if (strcmp(choice, "4") == 0) {
            // Delete book
            FILE *books = fopen("books.txt", "r");
            if (books == NULL) {
                perror("Error opening books file!\n");
                return;
            }

            // Receive book title to delete
            ret = read(nsd, buf, sizeof(buf) - 1);
            if (ret == -1) {
                perror("Read error");
                fclose(books);
                return;
            }
            buf[ret] = '\0';
            printf("book deleted");
            char delete_title[20];
            strcpy(delete_title, buf);

            FILE *temp = fopen("temp.txt", "w");
            if (temp == NULL) {
                perror("Error creating temporary file!\n");
                fclose(books);
                return;
            }

            int book_found = 0;
            char temp_book[20];
            while (fgets(temp_book, sizeof(temp_book), books)) {
                temp_book[strcspn(temp_book, "\n")] = '\0'; // Remove newline character
                if (strcmp(temp_book, delete_title) != 0) {
                    fputs(temp_book, temp);
                    fputs("\n", temp); // Add newline after each book title
                } else {
                    book_found = 1;
                }
            }

            fclose(books);
            fclose(temp);
            remove("books.txt");
            rename("temp.txt", "books.txt");

            if (book_found) {
                write(nsd, "Book deleted!\n", strlen("Book deleted!\n"));//book to be deleted
            } else {
                write(nsd, "Book not found!\n", strlen("Book not found!\n"));
            }
        } else if (strcmp(choice, "5") == 0) {
            // Modify book title
            char book_title[MAX_BOOK_TITLE_LENGTH];
            char modified_title[MAX_BOOK_TITLE_LENGTH];

            // Receive book titles from client
            ret = read(nsd, book_title, sizeof(book_title) - 1);
            if (ret == -1) {
                perror("Read error");
                return;
            }
            book_title[ret] = '\0';

            ret = read(nsd, modified_title, sizeof(modified_title) - 1);
            if (ret == -1) {
                perror("Read error");
                return;
            }
            modified_title[ret] = '\0';

            FILE *books = fopen("books.txt", "r+");
            if (books == NULL) {
                perror("Error opening books file!\n");
                return;
            }

            char temp_book[MAX_BOOK_TITLE_LENGTH];
            int book_found = 0;
            FILE *temp = fopen("temp.txt", "w");
            if (temp == NULL) {
                perror("Error creating temporary file!\n");
                fclose(books);
                return;
            }

            while (fgets(temp_book, sizeof(temp_book), books)) {
                temp_book[strcspn(temp_book, "\n")] = '\0'; // Remove newline character
                if (strcmp(temp_book, book_title) == 0) {
                    fprintf(temp, "%s\n", modified_title); // Write modified title
                    book_found = 1;
                } else {
                    fprintf(temp, "%s\n", temp_book); // Copy unchanged titles
                }
            }

            fclose(books);
            fclose(temp);
            remove("books.txt");
            rename("temp.txt", "books.txt");

            if (book_found) {
                write(nsd, "Book modified!\n", strlen("Book modified!\n"));
            } else {
                write(nsd, "Book not found!\n", strlen("Book not found!\n"));
            }
        } else if (strcmp(choice, "6") == 0) {
            // Search for book
            FILE *books = fopen("books.txt", "r");
            if (books == NULL) {
                perror("Error opening books file!\n");
                return;
            }

            // Receive book title to search
            ret = read(nsd, buf, sizeof(buf) - 1);
            if (ret == -1) {
                perror("Read error");
                fclose(books);
                return;
            }
            buf[ret] = '\0';
            printf("book modified");
            char search_title[20];
            strcpy(search_title, buf);

            int book_found = 0;
            char temp_book[20];
            while (fgets(temp_book, sizeof(temp_book), books)) {
                temp_book[strcspn(temp_book, "\n")] = '\0'; // Remove newline character
                if (strcmp(temp_book, search_title) == 0) {
                    write(nsd, temp_book, strlen(temp_book));
                    book_found = 1;
                    break;
                }
            }

            fclose(books);

            if (!book_found) {
                write(nsd, "Book not found!\n", strlen("Book not found!\n"));
            }
        } else if (strcmp(choice, "7") == 0) {
            write(nsd, "Logging out...\n", strlen("Logging out...\n"));
            close(nsd);
            exit(0);
        } else {
            write(nsd, "Invalid choice\n", strlen("Invalid choice\n"));
        }
    } while (strcmp(choice, "7") != 0);
}

void process_member(int nsd) {
    char buf[100], choice[10];
    int ret;
    FILE *books;

    do {
        // Receive the choice from the client
        ret = read(nsd, buf, sizeof(buf) - 1);
        if (ret == -1) {
            perror("Read error");
            return;
        }
        buf[ret] = '\0';
        strcpy(choice, buf);

        if (strcmp(choice, "1") == 0) {
            // Search for book
            books = fopen("books.txt", "r");
            if (books == NULL) {
                perror("Error opening books file!\n");
                return;
            }

            // Receive book title to search
            ret = read(nsd, buf, sizeof(buf) - 1);
            if (ret == -1) {
                perror("Read error");
                fclose(books);
                return;
            }
            buf[ret] = '\0';
            char search_title[20];
            strcpy(search_title, buf);

            int book_found = 0;
            char temp_book[20];
            while (fgets(temp_book, sizeof(temp_book), books)) {
                temp_book[strcspn(temp_book, "\n")] = '\0'; // Remove newline character
                if (strcmp(temp_book, search_title) == 0) {
                    write(nsd, temp_book, strlen(temp_book));
                    book_found = 1;
                    break;
                }
            }

            fclose(books);

            if (!book_found) {
                write(nsd, "Book not found!\n", strlen("Book not found!\n"));
            }
        } else if (strcmp(choice, "2") == 0) {
            write(nsd, "Logging out...\n", strlen("Logging out...\n"));
            close(nsd);
            exit(0);
        } else {
            write(nsd, "Invalid choice\n", strlen("Invalid choice\n"));
        }
    } while (strcmp(choice, "2") != 0);
}

