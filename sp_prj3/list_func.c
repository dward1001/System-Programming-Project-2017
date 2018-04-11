#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list_func.h"

/*NAME : make_history					*/
/*PURPOSE : to insert a new node in the linked list	*/
/*	to store the history of valid commands		*/
/*RETURN_VALUE : none					*/
void make_history(char* input)
{
	his_list *node, *temp;

	node = (his_list *)calloc(1,sizeof(his_list));
	strcpy(node->cmd,input);
	node->link = NULL;
	if(!hishead){
		hishead = node;
	}
	else{
		for(temp=hishead; temp->link!=NULL; temp=temp->link);
		temp->link =node;
	}
	return;
}
/*NAME : print_history					*/
/*PURPOSE : to print all the nodes in the linked list	*/
/*	of the history of valid commands		*/
/*RETURN_VALUE : none					*/
void print_history(void)
{
	his_list *temp;
	
	for(temp=hishead; temp!=NULL; temp=temp->link){
		printf("%s\n",temp->cmd);
	}
	return;
}

/*NAME : make_hashtable					*/
/*PURPOSE : to insert a new node in the linked lists	*/
/*	of the hash table to store the informtation	*/
/*	of opcodes					*/
/*RETURN_VALUE : none					*/
void make_hashtable(void)
{
	FILE *fp = fopen("opcode.txt","r");
	int i, idx;
	char tmp[4];
	op_table *op, *temp;
	
	for(i=0; i<20; i++){
		ophead[i] = NULL;
	}
	for(i=0; i<NUM_OF_OPCODE; i++){
		op = (op_table *)calloc(1,sizeof(op_table));		
		fscanf(fp,"%02X %s %s",&op->opcode,op->mnem,tmp);
		op->type = tmp[0] - '0';
		op->link = NULL;

		idx = hash_func(i);
		if(!ophead[idx]){
			ophead[idx] = op;
		}
		else{
			for(temp=ophead[idx]; temp->link!=NULL; temp=temp->link);
			temp->link = op;
		}
	}
	fclose(fp);
	return;
}
/*NAME : hash_func					*/
/*PURPOSE : to convert the value of i according to the	*/
/*	specific equation				*/
/*RETURN_VALUE : converted value			*/
int hash_func(int i)
{
	int idx = (i*17 + 19)%20;
	return idx;
}

/*NAME : search_hashtable				*/
/*PURPOSE : to search 'target' in the linked lists of	*/
/*	the hash table					*/
/*RETURN_VALUE : node of 'target' if found,		*/
/*		else NULL				*/
op_table* search_hashtable(char *target)
{
	int i;
	op_table *temp;
	for(i=0; i<20; i++){
		for(temp=ophead[i]; temp!=NULL; temp=temp->link){
			if(!strcmp(temp->mnem,target)){
				return temp;
			}
		}
	}
	return NULL;
}

/*NAME : print_hashtable				*/
/*PURPOSE : to print all the nodes in the linked lists	*/
/*	of the hash table				*/
/*RETURN_VALUE : none					*/
void print_hashtable(void)
{
	int i;
	op_table *temp;
	for(i=0; i<20; i++){
		printf("%d : ",i);
		for(temp=ophead[i]; temp!=NULL; temp=temp->link){
			printf("[%s,%02X] ",temp->mnem,temp->opcode);
			if(temp->link != NULL)
				printf("-> ");
		}
		printf("\n");
	}
}
