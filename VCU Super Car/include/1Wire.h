#ifndef __1Wire_h
#define __1Wire_h

#define HET_v2 1
#define AID1_7

#include "std_nhet.h"

#define HET_L00_0	(e_HETPROGRAM0_UN.Program0_ST.L00_0)
#define pHET_L00_0  	0

#define HET_L01_0	(e_HETPROGRAM0_UN.Program0_ST.L01_0)
#define pHET_L01_0  	1

#define HET_L02_0	(e_HETPROGRAM0_UN.Program0_ST.L02_0)
#define pHET_L02_0  	2

#define HET_L03_0	(e_HETPROGRAM0_UN.Program0_ST.L03_0)
#define pHET_L03_0  	3

#define HET_L04_0	(e_HETPROGRAM0_UN.Program0_ST.L04_0)
#define pHET_L04_0  	4

#define HET_L05_0	(e_HETPROGRAM0_UN.Program0_ST.L05_0)
#define pHET_L05_0  	5

#define HET_L06_0	(e_HETPROGRAM0_UN.Program0_ST.L06_0)
#define pHET_L06_0  	6

#define HET_L07_0	(e_HETPROGRAM0_UN.Program0_ST.L07_0)
#define pHET_L07_0  	7

#define HET_L08_0	(e_HETPROGRAM0_UN.Program0_ST.L08_0)
#define pHET_L08_0  	8

#define HET_L09_0	(e_HETPROGRAM0_UN.Program0_ST.L09_0)
#define pHET_L09_0  	9

#define HET_L10_0	(e_HETPROGRAM0_UN.Program0_ST.L10_0)
#define pHET_L10_0  	10

#define HET_L11_0	(e_HETPROGRAM0_UN.Program0_ST.L11_0)
#define pHET_L11_0  	11

#define HET_L12_0	(e_HETPROGRAM0_UN.Program0_ST.L12_0)
#define pHET_L12_0  	12

#define HET_L13_0	(e_HETPROGRAM0_UN.Program0_ST.L13_0)
#define pHET_L13_0  	13

#define HET_L14_0	(e_HETPROGRAM0_UN.Program0_ST.L14_0)
#define pHET_L14_0  	14

#define HET_L15_0	(e_HETPROGRAM0_UN.Program0_ST.L15_0)
#define pHET_L15_0  	15

#define HET_L16_0	(e_HETPROGRAM0_UN.Program0_ST.L16_0)
#define pHET_L16_0  	16

#define HET_L17_0	(e_HETPROGRAM0_UN.Program0_ST.L17_0)
#define pHET_L17_0  	17

#define HET_L18_0	(e_HETPROGRAM0_UN.Program0_ST.L18_0)
#define pHET_L18_0  	18

#define HET_L19_0	(e_HETPROGRAM0_UN.Program0_ST.L19_0)
#define pHET_L19_0  	19

#define HET_L20_0	(e_HETPROGRAM0_UN.Program0_ST.L20_0)
#define pHET_L20_0  	20

#define HET_L21_0	(e_HETPROGRAM0_UN.Program0_ST.L21_0)
#define pHET_L21_0  	21

#define HET_L22_0	(e_HETPROGRAM0_UN.Program0_ST.L22_0)
#define pHET_L22_0  	22

#define HET_L23_0	(e_HETPROGRAM0_UN.Program0_ST.L23_0)
#define pHET_L23_0  	23

#define HET_L24_0	(e_HETPROGRAM0_UN.Program0_ST.L24_0)
#define pHET_L24_0  	24

#define HET_L25_0	(e_HETPROGRAM0_UN.Program0_ST.L25_0)
#define pHET_L25_0  	25

#define HET_L26_0	(e_HETPROGRAM0_UN.Program0_ST.L26_0)
#define pHET_L26_0  	26

#define HET_L27_0	(e_HETPROGRAM0_UN.Program0_ST.L27_0)
#define pHET_L27_0  	27

#define HET_L28_0	(e_HETPROGRAM0_UN.Program0_ST.L28_0)
#define pHET_L28_0  	28

