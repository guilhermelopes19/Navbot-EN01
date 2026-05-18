#ifndef IMU_H
#define IMU_H

#define DEG2RAD PI/180

#include <MPU9250_asukiaaa.h>

class IMU{
    private:
        MPU9250_asukiaaa mpu9250;
        float angleX, angleY, angleZ;
        float gyroX_rads, gyroY_rads, gyroZ_rads;
        float gyroXoffset, gyroYoffset, gyroZoffset;
        unsigned long lastMpuUpdateTime;

    public:
        IMU(TwoWire *i2c);

        void updateMPU();

        float getAngleX();
        float getAngleY();
        float getAngleZ();

        float getGyroXRads();
        float getGyroYRads();
        float getGyroZRads();

        void calibrate();
};

#endif // IMU
