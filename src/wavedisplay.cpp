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

//sets the projection matrix
void wave::projection()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);

	glViewport(0, 0, w, h);

	double ratio;
	//calculate clipping plane distance to be used in the viewing frustum
	double clippingPlaneDistance = FRUSTUM_NEAR_CLIPPING_PLANE_DISTANCE
				     * tan(FRUSTUM_FIELD_OF_VIEW_ANGLE * M_PI / 360.);

	//set frustum depending on size of the window
	if (w > h)
	{
		ratio = ((double) w) / ((double) h);
    distanceClippingPlaneX = ratio * clippingPlaneDistance;
    distanceClippingPlaneY = clippingPlaneDistance;
		glFrustum( - distanceClippingPlaneX, distanceClippingPlaneX,
			   - distanceClippingPlaneY, distanceClippingPlaneY,
			   FRUSTUM_NEAR_CLIPPING_PLANE_DISTANCE, FRUSTUM_FAR_CLIPPING_PLANE_DISTANCE);
	}
	else
	{
		ratio = ((double) h) / ((double) w);
    distanceClippingPlaneX = clippingPlaneDistance;
    distanceClippingPlaneY = ratio * clippingPlaneDistance;
		glFrustum( - distanceClippingPlaneX, distanceClippingPlaneX,
			   - distanceClippingPlaneY, distanceClippingPlaneY,
			   FRUSTUM_NEAR_CLIPPING_PLANE_DISTANCE, FRUSTUM_FAR_CLIPPING_PLANE_DISTANCE);
	}
}

//sets the camera orientation
void wave::setView()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//if freeCamera is enabled calculate the camera position
  if (config->freeCamera())
  {
		//transform back from spherical coordinates to cartesion coordinates
    cameraX = cameraDistance * xRes * cos(phi * M_PI / 180) * cos(theta * M_PI / 180) + cameraTargetX;
    cameraY = cameraDistance * xRes * sin(phi * M_PI / 180) * cos(theta * M_PI / 180) + cameraTargetY;
    cameraZ = cameraDistance * xRes * sin(theta * M_PI / 180);
  }

	//mirror everything around x axis because the coordinate system of the given data is a left-handed one
	//and the openGL coordinate system a right-handed one
	glScalef(-1.0,1.0,1.0);

	//set camera position
	gluLookAt(cameraX, cameraY, cameraZ,
            cameraTargetX, cameraTargetY, 0.0,
            0.0, 0.0, 1.0);
}

//reset the camera orientation to default values
void wave::resetView()
{
  //set to initial camera position
  theta = 45;
  phi = 45;

	//set target to be the center of the wave
  cameraTargetX = xRes / 2.;
  cameraTargetY = yRes / 2.;

	cameraDistance = DEFAULT_CAMERA_DISTANCE;
}

