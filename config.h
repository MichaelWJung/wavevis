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

/*
	This is the config class. An instance of this class has to be passed
	to the main program.
*/


#ifndef _WAVE_CONFIG_H_
#define _WAVE_CONFIG_H_

#include "datasource.h"
#include "glutwindow.h"
#include "colormap.h"

enum DataSourceType {HEIGHT, SLOPE_X, SLOPE_Y, FIELD_1D, FIELD_2D};

class waveConfig
{
  public:
	//these are the elements of the data source list
	struct sourceTableEntry
	{
		DataSource* data;
		DataSourceType type;
		colorMap* colormap;
	};
	
	//these are the elements of the camera flight list
	struct cameraPoint
	{
		float x, y, z;
		float tx, ty;
		long duration;
	};

	typedef void(*idleFuncPointer)(waveConfig*);

  waveConfig();
  ~waveConfig();
  int xRes();
	void xRes(int);
  int yRes();
	void yRes(int);
	int frames();
	void frames(int);
	int framesPerSec();
	void framesPerSec(int);
  double scaling();
  void scaling(double);
  double ground();
  void ground(double);
  double bgred();
  void bgred(double);
  double bggreen();
  void bggreen(double);
  double bgblue();
  void bgblue(double);
  bool freeCamera();
  void freeCamera(bool);
  bool flightEnabled();
  void flightEnabled(bool);
  bool animationEnabled();
  void animationEnabled(bool);
	void idleFunc(idleFuncPointer);
	idleFuncPointer idleFunc(void);

	std::list <sourceTableEntry> dataList;
	std::list <cameraPoint> cameraFlight;

  protected:
  int _xRes, _yRes, _frames, _framesPerSec;
  bool _freeCamera, _flightEnabled, _animationEnabled;
  double _scaling, _ground, _bgRed, _bgGreen, _bgBlue;
	idleFuncPointer _idleFunc;
};


#endif
