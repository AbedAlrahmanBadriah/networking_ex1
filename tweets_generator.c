#include <stdio.h>
#include <string.h>
#include "markov_chain.h"


#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"


int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain) {
    if (fp == NULL || markov_chain == NULL || markov_chain->database == NULL) {
        fprintf(stderr, "Invalid parameters to fill_markov_chain.\n");
        return 1;
    }

    char buffer[1000]; // Buffer to read lines from the file
    char prev_word[1024] = {0}; // To store the previous word
    int words_read = 0;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Tokenize the line into words
        char *current_word = strtok(buffer, DELIMITERS); // Delimiters: space, tab, newline

        while (current_word != NULL) {
            if (words_read > 0) {
                // Add the pair (prev_word -> current_word) to the MarkovChain
                Node *prev_node = add_to_database(markov_chain, prev_word);
                Node *current_node = add_to_database(markov_chain, current_word);

                if (prev_node == NULL || current_node == NULL) {
                    fprintf(stderr, "Error: Failed to process words.\n");
                    return 1;
                }

                if (prev_word[strlen(prev_word) - 1] != '.') {
                    if (add_node_to_frequency_list((MarkovNode *)prev_node->data, (MarkovNode *)current_node->data) != 0) {
                        fprintf(stderr, "Error: Failed to add node to frequency list.\n");
                        return 1;
                    }
                }
            }

            // Move to the next word
            strncpy(prev_word, current_word, sizeof(prev_word) -1);
            prev_word[sizeof(prev_word) - 1] = '\0'; // Ensure null-termination
            words_read++;

            // Stop if the maximum word count is reached
            if (words_to_read > 0 && words_read >= words_to_read) {
                return 0;
            }

            current_word = strtok(NULL, " \t\n\r");
        }
    }

    return 0;
}



MarkovChain *initialize_markov_chain(){

    MarkovChain *markov_chain = (MarkovChain *)malloc(sizeof(MarkovChain));
    if (!markov_chain) {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        free(markov_chain);
        return NULL;
    }
    markov_chain->database = malloc(sizeof(LinkedList));
    if (!markov_chain->database) {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        free(markov_chain->database);
        free(markov_chain);
        exit(EXIT_FAILURE);
    }
    markov_chain->database->first = NULL;
    markov_chain->database->last = NULL;
    markov_chain->database->size = 0;
    return markov_chain;
}



void print_markov_chain(MarkovChain *markov_chain) {
    if (!markov_chain || !markov_chain->database) {
        printf("Markov Chain is empty or not initialized.\n");
        return;
    }

    Node *current = markov_chain->database->first;

    printf("Markov Chain:\n");
    while (current) {
        MarkovNode *markov_node = (MarkovNode *)current->data;

        // Print the data of the current MarkovNode
        printf("Node: %s\n", markov_node->data);

        // Print the frequency list for this MarkovNode
        if (markov_node->frequency_list) {
            printf("  Frequency List:\n");
            for (int i = 0; markov_node->frequency_list[i].markov_node != NULL; i++) {
                MarkovNode *next_node = markov_node->frequency_list[i].markov_node;
                int frequency = markov_node->frequency_list[i].frequency;

                printf("    - %s: %d\n", next_node->data, frequency);

            }
        } else {
            printf("  Frequency List: (empty)\n");
        }

        current = current->next;
    }
}



//
//int main(){
//    srand(65);
//    MarkovChain *markov_chain = initialize_markov_chain();
//
//    FILE *fp = fopen("/home/student/CLionProjects/ex1/test.txt", "r");
//    if (!fp) {
//        fprintf(stderr, FILE_PATH_ERROR);
//        return 1;
//    }
//
//    int words_to_read = 30; // Example limit
//    if (fill_database(fp, words_to_read, markov_chain) == 0) {
//        printf("Database successfully filled with words.\n");
//    } else {
//        printf("Error filling database.\n");
//    }
//
//
//
//    fclose(fp);
//
//    print_markov_chain(markov_chain);
//
//    MarkovNode *m = get_first_random_node(markov_chain);
//    printf("first node: %s\n", m->data);
//    MarkovNode *o = get_next_random_node(m);
//    o = (MarkovNode* )get_node_from_database(markov_chain, o->data)->data;
////    char* data = o->data;
////    printf("%s\n", data);
////    MarkovNode *tmp = (MarkovNode*)get_node_from_database(markov_chain, data)->data;
////    printf("%s\n", tmp->data);
//    printf("second node: %s\n", o->data);
//
//    generate_tweet(m, 30);
//    // Free memory (not shown here, but you should free the MarkovChain and database)
//    return 0;
//}

int main(int argc, char *argv[]){
//    for (int i =0; i< argc; i++) {
//        printf("%s ", argv[i]);
//    }
//    if(argc !=5){
//    fprintf(stderr, NUM_ARGS_ERROR);
//    exit(EXIT_FAILURE);
//}
    argv[1]  = "55";
    argv[2] = "30";
    argv[3] = "/home/student/CLionProjects/ex1/justdoit_tweets.txt";
    argv[4] = "30";
    int seed = atoi(argv[1]);
    int num_of_tweets = atoi(argv[2]);
    char* file_path = argv[3];
    int words_to_read = atoi(argv[4]);

    //Setting the seed
    srand(seed);

    //Opining the corpus file
    FILE *file = fopen(file_path, "r");
    if(file == NULL){
        fprintf(stderr, FILE_PATH_ERROR);
        exit(EXIT_FAILURE);
    }

    //Initializing the MarkovChain
    MarkovChain *markov_chain = initialize_markov_chain();
    if(markov_chain == NULL){
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        free_database(&markov_chain);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    //Filling the database
    if(fill_database(file, words_to_read, markov_chain) != 0){
        fprintf(stderr, "Error: Filling database failed");
        free_database(&markov_chain);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    //Close the file
    fclose(file);


    //Tweets generation
    for(int i=0; i< num_of_tweets; i++){
        MarkovNode *starting_node = get_first_random_node(markov_chain);
        if(starting_node == NULL){
            free_database(&markov_chain);
            exit(EXIT_FAILURE);
        }
        printf("Tweet<%d>", i+1);
        generate_tweet(starting_node, 20);
    }

    print_markov_chain(markov_chain);

    free_database(&markov_chain);

    exit(EXIT_SUCCESS);
}
