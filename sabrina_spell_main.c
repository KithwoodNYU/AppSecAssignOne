#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"

void free_hashtable(hashmap_t hashtable[])
{
    for(int i = 0; i < HASH_SIZE; i++)
    {
        hashmap_t hnode = hashtable[i];
        while(hnode)
        {
            hashmap_t tmp = hnode;
            hnode = hnode->next;
            free(tmp);
        }
    }
}

void free_misspelled(char* misspelled[])
{
    for(int i = 0; i < MAX_MISSPELLED; i++)
        if(misspelled[i] == NULL)
            free(misspelled[i]);
}

int test_load()
{
    hashmap_t hashtable[HASH_SIZE];
    int count = 0;
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
                        count++;
                    hnode = hnode->next;
                } 
            }
        }
        free_hashtable(hashtable);
    }
    else
    {
        printf("Failed to load.");
    }

    return count;
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
    return 0;
}

int test_check_and_load()
{
    hashmap_t hashtable[HASH_SIZE];
    
    if(load_dictionary("wordlist.txt", hashtable))
    {
        char* myword = "farrowed";
        char* myword2 = "AAA";
        char* myword3 = "1234455";
        char* myword4 = "Truth\0";
        char* myword5 = ".Truth";
        char* myword6 = "truth";
        char* myword7 = "Au";
        printf("%d\n", check_word(myword, hashtable));
        printf("%d\n", check_word(myword2, hashtable));
        printf("%d\n", check_word(myword3, hashtable));
        printf("%d\n", check_word(myword4, hashtable));
        printf("%d\n", check_word(myword5, hashtable));
        printf("%d\n", check_word(myword6, hashtable));
        printf("%d\n", check_word(myword7, hashtable));
        free_hashtable(hashtable);
    }
    
    return 0;
}

int test_checkwords(char* filename)
{
    hashmap_t hashtable[HASH_SIZE];
    char* misspelled[MAX_MISSPELLED];
    for(int i = 0; i < MAX_MISSPELLED; i++)
        misspelled[i] = NULL; //initialize
        
    if(load_dictionary("wordlist.txt", hashtable))
    {
        //printf("dict loaded\n");
        FILE* fp = fopen(filename, "r");
        int num_misspelled = check_words(fp, hashtable, misspelled);
        printf("%d misspelled\n", num_misspelled);
        //for(int i = 0; i < num_misspelled;i++)
        //    printf("\t%s misspelled.\n", misspelled[i]);
        free_hashtable(hashtable);
        free_misspelled(misspelled);
    }
    else
    {
        printf("Dictionary not loaded.\n");
    }
    
    return 0;
}

int main(int argc, char* argv[])
{   
    char* filename = "./afl_tests/test2.txt";
    if(argc == 2 && argv != NULL)
    {
        filename = argv[1];
    }
    else
    {
        printf("Usage: spell_check <filename>\n");
        return 0;
    }
    
    //test_check_and_load();
    //test_checkwords("test1.txt");
    test_checkwords(filename);
    //test_checkwords("test3.txt");
    //test_checkwords("/usr/bin/vim.tiny");
    //printf("%d\n", strncmp("\nTruth", "Truth", 5));
    return 0;
}