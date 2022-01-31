
#ifndef _COMMON_H
#define _COMMON_H

#define DICTIONARY_DIR "dictionaries/"

#define ENG_DIR "ENG/"
#define FRA_DIR "FRA/" 
#define ITA_DIR "ITA/"
#define ROM_DIR "ROM/"
#define ESP_DIR "ESP/"
#define DEU_DIR "DEU/"
#define POR_DIR "POR/"

#define ENG_3_DICT_FILE DICTIONARY_DIR ENG_DIR "3_letter_words_ENG.txt"
#define ENG_4_DICT_FILE DICTIONARY_DIR ENG_DIR "4_letter_words_ENG.txt"
#define ENG_5_DICT_FILE DICTIONARY_DIR ENG_DIR "5_letter_words_ENG.txt"
#define ENG_6_DICT_FILE DICTIONARY_DIR ENG_DIR "6_letter_words_ENG.txt"
#define ENG_7_DICT_FILE DICTIONARY_DIR ENG_DIR "7_letter_words_ENG.txt"
#define ENG_8_DICT_FILE DICTIONARY_DIR ENG_DIR "8_letter_words_ENG.txt"

#define ENG_DICT_FILE ENG_5_DICT_FILE

#define FRA_DICT_FILE DICTIONARY_DIR "FRA/" "5_letter_words_FRA.txt"
#define ITA_DICT_FILE DICTIONARY_DIR "ITA/" "5_letter_words_ITA.txt"
#define ROM_DICT_FILE DICTIONARY_DIR "ROM/" "5_letter_words_ROM.txt"
#define ESP_DICT_FILE DICTIONARY_DIR "ESP/" "5_letter_words_ESP.txt"
#define DEU_DICT_FILE DICTIONARY_DIR "DEU/" "5_letter_words_DEU.txt"
#define POR_DICT_FILE DICTIONARY_DIR "POR/" "5_letter_words_POR.txt"

#define ENG 1
#define FRA 2
#define ITA 3
#define ROM 4
#define ESP 5
#define DEU 6
#define POR 7

#define MAX_DICT_SIZE 50000

#define MAX_SCORE_FOR_PARTIAL_MATCH 200

#define BASE_SCORE 700

#define MAX_WORD_SIZE 16

#define VECT_SIZE 400

#define NOT_TRIED 0
#define TRIED_AND_NOT_FOUND 1
#define FOUND_IN_WRONG_PLACE 2
#define FOUND_IN_EXACT_PLACE 3

#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 8

#define MAX_ATTEMPTS 6

#define MAX_PLAYERS 9


unsigned short read_dict(unsigned char dict_file);

unsigned char no(char ch);

unsigned char yes(char ch);

unsigned char yes_or_no(char ch);

void reset_vect(unsigned short *vect);

void compute_secret_freq(char *secret);

unsigned char in_dict(const char *word);

void make_lower(char * str);

unsigned short compute_score(unsigned char exact_matches, unsigned char attempt_number, clock_t elapsed_time);

unsigned short max_score(unsigned short *score_function);

#endif // _COMMON_H
