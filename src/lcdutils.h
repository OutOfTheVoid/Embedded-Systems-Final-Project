#ifndef LCDUTILS_H
#define LCDUTILS_H

#include <stdint.h>
#include <stdbool.h>

#include "SSD2119.h"
#include "loop_delay.h"

static inline bool touch_is_touching() {
	return (Touch_ReadZ2() - Touch_ReadZ1()) < 1500;
}

typedef struct {
	float 
	x_c, y_c,
	x_u_r, y_u_r,
	x_l_r, y_l_r,
	x_u_l, y_u_l,
	x_l_l, y_l_l;
} TouchCalibration;

static inline void transform_touch(TouchCalibration * cal, float * x, float * y) {
	bool x_flipped = (cal -> x_u_l > cal -> x_u_r);
	bool y_flipped = (cal -> y_u_l > cal -> y_l_l);
	uint8_t quadrant = ((* x > cal -> x_c) ? 1 : 0) | ((* y > cal -> y_c) ? 2 : 0);
	if (x_flipped)
		quadrant ^= 1;
	if (y_flipped)
		quadrant ^= 2;
	
	float p1_x_real;
	float p1_y_real;
	float p2_x_real = 160.0f;
	float p2_y_real = 120.0f;
	
	float p1_x_cal;
	float p1_y_cal;
	float p2_x_cal = cal -> x_c;
	float p2_y_cal = cal -> y_c;
	
	switch (quadrant)
	{
		case 0: // upper left quadrant
			p1_x_real = 30.0f;
			p1_y_real = 30.0f;
			p1_x_cal = cal -> x_u_l;
			p1_y_cal = cal -> y_u_l;
			break;
		case 1: // upper right quadrant
			p1_x_real = 290.0f;
			p1_y_real = 30.0f;
			p1_x_cal = cal -> x_u_r;
			p1_y_cal = cal -> y_u_r;
			break;
		case 2: // lower left quadrant
			p1_x_real = 30.0f;
			p1_y_real = 210.0f;
			p1_x_cal = cal -> x_l_l;
			p1_y_cal = cal -> y_l_l;
			break;
		case 3: // lower right quadrant
			p1_x_real = 290.0f;
			p1_y_real = 210.0f;
			p1_x_cal = cal -> x_l_r;
			p1_y_cal = cal -> y_l_r;
			break;
	}
	
	* x = p2_x_real + ((* x - p2_x_cal) / (p1_x_cal - p2_x_cal)) * (p1_x_real - p2_x_real);
	* y = p2_y_real + ((* y - p2_y_cal) / (p1_y_cal - p2_y_cal)) * (p1_y_real - p2_y_real);
}

void await_calibration_touch(int x, int y, float * x_p, float * y_p) {
	LCD_ColorFill(0);
	LCD_SetCursor(100, 100);
	LCD_PrintString("Touch Calibration");
	LCD_SetCursor(100, 108);
	LCD_PrintString("Please touch the cross");
	LCD_DrawLine(x - 10, y, x + 10, y, 0xFFFF);
	LCD_DrawLine(x, y - 10, x, y + 10, 0xFFFF);
	* x_p = 0.0f;
	* y_p = 0.0f;
	while (!touch_is_touching());
	for (int i = 0; i < 20; i ++) {
		* x_p += (float) Touch_ReadX();
		* y_p += (float) Touch_ReadY();
	}
	* x_p *= 0.05f;
	* y_p *= 0.05f;
	loop_delay(10000);
	while (touch_is_touching());
}

static inline void calibrate_touch(TouchCalibration * cal) {
	await_calibration_touch(160, 120, & cal -> x_c, & cal -> y_c);
	await_calibration_touch(30, 30, & cal -> x_u_l, & cal -> y_u_l);
	await_calibration_touch(290, 30, & cal -> x_u_r, & cal -> y_u_r);
	await_calibration_touch(30, 210, & cal -> x_l_l, & cal -> y_l_l);
	await_calibration_touch(290, 210, & cal -> x_l_r, & cal -> y_l_r);
	LCD_ColorFill(0);
}

static inline void touch_get_position(float * x, float * y, TouchCalibration * cal) {
	float _x = 0.0f;
	float _y = 0.0f;
	for (int i = 0; i < 20; i ++) {
		_x += Touch_ReadX();
		_y += Touch_ReadY();
	}
	_x *= 0.05f;
	_y *= 0.05f;
	transform_touch(cal, & _x, & _y);
	* x = _x;
	* y = _y;
}

typedef struct {
	float x;
	float y;
	float width;
	float height;
	unsigned short border_color;
	unsigned short bg_color;
	uint32_t text_color;
	const char * text;
	bool triggered;
	bool visible;
	int text_off;
} LCDButton;

static inline void lcd_button_init(LCDButton * button, int x, int y, int width, int height, const char * text, int text_offset) {
	button -> x = x;
	button -> y = y;
	button -> width = width;
	button -> height = height;
	button -> text = text;
	button -> border_color = 0xFFFF;
	button -> bg_color = convertColor(0x80, 0x80, 0x40);
	button -> text_color = 0xFFFFFF;
	button -> visible = true;
	button -> text_off = text_offset;
	button -> triggered = false;
}

static inline void lcd_button_draw(LCDButton * button) {
	if (button -> visible) {
		LCD_DrawFilledRect(button -> x + 1, button -> y + 1, button -> width - 2, button -> height - 2, button -> bg_color);
		LCD_DrawRect(button -> x, button -> y, button -> width, button -> height, button -> border_color);
		LCD_SetCursor(button -> x + button -> width / 2 - button -> text_off, button -> y + button -> height / 2 - 4);
		LCD_SetTextColor(button -> text_color >> 16, button -> text_color >> 8, button -> text_color);
		LCD_PrintString((char *) button -> text);
	}
}

static inline bool lcd_button_update(LCDButton * button, float touch_x, float touch_y, bool touch_state) {
	bool over_button = (touch_x >= button -> x) && ((touch_x - button -> x) <= button -> width) && (touch_y >= button -> y) && ((touch_y - button -> y) <= button -> height);
	if (over_button && touch_state) {
		button -> triggered = true;
	} else if (button -> triggered) {
		button -> triggered = false;
		return true;
	}
	return false;
}

#endif
