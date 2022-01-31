
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <ctype.h>

#include "common.h"


#ifdef _WIN32
#define clrscr() system("cls");
#else
#include <stdio.h>
#define clrscr() printf("\e[1;1H\e[2J")
#endif


char dict[MAX_DICT_SIZE][MAX_WORD_SIZE];

char attempt[MAX_WORD_SIZE];

unsigned char dict_file;

unsigned short dict_size;
unsigned short secret_dict_size;

unsigned short freq[VECT_SIZE];
unsigned short hint[VECT_SIZE];
unsigned short letter_found[VECT_SIZE]; 

unsigned short match_score[MAX_PLAYERS];

unsigned short score[MAX_PLAYERS];

unsigned char wins[MAX_PLAYERS];

unsigned short total_time[MAX_PLAYERS];

unsigned short word_size;




void instructions(void)
{
    printf("-----------------------------------------------------------------------------\n");
    printf("                     INSTRUCTIONS\n");
    printf("-----------------------------------------------------------------------------\n");
    
    printf("Guess a secret word in max 6 attempts\n");
    printf("'-' means letter nowhere in the secret word\n");
    printf("'*' one occurence of this letter is elsewhere for each '*' on the same letter\n");
    printf("A displayed letter means that it is correct in the displayed place\n");
    printf("-----------------------------------------------------------------------------\n");
    printf("Only small letters and no diacritics\n");
    printf("Insert 'x' to give up\n");
    printf("-----------------------------------------------------------------------------\n");


}

void show_found_letters(void)
{
    unsigned char ch;
    
    for(ch='a';ch<='z';++ch)
    {
        if(letter_found[ch])
        {
            if(letter_found[ch]==FOUND_IN_EXACT_PLACE)
            {
                printf("%c ",ch-'a'+'A');
            }
            else
            {
                printf("%c ",ch);
            }
        }
        else
        {
                printf("%c ",ch);  
        }
    }
    printf("\n             ");
    for(ch='a';ch<='z';++ch)
    {
        if(letter_found[ch])
        {
            if(letter_found[ch]==FOUND_IN_WRONG_PLACE)
            {
                printf("* ");
            }
            else if(letter_found[ch]==TRIED_AND_NOT_FOUND)
            {
                printf("- ");
            }
            else
            {
                printf("  ");
            }
        }
        else
        {
            printf("? ");
        }
    }
    printf("\n");
}



void challenge(char *secret, unsigned char player)
{  
    unsigned char attempt_number;
    unsigned char i;
    unsigned char j;
    unsigned char char_found;
    unsigned char word_found;
    unsigned char exact_matches;
    
    clock_t start_t;
    clock_t elapsed_time;
    

    compute_secret_freq(secret);

    reset_vect(letter_found);
    
    printf("\n--------------------------------\n");
    
    #if defined(DEBUG)
        printf("secret: %s\n", secret);
    #endif
    

    attempt_number = 1;
    word_found = 0;
    
    start_t = clock() / (CLOCKS_PER_SEC);
    while(attempt_number<=MAX_ATTEMPTS)
    {
        printf("\n-------------");
        printf("\nTry no. %d  : ", attempt_number);
        show_found_letters();
        
        
        scanf("%s", attempt);
        
        make_lower(attempt);
        
        if(!strcmp("x",attempt))
        {
            break;
        }
        
        if(!in_dict(attempt))
        {
            printf("Not in dictionary\n");
        }
        else
        {
            
            reset_vect(hint);
            
            ++attempt_number;
            if(!strcmp(secret,attempt))
            {
                word_found = 1;
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
                        printf("%c",secret[i]);
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
                            printf("*");
                            ++hint[attempt[i]];
                        }
                        else
                        {
                            printf("-");
                        }
                        if(!letter_found[attempt[i]])
                        {
                            letter_found[attempt[i]]=TRIED_AND_NOT_FOUND;
                        }
                    }
                }
                printf("\n");
            }  
        }
    }

    --attempt_number;
    printf("\n\n");
    
    if(word_found)
    {
        printf("YOU WIN!\n");
        ++wins[player];
    }
    else
    {
        printf("YOU FAILED!\n");
        printf("Secret word: %s\n", secret);
    }
    
    elapsed_time = clock() / (CLOCKS_PER_SEC) - start_t;
    total_time[player]+=elapsed_time;
    printf("Attempts: %d\n", attempt_number);
    printf("Time: %d\n", elapsed_time);
    match_score[player] = compute_score(word_found, attempt_number, exact_matches, elapsed_time);   
}


void select_secret(unsigned short insert_secret_words, char *secret)
{
    if(!insert_secret_words)
    {
        unsigned short secret_index;

        secret_index = rand()%secret_dict_size;
        strcpy(secret,dict[secret_index]);
    }
    else
    {
        do
        {
            clrscr();
            printf("Insert secret word\n");
            scanf("%5s", secret);
        } while(!in_dict(secret));
        clrscr();
    }    
}


