#ifndef _APP_H_
#define _APP_H_




/**************** 用户任务声明 *******************/
void key_task(void *);

void system_init_task(void *);
void uart4_task(void *);
void menu_task(void *);
void data_process_task(void *);

void dds_ctrl_task(void *pdata);
void FirPara_task(void *pdata);

void Paradata_task(void *pdata);
#endif    //_APP_H_
