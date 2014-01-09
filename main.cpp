#include <GL/freeglut.h>
//#include <unistd.h>  used for usleep in Linux; replaces Sleep in Windows
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <math.h>
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>

#define _USE_MATH_DEFINES

#define PI 3.14159265
#define RAD PI/180
#define DEG 180/PI

using namespace std;

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;

const int SHIP_NOSE = 20;
const int SHIP_WING = 15;

const int MAX_BULLETS = 256;

const int ASTER_LRG_MAX = 40;
const int ASTER_LRG_MIN = 30;
const int ASTER_MED_MAX = 20;
const int ASTER_MED_MIN = 7;
const int ASTER_SML_MAX = 5;
const int ASTER_SML_MIN = 3;

const int FONT_1 = (int)GLUT_BITMAP_9_BY_15;
const int FONT_2 = (int)GLUT_BITMAP_HELVETICA_18;
const int FONT_3 = (int)GLUT_BITMAP_HELVETICA_12;


// A simple wrapper to store colors
struct Color
{
	float r;
	float g;
	float b;
	float a;

	Color() : r(0.0), g(0.0), b(0.0), a(1.0) {}
	Color(float r, float g, float b, float a = 1.0) 
		: r(r), g(g), b(b), a(a) 
	{}
};
// Vector class for simplified calculation calls
struct Vector2
{
	double x;
	double y;

	Vector2() : x(0.0), y(0.0)
	{}

	Vector2(double x, double y)
		: x(x), y(y)
	{}

	Vector2(const Vector2 & v)
		: x(v.x), y(v.y)
	{}

	Vector2 operator+(const Vector2 & rhs) const
	{ return Vector2(x + rhs.x, y + rhs.y); }
	Vector2 operator-(const Vector2 & rhs) const
	{ return Vector2(x - rhs.x, y - rhs.y); }
	Vector2 operator*(double rhs) const
	{ return Vector2(x * rhs, y * rhs); }
	Vector2 operator/(double rhs) const
	{ return Vector2(x / rhs, y / rhs); }
	Vector2 operator+=(const Vector2 & rhs)
	{ x += rhs.x; y += rhs.y; return *this; }
	Vector2 operator-=(const Vector2 & rhs)
	{ x -= rhs.x; y -= rhs.y; return *this; }
	Vector2 operator*=(double rhs)
	{ x *= rhs; y *= rhs; return *this; }
	Vector2 operator/=(double rhs)
	{ x /= rhs; y /= rhs; return *this; }

	double magnitude() const
	{ return sqrt(x * x + y * y); }
	void normalize()
	{ *this /= magnitude(); }
	double dot(const Vector2 & rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}
};
// Class for mouse tracking cursor aiming
struct Crosshairs
{
	double xPosition;
	double yPosition;
	
	double scaler;
	
	Crosshairs() 
		: xPosition(0), yPosition(0), scaler(5) 
	{}
	Crosshairs(double x, double y, double s = 5) 
		: xPosition(x), yPosition(y), scaler(s)
	{}
	
	void draw() {
		// Left to right crossline
		glBegin(GL_LINES);
        	glVertex2d(xPosition - scaler, yPosition - scaler);
        	glVertex2d(xPosition + scaler, yPosition + scaler);
    	glEnd();
    	// Right to left crossline
    	glBegin(GL_LINES);
        	glVertex2d(xPosition - scaler, yPosition + scaler);
        	glVertex2d(xPosition + scaler, yPosition - scaler);
    	glEnd();
    	// Center diamond
    	glBegin(GL_LINE_LOOP);
    		glVertex2d(xPosition - scaler, yPosition);
    		glVertex2d(xPosition, yPosition - scaler);
        	glVertex2d(xPosition + scaler, yPosition);
        	glVertex2d(xPosition, yPosition + scaler);
        glEnd();
	}

};
// Space craft character class
struct SpaceCraft
{
	double xPosition;
	double yPosition;
	
	double xNose;
	double yNose;
	
	double xWingLeft;
	double yWingLeft;
	
	double xWingRight;
	double yWingRight;
	
	double xTail;
	double yTail;
	
	double scaler;

	double lives;
	
