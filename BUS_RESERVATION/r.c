#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#ifdef _WIN32
#include <windows.h> // Required for Sleep() on Windows
#else
#include <unistd.h> // Required for sleep() on Unix-based systems
#endif

// Define function to clear console screen
void clearScreen() {
    #ifdef _WIN32 // For Windows
    system("cls");
    #else // For Linux and macOS
    system("clear");
    #endif
}
#define MAX_PHONE_NUMBER_LENGTH 15
#define MAX_TIME_LENGTH 10
#define MAX_LOGIN_ATTEMPTS 3
#define MAX_SEATS 50
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_ROUTES 100
#define MAX_ROUTE_NAME_LENGTH 50
#define MAX_ORIGIN_DEST_LENGTH 50
#define MAX_BUS_NAME_LENGTH 50
#define MAX_SECURITY_LENGTH 100
#define NUM_SECURITY_QUESTIONS 3

// Admin credentials
const char ADMIN_USERNAME[] = "admin";
char ADMIN_PASSWORD[MAX_PASSWORD_LENGTH] = "adminpass";
const char SECURITY_QUESTION[] = "What is the capital of France?";
const char SECURITY_ANSWER[] = "Paris";
typedef struct {
    char username[MAX_USERNAME_LENGTH];
    char busname[MAX_BUS_NAME_LENGTH];
    char routename[MAX_ROUTE_NAME_LENGTH];
    char origin[MAX_ORIGIN_DEST_LENGTH];
    char destination[MAX_ORIGIN_DEST_LENGTH];
    char arrivaltime[MAX_TIME_LENGTH];
    int num_seats;
    int selected_seats[2];
    float ticket_price;
} Booking;

// Structure to represent a bus route
typedef struct {
    char bus_name[MAX_BUS_NAME_LENGTH];
    char route_name[MAX_ROUTE_NAME_LENGTH];
    char origin[MAX_ORIGIN_DEST_LENGTH];
    char destination[MAX_ORIGIN_DEST_LENGTH];
    char departure_time[10]; // Assuming departure time format HH:MM
    char arrival_time[10];
    float ticket_price;
    int deleted_flag; // Flag to indicate if the route is deleted (0 for not deleted, 1 for deleted)
} BusRoute;

// Structure to represent a seat
typedef struct {
    int seat_number;
    float price;
    int is_available;
} Seat;

// Structure to represent a user
typedef struct {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char profile[MAX_USERNAME_LENGTH];
    char phoneNumber[MAX_PHONE_NUMBER_LENGTH];
    char security_question[MAX_SECURITY_LENGTH];
    char security_answer[MAX_SECURITY_LENGTH];
} User;

// Function prototypes
// float calculate_total_cost(int num_reserve,int selected_seats[]);
void load_seat_data(Seat seats[], int num_seats, char *busname, char *routename, char *origin, char *destination, char *arrivaltime);
void print_ticket(Booking booking);
int chooseSecurityQuestion();
void displaySecurityQuestions();
void signoutTravelers();
void signoutAdmin();
void resetPassword(User *traveler);
void resetAdminPassword();
void writePasswordToFile(const char *password);
void readPasswordFromFile();
void adminMenu();
void travelerMenu(User *traveler);
void viewProfile(User user);
void updateProfile(User *user);
void travelerRegister(User *traveler);
int adminLogin();
int travelerLogin(User *traveler);
void make_reservation(Seat seats[], int num_seats, User *traveler, float ticket_price,
                      char *busname, char *routename, char *origin, char *destination, char *arrivaltime);
void save_seat_data(Seat seats[], int num_seats, char *busname, char *routename, char *origin, char *destination, char *arrivaltime);
void busroutemanagement();
void initializeSeats(Seat seats[], int num_seats);
void displaySeatStatus(Seat seats[], int num_seats, char busname[], char routeName[], char origin[], char destination[], char arrivaltime[]);
void updateSeatStatus(Seat seats[], int num_seats, char busname[], char routename[], char origin[], char destination[], char arrivaltime[]);
void searchBuses(User *traveler);
void view_booking_details();
void UpdateBusRoute();
void DeleteBusRoute();
float displayBusRoutes();
void seatmanagement();
void bookingmanagement(User *traveler);
void searchBusRoutes();
void clearInputBuffer();
void displaybusroutestravelers(char *route, char *destination, char *date, User *traveler);
void processPayment(float total_cost);
void cancel_reservation(User *traveler, char *busname, char *routename, char *origin, char *destination, char *arrivaltime);
 void AddBusRoute();   
    char ch[10][130]={"Cardiff Express","Belfast Express","Derby Express","Chester Express","Newport Express","Truro Expre"};
int main() {
    clearScreen(); // Clear screen when program starts
    User admin;
    int role;
    printf("\n\n                                         ================================================");
    sleep(1);       
    printf("\n            **********************************  Welcome to Bus Ticket Reservation System  ****************************");
    sleep(1);       
    printf("\n                                           =================================================");
    sleep(1);
    printf("\n\n                                                          1. Administrator Login");
    sleep(1);
    printf("\n\n                                                          2. Traveler login");
    sleep(0);
    printf("\n\n                                                          Choose your Option: ");
    sleep(0);
    scanf("%d", &role);
    clearScreen(); // Clear screen after user input
    if (role == 1) {
        readPasswordFromFile(); // Read password from file
        if (adminLogin()) {
            clearScreen();
            adminMenu();
        }
    } else if (role == 2) {
        int loginChoice;
        User traveler;
        printf("\n\n                   **************************************************************");
        printf("\n\n\n                                             1. Login");
        printf("\n\n                                               Are you a new User?");
        printf("\n\n                                               2. Register");
        printf("\n\n\n                                             Enter your choice: ");
        scanf("%d", &loginChoice);
        if (loginChoice == 1) {
            if (travelerLogin(&traveler)) {
                travelerMenu(&traveler);
            }
        } else if (loginChoice == 2) {
            travelerRegister(&traveler);
            travelerMenu(&traveler);
        } else {
            printf("\n Invalid choice! Exiting...");
        }
    } else {
        printf("\n Invalid role! Exiting...");
    }
    return 0;
}
// Function to disable echoing in terminal
void disableEcho() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ECHO; // Turn off echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Function to enable echoing in terminal
void enableEcho() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ECHO; // Turn on echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
// Admin login function
int adminLogin() {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int attempts = 0;

    clearScreen();

    do {
        printf("\n                                             =============================================");
        printf("\n                           **********************************  Administrator Login   **********************************");
        printf("\n                                             =============================================");

        printf("\n\n                                                          Username: ");
        scanf("%s", username);
         printf("\n                                                           Password: ");
        disableEcho(); // Disable echoing
        scanf("%s", password);
        enableEcho(); // Enable echoing

        // Check if username and password are correct
        if (strcmp(username, ADMIN_USERNAME) == 0 && strcmp(password, ADMIN_PASSWORD) == 0) {
            printf("\n\n      Login successful!");
            clearScreen();
            return 1;
            clearScreen();
        } else {
            attempts++;
            printf("\n\n\n                                                  * Incorrect username or password!");

            if (attempts < MAX_LOGIN_ATTEMPTS) {
                printf(" You have %d attempts left.", MAX_LOGIN_ATTEMPTS - attempts);
            } else {
                printf(" You have reached the maximum number of login attempts.");
                char choice;
                printf("\n\n\n                                                  * Do you want to reset your password? (Y/N): ");
                scanf(" %c", &choice);
                if (choice == 'Y' || choice == 'y') {
                    clearScreen();
                    resetAdminPassword();
                    return 0;
                } else {
                    return 0;
                }
            }
        }
    } while (attempts < MAX_LOGIN_ATTEMPTS);

    return 0;
}

