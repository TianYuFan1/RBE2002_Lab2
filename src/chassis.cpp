#include <Romi32U4.h>
#include "chassis.h"

float RomiChassis::SpeedLeft(void)
{
    float v_tan = (C_wheel / N_wheel) * (count_left - prev_count_left) / (interval / 1000.0);
    return v_tan; //[mm/s]
}

float RomiChassis::SpeedRight(void)
{
    float v_tan = (C_wheel / N_wheel) * (count_right - prev_count_right) / (interval / 1000.0);
    return v_tan; // [mm/s]

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
    error_left = target_speed_left - SpeedLeft();
    error_right = target_speed_right - SpeedRight();
    
    E_left += error_left;
    E_right += error_right;

    u_left = Kp * error_left + Ki * E_left;
    u_right = Kp * error_right + Ki * E_right;
    motors.setEfforts(u_left,u_right);
    // motors.setEfforts(100, 100);
  }
}

void RomiChassis::SerialPlotter()
{
    // !!! ATTENTION !!!
    // USE this function for assignment 3!
    Serial.print(millis()); // time
    Serial.print(",");
    Serial.print(chassis.SpeedLeft()); // left speed
    Serial.print(",");
    Serial.print(chassis.SpeedRight()); // right speed
    Serial.print(",");
    Serial.print(u_left); // left effort
    Serial.print(",");
    Serial.print(u_right); // right effort
    Serial.print(",");
    Serial.print(error_left); // left error
    Serial.print(",");
    Serial.print(error_right); // right error
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
    chassis.SerialPlotter();
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