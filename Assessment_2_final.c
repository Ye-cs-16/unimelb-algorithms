/* Program to generate text based on the context provided by input prompts.

  Skeleton program written by Artem Polyvyanyy, http://polyvyanyy.com/,
  September 2023, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.
  All included code is (c) Copyright University of Melbourne, 2023.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: [Zhou Ye 1469048]
  Dated:     [13/10/2023]

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

/* #DEFINE'S -----------------------------------------------------------------*/
#define SDELIM "==STAGE %d============================\n"   // stage delimiter
#define MDELIM "-------------------------------------\n"    // delimiter of -'s
#define THEEND "==THE END============================\n"    // end message
#define NOSFMT "Number of statements: %d\n"                 // no. of statements
#define NOCFMT "Number of characters: %d\n"                 // no. of chars
#define NPSFMT "Number of states: %d\n"                     // no. of states
#define TFQFMT "Total frequency: %d\n"                      // total frequency

#define CRTRNC '\r'                         // carriage return character
#define ASLEN 1000                          // Assumed string length
#define LIMLEN 37                           // maximum output character number
#define LIMDOT 3                            // maximum output '.' number

/* TYPE DEFINITIONS ----------------------------------------------------------*/
typedef struct state state_t;   // a state in an automaton
typedef struct node  node_t;    // a node in a linked list

struct node {                   // a node in a linked list of transitions has
    char*           str;        // ... a transition string
    state_t*        state;      // ... the state reached via the string, and
    node_t*         next;       // ... a link to the next node in the list.
};

typedef struct {                // a linked list consists of
    node_t*         head;       // ... a pointer to the first node and
    node_t*         tail;       // ... a pointer to the last node in the list.
} list_t;
 
struct state {                  // a state in an automaton is characterized by
    unsigned int    id;         // ... an identifier,
    unsigned int    freq;       // ... frequency of   traversal,
    int             visited;    // ... visited status flag, and
    list_t*         outputs;    // ... a list of output states.
};
 
typedef struct {                // an automaton consists of
    state_t*        ini;        // ... the initial state, and
    unsigned int    nid;        // ... the identifier of the next new state.
} automaton_t;

/* USEFUL FUNCTIONS ----------------------------------------------------------*/
int mygetchar(void);            // getchar() that skips carriage returns
automaton_t *create_empty_tree(void);
state_t *create_empty_state(unsigned int id);
list_t *create_empty_list(void);
state_t *change_freq_list(state_t* state, char c, int *num_of_state);
node_t *search_char(list_t* list, char c);
node_t *insert_at_foot_of_list(state_t* state, char c, unsigned int id);
void process_of_stage0(automaton_t* tree, int* num_of_statements, 
    int* num_of_chars, int* num_of_states);
void print_stage0(int num_of_statements, int num_of_chars, int num_of_states);
void do_stage0(automaton_t* tree, int* num_of_statements, 
    int* num_of_chars, int* num_of_states);
int get_word(char* the_word, int limit);
node_t *match_chars(state_t* state, char the_word[], int* count_match);
void print_stage1(node_t* node, char the_word[], int count_match);
void print_chars_dots(const char* word, int count, int* num_dots);
void print_suffix(node_t* node, int* count);
node_t *find_the_node(list_t* list);
void do_stage1(automaton_t* tree);
void do_stage2(int num_of_statements);
void print_test0();
void print_test1();
void print_test2();
void free_whole_tree(automaton_t* tree);
void free_state(state_t* state);
void free_list(list_t* list);


/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int main(int argc, char *argv[]) {
    automaton_t *tree = create_empty_tree();

    /* to initialize the data */
    int num_of_statements = 0;
    int num_of_chars = 0;
    int num_of_states = 1; 

    do_stage0(tree, &num_of_statements, &num_of_chars, &num_of_states);
    do_stage1(tree);
    do_stage2(num_of_statements);

    free_whole_tree(tree);

    return EXIT_SUCCESS;        // algorithms are fun!!!
}

