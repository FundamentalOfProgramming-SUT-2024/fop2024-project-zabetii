#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include "menu.h"
#define MIN_ROOMS 6
#define MAX_ROOMS 9
#define COLOR_GREY 10
#define COLOR_ORANGE 11
#define COLOR_GOLD 12
user users[MAX_USER];
int usercounter = 0;
score scores[MAX_USER];
int scorecounter = 0;
int game_difficulty;
int color_of_character;
char current_message[COLMAXI];
int food_count[3] = {0, 0, 0};
int weapon_count[5] = {1, 0, 0, 0, 0};
int enemies_health[5] = {5, 10, 15, 20, 30};

typedef struct position
{
    int x;
    int y;
    int revealed;
    int color;
} position;

position enemies[5];
typedef struct room
{
    position position;
    int height;
    int width;
    position door[4];
    int num_of_doors;
} room;

typedef struct player
{
    position position;
} player;
typedef struct floor
{
    room **rooms;
    int num_rooms;
    position stair;
} floor;
int floor_number = 1;
room **mapsetup();
room *createroom(int x, int y, int width, int height);
player *playersetup();
int handleinput(int input, player *user, position *traps, int trap_count);
void drawroom(room *room);
int is_room_overlapping(room *new_room, room **rooms, int num_rooms);
void start_new_game();
void add_window_pillar(room *room);
void add_traps(room *room, position *traps, int *trap_count);
void add_staircase(room *room, position *stair);
void foodmenu();

