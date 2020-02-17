/*
 * File:    pll_init.h
 * Purpose: pll_driver specific declarations
 *
 * Notes:
 */
#ifndef __MCG_H__
#define __MCG_H__
/********************************************************************/

/* For some reason CW needs to have cw.h explicitly included here for
 * the code relocation of set_sys_dividers() to work correctly even
 * though common.h should pull in cw.h.
 */
#if (defined(CW))
	#include "cw.h"
#endif

unsigned char pll_init(unsigned char, unsigned char);
unsigned char fll_rtc_init(unsigned char, unsigned char);

void mcg_pee_2_blpi(void);
void mcg_blpi_2_pee(void);
void mcg_pbe_2_pee(void);
// prototypes
void rtc_as_refclk(void);
void fee_fei(void);
int fei_fbe(unsigned char crystal_val, unsigned char hgo_val);
void fbe_pbe(unsigned char prdiv_val, unsigned char vdiv_val);
int pbe_pee(unsigned char crystal_val);
int pee_pbe(unsigned char crystal_val);
void pbe_fbe(void);
void fbe_fbi_fast(void);
void fbe_fbi_slow(void);
void fbi_blpi(void);

int fll_freq(int fll_ref);
int atc (unsigned char irc, int irc_freq);

#if (defined(IAR))
	__ramfunc void set_sys_dividers(uint32 outdiv1, uint32 outdiv2, uint32 outdiv3, uint32 outdiv4);
#elif (defined(CW))
	__relocate_code__ 
	void set_sys_dividers(uint32 outdiv1, uint32 outdiv2, uint32 outdiv3, uint32 outdiv4);
#endif	

enum clk_option
{
  PLL50,
  PLL100,
  PLL96,
  PLL48
};

enum crystal_val
{
  XTAL2,
  XTAL4,
  XTAL6,
  XTAL8,
  XTAL10,
  XTAL12,
  XTAL14,
  XTAL16,
  XTAL18,
  XTAL20,
  XTAL22,
  XTAL24,
  XTAL26,
  XTAL28,
  XTAL30,
  XTAL32,
  CLK50 = 24 // special case for external 50 MHz canned osc
};

/********************************************************************/
#endif /* __MCG_H__ */
