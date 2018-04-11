#ifndef __loader__
#define __loader__

/*NAME_LIMIT : maximum length of symbol name*/
/*MAX_RECORD : maximum length of each record line*/
#define NAME_LIMIT 7
#define MAX_RECORD 75

/*data structure to store the breakpoints of the executable file*/
typedef struct BP {
	int addr;
	struct BP *link;
} bp_list;

/*data structure to store the external symbols of the objec file*/
typedef struct ESTAB {
	char name[NAME_LIMIT];
	int name_flag;
	int address;
	int length;
	struct ESTAB *link;
} es_table;

/*bp_head : head of the list of breakpoints*/
/*es_head : head of the list of external symbol table*/
bp_list *bp_head;
es_table *es_head;

/*NAME : loader_main																	*/
/*PURPOSE : to store the external symbol table and		*/
/*	set the start address of each subprogram					*/
/*RETURN_VALUE : 0 if any error, 1 if not 						*/
int loader_main(char *nextcom);
/*NAME : set_progaddr																	*/
/*PURPOSE : to set the beginning load address from 		*/
/*	the parameter input																*/
/*RETURN_VALUE : none																	*/
void set_progaddr(char *nextcom);
/*NAME : modif_memory																	*/
/*PURPOSE : to modify the address indicated as addr		*/
/*	with num : number of bytes needed to modify,			*/
/*	sign & ref : +ref or -ref													*/
/*RETURN_VALUE : none																	*/
void modif_memory(int addr, int num, char sign, int ref);

/*NAME : store_bp																			*/
/*PURPOSE : to store the breakpoints in the node with	*/
/*		the value bp_addr in the linked list						*/
/*RETURN_VALUE : none 																*/
void store_bp(int bp_addr);
/*NAME : handle_bp																		*/
/*PURPOSE : to handle the linked list of breakpoints	*/
/*	if hd_flag=0 : clear, hd_flag=2 : print						*/
/*RETURN_VALUE : none															 		*/
void handle_bp(int hd_flag);
/*NAME : search_bp																		*/
/*PURPOSE : to search the linked list of breakpoints	*/
/*	if target exists																	*/
/*RETURN_VALUE : 1 if found, 0 if not							 		*/
int search_bp(int target);

/*NAME : make_estab																		*/
/*PURPOSE : to make the external symbol table with		*/
/*	name, flag, address, length												*/
/*	flag indicates the kind of the symbol							*/
/*RETURN_VALUE : none																	*/
void make_estab(char *name, int flag, int addr, int len);
/*NAME : search_estab																	*/
/*PURPOSE : to search the external symbol table	if		*/
/*	the node with name exists													*/
/*RETURN_VALUE : address of found symbol if found, 		*/
/*								-1 if not														*/
int search_estab(char *name);
/*NAME : print_estab																	*/
/*PURPOSE : to print the external symbol table				*/
/*RETURN_VALUE : none																	*/
void print_estab(void);

/*NAME : run_prog																			*/
/*PURPOSE : to run the program loaded in the virtual	*/
/*	memory from progaddr															*/
/*RETURN_VALUE : 0 if any error, 1 if not 						*/
int run_prog(void);
/*NAME : target_addr																	*/
/*PURPOSE : to calculate the target address from idx	*/
/*	of virtual memory depending on the format					*/
/*RETURN_VALUE : calculated target address 						*/
int target_addr(int format, int idx);
/*NAME : store_mem																		*/
/*PURPOSE : to store the value in the target address	*/
/*	of the virtual memory															*/
/*RETURN_VALUE : none																	*/
void store_mem(unsigned int target,unsigned int value);
/*NAME : print_reg																		*/
/*PURPOSE : to print all registers										*/
/*RETURN_VALUE : none																	*/
void print_reg(void);

/*NAME : load_init																		*/
/*PURPOSE : to initiate all the values needed					*/
/*RETURN_VALUE : none																	*/
void load_init(void);

#endif
