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

#include "glutwindow.h"

std::map<int, glutWindow*> glutWindow::windowIDToWindow;

glutWindow::glutWindow()
{
	//create window and add it to the window map
	windowID = glutCreateWindow ("Wave Visualization");
	windowIDToWindow[windowID] = this;
}

glutWindow::~glutWindow()
{
	windowIDToWindow.erase(windowID);
	glutDestroyWindow(windowID);
}

void glutWindow::displayDispatch()
{
	int winID = glutGetWindow();
	windowIDToWindow[winID]->display();
}

void glutWindow::reshapeDispatch(int width, int height)
{
	int winID = glutGetWindow();
	windowIDToWindow[winID]->reshape(width, height);
}

void glutWindow::keyboardDispatch(unsigned char key, int x, int y)
{
	int winID = glutGetWindow();
	windowIDToWindow[winID]->keyboard(key, x, y);
}

void glutWindow::keyboardSpecialDispatch(int key, int x, int y)
{
	int winID = glutGetWindow();
	windowIDToWindow[winID]->keyboardSpecial(key, x, y);
}

void glutWindow::mouseDispatch(int button, int state, int x, int y)
{
	int winID = glutGetWindow();
	windowIDToWindow[winID]->mouse(button, state, x, y);
}

void glutWindow::motionDispatch(int x, int y)
{
	int winID = glutGetWindow();
	windowIDToWindow[winID]->motion(x, y);
}

void glutWindow::animationDispatch(int i)
{
	int winID = glutGetWindow();
	windowIDToWindow[winID]->animation(i);
}

void glutWindow::cameraFlightDispatch(int i)
{
	int winID = glutGetWindow();
	windowIDToWindow[winID]->cameraFlightFunction(i);
}

void glutWindow::idleDispatch()
{
	int winID = glutGetWindow();
	windowIDToWindow[winID]->idle();
}
