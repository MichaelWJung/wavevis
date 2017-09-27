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

// This is the DataSource base class. If you want to use another data source
// than the one that comes with this program you can create a child class
// and pass instances of that class to the config object.

#ifndef _DATASOURCE_H_
#define _DATASOURCE_H_


class DataSource
{
	protected:
	bool _inUse;

	public:
	DataSource();

	enum DataSourceType {ONE_DIMENSIONAL, TWO_DIMENSIONAL};

	DataSourceType type;

	virtual long nextFrame(float* buffer, int n);
	virtual long nextFrame(float* buffer1, float* buffer2, int n);
	virtual long nextFrame(int n);

	virtual long currentFrame(float* buffer) = 0;
	virtual long currentFrame(float*, float*) = 0;

	virtual long nextFrame(float* buffer) = 0;
	virtual long nextFrame(float*, float*) = 0;
	virtual long nextFrame() = 0;	

	virtual long prevFrame(float* buffer) = 0;
	virtual long prevFrame(float*, float*) = 0;
	virtual long prevFrame() = 0;

	virtual long firstFrame() = 0;

	virtual void idle(int a);

	virtual bool inUse();
	virtual void inUse(bool);

	//this function has to give back some sort of interpolation between the points if the
	//data source is a two dimensional one
	virtual void interpolate(float x, float y, float* vx, float* vy) = 0;
};

#endif
 