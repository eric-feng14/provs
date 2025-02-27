#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "pros/misc.h"
// #include "lemlib/asset.hpp"

// ASSET(example_txt);

pros::MotorGroup
    left_motors({-18, -19, -20},
                pros::MotorGearset::blue); // left motors use 600 RPM cartridges
pros::MotorGroup right_motors(
    {1, 2, 3}, pros::MotorGearset::blue); // right motors use 200 RPM cartridges
// drivetrain settings
lemlib::Drivetrain drivetrain(&left_motors,               // left motor group
                              &right_motors,              // right motor group
                              11.25,                      // 10 inch track width
                              lemlib::Omniwheel::NEW_275, // using new 4" omnis
                              450, // drivetrain rpm is 360
                              2    // horizontal drift is 2 (for now)
);
// create an imu on port 10
pros::Imu imu(10);
                                 
pros::Motor intake(6);
pros::Motor hook(-11);
pros::Motor arm(5);

pros::adi::DigitalOut clamp('H');
pros::adi::DigitalOut doinker('G');
pros::adi::DigitalOut pushPiston('F');

pros::adi::Encoder arm_encoder('A', 'B', false);

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve
    throttle_curve(3,    // joystick deadband out of 127
                   10,   // minimum output where drivetrain will move out of 127
                   1.019 // expo curve gain
    );

// input curve for steer input during driver control
lemlib::ExpoDriveCurve
    steer_curve(3,    // joystick deadband out of 127
                10,   // minimum output where drivetrain will move out of 127
                1.019 // expo curve gain
    );

// lateral motion controller
lemlib::ControllerSettings
    lateral_controller(10,  // proportional gain (kP)
                       0,   // integral gain (kI)
                       3,   // derivative gain (kD)
                       3,   // anti windup
                       1,   // small error range, in inches
                       100, // small error range timeout, in milliseconds
                       3,   // large error range, in inches
                       500, // large error range timeout, in milliseconds
                       20   // maximum acceleration (slew)
    );

// angular motion controller
lemlib::ControllerSettings
    angular_controller(2,   // proportional gain (kP)
                       0,   // integral gain (kI)
                       10,  // derivative gain (kD)
                       3,   // anti windup
                       1,   // small error range, in degrees
                       100, // small error range timeout, in milliseconds
                       3,   // large error range, in degrees
                       500, // large error range timeout, in milliseconds
                       0    // maximum acceleration (slew)
    );

lemlib::OdomSensors sensors(
    nullptr, // vertical tracking wheel 1, set to null
    nullptr, // vertical tracking wheel 2, set to nullptr as we are using IMEs
    nullptr, // horizontal tracking wheel 1
    nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a
             // second one
    &imu);   // inertial sensor

// create the chassis
lemlib::Chassis chassis(drivetrain,         // drivetrain settings
                        lateral_controller, // lateral PID settings
                        angular_controller, // angular PID settings
                        sensors, // odometry sensors (left, middle, right)
                        &throttle_curve, &steer_curve);

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
  static bool pressed = false;
  pressed = !pressed;
  if (pressed) {
    pros::lcd::set_text(2, "working");
  } else {
    pros::lcd::clear_line(2);
  }
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  pros::lcd::initialize(); // initialize brain screen
  chassis.calibrate();     // calibrate sensors
  // print position to brain screen
  pros::Task screen_task([&]() {
    while (true) {
      // print robot location to the brain screen
      pros::lcd::print(0, "X: %f", chassis.getPose().x);         // x
      pros::lcd::print(1, "Y: %f", chassis.getPose().y);         // y
      pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
      // delay to save resources
      pros::delay(20);
    }
  });
  // autonomous();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */




// /**
//  * Runs the user autonomous code. This function will be started in its own task
//  * with the default priority and stack size whenever the robot is enabled via
//  * the Field Management System or the VEX Competition Switch in the autonomous
//  * mode. Alternatively, this function may be called in initialize or opcontrol
//  * for non-competition testing purposes.
//  *
//  * If the robot is disabled or communications is lost, the autonomous task
//  * will be stopped. Re-enabling the robot will restart the task, not re-start it
//  * from where it left off.
//  */
void autonomous()
{
	
// AUTON ROUTES - 1 for each side of the field

//  RED RING SIDE

// 	 Moves the arm only
	clamp.set_value(false);
	arm.move(90);
	pros::delay(1500);
	arm.move(-90);
	pros::delay(1000);
	arm.move(0);

	chassis.moveToPoint(-24, 24, 3000, {.forwards = false});

	// Clamp the mobile goal
	clamp.set_value(true);
	pros::delay(1500);
	clamp.set_value(false);
	chassis.moveToPoint(-18, 24, 2000, {.forwards = false});
	chassis.moveToPoint(-24, 24, 500);
	chassis.moveToPoint(-20, 24, 500, {.forwards = false});

	intake.move(150);
	hook.move(-100);

	chassis.moveToPoint(-24, 48, 3000);
	chassis.moveToPoint(-3,  44, 3000);
	chassis.moveToPoint(-3, 52, 3000);
	

}