int main()
{
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    init_color(COLOR_YELLOW, 1000, 800, 0);
    init_color(COLOR_GREY, 500, 500, 500);
    init_color(COLOR_ORANGE, 1000, 500, 0);
    init_color(COLOR_GOLD, 600, 500, 50);
    init_pair(1, COLOR_BLACK, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREY, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(8, COLOR_ORANGE, COLOR_BLACK);
    init_pair(9, COLOR_GOLD, COLOR_BLACK);
    main_menu(); // menu
    endwin();
    return 0;
}

void add_traps(room *room, position *traps, int *trap_count)
{
    int num_of_traps = rand() % 5; // Up to 3 traps per room
    for (int i = 0; i < num_of_traps; i++)
    {
        int trap_x = room->position.x + 1 + rand() % (room->width - 2);
        int trap_y = room->position.y + 1 + rand() % (room->height - 2);
        traps[*trap_count].x = trap_x;
        traps[*trap_count].y = trap_y;
        (*trap_count)++;
    }
}

void add_staircase(room *room, position *stair)
{
    stair->x = room->position.x + 1 + rand() % (room->width - 2);
    stair->y = room->position.y + 1 + rand() % (room->height - 2);
    mvprintw(stair->y, stair->x, ">");
}

char hallways[200][200];

void drawgold()
{

    int count = 0;
    while (count < 20)
    {
        int x_random = rand() % COLS;
        int y_random = rand() % LINES;
        if (mvinch(y_random, x_random) == '.')
        {
            if (rand() % 10 < 9)
            {
                attron(COLOR_PAIR(2));
            }
            else
            {
                attron(COLOR_PAIR(3));
            }
            mvprintw(y_random, x_random, "*");
            count++;
        }
    }
    attroff(COLOR_PAIR(2));
    attroff(COLOR_PAIR(3));
}

void drawfood()
{
    int count = 0;
    while (count < 20)
    {
        int x_random = rand() % COLS;
        int y_random = rand() % LINES;
        if (mvinch(y_random, x_random) == '.')
        {
            int random = rand() % 3;
            if (random == 0)
            {
                mvprintw(y_random, x_random, "r");
            }
            else if (random == 1)
            {
                mvprintw(y_random, x_random, "g");
            }
            else
            {
                mvprintw(y_random, x_random, "m");
            }
            count++;
        }
    }
    attroff(COLOR_PAIR(2));
    attroff(COLOR_PAIR(3));
}

void drawweapons()
{
    int count = 0;
    while (count < 20)
    {
        int x_random = rand() % COLS;
        int y_random = rand() % LINES;
        if (mvinch(y_random, x_random) == '.')
        {
            int random = rand() % 3;
            if (random == 0)
            {
                mvprintw(y_random, x_random, ")"); // arrow
            }
            else if (random == 1)
            {
                mvprintw(y_random, x_random, "/"); // magical wand
            }
            else if (random == 2)
            {
                mvprintw(y_random, x_random, "I"); // sword
            }
            else
            {
                mvprintw(y_random, x_random, "!"); // dagger
            }

            count++;
        }
    }
}
char enemy[5] = "DFGSU";
char *enemy_names[5] = {"Demon", "Fire breathing monster", "Giant", "Snake", "Undead"};
void spawnenemy()
{
    int count = 0;
    while (count < 5)
    {
        int x_random = rand() % COLS;
        int y_random = rand() % LINES;
        if (mvinch(y_random, x_random) == '.')
        {
            mvprintw(y_random, x_random, "%c", enemy[count]);
            count++;
            enemies[count].x = x_random;
            enemies[count].y = y_random;
        }
    }
}

position password_door;

void drawpassworddoors()
{
    int count = 0;
    while (count < 1)
    {
        int x_random = rand() % COLS;
        int y_random = rand() % LINES;
        if (mvinch(y_random, x_random) == '+')
        {
            attron(COLOR_PAIR(4));
            mvprintw(y_random, x_random, "@");
            attroff(COLOR_PAIR(4));
            password_door.x = x_random;
            password_door.y = y_random;
            password_door.color = 4;
            count++;
        }
    }
}

position key;
void drawkey()
{
    int count = 0;
    while (count < 1)
    {
        int x_random = rand() % COLS;
        int y_random = rand() % LINES;
        if (mvinch(y_random, x_random) == '.')
        {
            attron(COLOR_PAIR(9));
            mvprintw(y_random, x_random, "\U000025B3");
            attroff(COLOR_PAIR(9));
            key.x = x_random;
            key.y = y_random;
            key.color = 4;
            count++;
        }
    }
}

void drawhallway()
{
    position doors[100];
    int door_count = 0;

    for (int i = 0; i < LINES; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (mvinch(i, j) == '+')
            {
                doors[door_count].x = j;
                doors[door_count].y = i;
                door_count++;
            }
        }
    }
    getch();

    for (int i = 0; i < door_count - 1; i++)
    {
        int x1 = doors[i].x;
        int y1 = doors[i].y;
        int x2 = doors[i + 1].x;
        int y2 = doors[i + 1].y;

        while (y1 != y2)
        {
            if (y1 < y2)
            {
                y1++;
            }
            else
            {
                y1--;
            }

            if (mvinch(y1, x1) == '|' || mvinch(y1, x1) == '_')
            {
                mvprintw(y1, x1, "+");
            }
            else if (mvinch(y1, x1) != '.' && mvinch(y1, x1) != '+')
            {
                mvprintw(y1, x1, "#");
            }
        }

        while (x1 != x2)
        {
            if (x1 < x2)
            {
                x1++;
            }
            else
            {
                x1--;
            }

            if (mvinch(y1, x1) == '|' || mvinch(y1, x1) == '_')
            {
                mvprintw(y1, x1, "+");
            }
            else if (mvinch(y1, x1) != '.' && mvinch(y1, x1) != '+')
            {
                mvprintw(y1, x1, "#");
            }
        }

        if (mvinch(y2, x2) == '|' || mvinch(y2, x2) == '_')
        {
            mvprintw(y2, x2, "+");
        }
        else if (mvinch(y2, x2) != '.' && mvinch(y2, x2) != '+')
        {
            mvprintw(y2, x2, "#");
        }
        // int dx = abs(x2 - x1);
        // int dy = abs(y2 - y1);
        // int sx = (x1 < x2) ? 1 : -1;
        // int sy = (y1 < y2) ? 1 : -1;
        // int err = dx - dy;

        // while (1)
        // {
        //     if (mvinch(y1, x1) != '|' && mvinch(y1, x1) != '_' && mvinch(y1, x1) != '.' && mvinch(y1, x1) != 'o' && mvinch(y1, x1) != '=')
        //     {
        //         mvaddch(y1, x1, '#');
        //     }
        //     else if (mvinch(y1, x1) == '|' || mvinch(y1, x1) == '_')
        //     {
        //         mvaddch(y1, x1, '+');
        //     }
        //     if (x1 == x2 && y1 == y2)
        //         break;
        //     int e2 = 2 * err;
        //     if (e2 > -dy)
        //     {
        //         err -= dy;
        //         x1 += sx;
        //     }
        //     if (e2 < dx)
        //     {
        //         err += dx;
        //         y1 += sy;
        //     }
        // }
    }
    getch();

    for (int i = 0; i < LINES; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (mvinch(i, j - 1) == '#' && mvinch(i, j) == '+' && mvinch(i, j + 1) == '+')
            {
                if (mvinch(i - 1, j) == '|' || mvinch(i - 1, j + 1) == '.')
                {
                    mvprintw(i + 1, j - 1, "#");
                    int k = 0;
                    while (mvinch(i, j + k) == '+')
                    {
                        mvprintw(i, j + k, "_");
                        mvprintw(i + 1, j + k, "#");
                        k++;
                    }
                    mvprintw(i + 1, j + k, "#");
                    if (mvinch(i, j + k) == '_')
                    {
                        mvprintw(i, j + k, "+");
                    }
                }

                if (mvinch(i + 1, j) == '|' || mvinch(i + 1, j + 1) == '.')
                {
                    mvprintw(i - 1, j - 1, "#");
                    int k = 0;
                    while (mvinch(i, j + k) == '+')
                    {
                        mvprintw(i, j + k, "_");
                        mvprintw(i - 1, j + k, "#");
                        k++;
                    }
                    mvprintw(i - 1, j + k, "#");
                    mvprintw(i - 1, j - 2, "#");
                    if (mvinch(i, j + k) == '_')
                    {
                        mvprintw(i, j + k, "+");
                    }
                }
            }
            if (mvinch(i - 1, j) == '#' && mvinch(i, j) == '+' && mvinch(i + 1, j) == '+')
            {
                if (mvinch(i, j - 1) == '_' || mvinch(i + 1, j - 1) == '.')
                {
                    mvprintw(i, j + 1, "#");
                    int k = 0;
                    while (mvinch(i + k, j) == '+')
                    {
                        mvprintw(i + k, j, "|");
                        mvprintw(i + k, j + 1, "#");
                        k++;
                    }
                    mvprintw(i, j, "_");
                    mvprintw(i - 1, j + 1, "#");
                    mvprintw(i + k, j + 1, "#");
                    mvprintw(i + k, j, "+");
                }

                if (mvinch(i, j + 1) == '_' || mvinch(i + 1, j + 1) == '.')
                {
                    mvprintw(i, j - 1, "#");
                    int k = 0;
                    while (mvinch(i + k, j) == '+')
                    {
                        mvprintw(i + k, j, "|");
                        mvprintw(i + k, j - 1, "#");
                        k++;
                    }
                    mvprintw(i, j, "|");
                    mvprintw(i, j - 1, "#");
                    mvprintw(i - 2, j - 1, "#");
                    mvprintw(i - 1, j - 1, "#");
                    mvprintw(i + k, j - 1, "#");
                }
            }
            if (mvinch(i, j + 1) == '#' && mvinch(i, j) == '+' && mvinch(i, j - 1) == '+')
            {
                if (mvinch(i - 1, j) == '|' || mvinch(i - 1, j - 1) == '.')
                {
                    mvprintw(i + 1, j - 1, "#");
                    int k = 0;
                    while (mvinch(i, j + k) == '+')
                    {
                        mvprintw(i, j + k, "_");
                        mvprintw(i + 1, j + k, "#");
                        k--;
                    }
                    mvprintw(i + 1, j + k, "#");
                    mvprintw(i + 1, j + 1, "#");
                    if (mvinch(i, j + k) == '_')
                    {
                        mvprintw(i, j + k, "+");
                    }
                }

                if (mvinch(i + 1, j) == '|' || mvinch(i + 1, j - 1) == '.')
                {
                    mvprintw(i - 1, j - 1, "#");
                    int k = 0;
                    while (mvinch(i, j + k) == '+')
                    {
                        mvprintw(i, j + k, "_");
                        mvprintw(i - 1, j + k, "#");
                        k--;
                    }
                    mvprintw(i - 1, j + k, "#");
                    mvprintw(i - 1, j + 1, "#");
                    if (mvinch(i, j + k) == '_')
                    {
                        mvprintw(i, j + k, "+");
                    }
                }
            }

            if (mvinch(i + 1, j) == '#' && mvinch(i, j) == '+' && mvinch(i - 1, j) == '+')
            {
                if (mvinch(i, j - 1) == '_' || mvinch(i - 1, j - 1) == '.')
                {
                    mvprintw(i, j + 1, "#");
                    int k = 0;
                    while (mvinch(i - k, j) == '+')
                    {
                        mvprintw(i - k, j, "|");
                        mvprintw(i - k, j + 1, "#");
                        k++;
                    }
                    mvprintw(i, j, "_");
                    mvprintw(i, j - 1, "_");
                    mvprintw(i - k, j + 1, "#");
                    mvprintw(i + 1, j + 1, "#");
                    mvprintw(i + 1, j - 1, " ");
                }

                if (mvinch(i, j + 1) == '_' || mvinch(i - 1, j - 1) == '.')
                {
                    mvprintw(i + 1, j - 1, "#");
                    int k = 0;
                    while (mvinch(i - k, j) == '+')
                    {
                        mvprintw(i - k, j, "|");
                        mvprintw(i - k, j - 1, "#");
                        k++;
                    }
                    mvprintw(i + k, j - 1, "#");
                }
            }
        }
    }
    getch();
    for (int i = 0; i < LINES; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (mvinch(i, j) == '#')
            {
                hallways[i][j] = '#';
                mvaddch(i, j, ' ');
            }
        }
    }

    refresh();
}

