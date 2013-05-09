/**
 * @file      mips1_isa.cpp
 * @author    Sandro Rigo
 *            Marcus Bartholomeu
 *            Alexandro Baldassin (acasm information)
 *
 *            The ArchC Team
 *            http://www.archc.org/
 *
 *            Computer Systems Laboratory (LSC)
 *            IC-UNICAMP
 *            http://www.lsc.ic.unicamp.br/
 *
 * @version   1.0
 * @date      Mon, 19 Jun 2006 15:50:52 -0300
 * 
 * @brief     The ArchC i8051 functional model.
 * 
 * @attention Copyright (C) 2002-2006 --- The ArchC Team
 * 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include  "mips1_isa.H"
#include  "mips1_isa_init.cpp"
#include  "mips1_bhv_macros.H"


//If you want debug information for this model, uncomment next line
//#define DEBUG_MODEL
#include "ac_debug_model.H"

// Container de C++
#include <vector>
#include <pipeline.h>
#define FASES 5

//!User defined macros to reference registers.
#define Ra 31
#define Sp 29

struct Onebit{
	long int endereco;
	bool state;
};

// 'using namespace' statement to allow access to all
// mips1-specific datatypes
using namespace mips1_parms;
using namespace std;

enum estagios
{
	IF_ID,
	ID_EX,
	EX_MEM,
	MEM_WB
};


int forward1A = 0;
int forward1B = 0;
int forward2A = 0;
int forward2B = 0;
int noFstalls = 0;	// Numero Stalls na ausencia de forwards

vector < fase > pipeline (FASES-1);
// Exemplo: pipeline de 5 estagios
//  IF | ID | EX | MEM | WB
//    [0]  [1]  [2]   [3] 

void verificaStalls () {
	if(pipeline[MEM_WB].init) {
		if (pipeline[EX_MEM].regWrite &&
				pipeline[EX_MEM].rd &&
				pipeline[EX_MEM].rd == pipeline[ID_EX].rs) {
			noFstalls += 1;		// 2 stalls se nao tivesse forward
			forward2A++;
		}
		if (pipeline[EX_MEM].regWrite &&
				pipeline[EX_MEM].rd &&
				pipeline[EX_MEM].rd == pipeline[ID_EX].rt) {
			noFstalls += 2;		// 2 stalls se nao tivesse forward
			forward2B++;
		}
		if (pipeline[MEM_WB].regWrite &&
				pipeline[MEM_WB].rd &&
				pipeline[MEM_WB].rd == pipeline[ID_EX].rs
				&&
				!(pipeline[EX_MEM].regWrite && pipeline[EX_MEM].rd) 
		   ) {
			if (pipeline[EX_MEM].rd != pipeline[ID_EX].rs)
				noFstalls += 1;		// 1 stall se nao tivesse forward
				forward1A++;
		}
		if (pipeline[MEM_WB].regWrite &&
				pipeline[MEM_WB].rd &&
				pipeline[MEM_WB].rd == pipeline[ID_EX].rt
				&&
				!(pipeline[EX_MEM].regWrite && pipeline[EX_MEM].rd) 
		   ) {
			if (pipeline[EX_MEM].rd != pipeline[ID_EX].rt)
				noFstalls += 1;		// 1 stall se nao tivesse forward
				forward1B++;
		}
	}
}

//!Generic instruction behavior method.
void ac_behavior( instruction )
{
	dbg_printf("----- PC=%#x ----- %lld\n", (int) ac_pc, ac_instr_counter);

	// Se a fase existir realmente
	// Ou seja nao for a inicial
	// Se o pipeline foi devidamente preenchido
	// Ou seja o ultimo estagio esta' iniciado

	//  dbg_printf("----- PC=%#x NPC=%#x ----- %lld\n", (int) ac_pc, (int)npc, ac_instr_counter);
	//
	//
	//  Fazendo o pipeline rodar
	pipeline.pop_back(); 		// Fim da fase WB
	fase f(op);				// Nova fase e Armazenando instrucao
	f.init = true;			// Fase existente
	//cout << op << endl;
	//	pipeline.push_front(f);
	pipeline.insert(pipeline.begin(),f);	// Nova fase no pipeline

#ifndef NO_NEED_PC_UPDATE
	ac_pc = npc;
	npc = ac_pc + 4;
#endif 
};

int mytyper = 0;
int mytypei = 0;
int mytypej = 0;

unsigned int exmem_rd = 1000, exmem_rs = 1000, exmem_rt = 1000;
unsigned int memwb_rd = 1000, memwb_rs = 1000, memwb_rt = 1000;

bool aux_mem = false;
bool reg_write = false;

int ex_hazards = 0;
int mem_hazards = 0;

bool load = false;
int stall = 0;

int branch_count = 0;
int not_taken = 0;
int always_taken = 0;

Onebit one_bit_preditor[10000000];
int one_bit_count = 0;
bool branch_state_aux;

bool naive = false;
int naive_count = 0;

void OneBitFunc(int imediato, bool branch_state){
	for(int i = 0; i < 10000000; i++){
		if (i == 9999999) printf("FODEUUUU");
		if (one_bit_preditor[i].endereco == imediato){
			if (branch_state != one_bit_preditor[i].state){
				one_bit_preditor[i].state = branch_state;
				one_bit_count++;
			}
			break;
		}
		if (one_bit_preditor[i].endereco == 0){
			one_bit_preditor[i].endereco = imediato;
			if (branch_state != one_bit_preditor[i].state){
				one_bit_preditor[i].state = branch_state;
				one_bit_count++;
			}
			break;
		}
	}
}

void NaiveFunc(bool branch_state){
	if (branch_state != naive) {
		naive = branch_state;
		naive_count++;
	}
}	

//! Instruction Format behavior methods.
void ac_behavior( Type_R ){ 
	// Registradores descobertos na fase IF->ID
	pipeline[IF_ID].rd = rd;
	pipeline[IF_ID].rs = rs;
	pipeline[IF_ID].rt = rt;
	mytyper++;


	if (reg_write && (exmem_rd != 0) && ((exmem_rd == rs) || (exmem_rd == rt))) { // EX Hazard
		aux_mem = true;
		ex_hazards += 2;

		if (load) stall++;
		load = false;
	}

	if (reg_write && (memwb_rd !=0) && !(aux_mem) && ((memwb_rd == rs) || (memwb_rd == rt))) { // MEM Hazard
		mem_hazards++;
		// printf("2 - %d\n", mem_hazards);
	}

	aux_mem = false;
	reg_write = false;

	memwb_rd = exmem_rd;
	memwb_rs = exmem_rs;
	memwb_rt = exmem_rt; 

	// printf("BEFORE r%d, r%d, r%d\n", exmem_rd, exmem_rs, exmem_rt);

	exmem_rd = rd;
	exmem_rs = rs;
	exmem_rt = rt;


	// printf("TYPE_R r%d, r%d, r%d\n\n", rd, rs, rt);

}

void ac_behavior( Type_I ){ 
	// Registradores descobertos na fase IF->ID
	pipeline[IF_ID].rs = rs;
	pipeline[IF_ID].rt = rt;
	pipeline[IF_ID].imm = imm;
	mytypei++;

	if (reg_write && (exmem_rd != 0) && ((exmem_rd == rs) || (exmem_rd == rt))) { // EX Hazard
		aux_mem = true;
		ex_hazards += 2;
		// printf("1 - %d\n", ex_hazards);

	}

	if (reg_write && (memwb_rd !=0) && !(aux_mem) && ((memwb_rd == rs) || (memwb_rd == rt))) { // MEM Hazard
		mem_hazards++;
		// printf("2 - %d\n", mem_hazards);
	}

	aux_mem = false;
	reg_write = false;

	memwb_rd = exmem_rd;
	memwb_rs = exmem_rs;
	memwb_rt = exmem_rt; 

	// printf("BEFORE r%d, r%d, r%d\n", exmem_rd, exmem_rs, exmem_rt);

	exmem_rd = rt;
	exmem_rs = rs;
	// exmem_rt = rt;

	// printf("TYPE_I r%d, %d(r%d)\n\n", rt, imm & 0xFFFF, rs);		
}

void ac_behavior( Type_J ){ 
	// Registradores descobertos na fase IF->ID
	pipeline[IF_ID].addr = addr;
	mytypej++;

	memwb_rd = exmem_rd;
	memwb_rs = exmem_rs;
	memwb_rt = exmem_rt; 

	// 	printf("TYTPE_J %d\n", addr);
}

int nadds = 0;
//!Behavior called before starting simulation
void ac_behavior(begin)
{
	dbg_printf("@@@ begin behavior @@@\n");
	RB[0] = 0;
	npc = ac_pc + 4;

	// Is is not required by the architecture, but makes debug really easier
	for (int regNum = 0; regNum < 32; regNum ++)
		RB[regNum] = 0;
	hi = 0;
	lo = 0;

	for (int i = 0; i < 10000000; i++){
		one_bit_preditor[i].endereco = 0;
		one_bit_preditor[i].state = false;
	} 
}

//!Behavior called after finishing simulation
void ac_behavior(end)
{
	dbg_printf("@@@ end behavior @@@\n");
	printf("\n---------- MINHAS SAIDAS ---------- \n\n");
	printf("Numero de ADDS: %d\n", nadds);
	printf("TYPE_R: %d, TYPE_I: %d, TYPE_J: %d\n", mytyper, mytypei, mytypej);
	printf("EX: %d, MEM: %d\n", ex_hazards, mem_hazards);
	printf("Stall: %d\n", stall);
	printf("Branch: %d | Not Taken: %d | Always Taken: %d\n", branch_count, not_taken, always_taken);
	printf("1-bit Preditor: %d\n", one_bit_count);
	printf("Naive: %d\n", naive_count);
	printf("\n---------- MINHAS SAIDAS ---------- \n\n");
	cout << forward1A << " " << forward1B << " " << forward2A << " " << forward2B << endl;
	cout << "Numero de stalls se nao houvesse forward: " << noFstalls << endl;
}


//!Instruction lb behavior method.
void ac_behavior( lb )
{
	char byte;
	dbg_printf("lb r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
	byte = DM.read_byte(RB[rs]+ imm);
	RB[rt] = (ac_Sword)byte ;
	dbg_printf("Result = %#x\n", RB[rt]);
	reg_write = true;
	load = true;
};

//!Instruction lbu behavior method.
void ac_behavior( lbu )
{
	unsigned char byte;
	dbg_printf("lbu r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
	byte = DM.read_byte(RB[rs]+ imm);
	RB[rt] = byte ;
	dbg_printf("Result = %#x\n", RB[rt]);
	reg_write = true;
	load = true;
};

//!Instruction lh behavior method.
void ac_behavior( lh )
{
	short int half;
	dbg_printf("lh r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
	half = DM.read_half(RB[rs]+ imm);
	RB[rt] = (ac_Sword)half ;
	dbg_printf("Result = %#x\n", RB[rt]);
	reg_write = true;
	load = true;
};

//!Instruction lhu behavior method.
void ac_behavior( lhu )
{
	unsigned short int  half;
	half = DM.read_half(RB[rs]+ imm);
	RB[rt] = half ;
	dbg_printf("Result = %#x\n", RB[rt]);
	reg_write = true;
	load = true;
};

//!Instruction lw behavior method.
void ac_behavior( lw )
{
	dbg_printf("lw r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
	RB[rt] = DM.read(RB[rs]+ imm);
	dbg_printf("Result = %#x\n", RB[rt]);
	reg_write = true;
	load = true;
};

//!Instruction lwl behavior method.
void ac_behavior( lwl )
{
	dbg_printf("lwl r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
	unsigned int addr, offset;
	ac_Uword data;

	addr = RB[rs] + imm;
	offset = (addr & 0x3) * 8;
	data = DM.read(addr & 0xFFFFFFFC);
	data <<= offset;
	data |= RB[rt] & ((1<<offset)-1);
	RB[rt] = data;
	dbg_printf("Result = %#x\n", RB[rt]);
	reg_write = true;
	load = true;
};

//!Instruction lwr behavior method.
void ac_behavior( lwr )
{
	dbg_printf("lwr r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
	unsigned int addr, offset;
	ac_Uword data;

	addr = RB[rs] + imm;
	offset = (3 - (addr & 0x3)) * 8;
	data = DM.read(addr & 0xFFFFFFFC);
	data >>= offset;
	data |= RB[rt] & (0xFFFFFFFF << (32-offset));
	RB[rt] = data;
	dbg_printf("Result = %#x\n", RB[rt]);
	reg_write = true;
	load = true;
};

//!Instruction sb behavior method.
void ac_behavior( sb )
{
	unsigned char byte;
	dbg_printf("sb r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
	byte = RB[rt] & 0xFF;
	DM.write_byte(RB[rs] + imm, byte);
	dbg_printf("Result = %#x\n", (int) byte);
};

//!Instruction sh behavior method.
void ac_behavior( sh )
{
	unsigned short int half;
	dbg_printf("sh r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
	half = RB[rt] & 0xFFFF;
	DM.write_half(RB[rs] + imm, half);
	dbg_printf("Result = %#x\n", (int) half);
};

//!Instruction sw behavior method.
void ac_behavior( sw )
{
	dbg_printf("sw r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
	DM.write(RB[rs] + imm, RB[rt]);
	dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction swl behavior method.
void ac_behavior( swl )
{
	dbg_printf("swl r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
	unsigned int addr, offset;
	ac_Uword data;

	addr = RB[rs] + imm;
	offset = (addr & 0x3) * 8;
	data = RB[rt];
	data >>= offset;
	data |= DM.read(addr & 0xFFFFFFFC) & (0xFFFFFFFF << (32-offset));
	DM.write(addr & 0xFFFFFFFC, data);
	dbg_printf("Result = %#x\n", data);
};

//!Instruction swr behavior method.
void ac_behavior( swr )
{
	dbg_printf("swr r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
	unsigned int addr, offset;
	ac_Uword data;

	addr = RB[rs] + imm;
	offset = (3 - (addr & 0x3)) * 8;
	data = RB[rt];
	data <<= offset;
	data |= DM.read(addr & 0xFFFFFFFC) & ((1<<offset)-1);
	DM.write(addr & 0xFFFFFFFC, data);
	dbg_printf("Result = %#x\n", data);
};

//!Instruction addi behavior method.
void ac_behavior( addi )
{
	dbg_printf("addi r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
	RB[rt] = RB[rs] + imm;
	dbg_printf("Result = %#x\n", RB[rt]);
	//Test overflow
	if ( ((RB[rs] & 0x80000000) == (imm & 0x80000000)) &&
			((imm & 0x80000000) != (RB[rt] & 0x80000000)) ) {
		fprintf(stderr, "EXCEPTION(addi): integer overflow.\n"); exit(EXIT_FAILURE);
	}
	reg_write = true;
};

//!Instruction addiu behavior method.
void ac_behavior( addiu )
{
	dbg_printf("addiu r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
	RB[rt] = RB[rs] + imm;
	dbg_printf("Result = %#x\n", RB[rt]);
	reg_write = true;
};

//!Instruction slti behavior method.
void ac_behavior( slti )
{
	dbg_printf("slti r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
	// Set the RD if RS< IMM
	if( (ac_Sword) RB[rs] < (ac_Sword) imm )
		RB[rt] = 1;
	// Else reset RD
	else
		RB[rt] = 0;
	dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sltiu behavior method.
void ac_behavior( sltiu )
{
	dbg_printf("sltiu r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
	// Set the RD if RS< IMM
	if( (ac_Uword) RB[rs] < (ac_Uword) imm )
		RB[rt] = 1;
	// Else reset RD
	else
		RB[rt] = 0;
	dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction andi behavior method.
void ac_behavior( andi )
{	
	dbg_printf("andi r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
	RB[rt] = RB[rs] & (imm & 0xFFFF) ;
	dbg_printf("Result = %#x\n", RB[rt]);
	reg_write = true;
};

//!Instruction ori behavior method.
void ac_behavior( ori )
{	
	dbg_printf("ori r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
	RB[rt] = RB[rs] | (imm & 0xFFFF) ;
	dbg_printf("Result = %#x\n", RB[rt]);
	reg_write = true;
};

//!Instruction xori behavior method.
void ac_behavior( xori )
{	
	dbg_printf("xori r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
	RB[rt] = RB[rs] ^ (imm & 0xFFFF) ;
	dbg_printf("Result = %#x\n", RB[rt]);
	reg_write = true;
};

//!Instruction lui behavior method.
void ac_behavior( lui )
{	
	dbg_printf("lui r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
	// Load a constant in the upper 16 bits of a register
	// To achieve the desired behaviour, the constant was shifted 16 bits left
	// and moved to the target register ( rt )
	RB[rt] = imm << 16;
	dbg_printf("Result = %#x\n", RB[rt]);
	reg_write = true;
};

//!Instruction add behavior method.
void ac_behavior( add )
{
	nadds++;
	dbg_printf("add r%d, r%d, r%d\n", rd, rs, rt);
	RB[rd] = RB[rs] + RB[rt];
	dbg_printf("Result = %#x\n", RB[rd]);
	//Test overflow
	if ( ((RB[rs] & 0x80000000) == (RB[rd] & 0x80000000)) &&
			((RB[rd] & 0x80000000) != (RB[rt] & 0x80000000)) ) {
		fprintf(stderr, "EXCEPTION(add): integer overflow.\n"); exit(EXIT_FAILURE);
	}
	reg_write = true;
};

//!Instruction addu behavior method.
void ac_behavior( addu )
{
	dbg_printf("addu r%d, r%d, r%d\n", rd, rs, rt);
	RB[rd] = RB[rs] + RB[rt];
	//cout << "  RS: " << (unsigned int)RB[rs] << " RT: " << (unsigned int)RB[rt] << endl;
	//cout << "  Result =  " <<  (unsigned int)RB[rd] <<endl;
	dbg_printf("Result = %#x\n", RB[rd]);
	reg_write = true;
};

//!Instruction sub behavior method.
void ac_behavior( sub )
{
	dbg_printf("sub r%d, r%d, r%d\n", rd, rs, rt);
	RB[rd] = RB[rs] - RB[rt];
	dbg_printf("Result = %#x\n", RB[rd]);
	//TODO: test integer overflow exception for sub
	reg_write = true;
};

//!Instruction subu behavior method.
void ac_behavior( subu )
{
	dbg_printf("subu r%d, r%d, r%d\n", rd, rs, rt);
	RB[rd] = RB[rs] - RB[rt];
	dbg_printf("Result = %#x\n", RB[rd]);
	reg_write = true;
};

//!Instruction slt behavior method.
void ac_behavior( slt )
{	
	dbg_printf("slt r%d, r%d, r%d\n", rd, rs, rt);
	// Set the RD if RS< RT
	if( (ac_Sword) RB[rs] < (ac_Sword) RB[rt] )
		RB[rd] = 1;
	// Else reset RD
	else
		RB[rd] = 0;
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sltu behavior method.
void ac_behavior( sltu )
{
	dbg_printf("sltu r%d, r%d, r%d\n", rd, rs, rt);
	// Set the RD if RS < RT
	if( RB[rs] < RB[rt] )
		RB[rd] = 1;
	// Else reset RD
	else
		RB[rd] = 0;
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction instr_and behavior method.
void ac_behavior( instr_and )
{
	dbg_printf("instr_and r%d, r%d, r%d\n", rd, rs, rt);
	RB[rd] = RB[rs] & RB[rt];
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction instr_or behavior method.
void ac_behavior( instr_or )
{
	dbg_printf("instr_or r%d, r%d, r%d\n", rd, rs, rt);
	RB[rd] = RB[rs] | RB[rt];
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction instr_xor behavior method.
void ac_behavior( instr_xor )
{
	dbg_printf("instr_xor r%d, r%d, r%d\n", rd, rs, rt);
	RB[rd] = RB[rs] ^ RB[rt];
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction instr_nor behavior method.
void ac_behavior( instr_nor )
{
	dbg_printf("nor r%d, r%d, r%d\n", rd, rs, rt);
	RB[rd] = ~(RB[rs] | RB[rt]);
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction nop behavior method.
void ac_behavior( nop )
{  
	dbg_printf("nop\n");
};

//!Instruction sll behavior method.
void ac_behavior( sll )
{  
	dbg_printf("sll r%d, r%d, %d\n", rd, rs, shamt);
	RB[rd] = RB[rt] << shamt;
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction srl behavior method.
void ac_behavior( srl )
{
	dbg_printf("srl r%d, r%d, %d\n", rd, rs, shamt);
	RB[rd] = RB[rt] >> shamt;
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sra behavior method.
void ac_behavior( sra )
{
	dbg_printf("sra r%d, r%d, %d\n", rd, rs, shamt);
	RB[rd] = (ac_Sword) RB[rt] >> shamt;
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sllv behavior method.
void ac_behavior( sllv )
{
	dbg_printf("sllv r%d, r%d, r%d\n", rd, rt, rs);
	RB[rd] = RB[rt] << (RB[rs] & 0x1F);
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction srlv behavior method.
void ac_behavior( srlv )
{
	dbg_printf("srlv r%d, r%d, r%d\n", rd, rt, rs);
	RB[rd] = RB[rt] >> (RB[rs] & 0x1F);
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction srav behavior method.
void ac_behavior( srav )
{
	dbg_printf("srav r%d, r%d, r%d\n", rd, rt, rs);
	RB[rd] = (ac_Sword) RB[rt] >> (RB[rs] & 0x1F);
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction mult behavior method.
void ac_behavior( mult )
{
	dbg_printf("mult r%d, r%d\n", rs, rt);

	long long result;
	int half_result;

	result = (ac_Sword) RB[rs];
	result *= (ac_Sword) RB[rt];

	half_result = (result & 0xFFFFFFFF);
	// Register LO receives 32 less significant bits
	lo = half_result;

	half_result = ((result >> 32) & 0xFFFFFFFF);
	// Register HI receives 32 most significant bits
	hi = half_result ;

	dbg_printf("Result = %#llx\n", result);
	reg_write = true;
};

//!Instruction multu behavior method.
void ac_behavior( multu )
{
	dbg_printf("multu r%d, r%d\n", rs, rt);

	unsigned long long result;
	unsigned int half_result;

	result  = RB[rs];
	result *= RB[rt];

	half_result = (result & 0xFFFFFFFF);
	// Register LO receives 32 less significant bits
	lo = half_result;

	half_result = ((result>>32) & 0xFFFFFFFF);
	// Register HI receives 32 most significant bits
	hi = half_result ;

	dbg_printf("Result = %#llx\n", result);
	reg_write = true;
};

//!Instruction div behavior method.
void ac_behavior( div )
{
	dbg_printf("div r%d, r%d\n", rs, rt);
	// Register LO receives quotient
	lo = (ac_Sword) RB[rs] / (ac_Sword) RB[rt];
	// Register HI receives remainder
	hi = (ac_Sword) RB[rs] % (ac_Sword) RB[rt];
	reg_write = true;
};

//!Instruction divu behavior method.
void ac_behavior( divu )
{
	dbg_printf("divu r%d, r%d\n", rs, rt);
	// Register LO receives quotient
	lo = RB[rs] / RB[rt];
	// Register HI receives remainder
	hi = RB[rs] % RB[rt];
	reg_write = true;
};

//!Instruction mfhi behavior method.
void ac_behavior( mfhi )
{
	dbg_printf("mfhi r%d\n", rd);
	RB[rd] = hi;
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction mthi behavior method.
void ac_behavior( mthi )
{
	dbg_printf("mthi r%d\n", rs);
	hi = RB[rs];
	dbg_printf("Result = %#x\n", hi);
};

//!Instruction mflo behavior method.
void ac_behavior( mflo )
{
	dbg_printf("mflo r%d\n", rd);
	RB[rd] = lo;
	dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction mtlo behavior method.
void ac_behavior( mtlo )
{
	dbg_printf("mtlo r%d\n", rs);
	lo = RB[rs];
	dbg_printf("Result = %#x\n", lo);
};

//!Instruction j behavior method.
void ac_behavior( j )
{
	dbg_printf("j %d\n", addr);
	addr = addr << 2;
#ifndef NO_NEED_PC_UPDATE
	npc =  (ac_pc & 0xF0000000) | addr;
#endif 
	dbg_printf("Target = %#x\n", (ac_pc & 0xF0000000) | addr );
};

//!Instruction jal behavior method.
void ac_behavior( jal )
{
	dbg_printf("jal %d\n", addr);
	// Save the value of PC + 8 (return address) in $ra ($31) and
	// jump to the address given by PC(31...28)||(addr<<2)
	// It must also flush the instructions that were loaded into the pipeline
	RB[Ra] = ac_pc+4; //ac_pc is pc+4, we need pc+8

	addr = addr << 2;
#ifndef NO_NEED_PC_UPDATE
	npc = (ac_pc & 0xF0000000) | addr;
#endif 

	dbg_printf("Target = %#x\n", (ac_pc & 0xF0000000) | addr );
	dbg_printf("Return = %#x\n", ac_pc+4);
};

//!Instruction jr behavior method.
void ac_behavior( jr )
{
	dbg_printf("jr r%d\n", rs);
	// Jump to the address stored on the register reg[RS]
	// It must also flush the instructions that were loaded into the pipeline
#ifndef NO_NEED_PC_UPDATE
	npc = RB[rs], 1;
#endif 
	dbg_printf("Target = %#x\n", RB[rs]);
};

//!Instruction jalr behavior method.
void ac_behavior( jalr )
{
	dbg_printf("jalr r%d, r%d\n", rd, rs);
	// Save the value of PC + 8(return address) in rd and
	// jump to the address given by [rs]

#ifndef NO_NEED_PC_UPDATE
	npc = RB[rs], 1;
#endif 
	dbg_printf("Target = %#x\n", RB[rs]);

	if( rd == 0 )  //If rd is not defined use default
		rd = Ra;
	RB[rd] = ac_pc+4;
	dbg_printf("Return = %#x\n", ac_pc+4);
};

//!Instruction beq behavior method.
void ac_behavior( beq )
{
	branch_count++;
	branch_state_aux = false;
	dbg_printf("beq r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
	if( RB[rs] == RB[rt] ){
#ifndef NO_NEED_PC_UPDATE
		npc = ac_pc + (imm<<2);
#endif 
		dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
		not_taken++;
		branch_state_aux = true;
	}	else always_taken++;
	OneBitFunc(imm, branch_state_aux);
	NaiveFunc(branch_state_aux);
};

//!Instruction bne behavior method.
void ac_behavior( bne )
{
	branch_count++;
	branch_state_aux = false;
	dbg_printf("bne r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
	if( RB[rs] != RB[rt] ){
#ifndef NO_NEED_PC_UPDATE
		npc = ac_pc + (imm<<2);
#endif 
		dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
		not_taken++;
		branch_state_aux = true;
	}	else always_taken++;
	OneBitFunc(imm, branch_state_aux);
	NaiveFunc(branch_state_aux);
};

//!Instruction blez behavior method.
void ac_behavior( blez )
{
	branch_count++;
	branch_state_aux = false;
	dbg_printf("blez r%d, %d\n", rs, imm & 0xFFFF);
	if( (RB[rs] == 0 ) || (RB[rs]&0x80000000 ) ){
#ifndef NO_NEED_PC_UPDATE
		npc = ac_pc + (imm<<2), 1;
#endif 
		dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
		not_taken++;
		branch_state_aux = true;
	}	else always_taken++;
	OneBitFunc(imm, branch_state_aux);
	NaiveFunc(branch_state_aux);
};

//!Instruction bgtz behavior method.
void ac_behavior( bgtz )
{
	branch_count++;
	branch_state_aux = false;
	dbg_printf("bgtz r%d, %d\n", rs, imm & 0xFFFF);
	if( !(RB[rs] & 0x80000000) && (RB[rs]!=0) ){
#ifndef NO_NEED_PC_UPDATE
		npc = ac_pc + (imm<<2);
#endif 
		dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
		not_taken++;
		branch_state_aux = true;
	}	else always_taken++;
	OneBitFunc(imm, branch_state_aux);
	NaiveFunc(branch_state_aux);
};

//!Instruction bltz behavior method.
void ac_behavior( bltz )
{
	branch_count++;
	branch_state_aux = false;
	dbg_printf("bltz r%d, %d\n", rs, imm & 0xFFFF);
	if( RB[rs] & 0x80000000 ){
#ifndef NO_NEED_PC_UPDATE
		npc = ac_pc + (imm<<2);
#endif 
		dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
		not_taken++;
		branch_state_aux = true;
	}	else always_taken++;
	OneBitFunc(imm, branch_state_aux);
	NaiveFunc(branch_state_aux);
};

//!Instruction bgez behavior method.
void ac_behavior( bgez )
{
	branch_count++;
	branch_state_aux = false;
	dbg_printf("bgez r%d, %d\n", rs, imm & 0xFFFF);
	if( !(RB[rs] & 0x80000000) ){
#ifndef NO_NEED_PC_UPDATE
		npc = ac_pc + (imm<<2);
#endif 
		dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
		not_taken++;
		branch_state_aux = true;
	}	else always_taken++;
	OneBitFunc(imm, branch_state_aux);
	NaiveFunc(branch_state_aux);
};

//!Instruction bltzal behavior method.
void ac_behavior( bltzal )
{
	branch_count++;
	branch_state_aux = false;
	dbg_printf("bltzal r%d, %d\n", rs, imm & 0xFFFF);
	RB[Ra] = ac_pc+4; //ac_pc is pc+4, we need pc+8
	if( RB[rs] & 0x80000000 ){
#ifndef NO_NEED_PC_UPDATE
		npc = ac_pc + (imm<<2);
#endif 
		dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
		not_taken++;
		branch_state_aux = true;
	}	else always_taken++;
	OneBitFunc(imm, branch_state_aux);
	NaiveFunc(branch_state_aux);
	dbg_printf("Return = %#x\n", ac_pc+4);
};

//!Instruction bgezal behavior method.
void ac_behavior( bgezal )
{
	branch_count++;
	branch_state_aux = false;
	dbg_printf("bgezal r%d, %d\n", rs, imm & 0xFFFF);
	RB[Ra] = ac_pc+4; //ac_pc is pc+4, we need pc+8
	if( !(RB[rs] & 0x80000000) ){
#ifndef NO_NEED_PC_UPDATE
		npc = ac_pc + (imm<<2);
#endif 
		dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
		not_taken++;
		branch_state_aux = true;
	}	else always_taken++;
	OneBitFunc(imm, branch_state_aux);
	NaiveFunc(branch_state_aux);
	dbg_printf("Return = %#x\n", ac_pc+4);
};

//!Instruction sys_call behavior method.
void ac_behavior( sys_call )
{
	dbg_printf("syscall\n");
	stop();
}

//!Instruction instr_break behavior method.
void ac_behavior( instr_break )
{
	fprintf(stderr, "instr_break behavior not implemented.\n"); 
	exit(EXIT_FAILURE);
}
