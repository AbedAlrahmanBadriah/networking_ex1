#include <string.h>
#include "markov_chain.h"
#define _POSIX_C_SOURCE 200809L


/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    if (max_number <= 0) {
        fprintf(stderr, "Error: Invalid max_number for random number generation.\n");
        exit(EXIT_FAILURE);
    }
    return rand() % max_number;
}


void free_database(MarkovChain **ptr_chain) {
    if (ptr_chain == NULL || *ptr_chain == NULL) return;

    MarkovChain *markovChain = *ptr_chain;
    if (markovChain == NULL || markovChain->database == NULL) return;

    Node *cur = markovChain->database->first;
    while (cur != NULL) {
        Node *next = cur->next;

        // Free the MarkovNode and its members
        MarkovNode *markovNode = cur->data;
        if (markovNode != NULL) {
            if (markovNode->data != NULL) {
                free(markovNode->data); // Free the string data
                markovNode->data = NULL;
            }

            if (markovNode->frequency_list != NULL) {
                free(markovNode->frequency_list); // Free the frequency list
                markovNode->frequency_list = NULL;
            }

            free(markovNode); // Free the MarkovNode itself
            markovNode = NULL;
        }

        // Free the linked list node
        free(cur);
        cur = next;
    }

    // Free the linked list structure
    free(markovChain->database);

    // Free the MarkovChain structure itself
    free(markovChain);

    *ptr_chain = NULL; // Nullify the pointer to avoid dangling pointers
}



Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr){
    //checks if markov_chain or the database exist.
    if (markov_chain == NULL || markov_chain->database == NULL) {
        return NULL;
    }

    //searching for node
    Node *current = markov_chain->database->first;

    while (current) {
        MarkovNode *markov_node = (MarkovNode *)current->data;

        if (markov_node == NULL || markov_node->data == NULL) {
            fprintf(stderr, "Error: Null MarkovNode or data in the database.\n");
            return NULL;
        }

        //printf("Comparing '%s' with '%s'\n", markov_node->data, data_ptr);

         //Compare data to see if it matches the current node's data
        if (strcmp(markov_node->data, data_ptr) == 0) {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

Node* add_to_database(MarkovChain *markov_chain, char *data_ptr) {
    // Check if markov_chain or the database exists
    if (markov_chain == NULL || markov_chain->database == NULL) {
        return NULL;
    }

    // Check if the node already exists in the database
    Node *checkIfExists = get_node_from_database(markov_chain, data_ptr);
    if (checkIfExists != NULL) {
        return checkIfExists; // Return the existing node
    }

    // Allocate a new MarkovNode
    MarkovNode *newMarkovNode = (MarkovNode *)malloc(sizeof(MarkovNode));
    if (newMarkovNode == NULL) {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }

    // Duplicate the string for newMarkovNode->data



    newMarkovNode->data = strdup(data_ptr);
    if (newMarkovNode->data == NULL) {
        free(newMarkovNode); // Free the MarkovNode structure if strdup fails
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }

    // Initialize frequency_list
    newMarkovNode->frequency_list = NULL;

    // Add the new node to the database
    if (add(markov_chain->database, newMarkovNode) != 0) {
        // If adding to the database fails, clean up allocated memory
        free(newMarkovNode->data);
        free(newMarkovNode);
        fprintf(stderr, "Error: Failed to add node to database.\n");
        return NULL;
    }

    return markov_chain->database->last; // Return the last added node
}


int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node){

    //check if first_node or second_node exist
    if(first_node == NULL || second_node == NULL){
        return 1;
    }


    // Check if the frequency list exists
    if (first_node->frequency_list == NULL) {
        first_node->frequency_list = malloc(2 * sizeof(MarkovNodeFrequency));
        if (first_node->frequency_list == NULL) {
            fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
            return 1;
        }

        // Initialize the new list
        first_node->frequency_list[0].markov_node = second_node;
        first_node->frequency_list[0].frequency = 1; // Initial frequency
        first_node->frequency_list[1].markov_node = NULL; // Null terminator
        first_node->frequency_list->size = 1; // Set size to 1
        return 0;
    }



    //finding the frequencyList size
    int currentSize = first_node->frequency_list->size;


    //checking if the second_node already exists in the frequencyList
    //if it does exist, it adds 1 to it frequency
    for(int i = 0; i< currentSize; i++){
        MarkovNodeFrequency *tmp = &first_node->frequency_list[i];
        if(tmp->markov_node == second_node){
            tmp->frequency++;
            return 0;
        }
    }

    //allocating space for adding the second_node if it doesnt already exists
    MarkovNodeFrequency *newList =
            realloc(first_node->frequency_list, (currentSize +2) * sizeof (MarkovNodeFrequency));


    if(newList == NULL){
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        free(newList);
        return 1;
    }

    //updating the frequencyList after adding the new node
    first_node->frequency_list = newList;
    first_node->frequency_list[currentSize].markov_node = second_node;
    first_node->frequency_list[currentSize].frequency = 1; // Initial frequency
    // Add a null terminator to mark the end of the list
    first_node->frequency_list[currentSize + 1].markov_node = NULL;
    first_node->frequency_list->size++;


    return 0;
}






MarkovNode* get_first_random_node(MarkovChain *markov_chain) {
    //checks if the markovNode and it's dataBase and it's first_node exist
    if (markov_chain == NULL || markov_chain->database == NULL || markov_chain->database->first == NULL) {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }

    Node* current = NULL;
    //this section checks if the word in the random node doesn't end with a '.'
    //it keeps selecting random nods until the word in the node doesn't end with a '.'
    while(1) {

        int randIndex = get_random_number(markov_chain->database->size);

        //pointing at the random node then returning it
        current = markov_chain->database->first;
        for (int i = 0; i < randIndex; i++) {
            current = current->next;
        }

        MarkovNode* markov_node = (MarkovNode*) current->data;
        if(current == NULL || current->data == NULL){
            fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
            return NULL;
        }

        char* word = markov_node->data;
        if(word[strlen(word) - 1] != '.'){
            return markov_node;
        }
    }


}

int calculate_total_frequencies(MarkovNode *markov_node) {
    // Check if the node or its frequency list exists
    if (markov_node == NULL || markov_node->frequency_list == NULL) {
        return 0;
    }

    int total = 0;
    for (int i = 0; i < markov_node->frequency_list->size; i++) {
        total += markov_node->frequency_list[i].frequency;
    }

    return total;
}

MarkovNode* get_next_random_node(MarkovNode *cur_markov_node){
    if(cur_markov_node == NULL || cur_markov_node->frequency_list == NULL){
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        exit(EXIT_FAILURE);
    }

    int frequencies = calculate_total_frequencies(cur_markov_node);

    int randomNode = get_random_number(frequencies);

    int sumOfFrequencies = 0;

    for(int i=0; i< cur_markov_node->frequency_list->size; i++) {
        MarkovNodeFrequency *tmp = &cur_markov_node->frequency_list[i];
        sumOfFrequencies += tmp->frequency;


        if (randomNode < sumOfFrequencies)
            return tmp->markov_node;

    }
    fprintf(stderr, "Failed to get a second node");
    return NULL;
}



void generate_tweet(MarkovNode *first_node, int max_length) {
    if (first_node == NULL) {
        fprintf(stderr, "Error: First node is NULL.\n");
        return;
    }
    MarkovNode *current_node = first_node;
    for (int i = 0; i < max_length && current_node != NULL; i++) {
        if (current_node->frequency_list == NULL || i == max_length - 1) {
            printf("%s", current_node->data);
            break;
        }
        printf("%s ", current_node->data);
        current_node = get_next_random_node(current_node);
    }
    printf("\n");
}