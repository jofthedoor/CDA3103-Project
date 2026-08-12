#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    if (ALUControl == 0)
        *ALUresult = A + B;//add
    else if (ALUControl == 1)
        *ALUresult = A - B;//sub
    else if (ALUControl == 2)
    {
        if (A < B)
            *ALUresult = 1;//slt
        else
            *ALUresult = 0;
    }
    else if (ALUControl == 3)//sltu
    {
        if (A < B)
            *ALUresult = 1;
        else
            *ALUresult = 0;
    }     
    else if (ALUControl == 4)//and
        *ALUresult = A & B;
    else if (ALUControl == 5)//or
        *ALUresult = A | B;
    else if (ALUControl == 6)//sl
        *ALUresult = B << 16;
    else if (ALUControl == 7)//not a
        *ALUresult = ~A;

    if (*ALUresult == 0) {
        *Zero = 1;
    } else {
        *Zero = 0;
    }
        
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    if (PC % 4 != 0)//if halt
        return 1;
    else{
        *instruction = Mem[PC >> 2];//fech instiction
        return 0;
    }

}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    *op = (instruction & 0b11111100000000000000000000000000) >> 26;
    *r1 = (instruction & 0b00000011111000000000000000000000) >> 21;
    *r2 = (instruction & 0b00000000000111110000000000000000) >> 16;
    *r3 = (instruction & 0b00000000000000001111100000000000) >> 11;
    *funct = instruction & 0b00000000000000000000000000111111;
    *offset = instruction & 0b00000000000000001111111111111111;
    *jsec = instruction & 0b00000011111111111111111111111111;
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    if (op == 0)// type
    {
        controls->RegDst = 1;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 7;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 1;
    }
    else if (op == 2)//jump
    {
        controls->RegDst = 2;
        controls->Jump = 1;
        controls->Branch = 2;
        controls->MemRead = 0;
        controls->MemtoReg = 2;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 2;
        controls->RegWrite = 0;
    }
    else if (op == 4)//bie
    {
        controls->RegDst = 2;
        controls->Jump = 0;
        controls->Branch = 1;
        controls->MemRead = 0;
        controls->MemtoReg = 2;
        controls->ALUOp = 1;
        controls->MemWrite = 0;
        controls->ALUSrc = 2;
        controls->RegWrite = 0;
    }
    else if (op == 8)//addi
    {
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    else if (op == 10)//slti
    {
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 2;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    else if (op == 11)//sltiu
    {
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 3;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    else if (op == 15)//lui
    {
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 6;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    else if (op == 35)//lw
    {
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    else if (op == 43)//sw
    {
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 0;
        controls->MemWrite = 1;
        controls->ALUSrc = 1;
        controls->RegWrite = 0;
    }
    else
        return 1;

    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value)
{
    if ((offset >> 15) == 1)
        *extended_value = offset | 0b11111111111111110000000000000000;
    else
        *extended_value = offset & 0b00000000000000001111111111111111;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    if (ALUOp == 7)
    {
        if (funct == 32) //add   
            ALUOp = 0;
        else if (funct == 34)  //sub
            ALUOp = 1;
        else if (funct == 42)  //slt
            ALUOp = 2;
        else if (funct == 43)  //sltu
            ALUOp = 3;
        else if (funct == 36)  //and
            ALUOp = 4;
        else if (funct == 37)  //or
            ALUOp = 5;
        else
            return 1; 
    }
    int val;
    if (ALUSrc == 1)
        val = extended_value;
    else
        val = data2;

    ALU(data1, val, ALUOp, ALUresult, Zero);
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    if (MemWrite == 1)
    {
        if (ALUresult % 4 == 0)
            Mem[ALUresult >> 2] = data2;
        else
            return 1;
    }  
    if (MemRead == 1)
    {
        if (ALUresult % 4 == 0)
            *memdata = Mem[ALUresult >> 2];
        else
            return 1;
    }

    return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if (RegWrite == 1)
    {
        if (MemtoReg == 0)
        {
            if (RegDst == 0)
                Reg[r2] = ALUresult;
            else
                Reg[r3] = ALUresult;
        }

        if (MemtoReg == 1){
            if (RegDst == 0)
                Reg[r2] = memdata;
        
            else if (RegDst == 1)
                Reg[r3] =memdata;
    }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    *PC += 4;

    if (Zero == 1 && Branch == 1)
        *PC += (extended_value << 2);

    if (Jump == 1)
        *PC = (*PC & 0b11110000000000000000000000000000) | (jsec << 2);
}