	SpaceCraft() 
		: xPosition(WINDOW_WIDTH / 2), yPosition(WINDOW_HEIGHT / 2) 
	{initShip(); lives = 3;}
	SpaceCraft(double x, double y) 
		: xPosition(x), yPosition(y)
	{}
	
	void initShip() {
		xNose = xPosition;
		yNose = yPosition + SHIP_NOSE;
		xWingRight = xPosition + SHIP_WING;
		yWingRight = yPosition - SHIP_NOSE;
		xWingLeft = xPosition - SHIP_WING;
		yWingLeft = yPosition - SHIP_NOSE;
		xTail = xPosition;
		yTail = yPosition  - SHIP_NOSE/2;
	}
	
	void draw() {
		glBegin(GL_LINE_LOOP);
			glVertex2d(xNose, yNose);
			glVertex2d(xWingRight, yWingRight);
			glVertex2d(xTail, yTail);
			glVertex2d(xWingLeft, yWingLeft);
		glEnd();
	}
	
	double getCenter (char coord) {
	switch (coord) {
		case 'x':
			return (xNose + xWingLeft + xWingRight + xTail) / 4;
			break;
		case 'y':
			return (yNose + yWingLeft + yWingRight + yTail) / 4;
			break;
		}
	}
	
	void translateOrig(int inverse, double xMid, double yMid) {	
		if (inverse > 0) {
			xNose -= xMid;
			yNose -= yMid;
			xWingRight -= xMid;
			yWingRight -= yMid;
			xWingLeft -= xMid;
			yWingLeft -= yMid;
			xTail -= xMid;
			yTail -= yMid;
		} else if (inverse < 0) {
			xNose += xMid;
			yNose += yMid;
			xWingRight += xMid;
			yWingRight += yMid;
			xWingLeft += xMid;
			yWingLeft += yMid;
			xTail += xMid;
			yTail += yMid;		
		}
		
	}

	void rotateTo(double x, double y) {
		Vector2 v(xPosition - xPosition, yPosition - WINDOW_HEIGHT);
		Vector2 u(xPosition - x, yPosition - y);
		double angle = acos((v.dot(u) / (v.magnitude()*u.magnitude())));
		angle *= DEG;
		if (xPosition < x)
			angle *= -1;
			
		initShip();
		
		double xMid = getCenter ('x');
		double yMid = getCenter ('y');

		translateOrig(1, xMid, yMid);
		
		double tmp1;
		double tmp2;
		tmp1 = xNose * cos(angle*RAD) - yNose * sin(angle*RAD);
		tmp2 = xNose * sin(angle*RAD) + yNose * cos(angle*RAD);
		xNose = tmp1;
		yNose = tmp2;
		tmp1 = xWingRight * cos(angle*RAD) - yWingRight * sin(angle*RAD);
		tmp2 = xWingRight * sin(angle*RAD) + yWingRight * cos(angle*RAD);
		xWingRight = tmp1;
		yWingRight = tmp2;
		tmp1 = xWingLeft * cos(angle*RAD) - yWingLeft * sin(angle*RAD);
		tmp2 = xWingLeft * sin(angle*RAD) + yWingLeft * cos(angle*RAD);
		xWingLeft = tmp1;
		yWingLeft = tmp2;
		tmp1 = xTail * cos(angle*RAD) - yTail * sin(angle*RAD);
		tmp2 = xTail * sin(angle*RAD) + yTail * cos(angle*RAD);	
		xTail = tmp1;
		yTail = tmp2;
		
		translateOrig(-1, xMid, yMid);
	}
	
	void move(double x, double y, int dropOff) {
		double newX = x - xPosition;
		double newY = y - yPosition;
		newX /= dropOff;
		newY /= dropOff;
		xPosition += newX;
		yPosition += newY;
		rotateTo(x, y);
	}
};
// Class for space craft projectile weapon
struct Bullet
{
	double xPosition;
	double yPosition;

	double xMove;
	double yMove;
	
	Bullet() 
		: xPosition(-1), yPosition(-1) 
	{}
	Bullet(double x, double y) 
		: xPosition(x), yPosition(y)
	{}

	void draw() {
		glPointSize(3.0);
		glBegin(GL_POINTS);
			glVertex2d(xPosition, yPosition);
		glEnd();
	}

