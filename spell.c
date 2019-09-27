
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "dictionary.h"

int MAX_FILE_SIZE = 1024 * 1024 * 2;
int MAX_READ = 1024 * 512;

int hstrcmp(char* str1, char* str2)
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if(len1 == len2)
    {
        for(int i = 0; i < len1; i++)
        {
            if(str1[i] > str2[i])
                return 1;
            else if (str1[i] < str2[i])
                return -1;
        }
        return 0;
    }
    
    return len1 - len2;
}

char* strip_punct(const char* word)
{
    size_t nlen = strlen(word);
    while(ispunct(word[nlen-1]) || word[nlen-1] == '\n' || word[nlen -1] == '\r')
    {
        nlen--;
        if(nlen == 0) // all punctuation
            return NULL;
    }

    int scnt = 0;
    for(int i = 0; i < strlen(word); i++)
        if(ispunct(word[i]))
            scnt++;
        else
            break;
    
    size_t newlen = nlen - scnt;
    if(newlen > 0)
    {
        char* mword = malloc(newlen);
        if(mword)
        {
            strncpy(mword, word + scnt, newlen);
            mword[newlen] = '\0';
            return mword;
        }
    }
    else
    {
        return NULL;
    }
    
}

int check_words(FILE* fp, hashmap_t hashtable[], char *misspelled[])
{
    if(misspelled == NULL || hashtable == NULL || fp == NULL)
        return 0;
    
    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    rewind(fp);
    if(sz > MAX_FILE_SIZE)
        return 0;

    int num_misspelled = 0;
    char* read;
    char line[MAX_READ];
    while((read=fgets(line, MAX_READ + 1, fp)) != NULL && num_misspelled < MAX_MISSPELLED)
    {
        char *sptr = strtok(line, " ");
        while(sptr != NULL)
        {
            if(strlen(sptr) > LENGTH) // word too big, it is? misspelled
            {   
                while((read = fgets(line, MAX_READ + 1, fp) != NULL && strlen(read) == MAX_READ))
                {
                    char* temp = sptr;
                    sptr = realloc(sptr, strlen(temp) + strlen(read) + 1);
                    sptr = strcat(sptr, temp);
                    sptr = strcat(sptr, read);
                }
                if(read != NULL)
                {
                    char* temp = sptr;
                    sptr = realloc(sptr, strlen(temp) + strlen(read) + 1);
                    sptr = strcat(sptr, temp);
                    sptr = strcat(sptr, read);
                }
                
                misspelled[num_misspelled++] = sptr;
                
                break;
            }
            if(check_word(sptr, hashtable) == false)
            {
                char* addw = strip_punct(sptr);
                if(addw != NULL)
                {
                    misspelled[num_misspelled++] = addw;
                }
            }
            sptr = strtok(NULL, " ");
        }
    }

    fclose(fp);

    return num_misspelled;
}

bool internal_check_word(hashmap_t hashtable[], char* mword)
{
    bool word_found = false;
    char* endstring;
    long val = strtol(mword, &endstring, 10); 
    if(val > 0 || strcmp(mword, endstring) != 0)
        return true;
    
    int bkt = hash_function(mword);
    if(bkt >= HASH_SIZE)
        return false;
        
    hashmap_t hnode = hashtable[bkt];
    hashmap_t head = hnode;
    while(hnode != NULL && word_found != true)
    {
        if(strlen(hnode->word) != strlen(mword))
        {
            hnode = hnode->next;
            if(hnode == head)
                break;
            continue;  
        }
        if(hstrcmp(hnode->word, mword) == 0)
        {
            word_found = true;
        }
        
        hnode = hnode->next;
        if(hnode == head)
            break;
    }

    return word_found;
}

