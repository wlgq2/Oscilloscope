/*
 * File:    mcg.c
 * Purpose: Driver for enabling the PLL in 1 of 4 options
 *
 * Notes:
 * Assumes the MCG mode is in the default FEI mode out of reset
 * One of 4 clocking oprions can be selected.
 * One of 16 crystal values can be used
 */

#include "common.h"
#include "mcg.h"
//#include "lptmr.h"

// global variables

extern int slow_irc_freq = 32768; // default slow irc frequency is 32768Hz
extern int fast_irc_freq = 4000000; // default fast irc frequency is 4MHz

extern int core_clk_khz;
extern int core_clk_mhz;
extern int periph_clk_khz;
extern char drs_val, dmx32_val;

unsigned char fll_rtc_init(unsigned char clk_option, unsigned char crystal_val)
{
  unsigned char pll_freq;

  rtc_as_refclk();
  pll_freq = 24;
  return pll_freq;
}

unsigned char pll_init(unsigned char clk_option, unsigned char crystal_val)
{
  unsigned char pll_freq;

  if (clk_option > 3) {return 0;} //return 0 if one of the available options is not selected
  if (crystal_val > 15) {return 1;} // return 1 if one of the available crystal options is not available
//This assumes that the MCG is in default FEI mode out of reset.

// First move to FBE mode
#if (defined(K60_CLK) || defined(ASB817)||defined(K53_CLK))
     MCG_C2 = 0;
#else
// Enable external oscillator, RANGE=2, HGO=1, EREFS=1, LP=0, IRCS=0
    MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
#endif

// after initialization of oscillator release latched state of oscillator and GPIO
    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
    LLWU_CS |= LLWU_CS_ACKISO_MASK;
  
// Select external oscilator and Reference Divider and clear IREFS to start ext osc
// CLKS=2, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
  MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);

  /* if we aren't using an osc input we don't need to wait for the osc to init */
#if (!defined(K60_CLK) && !defined(ASB817)&& !defined(K53_CLK))
    while (!(MCG_S & MCG_S_OSCINIT_MASK)){};  // wait for oscillator to initialize
#endif

  while (MCG_S & MCG_S_IREFST_MASK){}; // wait for Reference clock Status bit to clear

  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}; // Wait for clock status bits to show clock source is ext ref clk

// Now in FBE

#if (defined(K60_CLK)||defined(K53_CLK))
   MCG_C5 = MCG_C5_PRDIV(0x18);
#else
// Configure PLL Ref Divider, PLLCLKEN=0, PLLSTEN=0, PRDIV=5
// The crystal frequency is used to select the PRDIV value. Only even frequency crystals are supported
// that will produce a 2MHz reference clock to the PLL.
  MCG_C5 = MCG_C5_PRDIV(crystal_val); // Set PLL ref divider to match the crystal used
#endif

  // Ensure MCG_C6 is at the reset default of 0. LOLIE disabled, PLL disabled, clk monitor disabled, PLL VCO divider is clear
  MCG_C6 = 0x0;
// Select the PLL VCO divider and system clock dividers depending on clocking option
  switch (clk_option) {
    case 0:
      // Set system options dividers
      //MCG=PLL, core = MCG, bus = MCG, FlexBus = MCG, Flash clock= MCG/2
      set_sys_dividers(0,0,0,1);
      // Set the VCO divider and enable the PLL for 50MHz, LOLIE=0, PLLS=1, CME=0, VDIV=1
      MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(1); //VDIV = 1 (x25)
      pll_freq = 50;
      break;
   case 1:
      // Set system options dividers
      //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4
     set_sys_dividers(0,1,1,3);
      // Set the VCO divider and enable the PLL for 100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
      MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(26); //VDIV = 26 (x50)
      pll_freq = 100;
      break;
    case 2:
      // Set system options dividers
      //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4
      set_sys_dividers(0,1,1,3);
      // Set the VCO divider and enable the PLL for 96MHz, LOLIE=0, PLLS=1, CME=0, VDIV=24
      MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(24); //VDIV = 24 (x48)
      pll_freq = 96;
      break;
   case 3:
      // Set system options dividers
      //MCG=PLL, core = MCG, bus = MCG, FlexBus = MCG, Flash clock= MCG/2
      set_sys_dividers(0,0,0,1);
      // Set the VCO divider and enable the PLL for 48MHz, LOLIE=0, PLLS=1, CME=0, VDIV=0
      MCG_C6 = MCG_C6_PLLS_MASK; //VDIV = 0 (x24)
      pll_freq = 48;
      break;
  }
  while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set

  while (!(MCG_S & MCG_S_LOCK_MASK)){}; // Wait for LOCK bit to set

