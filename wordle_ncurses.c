
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <ctype.h>

#include <ncurses.h>

#include "common.h"


#define clrscr() clear()

#define gotoxy(x,y) do { move(y,x); refresh(); } while(0)
#define cputc(c) do { addch(c); refresh(); } while(0)
#define setcolor(c) attron(COLOR_PAIR(c))

#define YELLOW 1
#define CYAN 2
#define RED 3
#define GREEN 4
#define BLUE 5
#define WHITE 6
#define MAGENTA 7
#define BLACK 8

#define YELLOW_BACKGROUND 9
#define CYAN_BACKGROUND 10
#define RED_BACKGROUND 11
#define GREEN_BACKGROUND 12
#define BLUE_BACKGROUND 13
#define WHITE_BACKGROUND 14
#define MAGENTA_BACKGROUND 15
#define BLACK_BACKGROUND 16

#define XSize 80
#define YSize 24

char dict[MAX_DICT_SIZE][MAX_WORD_SIZE];

char attempt[MAX_WORD_SIZE];

unsigned char dict_file;

unsigned short dict_size;

unsigned short freq[VECT_SIZE];
unsigned short hint[VECT_SIZE];
unsigned short letter_found[VECT_SIZE]; 

unsigned short match_score;

unsigned short score[MAX_PLAYERS];

unsigned short wins[MAX_PLAYERS];

unsigned short total_time[MAX_PLAYERS];

unsigned short word_size;

unsigned short number_of_players;

unsigned int number_of_challenges;


// #if defined(_BACKGROUND_COLOR) && BACKGROUND_COLOR==_XL_WHITE
    // #define _NCURSES_BACKGROUND_COLOR COLOR_WHITE
    // #define _NCURSES_FOREGROUND_COLOR COLOR_BLACK

// #else
    #define _NCURSES_BACKGROUND_COLOR COLOR_BLACK
    #define _NCURSES_FOREGROUND_COLOR COLOR_WHITE

// #endif


void PRESS_ENTER_TO_CONTINUE(void)
{
    move(YSize-2,0);
    printw("Press ENTER to start");
    refresh();
    getchar();
    move(YSize-2,0);
    printw("                    ");
    refresh();
}

void printxy(uint8_t x, uint8_t y, char * str)
{
	move(y,x); 
	printw(str); 
	refresh();
}


// void printdxy(x,y,length,val)
// {
    // move(y,x);
    // printw("%0" #length "u",val);
    // refresh();
// }
    
// void cputcxy(x,y,ch) 
// {
    // move(y,x);
    // cputc(ch);
// }

void init_scrteen(void)
{
    #if defined(__ATARI_ST__)
        putenv("TERM=st52");
    #endif
	initscr();   
	// noecho();
	curs_set(0);
	start_color();
	cbreak();
    keypad(stdscr, true);
	intrflush(stdscr, TRUE);
	init_pair( 1, COLOR_YELLOW, _NCURSES_BACKGROUND_COLOR);
	init_pair( 2, COLOR_CYAN, _NCURSES_BACKGROUND_COLOR);
	init_pair( 3, COLOR_RED, _NCURSES_BACKGROUND_COLOR);
	init_pair( 4, COLOR_GREEN, _NCURSES_BACKGROUND_COLOR);
	init_pair( 5, COLOR_BLUE, _NCURSES_BACKGROUND_COLOR);
	init_pair( 6, COLOR_WHITE, _NCURSES_BACKGROUND_COLOR);
	init_pair( 7, COLOR_MAGENTA, _NCURSES_BACKGROUND_COLOR);
	init_pair( 8, COLOR_BLACK, _NCURSES_BACKGROUND_COLOR);
    
	init_pair( 9, COLOR_BLACK, COLOR_YELLOW);
	init_pair(10, _NCURSES_FOREGROUND_COLOR, COLOR_CYAN);
	init_pair(11, _NCURSES_FOREGROUND_COLOR, COLOR_RED);
	init_pair(12, COLOR_BLACK, COLOR_GREEN);
	init_pair(13, _NCURSES_FOREGROUND_COLOR, COLOR_BLUE);
	init_pair(14, COLOR_BLACK, COLOR_WHITE);
	init_pair(15, _NCURSES_FOREGROUND_COLOR, COLOR_MAGENTA);
	init_pair(16, _NCURSES_FOREGROUND_COLOR, COLOR_BLACK);
    
    wbkgd(stdscr, COLOR_PAIR(1));
    
    refresh();
}