char previous_character;
int gold_count = 0;
int hunger = 10;
long long int hunger_time;
long long int health_time;
int health = 10;

void foodmenu()
{
    chtype map[LINES][COLS];
    for (int i = 0; i < LINES; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            map[i][j] = mvinch(i, j);
        }
    }
    while (1)
    {
        clear();
        mvprintw(0, 0, "a) Regular food: %d", food_count[0]);
        mvprintw(1, 0, "b) Great food: %d", food_count[1]);
        mvprintw(2, 0, "c) Magical food: %d", food_count[2]);
        int c = getch();
        if (c == 'a')
        {
            if (food_count[0] > 0)
            {
                mvprintw(4, 0, "You ate regular food.");
                food_count[0]--;
                hunger_time = time(NULL);
                hunger++;
                getch();
            }
            else
            {
                mvprintw(4, 0, "You are out of regular food.");
                getch();
            }
        }
        if (c == 'b')
        {
            if (food_count[1] > 0)
            {
                mvprintw(4, 0, "You ate great food.");
                food_count[1]--;
                hunger_time = time(NULL);
                hunger++;
                getch();
            }
            else
            {
                mvprintw(4, 0, "You are out of great food.");
                getch();
            }
        }
        if (c == 'c')
        {
            if (food_count[2] > 0)
            {
                mvprintw(4, 0, "You ate magical food.");
                food_count[2]--;
                hunger_time = time(NULL);
                hunger++;
                getch();
            }
            else
            {
                mvprintw(4, 0, "You are out of magical food.");
                getch();
            }
        }
        else if (c == 32)
        {
            break;
        }
    }
    for (int i = 0; i < LINES; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            int color_pair = (map[i][j] & A_COLOR) >> 8; // Extract color pair value
            attron(COLOR_PAIR(color_pair));
            mvprintw(i, j, "%c", map[i][j] & A_CHARTEXT);
            attroff(COLOR_PAIR(color_pair));
        }
    }
}
int current_weapon = 0;
void weaponmenu()
{
    chtype map[LINES][COLS];
    for (int i = 0; i < LINES; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            map[i][j] = mvinch(i, j);
        }
    }
    char *names[5] = {"mace", "dagger", "magic wand", "normal arrow", "sword"};
    while (1)
    {
        clear();
        mvprintw(0, 0, "a) Mace: %d", weapon_count[0]);
        mvprintw(1, 0, "b) Dagger: %d", weapon_count[1]);
        mvprintw(2, 0, "c) Magic wand: %d", weapon_count[2]);
        mvprintw(3, 0, "d) Normal arrow: %d", weapon_count[3]);
        mvprintw(4, 0, "e) Sword: %d", weapon_count[4]);
        if (current_weapon != -1)
        {
            mvprintw(6, 0, "Your current weapon is %s.", names[current_weapon]);
            getch();
            mvprintw(6, 0, "                                                 ");
        }
        else
        {
            mvprintw(6, 0, "You have got no current weapon");
            getch();
            mvprintw(6, 0, "                              ");
        }
        int c = getch();
        if (c == 'a')
        {
            if (current_weapon == -1)
            {
                mvprintw(6, 0, "You should first put your current weapon in the backpack.");
                getch();
                mvprintw(6, 0, "                                                         ");
            }
            else
            {
                current_weapon = 0;
                mvprintw(6, 0, "You picked mace.");
                getch();
                mvprintw(6, 0, "                ");
            }
        }
        if (c == 'b')
        {
            if (current_weapon == -1)
            {
                mvprintw(6, 0, "You should first put your current weapon in the backpack.");
                getch();
                mvprintw(6, 0, "                                                         ");
            }
            else
            {
                current_weapon = 1;
                mvprintw(6, 0, "You picked dagger.");
                getch();
                mvprintw(6, 0, "                  ");
            }
        }
        if (c == 'c')
        {
            if (current_weapon == -1)
            {
                mvprintw(6, 0, "You should first put your current weapon in the backpack.");
                getch();
                mvprintw(6, 0, "                                                         ");
            }
            else
            {
                current_weapon = 2;
                mvprintw(6, 0, "You picked magic wand.");
                getch();
                mvprintw(6, 0, "                                                         ");
            }
        }
        if (c == 'd')
        {
            if (current_weapon == -1)
            {
                mvprintw(6, 0, "You should first put your current weapon in the backpack.");
                getch();
                mvprintw(6, 0, "                                                         ");
            }
            else
            {
                current_weapon = 3;
                mvprintw(6, 0, "You picked normal arrow.");
                getch();
                mvprintw(6, 0, "                        ");
            }
        }
        if (c == 'e')
        {
            if (current_weapon == -1)
            {
                mvprintw(6, 0, "You should first put your current weapon in the backpack.");
                getch();
                mvprintw(6, 0, "                                                         ");
            }
            else
            {
                current_weapon = 4;
                mvprintw(6, 0, "You picked sword.");
                getch();
                mvprintw(6, 0, "                 ");
            }
        }
        else if (c == 32)
        {
            break;
        }
        else
        {
            mvprintw(6, 0, "Invalid command!");
            getch();
        }
    }
    for (int i = 0; i < LINES; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            int color_pair = (map[i][j] & A_COLOR) >> 8; // Extract color pair value
            attron(COLOR_PAIR(color_pair));
            mvprintw(i, j, "%c", map[i][j] & A_CHARTEXT);
            attroff(COLOR_PAIR(color_pair));
        }
    }
}
int password = 1234;
int key_count = 0;
int broken_key_count = 0;
void drawenemies()
{
    for (int i = 0; i < 5; i++)
    {
        if (enemies_health[i] > 0)
        {
            mvprintw(enemies[i].y, enemies[i].x, "%c", enemy[i]);
        }
    }
}

