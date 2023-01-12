// Angles are 0 when the leg is completely straight
// Positive when bent toward the front of the robot
// Negative when bent toward the back of the robot

// Distances are in inches
// x - front of robot is positive, back of robot is negative
// y - up is positive, down is negative

#include <cmath>

constexpr double RAD_TO_DEG_MULT = 180.0/3.141592653589793238463;

class Leg {
    const double LEG_PART_LENGTH = 6.4375;
    double hip_angle, knee_angle;
    motor& hip_motor, knee_motor;

    void set_overall_leg(double length, double angle) {
        knee_angle = 2 * asin(length / (2 * LEG_PART_LENGTH)) * RAD_TO_DEG_MULT;
        hip_angle = 90 - knee_angle / 2;
        knee_angle -= 180.0;    // So the angle is 0 when the leg is straight
        knee_angle *= -1.0;     // Knee typically bends backwards
        hip_angle += angle;     // Bend overall leg
    }

public:
    Leg(motor& hip, motor& knee) : hip_motor {hip}, knee_motor {knee} {}

    void set_foot_position(double x, double y) {
        double overall_leg_length = sqrt(x*x + y*y);
        double overall_leg_angle = atan(x / -y) * RAD_TO_DEG_MULT;
        set_overall_leg(overall_leg_length, overall_leg_angle);
    }

    void update_leg() {
        hip_motor.spinToPosition(hip_angle, degrees, false);
        knee_motor.spinToPosition(knee_angle, degrees, false);
    }
};

class Quad {
    const double INITIAL_BODY_X = 0;
    const double INITIAL_BODY_Y = 10;
    double body_x = INITIAL_BODY_X;
    double body_y = INITIAL_BODY_Y;
    Leg front_left, front_right, back_left, back_right;

public:
    Quad(motor& front_left_hip, motor& front_left_knee,
         motor& front_right_hip, motor& front_right_knee,
         motor& back_left_hip, motor& back_left_knee,
         motor& back_right_hip, motor& back_right_knee) :
         front_left{front_left_hip, front_left_knee},
         front_right{front_right_hip, front_left_knee},
         back_left{back_left_hip, back_left_knee},
         back_right{back_right_hip, back_right_knee} {
        update_body();
    }

    void update_body() {
        front_left.set_foot_position(-body_x, -body_y);
        front_right.set_foot_position(-body_x, -body_y);
        back_left.set_foot_position(-body_x, -body_y);
        back_right.set_foot_position(-body_x, -body_y);
        front_left.update_leg();
        front_right.update_leg();
        back_left.update_leg();
        back_right.update_leg();
    }
}

int main() {
    Quad quad{motor_1, motor_2,
              motor_3, motor_4,
              motor_11, motor_12,
              motor_13, motor_14};
}