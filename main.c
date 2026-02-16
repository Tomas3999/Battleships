#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/*
 * Upravene:
 *      Player_vs_Player - done
 *      ship_place - done
 *      end_screen - done
 *      Computer_vs_Computer - done
 *      automatic_shots - done
 */

typedef struct screen_size
{
    int columns;
    int rows;
    int y_offset_screen;

} SCREEN_SIZE;

typedef struct position
{
    int x_coordinates;
    int y_coordinates;

} POSITION;


typedef struct ship_status
{
    POSITION* pos;
    int not_hit;
    int ship_size;
    int rotation;

} SHIP_STATUS;

typedef struct fleet_status
{
    SHIP_STATUS* fleet;
    int number_of_ships_in_play;
    int hit;
    int miss;

} FLEET_STATUS;

typedef struct auto_hit
{
    POSITION pos_A;
    int mode;

} AUTO_HIT;


#define DELAY_TIME 1000 //v milisekundách

#ifdef _WIN32
#include <windows.h>
 //#define DELAY()  Sleep(DELAY_TIME);
 void initial_setup_windows(SCREEN_SIZE* size_of_screen)
 {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD console_information;

    GetConsoleMode(hConsole, &console_information);
    SetConsoleMode(hConsole, console_information | ENABLE_VIRTUAL_TERMINAL_PROCESSING);  //inicializuje ANSI escape

    CONSOLE_SCREEN_BUFFER_INFO size_screen;

    GetConsoleScreenBufferInfo(hConsole, &size_screen);
    size_of_screen->columns = size_screen.dwSize.X;
    size_of_screen->rows = size_screen.dwSize.Y;
 };
#else
#include <unistd.h>
#include <sys/ioctl.h>
//#define DELAY() usleep(1000*DELAY_TIME)
void initial_setup_linux(SCREEN_SIZE* size_of_screen)
{
    struct winsize size_screen;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size_screen);
    size_of_screen->columns = size_screen.ws_col;
    size_of_screen->rows = size_screen.ws_row;
}
#endif


#define NUMBER_OF_SHIPS 5 // od 1 po 5
#define LINE_BUFFER 2
#define NUMBER_OF_PLAYERS 2 //nemeniť !!!

#define MAX_CHARACTER_END 12// veľkosť COMPUTER c:

//modes:
#define PLAYER_VS_PLAYER 0
#define PLAYER_VS_COMPUTER 1
#define COMPUTER_VS_COMPUTER 2

//modes for automatic shots
#define X_POSITIVE 1
#define X_NEGATIVE -1
#define Y_POSITIVE 2
#define Y_NEGATIVE -2
#define AUTO_RESET 0

//SYMBOLS MAP
#define HIT 'O'
#define MISS 'X'
#define HIT_NOW 'H'
#define MISS_NOW 'M'
#define WATER '~'

//SYMBOLS SHIP VERTICAL
#define SHIP_VERTICAL_START '^'
#define SHIP_VERTICAL_MID '|'
#define SHIP_VERTICAL_END 'v'

//SYMBOLS SHIP HORIZONTAL
#define SHIP_HORIZONTAL_START '<'
#define SHIP_HORIZONTAL_MID '='
#define SHIP_HORIZONTAL_END '>'

#define LINE_SYMBOL '-'
#define BUFFER_END 0
#define BUFFER_MID 5

#define DEFAULT_WIDTH 10
#define DEFAULT_LENGTH 10

#define WIDTH_MIN 5
#define LENGTH_MIN 5
#define SPACE_OFFSET 2

#define AUTOMATIC_PLACEMENT_ENABLED 1
#define AUTOMATIC_PLACEMENT_DISABLED 0

//rotation ship
#define VERTICAL_ROTATION 0
#define HORIZONTAL_ROTATION 1

//inputs
#define INPUT_ERROR -1
#define INPUT_ROTATION -2
#define INPUT_AUTOMATIC_PLACEMENT_ENABLED -3
#define INPUT_HELP -4

//shots
#define SHOT_HIT 0
#define SHOT_MISS 1
#define SHOT_OCCUPIED 2

//ship
#define SHIP_DESTROYED 0
#define SHIP_NOT_DESTROYED 2
#define SHIP_OCCUPIED 1
#define SHIP_PLACED 0
#define SHIP_NOT_PLACED 1

//characters
#define CHARACTER_CHANGED 0
#define CHARACTER_NOT_CHANGED 1


void map_generating(int width, int length, char map[length+1][width+1]);
void map_print(int width, int length, char map[length][width],char map_hit[length][width], int x_coord, int y_coord);
int change_character_map(int x_coordinates, int y_coordinates, int width, int length, char map[length][width],char new_symbol, char old_symbol);

int ship_place(int ship_length, int rotate, int x_coordinates, int y_coordinates, int width, int length,
               char map[length][width],SHIP_STATUS* ship);
void fleet_placement(int width, int length, char map[length][width],FLEET_STATUS* fleet_array, int automatic_placement,
                     SCREEN_SIZE* screen_info);

char* line_read();
int counter(char *input_string);
int test_input(char *input_string, int width, int length);
void rotate_graphics(int rotate, int ship_length, int width, int length, int y_offset);

void line(int number);
void go_to(int x_coord, int y_coord);
void reset_screen(int x, int y, int width, int length);
void delay();
void continue_button(int height);

int hit_check(int width, int length, char map_to_hit[length][width], char map_to_show[length][width],
              int x_coord_hit, int y_coord_hit,FLEET_STATUS* fleet_a, SCREEN_SIZE* screen_info);

void explosion_graphics(SHIP_STATUS* ship_explosion, int width, int length, char ship_hit_map[length][width]);

void setup(int width, int length, char map[2][length][width], char map_hit[2][length][width], FLEET_STATUS fleet_array[], int mode, SCREEN_SIZE* screen_info);

void game_Player_vs_Player(int width, int length, char map[2][length][width], char map_hit[2][length][width], FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info);
void game_Player_vs_Computer(int width, int length, char map[2][length][width], char map_hit[2][length][width], FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info);
void game_Computer_vs_Computer(int width, int length, char map[2][length][width], char map_hit[2][length][width], FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info);


int random_number(int down, int up);
void random_initialization();

void free_memory(FLEET_STATUS* fleet_array);

void automatic_shots(int width, int length, char map[length][width], char map_hit[length][width], FLEET_STATUS* fleet, AUTO_HIT* auto_info, SCREEN_SIZE* screen_info);
void find_coordinates_hit(int width, int length, char map_hit[length][width], AUTO_HIT* auto_info);
int shot_fired(int width, int length, char map[length][width], char map_hit[length][width], AUTO_HIT* auto_info, FLEET_STATUS* fleet, int x_offset, int y_offset, SCREEN_SIZE* screen_info);

void Battle_ship_title(int width);
void size_of_map(int* width, int* length, SCREEN_SIZE* size_of_screen);

void instructions_print(int x_coord, int y_coord, char text[]);
void instructions_fleet_placement(int width,SCREEN_SIZE* screen_info);
void instructions_game(SCREEN_SIZE* screen_info);

void end_screen(char winner_name[], char loser_name[], int index, FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info);

void show_entity_console(int entity, char* entityText, int width, int length, char map[NUMBER_OF_PLAYERS][length][width], char map_hit[NUMBER_OF_PLAYERS][length][width],
                         SCREEN_SIZE* screen_info);
int one_move(int player,int width, int length, char map[NUMBER_OF_PLAYERS][length][width], char map_hit[NUMBER_OF_PLAYERS][length][width],
            FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info);
int check_winner(int player, char* playerName, FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info);
int fire_on_new_ship(int width, int length, char map[length][width], char map_hit[length][width], FLEET_STATUS* fleet,
                     AUTO_HIT* auto_info, SCREEN_SIZE* screen_info);
int directional_shots(int width, int length, char map[length][width], char map_hit[length][width], FLEET_STATUS* fleet,
                      AUTO_HIT* auto_info, SCREEN_SIZE* screen_info);
void place_ship_map(int ship_length, int rotate, int x_coordinates, int y_coordinates, int width, int length, SHIP_STATUS* ship,
                    int x_active, int y_active);

//COLORS//
void red();
void blue();
void green();
void white();
void yellow();
void bright_red();
void orange();

