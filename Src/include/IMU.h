#ifndef IMU_H
#define IMU_H

#define DEG2RAD (PI/180)
#define RAD2DEG (180/PI)

#include <MPU9250_asukiaaa.h>

class IMU{
    private:
        MPU9250_asukiaaa mpu9250;

        float angleX, angleY, angleZ;
        float gyroX, gyroY, gyroZ;
        float gyroXoffset, gyroYoffset, gyroZoffset;

        float accCoef = 0.02f;
        float gyroCoef = 0.98f;

        unsigned long lastMpuUpdateTime;

    public:
        IMU(TwoWire *i2c);

        void updateMPU();

        float getAngleX();
        float getAngleY();
        float getAngleZ();

        float getGyroX();
        float getGyroY();
        float getGyroZ();

        void calibrate();
};

#endif // IMU