// Now running PBE Mode

// Transition into PEE by setting CLKS to 0
// CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
  MCG_C1 &= ~MCG_C1_CLKS_MASK;

// Wait for clock status bits to update
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};

// Now running PEE Mode

return pll_freq;
} //pll_init


 /*
  * This routine must be placed in RAM. It is a workaround for errata e2448.
  * Flash prefetch must be disabled when the flash clock divider is changed.
  * This cannot be performed while executing out of flash.
  * There must be a short delay after the clock dividers are changed before prefetch
  * can be re-enabled.
  */
#if (defined(IAR))
	__ramfunc void set_sys_dividers(uint32 outdiv1, uint32 outdiv2, uint32 outdiv3, uint32 outdiv4)
#elif (defined(CW))
__relocate_code__ 
void set_sys_dividers(uint32 outdiv1, uint32 outdiv2, uint32 outdiv3, uint32 outdiv4)
#endif
{
  uint32 temp_reg;
  uint8 i;
  
  temp_reg = FMC_PFAPR; // store present value of FMC_PFAPR
  
  // set M0PFD through M7PFD to 1 to disable prefetch
  FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
             | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
             | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;
  
  // set clock dividers to desired value  
  SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(outdiv1) | SIM_CLKDIV1_OUTDIV2(outdiv2) 
              | SIM_CLKDIV1_OUTDIV3(outdiv3) | SIM_CLKDIV1_OUTDIV4(outdiv4);

  // wait for dividers to change
  for (i = 0 ; i < outdiv4 ; i++)
  {}
  
  FMC_PFAPR = temp_reg; // re-store original value of FMC_PFAPR
  
  return;
} // set_sys_dividers