// BLUE RING SIDE

//  Moves the arm only
// 	clamp.set_value(false);
// 	arm.move(90);
// 	pros::delay(1500);
// 	arm.move(-90);
// 	pros::delay(1000);
// 	arm.move(0);

// 	chassis.moveToPoint(24, 24, 3000, {.forwards = false});

// 	 Clamp the mobile goal
// 	clamp.set_value(true);
// 	pros::delay(1500);
// 	clamp.set_value(false);
// 	chassis.moveToPoint(18, 24, 2000, {.forwards = false});
// 	chassis.moveToPoint(24, 24, 500);
// 	chassis.moveToPoint(20, 24, 500, {.forwards = false});

// 	intake.move(150);
// 	hook.move(-100);

// 	chassis.moveToPoint(24, 44, 3000);
// 	chassis.moveToPoint(3,  48, 3000);
// 	chassis.moveToPoint(3, 52, 3000);





//  RED MOGO SIDE

// Moves the arm only
// 	clamp.set_value(false);
// 	arm.move(90);
// 	pros::delay(1500);
// 	arm.move(-90);
// 	pros::delay(1000);
// 	arm.move(0);

// 	chassis.moveToPoint(-24, -24, 3000, {.forwards = false});

// 	 Clamp the mobile goal
// 	clamp.set_value(true);
// 	pros::delay(1500);
// 	clamp.set_value(false);
// 	chassis.moveToPoint(-18, -24, 2000, {.forwards = false});
// 	chassis.moveToPoint(-20, -24, 500);
// 	chassis.moveToPoint(-22, -24, 500, {.forwards = false});

// 	intake.move(150);
// 	hook.move(-100);

// 	chassis.moveToPoint(-24, -48, 3000);
//  chassis.moveToPoint(-22, -48, 3000);
//  chassis.moveToPoint(-28, -48, 3000);


// chassis.moveToPoint(-72, -72, 3000);






// BLUE MOGO SIDE

// Moves the arm only
// 	clamp.set_value(false);
// 	arm.move(90);
// 	pros::delay(1500);
// 	arm.move(-90);
// 	pros::delay(1000);
// 	arm.move(0);

// 	chassis.moveToPoint(24, -24, 3000, {.forwards = false});

// 	 Clamp the mobile goal
// 	clamp.set_value(true);
// 	pros::delay(1500);
// 	clamp.set_value(false);
// 	chassis.moveToPoint(18, -24, 2000, {.forwards = false});
// 	chassis.moveToPoint(22, -24, 500);
// 	chassis.moveToPoint(20, -24, 500, {.forwards = false});

// 	intake.move(150);
// 	hook.move(-100);

// 	chassis.moveToPoint(24, -48, 3000);
//  chassis.moveToPoint(22, -48, 3000);
//  chassis.moveToPoint(5, -48, 3000);


// chassis.moveToPoint(72, -72, 3000);
// chassis.moveToPoint(68, -72, 3000);
// chassis.moveToPoint(74, -72, 3000);






/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

pros::Controller controller(pros::E_CONTROLLER_MASTER);