void instructions(void)
{
    printxy(0, 6,"-----------------------------------------------------------------------------");
    printxy(0, 7,"                     INSTRUCTIONS");
    printxy(0, 8,"-----------------------------------------------------------------------------");
    
    printxy(0, 9,"Guess a secret word in max 6 attempts");
    printxy(0,10,"'-' means letter nowhere in the secret word");
    printxy(0,11,"'*' one occurence of this letter is elsewhere for each '*' on the same letter");
    printxy(0,12,"A displayed letter means that it is correct in the displayed place");
    printxy(0,13,"-----------------------------------------------------------------------------");
    printxy(0,14,"Only small letters and no diacritics");
    printxy(0,15,"Insert 'x' to give up");
    printxy(0,16,"-----------------------------------------------------------------------------");
}


void show_found_letters(void)
{
    unsigned char ch;
    unsigned char x;
    unsigned char y;
    
    move(YSize-10,2);
    
    for(ch='a';ch<='z';++ch)
    {

        if(letter_found[ch]==FOUND_IN_EXACT_PLACE)
        {
            setcolor(GREEN_BACKGROUND);
            printw("%c",ch);
            setcolor(WHITE);
            printw(" ");
        }
        else if(letter_found[ch]==FOUND_IN_WRONG_PLACE)
        {
            setcolor(YELLOW_BACKGROUND);
            printw("%c",ch);
            setcolor(WHITE);
            printw(" ");
        }
        else if(letter_found[ch]==TRIED_AND_NOT_FOUND)
        {
            setcolor(RED_BACKGROUND);
            printw("%c",ch);
            setcolor(WHITE);
            printw(" ");                
        }
        else
        {
            setcolor(WHITE_BACKGROUND);
            printw("%c",ch);
            setcolor(WHITE);
            printw(" ");                
        }
    }
    
    // y=YSize-10;
    // x=2;

    // move(YSize-10,2);

    // for(ch='a';ch<='z';++ch, x+=2)
    // {
        // if(letter_found[ch])
        // {
            // if(letter_found[ch]==FOUND_IN_WRONG_PLACE)
            // {
                // setcolor(YELLOW);
                // move(y,x);
                // printw("%c ",ch);
            // }
        // }
    // }
    refresh();
    // getchar();
    // printf("\n");
}



void challenge(char *secret, unsigned char player)
{  
    unsigned char attempt_number;
    unsigned char i;
    unsigned char j;
    unsigned char char_found;
    unsigned char word_found;
    unsigned char exact_matches;
    
    time_t start_t;
    time_t elapsed_time;
    

    compute_secret_freq(secret);

    reset_vect(letter_found);
    
    // clrscr();
    
    // printxy(0,0,"--------------------------------");
    
    #if defined(DEBUG)
        printw("secret: %s\n", secret);
    #endif
    

    attempt_number = 1;
    word_found = 0;
    
    start_t = time(NULL);
    
    while(attempt_number<=MAX_ATTEMPTS)
    {
        // printf("\n-------------");
        // printf("\nTry no. %d  : ", attempt_number);
        show_found_letters();
        
        // move(4,0);
        // printw("TIME: %03u\n", clock()/ (CLOCKS_PER_SEC) - start_t);
        // refresh();
        
        move(5+attempt_number,0);
        curs_set(1);
        setcolor(WHITE);
        refresh();
        // move(4+attempt_number*2,0);
        // setcolor(WHITE);
        // refresh();
        
        scanw("%s", attempt);
        curs_set(0);
        
        move(5+attempt_number,0);
        refresh();
        
        make_lower(attempt);
        
        if(!strcmp("x",attempt))
        {
            break;
        }
        
        if(!in_dict(attempt))
        {
            move(YSize-2,0);
            printw("Not in dictionary");
            refresh();
            sleep(1);
            move(YSize-2,0);
            printw("                 ");
            move(5+attempt_number,0);
            printw("                 ");
            refresh();
        }
        else
        {
            
            reset_vect(hint);
            
            ++attempt_number;
            if(!strcmp(secret,attempt))
            {
                word_found = 1;
                exact_matches = word_size;
                setcolor(GREEN_BACKGROUND);
                move(4+attempt_number,0);
                printw("%s",secret);
                refresh();
                sleep(1);
                setcolor(WHITE);
                break;
            }
            else
            {
                // First compute frequencies of exact matches
                exact_matches = 0;
                for(i=0;i<word_size;++i)
                {
                    if(secret[i]==attempt[i])
                    {
                        ++hint[attempt[i]];
                        ++exact_matches;
                    }
                }           
                
                // Compute matches and partial matches
                for(i=0;i<word_size;++i)
                {
                    if(secret[i]==attempt[i])
                    {
                        setcolor(GREEN_BACKGROUND);
                        printw("%c",secret[i]);
                        refresh();
                        letter_found[secret[i]] = FOUND_IN_EXACT_PLACE;
                    }
                    else
                    {
                        for(j=0;j<word_size;++j)
                        {
                            char_found = 0;
                            if((secret[j]==attempt[i]) && 
                               (hint[attempt[i]]<freq[attempt[i]]) // # hints of a letter in wrong position <= # occurrences of the letter in secret word
                               ) 
                            {
                                char_found = 1;
                                if(letter_found[attempt[i]]!=FOUND_IN_EXACT_PLACE)
                                {
                                    letter_found[attempt[i]] = FOUND_IN_WRONG_PLACE;
                                }
                                break;
                            }
                        }
                        if(char_found)
                        {    
                            setcolor(YELLOW_BACKGROUND);
                            printw("%c",attempt[i]);
                            refresh();
                            ++hint[attempt[i]];
                        }
                        else
                        {
                            setcolor(RED_BACKGROUND);
                            printw("%c",attempt[i]);
                            refresh();
                        }
                        if(!letter_found[attempt[i]])
                        {
                            letter_found[attempt[i]]=TRIED_AND_NOT_FOUND;
                        }
                    }
                }
                refresh();
                // printf("\n");
            }  
        }
    }

    setcolor(WHITE);
    refresh();
    --attempt_number;
    // printf("\n\n");
    
    if(word_found)
    {
        move(YSize-5,0);
        setcolor(GREEN_BACKGROUND);
        printw("YOU WIN!");
        setcolor(WHITE);
        refresh();
        ++wins[player];
    }
    else
    {
        move(YSize-5,0);
        setcolor(RED_BACKGROUND);
        printw("YOU FAILED!");
        setcolor(WHITE);
        move(YSize-4,0);
        printw("Secret word: ");
        setcolor(GREEN_BACKGROUND);
        printw("%s", secret);
        setcolor(WHITE);
        refresh();
    }
    sleep(2);

    PRESS_ENTER_TO_CONTINUE();
    
    elapsed_time = time(NULL) - start_t;
    total_time[player]+=elapsed_time;
    clrscr();
    move(0,0);
    printw("Attempts: %d\n", attempt_number);
    printw("Time: %d\n", elapsed_time);
    refresh();
    match_score = compute_score(exact_matches, attempt_number, elapsed_time);   
    printw("Match score: %u", match_score);
    refresh();
    sleep(1);
    PRESS_ENTER_TO_CONTINUE();
}