int main(int argc, char **argv)
{
    unsigned char i;
    unsigned int number_of_challenges;
    unsigned short number_of_players;
    unsigned char player;
    unsigned short insert_secret_words;
    unsigned short same_secret;
    
    char secret[9];    
    char yn[5];

    while(1)
    {
        clrscr();
        printf("\n\n\n-----------------------------------------------------------------------------\n");
        printf("                      WORDLE\n");
        printf("        ANSI C version by Fabrizio Caruso\n");

        instructions();
        printf("\nChoose language\n1 = English\n2 = French\n3 = Italian\n4 = Romanian\n5 = Spanish\n6 = German\n7 = Portuguese\n:");
        
        scanf("%d", &dict_file);
        
        if(dict_file==ENG)
        {
            printf("\nChoose word size (3-8) :");
            scanf("%d", &word_size);
            
            if((word_size<MIN_WORD_LENGTH)||(word_size>MAX_WORD_LENGTH))
            {
                word_size = 5;
            }
        }
        else
        {
            word_size = 5;
        }
        
        printf("\nSecret word will have %d letters\n", word_size);
        
        dict_size = read_dict(dict_file);
        
        if((word_size = 5) && dict_file==ENG)
        {
            secret_dict_size = 2500;
        }
        else
        {
            secret_dict_size = dict_size;
        }
        
        printf("Words in the dictionary %u\n", dict_size);
        
        srand(time(NULL));

        printf("\nQuick play (y/n) ?");
        do
        {
            scanf("%4s",yn);

        } while((yn[0]!='y')&&(yn[0]!='Y')&&(yn[0]!='n')&&(yn[0]!='N'));
        
        if((yn[0]=='y')||(yn[0]=='Y'))
        {
            insert_secret_words = 0;
            number_of_players = 1;
            number_of_challenges = 1;
            same_secret = 0;
        }
        else
        {
 
            printf("\nHow many players?");
            scanf("%d", &number_of_players);
            
            if(number_of_players==1)
            {
                same_secret = 0;
            }
            else
            {
                printf("\nDo you want the same secret words for all players (y/n) ?");

                do
                {
                    scanf("%4s",yn);

                } while((yn[0]!='y')&&(yn[0]!='Y')&&(yn[0]!='n')&&(yn[0]!='N'));
                
                if((yn[0]=='y')||(yn[0]=='Y'))
                {
                    same_secret = 1;
                    printf("ONLY ONE player at a time should look at the screen.\n");
                }
                else
                {
                    same_secret = 0;
                    printf("Each player gets different secret words.\n");
                } 
            }

            printf("\nDo you want a player to choose the secret words (y/n) ?");
            do
            {
                scanf("%4s",yn);

            } while((yn[0]!='y')&&(yn[0]!='Y')&&(yn[0]!='n')&&(yn[0]!='N'));
            
            
            if((yn[0]=='y')||(yn[0]=='Y'))
            {
                insert_secret_words = 1;
                printf("A player or other person will have to CHOOSE the secret words.\n");
            }
            else
            {
                insert_secret_words = 0;
                printf("Secret words will be RANDOMLY chosen by the computer.\n");
            }
            

            printf("\nHow many words per game?");
            scanf("%u", &number_of_challenges);
        }
        
        getchar();
        
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
                printf("\n-----------------------------------------------------------------------------\n");
                printf("Player no. %d       Challenge no. %d\n", player, i);
                printf(  "-----------------------------------------------------------------------------\n");

                printf("\n");
                printf("Press ENTER to start\n");
                getchar();

                challenge(secret, player);
                printf("POINTS OBTAINED: %5d", match_score[player]);
                if(match_score[player]==1000)
                {
                    printf(" MAX SCORE!");
                }
                printf("\n");
                score[player]+=match_score[player];
                printf("SCORE: %5d\n", score[player]);

                getchar();
                sleep(2);
                getchar();
            }
        }
        
        clrscr();
        printf("\n----------------------------------------------------------");
        
        printf("\nSCORE BOARD\n");
        printf("\n----------------------------------------------------------\n");

            
        for(player=1;player<=number_of_players;++player)
        {
            printf("PLAYER %u  |  GUESSED %02u/%02u  |  SCORE %06u  |  SECS %04u\n", player, wins[player], number_of_challenges, score[player], total_time[player]);
            printf("----------------------------------------------------------\n");
        }

        sleep(1);
        
        do
        {
            getchar();
            printf("Play again (Y/N)\n");

            scanf("%4s",yn);

        } while((yn[0]!='y')&&(yn[0]!='Y')&&(yn[0]!='n')&&(yn[0]!='N'));
        
        if((yn[0]!='y')&&(yn[0]!='Y'))
        {
            break;
        }
    }        

    return EXIT_SUCCESS;
}