void resetAdminPassword() {
    char answer[100];
    char new_password[MAX_PASSWORD_LENGTH];

    clearScreen();

    printf("\n\n\n                                                  * To reset your password, please answer the following security question:");
    printf("\n                                                  * %s\n", SECURITY_QUESTION);
    printf("\n\n\n                                                  * Your Answer: ");
    scanf("%s", answer);

    if (strcmp(answer, SECURITY_ANSWER) == 0) {
        printf("\n\n\n                                                  * Security question verified! Please enter your new password: ");
        scanf("%s", new_password);

        // Update the password
        strcpy(ADMIN_PASSWORD, new_password);
        writePasswordToFile(new_password);
        printf("\n\n\n                                                  * Password updated successfully!");
    } else {
        printf("\n\n\n                                                  * Incorrect answer to security question. Password reset failed.");
    }

    printf("\n\n\n                                                  * Press Enter to continue...");
    while (getchar() != '\n');
    getchar();

    // Go back to login phase
    adminLogin();
    clearScreen();
    adminMenu();
}
// Function to write password to file
void writePasswordToFile(const char *password) {
    FILE *file = fopen("admin_password.txt", "w");
    if (file != NULL) {
        fputs(password, file);
        fclose(file);
    } else {
        printf("\n\n\n                                                  * Error writing password to file.");
    }
}

// Function to read password from file
void readPasswordFromFile() {
    FILE *file = fopen("admin_password.txt", "r");
    if (file != NULL) {
        fgets(ADMIN_PASSWORD, MAX_PASSWORD_LENGTH, file);
        fclose(file);
    } else {
        printf("\n\n\n                                                  * Error reading password from file.");
    }
}
void signoutAdmin(){
   clearScreen();
    printf("\n                            --------------------------- Signing out... --------------------------");
    getchar();
    getchar();
    clearScreen(); // Clear the screen
    adminLogin(); // Go back to the admin login interface
    clearScreen(); // Clear the screen after login
    adminMenu(); // Display the admin menu
}

// Admin menu function
void adminMenu() {
    User admin;
    int choice;
    do {
        clearScreen(); // Clear screen before displaying menu
        printf("\n\n                      ================================================");
        printf("\n  ********************************  Welcome to the Admin Panel  *********************************");
        printf("\n                      ================================================");
        printf("\n\n                                   1. Bus Information Management");
        printf("\n\n                                   2. Seats management");
        printf("\n\n                                   3. Booking Management");
        printf("\n\n                                   4. View Profile");
        printf("\n\n                                   5. Update Profile");
        printf("\n\n                                   6. Sign out and Exit");
        printf("\n\n                                What do you want to manage: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
              busroutemanagement(); // Implement bus route management
                break;
            case 2:
               seatmanagement();// Implement seats management
                break;
            case 3:
              bookingmanagement(&admin); // Implement booking management
                break;
            case 4:
                viewProfile((User){.username = "admin", .profile = "admin"});
                break;
            case 5:
               updateProfile(&admin); // Implement update profile
                break;   
            case 6:
                signoutAdmin();
                return;
            default:
                printf("\n                       * Invalid choice! Please enter a valid option.");
        }
        printf("\n\n Press Enter to continue...");
        while (getchar() != '\n'); // Clear input buffer
        getchar(); // Wait for Enter key
        clearScreen(); // Clear screen before next iteration
    } while (1);
}
// Traveler login function
void displaySecurityQuestions() {
    printf("\n Select a security question:");
    printf("\n 1. What is your mother's maiden name?");
    printf("\n 2. What is the name of your first pet?");
    printf("\n 3. In what city were you born?");
}