int main() {

    SCREEN_SIZE size_screen;

#ifdef _WIN32
    initial_setup_windows(&size_screen);
#else
    initial_setup_linux(&size_screen);
#endif

    random_initialization();

    reset_screen(1,1,size_screen.columns,size_screen.rows);

    Battle_ship_title(size_screen.columns);

    int width_m;
    int length_m;

    size_of_map(&width_m,&length_m,&size_screen);

    char *input;
    char map[NUMBER_OF_PLAYERS][length_m][width_m];
    char map_hit[NUMBER_OF_PLAYERS ][length_m][width_m];


    FLEET_STATUS fleet[NUMBER_OF_PLAYERS];

    for (int i = 0; i<NUMBER_OF_PLAYERS; i++)
    {
        fleet[i].miss = 0;
        fleet[i].hit = 0;
    }


    go_to (1,size_screen.y_offset_screen);
    printf("SELECT MODE (type number(0,1,2)): \n");
    printf("PLAYER VS PLAYER:     0\n");
    printf("PLAYER VS COMPUTER:   1\n");
    printf("COMPUTER VS COMPUTER: 2");

    go_to(34,size_screen.y_offset_screen); // 34 = strlen(SELECT MODE (type number(0,1,2)):)
    input=line_read();

    int in;
    sscanf(input,"%d",&in);
    free(input);

    reset_screen(1,size_screen.y_offset_screen,size_screen.columns,4);

    setup(width_m,length_m,map,map_hit,fleet,in, &size_screen);

    switch(in)
    {
        case PLAYER_VS_COMPUTER:
            game_Player_vs_Computer(width_m,length_m,map,map_hit,fleet, &size_screen);
            break;
        case COMPUTER_VS_COMPUTER:
            game_Computer_vs_Computer(width_m,length_m,map,map_hit,fleet, &size_screen);
            break;
        default:
            game_Player_vs_Player(width_m,length_m,map,map_hit,fleet, &size_screen);
            break;
    }

    free_memory(fleet);

    return 0;
}

/*
FUNKCIA: size_of_map(int* width, int* length, SCREEN_SIZE* size_of_screen)

////// Vstup //////
width - pointer na celé kladné čislo, ktoré udáva šírku hracej plochy;
length - pointer na celé kladné číslo, ktoré udáva dĺžku hracej plochy;
size_of_screen - pointer na štruktúru SCREEN_SIZE, štruktúra SCREEN_SIZE udáva informácie o veľkosti konzoly (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia zistí veľkosť hracieho poľa, buď od používateľa alebo nastaví default hodnotu;

////// Výstup /////
Funkcia nič nevracia;
*/

void size_of_map(int* width, int* length, SCREEN_SIZE* size_of_screen)
{
    size_of_screen->y_offset_screen = (size_of_screen->columns>=104 ? 5 : 1) + SPACE_OFFSET;
    int width_max = (size_of_screen->columns -(BUFFER_END+BUFFER_MID)*2)/6;
    int length_max = size_of_screen->rows - size_of_screen->y_offset_screen - 5;

    int width_tem;
    int length_tem;

    char* input;
    go_to(1,size_of_screen->y_offset_screen);

    printf("Type width of field (from %d to %d): ", WIDTH_MIN ,width_max);
    input = line_read();

    if (sscanf(input, "%d", &width_tem)>0 && width_tem<=width_max && width_tem>=WIDTH_MIN)
    {
        free(input);
        go_to(1,size_of_screen->y_offset_screen + 1);
        printf("Type length of field (from %d to %d): ", LENGTH_MIN, length_max);
        input = line_read();

        if (sscanf(input, "%d", &length_tem)>0 && length_tem<=length_max && width_tem>=LENGTH_MIN)
        {
            free(input);
            *width=width_tem;
            *length=length_tem;
            reset_screen(1,size_of_screen->y_offset_screen,size_of_screen->columns,2);
            return;
        }
    }

    free(input);
    reset_screen(1,size_of_screen->y_offset_screen,size_of_screen->columns,2);

    if (width_max>=DEFAULT_WIDTH)
        *width=DEFAULT_WIDTH;
    else
        *width=width_max;

    if (length_max>=DEFAULT_LENGTH)
        *length=DEFAULT_LENGTH;
    else
        *length=length_max;

}

/*
FUNKCIA: Battle_ship_title(int width)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;

////// Čo robí funkcia /////
Funkcia vypíše názov: BATTLESHIPS;

////// Výstup /////
Funkcia nič nevracia;
*/

void Battle_ship_title(int width)
{
    if (width>=104)
    {
        int x=width/2 - 104/2;
        go_to(x, 1);
        char arr[5][104] = {
                "|||||      //\\\\   ||||||||||  ||||||||||  ||        ||||||||   ||||||   ||    ||  ||  |||||    |||||| ",
                "||  ||    //  \\\\      ||          ||      ||        ||        ||        ||    ||  ||  ||  ||  ||      ",
                "|||||    //||||\\\\     ||          ||      ||        ||||||||   ||||||   ||||||||  ||  |||||    |||||| ",
                "||  ||  //      \\\\    ||          ||      ||        ||              ||  ||    ||  ||  ||            ||",
                "|||||  //        \\\\   ||          ||      ||||||||  ||||||||   ||||||   ||    ||  ||  ||       |||||| "};
        for (int i = 0; i < 5; i++) {
            go_to(x,1+i);
            printf("%s", arr[i]);
        }
    }
    else
    {
        int x=width/2 - 12/2; //12 = strlen("BATTLESHIPS") + 1
        go_to(x, 1);
        printf("BATTLESHIPS");
    }
}

/*
FUNKCIA: setup(int width, int length, char map[NUMBER_OF_PLAYERS][length][width], char map_hit[NUMBER_OF_PLAYERS][length][width],
           FLEET_STATUS fleet_array[], int mode)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map - trojrozmerné pole, udáva pole hracích ploch všetkých hráčov, do ktorej hráč vloží svoje lode;
map hit - trojrozmerné pole, udáva pole hracích ploch všetkých hráčov, do ktorej sa bude zaznačovať zásahy a miss;
fleet_array[] - pole štruktúry FLEET_STATUS, udáva všetky informácie o stave flotily pre každého hráča;
mode - celé kladné číslo, udáva mod hry (PLAYER VS PLAYER, PLAYER VS COMPUTER, COMPUTER VS COMPUTER);

////// Čo robí funkcia /////
Funkcia vygeneruje hracie pole, do ktorého potom vloží hráč (počítač) lode a vygeneruje pole pre zaznámenávanie zásahov;

////// Výstup /////
Funkcia nič nevracia;
 */

void setup(int width, int length, char map[NUMBER_OF_PLAYERS][length][width], char map_hit[NUMBER_OF_PLAYERS][length][width],
           FLEET_STATUS fleet_array[], int mode, SCREEN_SIZE* screen_info)
{
    for (int i=0; i<NUMBER_OF_PLAYERS; i++)
    {
        map_generating(width,length,map[i]);
        memcpy(map_hit[i],map[i],width*length);

        go_to(1,screen_info->y_offset_screen);

        switch(mode)
        {
            case COMPUTER_VS_COMPUTER:
                fleet_placement(width,length, map[i], &fleet_array[i],AUTOMATIC_PLACEMENT_ENABLED, screen_info);
                break;

            case PLAYER_VS_COMPUTER:
                if (i==0)
                {
                    printf("PLAYER:");
                    map_print(width,length,map[i],NULL,1,screen_info->y_offset_screen+1);
                    fleet_placement(width,length, map[i], &fleet_array[i],AUTOMATIC_PLACEMENT_DISABLED, screen_info);
                    map_print(width,length,map[i],NULL,1,screen_info->y_offset_screen+1);
                    continue_button(screen_info->y_offset_screen+length+1+2);
                }
                else
                {
                    fleet_placement(width,length, map[i], &fleet_array[i],AUTOMATIC_PLACEMENT_ENABLED, screen_info);
                }
                break;
            default: //PLAYER VS PLAYER
                printf("PLAYER %d:", i + 1);
                map_print(width, length, map[i], NULL, 1, screen_info->y_offset_screen+1);

                fleet_placement(width, length, map[i], &fleet_array[i], AUTOMATIC_PLACEMENT_DISABLED, screen_info);
                map_print(width, length, map[i], NULL, 1, screen_info->y_offset_screen+1);
                continue_button(screen_info->y_offset_screen+length+1+2);

                break;
        }
        reset_screen(1,screen_info->y_offset_screen,screen_info->columns,length);
    }
}