/********************************************************************/
void mcg_pee_2_blpi(void)
{
    uint8 temp_reg;
    // Transition from PEE to BLPI: PEE -> PBE -> FBE -> FBI -> BLPI
  
    // Step 1: PEE -> PBE
    MCG_C1 |= MCG_C1_CLKS(2);  // System clock from external reference OSC, not PLL.
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){};  // Wait for clock status to update.
    
    // Step 2: PBE -> FBE
    MCG_C6 &= ~MCG_C6_PLLS_MASK;  // Clear PLLS to select FLL, still running system from ext OSC.
    while (MCG_S & MCG_S_PLLST_MASK){};  // Wait for PLL status flag to reflect FLL selected.
    
    // Step 3: FBE -> FBI
    MCG_C2 &= ~MCG_C2_LP_MASK;  // FLL remains active in bypassed modes.
    MCG_C2 |= MCG_C2_IRCS_MASK;  // Select fast (1MHz) internal reference
    temp_reg = MCG_C1;
    temp_reg &= ~(MCG_C1_CLKS_MASK | MCG_C1_IREFS_MASK);
    temp_reg |= (MCG_C1_CLKS(1) | MCG_C1_IREFS_MASK);  // Select internal reference (fast IREF clock @ 1MHz) as MCG clock source.
    MCG_C1 = temp_reg;
  
    while (MCG_S & MCG_S_IREFST_MASK){};  // Wait for Reference Status bit to update.
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x1){};  // Wait for clock status bits to update
    
    // Step 4: FBI -> BLPI
    MCG_C1 |= MCG_C1_IREFSTEN_MASK;  // Keep internal reference clock running in STOP modes.
    MCG_C2 |= MCG_C2_LP_MASK;  // FLL remains disabled in bypassed modes.
    while (!(MCG_S & MCG_S_IREFST_MASK)){};  // Wait for Reference Status bit to update.
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x1){};  // Wait for clock status bits to update.
  
} // end MCG PEE to BLPI
/********************************************************************/
void mcg_blpi_2_pee(void)
{
    uint8 temp_reg;
    // Transition from BLPI to PEE: BLPI -> FBI -> FEI -> FBE -> PBE -> PEE
  
    // Step 1: BLPI -> FBI
    MCG_C2 &= ~MCG_C2_LP_MASK;  // FLL remains active in bypassed modes.
    while (!(MCG_S & MCG_S_IREFST_MASK)){};  // Wait for Reference Status bit to update.
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x1){};  // Wait for clock status bits to update
    
    // Step 2: FBI -> FEI
    MCG_C2 &= ~MCG_C2_LP_MASK;  // FLL remains active in bypassed modes.
    temp_reg = MCG_C2;  // assign temporary variable of MCG_C2 contents
    temp_reg &= ~MCG_C2_RANGE_MASK;  // set RANGE field location to zero
    temp_reg |= (0x2 << 0x4);  // OR in new values
    MCG_C2 = temp_reg;  // store new value in MCG_C2
    MCG_C4 = 0x0E;  // Low-range DCO output (~10MHz bus).  FCTRIM=%0111.
    MCG_C1 = 0x04;  // Select internal clock as MCG source, FRDIV=%000, internal reference selected.
 
    while (!(MCG_S & MCG_S_IREFST_MASK)){};   // Wait for Reference Status bit to update 
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x0){}; // Wait for clock status bits to update
    
    // Handle FEI to PEE transitions using standard clock initialization routine.
    core_clk_mhz = pll_init(CORE_CLK_MHZ, REF_CLK); 

    /* Use the value obtained from the pll_init function to define variables
    * for the core clock in kHz and also the peripheral clock. These
    * variables can be used by other functions that need awareness of the
    * system frequency.
    */
    core_clk_khz = core_clk_mhz * 1000;
    periph_clk_khz = core_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);        
} // end MCG BLPI to PEE
/********************************************************************/

void mcg_pbe_2_pee(void)
{  
  MCG_C1 &= ~MCG_C1_CLKS_MASK; // select PLL as MCG_OUT
  // Wait for clock status bits to update 
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){}; 

  switch (CORE_CLK_MHZ) {
    case PLL50:
      core_clk_khz = 50000;
      break;
    case PLL100:
      core_clk_khz = 100000;
      break;
    case PLL96:
      core_clk_khz = 96000;
      break;  
    case PLL48:
      core_clk_khz = 48000;
      break;  
  }
}
void rtc_as_refclk(void)
{
  unsigned char temp_reg;
  
// Using the RTC OSC as FLL Ref Clk
// enable RTC clock gating
  SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;  
// set RTC in default state using software reset
  RTC_CR |= RTC_CR_SWR_MASK; // set SWR
  RTC_CR &= ~RTC_CR_SWR_MASK; // clear SWR
// Configure and enable the RTC OSC
// select the load caps (application dependent) and the oscillator enable bit
// note that other bits in this register may need to be set depending on the intended use of the RTC
  
  RTC_CR |= RTC_CR_OSCE_MASK;

  time_delay_ms(1000); // wait for the RTC oscillator to intialize
// select the RTC oscillator as the MCG reference clock
  SIM_SOPT2 |= SIM_SOPT2_MCGCLKSEL_MASK;
  
// ensure MCG_C2 is in the reset state, key items are RANGE = 0 to select the correct FRDIV factor
// and LP = 0 to keep FLL enabled. HGO and EREFS do not affect RTC oscillator  
  MCG_C2 = 0x0;
  
// Select the Reference Divider and clear IREFS to select the osc
// CLKS=0, select the FLL as the clock source for MCGOUTCLK
// FRDIV=0, set the FLL ref divider to divide by 1 
// IREFS=0, select the external clock 
// IRCLKEN=0, disable IRCLK (can enable if desired)
// IREFSTEN=0, disable IRC in stop mode (can keep it enabled in stop if desired)
  MCG_C1 = 0x0;

  while (MCG_S & MCG_S_IREFST_MASK){}; // wait for Reference clock to switch to external reference
    
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x0){}; // Wait for clock status bits to update
  
