#include <check.h>
#include "dictionary.h"
#include <stdlib.h>
#include <math.h>

#define DICTIONARY "wordlist.txt"
#define TESTDICT "small_wordlist.txt"

START_TEST(test_dictionary_normal)
{
    hashmap_t hashtable[HASH_SIZE];
    ck_assert(load_dictionary(TESTDICT, hashtable));
    // Here we can test if certain words ended up in certain buckets
    // to ensure that our load_dictionary works as intended. I leave
    // this as an exercise.
    char* test_words[7] = { "air", "flippant", "querulous", "ice", "castle", "wigwam", "pigskin"};
    int test_buckets[7] = { 316, 862, 1013, 305, 636, 652, 757 };        
    
    for(int i = 0; i < 7; i++)
    {
        bool found = false;
        hashmap_t hnode = hashtable[test_buckets[i]];
     
        while(!found && hnode != NULL)
        {
            if(strcmp(hnode->word, test_words[i]) == 0)
                found = true;
            hnode = hnode->next;    
        }
        ck_assert(found);
    }    
}
END_TEST

START_TEST(test_dictionary_eternal_link)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    hashmap_t hnode = hashtable[316]; // air
    hashmap_t head = hnode;
    while(hnode)
    {
        if(hnode->next == NULL)
        {
            hnode->next = head; //make a doubly linked list, tests availability
            break;
        }

        hnode = hnode->next;
    } 
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("small_wordlist.txt", "r");
    //this test would lead to an infinite loop if it didn't check for a doubly linked list
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 0);
}
END_TEST

START_TEST(test_check_word_normal)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    const char* correct_word = "Justice";
    const char* punctuation_word_2 = "pl.ace";
    ck_assert(check_word(correct_word, hashtable));
    ck_assert(!check_word(punctuation_word_2, hashtable));
    // Test here: What if a word begins and ends with "?
}
END_TEST

START_TEST(test_check_words_normal)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char* expected[3];
    expected[0] = "sogn";
    expected[1] = "skyn";
    expected[2] = "betta";
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test1.txt", "r");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 3);
    bool test = strlen(misspelled[0]) == strlen(expected[0]);
    int len1 = strlen(misspelled[0]);
    int len2 = strlen(expected[0]);
    ck_assert_msg(test, "%d!=%d", len1, len2);
    ck_assert_msg(strcmp(misspelled[0], expected[0]) == 0);
    ck_assert_msg(strcmp(misspelled[1], expected[1]) == 0);
    ck_assert_msg(strcmp(misspelled[2], expected[2]) == 0);
}
END_TEST

START_TEST(test_check_words_special_chars)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char* expected[3];
    expected[0] = "Ölive";
    expected[1] = "carrière";
    expected[2] = "año";
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test4.txt", "r");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 3);
    bool test = strlen(misspelled[0]) == strlen(expected[0]);
    int len1 = strlen(misspelled[0]);
    int len2 = strlen(expected[0]);
    ck_assert_msg(test, "%d!=%d", len1, len2);
    ck_assert_msg(strcmp(misspelled[0], expected[0]) == 0);
    ck_assert_msg(strcmp(misspelled[1], expected[1]) == 0);
    ck_assert_msg(strcmp(misspelled[2], expected[2]) == 0);
}
END_TEST

START_TEST(test_check_words_empty_file)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test5.txt", "r");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 0);
}
END_TEST

START_TEST(test_check_words_blank_lines)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test6.txt", "r");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 0);
}
END_TEST

START_TEST(test_check_words_null_input)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test2.txt", "r");
    int num_misspelled = check_words(NULL, hashtable, misspelled);
    ck_assert(num_misspelled == 0);
    num_misspelled = check_words(fp, NULL, misspelled);
    ck_assert(num_misspelled == 0);
    num_misspelled = check_words(fp, hashtable, NULL);
    ck_assert(num_misspelled == 0);
}
END_TEST

START_TEST(test_check_words_large_line_and_numbers)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test3.txt", "r");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 1);
}
END_TEST

START_TEST(test_check_words_binary_file)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("spell_check", "r");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled != 0);
}
END_TEST


Suite *
check_word_suite(void)
{
    Suite * suite;
    TCase * check_word_case;
    suite = suite_create("check_word");
    check_word_case = tcase_create("Core");
    tcase_add_test(check_word_case, test_dictionary_normal);
    tcase_add_test(check_word_case, test_check_word_normal);
    tcase_add_test(check_word_case, test_check_words_normal);
    tcase_add_test(check_word_case, test_dictionary_eternal_link);
    tcase_add_test(check_word_case, test_check_words_special_chars);
    tcase_add_test(check_word_case, test_check_words_empty_file);
    tcase_add_test(check_word_case, test_check_words_blank_lines);
    tcase_add_test(check_word_case, test_check_words_null_input);
    tcase_add_test(check_word_case, test_check_words_large_line_and_numbers);
    tcase_add_test(check_word_case, test_check_words_binary_file);
    suite_add_tcase(suite, check_word_case);

    return suite;
}

int
main(void)
{
    int failed;
    Suite *suite;
    SRunner *runner;
    
    suite = check_word_suite();
    runner = srunner_create(suite);
    srunner_run_all(runner, CK_NORMAL);
    failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}