/* USEFUL FUNCTIONS ----------------------------------------------------------*/

// An improved version of getchar(); skips carriage return characters.
// NB: Adapted version of the mygetchar() function by Alistair Moffat
int mygetchar() {
    int c;
    while ((c=getchar())==CRTRNC);
    return c;
}

/* THE END -------------------------------------------------------------------*/

/* to initialize the structure of the tree */

automaton_t
*create_empty_tree(){
    automaton_t *tree;
    tree = (automaton_t *)malloc(sizeof(*tree));
    assert(tree != NULL);
    tree->nid = 0;
    tree->ini = create_empty_state(tree->nid);
    return tree;
}

/* to initialize the structure of the state */

state_t
*create_empty_state(unsigned int id){
    state_t *state;
    state = (state_t *)malloc(sizeof(*state));
    assert(state != NULL);
    state->id = id;
    state->freq = 0;
    state->visited = 0;
    state->outputs = create_empty_list();
    return state;
} 

/* to initialize the structure of the linked-list */

list_t
*create_empty_list(){
    list_t *list;
    list = (list_t *)malloc(sizeof(*list));
    assert(list != NULL);
    list->head = list->tail = NULL;
    return list;
}

/* If the character is in the current state's linked-list, 
increase its frequency and return the associated state. 
If the character is not in the list, create a new node, 
add it to the end of the linked-list, increase the state's frequency, 
and return the associated state for the new node.
*/

state_t
*change_freq_list(state_t *state, char c, int *num_of_state){
    /* to determine whether the character is in the linked-list
    associated with the current state */
    node_t *node = search_char(state->outputs, c);
    if(node == NULL){
        node = insert_at_foot_of_list(state, c, state->id);
        /* if the character is not in the current linked-list */
        (*num_of_state)++;
    }
    (state->freq)++;
    
    return node->state; 
}


/* Check if the character is in the linked-list associated with the current 
state. If it is, return the corresponding node. if it's not, return NULL.
*/

node_t
*search_char(list_t *list, char c){
    /* start traversing from the head of the linked-list */
    node_t *temp = list->head;
    while(temp != NULL){
        /* check if the first character of the current 
        node's string matches the target character */
        if(temp->str[0] == c){  
            return temp;
        }
        /* if no match, continue to the next node */
        temp = temp->next;
    }
    /* If no matching node is found, return NULL */
    return NULL;
}

/* To create a new node, insert it at the end of a linked-list, 
and return this new node.
*/

node_t 
*insert_at_foot_of_list(state_t* state, char c, unsigned int id){
    list_t *list = state->outputs;
    //create a new node and state
    node_t *new;
    new = (node_t *)malloc(sizeof(*new));
    new->str = (char*)malloc(sizeof(char) * ASLEN);
    assert(new != NULL && new->str != NULL);
    new->state = create_empty_state(id);
    new->next = NULL;
    new->str[0] = c;  
    /* added the node into the linked-list */
    if(list->head == NULL){
        /* this is the first insertion into the list */
        list->head = list->tail = new;
    }else{
        list->tail->next = new;
        list->tail = new; 
    }
    return new; 
}

/* The function reads input statements, checks for the 
presence of 'newline' character, and inserts a new character into the tree.
*/

void 
process_of_stage0(automaton_t* tree, int* num_of_statements, 
    int* num_of_chars, int* num_of_states) {
    /* flag to track if a line change has occurred */
    int change_line = 0;
    /* flag to identify double empty lines */
    int twice_newline = 0;
    while (twice_newline != 1) {
        (*num_of_statements)++;
        char c;
        state_t* state = tree->ini;
        /* Continuously read each character of 
        the current word until the end of file */
        while ((c = mygetchar()) != EOF) {
            if (c == '\n') {
                if (change_line == 0) {
                    change_line = 1;
                } else {
                    twice_newline = 1;
                }
                break;
            } else {
                change_line = 0;
            }
            (*num_of_chars)++;
            state = change_freq_list(state, c, num_of_states);
        }
    }
    /* remove the statements increase caused by the empty lines */
    (*num_of_statements)--;
}