// Can select the FLL operating range/freq by means of the DRS and DMX32 bits
// Must first ensure the system clock dividers are set to keep the core and bus clocks
// within spec.
//  SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(0) 
//              | SIM_CLKDIV1_OUTDIV3(0) | SIM_CLKDIV1_OUTDIV4(1);

  temp_reg = MCG_C4;
  temp_reg &= ~(MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK); // clear the DMX32 bit and DRS field
  temp_reg |= (MCG_C4_DRST_DRS(drs_val) | (dmx32_val << MCG_C4_DMX32_SHIFT)); // select DRS range and dmx32 setting
  MCG_C4 = temp_reg;
  
// should enable clock monitor now that external reference is being used
//  MCG_C6 |= MCG_C6_CME_MASK;
  
  core_clk_khz = fll_freq(32768); // calculate core clock based on 32768 crystal reference
  periph_clk_khz = core_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);
} //end cmd_rtc_as_refclk


void fee_fei(void)
{
  unsigned char temp_reg;
  
  // first ensure clock monitor is disabled otherwise a loss of clock reset will occur
  MCG_C6 &= ~MCG_C6_CME_MASK;
  
  MCG_C1 |= MCG_C1_IREFS_MASK; // select internal reference
  
  // wait for Reference clock to switch to internal reference 
  while (!(MCG_S & MCG_S_IREFST_MASK)){}
  
  // Select the system oscillator as the MCG reference clock.
  // Not typically requred to set this as default is system oscillator.
  // This is not required as part of moving to FEI but is performed here to ensure the system
  // oscillator is used in future mode changes.
  SIM_SOPT2 &= ~SIM_SOPT2_MCGCLKSEL_MASK;
  
  temp_reg = MCG_C4;
  temp_reg &= ~(MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK); // clear the DMX32 bit and DRS field
  temp_reg |= (MCG_C4_DRST_DRS(drs_val) | (dmx32_val << MCG_C4_DMX32_SHIFT)); // select DRS range and dmx32 setting
  MCG_C4 = temp_reg;
  
  core_clk_khz = fll_freq(slow_irc_freq);
  periph_clk_khz = core_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);
    
} // fee_fei


int fll_freq(int fll_ref)
{
  int fll_freq_khz;
  
  if (MCG_C4 & MCG_C4_DMX32_MASK) // if DMX32 set
  {
    switch ((MCG_C4 & MCG_C4_DRST_DRS_MASK) >> MCG_C4_DRST_DRS_SHIFT) // determine multiplier based on DRS
    {
    case 0:
      fll_freq_khz = ((fll_ref * 732) / 1000);
      break;
    case 1:
      fll_freq_khz = ((fll_ref * 1464) / 1000);
      break;
    case 2:
      fll_freq_khz = ((fll_ref * 2197) / 1000);
      break;
    case 3:
      fll_freq_khz = ((fll_ref * 2929) / 1000);
      break;
    }
  }
  else // if DMX32 = 0
  {
    switch ((MCG_C4 & MCG_C4_DRST_DRS_MASK) >> MCG_C4_DRST_DRS_SHIFT) // determine multiplier based on DRS
    {
    case 0:
      fll_freq_khz = ((fll_ref * 640) / 1000);
      break;
    case 1:
      fll_freq_khz = ((fll_ref * 1280) / 1000);
      break;
    case 2:
      fll_freq_khz = ((fll_ref * 1920) / 1000);
      break;
    case 3:
      fll_freq_khz = ((fll_ref * 2560) / 1000);
      break;
    }
  }    
  return fll_freq_khz;
} // fll_freq

