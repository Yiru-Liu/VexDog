# Angles are 0 when the leg is completely straight
# Positive when bent toward the front of the robot
# Negative when bent toward the back of the robot

# Distances are in inches
# x - front of robot is positive, back of robot is negative
# y - up is positive, down is negative

class Leg:
    def __init__(self) -> None:
        self.hip_angle, self.knee_angle = 0, 0
        self.hip_motor = hip
        self.knee_motor = knee
        self.move_hip_event = Event()
        self.move_hip_event(set_hip)
        self.move_knee_event = Event()
        self.move_knee_event(set_knee)

    def set_hip(self) -> None:
        self.hip_motor.spin_to_position(self.hip_angle, DEGREES, wait=False)

    def set_knee(self) -> None:
        self.knee_motor.spin_to_position(self.knee_angle, DEGREES, wait=False)

    def update_leg(self) -> None:
        self.move_hip_event.broadcast();
        self.move_knee_event.broadcast();

    def set_overall_leg(self, length, angle) -> None:
        
