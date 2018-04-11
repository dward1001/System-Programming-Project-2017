#ifndef __list_func__
#define __list_func__

/*MAX_MNEMONIC : the maximum length of mnemonic	*/
/*NUM_OF_OPCODE : the number of existing opcodes*/
#define MAX_MNEMONIC 7
#define NUM_OF_OPCODE 58

/*data structure to store the history of valid commands*/
typedef struct HST {
	char cmd[50];
	struct HST *link;
} his_list;

/*data structure to store the information of opcodes	*/
typedef struct HASHT {
	int opcode;
	char mnem[MAX_MNEMONIC];
	int type;
	struct HASHT *link;
} op_table;

/*hishead : head of the linked list of history	*/
/*ophead : the array of head of the linked list	*/
/*	of the hash table			*/
his_list *hishead;
op_table *ophead[20];

void make_history(char* input);
void print_history(void);

void make_hashtable(void);
int hash_func(int i);
op_table* search_hashtable(char *target);
void print_hashtable(void);

#endif
