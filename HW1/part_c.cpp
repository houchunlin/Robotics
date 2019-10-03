
#include "Aria.h"

class Movements
{
public:
	Movements(ArRobot *robot) { myRobot = robot; }
	void Up();
	void Down();
	void Left();
	void Right();
	void Space();

private:
	ArRobot *myRobot;
};

void collisionAvoid(ArRobot& robot)
{
	robot.lock();
	double frontDist = robot.checkRangeDevicesCurrentPolar(-45, 45);
	double backDist = robot.checkRangeDevicesCurrentPolar(135, -135);
	
	if( (robot.getVel() > 0 && frontDist < 900) || (robot.getVel() < 0 && backDist < 700) )
	{
		robot.clearDirectMotion();
		robot.setVel(0);
	}
	
	robot.unlock();

	return;
}

int main(int argc, char **argv)
{
	ArRobot robot;
	ArSonarDevice sonar;

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
	Movements mv(&robot);
	ArFunctorC<Movements> UpCB(mv, &Movements::Up);
	ArFunctorC<Movements> DownCB(mv, &Movements::Down);
	ArFunctorC<Movements> LeftCB(mv, &Movements::Left);
	ArFunctorC<Movements> RightCB(mv, &Movements::Right);
	ArFunctorC<Movements> SpaceCB(mv, &Movements::Space);

	// 1. Lock the robot
	robot.lock();

	// 2. Write your control code here, 
	//    e.g. robot.setVel(150);  

	keyHandler.addKeyHandler(ArKeyHandler::UP, &UpCB);
	keyHandler.addKeyHandler(ArKeyHandler::DOWN, &DownCB);
	keyHandler.addKeyHandler(ArKeyHandler::LEFT, &LeftCB);
	keyHandler.addKeyHandler(ArKeyHandler::RIGHT, &RightCB);
	keyHandler.addKeyHandler(ArKeyHandler::SPACE, &SpaceCB);

	// 3. Unlock the robot
	robot.unlock();

	// 4. Sleep a while and let the robot move
	while(true){
		printf("%f %f %f\n", robot.getX(), robot.getY(), robot.getTh());
		ArUtil::sleep(300);

		collisionAvoid(robot);//COLLISION AVOIDANCE!!!
	}

	// End of controling


	Aria::shutdown();

	Aria::exit(0);
}

void Movements::Up()
{
	myRobot->lock();
	double v = myRobot->getVel();
	if(v < 201)
	{
		myRobot->setVel(v+100);
	}
	myRobot->unlock();
}

void Movements::Down()
{
	myRobot->lock();
	double v = myRobot->getVel();
	if(v > -201)
	{
		myRobot->setVel(v-100);
	}
	myRobot->unlock();
}

void Movements::Left()
{
	myRobot->lock();
	myRobot->setDeltaHeading(15);
	myRobot->unlock();
}

void Movements::Right()
{
	myRobot->lock();
	myRobot->setDeltaHeading(-15);
	myRobot->unlock();
}

void Movements::Space()
{
	myRobot->lock();
	myRobot->stop();
	myRobot->unlock();
}