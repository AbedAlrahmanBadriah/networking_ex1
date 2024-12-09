#include <stdio.h>
#include <string.h>
#include "markov_chain.h"


#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"


void process_words(MarkovChain *markov_chain, char *prev_word, char *current_word) {
    if (!prev_word || !current_word) return;

    // Find or create MarkovNodes for the words
    Node *prev_node = add_to_database(markov_chain, prev_word);
    Node *current_node = add_to_database(markov_chain, current_word);
    if(prev_node == NULL || current_node == NULL){
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
    }

    if (!prev_node || !current_node) {
        fprintf(stderr, "Error: Failed to add words to the database.\n");
        return;
    }

    // Add current_node to the frequency list of prev_node
    if (prev_word[strlen(prev_word) - 1 ] != '.' && add_node_to_frequency_list((MarkovNode *)prev_node->data, (MarkovNode *)current_node->data) != 0) {
        fprintf(stderr, "Error: Failed to add to frequency list.\n");
    }
}




int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain) {
    char prev_word[1024] = {0};
    char current_word[1024];
    int words_read = 0;

    while (fscanf(fp, "%1023s", current_word) == 1) {
        if (words_read > 0) {
            // Add the pair (prev_word -> current_word) to the Markov Chain
            process_words(markov_chain, prev_word, current_word);
        }

        // Copy current_word to prev_word for the next iteration
        strncpy(prev_word, current_word, sizeof (prev_word)- 1);
        prev_word[sizeof(prev_word) - 1] = '\0'; // Ensure null-termination

        words_read++;
        if (words_to_read > 0 && words_read >= words_to_read) {
            return 1;
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

    if(argc != 5){
        fprintf(stderr, NUM_ARGS_ERROR);
        exit(EXIT_FAILURE);
    }

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
    if(fill_database(file, words_to_read, markov_chain) < 0){
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
        printf("Tweet<%d>", i);
        generate_tweet(starting_node, 20);
    }

    free_database(&markov_chain);

    exit(EXIT_SUCCESS);
}