int fei_fbe(unsigned char crystal_val, unsigned char hgo_val)
{
  unsigned char frdiv_val;
  int i;
  
  if ((crystal_val > 15) && (crystal_val != 24)) {return 1;} // return 1 if one of the available crystal options is not available
  
  if (hgo_val > 0)
  {
    hgo_val = 1; // force hgo_val to 1 if > 0
  }

#if (defined(K60_CLK))
     MCG_C2 = MCG_C2_RANGE(1); // select external clock and set range to provide correct FRDIV range
#else
// Enable external oscillator, select range based on crystal frequency
  if (crystal_val < 4)
  {
    MCG_C2 = MCG_C2_RANGE(1) | (MCG_C2_HGO_MASK << MCG_C2_HGO_SHIFT) | MCG_C2_EREFS_MASK;
  }
  else
  {
    MCG_C2 = MCG_C2_RANGE(2) | (MCG_C2_HGO_MASK << MCG_C2_HGO_SHIFT) | MCG_C2_EREFS_MASK;
  }
#endif

  switch (crystal_val) // determine FRDIV based on reference clock frequency
    {
    case 0:
      frdiv_val = 1;
      break;
    case 1:
      frdiv_val = 2;
      break;
    case 2:
    case 3:
    case 4:
      frdiv_val = 3;
      break;
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      frdiv_val = 4;
      break;
    default:
      frdiv_val = 5;        
      break;
    }
// Select external oscilator and Reference Divider and clear IREFS to start ext osc
// CLKS=2, FRDIV=frdiv_val, IREFS=0, IRCLKEN=0, IREFSTEN=0
  MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(frdiv_val);

  /* if we aren't using an osc input we don't need to wait for the osc to init */
#if (!defined(K60_CLK))
//  while (!(MCG_S & MCG_S_OSCINIT_MASK)){};  // wait for oscillator to initialize
  for (i = 0 ; i < 10000 ; i++)
  {
    if (MCG_S & MCG_S_OSCINIT_MASK) break; // jump out early if OSCINIT sets before loop finishes
  }
  if (!(MCG_S & MCG_S_OSCINIT_MASK)) return 2; // check bit is really set and return with error if not set
#endif

//  while (MCG_S & MCG_S_IREFST_MASK){}; // wait for Reference clock Status bit to clear
  for (i = 0 ; i < 2000 ; i++)
  {
    if (!(MCG_S & MCG_S_IREFST_MASK)) break; // jump out early if IREFST clears before loop finishes
  }
  if (MCG_S & MCG_S_IREFST_MASK) return 3; // check bit is really clear and return with error if not set
  
//  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}; // Wait for clock status bits to show clock source is ext ref clk
  for (i = 0 ; i < 2000 ; i++)
  {
    if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x2) break; // jump out early if CLKST shows EXT CLK slected before loop finishes
  }
  if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2) return 4; // check EXT CLK is really selected and return with error if not
  
  return ((crystal_val + 1) * 2 * 1000); // MCGOUT frequency in kHz = crystal value * 2 * 1000

// Now in FBE
}

void fbe_pbe(unsigned char prdiv_val, unsigned char vdiv_val)
{  
  MCG_C5 = MCG_C5_PRDIV(prdiv_val);    //set PLL ref divider

// the PLLS bit is set to enable the PLL, MCGOUT still sourced from ext ref clk  
  MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(24);
  
  while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set

  while (!(MCG_S & MCG_S_LOCK_MASK)){}; // Wait for LOCK bit to set
// now in PBE 
// PBE frequency = FBE frequency, no need to change frequency
}

int pbe_pee(unsigned char crystal_val)
{
  unsigned char prdiv, vdiv;  
  MCG_C1 &= ~MCG_C1_CLKS_MASK; // switch CLKS mux to select PLL as MCG_OUT
  // Wait for clock status bits to update 
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){} 

  prdiv = ((MCG_C5 & MCG_C5_PRDIV_MASK) + 1);
  vdiv = ((MCG_C6 & MCG_C6_VDIV_MASK) + 24);
  
  return (((((crystal_val + 1) * 2)/prdiv) * vdiv) * 1000); //calculate PLL output frequency
}