int chooseSecurityQuestion() {
    int choice;
    displaySecurityQuestions();
    printf("\n Enter the number of your choice: ");
    scanf("%d", &choice);
    clearInputBuffer(); // Clear input buffer
    return choice;
}
void readUserDetails(User *user) {
     printf("\n Enter your Username (no spaces allowed): ");
     scanf("%s", user->username);
     user->username[strcspn(user->username, "\n")] = '\0'; 
 
     while (strchr(user->username, ' ') != NULL) {
         printf("\n Username cannot contain spaces. Please try again.");
         printf("\n Enter your Username (no spaces allowed): ");
         scanf("%s", user->username);
         user->username[strcspn(user->username, "\n")] = '\0'; 
     }
 
     printf("\n Enter your password: ");
     scanf("%s", user->password);
 
     printf("\n Enter your phone number: ");
     scanf("%s", user->phoneNumber);
 
     int choice = chooseSecurityQuestion();
     switch (choice) {
         case 1:
             strcpy(user->security_question, "What is your mother's maiden name?");
             break;
         case 2:
             strcpy(user->security_question, "What is the name of your first pet?");
             break;
         case 3:
             strcpy(user->security_question, "In what city were you born?");
             break;
         default:
             printf("\n Invalid choice. Defaulting to question 1.");
             strcpy(user->security_answer, "What is your mother's maiden name?");
     }
 
     printf("\n Enter your answer to the security question: ");
     scanf("%[^\n]s", user->security_answer);
 }
 
 void writeUserDetailsToFile(User *user) {
     FILE *file = fopen("travelers.txt", "a");
     if (file != NULL) {
         fprintf(file, "%s %s %s %s %s\n", user->username, user->password, user->phoneNumber, user->security_question, user->security_answer);
         fclose(file);
         printf("\n Registration successful! Please login now.");
     } else {
         printf("\n Error registering traveler.");
     }
 }
 
 void travelerRegister(User *travelers) {
     readUserDetails(travelers);
     writeUserDetailsToFile(travelers);
 }
 void resetPassword(User *travelers) {
    char username[MAX_USERNAME_LENGTH];
    char selected_question[MAX_SECURITY_LENGTH];
    char security_answer[MAX_SECURITY_LENGTH];
    char new_password[MAX_PASSWORD_LENGTH];
    int choice;

    printf("\n=====================================");
    printf("\n Password Reset");
    printf("\n=====================================");

    printf("\n Enter your username: ");
    scanf("%s", username);

    char registered_question[MAX_SECURITY_LENGTH];
    char registered_answer[MAX_SECURITY_LENGTH];
    FILE *file = fopen("travelers.txt", "r");
    if (file != NULL) {
        char tempUsername[MAX_USERNAME_LENGTH], tempPassword[MAX_PASSWORD_LENGTH], tempPhoneNumber[MAX_PHONE_NUMBER_LENGTH], tempSecurityQuestion[MAX_SECURITY_LENGTH], tempSecurityAnswer[MAX_SECURITY_LENGTH];
        while (fscanf(file, "%s %s %s %[^\n]s %s", tempUsername, tempPassword, tempPhoneNumber, tempSecurityQuestion, tempSecurityAnswer) != EOF) {
            if (strcmp(tempUsername, username) == 0) {
                strcpy(registered_question, tempSecurityQuestion);
                strcpy(registered_answer, tempSecurityAnswer);
                break;
            }
        }
        fclose(file);
    } else {
        printf("\n Error reading user data. Please try again later.");
        return;
    }
    printf("\n Security Question: %[^\n]s\n", registered_question);
    
    printf("\n Enter your answer to the security question: ");
    scanf("%s", security_answer);

    if (strcmp(security_answer, registered_answer) == 0) {
        printf("\n Security question verified. Please enter your new password: ");
        scanf("%s", new_password);

        // Update password in the file
        file = fopen("travelers.txt", "r+");
        if (file != NULL) {
            char tempUsername[MAX_USERNAME_LENGTH], tempPassword[MAX_PASSWORD_LENGTH], tempPhoneNumber[MAX_PHONE_NUMBER_LENGTH], tempSecurityQuestion[MAX_SECURITY_LENGTH], tempSecurityAnswer[MAX_SECURITY_LENGTH];
            while (fscanf(file, "%s %s %s %[^\n]s %s", tempUsername, tempPassword, tempPhoneNumber, tempSecurityQuestion, tempSecurityAnswer) != EOF) {
                if (strcmp(tempUsername, username) == 0) {
                    fseek(file, -strlen(tempPassword) - strlen(tempPhoneNumber) - strlen(tempSecurityQuestion) - strlen(tempSecurityAnswer) - 4, SEEK_CUR);
                    fprintf(file, "%s %s %s %[^\n]s %s\n", tempUsername, new_password, tempPhoneNumber, tempSecurityQuestion, tempSecurityAnswer);
                    break;
                }
            }
            fclose(file);
            printf("\n Password reset successfully!");
        } else {
            printf("\n Error resetting password. Please try again later.");
        }
    } else {
        printf("\n Incorrect security answer. Password reset failed.");
    }
}
int travelerLogin(User *travelers) {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int attempts = 0;
 
    printf("\n=====================================");
    printf("\n Traveler Login");
    printf("\n=====================================");
 
    while (attempts < MAX_LOGIN_ATTEMPTS) {
        printf("\n Enter your username: ");
         scanf("%s", username);
         clearInputBuffer();
 
         printf(" Enter your password: ");
         scanf("%s", password);
 
         FILE *file = fopen("travelers.txt", "r");
         if (file != NULL) {
             User user;
             while (fscanf(file, "%s %s %s %s %s", user.username, user.password, user.phoneNumber, user.security_question, user.security_answer) != EOF) {
                 if (strcmp(username, user.username) == 0 && strcmp(password, user.password) == 0) {
                     fclose(file);
                     printf("\n Login successful!");
                     return 1;
                 }
             }
             fclose(file);
         } else {
             printf("\n Error reading user data. Please try again later.");
             return 0;
         }
 
         attempts++;
         printf("\n Incorrect username or password! Please try again.");
         if (attempts < MAX_LOGIN_ATTEMPTS) {
             printf("\n Remaining attempts: %d", MAX_LOGIN_ATTEMPTS - attempts);
         } else {
             printf("\n Maximum login attempts reached. Do you want to reset your password? (Y/N): ");
             char choice;
             scanf(" %c", &choice);
             if (choice == 'Y' || choice == 'y') {
                 resetPassword(travelers);
             } else {
                 printf("\n Exiting...");
             }
             return 0;
         }
     }
 
     return 0;
 }
