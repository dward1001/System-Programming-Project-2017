
/*MAX_COM : the maximum length of the input	*/
/*ONE_MEGA : representing 1MB in decimal	*/
#define MAX_COM 100
#define ONE_MEGA 1048576

/*vir_mem : virtual memory in Shell*/
int *vir_mem;

/*NAME : is_valid_com					*/
/*PURPOSE : to check if the inpus is valid or not	*/
/*	except dump, edit, fill				*/
/*RETURN_VALUE : 0 if not valid, 1 if valid 		*/
/*		except dump, edit, fill			*/
int is_valid_com(char* com, char* nextcom);

/*NAME : dir_print					*/
/*PURPOSE : to print all the files in the current	*/
/*	directory					*/
/*RETURN_VALUE : none					*/
void dir_print(void);

/*NAME : dump_check					*/
/*PURPOSE : to check dump command if it is valid	*/
/*	and print by the input of command 		*/
/*RETURN_VALUE : 0 if not valid, 1 if valid		*/
int dump_check(char* nextcom, int* addr);

/*NAME : hex_to_dec					*/
/*PURPOSE : to convert hexadecimal input into decimal	*/
/*RETURN_VALUE : -1 if not valid			*/
/*		decimal value of hexadecimal input	*/
/*		if valid				*/
int hex_to_dec(char* input);

/*NAME : print_memory					*/
/*PURPOSE : to print the memory from start to end	*/
/*	and store the next address in addr		*/
/*RETURN_VALUE : none					*/
void print_memory(int start, int end, int* addr);

/*NAME : print_help					*/
/*PURPOSE : to print all the commands			*/
/*RETURN_VALUE : none					*/
void print_help(void);

/*NAME : edit_memory					*/
/*PURPOSE : to check edit command if it is valid	*/
/*	and edit the value in specific memory 		*/
/*RETURN_VALUE : 0 if not valid, 1 if valid		*/
int edit_memory(char *nextcom);

/*NAME : fill_memory					*/
/*PURPOSE : to check fill command if it is valid	*/
/*	and fill the specific range of the memory 	*/
/*	by the value inputted				*/
/*RETURN_VALUE : 0 if not valid, 1 if valid		*/
int fill_memory(char *nextcom);

/*NAME : reset_memory					*/
/*PURPOSE : to reset the values in the whole memory	*/
/*RETURN_VALUE : none					*/
void reset_memory(void);

/*NAME : find_opcode					*/
/*PURPOSE : to find the opcode of the mnemonic inputted	*/
/*RETURN_VALUE : opcode of the mnemonic inputted	*/
/*		if found, -1 if not			*/
int find_opcode(char *nextcom);

/*NAME : search_file					*/
/*PURPOSE : to find the file with the name inputted*/
/*	if flag==0, type command / if flag==1, assemble command*/
/*RETURN_VALUE : 1 if found / 0 if not found 	*/
int search_file(char *nextcom, int flag);


/*NAME : print_file					*/
/*PURPOSE : to print the content of the file inputted	*/
/*RETURN_VALUE : none				*/
void print_file(char *nextcom);