bool check_word(const char* word, hashmap_t hashtable[])
{
    if(word == NULL || hashtable == NULL)
        return false;

    size_t wlen = strlen(word);
    if(wlen > LENGTH) // word too big, it is ? misspelled
    {
        return false;   
    }

    char* mword = strip_punct(word); 
    bool word_found = false;
    if(mword != NULL)
    {
        //mword is correct here after removing punctuation
        word_found = internal_check_word(hashtable, mword);
        
        if(!word_found)
        {
            char* lword = malloc(strlen(mword) + 1);
            if(lword)
            {
                strcpy(lword, mword);
                lword[strlen(mword) + 1] = '\0'; 
                for(int i = 0; i < strlen(lword); i++)
                    lword[i] = tolower(lword[i]);
                
                word_found = internal_check_word(hashtable, lword);
                free(lword);
            }
        }
        free(mword);
    
    }

    return word_found;
}

void free_dictionary(hashmap_t hashtable[])
{
     for(int i = 0; i < HASH_SIZE; i++)
     {
         hashtable[i] = NULL;
     }
}

bool load_dictionary(const char* dictionary_file, hashmap_t hashtable[])
{
    if(dictionary_file == NULL || hashtable == NULL)
    {
        return false;
    } 

    free_dictionary(hashtable);
    struct stat buffer;
    int         status;

    status = stat(dictionary_file, &buffer);
    if(status == 0) {
        if(buffer.st_size > MAX_FILE_SIZE || !S_ISREG(buffer.st_mode))
        {
            return false;
        }
    }


    FILE* fp = fopen(dictionary_file, "r");

    bool load = false;
    if(fp != NULL)
    {
        char* line = NULL;
        size_t len = 0;
        ssize_t read = 0;
        while ((read = getline(&line, &len, fp)) >= 0) 
        {
            if(strlen(line) > LENGTH)
            {
                continue;
            }
            hashmap_t hnode = malloc(sizeof(node));
            hnode->next = NULL;
            if(len > LENGTH) 
                len = LENGTH;
            memset(hnode->word, '\0', LENGTH + 1);
            strcpy(hnode->word, line);
            for(int i = 0; i < strlen(hnode->word); i++)
                if(hnode->word[i]== '\r' || hnode->word[i] == '\n')
                    hnode->word[i] = '\0';

            int bkt = hash_function(hnode->word);
            if(bkt < HASH_SIZE)
            {
                if(hashtable[bkt] == NULL)
                    hashtable[bkt] = hnode;
                else
                {
                    hnode->next = hashtable[bkt];
                    hashtable[bkt] = hnode;
                }
            }
        }

        fclose(fp);
        load = true;
    }
    return load;
}

/*  
function check_word(string word, hashmap hashtable[])
{
    Set int bucket to the output of hash_function(word).
    Set hashmap_t cursor equal to hashmap[bucket].
    While cursor is not NULL:
        If word equals cursor->word:
            return True.
        Set curosr to cursor->next.
    Set int bucket to the output of hash_function(word).
    Set hashmap_t cursor equal to hashmap[bucket].
    While cursor is  not NULL:
        If lower_case(word) equals curosr->word:
            return True.
        Set curosr to cursor->next.
    return False.
}

function load_dictionary(string dictionary, hashmap hashtable[])
{
    Initialize all values in hash table to NULL.
    Open dict_file from path stored in dictionary.
    If dict_file is NULL:
        return false.
    While word in dict_file is not EOF (end of file):
        Set hashmap_t new_node to a new node.
        Set new_node->next to NULL.
        Set new_node->word equal to word.
        Set int bucket to hash_function(word).
        if hashtable[bucket] is NULL:
            Set hashtable[bucket] to new_node.
        else:
            Set new_node->next to hashtable[bucket].
            Set hashtable[bucket] to new_node.
    Close dict_file.
}

function check_words(file fp, hashmap hashtable[], string misspelled[])
{
    Set int num_misspelled to 0.
    While line in fp is not EOF (end of file):
        Read the line.
        Split the line on spaces.
        For each word in line:
            Remove punctuation from beginning and end of word.
            If not check_word(word):
                Append word to misspelled.
                Increment num_misspelled.
    Return num_misspelled.
}
*/