#include <IMU.h>

IMU::IMU(TwoWire *i2c) {
    angleX = 0;
    angleY = 0;
    angleZ = 0;
    gyroX = 0; 
    gyroY = 0;
    gyroZ = 0;
    gyroXoffset = 0; 
    gyroYoffset = 0; 
    gyroZoffset = 0;
    lastMpuUpdateTime = 0;

    mpu9250.setWire(i2c);
}

void IMU::calibrate() {
  mpu9250.beginAccel();
  mpu9250.beginGyro();
  
  Serial.println("Calibrando Giroscópio (não mova o robô)...");
  float sumX = 0, sumY = 0, sumZ = 0;

  int amo = 2000;

  for(int i = 0; i < amo; i++) {
    mpu9250.gyroUpdate();
    sumX += mpu9250.gyroX();
    sumY += mpu9250.gyroY();
    sumZ += mpu9250.gyroZ();
    delay(1);
  }

  gyroXoffset = sumX / (float) amo;
  gyroYoffset = sumY / (float) amo;
  gyroZoffset = sumZ / (float) amo;

  lastMpuUpdateTime = micros();

  Serial.println("Calibração concluída!");
}

void IMU::updateMPU() {
    mpu9250.accelUpdate();
    mpu9250.gyroUpdate();

    float aX = mpu9250.accelX();
    float aY = mpu9250.accelY();
    float aZ = mpu9250.accelZ();

    gyroX = (mpu9250.gyroX() - gyroXoffset) * DEG2RAD;
    gyroY = (mpu9250.gyroY() - gyroYoffset) * DEG2RAD;
    gyroZ = (mpu9250.gyroZ() - gyroZoffset) * DEG2RAD;

    /*// --- NOVO: Filtro passa-baixa no acelerômetro ANTES do complementar ---
    // Elimina vibração de alta frequência dos motores
    // Tf=0.05s → corte em ~3.2Hz, muito abaixo da freq. de vibração dos motores
    static float aX_f = 0, aY_f = 0, aZ_f = 0;*/
    
    unsigned long now = micros();
    float dt = (now - lastMpuUpdateTime) / 1000000.0f;
    lastMpuUpdateTime = now;
    if (dt > 0.5f || dt <= 0.0f) dt = 0.005f;

    
    float angleAccX = atan2(aY, sqrt(aX * aX + aZ * aZ));
    float angleAccY = atan2(-aX, sqrt(aY * aY + aZ * aZ));

    // 5. O Filtro Complementar Clássico (Exatamente a matemática do tockn)
    angleX = 0.995f * (gyroCoef * (angleX + gyroX * dt)) + 0.001f * (accCoef * angleAccX);
    angleY = 0.995f * (gyroCoef * (angleY + gyroY * dt)) + 0.001f * (accCoef * angleAccY);
    angleZ += gyroZ * dt; // O eixo Z usa apenas o giroscópio (Yaw)

    /*angleX *= DEG2RAD;
    angleY *= DEG2RAD;
    angleZ *= DEG2RAD;*/

    // Constante do filtro: alpha = dt / (Tf + dt)  com Tf = 0.05s
    /*float Tf_acc = 0.05f;
    float alpha_acc = dt / (Tf_acc + dt);
    aX_f += alpha_acc * (aX - aX_f);
    aY_f += alpha_acc * (aY - aY_f);
    aZ_f += alpha_acc * (aZ - aZ_f);

    // Usa acelerômetro filtrado para calcular ângulo
    float accAngleX = atan2(aY_f, aZ_f);
    float accAngleY = atan2(-aX_f, sqrt(aY_f * aY_f + aZ_f * aZ_f));*/

    // Converte giroscópio para rad/s
    

    // Filtro complementar — reduz peso do acelerômetro de 0.02 para 0.005
    // 0.02 era adequado para loops lentos (~100Hz); a ~1000Hz o acelerômetro
    // tem influência excessiva somada ao ruído de vibração
    /*angleX = 0.995f * (angleX + gyroX_rads * dt) + 0.001f * accAngleX;0.005
    angleY = 0.995f * (angleY + gyroY_rads * dt) + 0.001f * accAngleY;
    angleZ += gyroZ_rads * dt;*/
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

float IMU::getGyroX() {
  return gyroX;
}

float IMU::getGyroY() {
  return gyroY;
}

float IMU::getGyroZ() {
  return gyroZ;
}
