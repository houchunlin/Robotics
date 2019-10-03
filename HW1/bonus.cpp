#include <iostream>
#include "Aria.h"

int main(int argc, char **argv)
{
	ArRobot robot;
	ArSonarDevice sonar;

	double target_x, target_y, target_th;
	std::cout << "Input Pose ( x y theta(rad) ): ";
	std::cin >> target_x >> target_y >> target_th;
	

	robot.addRangeDevice(&sonar);

	Aria::init();
	
	ArSimpleConnector connector(&argc,argv);

	if (!connector.connectRobot(&robot)){
		printf("Could not connect to robot... exiting\n");
		Aria::shutdown();
		Aria::exit(1);
	}

	robot.comInt(ArCommands::ENABLE, 1);

	robot.runAsync(false);

  // Used to perform actions when keyboard keys are pressed
  ArKeyHandler keyHandler;
  Aria::setKeyHandler(&keyHandler);

  // ArRobot contains an exit action for the Escape key. It also 
  // stores a pointer to the keyhandler so that other parts of the program can
  // use the same keyhandler.
  robot.attachKeyHandler(&keyHandler);
  printf("You may press escape to exit\n");

	// TODO: control the robot

	// Start of controling
	ArActionGotoStraight moveAction("move");
	ArActionStop stopAction("stop");
	//ArActionAvoidFront avoidAction("Avoid Front Near", 225, 0);
	ArActionAvoidFront avoidActionFar("Avoid Front Far");

	//robot.addAction(&avoidAction, 100);
	robot.addAction(&avoidActionFar, 90);
	robot.addAction(&moveAction, 50);
	robot.addAction(&stopAction, 40);

	// 1. Lock the robot
	robot.lock();

	// 2. Write your control code here, 
	//    e.g. robot.setVel(150);  
	moveAction.setGoal(ArPose(target_x,target_y),false,false);
	moveAction.setCloseDist(50);
	
	// 3. Unlock the robot
	robot.unlock();

	// 4. Sleep a while and let the robot move
	while(true){
		printf("%f %f %f\n", robot.getX(), robot.getY(), robot.getTh());
		if(moveAction.haveAchievedGoal())
		{
			robot.lock();
			moveAction.cancelGoal();
			robot.unlock();
			break;
		}
		ArUtil::sleep(10);
	}

	// 5. Repeat above step with more precision
	robot.lock();
	moveAction.setGoal(ArPose(target_x,target_y),false,false);
	moveAction.setSpeed(1);
	moveAction.setCloseDist(1);
	robot.unlock();
	while(true){
		printf("%f %f %f\n", robot.getX(), robot.getY(), robot.getTh());
		if(moveAction.haveAchievedGoal())
		{
			robot.lock();
			moveAction.cancelGoal();
			robot.unlock();
			break;
		}
		ArUtil::sleep(10);
	}

	// 6. Final Orientation
	robot.lock();
	robot.setHeading(target_th*180/3.14);
	robot.unlock();
	ArUtil::sleep(5000);
	printf("%f %f %f\n", robot.getX(), robot.getY(), robot.getTh());

	// End of controling



	Aria::shutdown();

	Aria::exit(0);
}
