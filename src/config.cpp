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

#include "config.h"
#include "datasource.h"
#include "datasourceraw.h"
#include "colormap.h"
#include "colormapasc.h"

//set default values
waveConfig::waveConfig()
{
  _freeCamera = true;
  _flightEnabled = false;
  _animationEnabled = false;
	_bgRed = 0.0;
	_bgGreen = 0.0;
	_bgBlue = 0.0;
	_idleFunc = NULL;
}

//set / get values
int waveConfig::xRes()
{
  return _xRes;
}

void waveConfig::xRes(int i)
{
	_xRes = i;
}

int waveConfig::yRes()
{
  return _yRes;
}

void waveConfig::yRes(int i)
{
	_yRes = i;
}

int waveConfig::frames()
{
  return _frames;
}

void waveConfig::frames(int i)
{
	_frames = i;
}

int waveConfig::framesPerSec()
{
  return _framesPerSec;
}

void waveConfig::framesPerSec(int i)
{
	_framesPerSec = i;
}

double waveConfig::scaling()
{
  return _scaling;
}

void waveConfig::scaling(double i)
{
  _scaling = i;
}

double waveConfig::ground()
{
  return _ground;
}

void waveConfig::ground(double i)
{
  _ground = i;
}

double waveConfig::bgred()
{
	return _bgRed;
}

void waveConfig::bgred(double i)
{
	_bgRed = i;
}

double waveConfig::bggreen()
{
	return _bgGreen;
}

void waveConfig::bggreen(double i)
{
	_bgGreen = i;
}

double waveConfig::bgblue()
{
	return _bgBlue;
}

void waveConfig::bgblue(double i)
{
	_bgBlue = i;
}

bool waveConfig::freeCamera()
{
  return _freeCamera;
}

void waveConfig::freeCamera(bool i)
{
  _freeCamera = i;
}

bool waveConfig::flightEnabled()
{
  return _flightEnabled;
}

void waveConfig::flightEnabled(bool i)
{
  _flightEnabled = i;
}

bool waveConfig::animationEnabled()
{
  return _animationEnabled;
}

void waveConfig::animationEnabled(bool i)
{
  _animationEnabled = i;
}

void waveConfig::idleFunc(idleFuncPointer pointer)
{
	_idleFunc = pointer;
}

waveConfig::idleFuncPointer waveConfig::idleFunc(void)
{
	return _idleFunc;
}

