#include <Romi32U4.h>
#include "chassis.h"

float RomiChassis::SpeedLeft(void)
{
    // float v_tan_ms = (C_wheel / N_wheel) * (count_left - prev_count_left) / (millis() - last_update);
    float v_tan_ms = (C_wheel / N_wheel) * (count_left - prev_count_left) / (previous_time - previous_previous_time);
    float v_tan_s = v_tan_ms * (1.0 / 0.001);
    return v_tan_s; //[mm/s]
}

float RomiChassis::SpeedRight(void)
{
    float v_tan_ms = (C_wheel / N_wheel) * (count_right - prev_count_right) / (previous_time - previous_previous_time);
    float v_tan_s = v_tan_ms * (1.0 / 0.001);
    return v_tan_s; // [mm/s]

}

void RomiChassis::UpdateEffortDriveWheels(int left, int right)
{ 
    motors.setEfforts(left,right);
}

void RomiChassis::UpdateEffortDriveWheelsPI(int target_speed_left, int target_speed_right)
{
  // !!! ATTENTION !!!
  // Assignment 2
  {
    float error_left = target_speed_left - SpeedLeft();
    float error_right = target_speed_right - SpeedRight();
    
    E_left += error_left;
    E_right += error_right;

    float u_left = Kp * error_left + Ki * E_left;
    float u_right = Kp * error_right + Ki * E_right;
    // motors.setEfforts(u_left,u_right);
    motors.setEfforts(100, 100);
  }
}

void RomiChassis::SerialPlotter(float a, float b, float c, float d)
{
    // !!! ATTENTION !!!
    // USE this function for assignment 3!
    Serial.print(a);
    // Serial.print('\t');
    Serial.print(",");
    Serial.print(b);
    Serial.print(",");
    // Serial.print('\t');
    Serial.print(c);
    // Serial.print('\t');
    // Serial.print(d);
    Serial.println();
}

void RomiChassis::MotorControl(void)
{
  uint32_t now = millis();
  if(now - last_update >= interval)
  {    
    prev_count_left = count_left;
    prev_count_right = count_right;
    count_left = encoders.getCountsLeft();
    count_right = encoders.getCountsRight();
    previous_previous_time = previous_time;
    previous_time = millis();
    UpdateEffortDriveWheelsPI(target_left, target_right);
    last_update = now;
  }
}

void RomiChassis::StartDriving(float left, float right, uint32_t duration)
{
  target_left = left; target_right = right;
  start_time = millis();
  last_update = start_time;
  end_time = start_time + duration; //fails at rollover
  E_left = 0;
  E_right = 0;
}

bool RomiChassis::CheckDriveComplete(void)
{
  return millis() >= end_time;
}

void RomiChassis::Stop(void)
{
  target_left = target_right = 0;
  motors.setEfforts(0, 0);
}