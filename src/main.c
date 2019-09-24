#include "mylib.h"

#include "FreeRTOS.h"
#include "freertos_support.h"
#include "task.h"
#include "semphr.h"
#include "halt.h"

#include "keyboard_task.h"
#include "audio_task.h"
#include "serial_task.h"
#include "SSD2119.h"
#include "lcdutils.h"

void audio_feed_interrupt();

int main() {
	set_clock_16MHz();
	LCD_Init();
	Touch_Init();
	LCD_ColorFill(convertColor(0x99, 0x99, 0xFF));
	LCD_PrintString("touch the screen to continue");
	while (!touch_is_touching());
	LCD_ColorFill(convertColor(0x99, 0xFF, 0x99));
	
	set_clock_80MHz();
	freertos_setup();
	
	if (! serial_task_init()) {
		halt();
	}
	
	if (! keyboard_task_init()) {
		halt();
	}
	
	if (! audio_task_init()) {
		halt();
	}
	
	vTaskStartScheduler();
	while(1);
}
