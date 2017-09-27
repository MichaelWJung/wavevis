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

#include "wave.h"
#include "vector.h"

namespace
{
//used by CameraFlightFunction
	int flightCurrentFrameNumber;
	int flightFrames;

	vector lastPosition;
	vector lastTarget;
	vector destination;
	vector destinationTarget;

	std::list<waveConfig::cameraPoint>::iterator lastCameraPosition;
};

wave::wave()
{
	std::cout << "Error: a config object has to be passed to the contructor\n";
	exit (1);
}

wave::wave(waveConfig* _config)
{
	//set callback functions to the dispatcher routines
	glutDisplayFunc(displayDispatch);
	glutReshapeFunc(reshapeDispatch);
	glutKeyboardFunc(keyboardDispatch);
	glutSpecialFunc(keyboardSpecialDispatch);
	glutMouseFunc(mouseDispatch);
	glutMotionFunc(motionDispatch);
	glutIdleFunc(idleDispatch);

  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  this->config = _config;

	xRes = config->xRes();
	yRes = config->yRes();

	overlayEnabled = false;
	overlayAvailable = false;
	helpEnabled = false;
  framesPassedSinceRendering = 0.0;
	streaklineVisible = false;

	idleFunc = config->idleFunc();

	//allocate memory for one frame
  height = (float*) malloc(sizeof(float) * xRes * yRes);
  if (height == NULL) {std::cout << "Memory error\n"; exit (2);}

  slopeX = (float*) malloc(sizeof(float) * xRes * yRes);
  if (slopeX == NULL) {std::cout << "Memory error\n"; exit (2);}

  slopeY = (float*) malloc(sizeof(float) * xRes * yRes);
  if (slopeY == NULL) {std::cout << "Memory error\n"; exit (2);}

  overlayDim1 = (float*) malloc(sizeof(float) * xRes * yRes);
  if (overlayDim1 == NULL) {std::cout << "Memory error\n"; exit (2);}

  overlayDim2 = (float*) malloc(sizeof(float) * xRes * yRes);
  if (overlayDim2 == NULL) {std::cout << "Memory error\n"; exit (2);}

	std::list<waveConfig::sourceTableEntry>::iterator it;
	it = config->dataList.begin();

	//loop over data list and set pointers to the data sources
	while (it != config->dataList.end())
	{
		switch (it->type)
		{
			case HEIGHT:
				heightDataSource = it;
				heightDataSource->data->inUse(true);
				break;
			case SLOPE_X:
				slopeXDataSource = it;
				slopeXDataSource->data->inUse(true);
				break;
			case SLOPE_Y:
				slopeYDataSource = it;
				slopeYDataSource->data->inUse(true);
				break;
			case FIELD_1D:
      case FIELD_2D:
        if (!overlayAvailable)
        {
				  overlayDataSource = it;
					overlayDataSource->data->inUse(true);
				  overlayAvailable = true;
        }
				break;
			default:
				break;
		}
		it++;
	}

	//fill buffers for the first frame
	currentFrameNumber = heightDataSource->data->currentFrame(height);
	slopeXDataSource->data->currentFrame(slopeX);
	slopeYDataSource->data->currentFrame(slopeY);

	if (overlayAvailable)
    switch (overlayDataSource->type)
    {
      case FIELD_1D:
        overlayDataSource->data->currentFrame(overlayDim1);
        break;
      case FIELD_2D:
        overlayDataSource->data->currentFrame(overlayDim1, overlayDim2);
        break;
      default:
        break;
    }

	//set camera to initial position
	resetView();

	glutPostRedisplay();

	//start animation function if enabled
	if (config->animationEnabled())
		animation(0);

	//start camera flight function if enabled
	if (config->cameraFlight.size() != 0);
		cameraFlightFunction(0);
}

wave::~wave()
{
	glutIdleFunc(NULL);
	heightDataSource->data->inUse(false);
	slopeXDataSource->data->inUse(false);
	slopeYDataSource->data->inUse(false);
	overlayDataSource->data->inUse(false);
	free(height);
	free(slopeX);
	free(slopeY);
  free(overlayDim1);
  free(overlayDim2);
}

void wave::animation(int i)
{
  if (config->animationEnabled())
  {
		//increase the frame number
    framesPassedSinceRendering += ((double) config->framesPerSec() / (double) FRAMES_PER_SECOND);
		//if framesPassedSinceRendering is >= 1 new data has to be fetched and rendered
    int n = (int) framesPassedSinceRendering;
    framesPassedSinceRendering -= n;
    if (n != 0)
    {
      if (n == 1)
        nextFrame();
      else
        nextFrame(n);
      glutPostRedisplay();
    }
		//set recall timer
    glutTimerFunc(1000 / FRAMES_PER_SECOND, animationDispatch, 0);
  }
}

