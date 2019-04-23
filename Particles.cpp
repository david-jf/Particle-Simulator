

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#define M_PI 3.14159265358979323846 // Window doesn't seem to define this
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <cmath>
#include <iostream>
#include <ctime> //clock
#include <fstream>
using namespace std;


/*
David Fountain
ID 140203
CMPT 315
Assignment 3
March 12, 2018
*/





bool is_mouse_down = false;
double xPos = 0;
double yPos = 0;

double accelerationY = 0.0;
double accelerationX = 0.0;


//data type points 
struct points{
	double xLocation;
	double yLocation;
	double zLocation;
	double lifeSpan;
	double v_in_x;
	double v_in_y;
	double v_in_z;
	double time;
	double initX; //initial x
	double initY; //initial y
	double initZ; //initial z
	double red;
	double blue;
	double green;
} particle;

//static const struct points EmptyStruct = {0.0};



double initVy = 0.0; //initial velocity in y
double initVx = 0.0; //initial velocity in x
double initVz = 0.0; //initial velocity in z

///////SIMULATION CASES//////////
bool gravity_small_time = false;
bool gravity_big_time = false;
bool blackhole_small_time = false;
bool blackhole_big_time = false;
bool stop = true; //starting case
bool clear = false;

double d = 0.0; //used for gravity acceleration
//double a = 0.0;


int counter = 0;
int max_counter = 0;
points particlePoints [500] = {};


//method to capture key presses to change simulation states
void keyBoard(unsigned char key, int x, int y)
{
	//set the chosen boolean variable to be true and all others to be false
	if(key == 'g')
	{
		cout<<"Gravity with small timestep"<<endl;
		gravity_small_time=true;
		gravity_big_time=false;
		blackhole_small_time=false;
		blackhole_big_time=false;
		stop=false;
		clear=false;
	}
	else if(key == 'G')
	{
		cout<<"Gravity with big timestep"<<endl;
		gravity_big_time=true;
		gravity_small_time=false;
		blackhole_small_time=false;
		blackhole_big_time=false;
		stop=false;
		clear=false;
	}
	else if(key == 'b')
	{
		cout<<"Black hole with small timestep"<<endl;
		blackhole_small_time=true;
		gravity_small_time=false;
		gravity_big_time=false;
		blackhole_big_time=false;
		stop=false;
		clear=false;
	}
	else if(key == 'B')
	{
		cout<<"Black hole with big timestep"<<endl;
		blackhole_big_time=true;
		gravity_small_time=false;
		gravity_big_time=false;
		blackhole_small_time=false;
		stop=false;
		clear=false;
	}
	else if(key == 's' || key == 'S')
	{
		cout<<"Stop Particles"<<endl;
		stop=true;
		gravity_small_time=false;
		gravity_big_time=false;
		blackhole_small_time=false;
		blackhole_big_time=false;
		clear=false;
	}
	else if(key == 'o')
	{
		cout<<"Clear screen"<<endl;
		clear=true;
		stop=false;
		gravity_small_time=false;
		gravity_big_time=false;
		blackhole_small_time=false;
		blackhole_big_time=false;
		//clear screen
	}
	
	
}

//listens for left mouse click, records the initial x,y location
void mouse(int button, int state, int x, int y)
{
	if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
	{
		is_mouse_down=true; //cout<<"mouse"<<endl;
		xPos=x;
		yPos=y;

		//normalizing to -1 to 1 space
		xPos=2.0*(xPos/glutGet(GLUT_WINDOW_WIDTH))-1.0;
		yPos= glutGet(GLUT_WINDOW_HEIGHT)-yPos;
		yPos=2.0*(yPos/glutGet(GLUT_WINDOW_HEIGHT))-1.0;

		particle.xLocation=xPos;
		particle.yLocation=yPos;
		
	}
	else if(button==GLUT_LEFT_BUTTON && state==GLUT_UP)
		is_mouse_down=false;
}

//records the x,y location of the mouse when it 
//moves across the window
void mouseMotion(int x, int y)
{
	if(is_mouse_down)
	{
		initVx += x - xPos;
		initVy += y -yPos;
		xPos=x;
		yPos=y;

		
	}
	//cout<<x<<", "<<y<<endl;

	//normalizing
	xPos=2.0*(xPos/glutGet(GLUT_WINDOW_WIDTH))-1.0;
	yPos= glutGet(GLUT_WINDOW_HEIGHT)-yPos;
	yPos=2.0*(yPos/glutGet(GLUT_WINDOW_HEIGHT))-1.0;

	//normalizing
	initVx=2.0*(initVx/glutGet(GLUT_WINDOW_WIDTH))-1.0;
	initVy= glutGet(GLUT_WINDOW_HEIGHT)-initVy;
	initVy=2.0*(initVy/glutGet(GLUT_WINDOW_HEIGHT))-1.0;

	//set the parameters in the struct
	particle.xLocation=xPos;
	particle.yLocation=yPos;
	particle.v_in_y = initVy;
	particle.v_in_x = initVx;
	//cout<<"testing "<<initVx<< ", "<<initVy<<endl;
	//cout<<xPos<<", "<<yPos<<endl;
}


