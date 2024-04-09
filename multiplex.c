#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_NAME_LENGTH 50
#define MAX_SEATS 30
#define MOVIE_FILENAME "movies.txt"
#define ADMIN_FILENAME "admin.txt"
#define FILENAME "booking_list.txt"
#define TODAY_MOVIES_FILENAME "movies_today.txt"
#define LAST_SELECTED_DATE_FILENAME "last_selected_date.txt"
#define MAX_MOVIES 30

int MAX_SELECTED_MOVIES=3;

typedef enum { FRONT_CIRCLE, REAR_CIRCLE } Category;

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    int age;
    Category category;
    char movie[MAX_NAME_LENGTH];
} Ticket;

typedef struct {
    char title[MAX_NAME_LENGTH];
    char director[MAX_NAME_LENGTH];
    int year;
} MovieInfo;

typedef struct Node {
    Ticket ticket;
    struct Node* next;
} Node;

Node* head = NULL;

// Function prototypes
void clearInputBuffer();
void saveToFile(const char *filename);
void loadFromFile(const char *filename);
void bookTicket(const char selectedMovies[][MAX_NAME_LENGTH], int numSelectedMovies);
void inquireSeat();
int countTickets();
void loadMoviesFromFile(const char *filename, MovieInfo movies[], int *numMovies);
void adminLogin(char username[], char password[]);
void selectMoviesForToday(MovieInfo movies[], int numMovies, char selectedMovies[][MAX_NAME_LENGTH], int numSelectedMovies);
void displaySelectedMovies(const char selectedMovies[][MAX_NAME_LENGTH], int numSelectedMovies);
void movieBookingSystem();
void adminPanel(MovieInfo movies[], int *numMovies, char selectedMovies[][MAX_NAME_LENGTH], bool *moviesSelected);
void addMovieFromCatalog(MovieInfo movies[], int numMovies, char selectedMovies[][MAX_NAME_LENGTH]);
void addNewMovie(MovieInfo movies[], int *numMovies, char selectedMovies[][MAX_NAME_LENGTH]);
void deleteMovie(MovieInfo movies[], int *numMovies, char selectedMovies[][MAX_NAME_LENGTH], bool *moviesSelected);
void changeMovieSelection(MovieInfo movies[], int numMovies, char selectedMovies[][MAX_NAME_LENGTH], bool *moviesSelected);
void saveLastSelectedDate(time_t date);
void loadLastSelectedDate(time_t *date);
bool isSameDate(time_t date1, time_t date2);
void updateMoviesForToday(const char *movieFilename, const char *todayMovieFilename);
void checkAndUpdateMoviesForToday();
void saveTicketToFile( const Ticket *ticket);

int main() {
    // Check and update movies for today
    checkAndUpdateMoviesForToday();
   int c;
   c = countTickets();
   printf("%d",c);
    // Start movie booking system
    movieBookingSystem();
    

    return 0;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}



void saveToFile(const char *filename) {
    FILE* file = fopen(filename, "a+");
    if (file == NULL) {
        printf("Error opening file %s for writing!\n", filename);
        return;
    }

    Node* current = head;
    while (current != NULL) {
        fprintf(file, "%d,%s,%d,%d,%s\n", current->ticket.id, current->ticket.name, current->ticket.age, current->ticket.category, current->ticket.movie);
        current = current->next;
    }

    fclose(file);
}