// Traveler menu function
void travelerMenu(User *traveler) {
    int choice;
    do {
        clearScreen(); // Clear screen before displaying menu
        printf("\n\n=====================================");
        printf("\n   Welcome, %s", traveler->username);
        printf("\n =====================================");
        printf("\n 1. View Profile");
        printf("\n 2. Update Profile");
        printf("\n 3. Search Buses");
        printf("\n 4. View Booking Details");
        printf("\n 5. Cancel Booking");
        printf("\n 6. Sign out and Exit");
        printf("\n\n Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                viewProfile(*traveler);
                break;
            case 2:
                updateProfile(traveler);
                break;
            case 3:
                searchBuses(traveler);
                break;
            case 4:
                view_booking_details(traveler);
                break;
            case 5: {
                char busname[MAX_BUS_NAME_LENGTH];
                char routename[MAX_ROUTE_NAME_LENGTH];
                char origin[MAX_ORIGIN_DEST_LENGTH];
                char destination[MAX_ORIGIN_DEST_LENGTH];
                char arrivaltime[MAX_TIME_LENGTH];

                printf("\nEnter the booking details to cancel the reservation:\n");
                printf("Bus Name: ");
                scanf("%s", busname);
                printf("Route Name: ");
                scanf("%s", routename);
                printf("Origin: ");
                scanf("%s", origin);
                printf("Destination: ");
                scanf("%s", destination);
                printf("Arrival Time (HH:MM): ");
                scanf("%s", arrivaltime);

                cancel_reservation(traveler, busname, routename, origin, destination, arrivaltime);
                break;
            }
            case 6:
                printf("\n--------------------------- Signing out... --------------------------\n");
                clearScreen();
                // signoutTravelers(); // Assuming this function is defined elsewhere
                return;
            default:
                printf("\n* Invalid choice! Please enter a valid option.\n");
        }
        printf("\n\nPress Enter to continue...");
        while (getchar() != '\n'); // Clear input buffer
        getchar(); // Wait for Enter key
        clearScreen(); // Clear screen before next iteration
    } while (1);
}

void viewProfile(User user) {
    printf("\n\n=======================================================");
    printf("\n                       User Profile                      ");
    printf("\n=======================================================");
    printf("\n Username: %s", user.username);
    printf("\n Profile: %s", user.profile);
    // printf("\n\n Press Enter to continue...");
    getchar(); // Wait for Enter key
}

// Function to update user profile
void updateProfile(User *user) {
    printf("\n\n=======================================================");
    printf("\n                       Update Profile                      ");
    printf("\n=======================================================");
    printf("\n Enter new username: ");
    scanf("%s", user->username);
    printf(" Enter new password: ");
    scanf("%s", user->password);
    printf("\n Profile updated successfully!");
    // printf("\n\n Press Enter to continue...");
    getchar(); // Wait for Enter key
}
// Function to search for buses 
void searchBuses(User *traveler) {
    // Function to search for buses
    printf("Searching for buses...\n");
    char route[MAX_ROUTE_NAME_LENGTH];
    char destination[MAX_ORIGIN_DEST_LENGTH];
    char date[15];

    clearScreen();
    printf("\n\n=====================================");
    printf("\n          Search Buses");
    printf("\n=====================================");

    printf("\n Enter Route: ");
    scanf("%s", route);
    printf(" Enter Destination: ");
    scanf("%s", destination);
    printf(" Enter Date (YYYY-MM-DD): ");
    scanf("%s", date);

    // Display available bus routes based on the search criteria
    displaybusroutestravelers(route, destination, date, traveler);
}

// Function to display available bus routes for travelers
void displaybusroutestravelers(char *route, char *destination, char *date, User *traveler) {
    clearScreen();
    FILE *file = fopen("bus_routes.txt", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }

    Seat seats[MAX_SEATS];
    int num_seats = MAX_SEATS;
    char busname[MAX_BUS_NAME_LENGTH];
    char routename[MAX_ROUTE_NAME_LENGTH];
    char origin[MAX_ORIGIN_DEST_LENGTH];
    char arrivaltime[MAX_TIME_LENGTH];
    int found = 0;
    char temp_busname[MAX_BUS_NAME_LENGTH], temp_routename[MAX_ROUTE_NAME_LENGTH],
         temp_origin[MAX_ORIGIN_DEST_LENGTH], temp_destination[MAX_ORIGIN_DEST_LENGTH],
         temp_arrivaltime[MAX_TIME_LENGTH], temp_departure_time[MAX_TIME_LENGTH];
    float ticket_price;
    int deleted_flag;

    printf("\n                Search Result for Route: %s, Destination: %s, Date: %s\n", route, destination, date);
    printf("                      ========================================\n");

    while (fscanf(file, "%s %s %s %s %s %s %f %d", temp_busname,
                  temp_routename, temp_origin, temp_destination,
                  temp_arrivaltime, temp_departure_time, &ticket_price,
                  &deleted_flag) != EOF) {
        if (strcmp(temp_routename, route) == 0 &&
            strcmp(temp_destination, destination) == 0 &&
            !deleted_flag) {
            printf("Bus Name: %s\n", temp_busname);
            printf("Route Name: %s\n", temp_routename);
            printf("Origin: %s\n", temp_origin);
            printf("Destination: %s\n", temp_destination);
            printf("Arrival Time: %s\n", temp_arrivaltime);
            printf("Departure Time: %s\n", temp_departure_time);
            printf("Ticket Price: %.2f\n", ticket_price);
            printf("=============================================\n");
            found = 1;
        }
    }

    fclose(file);

    if (!found) {
        printf("\n\n               No bus routes found for Route: %s, Destination: %s, Date: %s\n", route, destination, date);
    } else {
        // Prompt the traveler to select a bus route
        printf("\n\nEnter the name of the bus, route, origin, destination, and arrival time you want to select:\n");
        scanf("%s %s %s %s %s", busname, routename, origin, destination, arrivaltime);
  load_seat_data(seats, num_seats, temp_busname, temp_routename, temp_origin, temp_destination, temp_arrivaltime);
        // Now you can display the seat availability for the selected bus
        displaySeatStatus(seats, num_seats, busname, routename, origin, destination, arrivaltime);

        // Make reservation
        make_reservation(seats, num_seats, traveler, ticket_price, busname, routename, origin, destination, arrivaltime);
    }
} 
void make_reservation(Seat seats[], int num_seats, User *traveler, float ticket_price,
                      char *busname, char *routename, char *origin, char *destination, char *arrivaltime) {
    printf("\nEnter the number of seats to reserve (maximum 2): ");
    int num_reserve;
    scanf("%d", &num_reserve);
    if (num_reserve > 2) {
        printf("You can only reserve up to 2 seats.\n");
        num_reserve = 2;
    }

    int selected_seats[num_reserve];
    printf("Enter the seat numbers to reserve (separated by spaces): ");
    for (int i = 0; i < num_reserve; i++) {
        scanf("%d", &selected_seats[i]);
    }

    // Confirm reservation
    printf("\nConfirm reservation of %d seats? (Y/N): ", num_reserve);
    char choice;
    scanf(" %c", &choice);
    if (choice == 'Y' || choice == 'y') {
        // Update seat availability
        for (int i = 0; i < num_reserve; i++) {
            seats[selected_seats[i] - 1].is_available = 0; // Mark seat as booked
        }

        // Save seat data
        save_seat_data(seats, num_seats, busname, routename, origin, destination, arrivaltime);

        // Store booking details
        FILE *booking_file = fopen("bookings.txt", "a");
        if (booking_file == NULL) {
            printf("Error opening booking file for appending.\n");
            return;
        }
        Booking booking;
        strcpy(booking.username, traveler->username);
        strcpy(booking.busname, busname);
        strcpy(booking.routename, routename);
        strcpy(booking.origin, origin);
        strcpy(booking.destination, destination);
        strcpy(booking.arrivaltime, arrivaltime);
        booking.num_seats = num_reserve;
        for (int i = 0; i < num_reserve; i++) {
            booking.selected_seats[i] = selected_seats[i];
        }
        fwrite(&booking, sizeof(Booking), 1, booking_file);
        fclose(booking_file);

        // Calculate and display total cost
        float total_cost = ticket_price * num_reserve;
        printf("\nReservation confirmed. Total cost: %.2f\n", total_cost);
         processPayment(total_cost);
        print_ticket(booking);

        // Process payment
       
    } else {
        printf("\nReservation cancelled.\n");
    }
}
void view_booking_details() {
Booking booking;
print_ticket(booking);

}

