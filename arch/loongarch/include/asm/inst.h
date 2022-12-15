/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020-2022 Loongson Technology Corporation Limited
 */
#ifndef _ASM_INST_H
#define _ASM_INST_H

#include <linux/types.h>
#include <asm/asm.h>

#define INSN_BREAK		0x002a0000

#define ADDR_IMMMASK_LU52ID	0xFFF0000000000000
#define ADDR_IMMMASK_LU32ID	0x000FFFFF00000000
#define ADDR_IMMMASK_ADDU16ID	0x00000000FFFF0000

#define ADDR_IMMSHIFT_LU52ID	52
#define ADDR_IMMSHIFT_LU32ID	32
#define ADDR_IMMSHIFT_ADDU16ID	16

#define ADDR_IMM(addr, INSN)	((addr & ADDR_IMMMASK_##INSN) >> ADDR_IMMSHIFT_##INSN)

enum reg0i26_op {
	b_op		= 0x14,
	bl_op		= 0x15,
};

enum reg1i20_op {
	lu12iw_op	= 0x0a,
	lu32id_op	= 0x0b,
	pcaddu12i_op	= 0x0e,
	pcaddu18i_op	= 0x0f,
};

enum reg1i21_op {
	beqz_op		= 0x10,
	bnez_op		= 0x11,
};

enum reg2_op {
	revb2h_op	= 0x0c,
	revb4h_op	= 0x0d,
	revb2w_op	= 0x0e,
	revbd_op	= 0x0f,
	revh2w_op	= 0x10,
	revhd_op	= 0x11,
};

enum reg2i5_op {
	slliw_op	= 0x81,
	srliw_op	= 0x89,
	sraiw_op	= 0x91,
};

enum reg2i6_op {
	sllid_op	= 0x41,
	srlid_op	= 0x45,
	sraid_op	= 0x49,
};

enum reg2i12_op {
	addiw_op	= 0x0a,
	addid_op	= 0x0b,
	lu52id_op	= 0x0c,
	andi_op		= 0x0d,
	ori_op		= 0x0e,
	xori_op		= 0x0f,
	ldb_op		= 0xa0,
	ldh_op		= 0xa1,
	ldw_op		= 0xa2,
	ldd_op		= 0xa3,
	stb_op		= 0xa4,
	sth_op		= 0xa5,
	stw_op		= 0xa6,
	std_op		= 0xa7,
	ldbu_op		= 0xa8,
	ldhu_op		= 0xa9,
	ldwu_op		= 0xaa,
};

enum reg2i14_op {
	llw_op		= 0x20,
	scw_op		= 0x21,
	lld_op		= 0x22,
	scd_op		= 0x23,
	ldptrw_op	= 0x24,
	stptrw_op	= 0x25,
	ldptrd_op	= 0x26,
	stptrd_op	= 0x27,
};

enum reg2i16_op {
	jirl_op		= 0x13,
	beq_op		= 0x16,
	bne_op		= 0x17,
	blt_op		= 0x18,
	bge_op		= 0x19,
	bltu_op		= 0x1a,
	bgeu_op		= 0x1b,
};

enum reg2bstrd_op {
	bstrinsd_op	= 0x2,
	bstrpickd_op	= 0x3,
};

enum reg3_op {
	addw_op		= 0x20,
	addd_op		= 0x21,
	subw_op		= 0x22,
	subd_op		= 0x23,
	nor_op		= 0x28,
	and_op		= 0x29,
	or_op		= 0x2a,
	xor_op		= 0x2b,
	orn_op		= 0x2c,
	andn_op		= 0x2d,
	sllw_op		= 0x2e,
	srlw_op		= 0x2f,
	sraw_op		= 0x30,
	slld_op		= 0x31,
	srld_op		= 0x32,
	srad_op		= 0x33,
	mulw_op		= 0x38,
	mulhw_op	= 0x39,
	mulhwu_op	= 0x3a,
	muld_op		= 0x3b,
	mulhd_op	= 0x3c,
	mulhdu_op	= 0x3d,
	divw_op		= 0x40,
	modw_op		= 0x41,
	divwu_op	= 0x42,
	modwu_op	= 0x43,
	divd_op		= 0x44,
	modd_op		= 0x45,
	divdu_op	= 0x46,
	moddu_op	= 0x47,
	ldxb_op		= 0x7000,
	ldxh_op		= 0x7008,
	ldxw_op		= 0x7010,
	ldxd_op		= 0x7018,
	stxb_op		= 0x7020,
	stxh_op		= 0x7028,
	stxw_op		= 0x7030,
	stxd_op		= 0x7038,
	ldxbu_op	= 0x7040,
	ldxhu_op	= 0x7048,
	ldxwu_op	= 0x7050,
	amswapw_op	= 0x70c0,
	amswapd_op	= 0x70c1,
	amaddw_op	= 0x70c2,
	amaddd_op	= 0x70c3,
	amandw_op	= 0x70c4,
	amandd_op	= 0x70c5,
	amorw_op	= 0x70c6,
	amord_op	= 0x70c7,
	amxorw_op	= 0x70c8,
	amxord_op	= 0x70c9,
};