void wave::cameraFlightFunction(int value)
{
  vector camera, target;
	std::list<waveConfig::cameraPoint>::iterator it;
  switch (value)
  {
    case 0: //begin flight
			it = config->cameraFlight.begin();

      //if there is only one given camera position
      if ((int) config->cameraFlight.size() == 1)
      {
        cameraX = it->x;
        cameraY = it->y;
        cameraZ = it->z;
        cameraTargetX = it->tx;
        cameraTargetY = it->ty;
      }
      else
      {
        //set lastPosition and destination vectors
        lastPosition.x = it->x;
        lastPosition.y = it->y;
        lastPosition.z = it->z;

        lastTarget.x = it->tx;
        lastTarget.y = it->ty;

        int flightDuration = it->duration;
				lastCameraPosition = it;
        
				it++;

        destination.x = it->x;
        destination.y = it->y;
        destination.z = it->z;

        destinationTarget.x = it->tx;
        destinationTarget.y = it->ty;

        //calculate the number of frames needed for the flight
        flightCurrentFrameNumber = 0;
        flightFrames = flightDuration / 20;

        //begin flight
        cameraFlightFunction(1);
      }
      break;

    case 1: //flying between two points
      if (config->flightEnabled())
      {
        //calculate new destination
				//linear interpolation between camera and target points
        camera = (destination - lastPosition)
               * ((double) flightCurrentFrameNumber / (double) flightFrames)
               + lastPosition;
        target = (destinationTarget - lastTarget)
               * ((double) flightCurrentFrameNumber / (double) flightFrames)
               + lastTarget;
        cameraX = camera.x;
        cameraY = camera.y;
        cameraZ = camera.z;
        cameraTargetX = target.x;
        cameraTargetY = target.y;

        flightCurrentFrameNumber++;

        glutPostRedisplay();

        //if reached destination advance to next position
        if (flightCurrentFrameNumber >= flightFrames)
          glutTimerFunc(20, cameraFlightDispatch, 2);
        //else continue flight
        else
          glutTimerFunc(20, cameraFlightDispatch, 1);
      }
      break;

    case 2: //advance to next position
			lastCameraPosition++;
			//if the end of the flight has not been reached yet
      if (lastCameraPosition != config->cameraFlight.end())
      {
				//set lastPosition and destination vectors
        lastPosition.x = lastCameraPosition->x;
        lastPosition.y = lastCameraPosition->y;
        lastPosition.z = lastCameraPosition->z;

        lastTarget.x = lastCameraPosition->tx;
        lastTarget.y = lastCameraPosition->ty;

        int flightDuration = lastCameraPosition->duration;

				lastCameraPosition++;

				//if there is another position left, set that one to be the destination
        if (lastCameraPosition != config->cameraFlight.end())
        {

          destination.x = lastCameraPosition->x;
          destination.y = lastCameraPosition->y;
          destination.z = lastCameraPosition->z;

          destinationTarget.x = lastCameraPosition->tx;
          destinationTarget.y = lastCameraPosition->ty;
          
					lastCameraPosition--;
        }
				//else set the first position to be the destination
        else
        {
					it = config->cameraFlight.begin();
          destination.x = it->x;
          destination.y = it->y;
          destination.z = it->z;

          destinationTarget.x = it->tx;
          destinationTarget.y = it->ty;
        }

        //calculate the number of frames needed for the flight
        flightCurrentFrameNumber = 0;
        flightFrames = flightDuration / 20;

        //begin flight
        cameraFlightFunction(1);
      }
      //if the end of the flight has been reached begin again
      else
        cameraFlightFunction(0);

      break;
  }
}

//call the nextFrame functions of each datasource in config->dataList
//and refill the buffers
void wave::nextFrame()
{
	std::list<waveConfig::sourceTableEntry>::iterator it;
	it = config->dataList.begin();

	//refill the buffers
	currentFrameNumber = heightDataSource->data->nextFrame(height);
	slopeXDataSource->data->nextFrame(slopeX);
	slopeYDataSource->data->nextFrame(slopeY);

	if (overlayAvailable)
    switch (overlayDataSource->type)
    {
      case FIELD_1D:
        overlayDataSource->data->nextFrame(overlayDim1);
        break;
      case FIELD_2D:
        overlayDataSource->data->nextFrame(overlayDim1, overlayDim2);
        break;
      default:
        break;
    }

	//call nextFrame() function for all data sources that are currently not in use
	while (it != config->dataList.end())
	{
		if (it->type == FIELD_1D && it->data != overlayDataSource->data)
			it->data->nextFrame();
		if (it->type == FIELD_2D && it->data != overlayDataSource->data)
    {
      it->data->nextFrame();
    }
		it++;
	}
	//if a streakline is visible calculate the new one for the next frame
	if (streaklineVisible && streakline_forward.size() != 0)
		calculateStreakline(streakline_forward[0][0], streakline_forward[0][1]);
}

