
/*MAX_LINE : maximum length of each line in file*/
/*MAX_NAME : maximum length of each argument*/
/*MAX_REC : maximum length of Text Record*/
#define MAX_LINE 150
#define MAX_NAME 30
#define MAX_REC 61

/*data structure to store the history of valid commands*/
typedef struct SYMTAB {
	char label[MAX_NAME];
	int locctr;
	struct SYMTAB *link;
} sym_tab;

/*data structure to store the history of valid commands*/
typedef struct MODI {
	int disp;
	int pt;
	struct MODI *link;
} modirec;

/*sym_head : real symbol table of this program*/
/*sym_temp : temporary symbol table*/
sym_tab *sym_head;
sym_tab *sym_temp;

/*modi_head : head of modification record list*/
modirec *modi_head;

/*NAME : pass1																*/
/*PURPOSE : to store the symbol table and the location*/
/*	counter of each command by reading through the file*/
/*RETURN_VALUE : 0 if any error, 1 if not 		*/
int pass1(char *nextcom);

/*NAME : insert_symbol								*/
/*PURPOSE : to insert a new node in the linked list*/
/*	to store the symbols in the file	*/
/*RETURN_VALUE : none							 		*/
void insert_symbol(char* sym, int addr);

/*NAME : search_symbol					*/
/*PURPOSE : to search 'sym' in the linked list	*/
/*	of the symbol table					*/
/*RETURN_VALUE : the location counter of 'sym' 	*/
/*		if found, -1 if not found		*/
int search_symbol(char* sym);

/*NAME : print_symbol					*/
/*PURPOSE : to print all the nodes in the linked list*/
/*	of the symbol table				*/
/*RETURN_VALUE : none 				*/
void print_symbol(void);

/*NAME : pass2					*/
/*PURPOSE : to store the object code of each command*/
/*	and make the list file and object code			*/
/*RETURN_VALUE : 0 if any error, 1 if not 			*/
int pass2(int start_addr, int proglen, char* filename);

/*NAME : is_regist					*/
/*PURPOSE : to check if the inpus is register of not*/
/*RETURN_VALUE : the number of register if register */
/*		-1 if not a register*/
int is_regist(char *input);

/*NAME : make_modi					*/
/*PURPOSE : to insert a new node in the linked list*/
/*	to store the modification record		*/
/*RETURN_VALUE : none										*/
void make_modi(int disp, int point);

/*NAME : make_final					*/
/*PURPOSE : to make the final list and object file*/
/*RETURN_VALUE : none					 		*/
void make_final(char* filename);