enum reg3sa2_op {
	alslw_op	= 0x02,
	alslwu_op	= 0x03,
	alsld_op	= 0x16,
};

struct reg0i26_format {
	unsigned int immediate_h : 10;
	unsigned int immediate_l : 16;
	unsigned int opcode : 6;
};

struct reg1i20_format {
	unsigned int rd : 5;
	unsigned int immediate : 20;
	unsigned int opcode : 7;
};

struct reg1i21_format {
	unsigned int immediate_h  : 5;
	unsigned int rj : 5;
	unsigned int immediate_l : 16;
	unsigned int opcode : 6;
};

struct reg2_format {
	unsigned int rd : 5;
	unsigned int rj : 5;
	unsigned int opcode : 22;
};

struct reg2i5_format {
	unsigned int rd : 5;
	unsigned int rj : 5;
	unsigned int immediate : 5;
	unsigned int opcode : 17;
};

struct reg2i6_format {
	unsigned int rd : 5;
	unsigned int rj : 5;
	unsigned int immediate : 6;
	unsigned int opcode : 16;
};

struct reg2i12_format {
	unsigned int rd : 5;
	unsigned int rj : 5;
	unsigned int immediate : 12;
	unsigned int opcode : 10;
};

struct reg2i14_format {
	unsigned int rd : 5;
	unsigned int rj : 5;
	unsigned int immediate : 14;
	unsigned int opcode : 8;
};

struct reg2i16_format {
	unsigned int rd : 5;
	unsigned int rj : 5;
	unsigned int immediate : 16;
	unsigned int opcode : 6;
};

struct reg2bstrd_format {
	unsigned int rd : 5;
	unsigned int rj : 5;
	unsigned int lsbd : 6;
	unsigned int msbd : 6;
	unsigned int opcode : 10;
};

struct reg3_format {
	unsigned int rd : 5;
	unsigned int rj : 5;
	unsigned int rk : 5;
	unsigned int opcode : 17;
};

struct reg3sa2_format {
	unsigned int rd : 5;
	unsigned int rj : 5;
	unsigned int rk : 5;
	unsigned int immediate : 2;
	unsigned int opcode : 15;
};

union loongarch_instruction {
	unsigned int word;
	struct reg0i26_format	reg0i26_format;
	struct reg1i20_format	reg1i20_format;
	struct reg1i21_format	reg1i21_format;
	struct reg2_format	reg2_format;
	struct reg2i5_format	reg2i5_format;
	struct reg2i6_format	reg2i6_format;
	struct reg2i12_format	reg2i12_format;
	struct reg2i14_format	reg2i14_format;
	struct reg2i16_format	reg2i16_format;
	struct reg2bstrd_format	reg2bstrd_format;
	struct reg3_format	reg3_format;
	struct reg3sa2_format	reg3sa2_format;
};

#define LOONGARCH_INSN_SIZE	sizeof(union loongarch_instruction)

enum loongarch_gpr {
	LOONGARCH_GPR_ZERO = 0,
	LOONGARCH_GPR_RA = 1,
	LOONGARCH_GPR_TP = 2,
	LOONGARCH_GPR_SP = 3,
	LOONGARCH_GPR_A0 = 4,	/* Reused as V0 for return value */
	LOONGARCH_GPR_A1,	/* Reused as V1 for return value */
	LOONGARCH_GPR_A2,
	LOONGARCH_GPR_A3,
	LOONGARCH_GPR_A4,
	LOONGARCH_GPR_A5,
	LOONGARCH_GPR_A6,
	LOONGARCH_GPR_A7,
	LOONGARCH_GPR_T0 = 12,
	LOONGARCH_GPR_T1,
	LOONGARCH_GPR_T2,
	LOONGARCH_GPR_T3,
	LOONGARCH_GPR_T4,
	LOONGARCH_GPR_T5,
	LOONGARCH_GPR_T6,
	LOONGARCH_GPR_T7,
	LOONGARCH_GPR_T8,
	LOONGARCH_GPR_FP = 22,
	LOONGARCH_GPR_S0 = 23,
	LOONGARCH_GPR_S1,
	LOONGARCH_GPR_S2,
	LOONGARCH_GPR_S3,
	LOONGARCH_GPR_S4,
	LOONGARCH_GPR_S5,
	LOONGARCH_GPR_S6,
	LOONGARCH_GPR_S7,
	LOONGARCH_GPR_S8,
	LOONGARCH_GPR_MAX
};

