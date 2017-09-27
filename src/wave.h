/***************************************************************************
 *   Copyright (C) 2008 by Michael Jung   *
 *   michael.jung@urz.uni-heidelberg.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

//This is the program's main class. An instance of the config class has to be
//passed to the contructor in order to work correctly.

#ifndef _WAVE_H_
#define _WAVE_H_

#include "config.h"
#include "glutwindow.h"

//sets the field of view
#define FRUSTUM_NEAR_CLIPPING_PLANE_DISTANCE 1.
#define FRUSTUM_FAR_CLIPPING_PLANE_DISTANCE 10000.
#define FRUSTUM_FIELD_OF_VIEW_ANGLE 40.

//sets the initial camera distance
#define DEFAULT_CAMERA_DISTANCE 1.8

//use this to adjust zoom speed
#define ZOOM_PER_PIXEL 1.005

//the display function will be called that often if animation is active
#define FRAMES_PER_SECOND 30

//maximum number of streakline steps to be calculated
#define STREAKLINE_LENGTH 4096



class wave: public glutWindow
{
  private:
	enum BorderNumberType { LEFT, RIGHT, FRONT, BACK, BOTTOM };

	//simple class used to store the two coordinates of a streakline point
	class coord
	{
		private:
			float x, y;
		public:
			coord(float, float);
			float operator[] (int);
	};

	//glutDisplayFunc
  void display();
	//glutReshapeFunc
  void reshape(int width, int height);
	//sets the camera position
  void setView();
	//resets the view
  void resetView();
	//sets the projection matrix
  void projection();
	//glutKeyboardFunc
  void keyboard(unsigned char key, int x, int y);
	//glutSpecialFunc
  void keyboardSpecial(int key, int x, int y);
	//glutMouseFunc
  void mouse(int button, int state, int x, int y);
	//glutMotionFunc
  void motion(int x, int y);
	//glutIdleFunc
	void idle();
	//function pointer to the idle function given by the config object
	waveConfig::idleFuncPointer idleFunc;

	//calculates the intersection point with the xy plane for given screen coordinates
  void calculateTarget(int x, int y, int* tx, int* ty);
	//calculates the streakline through a given point
	void calculateStreakline(int tx, int ty);

	//renders wave, "water box" and streaklines
	void render();
	//adds legend for one dimensional overlays
	void info1D();
	//adds HSV circle for two dimensional overlays
	void info2D();
	//shows help text for program interaction
	void showHelp();
	//shows the frame number in the lower left corner
	void showFrameNumber();
	//draw a given string at the given coordinates
	//modelview and projection matrices should already be set correctly
	void drawString(double x, double y, std::string string);
	//will be called regularly if animation is active
	void animation(int i);
	//will be called regularly if camera flights are enabled
	void cameraFlightFunction(int value);
	//these functions will fetch new data for another frame
	//they call the data sources' nextFrame / prevFrame functions
	void nextFrame();
  void nextFrame(int n);
	void prevFrame();

	//used to draw the "water box"
  void drawBorder(BorderNumberType number);
	//set vertex color according to the current overlay type
  void setColor(long i);
	//converts hsv colors to rgb colors
  void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );
	//this is used by the Info2D function
	void setColoredVertex(float r, int p);

	//pointer to the config object initially passed to this instance of the wave class
  waveConfig* config;

	//in these arrays are used to save the data of the current frame
	float* height;
	float* slopeX;
	float* slopeY;
	float* overlayDim1;
	float* overlayDim2;

	//streakline from and to the point
	std::vector<coord> streakline_forward;
	std::vector<coord> streakline_backward;

	//these iterators point to the DataSources currently in use
	std::list<waveConfig::sourceTableEntry>::iterator heightDataSource, slopeXDataSource, slopeYDataSource,
																				overlayDataSource;

	bool overlayAvailable;
	bool overlayEnabled;
	bool animationActive;
	bool streaklineVisible;
	bool helpEnabled;

	int xRes, yRes;
	//This variable is only used to display the frame number in the lower left.
	//Changing it will have no effect.
	long currentFrameNumber;

  double framesPassedSinceRendering;

  double cameraX, cameraY, cameraZ;
  double cameraTargetX, cameraTargetY;

	//these variables are not used if a camera flights are enabled
  double cameraDistance;
  int theta, phi;

	//left, right, bottom, top clipping plane distances as calculated by the projection() function
  double distanceClippingPlaneX, distanceClippingPlaneY;

  public:
  wave();
  wave(waveConfig* config);
  ~wave();
};  

#endif
 