//creates the space and draws points that are in the array
void display()
{
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f); // background colour
	glClear(GL_COLOR_BUFFER_BIT); // clear the background
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f); // define the size of the world
	glMatrixMode(GL_MODELVIEW);
	glColor3f(1.0, 0.0, 0.0); // change the current colour


	//glEnable(GL_PROGRAM_POINT_SIZE);
	glLoadIdentity();

	glPointSize(5);
	glBegin(GL_POINTS);


	//SMALL TIMESTEP GRAVITY
	if(gravity_small_time)
	{
	//simulating gravity particles
		for(int i=0; i<max_counter; i++)
		{
			//first check for life of particle
			if(particlePoints[i].lifeSpan>5)
			{
				//cout<<"timeUP"<<endl;
				//particlePoints[i].xLocation=NULL;
				//particlePoints[i].yLocation=0;
				//particlePoints[i] = NULL;
				//i++;

				//send particle off screen 
				particlePoints[i].xLocation = 10.0;
				particlePoints[i].yLocation = 10.0;
				particlePoints[i].v_in_y = 0.0;
				particlePoints[i].v_in_x = 0.0;
			}
			//eulers for gravity
			accelerationY=-9.8;
			
			particlePoints[i].lifeSpan+=0.1;
			
			//changing the colour of the particles
			particlePoints[i].red-=0.02;
			particlePoints[i].blue+=0.02;

			//increase time step
			particlePoints[i].time+=0.001;

			particlePoints[i].v_in_y = particlePoints[i].v_in_y+(accelerationY*particlePoints[i].time);
			particlePoints[i].yLocation = particlePoints[i].yLocation+(particlePoints[i].v_in_y*particlePoints[i].time);

			particlePoints[i].xLocation = particlePoints[i].xLocation+(particlePoints[i].v_in_x*particlePoints[i].time);

			//cout<<particlePoints[i].yLocation<<endl;

			glVertex3f((float)(particlePoints[i].xLocation), (float)particlePoints[i].yLocation, 0.0);
			glColor3f(particlePoints[i].red, 0.0, particlePoints[i].blue);
		}
	}

	//BIG TIMESTEP GRAVITY
	else if(gravity_big_time)
	{
		for(int i=0; i<max_counter; i++)
		{

			if(particlePoints[i].lifeSpan>5)
			{
				particlePoints[i].xLocation = 10.0;
				particlePoints[i].yLocation = 10.0;
				particlePoints[i].v_in_y = 0.0;
				particlePoints[i].v_in_x = 0.0;
			}
			//eulers for gravity
			accelerationY=-9.8;
			
			particlePoints[i].lifeSpan+=0.1;
			
			particlePoints[i].red-=0.02;
			particlePoints[i].blue+=0.02;
			particlePoints[i].time+=0.01;
			particlePoints[i].v_in_y = particlePoints[i].v_in_y+(accelerationY*particlePoints[i].time);
			particlePoints[i].yLocation = particlePoints[i].yLocation+(particlePoints[i].v_in_y*particlePoints[i].time);

			particlePoints[i].xLocation = particlePoints[i].xLocation+(particlePoints[i].v_in_x*particlePoints[i].time);

			//cout<<particlePoints[i].yLocation<<endl;

			glVertex3f((float)(particlePoints[i].xLocation), (float)particlePoints[i].yLocation, 0.0);
			glColor3f(particlePoints[i].red, 0.0, particlePoints[i].blue);
		}
	}

	//SMALL TIMESTEP BLACKHOLE
	else if(blackhole_small_time)
	{
		//simulating blackhole particles
		for(int i=0; i<max_counter; i++)
		{
			
			if(particlePoints[i].lifeSpan>5)
			{
				//cout<<"timeUP"<<endl;
				//particlePoints[i].xLocation=0;
				//particlePoints[i].yLocation=0;
				//particlePoints[i]*=NULL;
				//.red=0.0;
				//particle.blue=0.0;
				//particle.green=0.0;
				particlePoints[i].xLocation = 10.0;
				particlePoints[i].yLocation = 10.0;
				particlePoints[i].v_in_y = 0.0;
				particlePoints[i].v_in_x = 0.0;
			}

			d=sqrt(pow(particlePoints[i].xLocation,2)+pow(particlePoints[i].yLocation,2));
			//cout<<d<<endl;
			accelerationX=-particlePoints[i].xLocation/pow(d,2);
			accelerationY=-particlePoints[i].yLocation/pow(d,2);
			//cout<<accelerationX<<endl;
			
			particlePoints[i].lifeSpan+=0.1;
			//cout<<particlePoints[i].lifeSpan<<endl;
			
			particlePoints[i].red-=0.02;
			particlePoints[i].blue+=0.02;
			particlePoints[i].time+=0.001;
			

			particlePoints[i].v_in_y = particlePoints[i].v_in_y+(accelerationY*particlePoints[i].time);
			particlePoints[i].yLocation = particlePoints[i].yLocation+(particlePoints[i].v_in_y*particlePoints[i].time);

			particlePoints[i].v_in_x = particlePoints[i].v_in_x+(accelerationX*particlePoints[i].time);
			particlePoints[i].xLocation = particlePoints[i].xLocation+(particlePoints[i].v_in_x*particlePoints[i].time);

			//cout<<particlePoints[i].yLocation<<endl;

			glVertex3f((float)(particlePoints[i].xLocation), (float)particlePoints[i].yLocation, 0.0);
			glColor3f(particlePoints[i].red, particlePoints[i].green, particlePoints[i].blue);
		}

	}

	else if(blackhole_big_time)
	{
		//simulating blackhole particles
		for(int i=0; i<max_counter; i++)
		{
			if(particlePoints[i].lifeSpan>5)
			{
				//cout<<"timeUP"<<endl;
				//particlePoints[i].xLocation=0;
				//particlePoints[i].yLocation=0;
				//particlePoints[i]*=NULL;

				//i++;
				particlePoints[i].xLocation = 10.0;
				particlePoints[i].yLocation = 10.0;
				particlePoints[i].v_in_y = 0.0;
				particlePoints[i].v_in_x = 0.0;
			}

			d=sqrt(pow(particlePoints[i].xLocation,2)+pow(particlePoints[i].yLocation,2));
			//cout<<d<<endl;
			accelerationX=-particlePoints[i].xLocation/pow(d,2);
			accelerationY=-particlePoints[i].yLocation/pow(d,2);
			//cout<<accelerationX<<endl;
			
			particlePoints[i].lifeSpan+=0.1;
			//cout<<particlePoints[i].lifeSpan<<endl;
			
			particlePoints[i].red-=0.02;
			particlePoints[i].blue+=0.02;
			particlePoints[i].time+=0.01; //using a shorter timestep;

			particlePoints[i].v_in_y = particlePoints[i].v_in_y+(accelerationY*particlePoints[i].time);
			particlePoints[i].yLocation = particlePoints[i].yLocation+(particlePoints[i].v_in_y*particlePoints[i].time);

			particlePoints[i].v_in_x = particlePoints[i].v_in_x+(accelerationX*particlePoints[i].time);
			particlePoints[i].xLocation = particlePoints[i].xLocation+(particlePoints[i].v_in_x*particlePoints[i].time);

			//cout<<particlePoints[i].yLocation<<endl;

			glVertex3f((float)(particlePoints[i].xLocation), (float)particlePoints[i].yLocation, 0.0);
			glColor3f(particlePoints[i].red, 0.0, particlePoints[i].blue);
		}
	}
	//freezes the particles in there current positions
	else if(stop)
	{
		for(int i=0; i<max_counter; i++)
		{
			glVertex3f((float)(particlePoints[i].xLocation), (float)particlePoints[i].yLocation, 0.0);
			glColor3f(particlePoints[i].red, 0.0, particlePoints[i].blue);
		}
	}

	//clear the window by setting all particle off screen
	else if(clear==true)
	{	
		for(int i=0; i<max_counter; i++)
		{
			//cout<<"clear"<<endl;
			//particlePoints[i] = EmptyStruct;
			particlePoints[i].xLocation = 10.0;
			particlePoints[i].yLocation = 10.0;
			particlePoints[i].v_in_y = 0.0;
			particlePoints[i].v_in_x = 0.0;
		}
	}


	glEnd();

	glFlush(); // make sure the polygno is pushed to the graphics card
    
    glutSwapBuffers(); // flip the buffers so the polygon I just drew is now on the front buffer
}


