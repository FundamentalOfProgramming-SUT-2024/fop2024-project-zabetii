#ifndef MENU_H
#define MENU_H

#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LENGTH 20 // pass
#define MIN_LENGTH 8  // pass
#define MAX_USER 1000
#define MAX_SCORES 100
#define COLMAXI 100
typedef struct user
{
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    char email[MAX_LENGTH];
} user;
typedef struct score
{
    char username[MAX_LENGTH];
    int score;
    int gold;
    int played_games;
    int experience;
} score;
void start_new_game();
int is_valid_username(const char *username);
int is_valid_password(const char *password);
int is_valid_email(const char *email);
void main_menu();
void new_user_menu();
void create_user();
void login_menu();
void login_user();
void draw_menu_border();
void generate_random_password(char *password, int *length);
void pregame_menu();
void settings_menu();
void difficulty_level();
void characters_color();
void display_scoreboards(const char *curr_user);
void scores_sort();
void update_scoreboard(const char *username, int score, int gold, int played_games, int experience);
void display_message();
void new_message(const char *message);

extern int game_difficulty;
extern int color_of_character;
extern int scorecounter;
extern int usercounter;
extern user users[MAX_USER];
extern score scores[MAX_USER];
extern char current_message[COLMAXI];

int is_valid_username(const char *username)
{
    for (int i = 0; i < usercounter; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            return 0;
        }
    }
    return 1;
}
int is_valid_password(const char *password)
{
    int passlength = strlen(password);
    int has_upper = 0, has_lower = 0, has_digit = 0;
    if (passlength < 7)
    {
        return 0;
    }
    for (int i = 0; i < passlength; i++)
    {
        if (password[i] >= 'a' && password[i] <= 'z')
        {
            has_lower = 1;
        }
        if (password[i] >= 'A' && password[i] <= 'Z')
        {
            has_upper = 1;
        }
        if (password[i] >= '0' && password[i] <= '9')
        {
            has_digit = 1;
        }
    }
    int pass_result = has_lower && has_upper && has_digit;
    return pass_result;
}
int is_valid_email(const char *email)
{
    int email_length = strlen(email);
    int at_index = -1;
    int dot_index = -1;
    for (int i = 0; i < email_length; i++)
    {
        if (email[i] == '@')
        {
            at_index = i;
        }
        else if (email[i] == '.' && at_index != -1)
        {
            dot_index = i;
        }
    }
    if (at_index > 0 && dot_index > at_index + 1 && dot_index < email_length - 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void main_menu()
{
    curs_set(0);
    const char *choices[] = {"Create New User", "User Login", "Exit"};
    int num_choices = sizeof(choices) / sizeof(choices[0]);
    int highlight = 0;
    int choice;
    while (1)
    {
        clear();
        draw_menu_border();
        mvprintw(1, COLS / 2 - 4, "Main Menu");
        for (int i = 0; i < num_choices; i++)
        {
            if (i == highlight)
            {
                attron(A_REVERSE);
                mvprintw(10 + i, COLS / 2 - strlen(choices[i]) / 2, "%s", choices[i]);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(10 + i, COLS / 2 - strlen(choices[i]) / 2, "%s", choices[i]);
            }
        }
        choice = getch();
        switch (choice)
        {
        case KEY_UP:
            highlight = (highlight - 1 + num_choices) % num_choices;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % num_choices;
            break;
        case 10:
            if (highlight == 0)
            {
                new_user_menu();
            }
            else if (highlight == 1)
            {
                login_menu();
            }
            else if (highlight == 2)
            {
                return;
            }
            break;
        default:
            break;
        }
    }
}
void new_user_menu()
{
    clear();
    create_user();
    printw("Press any key to return to the main menu.");
    getch();
}
void create_user()
{
    echo();

    user new_user;
    char password_choice;
    printw("Enter your username: ");
    scanw("%s", new_user.username);
    if (!is_valid_username(new_user.username))
    {
        printw("Username is already taken. Try a different one.\n");
        noecho();
        return;
    }
    printw("Generate a random password? (y/n): ");
    scanw(" %c", &password_choice);
    if (password_choice == 'y' || password_choice == 'Y')
    {
        int length;
        generate_random_password(new_user.password, &length);
        printw("Generated Password: %s\n", new_user.password);
    }
    else
    {
        printw("Enter your password: ");
        scanw("%s", new_user.password);
        if (!is_valid_password(new_user.password))
        {
            printw("Password must be at least 7 characters long and should contain an uppercase letter, a lowercase letter, and a digit.\n");
            noecho(); // Disable input echoing
            return;
        }
    }
    printw("Enter your email: ");
    scanw("%s", new_user.email);
    if (!is_valid_email(new_user.email))
    {
        printw("The email format is invalid.\n");
        noecho(); // Disable input echoing
        return;
    }
    users[usercounter] = new_user;
    usercounter++;
    printw("User created successfully.\n");
    noecho(); // Disable input echoing
}
void login_menu()
{
    curs_set(0);
    clear();
    const char *choices[] = {"Login", "Login as Guest", "Back to Main Menu"};
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    int highlight = 0;
    int choice;
    while (1)
    {
        clear();
        refresh();
        draw_menu_border();
        mvprintw(1, COLS / 2 - 5, "Login Menu");
        for (int i = 0; i < n_choices; i++)
        {
            if (i == highlight)
            {
                attron(A_REVERSE);
                mvprintw(10 + i, COLS / 2 - strlen(choices[i]) / 2, "%s", choices[i]);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(10 + i, COLS / 2 - strlen(choices[i]) / 2, "%s", choices[i]);
            }
        }
        choice = getch();
        switch (choice)
        {
        case KEY_UP:
            highlight = (highlight - 1 + n_choices) % n_choices;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % n_choices;
            break;
        case 10:
            if (highlight == 0)
            {
                login_user();
            }
            else if (highlight == 1)
            {
                start_new_game();
                return;
            }
            else if (highlight == 2)
            {
                return;
            }
            break;
        default:
            break;
        }
    }
}
void login_user()
{
    echo();
    clear();
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    printw("Enter your username: ");
    scanw("%s", username);
    printw("Enter your password: ");
    scanw("%s", password);
    noecho();
    /*if (strcmp(password, "forgot") == 0)
    {
        forgot_password(username);
        return;
    }*/
    for (int i = 0; i < usercounter; i++)
    {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0)
        {
            printw("Login successful!\n");
            refresh();
            pregame_menu();
            return;
        }
    }
    printw("Invalid username or password. Try again.\n");
    getch();
}
void draw_menu_border()
{
    for (int x = 0; x < COLS; x++)
    {
        mvprintw(0, x, "-");
        mvprintw(LINES - 1, x, "-");
    }
    for (int y = 0; y < LINES; y++)
    {
        mvprintw(y, 0, "|");
        mvprintw(y, COLS - 1, "|");
    }
}
void generate_random_password(char *password, int *length)
{
    *length = MIN_LENGTH + rand() % (MAX_LENGTH - MIN_LENGTH + 1);
    const char lower[] = "abcdefghijklmnopqrstuvwxyz";
    const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char digits[] = "1234567890";
    const char all[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    password[0] = lower[rand() % (sizeof(lower) - 1)];
    password[1] = upper[rand() % (sizeof(upper) - 1)];
    password[2] = digits[rand() % (sizeof(digits) - 1)];
    for (int i = 3; i < *length; i++)
    {
        int key = rand() % (sizeof(all) - 1);
        password[i] = all[key];
    }
    for (int i = 0; i < *length; i++)
    {
        int j = rand() % *length;
        char temp = password[i];
        password[i] = password[j];
        password[j] = temp;
    }
    password[*length] = '\0';
}
void pregame_menu()
{
    curs_set(0);
    const char *choices[] = {"Resume Game", "New Game", "Settings", "Scoreboard", "Back to Main Menu"};
    int num_choices = sizeof(choices) / sizeof(choices[0]);
    int highlight = 0;
    int choice;
    while (1)
    {
        clear();
        draw_menu_border();
        mvprintw(1, COLS / 2 - 6, "PreGame Menu");
        for (int i = 0; i < num_choices; i++)
        {
            if (i == highlight)
            {
                attron(A_REVERSE);
                mvprintw(10 + i, COLS / 2 - strlen(choices[i]) / 2, "%s", choices[i]);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(10 + i, COLS / 2 - strlen(choices[i]) / 2, "%s", choices[i]);
            }
        }
        choice = getch();
        switch (choice)
        {
        case KEY_UP:
            highlight = (highlight - 1 + num_choices) % num_choices;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % num_choices;
            break;
        case 10:
            if (highlight == 0)
            {
                // Resume Game
            }
            else if (highlight == 1)
            {
                start_new_game();
                return;
            }
            else if (highlight == 2)
            {
                settings_menu();
            }
            else if (highlight == 3)
            {
                scores_sort();
                display_scoreboards("curr_user");
            }
            else if (highlight == 4)
            {
                main_menu();
                return;
            }
            break;
        default:
            break;
        }
    }
}
void settings_menu()
{
    const char *choices[] = {"Difficulty level", "Character Color", "Background Music", "Back"};
    int num_choices = sizeof(choices) / sizeof(choices[0]);
    int highlight = 0;
    int choice;
    while (1)
    {
        clear();
        draw_menu_border();
        mvprintw(1, COLS / 2 - 6, "Settings Menu");
        for (int i = 0; i < num_choices; i++)
        {
            if (i == highlight)
            {
                attron(A_REVERSE);
                mvprintw(10 + i, COLS / 2 - strlen(choices[i]) / 2, "%s", choices[i]);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(10 + i, COLS / 2 - strlen(choices[i]) / 2, "%s", choices[i]);
            }
        }
        choice = getch();
        switch (choice)
        {
        case KEY_UP:
            highlight = (highlight - 1 + num_choices) % num_choices;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % num_choices;
            break;
        case 10:
            if (highlight == 0)
            {
                difficulty_level();
            }
            else if (highlight == 1)
            {
                characters_color();
            }
            else if (highlight == 2)
            {
                // background_music();
            }
            else if (highlight == 3)
            {
                return;
            }
            break;
        default:
            break;
        }
    }
}
void difficulty_level()
{
    const char *choices[] = {"Easy", "Medium", "Hard"};
    int num_choices = sizeof(choices) / sizeof(choices[0]);
    int highlight = game_difficulty;
    int choice;
    while (1)
    {
        clear();
        draw_menu_border();
        mvprintw(1, COLS / 2 - 7, "Difficulty Level");
        for (int i = 0; i < num_choices; i++)
        {
            if (i == highlight)
            {
                attron(A_REVERSE);
                mvprintw(10 + i, COLS / 2 - strlen(choices[i]) / 2, "%s", choices[i]);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(10 + i, COLS / 2 - strlen(choices[i]) / 2, "%s", choices[i]);
            }
        }
        choice = getch();
        switch (choice)
        {
        case KEY_UP:
            highlight = (highlight - 1 + num_choices) % num_choices;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % num_choices;
            break;
        case 10:
            game_difficulty = highlight;
            return;
        default:
            break;
        }
    }
}
int character_color;
void characters_color()
{
    const char *colors[] = {"Yellow", "Green", "Cyan", "Magenta"};
    int color_pairs[4] = {2, 5, 6, 7};
    int num_colors = sizeof(colors) / sizeof(colors[0]);
    int highlight = color_of_character;
    int choice;
    while (1)
    {
        clear();
        draw_menu_border();
        mvprintw(1, COLS / 2 - 7, "Character Color");
        for (int i = 0; i < num_colors; i++)
        {
            if (i == highlight)
            {
                attron(A_REVERSE);
                mvprintw(10 + i, COLS / 2 - strlen(colors[i]) / 2, "%s", colors[i]);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(10 + i, COLS / 2 - strlen(colors[i]) / 2, "%s", colors[i]);
            }
        }
        choice = getch();
        switch (choice)
        {
        case KEY_UP:
            highlight = (highlight - 1 + num_colors) % num_colors;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % num_colors;
            break;
        case 10:
            color_of_character = color_pairs[highlight];
            return;
        default:
            break;
        }
    }
}
void display_scoreboards(const char *curr_user)
{
    clear();
    draw_menu_border();
    mvprintw(1, COLS / 2 - 5, "ScoreBoard");
    mvprintw(3, 2, "Rank");
    mvprintw(3, 10, "Username");
    mvprintw(3, 25, "Score");
    mvprintw(3, 35, "Gold");
    mvprintw(3, 45, "Games Played");
    mvprintw(3, 60, "Experience");
    for (int i = 0; i < scorecounter; i++)
    {
        if (strcmp(scores[i].username, curr_user) == 0)
        {
            attron(A_BOLD);
        }
        mvprintw(5 + i, 2, "%d", i + 1);
        mvprintw(5 + i, 10, "%s", scores[i].username);
        mvprintw(5 + i, 25, "%d", scores[i].score);
        mvprintw(5 + i, 35, "%d", scores[i].gold);
        mvprintw(5 + i, 45, "%d", scores[i].played_games);
        mvprintw(5 + i, 60, "%d", scores[i].experience);
        if (strcmp(scores[i].username, curr_user) == 0)
        {
            attroff(A_BOLD);
        }
    }
    getch();
}
void scores_sort()
{
    for (int i = 0; i < scorecounter - 1; i++)
    {
        for (int j = i + 1; j < scorecounter; j++)
        {
            if (scores[i].score < scores[j].score)
            {
                score temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }
}
void update_scoreboard(const char *username, int score, int gold, int played_games, int experience)
{
    for (int i = 0; i < scorecounter; i++)
    {
        if (strcmp(scores[i].username, username) == 0)
        {
            scores[i].score = score;
            scores[i].gold = gold;
            scores[i].played_games = played_games;
            scores[i].experience = experience;
            return;
        }
    }
    if (scorecounter != MAX_SCORES)
    {
        strcpy(scores[scorecounter].username, username);
        scores[scorecounter].score = score;
        scores[scorecounter].gold = gold;
        scores[scorecounter].played_games = played_games;
        scores[scorecounter].experience = experience;
        scorecounter++;
    }
}
void display_message()
{
    mvprintw(0, 0, "%s", current_message);
}
void new_message(const char *message)
{
    strcpy(current_message, message);
    display_message();
}
#endif