void cancel_reservation(User *traveler, char *busname, char *routename, char *origin, char *destination, char *arrivaltime) {

    // Load seat data
    Seat seats[MAX_SEATS];
    initializeSeats(seats, MAX_SEATS);
    load_seat_data(seats, MAX_SEATS, busname, routename, origin, destination, arrivaltime);

    // Open bookings file
    FILE *booking_file = fopen("bookings.txt", "r");
    FILE *temp_file = fopen("bookings_temp.txt", "w");
    if (booking_file == NULL || temp_file == NULL) {
        printf("Error opening booking file.\n");
        if (booking_file) fclose(booking_file);
        if (temp_file) fclose(temp_file);
        return;
    }

    Booking booking;
    int found = 0;
    while (fread(&booking, sizeof(Booking), 1, booking_file)) {
        if (strcmp(booking.username, traveler->username) == 0 &&
            strcmp(booking.busname, busname) == 0 &&
            strcmp(booking.routename, routename) == 0 &&
            strcmp(booking.origin, origin) == 0 &&
            strcmp(booking.destination, destination) == 0 &&
            strcmp(booking.arrivaltime, arrivaltime) == 0) {
            found = 1;
            // Mark seats as available
            for (int i = 0; i < booking.num_seats; i++) {
                seats[booking.selected_seats[i] - 1].is_available = 1;
            }
        } else {
            fwrite(&booking, sizeof(Booking), 1, temp_file);
        }
    }

    fclose(booking_file);
    fclose(temp_file);

    remove("bookings.txt");
    rename("bookings_temp.txt", "bookings.txt");

    if (found) {
        // Save updated seat data
        save_seat_data(seats, MAX_SEATS, busname, routename, origin, destination, arrivaltime);
        printf("\nReservation canceled successfully.\n");
    } else {
        printf("\nNo matching reservation found.\n");
    }
}
void print_ticket(Booking booking) {
    printf("\n         ----------- Ticket -----------\n");
    printf("            User: %s\n", booking.username);
    printf("            Bus Name: %s\n", booking.busname);
    printf("            Route Name: %s\n", booking.routename);
    printf("            Origin: %s\n", booking.origin);
    printf("            Destination: %s\n", booking.destination);
    printf("            Arrival Time: %s\n", booking.arrivaltime);
    printf("            Seats: ");
    for (int i = 0; i < booking.num_seats; i++) {
        printf("%d ", booking.selected_seats[i]);
    }
    printf("\n         ----------------------------------------\n");
}

void processPayment(float total_cost) {
    // Implement payment process using total_cost
    float amount_paid;
    printf("\n                    Total cost of tickets: $%.2f\n", total_cost);
    
    // Prompt user to enter the amount paid
    printf("  \n\n                Enter the amount paid: $");
    scanf("%f", &amount_paid);
    
    // Check if the amount paid is sufficient
    if (amount_paid >= total_cost) {
        float change = amount_paid - total_cost;
        printf("\n\n             Payment successful! Your change is: $%.2f\n", change);
    } else {
        printf("                 Insufficient amount paid. Payment failed.\n");
    }
}
void signoutTravelers(){
     
     clearScreen();
           int loginChoice;
      User traveler;
      clearScreen();
      printf("\n\n                   **************************************************************");
      printf("\n\n\n                                             1. Login");
      printf("\n\n                                               Are you a new User?");
      printf("\n\n                                               2. Register");
      printf("\n\n\n                                             Enter your choice: ");
      scanf("%d", &loginChoice);
      if (loginChoice == 1) {
          if (travelerLogin(&traveler)) {
              travelerMenu(&traveler);
          }
      } else if (loginChoice == 2) {
          travelerRegister(&traveler);
          travelerMenu(&traveler);
      } else  {
          printf("\n Invalid choice! Exiting...");
      }
 }
void initializeSeats(Seat seats[], int num_seats) {
    for (int i = 0; i < num_seats; i++) {
        seats[i].seat_number = i + 1;
        seats[i].is_available = 1; // Seat is available initially
    }
} 
void load_seat_data(Seat seats[], int num_seats, char *busname, char *routename, char *origin, char *destination, char *arrivaltime) {
    FILE *file = fopen("seat_data.txt", "r");
    if (file == NULL) {
        printf("Error opening seat data file for reading.\n");
        return;
    }

    int seat_number, is_available;
    float price;
    char temp_busname[MAX_BUS_NAME_LENGTH], temp_routename[MAX_ROUTE_NAME_LENGTH],
         temp_origin[MAX_ORIGIN_DEST_LENGTH], temp_destination[MAX_ORIGIN_DEST_LENGTH],
         temp_arrivaltime[MAX_TIME_LENGTH];
    while (fscanf(file, "%s %s %s %s %s %d %d %f", temp_busname,
                  temp_routename, temp_origin, temp_destination, temp_arrivaltime,
                  &seat_number, &is_available, &price) != EOF) {
        if (strcmp(temp_busname, busname) == 0 &&
            strcmp(temp_routename, routename) == 0 &&
            strcmp(temp_origin, origin) == 0 &&
            strcmp(temp_destination, destination) == 0 &&
            strcmp(temp_arrivaltime, arrivaltime) == 0) {
            seats[seat_number - 1].is_available = is_available;
        }
    }

    fclose(file);
}

