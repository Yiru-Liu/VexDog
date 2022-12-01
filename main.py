# Angles are 0 when the leg is completely straight
# Positive when bent toward the front of the robot
# Negative when bent toward the back of the robot
# So in order to do this, the directions of the motors on the left side of the
# need to be reversed

# Distances are in inches
# x - front of robot is positive, back of robot is negative
# y - up is positive, down is negative

import math

LEG_PART_LENGTH = 6.4375

class Leg:
    def __init__(self, hip_motor, knee_motor) -> None:
        self.hip_angle, self.knee_angle = 0, 0
        self.hip_motor = hip_motor
        self.hip_motor.set_stopping(HOLD)
        self.knee_motor = knee_motor
        self.knee_motor.set_stopping(HOLD)
        self.move_hip_event = Event()
        self.move_hip_event(self.set_hip)
        self.move_knee_event = Event()
        self.move_knee_event(self.set_knee)

    def set_hip(self) -> None:
        self.hip_motor.spin_to_position(self.hip_angle, DEGREES, wait=False)

    def set_knee(self) -> None:
        self.knee_motor.spin_to_position(self.knee_angle, DEGREES, wait=False)

    def update_leg(self) -> None:
        self.move_hip_event.broadcast();
        self.move_knee_event.broadcast();

    def set_overall_leg(self, length, angle) -> None:
        knee_angle = 2 * math.asin(length / (2 * LEG_PART_LENGTH))
        hip_angle = 90 - knee_angle/2
        knee_angle -= 180   # So the angle is 0 when the leg is straight
        knee_angle *= -1    # Knee typically bends backwards
        hip_angle += angle  # Bend overall leg

        self.hip_angle = hip_angle
        self.knee_angle = knee_angle

        self.update_leg()

    def set_foot_position(self, x, y) -> None:
        overall_leg_length = math.sqrt(x*x + y*y)
        overall_leg_angle = math.atan(x / -y)
        self.set_overall_leg(overall_leg_length, overall_leg_angle)


INITIAL_BODY_Y = 10
INITIAL_BODY_X = 0

class Quad:
    def __init__(self, front_left, front_right, back_left, back_right) -> None:
        self.legs = {
            "FL": Leg(*front_left),
            "FR": Leg(*front_right),
            "BL": Leg(*back_left),
            "BR": Leg(*back_right)
        }
        self.body_x = INITIAL_BODY_X
        self.body_y = INITIAL_BODY_Y
        self.update_body()

    def update_body(self) -> None:
        for leg in self.legs:
            leg.set_foot_position(-self.body_x, -self.body_y)
        for leg in self.legs:
            leg.update_leg()


def main():
    fl = (motor_1, motor_2)
    fr = (motor_3, motor_4)
    bl = (motor_11, motor_12)
    br = (motor_13, motor_14)


main()