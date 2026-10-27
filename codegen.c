#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int temp_count = 0;

char* new_temp() {
    char *temp = (char*)malloc(16);
    sprintf(temp, "t%d", temp_count++);
    return temp;
}

void init_code() {
    printf("; --- 8086 Assembly Code ---\n");
    printf(".MODEL SMALL\n");
    printf(".STACK 100H\n");
    printf(".DATA\n");
    printf(";\tVariables automatically declared dynamically in AX/BX or temp memory\n");
    printf(".CODE\n");
    printf("MAIN PROC\n");
    printf("\tMOV AX, @DATA\n");
    printf("\tMOV DS, AX\n\n");
}

void end_code() {
    printf("\n\tMOV AH, 4CH\n");
    printf("\tINT 21H\n");
    printf("MAIN ENDP\n");
    printf("END MAIN\n");
}

void gen_assign(char *id, char *val) {
    printf("\t; Assignment: %s = %s\n", id, val);
    printf("\tMOV AX, %s\n", val);
    printf("\tMOV %s, AX\n", id);
}

char* gen_arith(char *op, char *op1, char *op2) {
    char *res = new_temp();
    printf("\t; Arithmetic: %s = %s %s %s\n", res, op1, op, op2);
    printf("\tMOV AX, %s\n", op1);
    
    if (strcmp(op, "ADD") == 0) {
        printf("\tADD AX, %s\n", op2);
    } else if (strcmp(op, "SUB") == 0) {
        printf("\tSUB AX, %s\n", op2);
    } else if (strcmp(op, "MUL") == 0) {
        printf("\tMOV BX, %s\n", op2);
        printf("\tMUL BX\n");
    } else if (strcmp(op, "DIV") == 0) {
        printf("\tXOR DX, DX\n");
        printf("\tMOV BX, %s\n", op2);
        printf("\tDIV BX\n");
    }
    
    printf("\tMOV %s, AX\n", res);
    return res;
}

char* gen_relop(char *jump_instr, char *op1, char *op2) {
    /* Pass comparison jump condition dynamically */
    char *cond_info = (char*)malloc(32);
    sprintf(cond_info, "%s %s %s", jump_instr, op1, op2);
    return cond_info;
}

void gen_if_start(char *cond, int label_id) {
    char jump_instr[8], op1[16], op2[16];
    sscanf(cond, "%s %s %s", jump_instr, op1, op2);
    
    printf("\t; IF Condition\n");
    printf("\tMOV AX, %s\n", op1);
    printf("\tCMP AX, %s\n", op2);
    
    /* Invert jump logic: jump to ELSE/END if condition is FALSE */
    if (strcmp(jump_instr, "JE") == 0)      printf("\tJNE L_ELSE_%d\n", label_id);
    else if (strcmp(jump_instr, "JNE") == 0) printf("\tJE L_ELSE_%d\n", label_id);
    else if (strcmp(jump_instr, "JL") == 0)  printf("\tJGE L_ELSE_%d\n", label_id);
    else if (strcmp(jump_instr, "JLE") == 0) printf("\tJG L_ELSE_%d\n", label_id);
    else if (strcmp(jump_instr, "JG") == 0)  printf("\tJLE L_ELSE_%d\n", label_id);
    else if (strcmp(jump_instr, "JGE") == 0) printf("\tJL L_ELSE_%d\n", label_id);
}

void gen_if_else(int label_id) {
    printf("\tJMP L_END_%d\n", label_id);
    printf("L_ELSE_%d:\n", label_id);
}

void gen_if_end(int label_id) {
    printf("L_END_%d:\n", label_id);
}

void gen_while_start(int label_id) {
    printf("L_WHILE_START_%d:\n", label_id);
}

void gen_while_cond(char *cond, int label_id) {
    char jump_instr[8], op1[16], op2[16];
    sscanf(cond, "%s %s %s", jump_instr, op1, op2);
    
    printf("\t; WHILE Condition\n");
    printf("\tMOV AX, %s\n", op1);
    printf("\tCMP AX, %s\n", op2);
    
    /* Jump out of loop if FALSE */
    if (strcmp(jump_instr, "JE") == 0)      printf("\tJNE L_WHILE_END_%d\n", label_id);
    else if (strcmp(jump_instr, "JNE") == 0) printf("\tJE L_WHILE_END_%d\n", label_id);
    else if (strcmp(jump_instr, "JL") == 0)  printf("\tJGE L_WHILE_END_%d\n", label_id);
    else if (strcmp(jump_instr, "JLE") == 0) printf("\tJG L_WHILE_END_%d\n", label_id);
    else if (strcmp(jump_instr, "JG") == 0)  printf("\tJLE L_WHILE_END_%d\n", label_id);
    else if (strcmp(jump_instr, "JGE") == 0) printf("\tJL L_WHILE_END_%d\n", label_id);
}

void gen_while_end(int label_id) {
    printf("\tJMP L_WHILE_START_%d\n", label_id);
    printf("L_WHILE_END_%d:\n", label_id);
}
