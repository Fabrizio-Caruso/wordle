
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DICT_SIZE 9999

#define ENG_DICT_FILE "5_letter_words_ENG.txt"
#define FRA_DICT_FILE "5_letter_words_FRA.txt"
#define ITA_DICT_FILE "5_letter_words_ITA.txt"

#define ENG 1
#define FRA 2
#define ITA 3

#define MAX_ATTEMPTS 6

#define DEBUG

char dict[DICT_SIZE][6];

char attempt[6];

unsigned short secret_index;

char secret[6];

unsigned char dict_file;



void read_dict(unsigned char dict_file)
{
    FILE *fd;
    int i;
    int j;

    switch(dict_file)
    {
        case FRA:
            fd = fopen(FRA_DICT_FILE, "r");
        break;
        
        case ITA:
            fd = fopen(ITA_DICT_FILE, "r");
        break;
        
        default:
            fd = fopen(ENG_DICT_FILE, "r");
        break;
    }

    // for(i=0;i<DICT_SIZE;++i)
    i=0;
    while(!feof(fd))
    {
        fscanf(fd, "%s",&dict[i]);
        ++i;
    }
    
    #if defined(DEBUG)
    printf("\n");

    printf("Number of words in dictionary: %d\n", i);
    // for(j=0;j<i;++j)
    // {
        // printf("%s ", dict[j]);
    // }
    // printf("\n");
    #endif


    fclose(fd);
}


unsigned char in_dict(const char *word)
{
    unsigned char found = 0;
    unsigned short i;
    
    for(i=0;i<DICT_SIZE;++i)
    {
        // printf("comparing with %s\n", dict[i]);
        if(!strcmp(word,dict[i]))
        {
            return 1;
        }
    }
    return 0;
}


int main(int argc, char **argv)
{
    unsigned char attempt_number;
    char yn;
    unsigned char i;
    unsigned char j;
    unsigned char char_found;
    unsigned char word_found;

    printf("\nChoose language (1 = English, 2 = French, 3 = Italian)");
    
    scanf("%d", &dict_file);
    
    read_dict(dict_file);
    srand(time(NULL));

    while(1){
        
        secret_index = rand()%DICT_SIZE;
        strcpy(secret,dict[secret_index]);
        
        printf("\n\n\n--------------------------------");
        
        #if defined(DEBUG)
            printf("secret: %s\n", secret);
        #endif
        

        
        printf("\n\n\n--------------------------------");

        attempt_number = 1;
        word_found = 0;
        while(attempt_number<=MAX_ATTEMPTS)
        {
            printf("\nAttempt number: %d\n", attempt_number);
            scanf("%s", attempt);
            
            
            if(!in_dict(attempt))
            {
                printf("Not in dictionary\n");
            }
            else
            {
                ++attempt_number;
                if(!strcmp(secret,attempt))
                {
                    word_found = 1;
                    break;
                }
                else
                {
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
                                if(secret[j]==attempt[i])
                                {
                                    char_found = 1;
                                    break;
                                }
                            }
                            if(char_found)
                            {    
                                printf("*");   
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

            scanf("%c",&yn);
        

        } while((yn!='y')&&(yn!='Y')&&(yn!='n')&&(yn!='N'));
        
        if((yn!='y')&&(yn!='Y'))
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


