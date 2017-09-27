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
  //indicates if one of the mouse buttons is pressed
  bool leftButton = false, middleButton = false, rightButton = false;
	bool ctrlLeftButton = false;

  //last known mouse position
  int lastX = 0, lastY = 0;
};

//glutKeyboardFunc
void wave::keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
	case 'Q': exit(EXIT_SUCCESS); break;
	case 'o':
		if (overlayAvailable)
		{
			//switch overlay mode
			overlayEnabled = !overlayEnabled;
			if (!overlayEnabled)
				streaklineVisible = false;
			//redisplay
			glutPostRedisplay();
		}
		break;
  case 'r':
		//reset camera view
    resetView();
		//redisplay
    glutPostRedisplay();
    break;
	case ' ':
		//switch animation on/off
		if (!config->animationEnabled())
		{
			config->animationEnabled(true);
			//call animation function which itself sets its recall timer
			animation(0);
		}
		else
			config->animationEnabled(false);
		break;
  case 'f':
		//switch camera flights on/off
		if (!config->flightEnabled())
		{
			config->flightEnabled(true);
			//call flight function which itself sets its recall timer
			cameraFlightFunction(1);
		}
		else
			config->flightEnabled(false);
		break;
  
	default: break;
	}
}

//glutSpecialFunc
void wave::keyboardSpecial(int key, int x, int y)
{
	std::list<waveConfig::sourceTableEntry>::iterator it;
  switch (key)
  {
	case GLUT_KEY_F1:
		//switch help on/off
		helpEnabled = !helpEnabled;
		//redisplay
		glutPostRedisplay();
		break;

  case GLUT_KEY_RIGHT:
    nextFrame();
    glutPostRedisplay();
    break;

  case GLUT_KEY_LEFT:
    prevFrame();
    glutPostRedisplay();
    break;

	case GLUT_KEY_DOWN:
		//if there are overlays available
		if (overlayEnabled)
		{
			it = overlayDataSource;
			it->data->inUse(false);
			//loop over the data sources in config->dataList until the next overlay has been found
			do
			{
				it++;
				if (it == config->dataList.end())
					it = config->dataList.begin();
			} while (it->type != FIELD_1D && it->type != FIELD_2D);
			overlayDataSource = it;
			it->data->inUse(true);

			//load the new overlay data in the buffer(s)
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
			//hide streakline
			streaklineVisible = false;
			//redisplay
			glutPostRedisplay();
		}
		break;

	case GLUT_KEY_UP:
		//if there are overlays available
		if (overlayEnabled)
		{
			it = overlayDataSource;
			it->data->inUse(false);
			//loop over the data sources in config->dataList until the next overlay has been found
			do
			{
				if (it == config->dataList.begin())
					it = config->dataList.end();
				it--;
			} while (it->type != FIELD_1D && it->type != FIELD_2D);
			overlayDataSource = it;
			it->data->inUse(true);

			//load the new overlay data in the buffer(s)
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
			//hide streakline
			streaklineVisible = false;
			//redisplay
			glutPostRedisplay();
		}
		break;

  default:
    break;
  }
}

//glutMotionFunc
void wave::motion(int x, int y)
{
	//if left button is pressed add the changes in the mouse position to the camera angles
	if (leftButton)
	{
    int deltaX = x - lastX;
		int deltaY = y - lastY;

    theta = (theta + deltaY);
    if (theta > 89) theta = 89;
    if (theta < -89) theta = -89;
    phi = (phi + deltaX) % 360;
	}

	//if ctrl+left button are pressed and a two-dimensional overlay field is enabled
	if (ctrlLeftButton && overlayEnabled == true && overlayDataSource->type == FIELD_2D)
	{
		int tx, ty;
		//calculate the target clicked at
		calculateTarget(x, y, &tx, &ty);
		//calculate the streakline through that point
		calculateStreakline(tx, ty);
	}

	//if middle button is pressed change camera distance
	if (middleButton)
	{
		int deltaY = y - lastY;

		cameraDistance *= pow(ZOOM_PER_PIXEL, deltaY);
	}

	//redisplay
  glutPostRedisplay();

	//save last mouse position
	lastX = x;
	lastY = y;
}

//glutMouseFunc
void wave::mouse(int button, int state, int x, int y)
{
	//update mouse state variables
	switch (button)
	{
	case GLUT_LEFT_BUTTON: 
		if (state == GLUT_DOWN)
		{
			if (glutGetModifiers() & GLUT_ACTIVE_CTRL)
			{
				if (overlayEnabled == true && overlayDataSource->type == FIELD_2D)
				{
					//if ctrl+left button are pressed and a two-dimensional overlay field is active calculate a
					//streakline at the mouse target
					int tx, ty;
					calculateTarget(x, y, &tx, &ty);
					calculateStreakline(tx, ty);
					glutPostRedisplay();
				}
				ctrlLeftButton = true;
			}
			else
				leftButton = true;
		}
		else
		{
			ctrlLeftButton = false;
			leftButton = false;
		}
		break;
			
	case GLUT_MIDDLE_BUTTON: middleButton = (state == GLUT_DOWN); break;
	case GLUT_RIGHT_BUTTON:
    if (state == GLUT_DOWN && config->freeCamera())
    {
			//set the new camera target to the point that has been clicked at
			int tx, ty;
      calculateTarget(x, y, &tx, &ty);
			cameraTargetX = tx;
			cameraTargetY = ty;
      glutPostRedisplay();
    }
    break;
	default: break;
	}

	//save mouse position
	lastX = x;
	lastY = y;
}

//calculates the intersection point with the xy plane for given screen coordinates
void wave::calculateTarget(int x, int y, int* tx, int* ty)
{
	//flip x coordinate because the view is also mirrored around the x axis
	x = glutGet(GLUT_WINDOW_WIDTH) - x;

  vector camera (cameraX, cameraY, cameraZ);
  vector target (cameraTargetX, cameraTargetY, 0);
  vector d;

	//normalized camera direction vector
  d = target - camera;
  d.normalize();

	//projection of the camera direction vector to the xy plane
  vector dxy;
  dxy = d;
  dxy.z = 0;

	//this is the center of the near clipping plane of the projection frustum
  vector p;
  p = d * FRUSTUM_NEAR_CLIPPING_PLANE_DISTANCE;
  p = p + camera;

	//p + px is the center of the right side of the frustum's near clipping plane
  vector px;
  if (cameraZ > 0)
    px = d & dxy;
  if (cameraZ < 0)
    px = dxy & d;
  px.normalize();
  px = px * distanceClippingPlaneX;

	//p + py is the center of the upper side of the frustum's near clipping plane
  vector py;
  py = d & px;
  py.normalize();
  py = py * distanceClippingPlaneY;

	//this is the vector aligned in the direction of the ray through camera and the point
	//which has been clicked at in the clipping plane
  vector a;
  a = p + (px * (2. * (double) x / (double) glutGet(GLUT_WINDOW_WIDTH) - 1.))
        + (py * (2. * (double) y / (double) glutGet(GLUT_WINDOW_HEIGHT) - 1.))
        - camera;

	//intersection of the ray with the xy plane
  *tx = camera.x - camera.z * a.x / a.z;
  *ty = camera.y - camera.z * a.y / a.z;
}