#define HET_L29_0	(e_HETPROGRAM0_UN.Program0_ST.L29_0)
#define pHET_L29_0  	29

#define HET_L30_0	(e_HETPROGRAM0_UN.Program0_ST.L30_0)
#define pHET_L30_0  	30

#define HET_L31_0	(e_HETPROGRAM0_UN.Program0_ST.L31_0)
#define pHET_L31_0  	31

#define HET_L32_0	(e_HETPROGRAM0_UN.Program0_ST.L32_0)
#define pHET_L32_0  	32

#define HET_L33_0	(e_HETPROGRAM0_UN.Program0_ST.L33_0)
#define pHET_L33_0  	33

#define HET_L34_0	(e_HETPROGRAM0_UN.Program0_ST.L34_0)
#define pHET_L34_0  	34

#define HET_L35_0	(e_HETPROGRAM0_UN.Program0_ST.L35_0)
#define pHET_L35_0  	35

#define HET_L36_0	(e_HETPROGRAM0_UN.Program0_ST.L36_0)
#define pHET_L36_0  	36

#define HET_L37_0	(e_HETPROGRAM0_UN.Program0_ST.L37_0)
#define pHET_L37_0  	37

#define HET_L38_0	(e_HETPROGRAM0_UN.Program0_ST.L38_0)
#define pHET_L38_0  	38

#define HET_L39_0	(e_HETPROGRAM0_UN.Program0_ST.L39_0)
#define pHET_L39_0  	39

#define HET_DUMMY_0	(e_HETPROGRAM0_UN.Program0_ST.DUMMY_0)
#define pHET_DUMMY_0  	40



typedef union 
{ 
 	HET_MEMORY	Memory0_PST[41];
	struct
	{
		MOV32_INSTRUCTION L00_0;
		ECMP_INSTRUCTION L01_0;
		PWCNT_INSTRUCTION L02_0;
		PCNT_INSTRUCTION L03_0;
		BR_INSTRUCTION L04_0;
		SUB_INSTRUCTION L05_0;
		MCMP_INSTRUCTION L06_0;
		MOV32_INSTRUCTION L07_0;
		ECMP_INSTRUCTION L08_0;
		ECMP_INSTRUCTION L09_0;
		ECMP_INSTRUCTION L10_0;
		MOV32_INSTRUCTION L11_0;
		CNT_INSTRUCTION L12_0;
		BR_INSTRUCTION L13_0;
		AND_INSTRUCTION L14_0;
		ECMP_INSTRUCTION L15_0;
		MOV32_INSTRUCTION L16_0;
		MOV32_INSTRUCTION L17_0;
		AND_INSTRUCTION L18_0;
		MOV32_INSTRUCTION L19_0;
		PWCNT_INSTRUCTION L20_0;
		MOV32_INSTRUCTION L21_0;
		DJZ_INSTRUCTION L22_0;
		ECMP_INSTRUCTION L23_0;
		ECMP_INSTRUCTION L24_0;
		DJZ_INSTRUCTION L25_0;
		CNT_INSTRUCTION L26_0;
		BR_INSTRUCTION L27_0;
		DJZ_INSTRUCTION L28_0;
		MOV32_INSTRUCTION L29_0;
		PWCNT_INSTRUCTION L30_0;
		MOV32_INSTRUCTION L31_0;
		PCNT_INSTRUCTION L32_0;
		BR_INSTRUCTION L33_0;
		SUB_INSTRUCTION L34_0;
		MCMP_INSTRUCTION L35_0;
		OR_INSTRUCTION L36_0;
		OR_INSTRUCTION L37_0;
		AND_INSTRUCTION L38_0;
		MOV32_INSTRUCTION L39_0;
		BR_INSTRUCTION DUMMY_0;
	} Program0_ST; 

} HETPROGRAM0_UN;

extern volatile HETPROGRAM0_UN e_HETPROGRAM0_UN;

extern const HET_MEMORY HET_INIT0_PST[41];

#endif