int pee_pbe(unsigned char crystal_val)
{  
  MCG_C1 |= MCG_C1_CLKS(2); // switch CLKS mux to select external reference clock as MCG_OUT
  // Wait for clock status bits to update 
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}; 

return ((crystal_val + 1) * 2000); // MCGOUT frequency in kHz

} // pee_pbe, freq = REF_CLK

void pbe_fbe(void)
{
  MCG_C6 &= ~MCG_C6_PLLS_MASK; // clear PLLs to disable PLL, still clocked from ext ref clk
  
  while (MCG_S & MCG_S_PLLST_MASK){}; // wait for PLLS status bit to set
// FBE frequency = PBE frequency, no need to change frequency  
} 

void fbe_fbi_fast(void)
{
  unsigned char temp_reg;
  
  MCG_C2 |= MCG_C2_IRCS_MASK; // select fast IRC by setting IRCS
  
  temp_reg = MCG_C1;
  temp_reg &= ~MCG_C1_CLKS_MASK;
  temp_reg |= MCG_C1_CLKS(1); // select IRC as MCGOUT
  MCG_C1 = temp_reg; // update MCG_C1
  
  while (!(MCG_S & MCG_S_IRCST_MASK)){}; // wait until internal reference switches to fast clock.
  
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x1){}; // Wait for clock status bits to update
// fast IRC clock speed is determined by FCTRIM value and must be trimmed to a known frequency
} //fbe_fbi_fast

void fbe_fbi_slow(void)
{
  unsigned char temp_reg;
  
  MCG_C2 &= ~MCG_C2_IRCS_MASK; // select slow IRC by clearing IRCS
  
  temp_reg = MCG_C1;
  temp_reg &= ~MCG_C1_CLKS_MASK;
  temp_reg |= MCG_C1_CLKS(1); // select IRC as MCGOUT
  MCG_C1 = temp_reg; // update MCG_C1
  
  while (!(MCG_S & MCG_S_IRCST_MASK)){}; // wait until internal reference switches to fast clock.
  
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x1){}; // Wait for clock status bits to update
// fast IRC clock speed is determined by FCTRIM value and must be trimmed to a known frequency
} //fbe_fbi_slow

void fbi_blpi(void)
{ 
  MCG_C2 |= MCG_C2_LP_MASK; //set LP bit to disable the FLL 
// no change in MCGOUT frequency  
} 


