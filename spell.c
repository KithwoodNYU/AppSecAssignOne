
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"

int MAX_FILE_SIZE = 1024 * 1024 * 1024;

int check_words(FILE* fp, hashmap_t hashtable[], char * misspelled[])
{
    return 0;
}

bool check_word(const char* word, hashmap_t hashtable[])
{
    if(word == NULL || hashtable == NULL)
        return false;
    size_t wlen = strlen(word);
    int ncnt = 0;
    size_t nlen = wlen;
    while(ispunct(word[nlen-1]))
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
    int newlen = wlen - ncnt - scnt;
    if(newlen > 0)
    {
        char* mword = malloc(newlen);
        strncpy(mword, word + scnt, newlen);
        //printf("%s\n", mword);
        int bkt = hash_function(mword);
        hashmap_t node = hashtable[bkt];
        while(node != NULL && !word_found)
        {
            if(strncmp(node->word, mword, newlen) == 0)
                word_found = true;
            else
            {
                for(int i = 0; i < strlen(mword); i++)
                    mword[i] = tolower(mword[i]);
                
                if(strncmp(node->word, mword, newlen) == 0)
                    word_found = true;
            }
            node = node->next;
        }
        free(mword);
    }

    return word_found;
}

bool load_dictionary(const char* dictionary_file, hashmap_t hashtable[])
{
    if(dictionary_file == NULL || hashtable == NULL)
    {
        printf("something is NULL");
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
            hashmap_t hnode = malloc(sizeof(node));
            hnode->next = NULL;
            if(len > 45) len = 45;
            strncpy(hnode->word, line, len);
            int bkt = hash_function(hnode->word);
            if(hashtable[bkt] == NULL)
                hashtable[bkt] = hnode;
            else
            {
                hnode->next = hashtable[bkt];
                hashtable[bkt] = hnode;
            }
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