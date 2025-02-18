#ifndef _VS1053_H_
#define _VS1053_H_

#include <gpio.h>

class VS1053
{
  public:
    VS1053();
    ~VS1053();

    void set_volume(unsigned char volume);
    bool buffer_empty();

    void send_data(unsigned char *buffer);

  public:
    unsigned char spi_byte(unsigned char b);
    void delay_loops(uint32_t loops);

    void write_register(unsigned char address, uint16_t data);
    uint16_t read_register(unsigned char address);

  private:
    Gpio<TGPIOB, 5, GPIO_MODE_OUT> sck;
    Gpio<TGPIOB, 4, GPIO_MODE_IN_FLOATING> miso;
    Gpio<TGPIOB, 3, GPIO_MODE_OUT> mosi;

    Gpio<TGPIOB, 0, GPIO_MODE_OUT> x_reset;
    Gpio<TGPIOD, 7, GPIO_MODE_OUT> x_dcs;
    Gpio<TGPIOD, 6, GPIO_MODE_OUT> x_cs;

    Gpio<TGPIOD, 2, GPIO_MODE_IN_FLOATING> dreq;
};


#endif