//call the nextFrame(n) functions of each datasource in config->dataList
//and refill the buffers
void wave::nextFrame(int n)
{
	std::list<waveConfig::sourceTableEntry>::iterator it;
	it = config->dataList.begin();

	//refill the buffers
	currentFrameNumber = heightDataSource->data->nextFrame(height, n);
	slopeXDataSource->data->nextFrame(slopeX, n);
	slopeYDataSource->data->nextFrame(slopeY, n);

	if (overlayAvailable)
    switch (overlayDataSource->type)
    {
      case FIELD_1D:
        overlayDataSource->data->nextFrame(overlayDim1, n);
        break;
      case FIELD_2D:
        overlayDataSource->data->nextFrame(overlayDim1, overlayDim2, n);
        break;
      default:
        break;
    }

	//call nextFrame() function for all data sources that are currently not in use
	while (it != config->dataList.end())
	{
		if ((it->type == FIELD_1D || it->type == FIELD_2D) && it->data != overlayDataSource->data)
			it->data->nextFrame(n);
		it++;
	}
	//if a streakline is visible calculate the new one for the next frame
	if (streaklineVisible && streakline_forward.size() != 0)
		calculateStreakline(streakline_forward[0][0], streakline_forward[0][1]);
}

//call the prevFrame functions of each datasource in config->dataList
//and refill the buffers
void wave::prevFrame()
{
	std::list<waveConfig::sourceTableEntry>::iterator it;
	it = config->dataList.begin();

	//refill the buffers
	currentFrameNumber = heightDataSource->data->prevFrame(height);
	slopeXDataSource->data->prevFrame(slopeX);
	slopeYDataSource->data->prevFrame(slopeY);

	if (overlayAvailable)
    switch (overlayDataSource->type)
    {
      case FIELD_1D:
        overlayDataSource->data->prevFrame(overlayDim1);
        break;
      case FIELD_2D:
        overlayDataSource->data->prevFrame(overlayDim1, overlayDim2);
        break;
      default:
        break;
    }

	//call nextFrame() function for all data sources that are currently not in use
	while (it != config->dataList.end())
	{
		if ((it->type == FIELD_1D || it->type == FIELD_2D) && it->data != overlayDataSource->data)
			it->data->prevFrame();
		it++;
	}
	//if a streakline is visible calculate the new one for the previous frame
	if (streaklineVisible && streakline_forward.size() != 0)
		calculateStreakline(streakline_forward[0][0], streakline_forward[0][1]);
}

void wave::calculateStreakline(int tx, int ty)
{
	//if the target is on the wave
	if (tx >= 0 && tx <= config->xRes() && ty >= 0 && ty <= config->yRes())
	{
		//clear streakline vectors and put in the target at the first position
		streakline_forward.clear();
		streakline_backward.clear();
		streakline_forward.push_back(coord(tx, ty));
		streakline_backward.push_back(coord(tx, ty));

		//loop until maximum length has been reached
		for (int i = 1; i < STREAKLINE_LENGTH; ++i)
		{
			float vx, vy;
			//get interpolated velocity values at the last position
			overlayDataSource->data->interpolate(streakline_forward[i-1][0], streakline_forward[i-1][1], &vx, &vy);
			//add the velocities to the last position to get the new one
			streakline_forward.push_back(coord(streakline_forward[i-1][0] + vx,streakline_forward[i-1][1] + vy));
			
			//if the new point is outside the wave area stop the loop
			if (!(streakline_forward[i][0] >= 0 && streakline_forward[i][0] <= config->xRes() - 1 && streakline_forward[i][1] >= 0 && streakline_forward[i][1] <= config->yRes() - 1))
			{
				streakline_forward.erase(streakline_forward.end()-1);
				break;
			}
		}

		//do this again in backward direction
		for (int i = 1; i < STREAKLINE_LENGTH; ++i)
		{
			float vx, vy;
			overlayDataSource->data->interpolate(streakline_backward[i-1][0], streakline_backward[i-1][1], &vx, &vy);
			streakline_backward.push_back(coord(streakline_backward[i-1][0] - vx,streakline_backward[i-1][1] - vy));
			
			if (!(streakline_backward[i][0] >= 0 && streakline_backward[i][0] <= config->xRes() - 1 && streakline_backward[i][1] >= 0 && streakline_backward[i][1] <= config->yRes() - 1))
			{
				streakline_backward.erase(streakline_backward.end()-1);
				break;
			}
		}

		streaklineVisible = true;
	}
	//if target is outside the wave area clear and set to invisible
	else
	{
		streakline_forward.clear();
		streakline_backward.clear();
		streaklineVisible = false;
	}
}

void wave::idle()
{
	if (idleFunc != NULL)
		(*idleFunc)(config);
}

wave::coord::coord(float _x, float _y)
{
	x = _x;
	y = _y;
}

float wave::coord::operator[](int i)
{
	if (i == 0)
		return x;
	if (i == 1);
		return y;
}
