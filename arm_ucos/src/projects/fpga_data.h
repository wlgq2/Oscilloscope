#ifndef FPGA_DATA_H

#define FPGA_DATA_H

#include "common_header.h"
#include "character.h"
#include "task_header.h"
#include "bsp.h"

void write_command(uint8 data);
void write_data(uint8 data);
void dds_data(uint8 dds_num);
void Set_fre_div(uint8 fre_div);
void show_para(uint8 * ParaData);

#endif