//glutDisplayFunc
void wave::display()
{
	//set background color according to the config object
	glClearColor(config->bgred(), config->bggreen(), config->bgblue(), 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//set projection matrix
	projection();

	//set camera orientation
	setView();

	//render wave
	render();

	//add information
	if (overlayEnabled && overlayDataSource->type == FIELD_1D)
		info1D();
	if (overlayEnabled && overlayDataSource->type == FIELD_2D)
		info2D();

	//show help
	if (helpEnabled)
		showHelp();

	showFrameNumber();

	glFlush();
	glutSwapBuffers();
}

//glutReshapeFunc
void wave::reshape(int width, int height)
{
	//call display function if window has been resized
	glutPostRedisplay();
} 

void wave::info1D()
{
	//disable Z buffer to insure the text to be in front of everything else
	glDisable(GL_DEPTH_TEST);

	//set up the projection matrix for two-dimensional stuff
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//draw a bar with the different colors of the colormap
	glBegin(GL_LINES);
	for (int i = 20; i < ((float) h) / 3.; ++i)
	{
		float buffer[3];
		overlayDataSource->colormap->lookUp(1 - ((float) (i-20)) / ((float) (((float) h) / 3. - 20)), buffer);
		glColor3fv(buffer);
		glVertex2i(20, i);
		glVertex2i(40, i);
	}
	glEnd();

	glColor3f(1.0,0.0,0.0);

	//write tags beneath the bar
	drawString(50,20, overlayDataSource->colormap->maxTag());
	drawString(50,((double) h) / 3., overlayDataSource->colormap->minTag());
	glPopMatrix();
}

void wave::info2D()
{
	//set up the projection matrix for a three-dimensional perspective view
  glClear(GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, 1.0, 0.01, 100.0);
	//set the viewport to be a square in the upper left corner of the window
	glViewport(0, glutGet(GLUT_WINDOW_HEIGHT) - 120, 120, 120);
  
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
  glLoadIdentity();
	
	//calculate camera orientation
  vector camera (cameraX, cameraY, cameraZ);
  vector target (cameraTargetX, cameraTargetY, 0.0);
	//camera direction is the same as the one for the view of the wave
  vector direction = target - camera;
	//but only with a distance of 3 units
  direction.normalize();
  direction = direction * 3;

	//mirror around x axis to get a left handed coordinate system like in the view of the wave
	glScalef(-1.0,1.0,1.0);
  gluLookAt(-direction.x, -direction.y, -direction.z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

	//draw a HSV circle with V=1.0
	for (float r = 0.0; r <= 0.95; r += 0.05)
	{
		glBegin(GL_QUADS);
		for (int p = 0; p < 360; p += 10)
		{
			setColoredVertex(r, p);
			setColoredVertex(r, (p + 10) % 360);
			setColoredVertex(r + 0.05, (p + 10) % 360);
			setColoredVertex(r + 0.05, p);
		}
		glEnd();
	}

	//return to original matrices
	glPopMatrix();

	projection();
}

//put a vertex at the given position with the appropriate color in the HSV circle
void wave::setColoredVertex(float s, int h)
{
	float r, g, b;
	HSVtoRGB(&r, &g, &b, h, s, 1.0);
	glColor3f(r, g, b);
	glVertex3f(s * cos(h* M_PI / 180.), s * sin(h * M_PI / 180.), 0.0);
}

void wave::showHelp()
{
	//disable Z buffer to insure the text to be in front of everything else
	glDisable(GL_DEPTH_TEST);

	//set up the projection matrix for two-dimensional stuff
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1.0 - config->bgred(), 1.0 - config->bggreen(), 1.0 - config->bgblue());

	//fill a vector with the help text
	std::vector<std::string> help;
	help.push_back("Help:");
	help.push_back("F1: show / hide this help");
	help.push_back("space: start / stop animation");
	help.push_back("right: next frame");
	help.push_back("left: previous frame");
	help.push_back("o: enable / disable overlays");
	help.push_back("up / down: previous / next overlay");
	help.push_back("q: quit program");
	help.push_back("left mouse button: rotate view");
	help.push_back("middle mouse button: zoom view");
	help.push_back("right mouse button: set new camera target");
	help.push_back("ctrl + left mouse button: show streakline (only if 2D overlay field enabled)");

	int y = 30;
	int i = 0;
	//draw the help text line by line
	while (i < help.size())
	{
		drawString(50, y, help[i]);
		i++;
		y += 20;
	}

}

void wave::showFrameNumber()
{

	//disable Z buffer to insure the text to be in front of everything else
	glDisable(GL_DEPTH_TEST);

	//set up the projection matrix for two-dimensional stuff
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1.0 - config->bgred(), 1.0 - config->bggreen(), 1.0 - config->bgblue());

	std::stringstream frame;
	frame << currentFrameNumber;

	//draw frame number in the lower left corner
	drawString(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, frame.str());

	glPopMatrix();
}

