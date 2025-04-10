/* File: indexPage.c */
/* Author: Britton Wolfe */
/* Date: September 3rd, 2010 */

/* This program indexes a web page, printing out the counts of words on that page */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define ARR_SIZE 26
#define BUF_SIZE 300000
#define MAX_ADDR_LENGTH 1000

/* TODO: structure definitions */
struct trieNode {
    struct trieNode *children[ARR_SIZE];

    int wordCount;
    int isEndOfWord;
};


/* NOTE: int return values can be used to indicate errors (typically non-zero)
   or success (typically zero return value) */

/* TODO: change this return type */
struct trieNode* indexPage(const char* url);

int addWordOccurrence(const char* word, const int wordLength, struct trieNode *root
		       /* TODO: other parameters you need */);

void printTrieContents(struct trieNode *node, int outputLength, char *outputBuffer
    /* TODO: any parameters you need */);

int freeTrieMemory(/* TODO: any parameters you need */);

int getText(const char* srcAddr, char* buffer, const int bufSize);

struct trieNode *createNode();

void analyzeBuffer(char *buffer, struct trieNode *root);

int main(int argc, char** argv){
    /* TODO: write the (simple) main function */

    /* argv[1] will be the URL to index, if argc > 1 */

    // stores the url 
    char url[MAX_ADDR_LENGTH];

    // used to output the word counts
    char outputBuffer[BUF_SIZE];

    // gets the url from the command line and stores it in url
    if (argc > 1)
    {
        strncpy(url, argv[1], MAX_ADDR_LENGTH);
        url[MAX_ADDR_LENGTH - 1] = '\0';
    }

    // parses the words found on the webpage, inserts their counts into the trie
    struct trieNode *root = indexPage(url);

    // prints the word counts that were stored in the trie
    printTrieContents(root, 0, outputBuffer);

    // frees the memory that was allocated in the trie
    // freeTrieMemory(root);

    return 0;
}

/* TODO: define the functions corresponding to the above prototypes */

/* TODO: change this return type */
struct trieNode* indexPage(const char* url)
{
    // holds the string that is retrieved from the webpage
    char buffer[BUF_SIZE];

    // creates the root node
    struct trieNode *root = createNode();

    // gets the content of a page
    getText(url, buffer, BUF_SIZE);
    printf("%s\n", url); 

    // analyzes the buffer's content
    analyzeBuffer(buffer, root);

    return root;
}

// creates a new node and initializes child nodes to NULL
struct trieNode *createNode()
{
    // allocates memory for the new node
    struct trieNode *newNode = (struct trieNode*)malloc(sizeof(struct trieNode));
    if (newNode == NULL){
        printf("Could not allocate memory.\n");
        return NULL;
    }

    // sets all the child nodes to NULL
    for(int i = 0; i < ARR_SIZE; i++){
        newNode->children[i] = NULL;
    }

    // defaults the node's data
    newNode->wordCount = 0;
    newNode->isEndOfWord = 0; // boolean, 0 is false
    return newNode;
}

// adds the word and its count to the trie
int addWordOccurrence(const char* word, const int wordLength, struct trieNode *node
		       /* TODO: other parameters you need */)
{
    // if the recursion is at the end of the word, then sets the isEndofWord boolean to true and increments the wordCount by one
    if (word[0] == '\0')
    {
        node->isEndOfWord = 1;
        node->wordCount++;
        return 0;
    }
    
    // stores the ASCII decimal value of the first character in the word
    int charIndex = word[0] - 'a';

    // creates a node for that character, if it didn't previously exist
    if(node->children[charIndex] == NULL)
        node->children[charIndex] = createNode();

    // passes the word, but with the first character removed
    addWordOccurrence(word + 1, wordLength, node->children[charIndex]);

    return 0;
}

// prints the words that are stored in the trie and their number of occurences
void printTrieContents(struct trieNode *node, int bufferIndex, char *outputBuffer
    /* TODO: any parameters you need */)
{
    // immediately returns if the current node is empty
    if(node == NULL)
    {
        printf("trie is empty\n");
        return;
    }

    // prints the word and its word count if the character is the final letter in the word
    if(node->isEndOfWord == 1)
        printf("%s: %d\n", outputBuffer, node->wordCount);

    // loops through all 26 letters in the alphabet
    for(int letterIndex = 0; letterIndex < ARR_SIZE; letterIndex++)
    {
        // only adds the letter into the buffer if the node for the corresponding ASCII char exists
        if(node->children[letterIndex] != NULL)
        {
            // pushes character into the buffer
            outputBuffer[bufferIndex] = letterIndex + 'a'; // converts the ASCII decimal value back into a character ('122' -> 'z')
            outputBuffer[bufferIndex + 1] = '\0'; // adds terminating character to the end of the buffer

            // passes the child node into the function, increments the buffer index
            printTrieContents(node->children[letterIndex], bufferIndex + 1, outputBuffer);
        }
    }

    // pops character out of the buffer once all of the child nodes have been traversed
    outputBuffer[bufferIndex] = '\0';

    return;
}

int freeTrieMemory(/* TODO: any parameters you need */)
{
    return 0;
}

/* You should not need to modify this function */
int getText(const char* srcAddr, char* buffer, const int bufSize){
  FILE *pipe;
  int bytesRead;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getText.py", srcAddr);

  pipe = popen(buffer, "r");
  if(pipe == NULL){
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n",
	    buffer);
    return 0;
  }

  bytesRead = fread(buffer, sizeof(char), bufSize-1, pipe);
  buffer[bytesRead] = '\0';

  pclose(pipe);

  return bytesRead;
}

// analyzes the characters in the buffer, only lowercase letters 
void analyzeBuffer(char *buffer, struct trieNode *root)
{
    // array that stores the word to be added into the trie
    char wordToAdd[BUF_SIZE];
    int wordIndex = 0; // keeps track of where to add the next letter into the char array
    
    // loops until the end of the string is reached
    for(int bufferIndex = 0; buffer[bufferIndex] != '\0'; bufferIndex++)
    {
        // converts the char that is being analyzed into a lowercase letter
        char charToAddToWord = tolower(buffer[bufferIndex]);
        
        // only parses alphabetical letters into terms/words, adds them to the trie 
        if (charToAddToWord >= 'a' && charToAddToWord <= 'z')
        {
            wordToAdd[wordIndex] = charToAddToWord;
            wordIndex++;
        }
        else
        {
            // adds the terminating character to the end of the word
            wordToAdd[wordIndex] = '\0';

            // adds the word occurence into trie if the char array is not empty
            if ((wordToAdd[0] != '\0'))
            {
                printf("\t%s\n", wordToAdd);
                addWordOccurrence(wordToAdd, strlen(wordToAdd), root/* , 0 */);
            }
            
            // clears the array holding the word string
            memset(wordToAdd, '\0', sizeof(wordToAdd));
            wordIndex = 0;
        }
    }
}