#define is_imm12_negative(val)	is_imm_negative(val, 12)

static inline bool is_imm_negative(unsigned long val, unsigned int bit)
{
	return val & (1UL << (bit - 1));
}

static inline bool is_branch_ins(union loongarch_instruction *ip)
{
	return ip->reg1i21_format.opcode >= beqz_op &&
		ip->reg1i21_format.opcode <= bgeu_op;
}

static inline bool is_ra_save_ins(union loongarch_instruction *ip)
{
	/* st.d $ra, $sp, offset */
	return ip->reg2i12_format.opcode == std_op &&
		ip->reg2i12_format.rj == LOONGARCH_GPR_SP &&
		ip->reg2i12_format.rd == LOONGARCH_GPR_RA &&
		!is_imm12_negative(ip->reg2i12_format.immediate);
}

static inline bool is_stack_alloc_ins(union loongarch_instruction *ip)
{
	/* addi.d $sp, $sp, -imm */
	return ip->reg2i12_format.opcode == addid_op &&
		ip->reg2i12_format.rj == LOONGARCH_GPR_SP &&
		ip->reg2i12_format.rd == LOONGARCH_GPR_SP &&
		is_imm12_negative(ip->reg2i12_format.immediate);
}

u32 larch_insn_gen_lu32id(enum loongarch_gpr rd, int imm);
u32 larch_insn_gen_lu52id(enum loongarch_gpr rd, enum loongarch_gpr rj, int imm);
u32 larch_insn_gen_jirl(enum loongarch_gpr rd, enum loongarch_gpr rj, unsigned long pc, unsigned long dest);

static inline bool signed_imm_check(long val, unsigned int bit)
{
	return -(1L << (bit - 1)) <= val && val < (1L << (bit - 1));
}

static inline bool unsigned_imm_check(unsigned long val, unsigned int bit)
{
	return val < (1UL << bit);
}

#define DEF_EMIT_REG0I26_FORMAT(NAME, OP)				\
static inline void emit_##NAME(union loongarch_instruction *insn,	\
			       int offset)				\
{									\
	unsigned int immediate_l, immediate_h;				\
									\
	immediate_l = offset & 0xffff;					\
	offset >>= 16;							\
	immediate_h = offset & 0x3ff;					\
									\
	insn->reg0i26_format.opcode = OP;				\
	insn->reg0i26_format.immediate_l = immediate_l;			\
	insn->reg0i26_format.immediate_h = immediate_h;			\
}

DEF_EMIT_REG0I26_FORMAT(b, b_op)

#define DEF_EMIT_REG1I20_FORMAT(NAME, OP)				\
static inline void emit_##NAME(union loongarch_instruction *insn,	\
			       enum loongarch_gpr rd, int imm)		\
{									\
	insn->reg1i20_format.opcode = OP;				\
	insn->reg1i20_format.immediate = imm;				\
	insn->reg1i20_format.rd = rd;					\
}

DEF_EMIT_REG1I20_FORMAT(lu12iw, lu12iw_op)
DEF_EMIT_REG1I20_FORMAT(lu32id, lu32id_op)
DEF_EMIT_REG1I20_FORMAT(pcaddu18i, pcaddu18i_op)

#define DEF_EMIT_REG2_FORMAT(NAME, OP)					\
static inline void emit_##NAME(union loongarch_instruction *insn,	\
			       enum loongarch_gpr rd,			\
			       enum loongarch_gpr rj)			\
{									\
	insn->reg2_format.opcode = OP;					\
	insn->reg2_format.rd = rd;					\
	insn->reg2_format.rj = rj;					\
}

DEF_EMIT_REG2_FORMAT(revb2h, revb2h_op)
DEF_EMIT_REG2_FORMAT(revb2w, revb2w_op)
DEF_EMIT_REG2_FORMAT(revbd, revbd_op)

#define DEF_EMIT_REG2I5_FORMAT(NAME, OP)				\
static inline void emit_##NAME(union loongarch_instruction *insn,	\
			       enum loongarch_gpr rd,			\
			       enum loongarch_gpr rj,			\
			       int imm)					\
{									\
	insn->reg2i5_format.opcode = OP;				\
	insn->reg2i5_format.immediate = imm;				\
	insn->reg2i5_format.rd = rd;					\
	insn->reg2i5_format.rj = rj;					\
}

