/***************************************************************************
 *   Copyright (C) 2008 by Michael Jung   *
 *   michael.jung@mail.ru   *
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


#ifndef _DATASOURCERAWRINGBUFFER_H_
#define _DATASOURCERAWRINGBUFFER_H_

#include "datasource.h"
#include "glutwindow.h"

class DataSourceRawRingBuffer: public DataSource
{
	private:

	float *data1, *data2;		//ring buffers for the data
	//number of the first / last frame currently in buffer
	long lastFrameInBuffer;
	//position of the current frame in the buffer
	long currentFramePosition;
	//size of the buffer
	long bufferSize;

	FILE *pFile1, *pFile2;

	long currentFrameNumber;
	int xRes, yRes;
	int frames;		//number of frames
	float scalingFactor;		//factor by which the two dimensional vector field has been scaled
	float linearInterpolation(float h, float v1, float v2);
	//fills the buffer
	void load();
	//loads previous frame back into buffer
	void oneFrameBack();
	bool scalingEnabled;
	float maxValue, minValue;

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

	void idle(int a);
	void inUse(bool i);

	//standard constructor for a height or slope file
	DataSourceRawRingBuffer(char* filename, int x, int y, int numberOfFrames, int size);

	//This one scales according to the two given float values. Values < min or > max will be set to
	//0.0 or 1.0 accordingly
	DataSourceRawRingBuffer(char* filename, int x, int y, int numberOfFrames, float min, float max, int size);

	//This is the contructor to be used for two dimensional data
	DataSourceRawRingBuffer(char* file1, char* file2, int x, int y, int numberOfFrames, float longestVector, int size);

	~DataSourceRawRingBuffer();
};

#endif