/*
FUNKCIA: continue_button(int height)

////// Vstup //////
height - celé kladné číslo, udáva v ktorom riadtku od hora sa ma uskutočniť výpis;

////// Čo robí funkcia /////
Funkcia vypíše (PRESS ENTER TO CONTINUE!) a čaká na hociaký vstup, ak dostane vstup: zmaže čo vypísala a skončí;

////// Výstup /////
Funkcia nič nevracia;
 */

void continue_button(int height)
{
    go_to(1,height);
    printf("PRESS ENTER TO CONTINUE!");
    char* input=line_read();
    free(input);
    reset_screen(1,height,30,1);
}

/*
FUNKCIA: end_screen(char winner_name[], char loser_name[], int index, FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info)

////// Vstup //////
winner_name - reťazec, ktorý udáva meno hráča, ktorý vyhral;
loser_name - reťazec, ktorý udáva meno hráča, ktorý prehral;
index - celé kladné číslo (0 alebo 1), udáva index hráča ktorý vyhral;
fleet_array[] - pole štruktúry FLEET_STATUS, udáva všetky informácie o stave flotily pre každého hráča;
screen_info - pointer na štruktúru SCREEN_SIZE, štruktúra SCREEN_SIZE udáva informácie o veľkosti konzoly (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia vypíše konečný výstup s menom výhercu a so štatistikami hry;

////// Výstup /////
Funkcia nič nevracia;
*/

void end_screen(char winner_name[], char loser_name[], int index, FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info)
{
    int y=screen_info->y_offset_screen;
    int y_coordinates = (y+14<screen_info->rows ? y: 1);

    reset_screen(1,y_coordinates,screen_info->columns,
                 y+14 < screen_info->rows ? (screen_info->rows-y) : screen_info->rows);

    go_to(1,y_coordinates);
    printf("WINNER: %s\n\n", winner_name);
    printf("Statistics:\n");

    int indexWinner = (index == 0 ? 1:0);

    line(screen_info->columns);
    printf("\n");
    printf("%s\n", winner_name);
    printf("Shots fired: %d\n", fleet_array[indexWinner].miss+fleet_array[indexWinner].hit);
    printf("Shots miss:  %d\n", fleet_array[indexWinner].miss);
    printf("Shots hit:   %d\n", fleet_array[indexWinner].hit);

    line(screen_info->columns);
    printf("\n");
    printf("%s\n", loser_name);
    printf("Shots fired: %d\n", fleet_array[index].miss + fleet_array[index].hit);
    printf("Shots miss:  %d\n", fleet_array[index].miss);
    printf("Shots hit:   %d\n", fleet_array[index].hit);

    continue_button(y+12<screen_info->rows ? y + 14 : 14);
    reset_screen(1,1,screen_info->columns, screen_info->rows);
}

/*
FUNKCIA: game_Player_vs_Computer(int width, int length, char map[NUMBER_OF_PLAYERS][length][width],
                    char map_hit[NUMBER_OF_PLAYERS][length][width], FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map - trojrozmerné pole, udáva pole hracích ploch všetkých hráčov, kde sú umiestnené lode, každého hráča;
map hit - trojrozmerné pole, udáva pole hracích ploch všetkých hráčov, kde sú zaznačené zásahy a miss;
fleet_array[] - pole štruktúry FLEET_STATUS, udáva všetky informácie o stave flotily pre každého hráča;
screen_info - pointer na štruktúru SCREEN_SIZE, štruktúra SCREEN_SIZE udáva informácie o veľkosti konzoly (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia symuluje hru Battleships medzi hráčom a počítačom;

////// Výstup /////
Funkcia nič nevracia;
*/

void game_Player_vs_Computer(int width, int length, char map[NUMBER_OF_PLAYERS][length][width], char map_hit[NUMBER_OF_PLAYERS][length][width],
                             FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info)
{
    AUTO_HIT information_automatic;
    char* input;
    int y;

    int check_hit;
    information_automatic.mode=0;

    while (1)
    {
        while(1)
        {
            reset_screen(1,screen_info->y_offset_screen,screen_info->columns,length);
            go_to(1,screen_info->y_offset_screen);
            printf("PLAYER:");
            map_print(width,length,map[0],map_hit[1],1,screen_info->y_offset_screen+1);
            map_print(width,length,map_hit[0],NULL,screen_info->columns/2,screen_info->y_offset_screen+1);

            go_to(1,screen_info->y_offset_screen+length+1+2);
            printf("MOVE (type 'H' or 'h' for help):");
            input=line_read();
            reset_screen(1,screen_info->y_offset_screen+length+1+2,screen_info->columns,1);
            y = test_input(input,width,length);
            if (y>=0)
            {
                check_hit=hit_check(width,length,map[1],map_hit[0],*input,y,&fleet_array[1], screen_info);
                map_print(width,length,map_hit[0],NULL,screen_info->columns/2,screen_info->y_offset_screen+1);
                //continue_button(length + 5);
                delay();
                if (check_hit == SHOT_HIT)
                    break;
            }
            else if(y==INPUT_HELP)
            {
                instructions_game(screen_info);
            }
            free(input);
            if (fleet_array[1].number_of_ships_in_play==0)
            {
                break;
            }
        }
        if (fleet_array[1].number_of_ships_in_play==0)
        {
            end_screen("PLAYER","COMPUTER",0,fleet_array,screen_info);
            break;
        }

        go_to(1,screen_info->y_offset_screen);
        printf("COMPUTER:");
        automatic_shots(width,length,map[0],map_hit[1],&fleet_array[0],&information_automatic, screen_info);

        map_print(width,length,map_hit[1],NULL,screen_info->columns/2,screen_info->y_offset_screen+1);

        delay();

        if (fleet_array[0].number_of_ships_in_play==0)
        {
            end_screen("COMPUTER","PLAYER",1,fleet_array,screen_info);
            break;
        }
    }
    continue_button(length + 5);

}

/*
FUNKCIA: game_Player_vs_Player(int width, int length, char map[NUMBER_OF_PLAYERS][length][width],
                    char map_hit[NUMBER_OF_PLAYERS][length][width], FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map - trojrozmerné pole, udáva pole hracích ploch všetkých hráčov, kde sú umiestnené lode, každého hráča;
map hit - trojrozmerné pole, udáva pole hracích ploch všetkých hráčov, kde sú zaznačené zásahy a miss;
fleet_array[] - pole štruktúry FLEET_STATUS, udáva všetky informácie o stave flotily pre každého hráča;
screen_info - pointer na štruktúru SCREEN_SIZE, štruktúra SCREEN_SIZE udáva informácie o veľkosti konzoly (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia symuluje hru Battleships medzi dvoma hráčmi;

////// Výstup /////
Funkcia nič nevracia;
*/

void game_Player_vs_Player(int width, int length, char map[NUMBER_OF_PLAYERS][length][width], char map_hit[NUMBER_OF_PLAYERS][length][width],
                           FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info)
{
    int move_done;

    while(1)
    {
        for (int i=0; i<NUMBER_OF_PLAYERS; i++)
        {
            while (1)
            {
                show_entity_console(i, "PLAYER", width, length, map, map_hit, screen_info);
                move_done = one_move(i, width, length, map, map_hit, fleet_array, screen_info);

                if (move_done == SHOT_MISS)
                {
                    break;
                }
                else if (move_done == SHOT_HIT) {
                    if(check_winner(i, "PLAYER",fleet_array,screen_info)){
                        return;
                    }
                }
                delay();
            }
            delay();
        }
    }
}

void show_entity_console(int entity, char* entityText, int width, int length, char map[NUMBER_OF_PLAYERS][length][width], char map_hit[NUMBER_OF_PLAYERS][length][width],
                  SCREEN_SIZE* screen_info)
{
    reset_screen(1, screen_info->y_offset_screen, screen_info->columns, length);
    go_to(1, screen_info->y_offset_screen);
    printf("%s %d:", entityText, entity + 1);
    map_print(width, length, map[entity], map_hit[entity == 0 ? 1 : 0], 1, screen_info->y_offset_screen + 1);
    map_print(width, length, map_hit[entity], NULL, screen_info->columns / 2, screen_info->y_offset_screen + 1);
}

