#include <Wire.h>
#include <EEPROM.h>

#define PWM_1         9
#define DIR_1         7
#define PWM_2         10
#define DIR_2         5 

#define ENC_1         2
#define ENC_2         3

#define BRAKE         8
#define BUZZER        12
#define VBAT          A7

#define STEERING_MAX    50
#define SPEED_MAX       80

float Gyro_amount = 0.996;  

bool vertical = false;
bool calibrating = false;
bool calibrated = false;

float K1 = 21;
float K2 = 1.20;
float K3 = 6.60;
float K4 = 0.70;
float loop_time = 12;

struct OffsetsObj {
  int ID;
  int16_t AcY;
  int16_t AcZ;
};
OffsetsObj offsets;

float alpha = 0.4;

int16_t  AcY, AcZ, GyX, gyroX, gyroXfilt;

int16_t  AcYc, AcZc;
int16_t  GyX_offset = 0;
int32_t  GyX_offset_sum = 0;

float robot_angle;
float Acc_angle;

volatile byte pos;
volatile int motor_counter = 0, enc_count = 0;
int16_t motor_speed;
int32_t motor_pos;
int steering_remote = 0, speed_remote = 0;

int bat_divider = 56;

long currentT, previousT_1, previousT_2 = 0; 

void setup() {
  Serial.begin(115200);

  // Pins D9 and D10 - 7.8 kHz
  TCCR1A = 0b00000001; // 8bit
  TCCR1B = 0b00001010; // x8 fast pwm

  pinMode(DIR_1, OUTPUT);  
  pinMode(DIR_2, OUTPUT);
  pinMode(BRAKE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(ENC_1, INPUT);
  pinMode(ENC_2, INPUT);
  
  Motor1_control(0);
  Motor2_control(0);
  digitalWrite(BRAKE, HIGH);

  attachInterrupt(0, ENC_READ, CHANGE);
  attachInterrupt(1, ENC_READ, CHANGE);
  
  EEPROM.get(5, offsets);
  if (offsets.ID == 47) calibrated = true;
    else calibrated = false;
  delay(3000);
  beep();
  angle_setup();
}

void loop() {
  currentT = millis();
  if (currentT - previousT_1 >= loop_time) {
    //Tuning();  
    readControlParameters(); 
    angle_calc();

    motor_speed = -enc_count;
    enc_count = 0;

    if (vertical && calibrated && !calibrating) {
      gyroX = GyX / 131.0; 
      gyroXfilt = alpha * gyroX + (1 - alpha) * gyroXfilt;
      motor_pos += motor_speed;
      motor_pos = constrain(motor_pos, -90, 90) - steering_remote;
      int pwm = constrain(K1 * robot_angle + K2 * gyroXfilt + K3 * motor_speed + K4 * motor_pos, -255, 255); 
      if (motor_speed == 0) pwm += random(8)-4; // sometimes the motor stops 
      Motor1_control(-pwm);
      Motor2_control(speed_remote);
    } else { 
      Motor1_control(0);
      Motor2_control(0);
      motor_pos = 0;
    }
    previousT_1 = currentT;
  }
  
  if (currentT - previousT_2 >= 2000) {    
    battVoltage((double)analogRead(VBAT) / bat_divider); 
    if (!calibrated && !calibrating) {
      Serial.println("first you need to calibrate the balancing point...");
    }
    previousT_2 = currentT;
  }
}