//draw a string at the given position
void wave::drawString(double x, double y, std::string string) {
	int length, i;
	glRasterPos2f(x, y);
	length = string.length();
	for (i = 0; i < length; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
}

//main rendering funtion
void wave::render()
{
	//save the current matrix for later use
  glPushMatrix();

	//scale in z direction according to config object
  glScaled(1.0, 1.0, config->scaling());


	//draw wave surface

	//enable lighting
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

	//put light0 at this position
  GLfloat position[] = { -xRes / 4., -yRes / 4., yRes / 2., 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, position);

	//here the wave is drawn in strips along the x axis
  for (int i = 0; i < yRes - 1; ++i)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < xRes; ++j)
		{
			//draw two points at (x,y) = (j,i) and (x,y) = (j+1,i)
// 			std::cout << height[i*xRes+j] << std::endl;
      setColor(i * xRes + j);
			//normal vector for the given slope values
			//will be normalized automatically
      glNormal3f( - slopeX[i * xRes + j],
                  - slopeY[i * xRes + j],
                 1.0);
			//point at (x,y) = (j,i)
			glVertex3f(j, i, height[i * xRes + j]);

      setColor((i+1) * xRes + j);
      glNormal3f( - slopeX[(i+1) * xRes + j],
                  - slopeY[(i+1) * xRes + j],
                 1.0);
			//point at (x,y) = (j+1,i)
			glVertex3f(j, (i+1), height[(i+1) * xRes + j]);		}
		glEnd();
	}

	//draw streakline

	if (streaklineVisible)
	{
		//no lighting needed here
		glDisable(GL_LIGHTING);
		glColor3d(1.0, 1.0, 1.0);
		//three pixels thick line
		glLineWidth(3);
	
		//loop over the streakline vector and connect the points with straight lines
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < streakline_forward.size(); ++i)
		{
			glVertex3f(streakline_forward[i][0], streakline_forward[i][1],
								height[(int) streakline_forward[i][1] * xRes + (int) streakline_forward[i][0]] + 1);
		}
		glEnd();

		//do again for the backward direction
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < streakline_backward.size(); ++i)
		{
			glVertex3f(streakline_backward[i][0], streakline_backward[i][1],
								height[(int) streakline_backward[i][1] * xRes + (int) streakline_backward[i][0]] + 1);
		}
		glEnd();
	}

	//draw "water box"
	//lighting and blending are needed here
	glEnable(GL_LIGHTING);
  glEnable(GL_BLEND);

	//a blue color with an alpha value of 0.5
  glColor4d(0.2, 0.2, 1.0, 0.5);


	//next the different borders of the water box have to be drawn in the correct order
	//which depends on the camera position
  if (cameraZ > config->ground())
    drawBorder(BOTTOM);

  if (((cameraX <= xRes / 2.) && (cameraY <= yRes / 2.)
     && (cameraX >= 0.) && (cameraY >= 0.))
     || ((cameraX < 0.) && (cameraY < 0.)))
  {
    drawBorder(BACK);
    drawBorder(RIGHT);
    drawBorder(FRONT);
    drawBorder(LEFT);
  }

  if (((cameraX > xRes / 2.) && (cameraY <= yRes / 2.)
     && (cameraX <= xRes - 1) && (cameraY >= 0.))
     || ((cameraX > xRes - 1) && (cameraY < 0.)))
  {
    drawBorder(BACK);
    drawBorder(LEFT);
    drawBorder(FRONT);
    drawBorder(RIGHT);
  }

  if (((cameraX > xRes / 2.) && (cameraY > yRes / 2.)
     && (cameraX <= xRes - 1) && (cameraY <= yRes - 1))
     || ((cameraX > xRes - 1) && (cameraY > yRes - 1)))
  {
    drawBorder(FRONT);
    drawBorder(LEFT);
    drawBorder(BACK);
    drawBorder(RIGHT);
  }

  if (((cameraX <= xRes / 2.) && (cameraY > yRes / 2.)
     && (cameraX >= 0.) && (cameraY <= yRes - 1))
     || ((cameraX < 0.) && (cameraY > yRes - 1)))
  {
    drawBorder(FRONT);
    drawBorder(RIGHT);
    drawBorder(BACK);
    drawBorder(LEFT);
  }

  if ((cameraX >= 0.) && (cameraX <= xRes - 1) && (cameraY < 0.))
  {
    drawBorder(BACK);
    drawBorder(RIGHT);
    drawBorder(LEFT);
    drawBorder(FRONT);
  }

  if ((cameraX >= 0.) && (cameraX <= xRes - 1) && (cameraY > yRes - 1))
  {
    drawBorder(FRONT);
    drawBorder(RIGHT);
    drawBorder(LEFT);
    drawBorder(BACK);
  }

  if ((cameraX < 0.) && (cameraY >= 0.) && (cameraY <= yRes - 1))
  {
    drawBorder(RIGHT);
    drawBorder(BACK);
    drawBorder(FRONT);
    drawBorder(LEFT);
  }

  if ((cameraX > xRes - 1) && (cameraY >= 0.) && (cameraY <= yRes - 1))
  {
    drawBorder(LEFT);
    drawBorder(BACK);
    drawBorder(FRONT);
    drawBorder(RIGHT);
  }

  if (cameraZ <= config->ground())
    drawBorder(BOTTOM);

  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  glDisable(GL_BLEND);

	//restore original matrix
  glPopMatrix();
}

