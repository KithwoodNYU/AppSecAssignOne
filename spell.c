
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"

int MAX_FILE_SIZE = 1024 * 1024 * 1024;

int hstrcmp(char* str1, char* str2)
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if(len1 == len2)
    {
        for(int i = 0; i < len1; i++)
        {
            //printf("%c compared to %c\n", str1[i], str2[i]);
            if(str1[i] > str2[i])
                return 1;
            else if (str1[i] < str2[i])
                return -1;
        }
        return 0;
    }
    
    return len1 - len2;
}

int check_words(FILE* fp, hashmap_t hashtable[], char *misspelled[])
{
    if(misspelled == NULL)
        return 0;
    int num_misspelled = 0;
    size_t read = 0;
    size_t len = 0;
    char* line = NULL;
    while((read=getline(&line, &len, fp)) != -1)
    {
        char *sptr = strtok(line, " ");
        while(sptr != NULL)
        {
            //printf("Checking %s\n", sptr);
            if(check_word(sptr, hashtable) == false)
            {
                char* addw = malloc(strlen(sptr) + 1);
                strcpy(addw, sptr);
                misspelled[num_misspelled++] = addw;
                if(num_misspelled == 1000)
                    return num_misspelled; 
            }
            sptr = strtok(NULL, " ");
        }
    }
    return num_misspelled;
}

bool internal_check_word(hashmap_t hashtable[], char* mword)
{
    bool word_found = false;
    int bkt = hash_function(mword);
    hashmap_t node = hashtable[bkt];
    
    while(node != NULL && word_found != true)
    {
        if(strlen(node->word) != strlen(mword))
        {
            node = node->next;
            continue;  
        }
        if(hstrcmp(node->word, mword) == 0)
        {
            word_found = true;
        }
        else
        {
            char* lword = malloc(strlen(mword) + 1);
            strcpy(lword, mword); 
            for(int i = 0; i < strlen(lword); i++)
                lword[i] = tolower(lword[i]);
            
            if(hstrcmp(node->word, lword) == 0)
                word_found = true;

            free(lword);
        }
        node = node->next;
    }

    return word_found;
}

bool check_word(const char* word, hashmap_t hashtable[])
{
    if(word == NULL || hashtable == NULL)
        return false;
    size_t wlen = strlen(word);
    int ncnt = 0;
    size_t nlen = wlen;
    while(ispunct(word[nlen-1]) || word[nlen-1] == '\n' || word[nlen -1] == '\r')
    {
        nlen--;
        ncnt++;
    }

    bool word_found = false;
    int scnt = 0;
    for(int i = 0; i < wlen; i++)
        if(ispunct(word[i]))
            scnt++;
        else
            break;
    size_t newlen = wlen - ncnt - scnt;
    if(newlen > 0)
    {
        char* mword = malloc(newlen + 1);
        strncpy(mword, word + scnt, newlen);
        mword[newlen] = '\0';
        word_found = internal_check_word(hashtable, mword);
        
        if(!word_found)
        {
            char* lword = malloc(strlen(mword) + 1);
            strcpy(lword, mword); 
            for(int i = 0; i < strlen(lword); i++)
                lword[i] = tolower(lword[i]);
            
            word_found = internal_check_word(hashtable, lword);

            free(lword);
            free(mword);
        }
    }

    return word_found;
}



bool load_dictionary(const char* dictionary_file, hashmap_t hashtable[])
{
    if(dictionary_file == NULL || hashtable == NULL)
    {
        return false;
    } 

    memset(hashtable, '\0', sizeof(hashmap_t) * HASH_SIZE);

    struct stat buffer;
    int         status;

    status = stat(dictionary_file, &buffer);
    if(status == 0) {
        if(buffer.st_size > MAX_FILE_SIZE)
        {
            //printf("Buffer: %ld  and MAX: %d", buffer.st_size, MAX_FILE_SIZE);
            return false;
        }
    }
    FILE* fp = fopen(dictionary_file, "r");

    bool load = false;
    if(fp != NULL)
    {
        char* line = NULL;
        size_t len = 0;
        size_t read = 0;

        while ((read = getline(&line, &len, fp)) != -1) {
            if(line[read - 1] == '\n')
            {
                line[read - 1] = '\0';
            }
            if(strlen(line) > 45)
            {
                line = NULL;
                read = 0;
                len = 0;
                continue;
            }
            hashmap_t hnode = malloc(sizeof(node));
            hnode->next = NULL;
            if(len > 45) 
                len = 45;
            memset(hnode->word, '\0', 46);
            strcpy(hnode->word, line);
            int bkt = hash_function(hnode->word);
            if(hashtable[bkt] == NULL)
                hashtable[bkt] = hnode;
            else
            {
                hnode->next = hashtable[bkt];
                hashtable[bkt] = hnode;
            }

            line = NULL;
            read = 0;
            len = 0;
        }

        fclose(fp);
        load = true;
    }
    
    return load;
}

/*  
fuction check_word(string word, hashmap hashtable[])
{
    Remove punctuation from beginning and end of word.
    Set int bucket to the output of hash_function(word).
    Set hashmap_t cursor equal to hashmap[bucket].
    While cursor is not NULL:
        If word equals cursor->word:
            return True.
        Else if lower_case(word) equals curosr->word:
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
            If not check_word(word):
                Append word to misspelled.
                Increment num_misspelled.
    Return num_misspelled.
}
*/