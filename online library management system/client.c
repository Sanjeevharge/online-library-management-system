#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
//imt2022530
#define USER_TYPE_LEN 10
#define USERNAME_LEN 20
#define PASSWORD_LEN 20
#define MEMBER_NAME_LEN 20
#define BOOK_TITLE_LEN 50
#define CHOICE_LEN 10

int main() {
//initializations
    struct sockaddr_in server_address;
    int sock_descriptor;
    char user_role[USER_TYPE_LEN];
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char member_name[MEMBER_NAME_LEN];
    char book_name[BOOK_TITLE_LEN], search_query[BOOK_TITLE_LEN];
    char buffer[100];
    int response_length;
    char menu_choice[CHOICE_LEN];

    // Create socket
    sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_descriptor == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Server address setup
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change this to your server IP
    server_address.sin_port = htons(5000); // Change this to your server port

    // Connect to the server
    if (connect(sock_descriptor, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Connection failed");
        close(sock_descriptor);
        exit(EXIT_FAILURE);
    }

    // Authentication
    printf("Welcome to the online library\n");

    printf("Are you an admin or a member? (admin/member): ");
    scanf("%9s", user_role);

    write(sock_descriptor, user_role, strlen(user_role));

    printf("Enter your username: ");
    scanf("%19s", username);
    write(sock_descriptor, username, strlen(username));

    printf("Enter your password: ");
    scanf("%19s", password);
    write(sock_descriptor, password, strlen(password));

    // Read response from server
    while(getchar() != '\n'); // Clear input buffer
    getchar(); 
    response_length = read(sock_descriptor, buffer, sizeof(buffer));
    if (response_length == -1) {
        perror("Read error");
        close(sock_descriptor);
        exit(EXIT_FAILURE);
    }
    write(1, buffer, response_length);

    // Main menu loop
    while(1) {
        if (strcmp(user_role, "admin") == 0) {
            printf("\nAdmin Menu:\n");
            printf("1. Add member\n");
            printf("2. Delete member\n");
            printf("3. Add book\n");
            printf("4. Delete book\n");
            printf("5. Modify book\n");
            printf("6. Search book\n");
            printf("7. Exit\n");
            printf("Enter your choice: ");
            scanf("%9s", menu_choice);

            // Send choice to server
            write(sock_descriptor, menu_choice, strlen(menu_choice));

            // Handle different choices
            if (strcmp(menu_choice, "1") == 0) {
                int file_descriptor = open("members.txt", O_RDWR);
                printf("Inside Critical Section\n");

                struct flock lock;
                lock.l_type = F_WRLCK;
                lock.l_whence = SEEK_SET;
                lock.l_start = 0;
                lock.l_len = 0;
                lock.l_pid = getpid();

                // Set write lock
                fcntl(file_descriptor, F_SETLKW, &lock);

                // Critical section
                printf("Enter member username: ");
                scanf("%s", member_name);
                write(sock_descriptor, member_name, strlen(member_name));
                while(getchar() != '\n'); // Clear input buffer
                getchar(); 
                response_length = read(sock_descriptor, buffer, sizeof(buffer));
                if (response_length == -1) {
                    perror("Read error");
                    close(file_descriptor);
                    close(sock_descriptor);
                    exit(EXIT_FAILURE);
                }
                write(1, buffer, response_length);

                // Unlock
                lock.l_type = F_UNLCK;
                fcntl(file_descriptor, F_SETLK, &lock);  
                printf("Unlock successful\n");

                close(file_descriptor);
            } else if (strcmp(menu_choice, "2") == 0) {
                int file_descriptor = open("members.txt", O_RDWR);
                printf("Inside Critical Section\n");

                struct flock lock;
                lock.l_type = F_WRLCK;
                lock.l_whence = SEEK_SET;
                lock.l_start = 0;
                lock.l_len = 0;
                lock.l_pid = getpid();

                // Set write lock
                fcntl(file_descriptor, F_SETLKW, &lock);

                // Critical section
                printf("Enter member username to be deleted: ");
                scanf("%s", member_name);
                write(sock_descriptor, member_name, strlen(member_name));
                while(getchar() != '\n'); // Clear input buffer
                getchar(); 
                response_length = read(sock_descriptor, buffer, sizeof(buffer));
                if (response_length == -1) {
                    perror("Read error");
                    close(file_descriptor);
                    close(sock_descriptor);
                    exit(EXIT_FAILURE);
                }
                write(1, buffer, response_length);

                // Unlock
                lock.l_type = F_UNLCK;
                fcntl(file_descriptor, F_SETLK, &lock);  
                printf("Unlock successful\n");

                close(file_descriptor);
            } else if (strcmp(menu_choice, "3") == 0) {
                int file_descriptor = open("books.txt", O_RDWR);
                printf("Inside Critical Section\n");

                struct flock lock;
                lock.l_type = F_WRLCK;
                lock.l_whence = SEEK_SET;
                lock.l_start = 0;
                lock.l_len = 0;
                lock.l_pid = getpid();

                // Set write lock
                fcntl(file_descriptor, F_SETLKW, &lock);

                // Critical section
                printf("Enter book title: ");
                scanf("%s", book_name);
                write(sock_descriptor, book_name, strlen(book_name));
                while(getchar() != '\n'); // Clear input buffer
                getchar(); 
                response_length = read(sock_descriptor, buffer, sizeof(buffer));
                if (response_length == -1) {
                    perror("Read error");
                    close(file_descriptor);
                    close(sock_descriptor);
                    exit(EXIT_FAILURE);
                }
                write(1, buffer, response_length);

                // Unlock
                lock.l_type = F_UNLCK;
                fcntl(file_descriptor, F_SETLK, &lock);  
                printf("Unlock successful\n");

                close(file_descriptor);
            } else if (strcmp(menu_choice, "4") == 0) {
                int file_descriptor = open("books.txt", O_RDWR);
                printf("Inside Critical Section\n");

                struct flock lock;
                lock.l_type = F_WRLCK;
                lock.l_whence = SEEK_SET;
                lock.l_start = 0;
                lock.l_len = 0;
                lock.l_pid = getpid();

                // Set write lock
                fcntl(file_descriptor, F_SETLKW, &lock);

                // Critical section
                printf("Enter book title to be deleted: ");
                scanf("%s", book_name);
                write(sock_descriptor, book_name, strlen(book_name));
                while(getchar() != '\n'); // Clear input buffer
                getchar(); 
                response_length = read(sock_descriptor, buffer, sizeof(buffer));
                if (response_length == -1) {
                    perror("Read error");
                    close(file_descriptor);
                    close(sock_descriptor);
                    exit(EXIT_FAILURE);
                }
                write(1, buffer, response_length);

                // Unlock
                lock.l_type = F_UNLCK;
                fcntl(file_descriptor, F_SETLK, &lock);  
                printf("Unlock successful\n");

                close(file_descriptor);
            } else if (strcmp(menu_choice, "5") == 0) {
                FILE *books_file = fopen("books.txt", "r+");
                if (books_file == NULL) {
                    printf("Error opening books file!\n");
                    return 1;
                }

                char original_title[BOOK_TITLE_LEN];
                char new_title[BOOK_TITLE_LEN];

                // Receive input from user
                printf("Enter book title to modify: ");
                scanf("%s", original_title);
                printf("Enter new title: ");
                scanf("%s", new_title);

                // Send book titles to server
                write(sock_descriptor, original_title, strlen(original_title));
                write(sock_descriptor, new_title, strlen(new_title));

                // Read response from server
                response_length = read(sock_descriptor, buffer, sizeof(buffer));
                if (response_length == -1) {
                    perror("Read error");
                    fclose(books_file);
                    close(sock_descriptor);
                    exit(EXIT_FAILURE);
                }
                write(1, buffer, response_length);

                fclose(books_file);
            } else if (strcmp(menu_choice, "6") == 0) {
                int file_descriptor = open("books.txt", O_RDONLY);
                printf("Inside Critical Section\n");
//entering cs
                struct flock lock;
                lock.l_type = F_RDLCK;
                lock.l_whence = SEEK_SET;
                lock.l_start = 0;
                lock.l_len = 0;
                lock.l_pid = getpid();

                // Set read lock
                fcntl(file_descriptor, F_SETLKW, &lock);

                // Critical section
                printf("Enter book title to search: ");
                scanf("%s", search_query);
                write(sock_descriptor, search_query, strlen(search_query));
                while(getchar() != '\n'); // Clear input buffer
                getchar(); 
                response_length = read(sock_descriptor, buffer, sizeof(buffer));
                if (response_length == -1) {
                    perror("Read error");
                    close(file_descriptor);
                    close(sock_descriptor);
                    exit(EXIT_FAILURE);
                }
                write(1, buffer, response_length);
                printf("\n");

                // Unlock of cs
                lock.l_type = F_UNLCK;
                fcntl(file_descriptor, F_SETLK, &lock);  
                printf("Unlock successful\n");

                close(file_descriptor);
            } else if (strcmp(menu_choice, "7") == 0) {
                close(sock_descriptor);
                exit(0);
            } else {
                printf("Invalid choice\n");
            }
        } else if (strcmp(user_role, "member") == 0) {
            printf("\nMember Menu:\n");
            printf("1. Search book\n");
            printf("2. Exit\n");
            printf("Enter your choice: ");
            scanf("%9s", menu_choice);

            // Send choice to server
            write(sock_descriptor, menu_choice, strlen(menu_choice));

            // Handle different choices
            if (strcmp(menu_choice, "1") == 0) {
                int file_descriptor = open("books.txt", O_RDONLY);

                printf("Inside Critical Section\n");

                struct flock lock;
                lock.l_start = 0;
                lock.l_len = 0;
                lock.l_whence = SEEK_SET;
                lock.l_type = F_RDLCK;

                int res = fcntl(file_descriptor, F_SETLKW, &lock);
                if (res < 0) {
                    printf("Lock unsuccessful\n");
                    return 0;
                }

                printf("Locked\n");

                printf("Enter book title to search: ");//searchig for th particular title
                scanf("%s", search_query);
                write(sock_descriptor, search_query, strlen(search_query));
                while(getchar() != '\n'); // Clear input buffer
                getchar(); 
                response_length = read(sock_descriptor, buffer, sizeof(buffer));
                if (response_length == -1) {
                    perror("Read error");
                    close(file_descriptor);
                    close(sock_descriptor);
                    exit(EXIT_FAILURE);
                }
                write(1, buffer, response_length);

                printf("Unlocking\n");
//unlocking of cs
                lock.l_type = F_UNLCK;
                fcntl(file_descriptor, F_SETLKW, &lock);  
                printf("Unlock successful");

                close(file_descriptor);
            } else if (strcmp(menu_choice, "2") == 0) {
                close(sock_descriptor);
                exit(0);
            } else {
                printf("Invalid choice\n");
            }
        } else {
            perror("Invalid user role!\n");
            close(sock_descriptor);
            exit(EXIT_FAILURE);
        }
    }

    close(sock_descriptor);

    return 0;
}