void save_seat_data(Seat seats[], int num_seats, char *busname, char *routename, char *origin, char *destination, char *arrivaltime) {
    FILE *file = fopen("seat_data.txt", "r");
    if (file == NULL) {
        printf("Error opening seat data file for reading.\n");
        return;
    }

    FILE *temp_file = fopen("seat_data_temp.txt", "w");
    if (temp_file == NULL) {
        printf("Error opening temporary seat data file for writing.\n");
        fclose(file);
        return;
    }

    int seat_number, is_available;
    float price;
    char temp_busname[MAX_BUS_NAME_LENGTH], temp_routename[MAX_ROUTE_NAME_LENGTH],
         temp_origin[MAX_ORIGIN_DEST_LENGTH], temp_destination[MAX_ORIGIN_DEST_LENGTH],
         temp_arrivaltime[MAX_TIME_LENGTH];
    while (fscanf(file, "%s %s %s %s %s %d %d %f", temp_busname,
                  temp_routename, temp_origin, temp_destination, temp_arrivaltime,
                  &seat_number, &is_available, &price) != EOF) {
        if (strcmp(temp_busname, busname) == 0 &&
            strcmp(temp_routename, routename) == 0 &&
            strcmp(temp_origin, origin) == 0 &&
            strcmp(temp_destination, destination) == 0 &&
            strcmp(temp_arrivaltime, arrivaltime) == 0) {
            is_available = seats[seat_number - 1].is_available;
        }
        fprintf(temp_file, "%s %s %s %s %s %d %d %.2f\n", temp_busname,
                temp_routename, temp_origin, temp_destination, temp_arrivaltime,
                seat_number, is_available, price);
    }

    fclose(file);
    fclose(temp_file);

    remove("seat_data.txt");
    rename("seat_data_temp.txt", "seat_data.txt");
}
// void save_seat_data(Seat seats[], int num_seats, char busname[], char routename[], char origin[], char destination[], char arrivaltime[]) {
    // FILE *fp = fopen("seat_data.txt", "w");
    // if (fp == NULL) {
        // printf("Error opening file.\n");
        // return;
    // }
    // 
    // for (int i = 0; i < num_seats; i++) {
        // fprintf(fp, "%s %s %s %s %s %d\n", busname, routename, origin, destination, arrivaltime, seats[i].is_available);
    // }
    // 
    // fclose(fp);
// }

void updateSeatStatus(Seat seats[], int num_seats, char busname[], char routename[], char origin[], char destination[], char arrivaltime[]) {
    // Initialize variables
    int num_updates;
    printf("Enter the number of seats to update: ");
    scanf("%d", &num_updates);

    if (num_updates <= 0 || num_updates > num_seats) {
        printf("Invalid number of seats to update!\n");
        return;
    }

    int seat_numbers[num_updates];
    char choice;

    // Prompt for seat numbers to update status
    printf("Enter seat numbers to update status (separated by spaces): ");
    for (int i = 0; i < num_updates; i++) {
        scanf("%d", &seat_numbers[i]);
    }

    // Update seat statuses
    for (int i = 0; i < num_updates; i++) {
        int seat_index = seat_numbers[i] - 1; // Adjust for zero-based indexing

        if (seat_index < 0 || seat_index >= num_seats) {
            printf("Invalid seat number: %d\n", seat_numbers[i]);
            continue;
        }

        // Update seat status
        printf("Set seat %d as (A) Available or (B) Booked: ", seat_numbers[i]);
        scanf(" %c", &choice);
        if (choice == 'A' || choice == 'a') {
            seats[seat_index].is_available = 1;
            printf("Seat %d is now available.\n", seat_numbers[i]);
        } else if (choice == 'B' || choice == 'b') {
            seats[seat_index].is_available = 0;
            printf("Seat %d is now booked.\n", seat_numbers[i]);
        } else {
            printf("Invalid choice for seat %d!\n", seat_numbers[i]);
        }
    }

    // Save the updated data to file
     save_seat_data(seats, num_seats, busname, routename, origin, destination, arrivaltime);
}


void displaySeatStatus(Seat seats[], int num_seats, char busname[], char routeName[], char origin[], char destination[], char arrivaltime[]) {
    FILE *file = fopen("seat_data.txt", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }

    load_seat_data(seats, num_seats, busname, routeName, origin, destination, arrivaltime);
    printf("\n=============================================");
    printf("\n               Seat Status for %s", busname);
    printf("\n=============================================\n");

    for (int i = 0; i < num_seats; i++) {
        printf("Seat %d - %s\n", i + 1, seats[i].is_available ? "Available" : "Booked");
    }

    printf("\n=============================================\n");
    fclose(file);
}