void loadFromFile(const char *filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s for reading!\n", filename);
        return;
    }

    Ticket ticket;
    while (fscanf(file, "%d,%[^,],%d,%d,%[^\n]", &ticket.id, ticket.name, &ticket.age, &ticket.category, ticket.movie) == 5) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        if (newNode == NULL) {
            printf("Memory allocation failed!\n");
            fclose(file);
            exit(EXIT_FAILURE);
        }
        newNode->ticket = ticket;
        newNode->next = head;
        head = newNode;
    }

    fclose(file);
}
void bookTicket(const char selectedMovies[][MAX_NAME_LENGTH], int numSelectedMovies) 
{
    int totalTickets = countTickets();
    if (totalTickets >= MAX_SEATS) 
    {
        printf("Booking is full!\n");
        return;
    }
    else if (totalTickets >= MAX_SEATS / 2)
    {
        printf("Senior citizen preference!\n");
        int age;
        printf("Enter your age: ");
        if (scanf("%d", &age) != 1 || age < 60) 
        {
            printf("Sorry, only senior citizens are allowed to book at this time.\n");
            clearInputBuffer();
            return;
        }
        clearInputBuffer();

        Category category = FRONT_CIRCLE; // Senior citizen, assign to front circle
        int index;
        printf("Enter the index of the movie you want to watch: ");
        if (scanf("%d", &index) != 1 || index < 1 || index > numSelectedMovies)
        {
            printf("Invalid input! Please enter a valid index.\n");
            clearInputBuffer();
            return;
        }
        clearInputBuffer();

        int id;
        printf("Enter an ID:\n");
        if (scanf("%d", &id) != 1) {
            printf("Invalid ID! Please enter a valid integer.\n");
            clearInputBuffer();
            return;
        }
        clearInputBuffer();

        char name[MAX_NAME_LENGTH];
        printf("Enter your name: ");
        fgets(name, MAX_NAME_LENGTH, stdin);
        name[strcspn(name, "\n")] = '\0'; // Remove newline character

        Ticket ticket;
        ticket.id = id; // Generate a random ID
        strcpy(ticket.name, name);
        ticket.age = age;
        ticket.category = category;
        strcpy(ticket.movie, selectedMovies[index - 1]);

        saveTicketToFile(&ticket);

        printf("Ticket booked successfully!\n");
    }
    else
    {
        int id;
        printf("Enter an ID:\n");
        if (scanf("%d", &id) != 1) {
            printf("Invalid ID! Please enter a valid integer.\n");
            clearInputBuffer();
            return;
        }
        clearInputBuffer();

        char name[MAX_NAME_LENGTH];
        printf("Enter your name: ");
        fgets(name, MAX_NAME_LENGTH, stdin);
        name[strcspn(name, "\n")] = '\0'; // Remove newline character

        int age;
        printf("Enter your age: ");
        if (scanf("%d", &age) != 1 || age <= 0) {
            printf("Invalid age! Please enter a positive integer.\n");
            clearInputBuffer();
            return;
        }
        clearInputBuffer();

        Category category;
        printf("Enter your category (0 for Front Circle, 1 for Rear Circle): ");
        int cat;
        if (scanf("%d", &cat) != 1 || (cat != 0 && cat != 1)) {
            printf("Invalid category! Please enter 0 or 1.\n");
            clearInputBuffer();
            return;
        }
        clearInputBuffer();
        category = (cat == 0) ? FRONT_CIRCLE : REAR_CIRCLE;

        int index;
        printf("Enter the index of the movie you want to watch: ");
        if (scanf("%d", &index) != 1 || index < 1 || index > numSelectedMovies) {
            printf("Invalid input! Please enter a valid index.\n");
            clearInputBuffer();
            return;
        }
        clearInputBuffer();

        Ticket ticket;
        ticket.id = id; // Generate a random ID
        strcpy(ticket.name, name);
        ticket.age = age;
        ticket.category = category;
        strcpy(ticket.movie, selectedMovies[index - 1]);

        saveTicketToFile(&ticket);

        printf("Ticket booked successfully!\n");
    }
}



void saveTicketToFile(const Ticket *ticket) {
    FILE *file = fopen(FILENAME, "a+");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    fprintf(file, "%d,%s,%d,%d,%s\n", ticket->id, ticket->name, ticket->age, ticket->category, ticket->movie);
    fclose(file);
}


void inquireSeat() {
    FILE* fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("Enter your name: ");
    char name[MAX_NAME_LENGTH];
    fgets(name, MAX_NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = '\0'; // Remove newline character

    printf("Enter your ID: ");
    int id;
    scanf("%d", &id);
    clearInputBuffer(); // Clear input buffer

    Ticket ticket;
    bool found = false;
    while (fscanf(fp, "%d,%49[^,],%d,%d,%99[^\n]", &ticket.id, ticket.name, &ticket.age, &ticket.category, ticket.movie) == 5) {
        if (strcmp(ticket.name, name) == 0 && ticket.id == id) {
            found = true;
            printf("Ticket found! Details:\n");
            printf("Name: %s\n", ticket.name);
            printf("ID: %d\n", ticket.id);
            printf("Age: %d\n", ticket.age);
            printf("Category: %s\n", (ticket.category == FRONT_CIRCLE) ? "Front Circle" : "Rear Circle");
            printf("Movie: %s\n", ticket.movie);
            break;
        }
    }

    if (!found) {
        printf("Ticket not found!\n");
    }

    fclose(fp);
}   




int countTickets() {
   FILE *fp = fopen(FILENAME, "r");
   if (fp == NULL) {
       printf("Error opening file!\n");
       return 0;
   }

   int count = 0;
   char line[200];
   while (fgets(line, sizeof(line), fp) != NULL) {
       count++;
   }
   fclose(fp);
   return count;

}

void loadMoviesFromFile(const char *filename, MovieInfo movies[], int *numMovies) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s for reading!\n", filename);
        return;
    }

    int count = 0;
    char line[MAX_NAME_LENGTH * 3 + 10]; // Maximum line length assuming MAX_NAME_LENGTH for each field and ',' separators
    while (fgets(line, sizeof(line), file) != NULL && count < MAX_MOVIES) {
        char title[MAX_NAME_LENGTH], director[MAX_NAME_LENGTH];
        int year;
        if (sscanf(line, "%[^,],%[^,],%d", title, director, &year) == 3) {
            strcpy(movies[count].title, title);
            strcpy(movies[count].director, director);
            movies[count].year = year;
            count++;
        } else {
            printf("Error parsing line: %s\n", line);
        }
    }
    *numMovies = count;

    fclose(file);
}

