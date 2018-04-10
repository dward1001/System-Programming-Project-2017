#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "20141550.h"
#include "list_func.h"
#include "assembler.h"

int main(void)
{
	int input_len, com_num, dump_add=0;
	char *input, *token, *saveptr, *tmp;

	make_hashtable();
	input = (char *)calloc(MAX_COM,sizeof(char));
	vir_mem = (int *)calloc(ONE_MEGA,sizeof(int));	

	while(1){
		//input from the command line
		printf("sicsim> ");
		fgets(input, MAX_COM, stdin);
		input_len = strlen(input) - 1;
		input[input_len] = '\0';
		tmp = (char *)calloc(input_len,sizeof(char));
		strcpy(tmp,input);
	
		//after tokenizing the command, check if vaiid	
		token = strtok_r(input, " \t", &saveptr);
		com_num = is_valid_com(token, saveptr);
		
		//if not valid command
		if(!com_num)
			continue;
		//if valid command
		else{
			//execute the function according to com_num
			// 1:h[elp], 2:d[ir], 3:q[uit], 4:hi[story], 5:d[ump], 6:e[dit],
			// 7:f[ill], 8:opcode mnemonic, 9:reset, 10: opcodelist
			switch(com_num){
				case 1 : print_help(); break;
				case 2 : dir_print(); break;
				case 3 : return 0;
				case 4 : make_history(tmp); print_history(); continue; break;
				case 5 : if(!dump_check(saveptr,&dump_add)) continue; break;
				case 6 : if(!edit_memory(saveptr)) continue; break;
				case 7 : if(!fill_memory(saveptr)) continue; break;
				case 8 : printf("opcode is %02X\n",find_opcode(saveptr)); break;
				case 9 : reset_memory(); break;
				case 10 : print_hashtable(); break;
				case 11 : print_file(saveptr); break;
				case 12 : if(!pass1(saveptr)) continue; break;
				case 13 : print_symbol(); break;
				default : break;
			}
			make_history(tmp);
		}
	}

	return 0;
}
int is_valid_com(char* com, char* nextcom)
{
	int check_num;
	char *token1;

	if(!strcmp(com,"h") || !strcmp(com,"help"))
		check_num = 1;
	else if(!strcmp(com,"d") || !strcmp(com,"dir"))
		check_num = 2;
	else if(!strcmp(com,"q") || !strcmp(com,"quit"))
		check_num = 3;
	else if(!strcmp(com,"hi") || !strcmp(com,"history"))
		check_num = 4;
	else if(!strcmp(com,"du") || !strcmp(com,"dump"))
		check_num = 5;
	else if(!strcmp(com,"e") || !strcmp(com,"edit"))
		check_num = 6;
	else if(!strcmp(com,"f") || !strcmp(com,"fill"))
		check_num = 7;
	else if(!strcmp(com,"opcode"))
		check_num = 8;
	else if(!strcmp(com,"reset"))
		check_num = 9;
	else if(!strcmp(com,"opcodelist"))
		check_num = 10;
	else if(!strcmp(com,"type"))
		check_num = 11;
	else if(!strcmp(com,"assemble"))
		check_num = 12;
	else if(!strcmp(com,"symbol"))
		check_num = 13;
	else
		return 0;

	//check if any unnecessary input except the mneumonic
	//or invalid mnemonic
	if(check_num == 8)
	{
		token1 = strtok_r(nextcom," \t",&nextcom);
		if(token1){
			if(!search_hashtable(token1))
				return 0;
		}
		else
			return 0;
	}
	//check if any unnecessary input
	else if(check_num < 5 || (check_num > 8 && check_num < 11) ||check_num == 13)
	{
		if(*nextcom){
			token1 = strtok_r(nextcom," \t",&nextcom);
			if(token1)
				return 0;
		}
	}
	//check if the file exists
	else if(check_num == 11 || check_num == 12)
	{
		token1 = strtok_r(nextcom," \t",&nextcom);
		if(token1){
			if(!search_file(token1,check_num-11))
				return 0;
		}
		else
			return 0;	
	}
	return check_num;	
}
void dir_print(void)
{
	char cwd[100];
	DIR *dp;
	struct dirent *d;
	struct stat buf;
	int i=0;

	//get the information of current directory and open it
	getcwd(cwd, sizeof(cwd));
	dp = opendir(cwd);
	
	//read all the files in the directory
	while((d=readdir(dp)) != NULL){
		//get the status of the file
		stat(d->d_name, &buf);
		fprintf(stdout, "%s", d->d_name);
		//if it is a directory
		if(S_ISDIR(buf.st_mode))
			printf("/");
		//if it is an executable file
		else if(buf.st_mode == 33261)
			printf("*");
		printf("\t");
		if((++i)%4 == 0)
			printf("\n");
	}
	if(i%4 != 0)
		printf("\n");
	//close the directory
	closedir(dp);
	return ;
}