int one_move(int player,int width, int length, char map[NUMBER_OF_PLAYERS][length][width], char map_hit[NUMBER_OF_PLAYERS][length][width],
            FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info) {
    char* input;
    int y;
    int check_hit;

    go_to(1,screen_info->y_offset_screen+length+1+2);
    printf("MOVE (type 'H' or 'h' for help):");
    input=line_read();
    reset_screen(1,screen_info->y_offset_screen+length+1+2,screen_info->columns,1);

    y = test_input(input,width,length);

    if(y==INPUT_HELP)
    {
        instructions_game(screen_info);
        free(input);
        return 0;
    }

    if (y>=0)
    {
        int playerIndex = (player == 0 ? 1 : 0);
        check_hit=hit_check(width,length,map[playerIndex],map_hit[player],*input,y,
                            &fleet_array[playerIndex], screen_info);

        map_print(width,length,map_hit[player],NULL,screen_info->columns/2,screen_info->y_offset_screen+1);
        delay();

        free(input);
        return check_hit;
    }

    free(input);
    return 0;
}

int check_winner(int player, char* playerName, FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info) {
    int playerIndex = (player == 0 ? 1 : 0);
    if (fleet_array[playerIndex].number_of_ships_in_play==0)
    {
        char string_w[MAX_CHARACTER_END];
        sprintf(string_w,"%s %d", playerName, player + 1);

        char string_l[MAX_CHARACTER_END];
        sprintf(string_l,"%s %d",playerName, playerIndex + 1);

        end_screen(string_w,string_l,player,fleet_array,screen_info);
        delay();

        return 1;
    }
    return 0;
}

/*
FUNKCIA: game_Computer_vs_Computer(int width, int length, char map[NUMBER_OF_PLAYERS][length][width],
                   char map_hit[NUMBER_OF_PLAYERS][length][width], FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map - trojrozmerné pole, udáva pole hracích ploch všetkých hráčov, kde sú umiestnené lode, každého hráča;
map hit - trojrozmerné pole, udáva pole hracích ploch všetkých hráčov, kde sú zaznačené zásahy a miss;
fleet_array[] - pole štruktúry FLEET_STATUS, udáva všetky informácie o stave flotily pre každého hráča;
screen_info - pointer na štruktúru SCREEN_SIZE, štruktúra SCREEN_SIZE udáva informácie o veľkosti konzoly (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia symuluje hru Battleships medzi dvoma počítačmi;

////// Výstup /////
Funkcia nič nevracia;
*/

void game_Computer_vs_Computer(int width, int length, char map[NUMBER_OF_PLAYERS][length][width], char map_hit[NUMBER_OF_PLAYERS][length][width],
                               FLEET_STATUS fleet_array[], SCREEN_SIZE* screen_info)
{
    AUTO_HIT automatic_array[NUMBER_OF_PLAYERS];
    int b = 0;
    for (int j = 0; j < NUMBER_OF_PLAYERS; j++)
        automatic_array[j].mode=0;


    while (1) {
        for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {

            show_entity_console(i, "COMPUTER", width, length, map, map_hit, screen_info);

            int index = (i == 0 ? 1 : 0);
            automatic_shots(width, length, map[index], map_hit[i], &fleet_array[index],
                            &automatic_array[i], screen_info);

            map_print(width, length, map_hit[i], NULL, screen_info->columns/2,screen_info->y_offset_screen+1);

            if(check_winner(i,"COMPUTER",fleet_array,screen_info)){
                return;
            }
            delay();
        }
        delay();
    }
}

/*
FUNKCIA: map_generating(int width, int length, char map[length][width])

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map - dvojrozmerné pole, udáva hraciu plochu, do ktorej hráč vloží svoje lode;

////// Čo robí funkcia /////
Funkcia na každé miesto dvojrozmerného poľa map symbol vody;

////// Výstup //////
Funkcia nič nevracia;
 */

void map_generating(int width, int length, char map[length][width])
{
    for (int i=0; i<length; i++)
    {
        for (int j=0; j<width; j++)
        {
            map[i][j]=WATER;
        }
    }
}

/*
FUNKCIA: map_print(int width, int length, char map[length][width], char map_hit[length][width],  int x_coord, int y_coord)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map - dvojrozmerné pole, udáva hraciu plochu, v ktorej je umiestnenie lodí hráča;
map hit - dvojrozmerné pole, udáva hraciu plochu, do ktorej sa zaznačujú zásahy a miss;
x_coord - celé kladné čislo, udáva x_súradnicu kde sa ma začat výpis;
y_coord - celé kladné číslo, udáva y_súradnicu kde sa ma začat výpis;

////// Čo robí funkcia /////
Funkcia vypíše dvojrozerné pole map do súradníc (x_coord, y_coord);

////// Výstup /////
Funkcia nič nevracia;
*/

void map_print(int width, int length, char map[length][width], char map_hit[length][width],  int x_coord, int y_coord)
{
    go_to(x_coord,y_coord);
    printf("   ");

    for (int k=0; k<width; k++)
        printf(" %c ",65+k);  // 65 = ASCII kód pre 'A'

    for (int i=0; i<length; i++)
    {
        go_to(x_coord,y_coord+1+i);

        white();
        printf("%2d ",i);
        for (int j=0; j<width; j++)
        {
            if (map[i][j]==WATER)
                blue();
            else if (map[i][j]==HIT_NOW)
            {
                bright_red();
                map[i][j]=HIT;
                printf(" %c ",map[i][j]);
                delay();
                go_to(x_coord+(j+1)*3,y_coord+1+i);
                red();
            }
            else if (map[i][j]==MISS_NOW)
            {
                bright_red();
                map[i][j]=MISS;
                printf(" %c ",map[i][j]);
                delay();
                go_to(x_coord+(j+1)*3,y_coord+1+i);
                yellow();
            }
            else if (map[i][j]==HIT)
                red();
            else if (map[i][j]==MISS)
                yellow();
            else if (map_hit != NULL && map_hit[i][j]!=MISS && map_hit[i][j] !=WATER) // pridane map_hit != NULL
                orange();
            else
                green();

            printf(" %c ",map[i][j]);
        }
    }
    white();

}

/*
FUNKCIA: change_character_map(int x_coordinates, int y_coordinates, int width, int length, char map[length][width],
                                                                                    char new_symbol, char old_symbol)

////// Vstup //////
x_coordinates - celé kladné číslo, udáva x súradnicu symbola, ktorý sa ma vymeniť;
y_coordinates - celé kladné číslo, udáva y súradnicu symbola, ktorý sa ma vymeniť;
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map - dvojrozmerné pole, udáva hraciu plochu, v ktorej je umiestnenie lodí hráča;
new_symbol - znak, udáva symbol, za ktorý sa má vymeniť old_symbol;
old_symbol - znak, udáva symbol, ktroý má byť vymenený;

////// Čo robí funkcia /////
Funkcia pozrie či sa na daných súradniciach (x_coordinates,y_coordinates) nachádza starý symbol (old_symbol),
ak ÁNO: vymení ho za nový symbol (new_symbol);

////// Výstup /////
Funkcia vracia celé kladné číslo:
0: ak vymenil symboli;
1: ak nevymenil symboli;
*/

int change_character_map(int x_coordinates, int y_coordinates, int width, int length, char map[length][width],char new_symbol, char old_symbol)
{
    if (map[y_coordinates][x_coordinates]==old_symbol || new_symbol==WATER)
    {
        map[y_coordinates][x_coordinates] = new_symbol;
        return CHARACTER_CHANGED;
    }
    return CHARACTER_NOT_CHANGED;
}

/*
FUNKCIA: ship_place(int ship_length, int rotate, int x_coordinates, int y_coordinates, int width, int length, char map[length][width],
               SHIP_STATUS* ship)

////// Vstup //////
ship_length - celé kladné číslo, udáva dĺžku lode;
rotate - celé kladné číslo (0 alebo 1), udáva rotáciu vertikálne/ horizontalne umiestnenie lode;
x_coordinates - celé kladné číslo, udáva x súradnicu, kde sa umiestni loď;
y_coordinates - celé kladné číslo, udáva y súradnicu, kde sa umiestni loď;
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map - dvojrozmerné pole, udáva hraciu plochu, do ktorej bude umiestnena loď hráča;
ship - pointer na štruktúru SHIP_STATUS, štruktúra SHIP_STATUS udáva všetky informácie o lodi (pozri typedef hore)

////// Čo robí funkcia /////
Funkcia umiestni pomocou súradníc (x_coordinates, y_coordinates) a rotácie loď do dvojrozmerného poľa map;

////// Výstup /////
Funkcia vráti celé kladné číslo,
0: ak sa poodarilo umiestniť loď;
1: ak sa nepodarilo umiestniť loď;
*/


