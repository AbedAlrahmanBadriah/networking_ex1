#include <stdio.h>
#include <string.h>
#include "markov_chain.h"


#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"

/*
int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain) {
    char word[1024]; // Buffer to hold each word
    int words_read = 0; // Counter to track the number of words read

    if (!fp || !markov_chain) {
        fprintf(stderr, "Error: Invalid file pointer or MarkovChain.\n");
        return 1; // Return failure if invalid parameters
    }

    while (fscanf(fp, "%1023s", word) == 1) {
        // Add the word to the MarkovChain database
        if (add_to_database(markov_chain, word) == NULL) {
            fprintf(stderr, "Error: Failed to add word to database.\n");
            return 1; // Return failure if the operation fails
        }
        MarkovNode *current = (MarkovNode *)markov_chain->database->first;


        words_read++;

        // Stop if we've read the desired number of words
        if (words_to_read > 0 && words_read >= words_to_read) {
            break;
        }
    }

    return 0; // Return success
}
*/


void process_words(MarkovChain *markov_chain, char *prev_word, char *current_word) {
    if (!prev_word || !current_word) return;

    // Find or create MarkovNodes for the words
    Node *prev_node = add_to_database(markov_chain, prev_word);
    Node *current_node = add_to_database(markov_chain, current_word);

    if (!prev_node || !current_node) {
        fprintf(stderr, "Error: Failed to add words to the database.\n");
        return;
    }

    // Add current_node to the frequency list of prev_node
    if (add_node_to_frequency_list((MarkovNode *)prev_node->data, (MarkovNode *)current_node->data) != 0) {
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
        strncpy(prev_word, current_word, sizeof(prev_word) - 1);
        prev_word[sizeof(prev_word) - 1] = '\0'; // Ensure null-termination

        words_read++;
        if (words_to_read > 0 && words_read >= words_to_read) {
            return 1;
        }
    }

    return 0;
}


MarkovChain *initialize_markov_chain(){

    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    if (!markov_chain) {
        fprintf(stderr, "Memory allocation error for MarkovChain.\n");
        exit(EXIT_FAILURE);
    }
    markov_chain->database = malloc(sizeof(LinkedList));
    if (!markov_chain->database) {
        free(markov_chain);
        fprintf(stderr, "Memory allocation error for LinkedList.\n");
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
        //int freqsum = calculate_total_frequencies(markov_node);
       // printf("size: %d\n", markov_node->frequency_list->size);
     //   printf("freq: %d\n", freqsum);
        current = current->next;
    }
}

void generate_tweet(MarkovNode *first_node, int max_length){
if(first_node == NULL || max_length <= 0){
    fprintf(stderr, "Invalid parameter");
    exit(EXIT_FAILURE);
}

MarkovNode *current = first_node;
int wordCounter = 0;

while(wordCounter < max_length){
    printf("%s", current->data);
    wordCounter++;

    if(current->data[strlen(current->data) - 1] == '.')
        break;

    if(wordCounter < max_length)
        printf(" ");

    current = get_next_random_node(current);
}
    printf("\n");
}


int main(){
    srand(113454);
    MarkovChain *markov_chain = initialize_markov_chain();

    FILE *fp = fopen("/home/student/CLionProjects/ex1/test.txt", "r");
    if (!fp) {
        fprintf(stderr, FILE_PATH_ERROR);
        return 1;
    }

    int words_to_read = 15; // Example limit
    if (fill_database(fp, words_to_read, markov_chain) == 0) {
        printf("Database successfully filled with words.\n");
    } else {
        printf("Error filling database.\n");
    }



    fclose(fp);

    print_markov_chain(markov_chain);

    MarkovNode *m = get_first_random_node(markov_chain);
    printf("first node: %s\n", m->data);
    MarkovNode *o = get_next_random_node(m);
    o = (MarkovNode* )get_node_from_database(markov_chain, o->data)->data;
//    char* data = o->data;
//    printf("%s\n", data);
//    MarkovNode *tmp = (MarkovNode*)get_node_from_database(markov_chain, data)->data;
//    printf("%s\n", tmp->data);
    printf("second node: %s\n", o->data);

    generate_tweet(m, 15);
    // Free memory (not shown here, but you should free the MarkovChain and database)
    return 0;
}