void adminLogin(char username[], char password[]) {
    FILE* file = fopen(ADMIN_FILENAME, "r");
    if (file == NULL) {
        printf("Error opening admin file for reading!\n");
        return;
    }

    fscanf(file, "%s %s", username, password);
    fclose(file);
}

void selectMoviesForToday(MovieInfo movies[], int numMovies, char selectedMovies[][MAX_NAME_LENGTH], int numSelectedMovies) 
{
    // Check if the last selected date matches today's date
    time_t lastSelectedDate, currentDate;
    loadLastSelectedDate(&lastSelectedDate);
    time(&currentDate);

    if (!isSameDate(lastSelectedDate, currentDate)) 
    {
        // If dates don't match, select new movies randomly
        srand(time(NULL));
        int i, j;
        MovieInfo tempMovies[MAX_MOVIES]; // Temporary array to store movies
        for (i = 0; i < numMovies; i++) {
            strcpy(tempMovies[i].title, movies[i].title);
            strcpy(tempMovies[i].director, movies[i].director);
            tempMovies[i].year = movies[i].year;
        }

        for (i = 0; i < numSelectedMovies; i++) {
            int index = rand() % numMovies;
            strcpy(selectedMovies[i], tempMovies[index].title);
            // Remove the selected movie from the list to prevent duplicates
            for (j = index; j < numMovies - 1; j++) {
                strcpy(tempMovies[j].title, tempMovies[j + 1].title);
                strcpy(tempMovies[j].director, tempMovies[j + 1].director);
                tempMovies[j].year = tempMovies[j + 1].year;
            }
            numMovies--;
        }

        // Save the current date as the last selected date
        saveLastSelectedDate(currentDate);

        // Save selected movies to movies_today.txt
        FILE *file = fopen(TODAY_MOVIES_FILENAME, "w");
        if (file == NULL) {
            printf("Error opening file %s for writing!\n", TODAY_MOVIES_FILENAME);
            return;
        }
        for (int i = 0; i < numSelectedMovies; i++) {
            fprintf(file, "%s\n", selectedMovies[i]);
        }
        fclose(file);

        // Debug output
        printf("Selected movies for today:\n");
        for (int i = 0; i < numSelectedMovies; i++) {
            printf("%d. %s\n", i + 1, selectedMovies[i]);
        }
    } else {
        // If dates match, load selected movies from movies_today.txt
        FILE *file = fopen(TODAY_MOVIES_FILENAME, "r");
        if (file == NULL) {
            // Create the file if it doesn't exist
            file = fopen(TODAY_MOVIES_FILENAME, "w");
            if (file == NULL) {
                printf("Error creating file %s!\n", TODAY_MOVIES_FILENAME);
                return;
            }
            fclose(file);
            return; // No movies selected yet, return
        }

        for (int i = 0; i < numSelectedMovies; i++) {
            fgets(selectedMovies[i], MAX_NAME_LENGTH, file);
            selectedMovies[i][strcspn(selectedMovies[i], "\n")] = 0; // Remove newline character
        }

        fclose(file);

        // Debug output
        printf("Selected movies for today:\n");
        for (int i = 0; i < numSelectedMovies; i++) {
            printf("%d. %s\n", i + 1, selectedMovies[i]);
        }
    }
}


void displaySelectedMovies(const char selectedMovies[][MAX_NAME_LENGTH], int numSelectedMovies) {
    printf("Movies screened for today:\n");
    for (int i = 0; i < numSelectedMovies; i++) {
        printf("%d. %s\n", i + 1, selectedMovies[i]);
    }
}