int ship_place(int ship_length, int rotate, int x_coordinates, int y_coordinates, int width, int length, char map[length][width],
               SHIP_STATUS* ship)
{
    int i=1;
    int x_active;
    int y_active;
    char start_ship_symbol;
    char mid_ship_symbol;
    char end_ship_symbol;

    if (rotate == VERTICAL_ROTATION) {
        start_ship_symbol = SHIP_VERTICAL_START;
        mid_ship_symbol = SHIP_VERTICAL_MID;
        end_ship_symbol = SHIP_VERTICAL_END;
        x_active = 0;
        y_active = 1;
    }
    else {
        start_ship_symbol = SHIP_HORIZONTAL_START;
        mid_ship_symbol = SHIP_HORIZONTAL_MID;
        end_ship_symbol = SHIP_HORIZONTAL_END;
        x_active = 1;
        y_active = 0;
    }

    if(x_active * (x_coordinates + ship_length) >= width || y_active * (y_coordinates + ship_length) >= length){
        return SHIP_NOT_PLACED;
    }

    int status = change_character_map(x_coordinates,y_coordinates,width,length,map,start_ship_symbol,WATER);

    for (; i < ship_length-1 && status==CHARACTER_CHANGED; i++)
    {
        status = change_character_map(x_coordinates + (i * x_active),y_coordinates + (i * y_active),width,length,map,mid_ship_symbol,WATER);
    }

    if (status == CHARACTER_CHANGED)
        status = change_character_map(x_coordinates + (i * x_active),y_coordinates + (i * y_active),width,length,map,end_ship_symbol,WATER);
    else
        i--;

    if (status == CHARACTER_NOT_CHANGED)
    {
        for (int j = 0; j < i; j++)
            change_character_map(x_coordinates + (j * x_active),y_coordinates + (j * y_active),width,length,map,WATER,WATER);
        return SHIP_NOT_PLACED;
    }
    else {
        place_ship_map(ship_length, rotate, x_coordinates, y_coordinates, width,length, ship, x_active, y_active);
        return SHIP_PLACED;
    }
}

void place_ship_map(int ship_length, int rotate, int x_coordinates, int y_coordinates, int width, int length,
                         SHIP_STATUS* ship, int x_active, int y_active) {

    POSITION* pos_ship=(POSITION*)malloc(ship_length*sizeof(POSITION));

    for (int k = 0; k < ship_length; k++)
    {
        pos_ship[k].x_coordinates=x_coordinates + k * x_active;
        pos_ship[k].y_coordinates=y_coordinates + k * y_active;
    }

    ship->pos=pos_ship;
    ship->not_hit=ship_length;
    ship->ship_size=ship_length;
    ship->rotation=rotate;
}



/*
FUNKCIA: instructions_game(SCREEN_SIZE* screen_info)

////// Vstup //////
screen_info - pointer na štruktúru SCREEN_SIZE, štruktúra SCREEN_SIZE udáva informácie o veľkosti konzoly (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia vypíše inštrukcie na danú pozíciu;

////// Výstup /////
Funkcia nič nevracia;
*/

void instructions_print(int x_coord, int y_coord, char text[])
{
    const char break_symbol[] = ",";
    char *ptr;

    ptr = strtok(text, break_symbol);

    go_to(x_coord,y_coord);
    printf("Instruction:");

    for (int i=1 ;ptr != NULL; i++)
    {
        go_to(x_coord,y_coord+i);
        printf("%s", ptr);
        ptr = strtok(NULL, break_symbol );
    }
}

/*
FUNKCIA: instructions_game(SCREEN_SIZE* screen_info)

////// Vstup //////
screen_info - pointer na štruktúru SCREEN_SIZE, štruktúra SCREEN_SIZE udáva informácie o veľkosti konzoly (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia vypíše inštrukcie pre hru;

////// Výstup /////
Funkcia nič nevracia;
*/

void instructions_game(SCREEN_SIZE* screen_info)
{
    char info[]="Type coordinates where you want to shoot like this: A0 or a0,When hit is not registered,you typed incorrect coordinates try again";


    reset_screen(1,screen_info->y_offset_screen+1,screen_info->columns,
                 screen_info->rows-screen_info->y_offset_screen-1);

    instructions_print(1,screen_info->y_offset_screen+1, info);

    continue_button(screen_info->y_offset_screen+6);

    reset_screen(1,screen_info->y_offset_screen+1,screen_info->columns,
                 screen_info->rows-screen_info->y_offset_screen-1);
}

/*
FUNKCIA: instructions_fleet_placement(int width, SCREEN_SIZE* screen_info)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
screen_info - pointer na štruktúru SCREEN_SIZE, štruktúra SCREEN_SIZE udáva informácie o veľkosti konzoly (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia vypíše inštrukcie pre umiestnenie lodí;

////// Výstup /////
Funkcia nič nevracia;
*/

void instructions_fleet_placement(int width, SCREEN_SIZE* screen_info)
{

    char info[]="Type 'A' or 'a' to automatic place ships,(ships will be placed by computer),"
                "Type 'R' or 'r' to rotate ship,Type coordinates where you want,place ship like this: A0,Start of the ship is symbol '<' or '^',"
                "is painted red,Type coordinates where start of ship should be,When ship is not placed,you typed incorrect coordinates try again";

    int x = (width +1)*3 + 5*3 +5+5+3;

    if (x + 50 >= screen_info->columns) //50 = longest line
        x=1;

    reset_screen(x,screen_info->y_offset_screen+1,screen_info->columns - x,
                 screen_info->rows-screen_info->y_offset_screen-1);

    instructions_print(x,screen_info->y_offset_screen+1, info);
    continue_button(screen_info->y_offset_screen+14); // toto dorobiť;

    reset_screen(x,screen_info->y_offset_screen+1,screen_info->columns - x,
                 screen_info->rows-screen_info->y_offset_screen-1);
}

/*
FUNKCIA: fleet_placement(int width, int length, char map[length][width], FLEET_STATUS* fleet_array, int automatic_placement,
                                                                                            SCREEN_SIZE* screen_info)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map - dvojrozmerné pole, udáva hraciu plochu, do ktorej budú dané lode hráča;
fleet_array - pointer na štruktúru FLEET_STATUS, štruktúra FLEET_STATUS udáva všetky informácie o stave flotily hráča (pozri typedef hore);
int automatic_placement - celé kladné číslo (0 alebo 1), udáva či bude lode umiesni hráč alebo počítač;
screen_info - pointer na štruktúru SCREEN_SIZE, štruktúra SCREEN_SIZE udáva informácie o veľkosti konzoly (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia do dvojrozmerného poľa map umiesni lode, buď si hráč vyberie umiestnenie lodí sám (automatic_placement je 0)
alebo lode umiestni počítač (automatic_placement je 1);

////// Výstup /////
Funkcia nič nevracia;
*/


void fleet_placement(int width, int length, char map[length][width], FLEET_STATUS* fleet_array, int automatic_placement,
                     SCREEN_SIZE* screen_info)
{
    int status;
    int ship_size[]={5,4,3,3,2};
    int rotate;
    int y;

    char* input;
    int in;

    SHIP_STATUS *ship_array=malloc(NUMBER_OF_SHIPS*sizeof (SHIP_STATUS));
    fleet_array->number_of_ships_in_play=NUMBER_OF_SHIPS;


    for (int i=0; i<NUMBER_OF_SHIPS; i++)
    {
        status=SHIP_NOT_PLACED;
        rotate=VERTICAL_ROTATION;
        while(status==SHIP_NOT_PLACED)
        {

            if (automatic_placement==AUTOMATIC_PLACEMENT_DISABLED){
                rotate_graphics(rotate, ship_size[i], width, length, screen_info->y_offset_screen);
                go_to(1,screen_info->y_offset_screen+length+1+2);
                printf("MOVE (type 'H' or 'h' for help):");
                input=line_read();

                reset_screen(1,screen_info->y_offset_screen+length+1+2,screen_info->columns,1);
                y = test_input(input,width,length);

                if (y!=INPUT_ERROR)
                {
                    if (y==INPUT_ROTATION)
                    {
                        if (rotate==VERTICAL_ROTATION)
                            rotate=HORIZONTAL_ROTATION;
                        else
                            rotate=VERTICAL_ROTATION;
                    }
                    else if (y==INPUT_AUTOMATIC_PLACEMENT_ENABLED)
                    {
                        automatic_placement=AUTOMATIC_PLACEMENT_ENABLED;
                    }
                    else if (y==INPUT_HELP)
                    {
                        instructions_fleet_placement(width,screen_info);
                    }
                    else if (y>=0)
                        status=ship_place(ship_size[i],rotate,input[0],y,width, length, map,&ship_array[i]);
                }
                free(input);
                reset_screen(1,screen_info->y_offset_screen+1,screen_info->columns,length);
                map_print(width,length,map,NULL,1,screen_info->y_offset_screen+1);
            }
            else
            {
                rotate=random_number(0,1);
                in= random_number(0,width-1);
                y= random_number(0,length-1);

                status=ship_place(ship_size[i],rotate,in,y,width, length, map,&ship_array[i]);
            }
        }
        fleet_array->fleet=ship_array;
    }
}

