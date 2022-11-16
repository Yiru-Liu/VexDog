// Angles are 0 when the leg is completely straight
// Positive when bent toward the front of the robot
// Negative when bent toward the back of the robot

// Distances are in inches
// x - front of robot is positive, back of robot is negative
// y - up is positive, down is negative

#include <cmath>

const int LEG_PART_LENGTH = 6.4375;

class Leg {
    volatile double hip_angle, knee_angle;
    motor hip_motor, knee_motor;
    event move_hip_event, move_knee_event;

    public:
        Leg(motor hip, motor knee) {
            hip_motor = hip;
            knee_motor = knee;
            move_hip_event(set_hip);
            move_knee_event(set_knee);
        }

        void set_hip() {
            hip_motor.setPosition(hip_angle, degrees);
        }

        void set_knee() {
            knee_motor.setPosition(knee_angle, degrees);
        }

        void update_leg() {
            move_hip_event.broadcast();
            move_knee_event.broadcast();
        }

        void set_overall_leg(double length, double angle) {
            knee_angle = 2 * asin(length / (2 * LEG_PART_LENGTH));
            hip_angle = 90 - knee_angle/2;
            knee_angle -= 180.0;    // So the angle is 0 when the leg is straight
            knee_angle *= -1.0;     // Knee typically bends backwards
            hip_angle += angle;     // Bend overall leg
            update_leg();
        }

        void set_foot_position(double x, double y) {
            double overall_leg_length = sqrt(x*x + y*y);
            double overall_leg_angle = atan(x / -y);
            set_overall_leg(overall_leg_length, overall_leg_angle);
        }
}

class Quad {
    
}