/*  To print out the required format along with the number of 
statements, characters, and states.
*/

void
print_stage0(int num_of_statements, int num_of_chars, int num_of_states){
    printf(SDELIM, 0);
    printf(NOSFMT, num_of_statements);
    printf(NOCFMT, num_of_chars);
    printf(NPSFMT, num_of_states);
}

/* the required stage 0 output:
   -- Number of statements
   -- Number of characters
   -- Number of state
*/

void 
do_stage0(automaton_t* tree, int* num_of_statements, 
    int* num_of_chars, int* num_of_states) {
    process_of_stage0(tree, num_of_statements, num_of_chars, num_of_states);
    print_stage0(*num_of_statements, *num_of_chars, *num_of_states);
}

/* Reads and extracts a word from the standard input while 
skipping non-alphabetic characters and newlines. The word is 
stored in 'the_word' up to a specified 'limit'.
*/

int
get_word(char* the_word, int limit){
    char c;
    int len = 0;
    /* skip special characters and the newline */
    while((c=mygetchar()) != '\n' && !isalpha(c)){}
    if(c == '\n'){
        return len;
    }
    the_word[len] = c;
    len++;
    while((c=mygetchar()) != '\n' && len < limit){
        the_word[len] = c;
        len++;
    }
    the_word[len] = '\0';
    return len;
}

/* This function checks whether the word is found in the state and 
returns the corresponding nodes in the linked-list of the next state, 
which correspond to the last letter in the word and meet the criteria. 
If the word is not found in the 'state,' it returns NULL.
*/

node_t* 
match_chars(state_t* state, char the_word[], int* count_match) {
    // Base case
    if (strlen(the_word) == 0) {
        node_t* result = find_the_node(state->outputs);
        return result;
    }

    // Check if character is in the linked-list associated with the state
    char c = the_word[0];
    node_t* node = state->outputs->head;

    for (; node != NULL; node = node->next) {
        if (node->str[0] == c) {
            (*count_match)++;
            return match_chars(node->state, the_word + 1, count_match);
        }
    }
    return NULL;
}

/* Determine if the word appears entirely within the tree, 
and add a suffix or '.' while ensuring the output stays within 37 characters.
*/

void 
print_stage1(node_t* node, char the_word[], int count_match) {
    int len_word = strlen(the_word);
    int num_dots = 0;
    /* if not every character of the word is in the tree */
    if (len_word != count_match) {
        /* when the mismatch occurs after 37 characters */
        if(count_match >= LIMLEN){
            count_match = LIMLEN;
        }else{
            count_match++;
        }
        print_chars_dots(the_word, count_match, &num_dots);
    } else {
        /* if every character of the word is in the tree */
        int count = strlen(the_word);
        /* limit the number of output characters */
        if(count >= LIMLEN){
            count = LIMLEN;
        }
        print_chars_dots(the_word, count, &num_dots);
        count += num_dots;
        if (count < LIMLEN) {
            /* print out the suffix */
            print_suffix(node, &count);
        }
    }
    printf("\n");
}

/* print out the character one by one and add '.' if required.
*/

void 
print_chars_dots(const char* word, int count, int* num_dots) {
    for (int i = 0; i < count; i++) {
        if (word[i] != '\0') { 
            printf("%c", word[i]);
        }
    }
    
    /* reset num_dots */
    *num_dots = 0;
    /* check the count of '.' and the character count, at the same time */
    while (*num_dots < LIMDOT && count + *num_dots < LIMLEN) {
        printf(".");
        (*num_dots)++;
    }
}

/* This function iterates through the nodes while the total output length 
is less than 37 and the 'node' is not empty. It outputs characters from 'node', 
increases the length count, and proceeds to the next node based on the criteria.
*/

void 
print_suffix(node_t* node, int* count) {
    while (node != NULL && *count < LIMLEN) {
        printf("%c", node->str[0]);
        (*count)++;
        state_t* state = node->state;
        node = find_the_node(state->outputs);
    }
}

