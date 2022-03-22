#include "ch.h"
#include "hal.h"

static const SerialConfig usb_conf = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

static SerialDriver         *serial = &SD3;

void control_led(uint16_t mode)
{
    switch(mode){
    case 0:
        palSetLine(LINE_LED1);
        palClearLine(LINE_LED2);
        palClearLine(LINE_LED3);
        break;
    case 1:
        palSetLine(LINE_LED2);
        palClearLine(LINE_LED1);
        palClearLine(LINE_LED3);
        break;
    case 2:
        palSetLine(LINE_LED3);
        palClearLine(LINE_LED1);
        palClearLine(LINE_LED2);
        break;
    }
}

uint16_t buf[1] = {0};

int main(void) {
    halInit();
    chSysInit();

    sdStart(serial, &usb_conf);
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );

    while (1) {
        sdRead(serial, (uint8_t *)buf, 2);
        control_led(buf[0] % 3);

    }
}