void seatmanagement() {
    Seat seats[MAX_SEATS];
    int num_seats = MAX_SEATS;

    // Initialize seats with default values
    initializeSeats(seats, num_seats);

    int choice;
    do {
        printf("\n=====================================");
        printf("\n Seat Management");
        printf("\n=====================================");
        printf("\n 1. Display Seat Status" );
        printf("\n 2. Update Seat Status");
        printf("\n 3. Exit");
        printf("\n Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char busname[MAX_BUS_NAME_LENGTH];
                char routename[MAX_ROUTE_NAME_LENGTH];
                char origin[MAX_ORIGIN_DEST_LENGTH];
                char destination[MAX_ORIGIN_DEST_LENGTH];
                char arrivaltime[MAX_TIME_LENGTH];
                printf("Enter bus name: ");
                scanf("%s", busname);
                printf("Enter route name: ");
                scanf("%s", routename);
                printf("Enter origin: ");
                scanf("%s", origin);
                printf("Enter destination: ");
                scanf("%s", destination);
                printf("Enter arrival time (HH:MM): ");
                scanf("%s", arrivaltime);
                displaySeatStatus(seats, num_seats, busname, routename, origin, destination, arrivaltime);
                break;
            }
            case 2: {
                char busname[MAX_BUS_NAME_LENGTH];
                char routename[MAX_ROUTE_NAME_LENGTH];
                char origin[MAX_ORIGIN_DEST_LENGTH];
                char destination[MAX_ORIGIN_DEST_LENGTH];
                char arrivaltime[MAX_TIME_LENGTH];
                printf("Enter bus name: ");
                scanf("%s", busname);
                printf("Enter route name: ");
                scanf("%s", routename);
                printf("Enter origin: ");
                scanf("%s", origin);
                printf("Enter destination: ");
                scanf("%s", destination);
                printf("Enter arrival time (HH:MM): ");
                scanf("%s", arrivaltime);
                updateSeatStatus(seats, num_seats, busname, routename, origin, destination, arrivaltime);
                break;
            }
            case 3:
                printf("\nExiting...\n");
                break;
            default:
                printf("\nInvalid choice! Please enter a valid option.\n");
        }
    } while (choice != 3);
}

void busroutemanagement() {
    clearScreen();
    printf("\nBus Information Management");
    printf("\n1. Display Bus Routes");
    printf("\n2. Add Bus Route");
    printf("\n3. Update Bus Route");
    printf("\n4. Delete Bus Route");
    printf("\n5. Back to Admin Panel");
    printf("\nEnter your choice: ");
    
    int choice;
    scanf("%d", &choice);
    
    switch (choice) {
        case 1:
            clearScreen();
            displayBusRoutes();
            break;
        case 2:
            AddBusRoute();
            break;
        case 3:
            UpdateBusRoute();
            break;
        case 4:
            DeleteBusRoute();
            break;
        case 5:
            printf("Returning to Admin Panel...\n");
            break;
        default:
            printf("Invalid choice!\n");
    }
}
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
float displayBusRoutes() {
    clearScreen();
    char route_name[MAX_ROUTE_NAME_LENGTH];
    char destination[MAX_ORIGIN_DEST_LENGTH];
    char travel_date[11]; 
    clearScreen();
    printf("Enter the route name: ");
    scanf("%s", route_name);
    printf("Enter the destination: ");
    scanf("%s", destination);
    printf("Enter the travel date (YYYY-MM-DD): ");
    scanf("%s", travel_date);
    clearScreen();

    FILE *file = fopen("bus_routes.txt", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return 0; // Return 0 indicating failure
    }

    int found = 0;
    BusRoute currentRoute;
    float total_cost = 0; // Total cost accumulator

    printf("\n                Search Result for Route: %s, Destination: %s\n", route_name, destination);
    printf("                      =============================================\n");
    while (fscanf(file, "%s %s %s %s %s %s %f %d", currentRoute.bus_name, currentRoute.route_name, currentRoute.origin, currentRoute.destination, currentRoute.departure_time, currentRoute.arrival_time, &currentRoute.ticket_price, &currentRoute.deleted_flag) != EOF) {
        if (strcmp(currentRoute.route_name, route_name) == 0 && strcmp(currentRoute.destination, destination) == 0 && !currentRoute.deleted_flag) {
            printf("Bus Name: %s\n", currentRoute.bus_name);
            printf("Route Name: %s\n", currentRoute.route_name);
            printf("Origin: %s\n", currentRoute.origin);
            printf("Destination: %s\n", currentRoute.destination);
            printf("Arrival Time: %s\n", currentRoute.arrival_time);
            printf("Departure Time: %s\n", currentRoute.departure_time);
            printf("Ticket Price: %.2f\n", currentRoute.ticket_price);
            printf("Travel Date: %s\n", travel_date); 
            printf("                  ---------------------------------------------\n");
            
            // Increment total cost by ticket price
            total_cost += currentRoute.ticket_price;
            found = 1;
        }
    }

    fclose(file);

    if (found) {
        return total_cost; // Return total cost if routes found
    } else {
        return -1; // Return -1 indicating no routes found
    }
}

/// @brief 
void AddBusRoute() {
    clearScreen();
    BusRoute newRoute;
    printf("\nEnter bus name: ");
    scanf("%s", newRoute.bus_name);
    printf("Enter route name: ");
    scanf("%s", newRoute.route_name);
    printf("Enter origin: ");
    scanf("%s", newRoute.origin);
    printf("Enter destination: ");
    scanf("%s", newRoute.destination);
    printf("Enter Arrival time (HH:MM): ");
    scanf("%s", newRoute.arrival_time);
    printf("Enter departure time (HH:MM): ");
    scanf("%s", newRoute.departure_time);
    printf("Enter Ticket price: ");
    scanf("%f", &newRoute.ticket_price);
    FILE *file = fopen("bus_routes.txt", "a");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    fprintf(file, "%s %s %s %s %s %s %.2f %d\n", newRoute.bus_name, newRoute.route_name, newRoute.origin, newRoute.destination, newRoute.arrival_time, newRoute.departure_time, newRoute.ticket_price, 0);
    fclose(file);
    printf("Bus route added successfully!\n");
}