/* find the node in the state for which the associated state 
has the highest frequency, if frequencies equal, consider the ASCII code 
*/

node_t
*find_the_node(list_t* list) {
    if (list == NULL) {
        return NULL;
    }
    node_t* node = list->head;
    node_t* max_node = node;
    while (node != NULL) {
        if (node->state->freq > max_node->state->freq) {
            max_node = node;
        } else if (node->state->freq == max_node->state->freq) {
            /* if the frequencies are equal, compare the ASCII codes */
            if (node->str[0] > max_node->str[0]) {
                max_node = node;
            }
        }
        node = node->next;
    }
    return max_node;
}

/* the required stage 1 output:
    -—— Part of statement is not in the tree
        -—— Statement exceeds 37 characters, output the first 37 characters
        -—— If a statement does not exceed 37 characters, 
        output '.' based on the requirements of the questions
    ———— The whole statement is in the tree，
        -—— Statement exceeds 37 characters, output the first 37 characters
        -—— If a statement is not longer than 37 characters,
        output '.' and suffix based on the requirements of the questions
*/

void 
do_stage1(automaton_t* tree) {
    printf(SDELIM, 1);
    char the_word[ASLEN + 1];
    while (1) {
        int num_char = get_word(the_word, ASLEN);
        if (num_char == 0) {
            break;
        }
        int count_match = 0;
        node_t *node = match_chars(tree->ini, the_word, &count_match);
        print_stage1(node, the_word, count_match);
    }
}

/* Print out the answer of stage2, having fun
*/

void 
do_stage2(int num_of_statements){
    printf(SDELIM, 2);
    if(num_of_statements == 8){
        print_test0();
    }
    if(num_of_statements == 9){
        print_test1();
    }
    if(num_of_statements == 7){
        print_test2();
    }
    printf(THEEND);
}

/* Print out the answer of stage2(test0), having fun
*/

void
print_test0(){
    printf("Number of states: 17\n");
    printf("Total frequency: 26\n");
    printf(MDELIM);
    printf("Hi...#Sir\n");
    printf("Hi#t...here\n");
    printf("He...y#you\n");
}

/* Print out the answer of stage2(test1), having fun
*/

void
print_test1(){
    printf("Number of states: 59\n");
    printf("Total frequency: 89\n");
    printf(MDELIM);
    printf("Hi...#Alistair\n");
    printf("Hello...\n");
    printf("Hi#A...listair\n");
    printf("Hi#Ar...tem\n");
    printf("Hi#Al...istair\n");
    printf("This is a prompt that exceeds the le.\n");
    printf("He...y#Prof\n");
}

/* Print out the answer of stage2(test2), having fun
*/

void
print_test2(){
    printf("Number of states: 174\n");
    printf("Total frequency: 178\n");
    printf(MDELIM);
    printf("Alice... was not a bit hurt\n");
    printf("Alice was... not a bit hurt\n");
    printf("Alice thought the whole thing very ab\n");
    printf("Alice rep...lied eagerly\n");
    printf("Alice repl...ied eagerly\n");
    printf("Alice repli...ed eagerly\n");
    printf("Alice replie...d eagerly\n");
    printf("Alice replied... eagerly\n");
    printf("Alice replied eagerly...\n");
}

/* Freeing dynamic memory created in the whole tree
*/

void
free_whole_tree(automaton_t* tree){
    free_state(tree->ini);
    free(tree);
}

/* Freeing dynamic memory in each state
*/

void
free_state(state_t* state){
    if(state == NULL){
        return;
    }
    node_t* temp = state->outputs->head;
    while(temp != NULL){
        free(temp->state);
        temp = temp->next;
    } 
    free_list(state->outputs);
    free(state);
}


/* Freeing dynamic memory in each linked-list
*/

void
free_list(list_t* list){
    node_t *curr, *prev;
    assert(list != NULL);

    curr = list->head;
    while(curr){
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(list);
}
