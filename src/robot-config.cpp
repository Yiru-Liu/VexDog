#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
motor Motor1 = motor(PORT1, ratio18_1, false);
motor Motor2 = motor(PORT2, ratio18_1, true);
motor Motor3 = motor(PORT3, ratio18_1, true);
motor Motor4 = motor(PORT4, ratio18_1, false);
motor Motor11 = motor(PORT11, ratio18_1, false);
motor Motor12 = motor(PORT12, ratio18_1, true);
motor Motor13 = motor(PORT13, ratio18_1, true);
motor Motor14 = motor(PORT14, ratio18_1, false);
controller Controller1 = controller(primary);
sonar RangeFinderA = sonar(Brain.ThreeWirePort.A);

// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}