	void calcMove(double xShipPos, double yShipPos, double xCrossPos, double yCrossPos) {
		double moveInc = 15;
		
		xMove = xCrossPos - xShipPos;
		yMove = yCrossPos - yShipPos;
		xMove /= moveInc;
		yMove /= moveInc;
	}

	void move() {
		xPosition += xMove;
		yPosition += yMove;
	}
};
// Class for moving obsticle enemys
struct Asteroid
{
	char size;

	double xPosition;
	double yPosition;

	double xP1;
	double yP1;
	double xP2;
	double yP2;
	double xP3;
	double yP3;
	double xP4;
	double yP4;
	double xP5;
	double yP5;
	double xP6;
	double yP6;
	double xP7;
	double yP7;
	double xP8;
	double yP8;

	double xMove;
	double yMove;

	Asteroid() 
		: xPosition(-1), yPosition(-1), size('L')
	{}
	Asteroid(double x, double y, char s) 
		: xPosition(x), yPosition(y), size(s)
	{spawn(s);}

	int rRadius(int min, int max) {
		return rand() % max + min;
	}

	void spawn(char s) {
//		srand (time(NULL));
		int min, max, speed;
		switch(s) {
			case 'L':
				min = ASTER_LRG_MIN;
				max = ASTER_LRG_MAX;
				speed = 1000;
				break;
			case 'M':
				min = ASTER_MED_MIN;
				max = ASTER_MED_MAX;
				speed = 1000;
				break;
			case 'S':
				min = ASTER_SML_MIN;
				max = ASTER_SML_MAX;
				speed = 450;
				break;
			default:
				min = ASTER_LRG_MIN;
				max = ASTER_LRG_MAX;
				speed = 1000;
				break;
		}
		xP1 = 0;
		yP1 = rRadius(min, max);
		xP2 = rRadius(min, max);
		yP2 = xP2;
		xP3 = rRadius(min, max);
		yP3 = 0;
		xP4 = rRadius(min, max);
		yP4 = -(xP4);
		xP5 = 0;
		yP5 = -(rRadius(min, max));
		xP6 = -(rRadius(min, max));
		yP6 = xP6;
		xP7 = -(rRadius(min, max));
		yP7 = 0;
		xP8 = -(rRadius(min, max));
		yP8 = -(xP8);

		xMove = WINDOW_WIDTH/2 - xPosition;
		yMove = WINDOW_HEIGHT/2 - yPosition;
		xMove /= speed;
		yMove /= speed;
	}

	void randomize() {
		int r = rand() % 4 + 1;

		int boost = 3;
		if (size == 'S')
			boost =4;

		switch(r) {
			case 1:
				xMove = 0;
				yMove *= boost;
				break;
			case 2:
				yMove = 0;
				xMove *= boost;
				break;
			case 3:
				xMove = 0;
				yMove *= -boost;
				break;
			case 4:
				yMove = 0;
				xMove *= -boost;
				break;
			default:
				break;
		}
	}

	void draw() {
		glBegin(GL_LINE_LOOP);
			glVertex2d(xPosition + xP1, yPosition + yP1);
			glVertex2d(xPosition + xP2, yPosition + yP2);
			glVertex2d(xPosition + xP3, yPosition + yP3);
			glVertex2d(xPosition + xP4, yPosition + yP4);
			glVertex2d(xPosition + xP5, yPosition + yP5);
			glVertex2d(xPosition + xP6, yPosition + yP6);
			glVertex2d(xPosition + xP7, yPosition + yP7);
			glVertex2d(xPosition + xP8, yPosition + yP8);
		glEnd();
	}

	void move() {
		xPosition += xMove;
		yPosition += yMove;
		int buffer = 20;
		//Wrap around
		if (xPosition < 0 - ASTER_LRG_MAX - buffer) 
			xPosition = WINDOW_WIDTH + ASTER_LRG_MAX;
		else if (xPosition > WINDOW_WIDTH + ASTER_LRG_MAX + buffer) 
			xPosition = 0 - ASTER_LRG_MAX;
		if (yPosition < 0 - ASTER_LRG_MAX - buffer)
			yPosition = WINDOW_HEIGHT + ASTER_LRG_MAX;
		else if (yPosition > WINDOW_HEIGHT + ASTER_LRG_MAX + buffer) 
			yPosition = 0 - ASTER_LRG_MAX;
	}
};
// Heads Up Display class
struct HUD
{
	int lives;
	int score;