void select_secret(unsigned short insert_secret_words, char *secret)
{
    if(!insert_secret_words)
    {
        unsigned short secret_index;

        secret_index = rand()%dict_size;
        strcpy(secret,dict[secret_index]);
    }
    else
    {
        do
        {
            clrscr();
            move(0,0);
            printw("Insert secret word\n");
            refresh();
            curs_set(1);
            scanw("%5s", secret);
            curs_set(0);
        } while(!in_dict(secret));
        clrscr();
    }    
}

void score_board(void)
{
    unsigned char player;
    
    printw("----------------------------------------------------------\n");
    
    printw("SCORE BOARD\n");
    printw("----------------------------------------------------------\n");

        
    for(player=1;player<=number_of_players;++player)
    {
        printw("PLAYER %u  |  GUESSED %02u/%02u  |  SCORE %06u  |  SECS %04u\n", player, wins[player], number_of_challenges, score[player], total_time[player]);
        printw("----------------------------------------------------------\n");
    }
    refresh();
    
    sleep(1);
    
    PRESS_ENTER_TO_CONTINUE();
}


void display_winners(unsigned short max, unsigned short *score_function)
{
    unsigned char player;
    unsigned short max_score = 0;
    
    for(player=1;player<=number_of_players;++player)
    {
        if(score_function[player]==max)
        {
            printw("%u ", player);
        }
    }
}