/*
FUNKCIA: line_read()

////// Vstup //////
Funkcia nemá žiadne vstupy;

////// Čo robí funkcia /////
Funkcia načítava vstup (reťazec) z consoly po riadku;

////// Výstup /////
Funkcia vracia pointer na načítaný reťazec;
*/

char* line_read()
{
    int size=LINE_BUFFER;
    char* input=malloc(size*sizeof(char));
    size_t index=0;
    char* output= malloc(size*sizeof(char));
    size_t size_string=0;

    while(fgets(input,size,stdin)!=0)
    {

        if (size_string+ strlen((input))>=size)
        {
            size +=LINE_BUFFER;
            output= (char*)realloc(output,(size+size_string)*sizeof(char));
        }

        strcpy(output+index,input);
        index=index+ strlen(input);
        size_string+=strlen(input);

        if (strchr(input,'\n')!=NULL)
        {
            free(input);
            break;
        }
        free(input);
        input = (char*) malloc(size*sizeof(char));
    }
    return output;
}

/*
FUNKCIA: test_input(char* input_string, int width, int length)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
input_string - pointer na reťazec, ktorý je vstupom od hráča;

////// Čo robí funkcia /////
Funkcia kontroluje či vstup(input_string) spľňa dané kritéria;

////// Výstup /////
Funkcia vracia celé číslo,
-1: chybný vstup;
-2, rotacia lode;
-3, automaticke ukladanie lode;
-4; help;
alebo celé kladné číslo, ktoré reprezentuje y súradnicu;
*/

int test_input(char *input_string, int width, int length)
{
    int number_of_characters= counter(input_string);
    int y_coordinate;

    if (number_of_characters==0)
        return INPUT_ERROR;
    else if (number_of_characters==1)
    {
        if (*(input_string)=='R' || *(input_string)=='r')
            return INPUT_ROTATION;
        else if (*(input_string)=='A' || *(input_string)=='a')
            return INPUT_AUTOMATIC_PLACEMENT_ENABLED;
        else if (*(input_string)=='H' || *(input_string)=='h')
            return INPUT_HELP;
        else
            return INPUT_ERROR;
    }
    else
    {
        if (*input_string>=97)
            *input_string = *input_string - 97;
        else
            *input_string = *input_string - 65;


        int check_integer = sscanf(input_string + 1, "%d", &y_coordinate);
        if (*input_string>=0 && *input_string<width && check_integer>0)
        {
            if (y_coordinate >= 0 && y_coordinate < length)
                return y_coordinate;
            else
                return INPUT_ERROR;
        }
        else
            return INPUT_ERROR;
    }
}

/*
FUNKCIA: int counter(char* input_string)

////// Vstup //////
input_string - pointer na reťazec, ktorý je vstupom od hráča;

////// Čo robí funkcia /////
Funkcia počíta veľkosť reťazca po '\n';

////// Výstup /////
Funkcia vracia veľkosť reťazca;
*/

int counter(char* input_string)
{
    int counter=0;

    for (;*(input_string+counter)!='\n';counter++);

    return counter;
}

/*
FUNKCIA: rotate_graphics (int rotate, int ship_length, int width, int length, int y_offset)

////// Vstup //////
rotate - celé kladné číslo (0 alebo 1), udáva rotáciu: vertikálne/ horizontalne umiestnenie lode;
ship_length - celé kladné číslo, udáva dĺžku lode;
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
y_offset - celé kladné číslo, udáva offset od začiatku konzoly

////// Čo robí funkcia /////
Funkcia vypíše grafickú reprezentáciu lode v správnom smere buď vertikálne alebo horizontálne;

////// Výstup /////
Funkcia nič nevracia;
*/

void rotate_graphics (int rotate, int ship_length, int width, int length, int y_offset)
{
    int x_coordinates= (width+1)*3 + 5;
    int y_coordinates;

    if (rotate==0)
    {
        y_coordinates = y_offset+(length-ship_length+2)/2;
        go_to(x_coordinates, y_coordinates);
        line(5);
        go_to(x_coordinates, y_coordinates + 1);

        printf("| ");
        red();
        printf("^");
        white();
        printf(" |");

        int j=0;
        for (; j<ship_length-2;j++)
        {
            go_to(x_coordinates, y_coordinates + 2 + j);
            printf("| | |");
        }
        go_to(x_coordinates, y_coordinates + 2 + j);
        printf("| v |");
        go_to(x_coordinates, y_coordinates + 3 + j);
        line(5);
    }
    else {
        y_coordinates = y_offset+(length/2);

        go_to(x_coordinates, y_coordinates);
        line(ship_length + 6 + (ship_length - 2) * 2);
        go_to(x_coordinates, y_coordinates + 1);
        //printf("| < ");
        printf("| ");
        red();
        printf("< ");
        white();


        go_to(x_coordinates + 4, y_coordinates + 1);
        int j=0;
        for (; j < ship_length - 2; j++)
        {
            printf(" = ");
        }
        go_to(x_coordinates + 4 + j * 3, y_coordinates + 1);
        printf(" > | \n");
        go_to(x_coordinates, y_coordinates + 2);
        line(ship_length + 6 + (ship_length-2)*2);
    }
}

/*
FUNKCIA: line(int number)

////// Vstup //////
number - celé kladné číslo, udáva koľko krát sa ma symbol vypísať za sebou v riadku;

////// Čo robí funkcia /////
Funkcia vypisuje symbol za sebou do riadku;

////// Výstup /////
Funkcia nič nevracia;
*/

void line(int number)
{
    for (int i = 0; i<number; i++)
        printf("%c",LINE_SYMBOL);
}

/*
FUNKCIA: go_to(int x_coord, int y_coord)

////// Vstup //////
x_coord - celé kladné číslo, ktoré udáva x súradnicu;
y_coord - celé kladné číslo, ktoré udáva y súradnicu;

////// Čo robí funkcia /////
Funkcia nastaví kurzor na konzole na súradnice (x_coord, y_coord);

////// Výstup /////
Funkcia nič nevracia;
*/

void go_to(int x_coord, int y_coord)
{
    char x[15];
    sprintf(x,"\033[%d;%dH",y_coord,x_coord);
    printf(x);

}


/*
FUNKCIA: reset_screen(int x, int y, int width, int length)

////// Vstup //////
x - celé kladné číslo, udáva x súradnicu začatia mazania;
y - celé kladné číslo, udáva y súradnicu začatia mazania;
width - celé kladné čislo, udáva šírku mazania;
length - celé kladné číslo, udáva dĺžku mazania;

////// Čo robí funkcia /////
Funkcia ide na súradnice (x,y) a zmaže obrazovku v rozsahu šírky a dĺžky;

////// Výstup /////
Funkcia nič nevracia;
*/

void reset_screen(int x, int y, int width, int length)
{
    for (int j=0; j<length; j++)
    {
        go_to(x,y+j);
        for (int i=0; i<width; i++)
            printf(" ");
    }
    go_to(x,y);
}