int areinsameroom(int y1, int x1, int y2, int x2)
{
    int temp = x1;
    if (x1 > x2)
    {
        x1 = x2;
        x2 = temp;
    }
    temp = y1;
    if (y1 > y2)
    {
        y1 = y2;
        y2 = temp;
    }
    int check = 0;
    while (x1 < x2)
    {
        x1++;
        if ((mvinch(y1, x1) & A_CHARTEXT) == ' ')
        {
            return 0;
        }
    }
    while (y1 < y2)
    {
        y1++;
        if ((mvinch(y1, x1) & A_CHARTEXT) == ' ')
        {
            return 0;
        }
    }
    return 1;
}
int damage;
int handleinput(int input, player *user, position *traps, int trap_count)
{
    previous_character = '.';
    int new_x = user->position.x;
    int new_y = user->position.y;
    attron(COLOR_PAIR(password_door.color));
    mvprintw(password_door.y, password_door.x, "@");
    attroff(COLOR_PAIR(password_door.color));

    if (input == ' ')
    {
        if (current_weapon == 0 || current_weapon == 4)
        {
            if (current_weapon == 0)
            {
                damage = 5;
            }
            else
            {
                damage = 10;
            }
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    if (i == 0 && j == 0)
                    {
                        continue;
                    }

                    for (int k = 0; k < 5; k++)
                    {
                        if ((mvinch(i + user->position.y, j + user->position.x) & A_CHARTEXT) == enemy[k])
                        {
                            enemies_health[k] -= damage;
                            mvprintw(0, 0, "You hit the %s!", enemy_names[k]);
                            getch();
                            mvprintw(0, 0, "                                       ");
                        }
                    }
                }
            }
        }
    }
    else if (input == 'h' || input == 'H' || input == KEY_LEFT)
    { // Left
        new_x--;
    }
    else if (input == 'j' || input == 'J' || input == KEY_DOWN)
    { // Down
        new_y++;
    }
    else if (input == 'k' || input == 'K' || input == KEY_UP)
    { // Up
        new_y--;
    }
    else if (input == 'l' || input == 'L' || input == KEY_RIGHT)
    { // Right
        new_x++;
    }
    else if (input == 'y' || input == 'Y')
    { // Up Left
        new_x--;
        new_y--;
    }
    else if (input == 'u' || input == 'U')
    { // Up Right
        new_x++;
        new_y--;
    }
    else if (input == 'b' || input == 'B')
    { // Down Left
        new_x--;
        new_y++;
    }
    else if (input == 'n' || input == 'N')
    { // Down Right
        new_x++;
        new_y++;
    }
    else if (input == 'e' || input == 'E')
    {
        foodmenu();
    }
    else if (input == 'i' || input == 'I')
    {
        weaponmenu();
    }
    else if (input == 'w' || input == 'W')
    {
        if (current_weapon != -1)
        {
            mvprintw(0, 0, "You put your current weapon in the backpack.");
            current_weapon = -1;
        }
        else
        {
            mvprintw(0, 0, "You have no current weapon.");
        }
    }

    if ((mvinch(new_y, new_x) & A_CHARTEXT) == '@' && (mvinch(new_y, new_x) & A_COLOR) == COLOR_PAIR(4))
    {
        int try_count = 0;
        mvprintw(0, 0, "Enter P if you want to use a password and enter K if you want to use a key: ");
        int c = getch();
        mvprintw(0, 0, "                                                                             ");
        if (c == 'p' || c == 'P')
        {
            while (try_count < 3)
            {
                mvprintw(0, 0, "Enter Password: ");
                curs_set(1);
                int entered_password;
                echo();
                scanw("%d", &entered_password);
                noecho();
                if (entered_password == password)
                {
                    attron(COLOR_PAIR(5));
                    mvprintw(1, 0, "Correct!");
                    attroff(COLOR_PAIR(5));
                    password_door.color = 5;
                    curs_set(0);
                    getch();
                    break;
                }
                else
                {
                    if (try_count == 0)
                    {
                        attron(COLOR_PAIR(2));
                    }
                    if (try_count == 1)
                    {
                        attron(COLOR_PAIR(8));
                    }
                    if (try_count == 2)
                    {
                        attron(COLOR_PAIR(4));
                    }

                    mvprintw(1, 0, "Wrong");
                    attroff(COLOR_PAIR(2));
                    attroff(COLOR_PAIR(4));
                    attroff(COLOR_PAIR(4));
                    try_count++;
                }
                mvprintw(0, 0, "Enter Password:     ");
                curs_set(0);
            }
        }
        else if (c == 'k' || c == 'K')
        {
            if (key_count < 1)
            {
                mvprintw(0, 0, "You have no key.");
            }
            else if (broken_key_count >= 2)
            {
                mvprintw(0, 0, "You opened the door using two broken keys.");
                broken_key_count -= 2;
                password_door.color = 5;
                getch();
                mvprintw(0, 0, "                                          ");
            }
            else
            {
                mvprintw(0, 0, "You opened the door using one key.");
                key_count--;
                password_door.color = 5;
                getch();
                mvprintw(0, 0, "                                  ");
            }
        }
        mvprintw(0, 0, "                         ");
        mvprintw(1, 0, "                         ");
    }

    if (new_y == key.y && new_x == key.x)
    {
        mvprintw(0, 0, "You picked up a key.");
        getch();
        mvprintw(0, 0, "                    ");
        key_count++;
        mvprintw(user->position.y, user->position.x, "%c", previous_character);
        user->position.x = new_x;
        user->position.y = new_y;
    }

    if (((mvinch(new_y, new_x) & A_CHARTEXT) == '@' && (mvinch(new_y, new_x) & A_COLOR) == COLOR_PAIR(5)) || mvinch(new_y, new_x) == '.' || mvinch(new_y, new_x) == '+' || mvinch(new_y, new_x) == '#' || mvinch(new_y, new_x) == 'r' || mvinch(new_y, new_x) == 'g' || mvinch(new_y, new_x) == 'm' || mvinch(new_y, new_x) == '/' || mvinch(new_y, new_x) == 'I' || mvinch(new_y, new_x) == '!' || mvinch(new_y, new_x) == ')' || (mvinch(new_y, new_x) & A_CHARTEXT) == '*')
    {
        // attron(COLOR_PAIR(character_color));
        mvprintw(user->position.y, user->position.x, "%c", previous_character);
        user->position.x = new_x;
        user->position.y = new_y;
    }

    if ((mvinch(new_y, new_x) & A_CHARTEXT) == '*')
    {
        if ((mvinch(new_y, new_x) & A_COLOR) == COLOR_PAIR(2))
        {
            gold_count += 5;
            mvprintw(0, 0, "You picked up 5 gold pieces.");
            getch();
            mvprintw(0, 0, "                            ");
        }
        else
        {
            gold_count += 20;
            mvprintw(0, 0, "You picked up 20 gold pieces. (black gold)");
            getch();
            mvprintw(0, 0, "                                          ");
        }
    }

    if (mvinch(new_y, new_x) == 'r')
    {
        food_count[0]++;
        mvprintw(0, 0, "You picked up regular food.");
        getch();
        mvprintw(0, 0, "                           ");
    }

    if (mvinch(new_y, new_x) == 'g')
    {
        food_count[1]++;
        mvprintw(0, 0, "You picked up great food.");
        getch();
        mvprintw(0, 0, "                           ");
    }

    if (mvinch(new_y, new_x) == 'm')
    {
        food_count[2]++;
        mvprintw(0, 0, "You picked up magical food.");
        getch();
        mvprintw(0, 0, "                           ");
    }

    if (mvinch(new_y, new_x) == '!')
    {
        weapon_count[3] += 10;
        mvprintw(0, 0, "You picked up 10 daggers.");
        getch();
        mvprintw(0, 0, "                            ");
    }

    if (mvinch(new_y, new_x) == '/')
    {
        weapon_count[3] += 8;
        mvprintw(0, 0, "You picked up 8 magic wands.");
        getch();
        mvprintw(0, 0, "                            ");
    }

    if (mvinch(new_y, new_x) == ')')
    {
        weapon_count[3] += 20;
        mvprintw(0, 0, "You picked up 20 normal arrows.");
        getch();
        mvprintw(0, 0, "                               ");
    }

    for (int i = 0; i < trap_count; i++)
    {
        if (traps[i].x == user->position.x && traps[i].y == user->position.y)
        {
            mvprintw(user->position.y, user->position.x, "^");
            mvprintw(0, 0, "You have stepped on a trap!");
            traps[i].revealed = 1;
            health--;
            getch();
            mvprintw(0, 0, "                           ");
            break;
        }
    }
    attron(A_REVERSE);
    mvprintw(user->position.y, user->position.x, "@");
    attroff(A_REVERSE);
    mvprintw(LINES - 1, COLS - 20, "Keys:               ");
    move(LINES - 1, COLS - 14);
    attron(COLOR_PAIR(9));
    for (int i = 0; i < key_count; i++)
    {
        printw("\U000025B6 ");
    }
    for (int i = 0; i < broken_key_count; i++)
    {
        printw("\U000025B7 ");
    }
    attroff(COLOR_PAIR(9));
    mvprintw(LINES - 1, 10, "Gold: %d", gold_count);
    if (hunger > 10)
    {
        hunger = 10;
    }
    if (difftime(time(NULL), hunger_time) >= 20)
    {
        if (hunger > 0)
        {
            hunger--;
            hunger_time = time(NULL);
        }
    }
    if (hunger == 0)
    {
        if (difftime(time(NULL), health_time) >= 10)
        {
            health--;
            health_time = time(NULL);
        }
    }
    mvprintw(LINES - 1, 30, "Hunger:                     ");
    move(LINES - 1, 39);
    for (int i = 0; i < hunger; i++)
    {
        printw("\U0001F355");
    }
    mvprintw(LINES - 1, 70, "Health:                         ");
    move(LINES - 1, 78);
    attron(COLOR_PAIR(4));
    for (int i = 0; i < health; i++)
    {
        printw("\U00002764 ");
    }
    attroff(COLOR_PAIR(4));
    for (int i = 0; i < trap_count; i++)
    {
        if (traps[i].revealed == 1)
        {
            mvprintw(traps[i].y, traps[i].x, "^");
        }
    }
    refresh();
    for (int i = 0; i < 5; i++)
    {
        if (areinsameroom(enemies[i].y, enemies[i].x, user->position.y, user->position.x))
        {
            int x_new = enemies[i].x, y_new = enemies[i].y;
            if (enemies[i].x < user->position.x)
            {
                x_new++;
            }
            else if (enemies[i].x > user->position.x)
            {
                x_new--;
            }
            else if (enemies[i].y > user->position.y)
            {
                y_new--;
            }
            else if (enemies[i].y > user->position.y)
            {
                y_new++;
            }
            if (((mvinch(y_new, x_new) & A_CHARTEXT) == '@' && (mvinch(y_new, x_new) & A_COLOR) == COLOR_PAIR(5)) || mvinch(y_new, x_new) == '.' || mvinch(y_new, x_new) == '+' || mvinch(y_new, x_new) == '#' || mvinch(y_new, x_new) == 'r' || mvinch(y_new, x_new) == 'g' || mvinch(y_new, x_new) == 'm' || mvinch(y_new, x_new) == '/' || mvinch(y_new, x_new) == 'I' || mvinch(y_new, x_new) == '!' || mvinch(y_new, x_new) == ')' || (mvinch(y_new, x_new) & A_CHARTEXT) == '*')
            {
                mvprintw(enemies[i].y, enemies[i].x, "%c", previous_character);
                enemies[i].x = x_new;
                enemies[i].y = y_new;
            }
        }
    }
    drawenemies();
    return 0;
}

