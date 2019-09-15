#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"


int test_load()
{
    hashmap_t hashtable[HASH_SIZE];
    
    if(load_dictionary("wordlist.txt", hashtable))
    {
        for(int i = 0; i < HASH_SIZE; i++)
        {
            hashmap_t hnode = hashtable[i];
            if(hnode != NULL)
            {
                while(hnode)
                {
                    if(hnode->word != NULL && strlen(hnode->word) > 0)
                        printf("%s\n", hnode->word);
                    hnode = hnode->next;
                } 
            }
        }
    }
    else
    {
        printf("Failed to load.");
    }
}

int test_check()
{
    char* myword = "My Word!";
    char* myword2 = "..JimmyFizz[]";
    char* myword3 = "1234455";
    char* myword4 = ".JimmyFizz";
    char* myword5 = "..[]";
    check_word(myword, NULL);
    check_word(myword2, NULL);
    check_word(myword3, NULL);
    check_word(myword4, NULL);
    check_word(myword5, NULL);
}

int test_check_and_load()
{
    hashmap_t hashtable[HASH_SIZE];
    
    if(load_dictionary("wordlist.txt", hashtable))
    {
        char* myword = "farrowed";
        char* myword2 = "..JimmyFizz[]";
        char* myword3 = "1234455";
        char* myword4 = ".JimmyFizz";
        char* myword5 = ".Truth";
        printf("%d\n", check_word(myword, hashtable));
        printf("%d\n", check_word(myword2, hashtable));
        printf("%d\n", check_word(myword3, hashtable));
        printf("%d\n", check_word(myword4, hashtable));
        printf("%d\n", check_word(myword5, hashtable));
    }
}

int main(void)
{   
    test_check_and_load();
    return 0;
}