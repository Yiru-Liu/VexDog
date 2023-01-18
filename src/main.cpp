/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Yiru Liu                                                  */
/*    Created:      Thu Jan 12 2023                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Motor1               motor         1               
// Motor2               motor         2               
// Motor3               motor         3               
// Motor4               motor         4               
// Motor11              motor         11              
// Motor12              motor         12              
// Motor13              motor         13              
// Motor14              motor         14              
// Controller1          controller                    
// RangeFinderA         sonar         A, B            
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

#include <cmath>

constexpr double RAD_TO_DEG_MULT = 180.0 / 3.141592653589793238463;

constexpr double CONTROLLER_SENSITIVITY = 0.001;
constexpr double FRONT_MIN_DISTANCE = 5;

constexpr double square(const double x) {
  return x * x;
}

class Leg {
  static constexpr double LEG_PART_LENGTH = 6.4375;
  static constexpr size_t FRAMEBUFFER_SIZE = 17;
  double hip_angle, knee_angle;
  double x, y;
  motor &hip_motor, knee_motor;
  double framebuffer_x[FRAMEBUFFER_SIZE];
  double framebuffer_y[FRAMEBUFFER_SIZE];
  vex::task *t{nullptr};

  void set_overall_leg(const double length, const double angle) {
    knee_angle = 2 * asin(length / (2 * LEG_PART_LENGTH)) * RAD_TO_DEG_MULT;
    hip_angle = 90 - knee_angle / 2;
    knee_angle -= 180.0; // So the angle is 0 when the leg is straight
    knee_angle *= -1.0;  // Knee typically bends backwards
    hip_angle += angle;  // Bend overall leg
  }

  static int execute_framebuffer_blocking(void *obj) {
    Leg *ths = static_cast<Leg*>(obj);
    for (size_t i = 0; i < FRAMEBUFFER_SIZE; i++) {
      ths->set_foot_position(ths->framebuffer_x[i], ths->framebuffer_y[i]);
      ths->update_leg(true);
    }
    ths->x = ths->framebuffer_x[FRAMEBUFFER_SIZE - 1];
    ths->y = ths->framebuffer_y[FRAMEBUFFER_SIZE - 1];
    return 0;
  }

public:
  Leg(motor &hip, motor &knee) : hip_motor{hip}, knee_motor{knee} {
    hip_motor.setStopping(hold);
    knee_motor.setStopping(hold);
  }

  ~Leg() {
    if(t) {
      t->stop();
      delete t;
    }
  }

  void set_foot_position(const double new_x, const double new_y) {
    x = new_x;
    y = new_y;
    const double overall_leg_length = sqrt(x * x + y * y);
    const double overall_leg_angle = atan(x / -y) * RAD_TO_DEG_MULT;
    set_overall_leg(overall_leg_length, overall_leg_angle);
  }

  void update_leg(const bool block) {
    hip_motor.spinToPosition(hip_angle, degrees, block);
    knee_motor.spinToPosition(knee_angle, degrees, block);
  }

  void write_framebuffer_step(const double forward, const double lift) {
    const double h = x + forward/2;
    const double k = y + lift;
    const double a = (y - k) / square(x - h);

    double current_x = x;
    const double x_increment = forward / (FRAMEBUFFER_SIZE - 1);
    for (size_t i = 0; i < FRAMEBUFFER_SIZE; i++) {
      framebuffer_x[i] = current_x;
      framebuffer_y[i] = a * square(current_x - h) + k;
      current_x += x_increment;
    }
  }

  void write_framebuffer_slide(const double slide) {
    double current_x = x;
    const double x_increment = slide / (FRAMEBUFFER_SIZE - 1);
    for (size_t i = 0; i < FRAMEBUFFER_SIZE; i++) {
      framebuffer_x[i] = current_x;
      framebuffer_y[i] = y;
      current_x += x_increment;
    }
  }

  void execute_framebuffer() {
    if(t) {
      t->stop();
      delete t;
    }
    t = new vex::task(execute_framebuffer_blocking, this);
  }
};

class Quad {
  static constexpr double INITIAL_BODY_X = 0;
  static constexpr double INITIAL_BODY_Y = 10;
  static constexpr double FOOT_STEP_FORWARD = 1.5;
  static constexpr double FOOT_STEP_LIFT = 1.0;
  static constexpr double FOOT_SLIDE_BACK = -0.5;
  static constexpr int WALK_WAIT = 1000;
  double body_x = INITIAL_BODY_X;
  double body_y = INITIAL_BODY_Y;
  Leg front_left, front_right, back_left, back_right;

public:
  Quad(motor &front_left_hip, motor &front_left_knee, 
       motor &front_right_hip, motor &front_right_knee, 
       motor &back_left_hip, motor &back_left_knee,
       motor &back_right_hip, motor &back_right_knee)
      : front_left{front_left_hip, front_left_knee},
        front_right{front_right_hip, front_left_knee},
        back_left{back_left_hip, back_left_knee}, 
        back_right{back_right_hip, back_right_knee} {
    update_body();
  }

  void set_body_x(const double new_body_x) {
    body_x = new_body_x;
  }

  void change_body_x(const double increment) {
    body_x += increment;
  }

  void set_body_y(const double new_body_y) {
    body_y = new_body_y;
  }

  void change_body_y(const double increment) {
    body_y += increment;
  }

  void update_body() {
    front_left.set_foot_position(-body_x, -body_y);
    front_right.set_foot_position(-body_x, -body_y);
    back_left.set_foot_position(-body_x, -body_y);
    back_right.set_foot_position(-body_x, -body_y);
    front_left.update_leg(false);
    front_right.update_leg(false);
    back_left.update_leg(false);
    back_right.update_leg(false);
  }

  void walk_cycle() {
    // Step front left leg forward
    front_left.write_framebuffer_step(FOOT_STEP_FORWARD, FOOT_STEP_LIFT);
    front_right.write_framebuffer_slide(FOOT_SLIDE_BACK);
    back_left.write_framebuffer_slide(FOOT_SLIDE_BACK);
    back_right.write_framebuffer_slide(FOOT_SLIDE_BACK);
    front_left.execute_framebuffer();
    front_right.execute_framebuffer();
    back_left.execute_framebuffer();
    back_right.execute_framebuffer();
    this_thread::sleep_for(WALK_WAIT);

    // Step back right leg forward
    front_left.write_framebuffer_slide(FOOT_SLIDE_BACK);
    front_right.write_framebuffer_slide(FOOT_SLIDE_BACK);
    back_left.write_framebuffer_slide(FOOT_SLIDE_BACK);
    back_right.write_framebuffer_step(FOOT_STEP_FORWARD, FOOT_STEP_LIFT);
    front_left.execute_framebuffer();
    front_right.execute_framebuffer();
    back_left.execute_framebuffer();
    back_right.execute_framebuffer();
    this_thread::sleep_for(WALK_WAIT);

    // Step front right leg forward
    front_left.write_framebuffer_slide(FOOT_SLIDE_BACK);
    front_right.write_framebuffer_step(FOOT_STEP_FORWARD, FOOT_STEP_LIFT);
    back_left.write_framebuffer_slide(FOOT_SLIDE_BACK);
    back_right.write_framebuffer_slide(FOOT_SLIDE_BACK);
    front_left.execute_framebuffer();
    front_right.execute_framebuffer();
    back_left.execute_framebuffer();
    back_right.execute_framebuffer();
    this_thread::sleep_for(WALK_WAIT);

    // Step back left leg forward
    front_left.write_framebuffer_slide(FOOT_SLIDE_BACK);
    front_right.write_framebuffer_slide(FOOT_SLIDE_BACK);
    back_left.write_framebuffer_step(FOOT_STEP_FORWARD, FOOT_STEP_LIFT);
    back_right.write_framebuffer_slide(FOOT_SLIDE_BACK);
    front_left.execute_framebuffer();
    front_right.execute_framebuffer();
    back_left.execute_framebuffer();
    back_right.execute_framebuffer();
    this_thread::sleep_for(WALK_WAIT);
  }
};

int main() {
  bool front_clear;

  vexcodeInit();  // Initializing Robot Configuration. DO NOT REMOVE!
  Quad quad{Motor1, Motor2,
            Motor3, Motor4,
            Motor11, Motor12, 
            Motor13, Motor14};

  while (true) {
    front_clear = RangeFinderA.distance(inches) >= FRONT_MIN_DISTANCE;
    quad.change_body_y(Controller1.Axis3.position() * CONTROLLER_SENSITIVITY);
    quad.change_body_x(-Controller1.Axis2.position() * CONTROLLER_SENSITIVITY);
    quad.update_body();
    this_thread::sleep_for(10);
    if (Controller1.ButtonA.pressing() && front_clear) {
      quad.walk_cycle();
      this_thread::sleep_for(3000);
    }
  }
}