int main(int argc, char **argv)
{
    unsigned char i;
    unsigned char player;
    unsigned short insert_secret_words;
    unsigned short same_secret;
    
    char secret[9];    
    char selection;

    init_scrteen();
    setcolor(WHITE);

    while(1)
    {
        clrscr();
        
        printxy(0, 0,"-----------------------------------------------------------------------------");
        printxy(0, 1,"                      WORDLE\n");
        printxy(0, 2,"        ANSI C version by Fabrizio Caruso\n");
        printxy(0, 3,"-----------------------------------------------------------------------------");

        instructions();
        
        getchar();
        
        clrscr();
        
        printxy(0,0,"Choose language");
        printxy(0,1,"1 = English");
        printxy(0,2,"2 = French");
        printxy(0,3,"3 = Italian");
        printxy(0,4,"4 = Romanian");
        printxy(0,5,"5 = Spanish");
        printxy(0,6,"6 = German");
        printxy(0,7,"7 = Portuguese");
        
        move(9,0);
        selection = getch();
        dict_file = selection-'0';
        // scanw("%d", &dict_file);
        
        // printxy(0,,"SELECTION");
        refresh();
        
        sleep(1);
        // while(1){};
        // getchar();
       
        clrscr();
        
        printxy(0,4,"\nQuick play (y/n) ?");
        do
        {
            selection = getch();

        } while(!yes_or_no(selection));
                
        if(yes(selection))
        {
            word_size = 5;
            insert_secret_words = 0;
            number_of_players = 1;
            number_of_challenges = 1;
            same_secret = 0;
        }
        else
        {
            clrscr();
            
            if(dict_file==ENG)
            {
                clrscr();
                printxy(0,0,"\nChoose word size (3-8) :");
                
                selection = getch();
                word_size = selection - '0';
                
                if((word_size<MIN_WORD_LENGTH)||(word_size>MAX_WORD_LENGTH))
                {
                    word_size = 5;
                }
            }
            else
            {
                word_size = 5;
            }
        
            clrscr();
            printxy(0,0,"How many players?");
            
            selection = getch();
            
            number_of_players = selection - '0';
            
            curs_set(1);

            if(number_of_players==1)
            {
                same_secret = 0;
            }
            else
            {
                printxy(0,2,"Do you want the same secret words for all players (y/n) ?");

                move(3,0);
                refresh();
                do
                {
                    // scanw("%4s",yn);
                    selection = getch();

                } while(!yes_or_no(selection));
                
                if(yes(selection))
                {
                    same_secret = 1;
                    printxy(0,3,"ONLY ONE player at a time should look at the screen.");
                }
                else
                {
                    same_secret = 0;
                    printxy(0,3,"Each player gets different secret words.");
                } 
            }

            
            printxy(0,5,"Do you want a player to choose the secret words (y/n) ?");
            do
            {
                // scanw("%4s",yn);
                selection = getch();

            } while(!yes_or_no(selection));
            
            
            if(yes(selection))
            {
                insert_secret_words = 1;
                printxy(0,6,"A player or other person will have to CHOOSE the secret words.");
            }
            else
            {
                insert_secret_words = 0;
                printxy(0,6,"Secret words will be RANDOMLY chosen by the computer.");
            }
            

            curs_set(0);

            printxy(0,8,"How many words per game?");
            
            selection = getchar();
            
            number_of_challenges = selection - '0';    

            move(9,0);
            printxy(0,9,"You have selected %d words per game per player.");
            refresh();
        }

        clrscr();
        dict_size = read_dict(dict_file);
        
        gotoxy(0,2);
        printw("Words in the dictionary %u\n", dict_size);
        refresh();
        
        sleep(1);
        
        
        gotoxy(0,4);
        printw("Secret word will have %d letters", word_size);
        refresh();
        sleep(1);

        PRESS_ENTER_TO_CONTINUE();

        srand(time(NULL));

        
        for(player=1;player<=number_of_players;++player)
        {
            
            score[player] = 0;
            wins[player] = 0;
            total_time[player] = 0;
        }
        
        for(i=1;i<=number_of_challenges;++i)
        {
            
            if(same_secret)
            {
                select_secret(insert_secret_words, secret);
            }

            clrscr();

            for(player=1;player<=number_of_players;++player)
            {
                clrscr();

                if(!same_secret)
                {
                    select_secret(insert_secret_words, secret);
                }
                
                clrscr();
                printxy(0,0,"-----------------------------------------------------------------------------");
                gotoxy(0,1);
                printw("Player no. %d       Challenge no. %d", player, i);
                refresh();
                printxy(0,2,"-----------------------------------------------------------------------------");
           
                sleep(1);
                PRESS_ENTER_TO_CONTINUE();

                challenge(secret, player);
                
                sleep(1);
                
                // clrscr();
                // gotoxy(0,0);
                // printw("POINTS OBTAINED: %5d", match_score[player]);
                // if(match_score[player]==1000)
                // {
                    // setcolor(GREEN);
                    // printw(" MAX SCORE!");
                    // setcolor(WHITE);
                // }
                // refresh();
                // gotoxy(0,2);
                score[player]+=match_score;
                clrscr();
                score_board();
            }
        }
        
        clrscr();
        printw("----------------------------------------------------------\n");
        printw("F I N A L   R E S U L T S\n");
        score_board();
        
        move(YSize-7,0);
        printw("Player(s) with highest score: ");
        display_winners(max_score(score), score);
        
        move(YSize-5,0);
        printw("Player(s) with most wins: ");
        display_winners(max_score(wins), wins);
        
        move(YSize-2,0);
        do
        {            
            printw("Play again (Y/N)\n");
            refresh();
            selection = getch();

        } while(!yes_or_no(selection));
        
        if(no(selection))
        {
            break;
        }
    }        

    return EXIT_SUCCESS;
}



