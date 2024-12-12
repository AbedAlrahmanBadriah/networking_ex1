Name: Abdualrahman Badrieh
ID: 322947227

Markov Chain Tweet Generator
What does this program do?
This program uses a Markov Chain model to generate random "tweets" or sentences based on a text corpus provided by the user. The generated sentences mimic the style and structure of the text from the input file.

Functions and their purposes
1. initialize_markov_chain()
Purpose: Initializes and allocates memory for a MarkovChain structure, which contains a database of nodes.
Returns: A pointer to the initialized MarkovChain.

2. add_to_database()
Purpose: Adds a new word to the Markov Chain's database or retrieves an existing node if the word is already present.
Parameters: MarkovChain *markov_chain, char *data_ptr
Returns: A pointer to the node containing the word.

3. add_node_to_frequency_list()
Purpose: Updates the frequency list of a node to record transitions between words.
Parameters: MarkovNode *first_node, MarkovNode *second_node
Returns: 0 for success, 1 for failure.

4. fill_database()
Purpose: Reads an input text file and populates the Markov Chain database with words and their relationships.
Parameters: FILE *fp, int words_to_read, MarkovChain *markov_chain
Returns: 0 for success, 1 for failure.

5. generate_tweet()
Purpose: Generates a tweet (random sentence) starting from a given node, up to a specified maximum length.
Parameters: MarkovNode *first_node, int max_length

6. get_first_random_node()
Purpose: Retrieves a random starting node from the database whose word does not end with a period (.).

7. get_next_random_node()
Purpose: Chooses the next word in the sentence based on the current word's frequency list.

8. free_database()
Purpose: Frees all allocated memory for the Markov Chain, including its database and nodes.

How to run the code:
Compile the program using the following command:
gcc -Wall -Wextra -Wvla -std=c99 tweets_generator.c markov_chain.c linked_list.c -o tweets_generator

Run the program with the following syntax:
./tweets_generator <seed> <number_of_tweets> <file_path> <words_to_read>

<seed>: The seed for random number generation.
<number_of_tweets>: Number of tweets to generate.
<file_path>: Path to the input text file (corpus).
<words_to_read>: Maximum number of words to read from the file.

Example:
./tweets_generator 12345 5 corpus.txt 100

Dependencies
The program depends on three files: tweets_generator.c, markov_chain.c, and linked_list.c.
Include their corresponding headers: markov_chain.h and linked_list.h.

Notes:
Ensure the input file exists and is readable.
The program handles memory allocation errors and cleans up resources upon failure.