int atc (unsigned char irc, int irc_freq)
{
    uint32 temp_reg = 0;
    unsigned char temp_reg8 = 0;
    uint32 orig_SIM_CLKDIV1;
    unsigned short atcv;
    unsigned char clks_val;
    unsigned int i;
             
    printf("\n-------------------------------------------\n");
    printf("  MCG AutoTrim Utility\n");
    printf("-------------------------------------------\n");
    printf("\n");
    {
        if (irc > 0) // force irc to 1 if greater than 0
        {
          irc = 1;
        }
        
        clks_val = ((MCG_C1 & MCG_C1_CLKS_MASK) >> 6);
        
        if (((clks_val == 1) || (clks_val ==3)))
        {
          printf("\nAuto trim error - cannot use the internal clock source.\r\n\n");
          return 1; //error using IRC as system clock
        }
              
        if ((clks_val == 0) && (!(MCG_C6 & MCG_C6_PLLS_MASK)) && (MCG_C1 & MCG_C1_IREFS_MASK))
        {
          printf("\nAuto trim error - cannot use the FLL with internal clock source.\r\n\n");
          return 2; // error using FLL with IRC
        }
        if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 3)
        {
          printf("\nWARNING - PLL is not the internal clock source. Auto trim value will not be correct\r\n\n");
        }
                
        if(!irc) //determine if slow or fast IRC to be trimmed
        {
          if (irc_freq < 31250) // check frequency is above min spec.
          {
            printf("\nAuto trim error - target frequency is below 31250 Hz.\r\n\n");
            return 3;
          }
          if (irc_freq > 39062) // check frequency is below max spec.
          {
            printf("\nAuto trim error - target frequency is above 39062 Hz.\r\n\n");
            return 4;
          }         
        }
        else
        {
          if (irc_freq < 3000000) // check frequency is above min spec.
          {
            printf("\nAuto trim error - target frequency is below 3 MHz.\r\n\n");
            return 5;
          }
          if (irc_freq > 5000000) // check frequency is below max spec.
          {
            printf("\nAuto trim error - target frequency is above 5 MHz.\r\n\n");
            return 6;
          }            
        } // if
        for (i=0;i<0xffff;i++);
        orig_SIM_CLKDIV1 = SIM_CLKDIV1;        // backup current divider value
        temp_reg = SIM_CLKDIV1;                // bus clock needs to be between 8 and 16 MHz
        temp_reg &= ~SIM_CLKDIV1_OUTDIV2_MASK; // clear bus divider field
        temp_reg |= SIM_CLKDIV1_OUTDIV2(5);    //divide 96MHz PLL by 6 = 16MHz bus clock
        SIM_CLKDIV1 = temp_reg; 
// Set up autocal registers, must use floating point calculation
        if (irc)
          atcv = (unsigned short)(128.0f * (21.0f * (16000000.0f / (float)irc_freq))); 
        else
          atcv = (unsigned short)(21.0f * (16000000.0f / (float)irc_freq));
        
        MCG_ATCVL = (atcv & 0xFF); //Set ATCVL to lower 8 bits of count value
        MCG_ATCVH = ((atcv & 0xFF00) >> 8); // Set ATCVH to upper 8 bits of count value

// Enable autocal
        MCG_ATC = 0x0; // clear auto trim control register
        temp_reg8 |= (MCG_ATC_ATME_MASK | (irc << MCG_ATC_ATMS_SHIFT)); //Select IRC to trim and enable trim machine
        MCG_ATC = temp_reg8;
        
        while (MCG_ATC & MCG_ATC_ATME_MASK) {}; //poll for ATME bit to clear
        
        SIM_CLKDIV1 = orig_SIM_CLKDIV1; //restore the divider value
        
        if (MCG_ATC & MCG_ATC_ATMF_MASK) // check if error flag set
        {
          printf("Autotrim error.\r\n\n");
          printf("\n");
          printf("MCG_C1   = %#02X \r\n", (MCG_C1));
          printf("MCG_C2   = %#02X \r\n", (MCG_C2));
          printf("MCG_C3   = %#02X \r\n", (MCG_C3));
          printf("MCG_C4   = %#02X \r\n", (MCG_C4)) ;
          printf("MCG_C5   = %#02X \r\n", (MCG_C5));
          printf("MCG_C6   = %#02X \r\n\n", (MCG_C6));
          printf("MCG_S    = %#02X \r\n\n", (MCG_S)) ;
          printf("MCG_ATC   = %#02X \r\n",   (MCG_ATC)) ;
          printf("MCG_ATCVL = %#02X \r\n",   (MCG_ATCVL)) ;
          printf("MCG_ATVCH = %#02X \r\n",   (MCG_ATCVH));
          MCG_ATC |= MCG_ATC_ATMF_MASK; // clear fail flag
          return 7;
        } 
        else 
        {      
          printf("Autotrim Passed.\r\n\n");
          printf("MCG_C3   = %#02X \r\n", (MCG_C3));
          printf("MCG_C4   = %#02X \r\n", (MCG_C4));
          // Check trim value is not at either extreme of the range
          if (!irc)
          {
            if ((MCG_C3 == 0xFF) || (MCG_C3 == 0))
            {
              printf("\nAutotrim result is not valid.\r\n\n");
              return 8;
            }
          }
          else
          {
            if ((((MCG_C4 & MCG_C4_FCTRIM_MASK) >> MCG_C4_FCTRIM_SHIFT) == 0xF) ||
              (((MCG_C4 & MCG_C4_FCTRIM_MASK) >> MCG_C4_FCTRIM_SHIFT) == 0))
            {
              printf("\nAutotrim result is not valid.\r\n\n");
              return 8;
            }
          }
        }
          
    }// end else
    return irc;
}// end atc