void UpdateBusRoute() {
    clearScreen();
    char busname[MAX_BUS_NAME_LENGTH];
 char busroute[MAX_ROUTE_NAME_LENGTH];
 char originname[MAX_ORIGIN_DEST_LENGTH];
 char destinationname[MAX_ORIGIN_DEST_LENGTH];
 char arrivaltime[MAX_TIME_LENGTH];
 printf("Enter the name of the bus you want to Update: ");
 scanf("%s", busname);
 printf("Enter the name of the bus route you want to Update: ");
 scanf("%s", busroute);
 printf("Enter the name of the bus Origin you want to Update: ");
 scanf("%s", originname);
 printf("Enter the name of the bus destination you want to Update: ");
 scanf("%s", destinationname);
 printf("Enter the bus arrival time you want to Update (HH:MM): ");
 scanf("%s", arrivaltime);
 clearInputBuffer(); 
 clearScreen();
    FILE *file = fopen("bus_routes.txt", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error opening temporary file for writing.\n");
        fclose(file);
        return;
    }
    int found = 0;
    BusRoute currentRoute;
clearScreen();
    while (fscanf(file, "%s %s %s %s %s %s %f %d", currentRoute.bus_name, currentRoute.route_name, currentRoute.origin, currentRoute.destination, currentRoute.arrival_time, currentRoute.departure_time, &currentRoute.ticket_price, &currentRoute.deleted_flag) != EOF) {
        if (strcmp(currentRoute.bus_name, busname) == 0 && strcmp(currentRoute.route_name, busroute) == 0 && strcmp(currentRoute.origin, originname) == 0 && strcmp(currentRoute.destination, destinationname) == 0 && strcmp(currentRoute.arrival_time, arrivaltime) == 0) {
            found = 1;
            printf("===============================================");
            printf("\n             Bus Route Information");
            printf("\n===============================================\n");
            printf("\n\n                       Bus Name: %s\n", currentRoute.bus_name);
            printf(" \n                       Route: %s\n", currentRoute.route_name);
            printf(" \n                       Origin: %s\n", currentRoute.origin);
            printf(" \n                       Destination: %s\n", currentRoute.destination);
            printf(" \n                       Arrival Time: %s\n", currentRoute.arrival_time);
            printf(" \n                       Departure Time: %s\n", currentRoute.departure_time);
            printf(" \n                       Ticket Price: %.2f\n", currentRoute.ticket_price);
            printf("\nEnter new bus name: ");
            scanf("%s", currentRoute.bus_name);
            printf("Enter new route name: ");
            scanf("%s", currentRoute.route_name);
            printf("Enter new origin: ");
            scanf("%s", currentRoute.origin);
            printf("Enter new destination: ");
            scanf("%s", currentRoute.destination);
            printf("Enter new arrival time (HH:MM): ");
            scanf("%s", currentRoute.arrival_time);
            printf("Enter new departure time (HH:MM): ");
            scanf("%s", currentRoute.departure_time);
            printf("Enter new ticket price: ");
            scanf("%f", &currentRoute.ticket_price);
        }
        fprintf(tempFile, "%s %s %s %s %s %s %.2f %d\n", currentRoute.bus_name, currentRoute.route_name, currentRoute.origin, currentRoute.destination, currentRoute.arrival_time, currentRoute.departure_time, currentRoute.ticket_price, currentRoute.deleted_flag);
    }
    fclose(file);
    fclose(tempFile);
    remove("bus_routes.txt");
    rename("temp.txt", "bus_routes.txt");
    if (found) {
        printf("Bus route updated successfully!\n");
    } else {
        printf("No bus route found matching the criteria!\n");
    }
}
void DeleteBusRoute() {
    clearScreen();
    char busname[MAX_BUS_NAME_LENGTH];
    char busroute[MAX_ROUTE_NAME_LENGTH];
    char originname[MAX_ORIGIN_DEST_LENGTH];
    char destinationname[MAX_ORIGIN_DEST_LENGTH];
    char arrivaltime[MAX_TIME_LENGTH];
    printf("Enter the name of the bus you want to delete: ");
    scanf("%s", busname);
    printf("Enter the name of the bus route you want to delete: ");
    scanf("%s", busroute);
    printf("Enter the name of the bus origin you want to delete: ");
    scanf("%s", originname);
    printf("Enter the name of the bus destination you want to delete: ");
    scanf("%s", destinationname);
    printf("Enter the bus arrival time you want to delete (HH:MM): ");
    scanf("%s", arrivaltime);
    clearInputBuffer();
    clearScreen();
    FILE *file = fopen("bus_routes.txt", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error opening temporary file for writing.\n");
        fclose(file);
        return;
    }
    BusRoute currentRoute;
    int found = 0;
    while (fscanf(file, "%s %s %s %s %s %s %f %d", currentRoute.bus_name, currentRoute.route_name, currentRoute.origin, currentRoute.destination, currentRoute.arrival_time, currentRoute.departure_time, &currentRoute.ticket_price, &currentRoute.deleted_flag) != EOF) {
        if (strcmp(currentRoute.bus_name, busname) == 0 && strcmp(currentRoute.route_name, busroute) == 0 && strcmp(currentRoute.origin, originname) == 0 && strcmp(currentRoute.destination, destinationname) == 0 && strcmp(currentRoute.arrival_time, arrivaltime) == 0) {
            found = 1;
            currentRoute.deleted_flag = 1; 
        }
       
        fprintf(tempFile, "%s %s %s %s %s %s %.2f %d\n", currentRoute.bus_name, currentRoute.route_name, currentRoute.origin, currentRoute.destination, currentRoute.arrival_time, currentRoute.departure_time, currentRoute.ticket_price, currentRoute.deleted_flag);
    }
    fclose(file);
    fclose(tempFile);

    if (!found) {
        printf("Route not found!\n");
    } else {
        remove("bus_routes.txt");
        rename("temp.txt", "bus_routes.txt");
        printf("Route marked as deleted successfully!\n");
    }
}
void bookingmanagement(User *traveler) {
    int choice;
    do {
        printf("\nBooking Management Menu:");
        printf("\n1. View Bookings");
        printf("\n2. Cancel Booking");
        printf("\n3. Exit");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                view_booking_details(traveler);
                break;
            case 2: {
                char busname[MAX_BUS_NAME_LENGTH];
                char routename[MAX_ROUTE_NAME_LENGTH];
                char origin[MAX_ORIGIN_DEST_LENGTH];
                char destination[MAX_ORIGIN_DEST_LENGTH];
                char arrivaltime[MAX_TIME_LENGTH];
                printf("\nEnter the booking details to cancel the reservation:\n");
                printf("Bus Name: ");
                scanf("%s", busname);
                printf("Route Name: ");
                scanf("%s", routename);
                printf("Origin: ");
                scanf("%s", origin);
                printf("Destination: ");
                scanf("%s", destination);
                printf("Arrival Time (HH:MM): ");
                scanf("%s", arrivaltime);
                cancel_reservation(traveler, busname, routename, origin, destination, arrivaltime);
                break;
            }
            case 3:
                printf("Exiting Booking Management Menu...\n");
                break;
            default:
                printf("Invalid choice! Please enter a valid option.\n");
        }
    } while (choice != 3);
}