int dump_check(char* nextcom, int* addr)
{
	char *token;
	int start=-1, end=-1, comma_flag=0, i;
	
	//check the number of commas in nextcom	
	for(i=0; nextcom[i]!='\0'; i++){
		if(nextcom[i] == ',')
			comma_flag++;
	}
	//if the number of commas is bigger than 1
	//which implies it is an invalid input
	if(comma_flag > 1)
		return 0;

	//if the input needs two address input
	if(comma_flag){
		//check if the input is valid or not, by tokenizing nextcom
		token = strtok_r(nextcom," \t,",&nextcom);
		if(token){
			if(strlen(token) > 5) return 0;
			//store the start address
			start = hex_to_dec(token);
			if(start < 0 || start >= ONE_MEGA) return 0;
			if(nextcom){
				token = strtok_r(nextcom," \t,",&nextcom);
				if(token){
					if(strlen(token) > 5) return 0;
					//store the end address
					end = hex_to_dec(token);
					if(end < 0 || start > end) return 0;
				}
				else return 0;
			}
			else return 0;
		}
		else return 0;
	}
	//if the input needs one address input or none
	else{
		if(nextcom){
			//check if the input is valid or not, by tokenizing nextcom
			token = strtok_r(nextcom," \t,",&nextcom);
			if(token){
				if(strlen(token) > 5) return 0;
				//store the start address
				start = hex_to_dec(token);
				if(start < 0 || start >= ONE_MEGA) return 0;
				if(nextcom){
					token = strtok_r(nextcom," \t,",&nextcom);
					if(token) return 0;
				}
			}
		}
	}
	//print the value in the memory from start to end
	print_memory(start,end,addr);	
	return 1;
}
int hex_to_dec(char *input)
{
	int len = strlen(input);
	int i, sum=0;
	//calculate the hexademal value to convert into decimal
	for(i=0; i<len; i++){
		if((input[i]>='a' && input[i]<='f')){
			sum += (input[i]-'a'+10)*pow(16.0,len-1.0-i);
		}
		else if(input[i]>='A' && input[i]<='F'){
			sum += (input[i]-'A'+10)*pow(16.0,len-1.0-i);
		}
		else if(input[i]>='0' && input[i]<='9'){
			sum += (input[i]-'0')*pow(16.0,len-1.0-i);
		}
		//if not valid input
		else
			return -1;
	}
	return sum;
}
void print_memory(int start, int end, int *addr)
{
	int i, j, k;
	int stline, stpoint, endline, endpoint;
	char tmp;

	//set the start and end address
	//if start address is not set
	if(start < 0){
		start = *addr;
		end = (*addr) +159;
	}
	//if end address is not set
	else if(end < 0)
		end = start + 159;
	*addr = end + 1;
	//if end address is out of range
	if(end >= ONE_MEGA){
		end = ONE_MEGA - 1;
		*addr = 0;
	}
	if(*addr >= ONE_MEGA)
		*addr = 0;

	//set the starting point and ending point
	stline = start / 16;
	stpoint = start % 16;
	endline = end / 16;
	endpoint = end % 16;

	//print the value in the memory by specific standards
	for(i=stline; i<=endline; i++){
		printf("%05X ",i*16);
		if(stline==endline){
			for(j=0; j<stpoint; j++)
				printf("   ");
			for(j=stpoint; j<=endpoint; j++)
				printf("%02X ", vir_mem[i*16 + j]);
			for(j=endpoint+1; j<16; j++)
				printf("   ");
		}
		else if(i==stline){
			for(j=0; j<stpoint; j++)
				printf("   ");
			for(j=stpoint; j<16; j++)
				printf("%02X ", vir_mem[i*16 + j]);
		}
		else if(i==endline){
			for(j=0; j<=endpoint; j++)
				printf("%02X ", vir_mem[i*16+ j]);
			for(j=endpoint+1; j<16; j++)
				printf("   ");
		}
		else{
			for(j=0; j<16; j++)
				printf("%02X ", vir_mem[i*16 + j]);
		}
		printf("; ");
		for(k=0; k<16; k++){
			tmp = vir_mem[i*16 + k];
			if(tmp>=32 && tmp<=126)
				printf("%c", tmp);
			else
				printf(".");
		}
		printf("\n");
	}

	return;
}