//timer will increment counter and max counter
//fills in particlePoints array at position counter
void timer(int value)
{
	particle.time = 0.0;
	
	//randomizing initial velocity in x and y directions
	initVx = 2*((double)rand()/RAND_MAX)-1;
	initVy = abs(2*((double)rand()/RAND_MAX)-1);

	particle.v_in_x=initVx;
	particle.v_in_y=initVy;

	

	//particlePoints [500] = {};
	if(is_mouse_down)
	{
		particle.red=1.0;
		particle.blue=0.0;
		particle.green=0.0;
		//counter starts at 0
		
		//each index of particlePoints array contains a points struct
		particlePoints[counter]=particle; 
		particlePoints[counter].lifeSpan = 0.0;
		counter++;
		if(counter>500) //will reset to 0 when it get bigger than 500
			counter = 0; //rewrites over points in particlePoints
		if(max_counter<500)//determines the length of the loop in display()
			//only counts as many items that are currently in the array
			max_counter++;
	}
	
	glutPostRedisplay();
	glutTimerFunc(30, timer, 42);	
}


int main(int argc, char** argv)
{
    // First set up GLUT
    glutInit( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);

    // Make the window
    int windowHandle = glutCreateWindow("Particle Simulator");
    glutSetWindow(windowHandle);

    // Place and size the window
    glutPositionWindow ( 100, 100 );
    glutReshapeWindow( 500, 500 );
    //glEnable(GL_DEPTH_TEST);
 
    // set the drawing and mouse callback functions
    // notice the names are the functions defined above
    glutDisplayFunc(display);
    glutTimerFunc(1000, timer, 42);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyBoard);
    
    // Go into an infinite loop waiting for messages
    glutMainLoop();
    return 0;
}