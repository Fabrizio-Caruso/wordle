
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <ctype.h>

#include "common.h"

extern unsigned short word_size;
extern char dict[MAX_DICT_SIZE][MAX_WORD_SIZE];
// char attempt[MAX_WORD_SIZE];
// unsigned char dict_file;
extern unsigned short dict_size;
extern unsigned short secret_dict_size;

extern unsigned short freq[VECT_SIZE];

extern unsigned short number_of_players;

unsigned char no(char ch)
{
    return ch=='n' || ch=='N';
}

unsigned char yes(char ch)
{
    return ch=='y' || ch=='Y';
}

unsigned char yes_or_no(char ch)
{
    return yes(ch) || no(ch);
}


unsigned short read_dict(unsigned char dict_file)
{
    FILE *fd;
    int count;
    int j;

    switch(dict_file)
    {
        case FRA:
            fd = fopen(FRA_DICT_FILE, "r");
        break;
        
        case ITA:
            fd = fopen(ITA_DICT_FILE, "r");
        break;
        
        case ROM:
            fd = fopen(ROM_DICT_FILE, "r");
        break;
        
        case ESP:
            fd = fopen(ESP_DICT_FILE, "r");
        break;
        
        case DEU:
            fd = fopen(DEU_DICT_FILE, "r");
        break;
        
        case POR:
            fd = fopen(POR_DICT_FILE, "r");
        break;
        
        case ENG:
            switch(word_size)
            {
                case 3:
                    fd = fopen(ENG_3_DICT_FILE, "r");
                break;
                case 4:
                    fd = fopen(ENG_4_DICT_FILE, "r");
                break;
                case 5:
                    fd = fopen(ENG_5_DICT_FILE, "r");
                break;
                case 6:
                    fd = fopen(ENG_6_DICT_FILE, "r");
                break;
                case 7:
                    fd = fopen(ENG_7_DICT_FILE, "r");
                break;
                case 8:
                    fd = fopen(ENG_8_DICT_FILE, "r");
                break;
            }
        break;
        
        default:
            fd = fopen(ENG_DICT_FILE, "r");
        break;
    }

    count=0;
    while(!feof(fd))
    {
        switch(word_size)
        {
            case 3:
                fscanf(fd, "%3s",&dict[count]);
            break;
            case 4:
                fscanf(fd, "%4s",&dict[count]);
            break;
            case 5:
                fscanf(fd, "%5s",&dict[count]);
            break;
            case 6:
                fscanf(fd, "%6s",&dict[count]);
            break;
            case 7:
                fscanf(fd, "%7s",&dict[count]);
            break;
            case 8:
                fscanf(fd, "%8s",&dict[count]);
            break;
        }
        ++count;
    }
    
    #if defined(DEBUG)
        for(j=0;j<count;++j)
        {
            printf("%s ", dict[j]);
        }
        printf("\n");
    #endif


    fclose(fd);
    
    return count;
}


void reset_vect(unsigned short *vect)
{
    unsigned char i;
    
    for(i='a';i<='z';++i)
    {
        vect[i] = 0;
    }
}


void compute_secret_freq(char *secret)
{
    unsigned char i;
    
    reset_vect(freq);
    
    for(i=0;i<word_size;++i)
    {
        ++freq[secret[i]];
    }
}


unsigned char in_dict(const char *word)
{
    unsigned char found = 0;
    unsigned short i;
    
    for(i=0;i<dict_size;++i)
    {
        if(!strcmp(word,dict[i]))
        {
            return 1;
        }
    }
    return 0;
}


void make_lower(char * str)
{
    unsigned char i;
    
    for(int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}

#include <ncurses.h>
unsigned short compute_score(unsigned char exact_matches, unsigned char attempt_number, clock_t elapsed_time)
{
    
    unsigned short score = 0;
    
    unsigned short bonus = 0;
    
    unsigned short time_penalty = 0;
        
    // clear();
    // printw("exact_matches %u\n", exact_matches);
    // printw("attempt_number %u\n", attempt_number);
    // printw("word_size %u\n", word_size);
    // sleep(2);
    
    if(elapsed_time>300)
    {
        time_penalty = 300;
    }
    else if (elapsed_time>60)
    {
        time_penalty = elapsed_time - 60;
    }
    
    if(!exact_matches)
    {
        return 0;
    }
    else if(exact_matches<word_size)
    {
        unsigned short points_for_single_letter = MAX_SCORE_FOR_PARTIAL_MATCH/(word_size-1);

        return exact_matches * points_for_single_letter - time_penalty/12;
    }
    else
    {    
        if (attempt_number<5)
        {
            bonus = 300;
        }
        else if (attempt_number<6)
        {
            bonus = 100;
        }
        
        return BASE_SCORE + bonus - time_penalty;
    }
}


unsigned short max_score(unsigned short *score_function)
{
    unsigned char player;
    unsigned short max_score = 0;
    
    for(player=1;player<=number_of_players;++player)
    {
        if(score_function[player]>max_score)
        {
            max_score = score_function[player];
        }
    }
    return max_score;
}

