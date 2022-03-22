#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <ff.h>
#include <string.h>

SerialDriver *uart3 = &SD3;
static BaseSequentialStream *uart3_stream = NULL;

static const SerialConfig uart_conf = {
  .speed = 115200,
  .cr1 = 0,
  .cr2 = 0,
  .cr3 = 0
};

void Uart_Init(void)
{
  // запускаем драйвер в работу
  sdStart(uart3, &uart_conf);
  // Переводим ноги в состояние Rx, Tx
  palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );
  palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );
  // Переопределяем указатель на поток
  uart3_stream = (BaseSequentialStream *)uart3;
}


// Функция отправки строки в терминал
void dbgprintf( const char* format, ... )
{
// Проверяем, что debug_stream_init() случился
    if ( !uart3_stream )
    return;

// Отправляем в chvprintf() данные и ждём чуда
    va_list ap;
    va_start(ap, format);
    chvprintf(uart3_stream, format, ap);
    va_end(ap);
}


MMCDriver MMCD1;

static SPIConfig lowspeed = {
    .circular = false,
    .end_cb = NULL,
    .ssport = GPIOD,
    .sspad = 14,
    .cr1 = SPI_CR1_BR_1 | SPI_CR1_BR_0,
    .cr2 = 0
};

static SPIConfig highspeed = {
    .circular = false,
    .end_cb = NULL,
    .ssport = GPIOD,
    .sspad = 14,
    .cr1 = SPI_CR1_BR_1 | SPI_CR1_BR_0,
    .cr2 = 0
};

static MMCConfig sdconf = {
    .spip = &SPID1,
    .lscfg = &lowspeed,
    .hscfg = &highspeed
};

FATFS   MMC_FS;
FIL     fd;
bool    isFileOpened    = false;
char fbuf[1000]      = {0};



int blackBoxListFiles( char* path )
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;

    res = f_opendir(&dir, path);
    if (res == FR_OK)
    {
        i = strlen(path);
        for (;;)
        {
            res = f_readdir(&dir, &fno);

            if (res != FR_OK || fno.fname[0] == 0)
                break;

            if (fno.fname[0] == '.')
                continue;

            fn = fno.fname;

            if (fno.fattrib & AM_DIR)
            {
                path[i++] = '/';
                strcpy( &path[i], fn );
                res = blackBoxListFiles(path);
                if (res != FR_OK)
                  break;
                path[--i] = 0;
            }
            else
            {
              dbgprintf("%s /%s\n\r", path, fn);
            }
        }
    }

    return res;
}

uint8_t read_bytes[255] = {0};
UINT position = 10;

int main(void) {
    halInit();
    chSysInit();
    Uart_Init();
    palSetPadMode(GPIOA, 5, PAL_MODE_ALTERNATE(5));
    palSetPadMode(GPIOA, 6, PAL_MODE_ALTERNATE(5));
    palSetPadMode(GPIOA, 7, PAL_MODE_ALTERNATE(5));
    palSetPadMode(GPIOD, 14, PAL_MODE_OUTPUT_PUSHPULL);
    mmcObjectInit(&MMCD1);
    mmcStart(&MMCD1, &sdconf);
    if(mmcConnect(&MMCD1) == HAL_SUCCESS) palSetLine(LINE_LED2);
    chThdSleepSeconds(1);
    f_mount(&MMC_FS, "/", 1);

    blackBoxListFiles(fbuf);
    f_open(&fd, "/test1.txt", FA_READ);
    f_read(&fd, read_bytes, 10, &position);
    dbgprintf("%s", read_bytes);

    while (true) {
        chThdSleepS(1);
        if(mmcIsCardInserted(&MMCD1)) palSetLine(LINE_LED1);
        else palClearLine(LINE_LED1);


    }
}