void movieBookingSystem() {
    char adminUsername[MAX_NAME_LENGTH];
    char adminPassword[MAX_NAME_LENGTH];
    adminLogin(adminUsername, adminPassword);

    char enteredUsername[MAX_NAME_LENGTH];
    char enteredPassword[MAX_NAME_LENGTH];

    bool moviesSelected = false;
    char selectedMovies[MAX_SELECTED_MOVIES][MAX_NAME_LENGTH];

    // Load movies
    MovieInfo movies[MAX_MOVIES];
    int numMovies = 0;
    loadMoviesFromFile(MOVIE_FILENAME, movies, &numMovies);

    // Select movies for today if not already selected
    if (!moviesSelected) {
        selectMoviesForToday(movies, numMovies, selectedMovies, MAX_SELECTED_MOVIES);
        moviesSelected = true;
    }

    while (1) {
        printf("\nWelcome to Movie Ticket Booking System\n");
        printf("Movies for today:\n");
        displaySelectedMovies(selectedMovies, MAX_SELECTED_MOVIES); // Display movies for today
        printf("1. Book a ticket\n");
        printf("2. Inquire about seat\n");
        printf("3. Admin Login\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                if (moviesSelected)
                    bookTicket(selectedMovies, MAX_SELECTED_MOVIES);
                else
                    printf("Movies for today are not selected yet. Please contact admin.\n");
                break;
            case 2:
                inquireSeat();
                break;
            case 3:
                printf("Enter Admin Username: ");
                fgets(enteredUsername, MAX_NAME_LENGTH, stdin);
                enteredUsername[strcspn(enteredUsername, "\n")] = 0; // Remove newline character

                printf("Enter Admin Password: ");
                fgets(enteredPassword, MAX_NAME_LENGTH, stdin);
                enteredPassword[strcspn(enteredPassword, "\n")] = 0; // Remove newline character

                if (strcmp(enteredUsername, adminUsername) == 0 && strcmp(enteredPassword, adminPassword) == 0) {
                    adminPanel(movies, &numMovies, selectedMovies, &moviesSelected);
                } else {
                    printf("Invalid credentials! Access denied.\n");
                }
                break;
            case 4:
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
}

void adminPanel(MovieInfo movies[], int *numMovies, char selectedMovies[][MAX_NAME_LENGTH], bool *moviesSelected) {
    while (1) {
        printf("\nAdmin Panel\n");
        printf("1. Add a movie from catalog\n");
        printf("2. Add a new movie\n");
        printf("3. Delete a movie\n");
        printf("4. Change movie selection for today\n");
        printf("5. Back to main menu\n");
        printf("Enter your choice: ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                addMovieFromCatalog(movies, *numMovies, selectedMovies);
                break;
            case 2:
                addNewMovie(movies, numMovies, selectedMovies);
                break;
            case 3:
                deleteMovie(movies, numMovies, selectedMovies, moviesSelected);
                break;
            case 4:
                changeMovieSelection(movies, *numMovies, selectedMovies, moviesSelected);
                break;
            case 5:
                return;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
}

void addMovieFromCatalog(MovieInfo movies[], int numMovies, char selectedMovies[][MAX_NAME_LENGTH]) {
    printf("Catalog Movies:\n");
    for (int i = 0; i < numMovies; i++) {
        printf("%d. %s\n", i + 1, movies[i].title);
    }

    printf("Enter the index of the movie to add: ");
    int index;
    if (scanf("%d", &index) != 1 || index < 1 || index > numMovies) {
        printf("Invalid input! Please enter a valid index.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (index < 1 || index > numMovies) {
        printf("Invalid index! Please try again.\n");
        return;
    }

    // Check if the movie is already selected for today
    for (int i = 0; i < MAX_SELECTED_MOVIES; i++) {
        if (strcmp(selectedMovies[i], movies[index - 1].title) == 0) {
            printf("Movie is already selected for today!\n");
            return;
        }
    }

    // Add the selected movie to today's movies
    for (int i = 0; i < MAX_SELECTED_MOVIES; i++) {
        if (selectedMovies[i][0] == '\0') {
            strcpy(selectedMovies[i], movies[index - 1].title);
            printf("Movie added successfully!\n");
            return;
        }
    }

    printf("Maximum movies selected for today!\n");
}

void addNewMovie(MovieInfo movies[], int *numMovies, char selectedMovies[][MAX_NAME_LENGTH]) {
    if (*numMovies >= MAX_MOVIES) {
        printf("Maximum number of movies reached!\n");
        return;
    }

    printf("Enter movie title: ");
    fgets(movies[*numMovies].title, MAX_NAME_LENGTH, stdin);
    movies[*numMovies].title[strcspn(movies[*numMovies].title, "\n")] = 0; // Remove newline character

    printf("Enter movie director: ");
    fgets(movies[*numMovies].director, MAX_NAME_LENGTH, stdin);
    movies[*numMovies].director[strcspn(movies[*numMovies].director, "\n")] = 0; // Remove newline character

    printf("Enter release year: ");
    if (scanf("%d", &movies[*numMovies].year) != 1) {
        printf("Invalid input! Please enter a valid year.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    // Add the movie to today's movies if not already selected
    for (int i = 0; i < MAX_SELECTED_MOVIES; i++) {
        if (selectedMovies[i][0] == '\0') {
            strcpy(selectedMovies[i], movies[*numMovies].title);
            break;
        }
    }

    (*numMovies)++;
    printf("New movie added successfully!\n");
}

void deleteMovie(MovieInfo movies[], int *numMovies, char selectedMovies[][MAX_NAME_LENGTH], bool *moviesSelected) {
    if (*numMovies == 0) {
        printf("No movies available to delete!\n");
        return;
    }

    printf("Movies:\n");
    for (int i = 0; i < *numMovies; i++) {
        printf("%d. %s\n", i + 1, movies[i].title);
    }

    printf("Enter the index of the movie to delete: ");
    int index;
    if (scanf("%d", &index) != 1 || index < 1 || index > *numMovies) {
        printf("Invalid input! Please enter a valid index.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    // Delete the movie from today's movies if selected
    for (int i = 0; i < MAX_SELECTED_MOVIES; i++) {
        if (strcmp(selectedMovies[i], movies[index - 1].title) == 0) {
            selectedMovies[i][0] = '\0';
            break;
        }
    }

    // Shift remaining movies in the array to cover the deleted movie
    for (int i = index - 1; i < *numMovies - 1; i++) {
        strcpy(movies[i].title, movies[i + 1].title);
        strcpy(movies[i].director, movies[i + 1].director);
        movies[i].year = movies[i + 1].year;
    }

    (*numMovies)--;
    printf("Movie deleted successfully!\n");

    // Update movies for today if needed
    if (*moviesSelected) {
        selectMoviesForToday(movies, *numMovies, selectedMovies, MAX_SELECTED_MOVIES);
    }
}

void changeMovieSelection(MovieInfo movies[], int numMovies, char selectedMovies[][MAX_NAME_LENGTH], bool *moviesSelected) {
    if (numMovies == 0) {
        printf("No movies available to select!\n");
        return;
    }

    // Select new movies for today
    selectMoviesForToday(movies, numMovies, selectedMovies, MAX_SELECTED_MOVIES);
    *moviesSelected = true;
}

void saveLastSelectedDate(time_t date) {
    FILE *file = fopen(LAST_SELECTED_DATE_FILENAME, "w");
    if (file == NULL) {
        printf("Error opening file %s for writing!\n", LAST_SELECTED_DATE_FILENAME);
        return;
    }
    fprintf(file, "%ld", date);
    fclose(file);
}

void loadLastSelectedDate(time_t *date) {
    FILE *file = fopen(LAST_SELECTED_DATE_FILENAME, "r");
    if (file == NULL) {
        // Create the file if it doesn't exist
        file = fopen(LAST_SELECTED_DATE_FILENAME, "w");
        if (file == NULL) {
            printf("Error creating file %s!\n", LAST_SELECTED_DATE_FILENAME);
            return;
        }
        fclose(file);
        *date = -1;
        return;
    }
    fscanf(file, "%ld", date);
    fclose(file);
}

bool isSameDate(time_t date1, time_t date2) {
    struct tm* tm1 = localtime(&date1);
    struct tm* tm2 = localtime(&date2);

    return tm1->tm_mday == tm2->tm_mday &&
           tm1->tm_mon == tm2->tm_mon &&
           tm1->tm_year == tm2->tm_year;
}

void updateMoviesForToday(const char *movieFilename, const char *todayMovieFilename) {
    // Load movies
    MovieInfo movies[MAX_MOVIES];
    int numMovies = 0;
    loadMoviesFromFile(movieFilename, movies, &numMovies);

    // Select movies for today
    char selectedMovies[MAX_SELECTED_MOVIES][MAX_NAME_LENGTH];
    selectMoviesForToday(movies, numMovies, selectedMovies, MAX_SELECTED_MOVIES);
}

void checkAndUpdateMoviesForToday() {
    time_t lastSelectedDate, currentDate;
    loadLastSelectedDate(&lastSelectedDate);
    time(&currentDate);

    if (!isSameDate(lastSelectedDate, currentDate)) {
        // Update movies for today if the last selected date is not today
        updateMoviesForToday(MOVIE_FILENAME, TODAY_MOVIES_FILENAME);
    }
}