room *createroom(int x, int y, int height, int width)
{
    room *newroom = malloc(sizeof(room));
    newroom->position.x = x;
    newroom->position.y = y;
    newroom->height = height;
    newroom->width = width;
    int num_of_doors = 2; // 1 to 4 doors
    newroom->num_of_doors = num_of_doors;
    int door_1_position, door_position;
    for (int i = 0; i < num_of_doors; i++)

    {
        if (i == 1)
        {
            do
            {
                door_position = rand() % 4;

            } while (door_position == door_1_position);
        }
        else
        {

            door_position = rand() % 4;
        }
        if (door_position == 0) // up

        {
            newroom->door[i].x = x + 1 + rand() % (width - 2);
            newroom->door[i].y = y;
        }
        else if (door_position == 1) // left

        {
            newroom->door[i].x = x;
            newroom->door[i].y = y + 1 + rand() % (height - 2);
        }
        else if (door_position == 2) // down

        {
            newroom->door[i].x = x + 1 + rand() % (width - 2);
            newroom->door[i].y = y + height - 1;
        }
        else if (door_position == 3) // right

        {
            newroom->door[i].x = x + width - 1;
            newroom->door[i].y = y + 1 + rand() % (height - 2);
        }
        door_1_position = door_position;
    }
    return newroom;
}