/*
FUNKCIA: hit_check(int width, int length, char map_to_hit[length][width], char map_to_show[length][width], int x_coord_hit,
              int y_coord_hit, FLEET_STATUS* fleet_a, SCREEN_SIZE* screen_info)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map_to_hit - dvojrozmerné pole, udáva hraciu plochu, v ktorej sú dané lode hráča;
map_to_show - dvojrozmerné pole, udáva hraciu plochu, do ktorej sa označujú zásahy a miss;
x_coord_hit - celé kladné číslo(od 0 po (width-1)), udáva x súradnicu strely;
y_coord_hit - celé kladné číslo(od 0 po (length-1)), udáva y súradnicu strely;
fleet_a - pointer na štruktúru FLEET_STATUS, štruktúra FLEET_STATUS udáva všetky informácie o flotile hráča (pozri typedef hore);
screen_info - pointer na štruktúru SCREEN_SIZE, štruktúra SCREEN_SIZE udáva informácie o veľkosti konzoly (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia pozrie či strela zasiahla loď,
ak ÁNO: zmení symbol vody na symbol zásah v map_to_show;
        dalej pozrie či loď nie je zničená,
        ak ÁNO: zavolá explosion_graphics (ukáže loď) a zníži počet lodí o 1;
ak NIE: zmení symbol vody na symbol miss v map_to_show;

////// Výstup /////
Funkcia vracia celé kladné číslo,
0: zásah;
1: miss;
2: políčko už bolo vybraté predtým;
*/

int hit_check(int width, int length, char map_to_hit[length][width], char map_to_show[length][width], int x_coord_hit,
              int y_coord_hit, FLEET_STATUS* fleet_a, SCREEN_SIZE* screen_info)
{
    if (map_to_hit[y_coord_hit][x_coord_hit]==WATER)
    {
        if (map_to_show[y_coord_hit][x_coord_hit]==MISS)
            return SHOT_OCCUPIED;
        else
        {
            map_to_show[y_coord_hit][x_coord_hit]=MISS_NOW;
            fleet_a->miss++;
            return SHOT_MISS;
        }
    }
    else
    {
        for (int i=0; i<NUMBER_OF_SHIPS; i++)
        {
            for (int k = 0; k<fleet_a->fleet[i].ship_size; k++)
            {
                if (fleet_a->fleet[i].pos[k].x_coordinates == x_coord_hit && fleet_a->fleet[i].pos[k].y_coordinates == y_coord_hit)
                {
                    if (map_to_show[y_coord_hit][x_coord_hit]!=HIT)
                    {
                        fleet_a->hit++;
                        map_to_show[y_coord_hit][x_coord_hit]=HIT_NOW;
                        fleet_a->fleet[i].not_hit = fleet_a->fleet[i].not_hit - 1;
                        if (fleet_a->fleet[i].not_hit == SHIP_DESTROYED)
                        {
                            map_print(width, length, map_to_show, NULL, screen_info->columns/2, screen_info->y_offset_screen+1);
                            explosion_graphics(&fleet_a->fleet[i], width, length, map_to_show);
                            fleet_a->fleet[i].ship_size = 0;
                            fleet_a->number_of_ships_in_play--;
                        }
                        return SHOT_HIT;
                    }
                    return SHOT_HIT;
                }
            }
        }
        return SHOT_OCCUPIED;
    }
}

/*
FUNKCIA: explosion_graphics(SHIP_STATUS* ship_explosion, int width, int length, char ship_hit_map[length][width])

////// Vstup //////
ship_explosion - pointer na štruktúru SHIP_STATUS, štruktúra SHIP_STATUS udáva všetky informácie o lodi (pozri typedef hore);
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
ship_hit_map - dvojrozmerné pole, udáva hraciu plochu, do ktorej sa zaznačujú zásahy a miss;

////// Čo robí funkcia /////
Funkcia do dvojrozmerného poľa ship_hit_map zapíše postupne symboli, ktoré budú reprezentovať zničenú loď;

////// Výstup /////
Funkcia nič nevracia;
*/

void explosion_graphics(SHIP_STATUS* ship_explosion, int width, int length, char ship_hit_map[length][width])
{
    if (ship_explosion->rotation==0)
    {
        ship_hit_map[ship_explosion->pos[0].y_coordinates][ship_explosion->pos[0].x_coordinates]=SHIP_VERTICAL_START;
        int k = 1;
        for (; k < ship_explosion->ship_size-1; k++)
        {
            ship_hit_map[ship_explosion->pos[0].y_coordinates+k][ship_explosion->pos[0].x_coordinates]=SHIP_VERTICAL_MID;
        }
        ship_hit_map[ship_explosion->pos[0].y_coordinates+k][ship_explosion->pos[0].x_coordinates]=SHIP_VERTICAL_END;
    }
    else
    {
        ship_hit_map[ship_explosion->pos[0].y_coordinates][ship_explosion->pos[0].x_coordinates]=SHIP_HORIZONTAL_START;
        int k = 1;
        for (; k < ship_explosion->ship_size-1; k++)
        {
            ship_hit_map[ship_explosion->pos[0].y_coordinates][ship_explosion->pos[0].x_coordinates+k]=SHIP_HORIZONTAL_MID;
        }
        ship_hit_map[ship_explosion->pos[0].y_coordinates][ship_explosion->pos[0].x_coordinates+k]=SHIP_HORIZONTAL_END;
    }
}

/*
FUNKCIA: red()

////// Vstup //////
Funkcia nemá žiadne vstupy;

////// Čo robí funkcia /////
Funkcia nastaví farbu písma na červenú;

////// Výstup /////
Funkcia nič nevracia;
*/

void red()
{
    printf("\033[31m");
}

/*
FUNKCIA: green()

////// Vstup //////
Funkcia nemá žiadne vstupy;

////// Čo robí funkcia /////
Funkcia nastaví farbu písma na zelenú;

////// Výstup /////
Funkcia nič nevracia;
*/

void green()
{
    printf("\033[92m");
}

/*
FUNKCIA: blue()

////// Vstup //////
Funkcia nemá žiadne vstupy;

////// Čo robí funkcia /////
Funkcia nastaví farbu písma na modrú;

////// Výstup /////
Funkcia nič nevracia;
*/

void blue()
{
    printf("\033[94m");
}

/*
FUNKCIA: white()

////// Vstup //////
Funkcia nemá žiadne vstupy;

////// Čo robí funkcia /////
Funkcia nastaví farbu písma na bielu;

////// Výstup /////
Funkcia nič nevracia;
*/

void white()
{
    printf("\033[37m");
}

/*
FUNKCIA: yellow()

////// Vstup //////
Funkcia nemá žiadne vstupy;

////// Čo robí funkcia /////
Funkcia nastaví farbu písma na žltú;

////// Výstup /////
Funkcia nič nevracia;
*/

void yellow()
{
    printf("\033[93m");
}

/*
FUNKCIA: bright_red()

////// Vstup //////
Funkcia nemá žiadne vstupy;

////// Čo robí funkcia /////
Funkcia nastaví farbu písma na svetlo červenú

////// Výstup /////
Funkcia nič nevracia;
*/

void bright_red()
{
    printf("\033[97m");
}

/*
FUNKCIA: orange()

////// Vstup //////
Funkcia nemá žiadne vstupy;

////// Čo robí funkcia /////
Funkcia nastaví farbu písma na oranžovú

////// Výstup /////
Funkcia nič nevracia;
*/

void orange()
{
    printf("\033[91m");
}

/*
FUNKCIA: random_initialization()

////// Vstup //////
Funkcia nemá žiadne vstupy;

////// Čo robí funkcia /////
Funkcia vygeneruje seed pre rand() funkciu podľa aktuálneho času;

////// Výstup /////
Funkcia nič nevracia
*/


void random_initialization()
{
    srand(time(NULL));
}

/*
FUNKCIA: random_number(int down, int up)

////// Vstup //////
down - celé číslo, udáva spodnú hranicu generovania čísla;
up - celé číslo, udáva hornú hranicu generovania čísla;
podmienka: down <= up

////// Čo robí funkcia /////
Funkcia vygeneruje náhodné číslo v rozsahu <down, up>

////// Výstup /////
Funkcia vracia vygenerované číslo (celé číslo);
*/

int random_number(int down, int up)
{
    return (rand() % (up - down + 1)) + down;
}

/*
FUNKCIA: delay()

////// Vstup //////
Funkcia nemá žiadne vstupy;

////// Čo robí funkcia /////
Funkcia pozastaví program na čas daný v DELAY_TIME;

////// Výstup /////
Funkcia nič nevracia;
*/

void delay()
{
    fflush(stdin);
#ifdef _WIN32
    Sleep(DELAY_TIME);
#else
    usleep(1000*DELAY_TIME);  //viem, že tie printy su navyše a že som to mohol definovať hore, ale ako samostatne usleep() mi to nešlo, nwm prečo
    printf("\n");
    fflush(stdin);
#endif
}