DEF_EMIT_REG2I5_FORMAT(slliw, slliw_op)
DEF_EMIT_REG2I5_FORMAT(srliw, srliw_op)
DEF_EMIT_REG2I5_FORMAT(sraiw, sraiw_op)

#define DEF_EMIT_REG2I6_FORMAT(NAME, OP)				\
static inline void emit_##NAME(union loongarch_instruction *insn,	\
			       enum loongarch_gpr rd,			\
			       enum loongarch_gpr rj,			\
			       int imm)					\
{									\
	insn->reg2i6_format.opcode = OP;				\
	insn->reg2i6_format.immediate = imm;				\
	insn->reg2i6_format.rd = rd;					\
	insn->reg2i6_format.rj = rj;					\
}

DEF_EMIT_REG2I6_FORMAT(sllid, sllid_op)
DEF_EMIT_REG2I6_FORMAT(srlid, srlid_op)
DEF_EMIT_REG2I6_FORMAT(sraid, sraid_op)

#define DEF_EMIT_REG2I12_FORMAT(NAME, OP)				\
static inline void emit_##NAME(union loongarch_instruction *insn,	\
			       enum loongarch_gpr rd,			\
			       enum loongarch_gpr rj,			\
			       int imm)					\
{									\
	insn->reg2i12_format.opcode = OP;				\
	insn->reg2i12_format.immediate = imm;				\
	insn->reg2i12_format.rd = rd;					\
	insn->reg2i12_format.rj = rj;					\
}

DEF_EMIT_REG2I12_FORMAT(addiw, addiw_op)
DEF_EMIT_REG2I12_FORMAT(addid, addid_op)
DEF_EMIT_REG2I12_FORMAT(lu52id, lu52id_op)
DEF_EMIT_REG2I12_FORMAT(andi, andi_op)
DEF_EMIT_REG2I12_FORMAT(ori, ori_op)
DEF_EMIT_REG2I12_FORMAT(xori, xori_op)
DEF_EMIT_REG2I12_FORMAT(ldbu, ldbu_op)
DEF_EMIT_REG2I12_FORMAT(ldhu, ldhu_op)
DEF_EMIT_REG2I12_FORMAT(ldwu, ldwu_op)
DEF_EMIT_REG2I12_FORMAT(ldd, ldd_op)
DEF_EMIT_REG2I12_FORMAT(stb, stb_op)
DEF_EMIT_REG2I12_FORMAT(sth, sth_op)
DEF_EMIT_REG2I12_FORMAT(stw, stw_op)
DEF_EMIT_REG2I12_FORMAT(std, std_op)

#define DEF_EMIT_REG2I14_FORMAT(NAME, OP)				\
static inline void emit_##NAME(union loongarch_instruction *insn,	\
			       enum loongarch_gpr rd,			\
			       enum loongarch_gpr rj,			\
			       int imm)					\
{									\
	insn->reg2i14_format.opcode = OP;				\
	insn->reg2i14_format.immediate = imm;				\
	insn->reg2i14_format.rd = rd;					\
	insn->reg2i14_format.rj = rj;					\
}

DEF_EMIT_REG2I14_FORMAT(llw, llw_op)
DEF_EMIT_REG2I14_FORMAT(scw, scw_op)
DEF_EMIT_REG2I14_FORMAT(lld, lld_op)
DEF_EMIT_REG2I14_FORMAT(scd, scd_op)
DEF_EMIT_REG2I14_FORMAT(ldptrw, ldptrw_op)
DEF_EMIT_REG2I14_FORMAT(stptrw, stptrw_op)
DEF_EMIT_REG2I14_FORMAT(ldptrd, ldptrd_op)
DEF_EMIT_REG2I14_FORMAT(stptrd, stptrd_op)

#define DEF_EMIT_REG2I16_FORMAT(NAME, OP)				\
static inline void emit_##NAME(union loongarch_instruction *insn,	\
			       enum loongarch_gpr rj,			\
			       enum loongarch_gpr rd,			\
			       int offset)				\
{									\
	insn->reg2i16_format.opcode = OP;				\
	insn->reg2i16_format.immediate = offset;			\
	insn->reg2i16_format.rj = rj;					\
	insn->reg2i16_format.rd = rd;					\
}

DEF_EMIT_REG2I16_FORMAT(beq, beq_op)
DEF_EMIT_REG2I16_FORMAT(bne, bne_op)
DEF_EMIT_REG2I16_FORMAT(blt, blt_op)
DEF_EMIT_REG2I16_FORMAT(bge, bge_op)
DEF_EMIT_REG2I16_FORMAT(bltu, bltu_op)
DEF_EMIT_REG2I16_FORMAT(bgeu, bgeu_op)
DEF_EMIT_REG2I16_FORMAT(jirl, jirl_op)