	HUD() 
		: lives(3), score(0)
	{}

	void livesDraw(double xp, double yp) {
		double nX = xp;
		double nY = yp + (SHIP_NOSE / 2);
		double wrX = xp + (SHIP_WING / 2);
		double wrY = yp - (SHIP_NOSE / 2);
		double wlX = xp - (SHIP_WING / 2);
		double wlY = yp - (SHIP_NOSE / 2);
		double tX = xp;
		double tY = yp  - (SHIP_NOSE/4);
		glBegin(GL_LINE_LOOP);
			glVertex2d(nX, nY);
			glVertex2d(wrX, wrY);
			glVertex2d(tX, tY);
			glVertex2d(wlX, wlY);
		glEnd();
	}

	void invisible() {
		glBegin(GL_LINE_LOOP);
		glEnd();
	}
};

void Initialize() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
}

// Some GLOBAL variables for testing ////////////////////////////////////////////!!!!!!!!!!!!!!!!!
bool gameStart = true;
bool gameOver = false;
bool resetOK = false;
bool gamePause = false;

int updateClicker = 0;

int shipMoveDropOff = 0;
int shipMoveX = 0;
int shipMoveY = 0;
int shipRotX = 0;
int shipRotY = 0;

Crosshairs aim (WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
SpaceCraft ship;
HUD hud;
vector<Bullet> bulletClip;
vector<Asteroid> asterCluster;
//Asteroid asterTest(WINDOW_WIDTH, WINDOW_HEIGHT, 'L');

void renderBitmapString(float x, float y, void *font, const char *string){
    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
} 

string to_string(int number)
{
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

void renderGame() {
	aim.draw();
	ship.draw();
	for (int i = 0; i < bulletClip.size(); i++)
		bulletClip[i].draw();
	for (int i = 0; i < asterCluster.size(); i++) 
		asterCluster[i].draw();
	if (!gameStart) {
		string scoreFont = to_string(hud.score);
		renderBitmapString(30, WINDOW_HEIGHT-30, (void *)FONT_1, scoreFont.c_str());
		if (ship.lives >= 3) {
			hud.livesDraw(75, WINDOW_HEIGHT-45);
		}
		if (ship.lives >= 2) {
			hud.livesDraw(55, WINDOW_HEIGHT-45);
		}
		if (ship.lives >= 1) {
			hud.livesDraw(35, WINDOW_HEIGHT-45);
		}
	}
}

void renderGameOver() {
	renderBitmapString((WINDOW_WIDTH / 2) - 50, WINDOW_HEIGHT / 2, (void *)FONT_2, "GAME OVER");
	renderBitmapString((WINDOW_WIDTH / 2) - 60, (WINDOW_HEIGHT / 2) - 50, 
		(void *)FONT_3, "press ENTER to play again");
	resetOK = true;
}

void renderPause() {
	renderBitmapString((WINDOW_WIDTH / 2) - 50, WINDOW_HEIGHT / 2, (void *)FONT_2, "PAUSE");
}

void renderIntro() {
	renderBitmapString((WINDOW_WIDTH / 2) - 50, WINDOW_HEIGHT / 2, (void *)FONT_2, "ASTEROIDS!");
	renderBitmapString((WINDOW_WIDTH / 2) - 45, (WINDOW_HEIGHT / 2) - 50, 
		(void *)FONT_3, "press ENTER play");
	resetOK = true;
	aim.xPosition = -20;
	aim.yPosition = -20;
	ship.xPosition = -20;
	ship.yPosition = -20;
}

void resetGame() {
	while (!asterCluster.empty()) {
		asterCluster.pop_back();
	}
	while (!bulletClip.empty()) {
		bulletClip.pop_back();
	}
	aim.xPosition = WINDOW_WIDTH / 2;
	aim.yPosition = WINDOW_HEIGHT / 2;
	ship.xPosition = WINDOW_WIDTH / 2;
	ship.yPosition = WINDOW_HEIGHT / 2;
	ship.lives = 3;
	ship.initShip();
	hud.score = 0;
	hud.lives = 3;
	gameOver = false;
}

void GLrender()
{
	glClear(GL_COLOR_BUFFER_BIT); 
	if (gameStart) {
		renderIntro();
		renderGame();
	}
	else if (!gameOver) {
		renderGame();
		if (gamePause) {
			renderPause();
		}
	} else {
		renderGameOver();
	}

	
}

void addAsteroid () {
	int buffer = 20;
	double x = rand() % 3;
	double y;
	if (x == 0) {
		x = rand() % WINDOW_WIDTH;
		y = WINDOW_HEIGHT + ASTER_LRG_MAX + buffer;
	}
	else if (x == 1) {
		x = WINDOW_WIDTH + ASTER_LRG_MAX + buffer;
		y = rand() % WINDOW_HEIGHT;
	}
	else if (x == 2) {
		x = rand() % WINDOW_WIDTH;
		y = -ASTER_LRG_MAX - buffer;
	}
	else if (x == 3) {
		x = -ASTER_LRG_MAX - buffer;
		y = rand() % WINDOW_HEIGHT;
	}
	Asteroid a(x, y, 'L');
	asterCluster.push_back(a);
}

void checkCollision (Asteroid &a, Asteroid &b) {
	if (a.size == 'L' && b.size == 'L') {
		if ( (a.xPosition - ASTER_LRG_MAX <= b.xPosition + ASTER_LRG_MAX && 
			a.xPosition + ASTER_LRG_MAX >= b.xPosition - ASTER_LRG_MAX) 
			 && 
			 (a.yPosition - ASTER_LRG_MAX <= b.yPosition + ASTER_LRG_MAX && 
			 a.yPosition + ASTER_LRG_MAX >= b.yPosition - ASTER_LRG_MAX)){
				double iTempX = a.xMove;
				double iTempY = a.yMove;
				a.xMove = b.xMove;
				a.yMove = b.yMove;
				b.xMove = iTempX;
				b.yMove = iTempY;
		}
		return;
	}
	if (a.size == 'M' && b.size == 'M') {
		if ( (a.xPosition - ASTER_MED_MAX <= b.xPosition + ASTER_MED_MAX && 
			a.xPosition + ASTER_MED_MAX >= b.xPosition - ASTER_MED_MAX) 
			 && 
			 (a.yPosition - ASTER_MED_MAX <= b.yPosition + ASTER_MED_MAX && 
			 a.yPosition + ASTER_MED_MAX >= b.yPosition - ASTER_MED_MAX)){
				double iTempX = a.xMove;
				double iTempY = a.yMove;
				a.xMove = b.xMove;
				a.yMove = b.yMove;
				b.xMove = iTempX;
				b.yMove = iTempY;
		}
		return;
	}
	if (a.size == 'S' && b.size == 'S') {
		if ( (a.xPosition - ASTER_SML_MAX <= b.xPosition + ASTER_SML_MAX && 
			a.xPosition + ASTER_SML_MAX >= b.xPosition - ASTER_SML_MAX) 
			 && 
			 (a.yPosition - ASTER_SML_MAX <= b.yPosition + ASTER_SML_MAX && 
			 a.yPosition + ASTER_SML_MAX >= b.yPosition - ASTER_SML_MAX)){
				double iTempX = a.xMove;
				double iTempY = a.yMove;
				a.xMove = b.xMove;
				a.yMove = b.yMove;
				b.xMove = iTempX;
				b.yMove = iTempY;
		}
		return;
	}
	if (a.size == 'L' && b.size == 'M') {
		if ( (a.xPosition - ASTER_LRG_MAX <= b.xPosition + ASTER_MED_MAX && 
			a.xPosition + ASTER_LRG_MAX >= b.xPosition - ASTER_MED_MAX) 
			 && 
			 (a.yPosition - ASTER_LRG_MAX <= b.yPosition + ASTER_MED_MAX && 
			 a.yPosition + ASTER_LRG_MAX >= b.yPosition - ASTER_MED_MAX)){
				b.xMove = a.xMove;
				b.yMove = a.yMove;
				b.xMove *= 3;
				b.yMove *= 3;
		}
		return;
	}
	if (a.size == 'M' && b.size == 'L') {
		if ( (a.xPosition - ASTER_MED_MAX <= b.xPosition + ASTER_LRG_MAX && 
			a.xPosition + ASTER_MED_MAX >= b.xPosition - ASTER_LRG_MAX) 
			 && 
			 (a.yPosition - ASTER_MED_MAX <= b.yPosition + ASTER_LRG_MAX && 
			 a.yPosition + ASTER_MED_MAX >= b.yPosition - ASTER_LRG_MAX)){
				a.xMove = b.xMove;
				a.yMove = b.yMove;
				a.xMove *= 3;
				a.yMove *= 3;
		}
		return;
	}
	if (a.size == 'L' && b.size == 'S') {
		if ( (a.xPosition - ASTER_LRG_MAX <= b.xPosition + ASTER_SML_MAX && 
			a.xPosition + ASTER_LRG_MAX >= b.xPosition - ASTER_SML_MAX) 
			 && 
			 (a.yPosition - ASTER_LRG_MAX <= b.yPosition + ASTER_SML_MAX && 
			 a.yPosition + ASTER_LRG_MAX >= b.yPosition - ASTER_SML_MAX)){
				b.xMove = a.xMove;
				b.yMove = a.yMove;
				b.xMove *= 4;
				b.yMove *= 4;
		}
		return;
	}
	if (a.size == 'S' && b.size == 'L') {
		if ( (a.xPosition - ASTER_SML_MAX <= b.xPosition + ASTER_LRG_MAX && 
			a.xPosition + ASTER_SML_MAX >= b.xPosition - ASTER_LRG_MAX) 
			 && 
			 (a.yPosition - ASTER_SML_MAX <= b.yPosition + ASTER_LRG_MAX && 
			 a.yPosition + ASTER_SML_MAX >= b.yPosition - ASTER_LRG_MAX)){
				a.xMove = b.xMove;
				a.yMove = b.yMove;
				a.xMove *= 4;
				a.yMove *= 4;
		}
		return;
	}
	if (a.size == 'S' && b.size == 'M') {
		if ( (a.xPosition - ASTER_SML_MAX <= b.xPosition + ASTER_MED_MAX && 
			a.xPosition + ASTER_SML_MAX >= b.xPosition - ASTER_MED_MAX) 
			 && 
			 (a.yPosition - ASTER_SML_MAX <= b.yPosition + ASTER_MED_MAX && 
			 a.yPosition + ASTER_SML_MAX >= b.yPosition - ASTER_MED_MAX)){
				a.xMove = b.xMove;
				a.yMove = b.yMove;
				a.xMove *= 2;
				a.yMove *= 2;
		}
		return;
	}
	if (a.size == 'M' && b.size == 'S') {
		if ( (a.xPosition - ASTER_MED_MAX <= b.xPosition + ASTER_SML_MAX && 
			a.xPosition + ASTER_MED_MAX >= b.xPosition - ASTER_SML_MAX) 
			 && 
			 (a.yPosition - ASTER_MED_MAX <= b.yPosition + ASTER_SML_MAX && 
			 a.yPosition + ASTER_MED_MAX >= b.yPosition - ASTER_SML_MAX)){
				b.xMove = a.xMove;
				b.yMove = a.yMove;
				b.xMove *= 2;
				b.yMove *= 2;
		}
		return;
	}
}

bool checkBulletHit (Asteroid a, Bullet b) {
	if (a.size == 'L') {
		if ( (a.xPosition - ASTER_LRG_MAX <= b.xPosition && 
			a.xPosition + ASTER_LRG_MAX >= b.xPosition) 
			 && 
			 (a.yPosition - ASTER_LRG_MAX <= b.yPosition && 
			 a.yPosition + ASTER_LRG_MAX >= b.yPosition)){

				hud.score ++;
				return true;
		}
	}
	if (a.size == 'M') {
		if ( (a.xPosition - ASTER_MED_MAX <= b.xPosition && 
			a.xPosition + ASTER_MED_MAX >= b.xPosition) 
			 && 
			 (a.yPosition - ASTER_MED_MAX <= b.yPosition && 
			 a.yPosition + ASTER_MED_MAX >= b.yPosition)){

				hud.score += 3;
				return true;
		}
	}
	if (a.size == 'S') {
		if ( (a.xPosition - ASTER_SML_MAX <= b.xPosition && 
			a.xPosition + ASTER_SML_MAX >= b.xPosition) 
			 && 
			 (a.yPosition - ASTER_SML_MAX <= b.yPosition && 
			 a.yPosition + ASTER_SML_MAX >= b.yPosition)){

				 hud.score += 10;
				 return true;
		}
	}
	return false;
}

bool checkShipHit (Asteroid a, SpaceCraft b) {
	if (a.size == 'L') {
		if ( (a.xPosition - ASTER_LRG_MAX <= b.xPosition + SHIP_WING && 
			a.xPosition + ASTER_LRG_MAX >= b.xPosition - SHIP_WING) 
			 && 
			 (a.yPosition - ASTER_LRG_MAX <= b.yPosition + SHIP_WING && 
			 a.yPosition + ASTER_LRG_MAX >= b.yPosition - SHIP_WING)){

				hud.lives--;
				ship.lives--;
				ship.xPosition = WINDOW_WIDTH / 2;
				ship.yPosition = WINDOW_HEIGHT / 2;
				ship.initShip();
				while (!asterCluster.empty()) {
					asterCluster.pop_back();
				}
				return true;
		}
	}
	if (a.size == 'M') {
		if ( (a.xPosition - ASTER_MED_MAX <= b.xPosition + SHIP_WING && 
			a.xPosition + ASTER_MED_MAX >= b.xPosition - SHIP_WING) 
			 && 
			 (a.yPosition - ASTER_MED_MAX <= b.yPosition + SHIP_WING && 
			 a.yPosition + ASTER_MED_MAX >= b.yPosition - SHIP_WING)){

				hud.lives--;
				ship.lives--;
				ship.xPosition = WINDOW_WIDTH / 2;
				ship.yPosition = WINDOW_HEIGHT / 2;
				ship.initShip();
				int cluster = asterCluster.size();
				while (!asterCluster.empty()) {
					asterCluster.pop_back();
				}
				return true;
		}
	}
	if (a.size == 'S') {
		if ( (a.xPosition - ASTER_SML_MAX <= b.xPosition + SHIP_WING && 
			a.xPosition + ASTER_SML_MAX >= b.xPosition - SHIP_WING) 
			 && 
			 (a.yPosition - ASTER_SML_MAX <= b.yPosition + SHIP_WING && 
			 a.yPosition + ASTER_SML_MAX >= b.yPosition - SHIP_WING)){

				hud.lives--;
				ship.lives--;
				ship.xPosition = WINDOW_WIDTH / 2;
				ship.yPosition = WINDOW_HEIGHT / 2;
				ship.initShip();
				int cluster = asterCluster.size();
				while (!asterCluster.empty()) {
					asterCluster.pop_back();
				}
				return true;
		}
	}
	return false;
}

void GLupdate()
{
	if (!gamePause) {
		// Update ship position & health
		if (ship.lives < 1) {
			gameOver = true;
		}

		if (shipMoveDropOff) {
			ship.move(shipMoveX, shipMoveY, shipMoveDropOff);
			shipMoveDropOff++;
			if (shipMoveDropOff == 150)
				shipMoveDropOff = 0;
		}
		ship.rotateTo(shipRotX, shipRotY);
		for (int i = 0; i < asterCluster.size(); i++) {
			checkShipHit (asterCluster[i], ship);
		}

		// Update asteroid count & positions
		if (updateClicker == 500) {
			addAsteroid();
			updateClicker = 0;
		}

		for (int i = 0; i < asterCluster.size(); i++) {
			for (int j = 0; j < asterCluster.size(); j++) {
				if (i == j)
					continue;
				checkCollision(asterCluster[i], asterCluster[j]);
			}
			asterCluster[i].move();
		}

		// Update bullet positions
		for (int i = 0; i < bulletClip.size(); i++) {
			bulletClip[i].move();
			if (bulletClip[i].xPosition > WINDOW_WIDTH || bulletClip[i].xPosition < 0 || 
				bulletClip[i].yPosition > WINDOW_HEIGHT || bulletClip[i].yPosition < 0) {

					bulletClip.erase(bulletClip.begin() + i);
					continue;
			}
			for (int j = 0; j < asterCluster.size(); j++) {
				if (checkBulletHit(asterCluster[j], bulletClip[i])) {
					if (asterCluster[j].size == 'L') {
						Asteroid a1(asterCluster[j].xP1 + asterCluster[j].xPosition, 
							asterCluster[j].yP1 + asterCluster[j].yPosition, 'M');
						Asteroid a2(asterCluster[j].xP3 + asterCluster[j].xPosition, 
							asterCluster[j].yP3 + asterCluster[j].yPosition, 'M');
						Asteroid a3(asterCluster[j].xP5 + asterCluster[j].xPosition, 
							asterCluster[j].yP5 + asterCluster[j].yPosition, 'M');
						Asteroid a4(asterCluster[j].xP7 + asterCluster[j].xPosition, 
							asterCluster[j].yP7 + asterCluster[j].yPosition, 'M');

						a1.randomize();
						a2.randomize();
						a3.randomize();
						a4.randomize();

						asterCluster.push_back(a1);
						asterCluster.push_back(a2);
						asterCluster.push_back(a3);
						asterCluster.push_back(a4);
					}
					else if (asterCluster[j].size == 'M') {
						Asteroid a1(asterCluster[j].xP1 + asterCluster[j].xPosition, 
							asterCluster[j].yP1 + asterCluster[j].yPosition, 'S');
						Asteroid a2(asterCluster[j].xP3 + asterCluster[j].xPosition, 
							asterCluster[j].yP3 + asterCluster[j].yPosition, 'S');
						Asteroid a3(asterCluster[j].xP5 + asterCluster[j].xPosition, 
							asterCluster[j].yP5 + asterCluster[j].yPosition, 'S');
						Asteroid a4(asterCluster[j].xP7 + asterCluster[j].xPosition, 
							asterCluster[j].yP7 + asterCluster[j].yPosition, 'S');

						a1.randomize();
						a2.randomize();
						a3.randomize();
						a4.randomize();

						asterCluster.push_back(a1);
						asterCluster.push_back(a2);
						asterCluster.push_back(a3);
						asterCluster.push_back(a4);
					}
					asterCluster.erase(asterCluster.begin() + j);
					bulletClip.erase(bulletClip.begin() + i);
					break;
				}
			}
		}

		int lastUpdate = 0;
		int frames = 0;

		GLrender();
		glutPostRedisplay();
		glutSwapBuffers();

		int currentTime = glutGet( GLUT_ELAPSED_TIME );
		frames++;

		if ( ( currentTime - lastUpdate ) >= 1000 ){
			frames = 0;
			lastUpdate = currentTime;
		}

		updateClicker++;
		Sleep(10);
	} else {
		GLrender();
		glutPostRedisplay();
		glutSwapBuffers();
	}
}

void GLinit(int argc, char** argv)
{
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("ASTEROIDS!");
	glutSetCursor(GLUT_CURSOR_NONE);
	glutDisplayFunc(GLrender);
	glutIdleFunc(GLupdate);
	Initialize();
}

void aimingCursor (int x, int y) {
	// Update Cursor position
	aim.xPosition = x;
	aim.yPosition = WINDOW_HEIGHT - y;
	shipRotX = x;
	shipRotY = WINDOW_HEIGHT - y;
//	ship.rotateTo(x, WINDOW_HEIGHT - y);
}

void fireGun (int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		Bullet b(ship.xNose, ship.yNose);
		b.calcMove(ship.xTail, ship.yTail, ship.xNose, ship.yNose);
		bulletClip.push_back(b);
	}
		
}

void controls(unsigned char key, int x, int y) {
	if (key == ' ') {
		shipMoveDropOff = 45;
		shipMoveX = x;
		shipMoveY = WINDOW_HEIGHT - y;
	}
	if (key == 13 && resetOK) {
		resetGame();
		gameStart = false;
	}
	if (key == 27 && !gameOver) {
		if (gamePause == false)
			gamePause = true;
		else
			gamePause = false;
	}
}

int main(int argc, char** argv)
{
	srand (time(NULL));
	GLinit(argc, argv);
	addAsteroid();
	glutPassiveMotionFunc(aimingCursor);
	glutMouseFunc(fireGun);
	glutKeyboardFunc(controls);
	glutMainLoop();

	return 0;
}