void print_help(void)
{
	//print all the commands available
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\n");
	printf("f[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
	printf("assemble filename\ntype filename\nsymbol\n");
	return ;
}
int edit_memory(char *nextcom)
{	
	char *token;
	int i, target, value, comma_flag=0;
	
	//check the number of commas in nextcom	
	for(i=0; nextcom[i]!='\0'; i++){
		if(nextcom[i] == ',')
			comma_flag++;
	}
	//if the number of commas is not 1
	//which implies it is an invalid input
	if(comma_flag != 1)
		return 0;

	//check if the input is valid or not, by tokenizing nextcom
	token = strtok_r(nextcom," \t,",&nextcom);
	if(token){
		if(strlen(token) > 5) return 0;
		//store the target address
		target = hex_to_dec(token);
		if(target < 0 || target >= ONE_MEGA) return 0;
		if(nextcom){
			token = strtok_r(nextcom," \t,",&nextcom);
			if(!token || strlen(token)>2) return 0;
			else{
				//store the value inputted
				value = hex_to_dec(token);
				if(value < 0) return 0;
			}
		}
		else return 0;
	}
	else return 0;
	//edit the value of target address to value
	vir_mem[target] = value;	
	return 1;
}
int fill_memory(char *nextcom)
{
	char *token;
	int i, start, end, value, comma_flag=0;
	
	//check the number of commas in nextcom	
	for(i=0; nextcom[i]!='\0'; i++){
		if(nextcom[i] == ',')
			comma_flag++;
	}
	//if the number of commas is not 2
	//which implies it is an invalid input
	if(comma_flag != 2)
		return 0;

	//check if the input is valid or not, by tokenizing nextcom
	token = strtok_r(nextcom," \t,",&nextcom);
	if(token){
		if(strlen(token) > 5) return 0;
		//store the start address
		start = hex_to_dec(token);
		if(start < 0 || start >= ONE_MEGA) return 0;
		if(nextcom){
			token = strtok_r(nextcom," \t,",&nextcom);
			if(token){
				if(strlen(token) > 5) return 0;
				//store the end address
				end = hex_to_dec(token);
				if(start > end || end >= ONE_MEGA) return 0;
				if(nextcom){
					token = strtok_r(nextcom," \t,",&nextcom);
					if(!token || strlen(token)>2) return 0;
					else{
						//store the value
						value = hex_to_dec(token);
						if(value < 0) return 0;
					}
				}
				else return 0;
			}
			else return 0;
		}
		else return 0;
	}
	else return 0;
	//fill value in the memory from start to end address
	for(i=start; i<=end; i++)
		vir_mem[i] = value;
	return 1;
}
void reset_memory(void)
{
	int i;
	//reset values in the whole memory
	for(i=0; i<ONE_MEGA; i++)
		vir_mem[i] = 0;	
}

int find_opcode(char *nextcom)
{
	char *token;
	
	//search the mnemonic in the hashtable by tokenizing nextcom
	token = strtok_r(nextcom," \t",&nextcom);
	return search_hashtable(token)->opcode;
}
//flag=0 : type / flag=1 : assemble
int search_file(char *nextcom, int flag)
{
	char cwd[100];
	DIR *dp;
	struct dirent *d;
	struct stat buf;
	int i;

	//get the information of current directory and open it
	getcwd(cwd, sizeof(cwd));
	dp = opendir(cwd);
	
	//read all the files in the directory
	while((d=readdir(dp)) != NULL){
		//if the command is type
		if(!flag){
			stat(d->d_name, &buf);
			//if directory
			if(S_ISDIR(buf.st_mode))
				continue;
			//if found
			if(!strcmp(nextcom, d->d_name)){
				closedir(dp);
				return 1;
			}
		}
		//if the command is assemble
		else{
			//if found with the name
			if(!strcmp(nextcom, d->d_name)){
				//if the file ends with .asm
				if(!strcmp(nextcom+strlen(nextcom)-4,".asm")){
					closedir(dp);
					return 1;
				}
			}	
		}
	}
	//if not found
	printf("ERROR : No Such File!\n");
	//close the directory
	closedir(dp);
	return 0;
}
void print_file(char *nextcom)
{
	char *token = strtok_r(nextcom," \t",&nextcom);
	FILE *fp = fopen(token,"r");
	char *line;
	
	//get one line from fp
	while(fgets(line,1000,fp))
		//put one line to stdout
		fputs(line,stdout);
	printf("\n");

	fclose(fp);
	return;
}
