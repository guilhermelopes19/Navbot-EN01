#ifndef SERVOS_H
#define SERVOS_H

#include <BitBang_I2C.h>

class Servos{
    private:
        BBI2C bbi2c; 

    public:
        void inicializarPCA9685();
        void setPWM_Software(uint8_t servo_num, uint16_t pulse);
};

#endif