void opcontrol() {
 
  // EMERGENCY RESET - Set arm to zero immediately
  arm.move(0);
  arm.tare_position();
  arm.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  arm.set_voltage_limit(12000);
  
  // Wait to ensure the motor stops
  pros::delay(500);
  
  // Initialize target position with current position
  int targetArmPos = arm_encoder.get_value();
  
  // Position holding mode flag - starts enabled by default
  bool positionHoldingEnabled = true;
  
  // Track Y button state for toggle detection
  bool lastYState = false;
  
  // Initial status message
  controller.clear();
  controller.print(0, 0, "Position Hold ON");
  controller.rumble(".");
  
  // Main control loop
  while (true) {
    // Drive control
    int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
    chassis.curvature(leftY, rightX);
    
    // Intake control
    if (controller.get_digital(DIGITAL_R1)) {
      intake.move(127); hook.move(-115);
    } else if (controller.get_digital(DIGITAL_R2)) {
      hook.move(115); intake.move(-127);
    } else {
      hook.move(0); intake.move(0);
    }
    
    // Get current arm position
    int currentArmPos = arm_encoder.get_value();
    
    // TOGGLE POSITION HOLDING MODE with Y button
    bool currentYState = controller.get_digital(DIGITAL_Y);
    if (currentYState && !lastYState) {
      // Toggle position holding mode on button press
      positionHoldingEnabled = !positionHoldingEnabled;
      
      // Provide feedback
      controller.clear();
      if (positionHoldingEnabled) {
        // Switching to position holding mode - update target to current position
        targetArmPos = currentArmPos;
        controller.print(0, 0, "Position Hold ON");
      } else {
        // Switching to direct control mode
        controller.print(0, 0, "Direct Control");
        // Immediately stop the arm when switching modes
        arm.move(0);
      }
      controller.rumble(".");
    }
    lastYState = currentYState;
    
    // EMERGENCY RESET with UP button (works in both modes)
    if (controller.get_digital(DIGITAL_UP)) {
      arm.move(0);
      targetArmPos = currentArmPos;
      controller.rumble("-");
      controller.print(0, 0, positionHoldingEnabled ? "Position Reset" : "Arm Stopped");
      pros::delay(200);
      controller.print(0, 0, positionHoldingEnabled ? "Position Hold ON" : "Direct Control");
    }
    
    // ARM CONTROL - TWO MODES
    if (controller.get_digital(DIGITAL_L1)) {
      // Move arm up with full power in both modes
      arm.move(127);
      
      // In position holding mode, update target to track current position
      if (positionHoldingEnabled) {
        targetArmPos = currentArmPos;
        controller.print(0, 0, "UP: T=%d", targetArmPos);
      } else {
        controller.print(0, 0, "UP: Direct");
      }
    } 
    else if (controller.get_digital(DIGITAL_L2)) {
      // Move arm down with controlled power in both modes
      arm.move(-127);
      
      // In position holding mode, update target to track current position
      if (positionHoldingEnabled) {
        targetArmPos = currentArmPos;
        controller.print(0, 0, "DOWN: T=%d", targetArmPos);
      } else {
        controller.print(0, 0, "DOWN: Direct");
      }
    }
    else {
      // No buttons pressed - behavior depends on mode
      
      if (positionHoldingEnabled) {
        // POSITION HOLDING MODE - maintain target position
        
        // Calculate position difference
        int posDiff = targetArmPos - currentArmPos;
        
        // Position-based control with more aggressive parameters
        int holdPower = 0;
        
        if (abs(posDiff) < 5) {
          // Within deadband - constant holding power
          holdPower = 25; // Increased from 15 for stronger hold
        }
        else if (posDiff > 0) {
          // Need to move up - stronger proportional response
          holdPower = 40 + posDiff * 3; // More aggressive response to error
          if (holdPower > 127) holdPower = 127; // Maximum power
        }
        else {
          // Need to move down
          holdPower = -25 + posDiff * 2; 
          if (holdPower < -90) holdPower = -90;
        }
        
        // Apply calculated power to motor
        arm.move(holdPower);
        
        // Display status
        controller.print(0, 0, "Hold: %d (%+d)", holdPower, posDiff);
      }
      else {
        // DIRECT CONTROL MODE - stop motor when no buttons pressed
        arm.move(0);
        controller.print(0, 0, "Direct Control");
      }
    }
    
    // Controls for peripherals
    static bool clamp_state = false;
    static bool clamp_last_a_state = false;
    bool clamp_current_a_state = controller.get_digital(DIGITAL_A);

    // Toggle state on button press (not hold)
    if (clamp_current_a_state && !clamp_last_a_state) {
      clamp_state = !clamp_state;
    }
    clamp_last_a_state = clamp_current_a_state;

    // Set solenoid based on toggled state
    clamp.set_value(clamp_state);

    // DOINKERRR
    static bool doinker_state = false;
    static bool doinker_last_x_state = false;
    bool doinker_current_x_state = controller.get_digital(DIGITAL_X);

    // Toggle state on button press (not hold)
    if (doinker_current_x_state && !doinker_last_x_state) {
      doinker_state = !doinker_state;
    }
    doinker_last_x_state = doinker_current_x_state;

    // Set solenoid based on toggled state
    doinker.set_value(doinker_state);

    // PUSH PISTON
    static bool pushPiston_state = false;
    static bool pushPiston_last_x_state = false;
    bool pushPiston_current_x_state = controller.get_digital(DIGITAL_RIGHT);

    // Toggle state on button press (not hold)
    if (pushPiston_current_x_state && !pushPiston_last_x_state) {
      pushPiston_state = !pushPiston_state;
    }
    pushPiston_last_x_state = pushPiston_current_x_state;

    // Set solenoid based on toggled state
    pushPiston.set_value(pushPiston_state);

    
    
    // Show motor diagnostics less frequently to avoid display clutter
    static int displayCounter = 0;
    if (++displayCounter >= 50) { // Update approximately every 1 second
      double motor_temp = arm.get_temperature();
      double motor_current = arm.get_current_draw();
      controller.print(1, 0, "I:%.1fA T:%.1fC", motor_current, motor_temp);
      displayCounter = 0;
    }
    
    pros::delay(20);
  }
}