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

#ifndef _DATASOURCERAW_H_
#define _DATASOURCERAW_H_

#include "datasource.h"
#include "glutwindow.h"

class DataSourceRaw: public DataSource
{
	private:

	float* data1, * data2;		//contain the complete data of the raw files
	long currentFrameNumber;
	int xRes, yRes;
	int frames;		//number of frames
	float scalingFactor;		//factor by which the two dimensional vector field has been scaled
	float linearInterpolation(float h, float v1, float v2);

  public:
	long currentFrame(float* buffer);
	long currentFrame(float*, float*);
	
	long nextFrame(float* buffer);
	long nextFrame(float*, float*);
	long nextFrame();
	
	long prevFrame(float* buffer);
	long prevFrame(float*, float*);
	long prevFrame();

	long firstFrame();
	
	void interpolate(float x, float y, float* vx, float* vy);

	//standard constructor for a height or slope file
	DataSourceRaw(char* filename, int x, int y, int numberOfFrames);

	//This one scales the data. They are all between 1.0 and 0.0 afterward. The former min and max
	//values are put into the strings
	DataSourceRaw(char* filename, int x, int y, int numberOfFrames, std::string* min, std::string* max);

	//This one scales according to the two given float values. Values < min or > max will be set to
	//0.0 or 1.0 accordingly
	DataSourceRaw(char* filename, int x, int y, int numberOfFrames, float min, float max, std::string* minString, std::string* maxString);

	//This is the contructor to be used for two dimensional data
	DataSourceRaw(char* file1, char* file2, int x, int y, int numberOfFrames, float longestVector);
};

#endif