void drawroom(room *room)
{
    if (room == NULL)
        return;
    for (int x = room->position.x; x < room->position.x + room->width; x++)

    {
        mvprintw(room->position.y, x, "_");                    // Top border
        mvprintw(room->position.y + room->height - 1, x, "_"); // Bottom border
    }

    for (int y = room->position.y + 1; y < room->position.y + room->height - 1; y++)

    {
        mvprintw(y, room->position.x, "|");                   // Left border
        mvprintw(y, room->position.x + room->width - 1, "|"); // Right border
        for (int x = room->position.x + 1; x < room->position.x + room->width - 1; x++)

        {
            mvprintw(y, x, "."); // Floor
        }
    }
    for (int i = 0; i < room->num_of_doors; i++)

    {
        mvprintw(room->door[i].y, room->door[i].x, "+");
    }
}
int is_room_overlapping(room *new_room, room **rooms, int num_rooms)
{
    for (int i = 0; i < num_rooms; i++)

    {
        room *existing_room = rooms[i];
        if (new_room->position.x < existing_room->position.x + existing_room->width + 1 &&
            new_room->position.x + new_room->width + 1 > existing_room->position.x &&
            new_room->position.y < existing_room->position.y + existing_room->height + 1 &&
            new_room->position.y + new_room->height + 1 > existing_room->position.y)

        {
            return 1; // Overlapping
        }
    }
    return 0; // Not overlapping
}
room **mapsetup()
{
    srand(time(NULL));
    int num_of_room = MIN_ROOMS + rand() % (MAX_ROOMS - MIN_ROOMS + 1); // 6 to 10 rooms
    room **rooms = malloc(num_of_room * sizeof(room *));
    for (int i = 0; i < num_of_room; i++)

    {
        room *new_room;
        int overlap;
        do

        {
            int width = 6 + rand() % 13; // min 16 noghte
            int height = 6 + rand() % 13;
            int x = rand() % (COLS - width);
            int y = rand() % (LINES - height);
            new_room = createroom(x, y, height, width);
            overlap = is_room_overlapping(new_room, rooms, i);
        } while (overlap);
        rooms[i] = new_room;
        drawroom(rooms[i]);
    }
    return rooms;
}
void start_new_game()
{
    clear();
    refresh();
    room **rooms = mapsetup();
    int num_rooms = MIN_ROOMS + rand() % (MAX_ROOMS - MIN_ROOMS + 1); // Random number of rooms
    player *user = playersetup(rooms, num_rooms);
    position traps[100];
    int trap_count = 0;

    for (int i = 0; i < num_rooms; i++)
    {
        drawroom(rooms[i]);
        add_traps(rooms[i], traps, &trap_count);
    }
    mvprintw(0, COLS - 25, "You are on floor %d", floor_number);
    mvprintw(user->position.y, user->position.x, "@");
    refresh();
    int ch;
    drawhallway();
    drawgold();
    drawfood();
    drawweapons();
    drawpassworddoors();
    spawnenemy();
    drawkey();
    // add_window_pillar(room);
    while ((ch = getch()) != 'q')
    {
        for (int i = 0; i < LINES; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                if (hallways[i][j] == '#' && abs(i - user->position.y) <= 5 && abs(j - user->position.x) <= 5)
                {
                    mvprintw(i, j, "#");
                }
            }
        }
        handleinput(ch, user, traps, trap_count);
    }
}