#define DEF_EMIT_REG2BSTRD_FORMAT(NAME, OP)				\
static inline void emit_##NAME(union loongarch_instruction *insn,	\
			       enum loongarch_gpr rd,			\
			       enum loongarch_gpr rj,			\
			       int msbd,				\
			       int lsbd)				\
{									\
	insn->reg2bstrd_format.opcode = OP;				\
	insn->reg2bstrd_format.msbd = msbd;				\
	insn->reg2bstrd_format.lsbd = lsbd;				\
	insn->reg2bstrd_format.rj = rj;					\
	insn->reg2bstrd_format.rd = rd;					\
}

DEF_EMIT_REG2BSTRD_FORMAT(bstrpickd, bstrpickd_op)

#define DEF_EMIT_REG3_FORMAT(NAME, OP)					\
static inline void emit_##NAME(union loongarch_instruction *insn,	\
			       enum loongarch_gpr rd,			\
			       enum loongarch_gpr rj,			\
			       enum loongarch_gpr rk)			\
{									\
	insn->reg3_format.opcode = OP;					\
	insn->reg3_format.rd = rd;					\
	insn->reg3_format.rj = rj;					\
	insn->reg3_format.rk = rk;					\
}

DEF_EMIT_REG3_FORMAT(addd, addd_op)
DEF_EMIT_REG3_FORMAT(subd, subd_op)
DEF_EMIT_REG3_FORMAT(muld, muld_op)
DEF_EMIT_REG3_FORMAT(divdu, divdu_op)
DEF_EMIT_REG3_FORMAT(moddu, moddu_op)
DEF_EMIT_REG3_FORMAT(and, and_op)
DEF_EMIT_REG3_FORMAT(or, or_op)
DEF_EMIT_REG3_FORMAT(xor, xor_op)
DEF_EMIT_REG3_FORMAT(sllw, sllw_op)
DEF_EMIT_REG3_FORMAT(slld, slld_op)
DEF_EMIT_REG3_FORMAT(srlw, srlw_op)
DEF_EMIT_REG3_FORMAT(srld, srld_op)
DEF_EMIT_REG3_FORMAT(sraw, sraw_op)
DEF_EMIT_REG3_FORMAT(srad, srad_op)
DEF_EMIT_REG3_FORMAT(ldxbu, ldxbu_op)
DEF_EMIT_REG3_FORMAT(ldxhu, ldxhu_op)
DEF_EMIT_REG3_FORMAT(ldxwu, ldxwu_op)
DEF_EMIT_REG3_FORMAT(ldxd, ldxd_op)
DEF_EMIT_REG3_FORMAT(stxb, stxb_op)
DEF_EMIT_REG3_FORMAT(stxh, stxh_op)
DEF_EMIT_REG3_FORMAT(stxw, stxw_op)
DEF_EMIT_REG3_FORMAT(stxd, stxd_op)
DEF_EMIT_REG3_FORMAT(amaddw, amaddw_op)
DEF_EMIT_REG3_FORMAT(amaddd, amaddd_op)
DEF_EMIT_REG3_FORMAT(amandw, amandw_op)
DEF_EMIT_REG3_FORMAT(amandd, amandd_op)
DEF_EMIT_REG3_FORMAT(amorw, amorw_op)
DEF_EMIT_REG3_FORMAT(amord, amord_op)
DEF_EMIT_REG3_FORMAT(amxorw, amxorw_op)
DEF_EMIT_REG3_FORMAT(amxord, amxord_op)
DEF_EMIT_REG3_FORMAT(amswapw, amswapw_op)
DEF_EMIT_REG3_FORMAT(amswapd, amswapd_op)

#define DEF_EMIT_REG3SA2_FORMAT(NAME, OP)				\
static inline void emit_##NAME(union loongarch_instruction *insn,	\
			       enum loongarch_gpr rd,			\
			       enum loongarch_gpr rj,			\
			       enum loongarch_gpr rk,			\
			       int imm)					\
{									\
	insn->reg3sa2_format.opcode = OP;				\
	insn->reg3sa2_format.immediate = imm;				\
	insn->reg3sa2_format.rd = rd;					\
	insn->reg3sa2_format.rj = rj;					\
	insn->reg3sa2_format.rk = rk;					\
}

DEF_EMIT_REG3SA2_FORMAT(alsld, alsld_op)

#endif /* _ASM_INST_H */
