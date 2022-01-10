
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DICT_SIZE 19999

#define ENG_DICT_FILE "5_letter_words_ENG.txt"
#define FRA_DICT_FILE "5_letter_words_FRA.txt"
#define ITA_DICT_FILE "5_letter_words_ITA.txt"
#define ROM_DICT_FILE "5_letter_words_ROM.txt"

#define ENG 1
#define FRA 2
#define ITA 3
#define ROM 4

#define MAX_ATTEMPTS 6

// #define DEBUG

char dict[MAX_DICT_SIZE][6];

char attempt[6];

unsigned short secret_index;

char secret[6];

unsigned char dict_file;

unsigned short dict_size;

unsigned char freq[100];
unsigned char hint[100];

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
        
        default:
            fd = fopen(ENG_DICT_FILE, "r");
        break;
    }

    count=0;
    while(!feof(fd))
    {
        fscanf(fd, "%5s",&dict[count]);
        ++count;
    }
    
    printf("\n");

    printf("Number of words in dictionary: %d\n", count);
    
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

void reset_freq(unsigned char *vect)
{
    unsigned char i;
    
    for(i='a';i<'z';++i)
    {
        vect[i] = 0;
    }
}

void compute_freq(void)
{
    unsigned char i;
    
    reset_freq(freq);
    
    for(i=0;i<5;++i)
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
        // printf("comparing with %s\n", dict[i]);
        if(!strcmp(word,dict[i]))
        {
            return 1;
        }
    }
    return 0;
}


void instructions(void)
{
    printf("-----------------------------------------------------------------------------\n");
    printf("                     INSTRUCTIONS\n");
    printf("-----------------------------------------------------------------------------\n");
    
    printf("Guess a 5-letter secret word in max 6 attempts\n");
    printf("'-' means letter nowhere in the secret word\n");
    printf("'*' one occurence of this letter is elsewhere for each '*' on the same letter\n");
    printf("A displayed letter means that it is correct in the displayed place\n");
    printf("-----------------------------------------------------------------------------\n");
    printf("Only small letters and no diacritics\n");
    printf("Insert 'x' to give up\n");
    printf("-----------------------------------------------------------------------------\n");


}


int main(int argc, char **argv)
{
    unsigned char attempt_number;
    char yn[5];
    unsigned char i;
    unsigned char j;
    unsigned char char_found;
    unsigned char word_found;
    unsigned char insert_secret_words;

    printf("\n\n\n-----------------------------------------------------------------------------\n");
    printf("                      WORDLE\n");
    printf("        ANSI C version by Fabrizio Caruso\n");

    instructions();
    printf("\nChoose language (1 = English, 2 = French, 3 = Italian, 4 = Romanian) ");
    
    scanf("%d", &dict_file);
    
    dict_size = read_dict(dict_file);
    srand(time(NULL));

    printf("\nDo you want to insert the secret words (y/n) ?");
    do
    {
        scanf("%4s",yn);

    } while((yn[0]!='y')&&(yn[0]!='Y')&&(yn[0]!='n')&&(yn[0]!='N'));
    
    if((yn[0]!='y')&&(yn[0]!='Y'))
    {
        insert_secret_words = 0;
    }
    else
    {
        insert_secret_words = 1;
    }

    while(1){
        
        if(!insert_secret_words)
        {
        
            secret_index = rand()%dict_size;
            strcpy(secret,dict[secret_index]);
        }
        else
        {
            do
            {
                printf("Insert secret word\n");
                scanf("%5s", secret);
            } while(!in_dict(secret));
        }
        
        compute_freq();
        
        // for(i='a';i<'z';++i)
        // {
            // printf("%d", freq[i]);
        // }
        
        printf("\n\n--------------------------------\n");
        
        #if defined(DEBUG)
            printf("secret: %s\n", secret);
        #endif
        

        attempt_number = 1;
        word_found = 0;
        while(attempt_number<=MAX_ATTEMPTS)
        {
            printf("\nTry no. %d\n", attempt_number);
            scanf("%5s", attempt);
            
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
                
                reset_freq(hint);
                
                ++attempt_number;
                if(!strcmp(secret,attempt))
                {
                    word_found = 1;
                    break;
                }
                else
                {
                    // First compute frequencies of exact matches
                    for(i=0;i<5;++i)
                    {
                        if(secret[i]==attempt[i])
                        {
                            ++hint[attempt[i]];
                        }
                    }           
                    
                    // Compute matches and partial matches
                    for(i=0;i<5;++i)
                    {
                        if(secret[i]==attempt[i])
                        {
                            printf("%c",secret[i]);
                        }
                        else
                        {
                            for(j=0;j<5;++j)
                            {
                                char_found = 0;
                                if((secret[j]==attempt[i]) && 
                                   (hint[attempt[i]]<freq[attempt[i]]) // # hints of a letter in wrong position <= # occurrences of the letter in secret word
                                   ) 
                                {
                                    char_found = 1;
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
                        }
                    }
                    printf("\n");
                }  
            }
        }
        
        printf("\n\n");
        
        if(word_found)
        {
            printf("YOU WIN!\n");
        }
        else
        {
            printf("YOU FAILED!\n");
            printf("Secret word: %s\n", secret);
        }
        
        printf("\n\n");
        
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
        // printf("attempt: %s\n", attempt);
        
        // if(in_dict(attempt))
        // {
            // printf("In dictionary\n");
        // }
        // else
        // {
            // printf("Not in dictionary\n");
        // }
    }
    
    return EXIT_SUCCESS;

}


