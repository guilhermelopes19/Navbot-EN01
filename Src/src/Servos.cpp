#include <Arduino.h>
#include <Servos.h>

void Servos::inicializarPCA9685() {
  // 1. Configura os pinos do I2C
  memset(&bbi2c, 0, sizeof(bbi2c));
  bbi2c.bWire = 0; // 0 significa usar os pinos digitais (BitBang)
  bbi2c.iSDA = 21; // Seu pino SDA
  bbi2c.iSCL = 4;  // Seu pino SCL
  I2CInit(&bbi2c, 100000); // Velocidade de 100kHz

  // 2. Configura o PCA9685 para servos (50Hz)
  uint8_t sleep_cmd[2] = {0x00, 0x10}; // Coloca o chip em sleep para mudar a frequência
  I2CWrite(&bbi2c, 0x40, sleep_cmd, 2);

  uint8_t prescale_cmd[2] = {0xFE, 121}; // Define a frequência para ~50Hz
  I2CWrite(&bbi2c, 0x40, prescale_cmd, 2);

  uint8_t wake_cmd[2] = {0x00, 0x00}; // Acorda o chip
  I2CWrite(&bbi2c, 0x40, wake_cmd, 2);
  delay(5);

  uint8_t mode_cmd[2] = {0x00, 0xA1}; // Liga o Auto-Incremento para escrita rápida
  I2CWrite(&bbi2c, 0x40, mode_cmd, 2);
}

// Função que substitui a 'board1.setPWM' da Adafruit
void Servos::setPWM_Software(uint8_t servo_num, uint16_t pulse) {
  uint8_t base_reg = 0x06 + (4 * servo_num); // Descobre o registrador do motor desejado
  
  // Monta o pacote de dados (inicia em 0, termina no valor do pulso)
  uint8_t data[5];
  data[0] = base_reg;
  data[1] = 0x00;                 // Tempo ON LSB
  data[2] = 0x00;                 // Tempo ON MSB
  data[3] = pulse & 0xFF;         // Tempo OFF LSB
  data[4] = (pulse >> 8) & 0xFF;  // Tempo OFF MSB
  
  // Envia via I2C de Software
  I2CWrite(&bbi2c, 0x40, data, 5);
}