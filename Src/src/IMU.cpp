#include <IMU.h>

IMU::IMU(TwoWire *i2c) {
    angleX = 0;
    angleY = 0;
    angleZ = 0;
    gyroX_rads = 0; 
    gyroY_rads = 0;
    gyroZ_rads = 0;
    gyroXoffset = 0; 
    gyroYoffset = 0; 
    gyroZoffset = 0;
    lastMpuUpdateTime = 0;

    mpu9250.setWire(i2c);
}

void IMU::calibrate() {
  mpu9250.beginAccel();
  mpu9250.beginGyro();
  mpu9250.beginMag();
  
  Serial.println("Calibrando Giroscópio (não mova o robô)...");
  float sumX = 0, sumY = 0, sumZ = 0;
  for(int i = 0; i < 2000; i++) {
    mpu9250.gyroUpdate();
    sumX += mpu9250.gyroX();
    sumY += mpu9250.gyroY();
    sumZ += mpu9250.gyroZ();
    delay(1);
  }
  gyroXoffset = sumX / 2000.0;
  gyroYoffset = sumY / 2000.0;
  gyroZoffset = sumZ / 2000.0;
  lastMpuUpdateTime = micros();
  Serial.println("Calibração concluída!");
}

void IMU::updateMPU() {
  mpu9250.accelUpdate();
  mpu9250.gyroUpdate();

  float aX = mpu9250.accelX();
  float aY = mpu9250.accelY();
  float aZ = mpu9250.accelZ();

  // Cálculo de ângulo via acelerômetro
  //float accAngleX = atan2(aY, aZ) * 180.0 / PI;
  //float accAngleY = atan2(-aX, sqrt(aY * aY + aZ * aZ)) * 180.0 / PI;
  float accAngleX = atan2(aY, aZ);
  float accAngleY = atan2(-aX, sqrt(aY * aY + aZ * aZ));

  // gyro do MPU9250: padrão é 131 LSB/(°/s) no range ±250°/s
  // A biblioteca já entrega em °/s — converte para rad/s:
  gyroX_rads = (mpu9250.gyroX() - gyroXoffset) * DEG2RAD;
  gyroY_rads = (mpu9250.gyroY() - gyroYoffset) * DEG2RAD;
  gyroZ_rads = (mpu9250.gyroZ() - gyroZoffset) * DEG2RAD;

  // Aplica offset no giroscópio

  unsigned long now = micros();
  float dt = (now - lastMpuUpdateTime) / 1000000.0; // Divide por 1 milhão para ter segundos
  lastMpuUpdateTime = now;

  
  // Filtro Complementar
  angleX = 0.98f * (angleX + gyroX_rads * dt) + 0.02f * accAngleX;
  angleY = 0.98f * (angleY + gyroY_rads * dt) + 0.02f * accAngleY;
  angleZ = gyroZ_rads * dt;
}

float IMU::getAngleX() {
  return angleX;
}

float IMU::getAngleY() {
  return angleY;
}

float IMU::getAngleZ() {
  return angleZ;
}

float IMU::getGyroXRads() {
  return gyroX_rads;
}

float IMU::getGyroYRads() {
  return gyroY_rads;
}

float IMU::getGyroZRads() {
  return gyroZ_rads;
}
