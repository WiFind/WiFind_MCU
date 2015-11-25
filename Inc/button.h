#ifndef BUTTON_H
#define BUTTON_H

#include "cmsis_os.h"
#include "semphr.h"


extern xSemaphoreHandle raw_button_sem;
void button_thread(void const *);


#endif //BUTTON_H
