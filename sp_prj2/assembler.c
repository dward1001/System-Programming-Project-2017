#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "assembler.h"
#include "list_func.h"

int pass1(char *nextcom)
{
	//store the file name in filename
	char *filename = strtok_r(nextcom," \t\n",&nextcom);
	FILE *fp1 = fopen(filename,"r");
	FILE *fp2 = fopen("itmd.txt","w");
	
	char *line = (char *)calloc(MAX_LINE,sizeof(char));
	char *line_tmp, *nextptr, *temp, *LABEL, *OPCODE, *token;
	int line_cnt=5, i, start_addr=0, start_flag=0, loc_cnt=0, plus_flag=0;
	int operand=0, len, loc_temp, proglen;
	op_table *target;

	filename[strlen(filename)-4] = '\0';
	//get one line from fp1
	while(fgets(line,MAX_LINE,fp1)){
		if(feof(fp1)) break;
		
		line_tmp = (char *)calloc(strlen(line)+1,sizeof(char));
		strcpy(line_tmp,line);
		LABEL = (char *)calloc(MAX_NAME+1,sizeof(char));
		LABEL[0] = '\0';
		OPCODE = (char *)calloc(MAX_NAME+1,sizeof(char));
	
		//if the first line
		if(line_cnt == 5){
			token = strtok_r(line," \t\n",&nextptr);
			if(token){
				if(!strcmp(token,"START"))
					start_flag = 1;
				else{
					if(*nextptr)
						temp = strtok_r(nextptr," \t\n",&nextptr);
					if(temp){
						if(!strcmp(temp,"START"))
							start_flag = 1;
					}
				}
				//if START included
				if(start_flag){
					token = strtok_r(nextptr," \t\n",&nextptr);
					if(token){
						//calculate the starting address
						for(i=0; token[i]!='\0'; i++){
							if(token[i] < '0' || token[i] > '9'){
								printf("**ERROR** Line %d : Invalid Starting Address\n", line_cnt);
								return 0;
							}
							else
								start_addr += (token[i]-'0')*pow(16.0,strlen(token)-1.0-i);
						}
						//set the first location counter
						loc_cnt = start_addr;
						fprintf(fp2,"%5d %04d %s",line_cnt,loc_cnt,line_tmp);
					}
					else{
						printf("**ERROR** Line %d : Invalid Starting Address\n", line_cnt);
						return 0;
					}
				}
				//if START not included
				else{
				/*************************************************/
					strcpy(line, line_tmp);
					loc_temp = loc_cnt;
					token = strtok_r(line_tmp," \t\n",&nextptr);
					if(token){
						//if not a comment
						if(token[0] != '.'){
							//if the token includes '+'
							if(token[0] == '+'){
								token++;
								plus_flag = 1;
							}
							//if found in opcode table
							if((target = search_hashtable(token))){
								strcpy(OPCODE,token);
								//if format 4
								if(plus_flag && (target->type)==3)
									loc_cnt += 4;
								//if format 1, 2, 3
								else
									loc_cnt += (target->type);
							}
							//if not found in opcode table
							else{
								//if BASE
								if(!strcmp(token,"BASE"))
									strcpy(OPCODE,token);
								//if not BASE
								else{
									//if END
									if(!strcmp(token,"END"))
										break;
									//if LABEL starts with the number
									if(token[0]>='0' && token[0]<='9'){
										printf("**ERROR** Line %d : Invalid Label Name\n", line_cnt);
										return 0;
									}
									//if valid LABEL
									else{
										strcpy(LABEL,token);
										if(nextptr){
											token = strtok_r(nextptr," \t\n",&nextptr);
											if(token){
												//if OPCODE = WORD
												if(!strcmp(token,"WORD"))
													loc_cnt += 3;
												//if OPCODE = RESW
												else if(!strcmp(token,"RESW")){
													token = strtok_r(nextptr," \t\n",&nextptr);
													if(token){
														len = strlen(token);
														//calculate the operand
														for(i=0; i<len; i++){
															if(token[i]<'0' || token[i]>'9'){
																printf("**ERROR** Line %d : Invalid OPERAND\n", line_cnt);
																return 0;
															}
															else
																operand += (token[i]-'0')*pow(10.0,len-i-1.0);
														}
														loc_cnt += 3*operand;
													}
													else{
														printf("**ERROR** Line %d : Need OPERAND\n", line_cnt);
														return 0;
													}
												}
												//if OPCODE = RESB
												else if(!strcmp(token,"RESB")){
													token = strtok_r(nextptr," \t\n",&nextptr);
													if(token){
														len = strlen(token);
														//calculate the operand
														for(i=0; i<len; i++){
															if(token[i]<'0' || token[i]>'9'){
																printf("**ERROR** Line %d : Invalid OPERAND\n", line_cnt);
																return 0;
															}
															else
																operand += (token[i]-'0')*pow(10.0,len-i-1.0);
														}
														loc_cnt += operand;
													}
													else{
														printf("**ERROR** Line %d : Need OPERAND\n", line_cnt);
														return 0;
													}
												}
												//if OPCODE = BYTE
												else if(!strcmp(token,"BYTE")){
													token = strtok_r(nextptr," \t\n",&nextptr);
													if(token){
														len = strlen(token);
														//if it starts with 'C'
														if(token[0] == 'C'){
															if(token[1] == 39 && token[len-1] == 39){
																//calculate the length of operand
																for(i=2;i<len-1;i++)
																	operand++;
																loc_cnt += operand;
															}
															else{
																printf("**ERROR** Line %d : Invalid OPERAND\n", line_cnt);
																return 0;
															}
														}
														//if it starts with 'X'
														else if(token[0] == 'X'){
															if(token[1] == 39 && token[len-1] == 39){
																//calculate the operand
																for(i=2;i<len-1;i++){
																	if((token[i]>='0' && token[i]<='9') || (token[i]>='A' && token[i]<='F'))
																		operand++;
																	else{
																		printf("**ERROR** Line %d : Invalid OPERAND\n", line_cnt);
																		return 0;
																	}
																}
																loc_cnt += operand/2 + operand%2;
															}
															else{
																printf("**ERROR** Line %d : Invalid OPERAND\n", line_cnt);
																return 0;
															}
														}
														else{
															printf("**ERROR** Line %d : Invalid OPERAND\n", line_cnt);
															return 0;
														}
													}
													else{
														printf("**ERROR** Line %d : Need OPERAND\n", line_cnt);
														return 0;
													}	
												}
												else{
													if(token[0] == '+'){
														token++;
														plus_flag = 1;
													}
													//if OPCODE
													if((target = search_hashtable(token))){
														strcpy(OPCODE,token);
														//if format 4
														if(plus_flag && (target->type)==3)
															loc_cnt += 4;
														//if format 1, 2, 3
														else
															loc_cnt += (target->type);
													}
													else{
														printf("**ERROR** Line %d : Invalid OPCODE\n", line_cnt);
														return 0;
													}
												}
											}
											else{
												printf("**ERROR** Line %d : Need More Arguments\n", line_cnt);
												return 0;
											}
										}
										else{
											printf("**ERROR** Line %d : Need More Arguments\n", line_cnt);
											return 0;
										}
									}
								}
							}
							//if LABEL exists
							if(*LABEL){
								//insert into the symbol table if not exists
								if(search_symbol(LABEL)<0)
									insert_symbol(LABEL,loc_temp);
								else{
									printf("**ERROR** Line %d : Symbol Duplicated!\n", line_cnt);
									return 0;
								}
								LABEL[0] = '\0';
							}
							if(!strcmp(OPCODE,"BASE"))
								fprintf(fp2,"%5d \t  %s",line_cnt,line);
							else
								fprintf(fp2,"%5d %04X %s",line_cnt,loc_temp,line);
						}
						//if a comment
						else{
							fprintf(fp2,"%5d \t   %s",line_cnt,line);
						}
					}
				}
				/*************************************************/
			}
		}
		//if not first line
		else{
			/*************************************************/
			strcpy(line_tmp, line);
			loc_temp = loc_cnt;
			token = strtok_r(line," \t\n",&nextptr);
			if(token){
				if(token[0] != '.'){
					if(token[0] == '+'){
						token++;
						plus_flag = 1;
					}
					if((target = search_hashtable(token))){
						strcpy(OPCODE,token);
						if(plus_flag && (target->type)==3)
							loc_cnt += 4;
						else
							loc_cnt += (target->type);
					}
					else{
						if(!strcmp(token,"BASE"))
							strcpy(OPCODE,token);
						else{
							if(!strcmp(token,"END"))
								break;
							if(token[0]>='0' && token[0]<='9'){
								printf("**ERROR** Line %d : Invalid Label Name\n", line_cnt);
								return 0;
							}
							else{
								strcpy(LABEL,token);
								if(nextptr){
									token = strtok_r(nextptr," \t\n",&nextptr);
									if(token){
										if(!strcmp(token,"WORD"))
											loc_cnt += 3;
										else if(!strcmp(token,"RESW")){
											token = strtok_r(nextptr," \t\n",&nextptr);
											if(token){
												len = strlen(token);
												for(i=0; i<len; i++){
													if(token[i]<'0' || token[i]>'9'){
														printf("**ERROR** Line %d : Invalid OPERAND\n", line_cnt);
														return 0;
													}
													else
														operand += (token[i]-'0')*pow(10.0,len-i-1.0);
												}
												loc_cnt += 3*operand;
											}
											else{
												printf("**ERROR** Line %d : Need OPERAND\n", line_cnt);
												return 0;
											}
										}
										else if(!strcmp(token,"RESB")){
											token = strtok_r(nextptr," \t\n",&nextptr);
											if(token){
												len = strlen(token);
												for(i=0; i<len; i++){
													if(token[i]<'0' || token[i]>'9'){
														printf("**ERROR** Line %d : Invalid OPERAND\n", line_cnt);
														return 0;
													}
													else
														operand += (token[i]-'0')*pow(10.0,len-i-1.0);
												}
												loc_cnt += operand;
											}
											else{
												printf("**ERROR** Line %d : Need OPERAND\n", line_cnt);
												return 0;
											}
										}
										else if(!strcmp(token,"BYTE")){
											token = strtok_r(nextptr," \t\n",&nextptr);
											if(token){
												len = strlen(token);
												if(token[0] == 'C'){
													if(token[1] == 39 && token[len-1] == 39){
														for(i=2;i<len-1;i++)
															operand++;
														loc_cnt += operand;
													}
													else{
														printf("**ERROR** Line %d : Invalid OPERAND\n", line_cnt);
														return 0;
													}
												}
												else if(token[0] == 'X'){
													if(token[1] == 39 && token[len-1] == 39){
														for(i=2;i<len-1;i++){
															if((token[i]>='0' && token[i]<='9') || (token[i]>='A' && token[i]<='F'))
																operand++;
															else{
																printf("**ERROR** Line %d : Invalid OPERAND\n", line_cnt);
																return 0;
															}
														}
														loc_cnt += operand/2 + operand%2;
													}
													else{
														printf("**ERROR** Line %d : Invalid OPERAND\n", line_cnt);
														return 0;
													}
												}
												else{
													printf("**ERROR** Line %d : Invalid OPERAND\n", line_cnt);
													return 0;
												}
											}
											else{
												printf("**ERROR** Line %d : Need OPERAND\n", line_cnt);
												return 0;
											}	
										}
										else{
											if(token[0] == '+'){
												token++;
												plus_flag = 1;
											}
											if((target = search_hashtable(token))){
												strcpy(OPCODE,token);
												if(plus_flag && (target->type)==3)
													loc_cnt += 4;
												else
													loc_cnt += (target->type);
											}
											else{
												printf("**ERROR** Line %d : Invalid OPCODE\n", line_cnt);
												return 0;
											}
										}
									}
									else{
										printf("**ERROR** Line %d : Need More Arguments\n", line_cnt);
										return 0;
									}
								}
								else{
									printf("**ERROR** Line %d : Need More Arguments\n", line_cnt);
									return 0;
								}
							}
						}
					}
					if(*LABEL){
						if(search_symbol(LABEL)<0)
							insert_symbol(LABEL,loc_temp);
						else{
							printf("**ERROR** Line %d : Symbol Duplicated!\n", line_cnt);
							return 0;
						}
						LABEL[0] = '\0';
					}
					if(!strcmp(OPCODE,"BASE"))
						fprintf(fp2,"%5d \t   %s",line_cnt,line_tmp);
					else
						fprintf(fp2,"%5d %04X %s",line_cnt,loc_temp,line_tmp);
				}
				//if a comment
				else{
					fprintf(fp2,"%5d \t   %s",line_cnt,line_tmp);
				}
			}
			/*************************************************/
		}
		operand = 0;
		plus_flag = 0;
		line_cnt += 5;
	}
	fprintf(fp2,"%5d \t   %s",line_cnt,line_tmp);
	//store the program length;
	proglen = loc_cnt - start_addr;

	fclose(fp1);
	fclose(fp2);
	//call pass2 if no error
	return pass2(start_addr,proglen,filename);
}
void insert_symbol(char* sym, int addr)
{
	sym_tab *node, *temp;

	node = (sym_tab *)calloc(1,sizeof(sym_tab));
	strcpy(node->label, sym);
	node->locctr = addr;
	node->link = NULL;
	if(!sym_temp)
		sym_temp = node;
	else{
		if(strcmp(sym_temp->label,node->label) > 0){
			node->link = sym_temp;
			sym_temp = node;
		}
		else if(!(sym_temp->link))
			sym_temp->link = node;
		else{
			for(temp=sym_temp; temp->link!=NULL; temp=temp->link){
				if(strcmp(temp->label,node->label) < 0){
					if(strcmp(temp->link->label,node->label) > 0)
						break;
				}
			}
			node->link = temp->link;
			temp->link = node;
		}
	}
	return;
}
int search_symbol(char* sym)
{
	sym_tab *temp;
	for(temp=sym_temp; temp!=NULL; temp=temp->link){
		if(!strcmp(temp->label, sym))
			return temp->locctr;
	}
	return -1;
}
void print_symbol(void)
{
	sym_tab *temp;

	for(temp=sym_head; temp!=NULL; temp=temp->link)
		printf("\t%s\t%04X\n",temp->label,temp->locctr);
	return;	
}
int pass2(int start_addr, int proglen, char* filename)
{
	FILE *fp = fopen("itmd.txt","r");
	FILE *fp1 = fopen("lst.txt","w");
	FILE *fp2 = fopen("obj.txt","w");	
	
	char *line = (char *)calloc(MAX_LINE,sizeof(char));
	char *line_tmp = (char *)calloc(MAX_LINE,sizeof(char));
	char *token = (char *)calloc(MAX_NAME,sizeof(char)); 
	char *nextptr, *arg1, *arg2;
	char *token1 = (char *)calloc(MAX_NAME,sizeof(char));
	char *record = (char *)calloc(MAX_REC,sizeof(char));
	
	int linenum=0, loc, start_flag=0, plus_flag=0, objcode=0, comma_flag=0, type;
	int len, i, op_addr=0, op_addr2=0, ni, xbpe=0, disp, base_addr=-1, rec_len=0, rec_start;
	op_table *target = (op_table*)calloc(1,sizeof(op_table));
	modirec *tmp;

	fgets(line,MAX_LINE,fp);
	strcpy(line_tmp,line);
	while(token = strtok_r(line_tmp," \t\n",&line_tmp)){
		if(!strcmp(token,"START")){
			start_flag = 1;
			fprintf(fp1,"%s",line);
			break;
		}
		strcpy(token1,token);
	}
	if(start_flag){
		sscanf(line,"%d %d",&linenum,&loc);
		fprintf(fp2,"H%-6s%06X%06X\n",token1,start_addr,proglen);
	}
	else
		fprintf(fp2,"H%-6s%06X%06X\n","TEMP",start_addr,proglen);
	
	do{
		if(feof(fp)) break;
		if(start_flag && linenum == 5){
			start_flag = 0;
			continue;
		}
		strcpy(line_tmp,line);
		line[strlen(line)-1] = '\0';
		token = strtok_r(line_tmp," \t\n",&nextptr);	//line number
		sscanf(token,"%d",&linenum);
		token = strtok_r(nextptr," \t\n",&nextptr);	//location counter
		
		if(!strcmp(token,"END"))
			break;
		else if(token[0] == '.' || !strcmp(token,"BASE")){
			if(!strcmp(token,"BASE")){
				token = strtok_r(nextptr," \t\n",&nextptr);
				if(token){
					if((base_addr = search_symbol(token)) < 0){
						printf("**ERROR** Line %d : Invalid Operand!\n", linenum);
						return 0;
					}
				}
				else{
					printf("**ERROR** Line %d : Need OPERAND\n", linenum);
					return 0;
				}
			}
			fprintf(fp1,"%-100s\n",line);
			continue;
		}
		else{
			sscanf(token,"%X",&loc);
			token = strtok_r(nextptr," \t\n",&nextptr);	//label or opcode
			if(token[0] == '+'){
				plus_flag = 1;
				token++;
			}
			//if the line includes LABEL
			if(!(target = search_hashtable(token))){
				token = strtok_r(nextptr," \t\n",&nextptr);	//opcode or variable
				if(!strcmp(token,"BYTE")){
					token = strtok_r(nextptr," \t\n",&nextptr);
					len = strlen(token);
					if(token[0] == 'C'){
						for(i=2; i<len-1; i++)
							objcode += token[i]*pow(16.0,2.0*(len-i-2));
						type = (len-3)*2;
					}
					else{
						sscanf(token+2,"%X",&objcode);
						type = 2;
					}
					fprintf(fp1,"%-100s%02X\n",line,objcode);
				}
				else if(!strcmp(token,"WORD")){
					token = strtok_r(nextptr," \t\n",&nextptr);
					sscanf(token,"%d",&objcode);
					type = 6;
					fprintf(fp1,"%-100s%06X\n",line,objcode);
				}
				else if(!strcmp(token,"RESW") || !strcmp(token,"RESB")){
					fprintf(fp1,"%-100s\n",line);
					type = 0;
				}
				else{
					if(token[0] == '+'){
						plus_flag = 1;
						token++;
					}
					target = search_hashtable(token);
					if(target){
						if((target->type != 1) && (strcmp(token,"RSUB"))){
							len = strlen(nextptr);
							for(i=0; i<len; i++){
								if(nextptr[i] == ',')
									comma_flag = 1;
							}
							token1 = strtok_r(nextptr," \t\n",&nextptr);
							if(token1){
								if(target->type == 2){
									if(!strcmp(token,"CLEAR") || !strcmp(token,"TIXR")){
										if((op_addr = is_regist(token1)) < 0){
											printf("**ERROR** Line %d : Operand needs to be a Register!\n", linenum);
											return 0;
										}
									}
									else if(!strcmp(token,"SVC")){
										len = strlen(token1);
										for(i=0; i<len; i++){
											if((token1[i]>='0' && token1[i]<='9') || (token1[i]>='A' && token1[i]<='F'))
												continue;
											else{
												printf("**ERROR** Line %d : Operand needs to be a Number!\n", linenum);
												return 0;
											}
										}
										sscanf(token1,"%X",&op_addr2);
									}
									else if(!strcmp(token,"SHIFTL") || !strcmp(token,"SHIFTR")){
										arg2 = strtok_r(nextptr," \t\n,",&nextptr);
										arg1 = strtok_r(token1," \t\n,",&nextptr);
										if(!arg1 || !arg2){
											printf("**ERROR** Line %d : Need More Operand!\n", linenum);
											return 0;
										}
										if((op_addr = is_regist(arg1)) < 0){
											printf("**ERROR** Line %d : Operand needs to be a Register!\n", linenum);
											return 0;
										}
										len = strlen(arg2);
										for(i=0; i<len; i++){
											if((arg2[i]>='0' && arg2[i]<='9') || (arg2[i]>='A' && arg2[i]<='F'))
												continue;
											else{
												printf("**ERROR** Line %d : Operand needs to be a Number!\n", linenum);
												return 0;
											}
										}
										sscanf(arg2,"%X",&op_addr2);
									}
									else{
										arg2 = strtok_r(nextptr," \t\n,",&nextptr);
										arg1 = strtok_r(token1," \t\n,",&nextptr);
										if(!arg1 || !arg2){
											printf("**ERROR** Line %d : Need More Operand!\n", linenum);
											return 0;
										}
										if((op_addr = is_regist(arg1)) < 0){
											printf("**ERROR** Line %d : Operand needs to be a Register!\n", linenum);
											return 0;
										}
										if((op_addr2 = is_regist(arg2)) < 0){
											printf("**ERROR** Line %d : Operand needs to be a Register!\n", linenum);
											return 0;
										}	
									}
									objcode += (target->opcode)*pow(16.0,2.0) + op_addr*16 + op_addr2;
									type = 4;
									fprintf(fp1,"%-100s%04X\n",line,objcode);
								}
								//if the type of OPCODE is 3/4
								else{
									if(token1[0] == '#'){
										ni = 1;
										token1++;
									}
									else if(token1[0] == '@'){
										ni = 2;
										token1++;
									}
									else	ni = 3;
										
									len = strlen(token1);
									if(comma_flag){
										if(!strcmp(token1+len-2,",X")){
											xbpe += 8;
											token1[len-2] = '\0';
										}
										else{
											if(!strcmp(token1+len-1,","))
												token1[len-1] = '\0';
											token = strtok_r(nextptr," \t\n,",&nextptr);
											if(!token){
												printf("**ERROR** Line %d : Invalid Operand!\n", linenum);
												return 0;
											}
											else if(token[0] != 'X'){
												printf("**ERROR** Line %d : Invalid Operand!\n", linenum);
												return 0;
											}
											xbpe += 8;
										}
									}
									if(token1[0]<'0' || token1[0]>'9'){
										if((op_addr = search_symbol(token1)) < 0){
											printf("**ERROR** Line %d : Invalid Operand!\n", linenum);
											return 0;
										}
										else{
											if(!plus_flag){
												disp = op_addr - loc -3;
												if((disp > 2047 || disp < -2048)){
													if((base_addr != -1) && (op_addr >= base_addr)){
														xbpe += 4;
														disp = op_addr - base_addr;
													}
													else{
														printf("**ERROR** Line %d : Unable to reach the operand\n", linenum);
														return 0;		
													}
												}
												else{
													if(disp < 0)
														disp = disp & 0xFFF;
													xbpe += 2;
												}
												op_addr = disp;
											}
										}
									}
									else{
										if(ni == 1)
											sscanf(token1,"%d",&op_addr);
										else{
											printf("**ERROR** Line %d : Invalid Operand!\n", linenum);
											return 0;
										}
									}
									if(plus_flag){
										if(ni != 1)
											make_modi(loc-start_addr+1,5);
										type = 8;
										objcode += (target->opcode + ni)*pow(16.0,6.0) + pow(16.0,5.0) + op_addr;
									}
									else{
										type = 6;
										objcode += (target->opcode + ni)*pow(16.0,4.0) + xbpe*pow(16.0,3.0) + op_addr;
									}
									fprintf(fp1,"%-100s%06X\n",line,objcode);
								}
							}
							else{
								printf("**ERROR** Line %d : Need More Operand!\n", linenum);
								return 0;
							}
						}
						//if the type of OPCODE is 1 or OPCODE is RSUB, which doesn't need any operands
						else{
							if(!strcmp(token,"RSUB")){
								objcode += (target->opcode + 3)*pow(16.0,4.0);
								type = 6;
								fprintf(fp1,"%-100s%06X\n",line,objcode);
							}
							else{
								objcode += target->opcode;
								type = 2;
								fprintf(fp1,"%-100s%02X\n",line,objcode);
							}
						}
					}
				}
			}
			else{
				target = search_hashtable(token);
				if(target){
					if((target->type != 1) && (strcmp(token,"RSUB"))){
						len = strlen(nextptr);
						for(i=0; i<len; i++){
							if(nextptr[i] == ',')
								comma_flag = 1;
						}
						token1 = strtok_r(nextptr," \t\n",&nextptr);
						if(token1){
							if(target->type == 2){
								if(!strcmp(token,"CLEAR") || !strcmp(token,"TIXR")){
									if((op_addr = is_regist(token1)) < 0){
										printf("**ERROR** Line %d : Operand needs to be a Register!\n", linenum);
										return 0;
									}
								}
								else if(!strcmp(token,"SVC")){
									len = strlen(token1);
									for(i=0; i<len; i++){
										if((token1[i]>='0' && token1[i]<='9') || (token1[i]>='A' && token1[i]<='F'))
											continue;
										else{
											printf("**ERROR** Line %d : Operand needs to be a Number!\n", linenum);
											return 0;
										}
									}
									sscanf(token1,"%X",&op_addr);
								}
								else if(!strcmp(token,"SHIFTL") || !strcmp(token,"SHIFTR")){
									if(!comma_flag){
										printf("**ERROR** Line %d : Invalid Operand!\n", linenum);
										return 0;
									}
									arg2 = strtok_r(nextptr," \t\n,",&nextptr);
									arg1 = strtok_r(token1," \t\n,",&nextptr);
									if(!arg1 || !arg2){
										printf("**ERROR** Line %d : Need More Operand!\n", linenum);
										return 0;
									}
									if((op_addr = is_regist(arg1)) < 0){
										printf("**ERROR** Line %d : Operand needs to be a Register!\n", linenum);
										return 0;
									}
									len = strlen(arg2);
									for(i=0; i<len; i++){
										if((arg2[i]>='0' && arg2[i]<='9') || (arg2[i]>='A' && arg2[i]<='F'))
											continue;
										else{
											printf("**ERROR** Line %d : Operand needs to be a Number!\n", linenum);
											return 0;
										}
									}
									sscanf(arg2,"%X",&op_addr2);
								}
								else{
									arg2 = strtok_r(nextptr," \t\n,",&nextptr);
									arg1 = strtok_r(token1," \t\n,",&nextptr);
									if(!arg1 || !arg2){
										printf("**ERROR** Line %d : Need More Operand!\n", linenum);
										return 0;
									}
									if((op_addr = is_regist(arg1)) < 0){
										printf("**ERROR** Line %d : Operand needs to be a Register!\n", linenum);
										return 0;
									}
									if((op_addr2 = is_regist(arg2)) < 0){
										printf("**ERROR** Line %d : Operand needs to be a Register!\n", linenum);
										return 0;
									}	
								}
								objcode += (target->opcode)*pow(16.0,2.0) + op_addr*16 + op_addr2;
								type = 4;
								fprintf(fp1,"%-100s%04X\n",line,objcode);
							}
							//if the type of OPCODE is 3/4
							else{
								if(token1[0] == '#'){
									ni = 1;
									token1++;
								}
								else if(token1[0] == '@'){
									ni = 2;
									token1++;
								}
								else	ni = 3;
									
								len = strlen(token1);
								if(comma_flag){
									if(!strcmp(token1+len-2,",X")){
										xbpe += 8;
										token1[len-2] = '\0';
									}
									else{
										if(!strcmp(token1+len-1,","))
											token1[len-1] = '\0';
										token = strtok_r(nextptr," \t\n,",&nextptr);
										if(!token){
											printf("**ERROR** Line %d : Invalid Operand!\n", linenum);
											return 0;
										}
										else if(token[0] != 'X'){
											printf("**ERROR** Line %d : Invalid Operand!\n", linenum);
											return 0;
										}
										xbpe += 8;
									}
								}
								if(token1[0]<'0' || token1[0]>'9'){
									if((op_addr = search_symbol(token1)) < 0){
										printf("**ERROR** Line %d : Invalid Operand!\n", linenum);
										return 0;
									}
									else{
										if(!plus_flag){
											disp = op_addr - loc -3;
											if((disp > 2047 || disp < -2048)){
												if((base_addr != -1) && (op_addr >= base_addr)){
													xbpe += 4;
													disp = op_addr - base_addr;
												}
												else{
													printf("**ERROR** Line %d : Unable to reach the operand\n", linenum);
													return 0;		
												}
											}
											else{
												if(disp < 0)
													disp = disp & 0xFFF;
												xbpe += 2;
											}
											op_addr = disp;
										}
									}
								}
								else{
									if(ni == 1)
										sscanf(token1,"%d",&op_addr);
									else{
										printf("**ERROR** Line %d : Invalid Operand!\n", linenum);
										return 0;
									}
								}
								if(plus_flag){
									if(ni != 1)
										make_modi(loc-start_addr+1,5);
									type = 8;
									objcode += (target->opcode + ni)*pow(16.0,6.0) + pow(16.0,5.0) + op_addr;
								}
								else{
									type = 6;
									objcode += (target->opcode + ni)*pow(16.0,4.0) + xbpe*pow(16.0,3.0) + op_addr;
								}
								fprintf(fp1,"%-100s%06X\n",line,objcode);
							}
						}
						else{
							printf("**ERROR** Line %d : Need More Operand!\n", linenum);
							return 0;
						}
					}
					//if the type of OPCODE is 1 or OPCODE is RSUB, which doesn't need any operands
					else{
						if(!strcmp(token,"RSUB")){
							objcode += (target->opcode + 3)*pow(16.0,4.0);
							type = 6;
							fprintf(fp1,"%-100s%06X\n",line,objcode);
						}
						else{
							objcode += target->opcode;
							type = 2;
							fprintf(fp1,"%-100s%02X\n",line,objcode);
						}
					}
				}
			}
		}
		if(((rec_len+type) > MAX_REC) || (!type && rec_len)){
			fprintf(fp2,"T%06X%02X%-60s\n",rec_start,rec_len/2,record);
			rec_len = 0;
			rec_start = loc;
			memset(record,0,MAX_REC);
		}
		if(type){
			sprintf(record+rec_len,"%0*X",type,objcode);
			rec_len += type;
		}
		
		ni = xbpe = disp = 0;
		objcode = op_addr = op_addr2 = 0;
		arg1 = arg2 = NULL;
		plus_flag = comma_flag = 0;
	}while(fgets(line,MAX_LINE,fp));
	
	fprintf(fp1,"%s",line);
	fprintf(fp2,"T%06X%02X%-60s\n",rec_start,rec_len/2,record);
	for(tmp=modi_head; tmp!=NULL; tmp=tmp->link)
					fprintf(fp2,"M%06X%02X\n",tmp->disp,tmp->pt);
	fprintf(fp2,"E%06X",start_addr);

	fclose(fp);
	fclose(fp1);
	fclose(fp2);
	
	sym_head = sym_temp;
	make_final(filename);
	
	return 1;
}
int is_regist(char *input)
{
	switch(input[0]){
	case 'A' : return 0;
	case 'X' : return 1;
	case 'L' : return 2;
	case 'B' : return 3;
	case 'S' : return 4;
	case 'T' : return 5;
	case 'F' : return 6;
	default : break;
	}
	if(!strcmp(input,"PC"))
		return 8;
	else if(!strcmp(input,"SW"))
		return 9;
	else
		return -1;
}
void make_modi(int disp, int point)
{
	modirec *node, *temp;

	node = (modirec*)calloc(1,sizeof(modirec));
	node->disp = disp;
	node->pt = point;
	node->link = NULL;
	if(!modi_head)
		modi_head = node;
	else{
		for(temp=modi_head; temp->link!=NULL; temp=temp->link);
		temp->link =node;
	}
	return;
}
void make_final(char* filename)
{
		FILE *fp1 = fopen("lst.txt","r");
		FILE *fp2 = fopen("obj.txt","r");
		FILE *fp3, *fp4;
		char *temp = (char *)calloc(MAX_NAME,sizeof(char));
		char *temp1 = (char *)calloc(MAX_LINE,sizeof(char));
		strcpy(temp,filename);
		fp3 = fopen(strcat(filename,".lst"),"w");
		fp4 = fopen(strcat(temp,".obj"),"w");

		while(fgets(temp1,MAX_LINE,fp1)){
			fputs(temp1,fp3);
			if(feof(fp1)) break;
		}
		while(fgets(temp1,MAX_LINE,fp2)){
			fputs(temp1,fp4);
			if(feof(fp2)) break;
		}
		printf("\toutput file : [%s], [%s]\n",filename,temp);
		fclose(fp1);
		fclose(fp2);
		fclose(fp3);
		fclose(fp4);
		return;
}
