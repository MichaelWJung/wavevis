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

//This class is the base class of the wave object. It is used to call the
//appropriate functions if there is a glut callback event.

#ifndef _GLUTWINDOW_H_
#define _GLUTWINDOW_H_

#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <math.h>
#include <map>
#include <list>
#include <vector>

#ifndef M_PI
#define M_PI 3.1415926535
#endif

class glutWindow
{
protected:
	//used to store all instances of the wave class
	static std::map <int, glutWindow*> windowIDToWindow;

	//function pointers to these functions are used for
	// glutDisplayFunc, glutKeyboardFunc, glutMouseFunc, etc.
	static void displayDispatch(void);
	static void reshapeDispatch(int width, int height);
	static void keyboardDispatch(unsigned char key, int x, int y);
	static void keyboardSpecialDispatch(int key, int x, int y);
	static void mouseDispatch(int button, int state, int x, int y);
	static void motionDispatch(int x, int y);
	static void animationDispatch(int i);
	static void cameraFlightDispatch(int i);
	static void idleDispatch();

	int windowID;

public:
	virtual void display(void) = 0;
	virtual void reshape(int width, int height) = 0;
	virtual void keyboard(unsigned char key, int x, int y) = 0;
	virtual void keyboardSpecial(int key, int x, int y) = 0;
	virtual void mouse(int button, int state, int x, int y) = 0;
	virtual void motion(int x, int y) = 0;
	virtual void animation(int i) = 0;
	virtual void cameraFlightFunction(int i) = 0;
	virtual void idle() = 0;

	glutWindow();
	~glutWindow();
};

#endif