/*
FUNKCIA:  shot_fired(int width, int length, char map[length][width], char map_hit[length][width], AUTO_HIT* auto_info, FLEET_STATUS* fleet,
                                                                                                        int x_offset, int y_offset, SCREEN_SIZE* screen_info)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map - dvojrozmerné pole, udáva hraciu plochu, v ktorej sú dané lode hráča;;
map hit - dvojrozmerné pole, udáva hraciu plochu, do ktorej sa zaznačujú zásahy a miss;
auto_info - pointer na štruktúru AUTO_HIT, štruktúra AUTO_HIT udáva informácie pre automatickú strelbu (pozri typedef hore);
fleet - pointer na štruktúru FLEET_STATUS, štruktúra FLEET_STATUS udáva všetky informácie o flotile hráča (pozri typedef hore);
x_offset - celé čislo, udáva offset od x súradnice;
y_offset - celé číslo, udáva offset od y súradnice;
screen_info - pointer na štruktúru SCREEN_SIZE, štruktúra SCREEN_SIZE udáva informácie o veľkosti konzoly (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia vypáli strelu;

////// Výstup /////
Funkcia vracia celé kladné číslo;
0: ak bola zničená loď;
1: ak strela trafila už označený bod;
2: ak strela minula loď;
*/

int shot_fired(int width, int length, char map[length][width], char map_hit[length][width], AUTO_HIT* auto_info, FLEET_STATUS* fleet,
               int x_offset, int y_offset, SCREEN_SIZE* screen_info)
{
    int check_hit=SHOT_HIT;

    int x=x_offset;
    int y=y_offset;
    int number_of_ship=fleet->number_of_ships_in_play;

    while(check_hit == SHOT_HIT)
    {
        if ((auto_info->pos_A.x_coordinates + x < width && auto_info->pos_A.x_coordinates + x >=0) &&
            (auto_info->pos_A.y_coordinates + y < length && auto_info->pos_A.y_coordinates + y >=0))
        {
            check_hit = hit_check(width, length, map, map_hit, auto_info->pos_A.x_coordinates + x,
                                  auto_info->pos_A.y_coordinates+y, fleet, screen_info);
            if (check_hit==SHOT_HIT)
                map_print(width,length,map_hit,NULL,screen_info->columns/2,screen_info->y_offset_screen+1);
            delay();
            x+=x_offset;
            y+=y_offset;
        }
        else
            break;

        if (number_of_ship - fleet->number_of_ships_in_play == 1) {
            auto_info->mode = AUTO_RESET;
            return SHIP_DESTROYED;
        }
    }
    switch(auto_info->mode)
    {
        case X_POSITIVE:
            auto_info->mode=X_NEGATIVE;
            break;
        case X_NEGATIVE:
            auto_info->mode=Y_POSITIVE;
            break;
        case Y_POSITIVE:
            auto_info->mode=Y_NEGATIVE;
            break;
        case Y_NEGATIVE:
            auto_info->mode=AUTO_RESET;
            break;
        default:
            auto_info->mode=X_POSITIVE;
            break;
    }
    if (check_hit==SHOT_MISS)
        return SHIP_NOT_DESTROYED;
    else
        return SHIP_OCCUPIED;
}

/*
FUNKCIA: automatic_shots(int width, int length, char map[length][width], char map_hit[length][width], FLEET_STATUS* fleet, AUTO_HIT* auto_info,
                                                                                                                    SCREEN_SIZE* screen_info)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map - dvojrozmerné pole, udáva hraciu plochu, v ktorej sú dané lode hráča;;
map hit - dvojrozmerné pole, udáva hraciu plochu, do ktorej sa zaznačujú zásahy a miss;
auto_info - bude to štruktúra
fleet - pointer na štruktúru FLEET_STATUS, štruktúra FLEET_STATUS udáva všetky informácie o flotile hráča (pozri typedef hore);
auto_info - pointer na štruktúru AUTO_HIT, štruktúra AUTO_HIT udáva informácie pre automatickú strelbu (pozri typedef hore);
screen_info - pointer na štruktúru SCREEN_SIZE, štruktúra SCREEN_SIZE udáva informácie o veľkosti konzoly (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia automatický strieľa;

////// Výstup /////
Funkcia nič nevracia;
*/

void automatic_shots(int width, int length, char map[length][width], char map_hit[length][width], FLEET_STATUS* fleet,
                     AUTO_HIT* auto_info, SCREEN_SIZE* screen_info)
{
    int check_destroy = SHIP_NOT_DESTROYED;

    while(fleet->number_of_ships_in_play > 0) {
        if (auto_info->mode == AUTO_RESET)  //nova strelba na novu lod
        {
            if (fire_on_new_ship(width,length, map, map_hit, fleet, auto_info, screen_info) == SHOT_MISS) {
                break;
            }
        }
        else
        {
            check_destroy = directional_shots(width,length, map, map_hit, fleet, auto_info, screen_info);
            if (check_destroy == SHIP_NOT_DESTROYED)
                break;
        }
        if (check_destroy== SHIP_DESTROYED)
        {
            find_coordinates_hit(width,length,map_hit,auto_info);
        }
    }
}

int fire_on_new_ship(int width, int length, char map[length][width], char map_hit[length][width], FLEET_STATUS* fleet,
                     AUTO_HIT* auto_info, SCREEN_SIZE* screen_info) {

    int x_shot = random_number(0,  width - 1);
    int y_shot = random_number(0, length - 1);

    int check_hit = hit_check(width, length, map, map_hit, x_shot, y_shot,
                          fleet, screen_info);

    if (check_hit==SHOT_HIT) {
        map_print(width, length, map_hit, NULL, screen_info->columns / 2, screen_info->y_offset_screen + 1);
        auto_info->pos_A.x_coordinates = x_shot;
        auto_info->pos_A.y_coordinates = y_shot;
        auto_info->mode = X_POSITIVE;
    }

    return check_hit;
}

int directional_shots(int width, int length, char map[length][width], char map_hit[length][width], FLEET_STATUS* fleet,
                     AUTO_HIT* auto_info, SCREEN_SIZE* screen_info) {
    switch (auto_info->mode)
    {
        case X_POSITIVE:
            return shot_fired(width,length,map, map_hit, auto_info, fleet,1,0, screen_info);
        case X_NEGATIVE:
            return shot_fired(width,length,map, map_hit, auto_info, fleet,-1,0, screen_info);
        case Y_POSITIVE:
            return shot_fired(width,length,map, map_hit, auto_info, fleet,0,1, screen_info);
        case Y_NEGATIVE:
            return shot_fired(width,length,map, map_hit, auto_info, fleet,0,-1, screen_info);
        default:
            auto_info->mode=AUTO_RESET;
            return SHIP_NOT_DESTROYED;
    }
}

/*
FUNKCIA: free_memory(FLEET_STATUS* fleet_array)

////// Vstup //////
fleet_array - pole štruktúry FLEET_STATUS, udáva všetky informácie o stave flotily pre každého hráča;

////// Čo robí funkcia /////
Funkcia dealokuje alokovanú pamäť;

////// Výstup /////
Funkcia nič nevracia;
*/

void free_memory(FLEET_STATUS* fleet_array)
{
    for(int i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        for (int j = 0; j < NUMBER_OF_SHIPS; j++)
        {
            free(fleet_array[i].fleet[j].pos);
        }
        free(fleet_array[i].fleet);
    }
}

/*
FUNKCIA: find_coordinates_hit(int width, int length, char map_hit[length][width], AUTO_HIT* auto_info)

////// Vstup //////
width - celé kladné čislo, udáva šírku hracej plochy;
length - celé kladné číslo, udáva dĺžku hracej plochy;
map hit - dvojrozmerné pole, udáva hraciu plochu, do ktorej sa zaznačujú zásahy a miss;
auto_info - pointer na štruktúru AUTO_HIT, štruktúra AUTO_HIT udáva informácie pre automatickú strelbu (pozri typedef hore);

////// Čo robí funkcia /////
Funkcia nájde súradnice zásahu;

////// Výstup /////
Funkcia nič nevracia;
*/

void find_coordinates_hit(int width, int length, char map_hit[length][width], AUTO_HIT* auto_info)
{
    for (int i = 0; i < length; i++)
    {
        for(int j = 0; j < width; j++)
        {
            if (map_hit[i][j]==HIT)
            {
                auto_info->pos_A.x_coordinates=j;
                auto_info->pos_A.y_coordinates=i;
                auto_info->mode=1;
                return;
            }
        }
    }
}
