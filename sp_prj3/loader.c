#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "loader.h"
#include "20141550.h"

static int prog = 0;
static int exec_addr = 0;
static int total_len = 0;
static int regist[10] = {0,};

int loader_main(char *nextcom)
{
	char *token;
	char *nextptr = (char *)calloc(strlen(nextcom)+1,sizeof(char));
	char *line = (char *)calloc(MAX_RECORD,sizeof(char));
	char *name = (char *)calloc(NAME_LIMIT,sizeof(char));
	int csaddr = prog, st_addr, sym_addr, cslth, i, j;
	int t_start, t_len, m_addr, m_pt, m_ref, idx=0;
	int *extref = (int *)calloc(NAME_LIMIT,sizeof(int));
	char m_flag;
	FILE *fp;
	strcpy(nextptr,nextcom);
	memset(extref,-1,NAME_LIMIT);

	// pass1
	for(i=0; i<3; i++){
		if(!(*nextcom))
			break;
		token = strtok_r(nextcom," \t",&nextcom);
		if(!token)
			break;
		// open the object file
		fp = fopen(token,"r");
		memset(line,0,MAX_RECORD);
		// get one line from fp
		while(fgets(line,MAX_RECORD,fp)){
			// if End Record
			if(line[0] == 'E')
				break;
			// if Head Record
			else if(line[0] == 'H'){
				sscanf(line+1,"%6s%06X%06X",name,&st_addr,&cslth);
				// check if name exists in the ESTAB
				if(search_estab(name) >= 0){
					printf("ERROR : Duplicate Control Section Name\n");
					return 0;
				}
				else{
					make_estab(name,0,csaddr,cslth);
				}
			}
			// if Define Record
			else if(line[0] == 'D'){
				idx++;
				// get the external symbol defined
				while(*(line+idx)!='\n' && *(line+idx)!='\0'){
					sscanf(line+idx,"%6s",name);
					if(search_estab(name) >= 0){
						printf("ERROR : Duplicate External Symbol\n");
						return 0;
					}
					else{
						idx += 6;
						sscanf(line+idx,"%06X",&sym_addr);
						make_estab(name,1,csaddr+sym_addr-st_addr,-1);
					}
					idx += 6;
				}
			}
			idx = 0;
		}
		csaddr += cslth;
	}

	// pass2
	csaddr = prog;
	exec_addr = prog;
	
	for(i=0; i<3; i++){
		if(!(*nextptr))
			break;
		token = strtok_r(nextptr," \t",&nextptr);
		if(!token)
			break;
		// open the object file
		fp = fopen(token,"r");
		memset(line,0,MAX_RECORD);
		// get one line from fp
		while(fgets(line,MAX_RECORD,fp)){
			// if End Record
			if(line[0] == 'E'){
				if(*(line+1)!='\n' && *(line+1)!='\0'){
					sscanf(line+1,"%06X",&exec_addr);
					exec_addr += prog;
				}
				break;
			}
			// if Head Record
			else if(line[0] == 'H'){
				sscanf(line+1,"%6s%06X%06X",name,&st_addr,&cslth);
				extref[1] = csaddr;
			}
			// if Refer Record
			else if(line[0] == 'R'){
				idx++;
				// get the external symbol referenced
				while(*(line+idx)!='\n' && *(line+idx)!='\0'){
					sscanf(line+idx,"%02X%6s",&m_ref,name);
					sym_addr = search_estab(name);
					if(sym_addr < 0){
						printf("ERROR : External reference not defined\n");
						return 0;
					}
					else extref[m_ref] = sym_addr;
					idx += 8;
				}
			}
			// if Text Record
			else if(line[0] == 'T'){
				sscanf(line+1,"%06X%02X",&t_start,&t_len);
				idx += 9;
				for(j=0; j<t_len; j++)
					sscanf(line+idx+2*j,"%02X",&vir_mem[csaddr+t_start+j]);
			}
			// if Modification Record
			else if(line[0] == 'M'){
				sscanf(line+1,"%06X%02X%c%02X",&m_addr,&m_pt,&m_flag,&m_ref);
				if(extref[m_ref] < 0){
					printf("ERROR : External reference not defined\n");
					return 0;
				}
				else
					modif_memory(csaddr+m_addr-st_addr,m_pt,m_flag,extref[m_ref]);
			}
			idx = 0;
		}
		csaddr += cslth;
		memset(extref,-1,NAME_LIMIT);
	}

	// print the ESTAB
	print_estab();
	return 1;
}
void set_progaddr(char *nextcom)
{
	char *token = strtok_r(nextcom," \t",&nextcom);
	sscanf(token,"%04X",&prog);
	return;
}
void modif_memory(int addr, int num, char sign, int ref)
{
	int i;
	unsigned int modi=0;

	// calculate the value in the addr of virtual memory
	if(num%2) modi += (vir_mem[addr]%16)*pow(16.0,num-1.0);
	for(i=0; i<num/2; i++)
		modi += vir_mem[addr+i+num%2]*pow(16.0,2*num-2*i-8.0);

	// modify with sign and ref	
	if(sign == '+') modi += ref;
	else modi -= ref;
	modi = modi%(int)pow(16.0,(double)num);

	// store the value in the addr of virtual memory	
	if(num%2)
		vir_mem[addr] += modi/(int)pow(16.0,num-1.0);
	for(i=0; i<num/2; i++){
		modi = modi%(int)pow(16.0,num-2*i-num%2);
		vir_mem[addr+i+num%2] = modi/(int)pow(16.0,2*num-2*i-8.0);
	}
	return;
}
// store the breakpoints in the ascending order
void store_bp(int bp_addr)
{
	bp_list *node, *temp;

	node = (bp_list *)calloc(1,sizeof(bp_list));
	node->addr = bp_addr;
	node->link = NULL;
	if(!bp_head){
		bp_head = node;
	}
	else{
		if(bp_head->addr > node->addr){
			node->link = bp_head;
			bp_head = node;
		}
		else if(!(bp_head->link))
			bp_head->link = node;
		else{
			for(temp=bp_head; temp->link!=NULL; temp=temp->link){
				if(temp->addr < node->addr){
					if(temp->link->addr > node->addr)
						break;
				}
			}
			node->link = temp->link;
			temp->link = node;
		}
	}
	printf("\t\t[ok] create breakpoint %04X\n",bp_addr);
	return;
}
// 0:clear / 1:print
void handle_bp(int hd_flag)
{
	bp_list *temp = bp_head, *next;

	// clear the linked list of breakpoints
	if(!hd_flag){
		while(temp != NULL){
			next = temp->link;
			free(temp);
			temp = next;
		}
		bp_head = NULL;
		printf("\t\t[ok] create all breakpoints\n");
	}
	// print the linked list of breakpoints
	else{
		printf("\t  breakpoint\n\t  ----------\n");
		for(temp=bp_head; temp!=NULL; temp=temp->link)
			printf("\t  %04X\n",temp->addr);
	}
	return;
}
int search_bp(int target)
{
	bp_list *temp;
	for(temp=bp_head; temp!=NULL; temp=temp->link){
		if(temp->addr == target)
			return 1;
	}
	return 0;
}
void make_estab(char *name, int flag, int addr, int len)
{
	es_table *node, *temp;

	node = (es_table *)calloc(1,sizeof(es_table));
	strcpy(node->name,name);
	node->name_flag = flag;
	node->address = addr;
	node->length = len;
	node->link = NULL;
	
	if(!es_head)
		es_head = node;
	else{
		for(temp=es_head; temp->link!=NULL; temp=temp->link);
		temp->link = node;
	}
	return;
}
int search_estab(char *name)
{
	es_table *temp;
	for(temp=es_head; temp!=NULL; temp=temp->link){
		if(!strcmp(name,temp->name))
			return temp->address; 
	}
	return -1;
}
void print_estab(void)
{
	es_table *temp;

	printf("\t\tcontrol\tsymbol\taddress\t\tlength\n\t\tsection\tname\n");
	printf("\t\t---------------------------------------\n");
	for(temp=es_head; temp!=NULL; temp=temp->link){
		if(!temp->name_flag){
			printf("\t\t%-6s\t      \t%04X\t\t%04X\n",temp->name,temp->address,temp->length);
			total_len += temp->length;
		}
		else
			printf("\t\t      \t%-6s\t%04X\n",temp->name,temp->address);
	}
	printf("\t\t---------------------------------------\n");
	printf("\t\t      \t      \ttotal length\t%04X\n",total_len);
	return;
}
int run_prog(void)
{
	int opcode, ni, format, end_exec = prog + total_len;
	int reg1, reg2, extnd, i, pc_flag=0, ch_flag=0, brk_point;
	unsigned int target, value=0;
	static int bp_flag=0, cond, cond_flag=0;

	// if first run 
	if(!bp_flag){
		regist[8]	= exec_addr;
		regist[2] = end_exec;
		bp_flag = 1;
	}

	while(regist[8] < end_exec){
			ni = vir_mem[regist[8]] % 4;
			opcode = vir_mem[regist[8]] - ni;
		
			// check if breakpoints	
			if(search_bp(regist[8])){
				ch_flag = 1;	
				brk_point = regist[8];
			}

			// if format 2
			if(opcode==0x90 || opcode==0xB4 || opcode==0xA0 || opcode==0x9C || opcode==0x98 || opcode==0xAC || opcode==0xA4 || opcode==0xA8 || opcode==0x94 || opcode==0xB8){
				format = 2;
				reg1 = vir_mem[regist[8]+1]/16;
				reg2 = vir_mem[regist[8]+1]%16;
				if(reg1<0 || reg1>9 || reg1==7){
					printf("ERROR : Invalid Operand - Register Not Defined\n");
					return 0;
				}
				else{
					if((opcode!=0xA4 && opcode!=0xA8) && (reg2<0 || reg2>9 || reg2==7)){
						printf("ERROR : Invalid Operand - Register Not Defined\n");
						return 0;
					}
				}
			}
			// if format 3 or 4
			else{
				// check the format
				extnd = vir_mem[regist[8]+1] & 0x10;
				if(extnd) format = 4;
				else format = 3;
				// get the target address and calculate the value
				target = target_addr(format,regist[8]);
				if(ni == 1) value = target;
				else{
					// error handling with target address
					if(target < 0 || target > end_exec){
						printf("ERROR : Invalid Target Address\n");
						return 0;
					}
					for(i=0; i<3; i++)
						value += vir_mem[target+i]*pow(16.0,4.0-2*i);
					if(ni == 2)
							target = value;
				}
			}

			// opcode handling
			switch(opcode){
				// ADDR
				case 0x90 : regist[reg2] = regist[reg2]+regist[reg1]; break;
				// CLEAR
				case 0xB4 : regist[reg1] = 0; break;
				// COMPR
				case 0xA0 : cond_flag = 1;
										if(regist[reg1] > regist[reg2]) cond = 1;
										else if(regist[reg1] == regist[reg2]) cond = 0;
										else cond = -1; break;
				// DIVR
				case 0x9C : regist[reg2] = regist[reg2]/regist[reg1]; break;
				// MULR
				case 0x98 : regist[reg2] = regist[reg2]*regist[reg1]; break;
				// RMO
				case 0xAC : regist[reg2] = regist[reg1]; break;
				// SHIFTL
				case 0xA4 : regist[reg1] = regist[reg1] << reg2; break;
				// SHIFTR
				case 0xA8 : regist[reg1] = regist[reg1] >> reg2; break;
				// SUBR
				case 0x94 : regist[reg2] = regist[reg2]-regist[reg1]; break;
				// TIXR
				case 0xB8 : regist[1] = regist[1]+1;
										cond_flag = 1;
										if(regist[1] > regist[reg1]) cond = 1;
										else if(regist[1] == regist[reg1]) cond = 0;
										else cond = -1; break;

				// format 3/4
				// ADD
				case 0x18 : regist[0] += value; break;	
				// AND
				case 0x40 : regist[0] &= value; break;
				// COMP
				case 0x28 : cond_flag = 1;
										if(regist[0] > value) cond = 1;
										else if(regist[0] == value) cond = 0;
										else cond = -1;
										break;
				// DIV
				case 0x24 : if(!value){
											printf("ERROR : Invalid Operand - DIVISION BY 0\n"); return 0; }
										regist[0] /= value; break;
				// J
				case 0x3C : regist[8] = target; pc_flag = 1; break;
				// JEQ
				case 0x30 : if(!cond_flag){
											printf("ERROR : No Condition Exists\n"); return 0; }
									 	if(!cond){ regist[8] = target; pc_flag = 1; } break;
				// JGT
				case 0x34 : if(!cond_flag){
											printf("ERROR : No Condition Exists\n"); return 0; }
										if(cond > 0){ regist[8] = target;  pc_flag = 1; } break;
				// JLT
				case 0x38 : if(!cond_flag){
											printf("ERROR : No Condition Exists\n"); return 0; }
										if(cond < 0){ regist[8] = target; pc_flag = 1; }break;
				// JSUB
				case 0x48 : regist[2] = regist[8]+format; regist[8] = target; pc_flag = 1; break;
				// LDA
				case 0x00 : regist[0] = value; break;
				// LDB
				case 0x68 : regist[3] = value; break;
				// LDCH
				case 0x50 : regist[0] = value/(int)pow(16.0,4.0); break;
				// LDL
				case 0x08 : regist[2] = value; break;
				// LDS
				case 0x6C : regist[5] = value; break;
				// LDT
				case 0x74 : regist[5] = value; break;
				// LDX
				case 0x04 : regist[1] = value; break;
				// MUL
				case 0x20 : regist[0] *= value; break;
				// OR
				case 0x44 : regist[0] |= value; break;
				// RD
				case 0xD8 : regist[0] = 0; break;
				// RSUB
				case 0x4C : regist[8] = regist[2]; pc_flag = 1; break;
				// STA
				case 0x0C : store_mem(target, regist[0]); break;
				// STB
				case 0x78 : store_mem(target, regist[3]); break;
				// STCH
				case 0x54 : store_mem(target, regist[0]%256);break;
				// STL
				case 0x14 : store_mem(target, regist[2]); break;
				// STS
				case 0x7C : store_mem(target, regist[4]); break;
				// STT
				case 0x84 : store_mem(target, regist[5]); break;
				// STX
				case 0x10 : store_mem(target, regist[1]); break;
				// SUB
				case 0x1C : regist[0] -= value; break;
				// TD
				case 0xE0 : cond = -1; cond_flag = 1; break;
				// TIX
				case 0x2C : regist[2] += 1; cond_flag = 1;
										if(regist[2] > value) cond = 1;
										else if(regist[2] == value) cond = 0;
										else cond = -1; break;
				// WD
				case 0xDC : regist[1] = regist[5]; break;
				
				default : printf("ERROR : Invalid Opcode in MEMORY\n"); return 0;
			}
			// if PC register not changed	
			if(!pc_flag)
				regist[8] += format;
			pc_flag = 0;
			value = 0;
			// if breakpoint
			if(ch_flag) break;
	}
	// if breakpoint
	if(ch_flag){
		print_reg();
		printf("\tStop at checkpoint[%04X]\n\n",brk_point);
		ch_flag = 0;
	}
	// if the program ends
	else{
		print_reg();
		printf("\tEnd Program\n\n");
	}

	return 1;
}
int target_addr(int format, int idx)
{
	unsigned int target=0, xbpe, sign_flag=0;

	xbpe = vir_mem[idx+1];
	if((xbpe%16) & 0x08) sign_flag=1;

	// modify the target address with xbp address
	if(xbpe & 0x80) target += regist[1];
	if(xbpe & 0x40) target += regist[3];
	if(xbpe & 0x20) target += (regist[8]+format);

	// calculate the target address
	if(format == 4)
		target += (vir_mem[idx+1]%16)*pow(16.0,4.0) + (vir_mem[idx+2]/16)*pow(16.0,3.0);
	target += (vir_mem[idx+format-2]%16)*pow(16.0,2.0) + vir_mem[idx+format-1];

	if(sign_flag){
		target += 0xF000;
		if(format == 3)
			target &= 0x0FFFF;
	}
	return target;
}
void store_mem(unsigned int target,unsigned int value)
{
	int i;
	for(i=0; i<3; i++)
		vir_mem[target+i] = (value%(int)pow(16.0,6.0-2*i))/(int)pow(16.0,4.0-2*i);
	return;

}
void print_reg(void)
{
	printf("\t\tA : %012X X : %08X\n\t\tL : %012X PC: %012X\n\t\tB : %012X S : %012X\n\t\tT : %012X\n",regist[0],regist[1],regist[2],regist[8],regist[3],regist[4],regist[5]); return;
}
void load_init(void)
{
	es_table *tmp1=es_head, *next1;
	bp_list *tmp2=bp_head, *next2;
	int i;

	// initialize the ESTAB	
	while(tmp1 != NULL){
		next1 = tmp1->link;
		free(tmp1);
		tmp1 = next1;
	}
	es_head = NULL;
	exec_addr = 0;
	total_len = 0;
	
	// initialize the breakpoints
	while(tmp2 != NULL){
		next2 = tmp2->link;
		free(tmp2);
		tmp2 = next2;
	}
	bp_head = NULL;
	// initialize the registers
	for(i=0; i<10; i++)
		regist[i] = 0;
	return;
}