//draw the borders of the "water box"
void wave::drawBorder(BorderNumberType number)
{
  switch (number)
  {
  case LEFT:
		//draw the border as a triangle strip from config->ground() to surface of the wave
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i < yRes; ++i)
    {
      glNormal3f(-1.0, 0.0, 0.0);
      glVertex3f(0, i, config->ground());
      glVertex3f(0, i, height[i * xRes]);
    }
    glEnd();
    break;


  case RIGHT:
		//draw the border as a triangle strip from config->ground() to surface of the wave
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i < yRes; ++i)
    {
      glNormal3f(1.0, 0.0, 0.0);
      glVertex3f(xRes - 1, i, config->ground());
      glVertex3f(xRes - 1, i, height[i * xRes + xRes - 1]);
    }
    glEnd();
    break;

  case FRONT:
		//draw the border as a triangle strip from config->ground() to surface of the wave
    glBegin(GL_TRIANGLE_STRIP);
    for (int j = 0; j < xRes; ++j)
    {
      glNormal3f(0.0, -1.0, 0.0);
      glVertex3f(j, 0.0, config->ground());
      glVertex3f(j, 0.0, height[j]);
    }
    glEnd();
    break;

  case BACK:
		//draw the border as a triangle strip from config->ground() to surface of the wave
    glBegin(GL_TRIANGLE_STRIP);
    for (int j = 0; j < xRes; ++j)
    {
      glNormal3f(0.0, 1.0, 0.0);
      glVertex3f(j, yRes - 1, config->ground());
      glVertex3f(j, yRes - 1, height[(yRes - 1) * xRes + j]);
    }
    glEnd();
    break;

  case BOTTOM:
		//draw the bottom rectangle of the box
    glBegin(GL_QUADS);
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(0.0,0.0,config->ground());
    glVertex3f(0.0,yRes-1,config->ground());
    glVertex3f(xRes-1,yRes-1,config->ground());
    glVertex3f(xRes-1,0.0,config->ground());
    glEnd();
    break;

  default: break;
  }
}

void wave::setColor(long i)
{
	if (overlayEnabled)
	  switch (overlayDataSource->type)
    {
      case FIELD_1D:
				//look up the color in the colormap if we have a scalar field
        float buffer[3];
		    overlayDataSource->colormap->lookUp(overlayDim1[i], buffer);
		    glColor3fv(buffer);
        break;
      case FIELD_2D:
				//If we have a two-dimensional field use the direction as hue and the vector length
				//as saturation in the HSV color model. Value is 1.0.
        float saturation, hue;
        saturation = sqrt(pow(overlayDim1[i],2) + pow(overlayDim2[i],2));
        hue = (overlayDim2[i] >= 0 ? acos(overlayDim1[i]/saturation) : 2 * M_PI - acos(overlayDim1[i]/saturation));
				//radian -> degrees
        hue *= 180 / M_PI;
				//avoid strange effects if, for some reason, the vector is longer than one
				if (saturation > 1.0) saturation = 1.0;
        float r, g, b;
        HSVtoRGB(&r, &g, &b, hue, saturation, 1.0);
        glColor3d(r, g, b);
        
        break;
	  }
	//if no overlay enabled use this color
  else
    glColor3d(0.2, 0.2, 1.0);
}

//convert HSV to RGB colors
void wave::HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}

}