player *playersetup(room **rooms, int num_rooms)
{
    player *new_player = malloc(sizeof(player));
    room *starting_room = rooms[rand() % num_rooms];
    new_player->position.x = starting_room->position.x + 1 + rand() % (starting_room->width - 2);
    new_player->position.y = starting_room->position.y + 1 + rand() % (starting_room->height - 2);
    return new_player;
}
void add_window_pillar(room *room)
{
    int num_of_windows = rand() % 5;
    int num_of_pillars = rand() % 5;
    for (int i = 0; i < num_of_pillars; i++)

    {
        int pillar_x = room->position.x + 1 + rand() % (room->width - 2);
        int pillar_y = room->position.y + 1 + rand() % (room->height - 2);
        mvprintw(pillar_y, pillar_x, "o");
    }
    int window_x, window_y;
    for (int i = 0; i < num_of_windows; i++)

    {
        int whichwall = rand() % 4; // 0 1 2 3
        if (whichwall == 0)         // up

        {
            window_x = room->position.x + 1 + rand() % (room->width - 2);
            window_y = room->position.y;
        }
        else if (whichwall == 1) // left

        {
            window_x = room->position.x;
            window_y = room->position.y + rand() % (room->height - 2);
        }
        else if (whichwall == 2) // down

        {
            window_x = room->position.x + 1 + rand() % (room->width - 2);
            window_y = room->position.y + room->height - 1;
        }
        else if (whichwall == 3) // right

        {
            window_x = room->position.x + room->width - 1;
            window_y = room->position.y + rand() % (room->height - 2);
        }
        mvprintw(window_y, window_x, "=");
    }
}