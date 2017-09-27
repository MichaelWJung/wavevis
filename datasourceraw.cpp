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

#include "datasourceraw.h"

//standard constructor for a height or slope file
DataSourceRaw::DataSourceRaw(char* filename, int x, int y, int numberOfFrames)
{
  type = ONE_DIMENSIONAL;
  
	currentFrameNumber = 0;
	xRes = x;
	yRes = y;
	frames = numberOfFrames;

  FILE * pFile = fopen(filename, "rb");

  if (pFile == NULL)
	{
		std::cout << "Error: Can't open file " << filename << "\n";
		exit(1);
	}

  data1 = (float*) malloc(sizeof(float) * y * x * frames);
  if (data1 == NULL) {std::cout << "Memory error\n"; exit (2);}

  fread(data1, sizeof(float), y * x * frames, pFile);

  fclose(pFile);
}

//This one scales the data. They are all between 1.0 and 0.0 afterward. The former min and max
//values are put into the strings
DataSourceRaw::DataSourceRaw(char* filename, int x, int y, int numberOfFrames, std::string* min, std::string* max)
{
  type = ONE_DIMENSIONAL;
  
	currentFrameNumber = 0;
	xRes = x;
	yRes = y;
	frames = numberOfFrames;

  FILE * pFile = fopen(filename, "rb");

  if (pFile == NULL)
	{
		std::cout << "Error: Can't open file " << filename << "\n";
		exit(1);
	}

  data1 = (float*) malloc(sizeof(float) * y * x * frames);
  if (data1 == NULL) {std::cout << "Memory error\n"; exit (2);}

  fread(data1, sizeof(float), y * x * frames, pFile);

	//float number 0 is skipped because this one will be used for other purposes sometimes
	float minValue = data1[1];
	float maxValue = data1[1];

	//look for min / max values
	for (long i = 1; i < yRes * xRes * frames; ++i)
	{
		if (data1[i] > maxValue) maxValue = data1[i];
		if (data1[i] < minValue) minValue = data1[i];
	}

	//scale the values to be between 0.0 and 1.0
	if (!(minValue == data1[1] && maxValue == data1[1]))
		for (long i = 0; i < yRes * xRes * frames; ++i)
			data1[i] = (data1[i] - minValue) / (maxValue - minValue);

	std::stringstream _min, _max;
	_min << minValue;
	_max << maxValue;
	*min = _min.str();
	*max = _max.str();

  fclose(pFile);
}

//This one scales according to the two given float values. Values < min or > max will be set to
//0.0 or 1.0 accordingly
DataSourceRaw::DataSourceRaw(char* filename, int x, int y, int numberOfFrames, float min, float max, std::string* minString, std::string* maxString)
{
  type = ONE_DIMENSIONAL;
  
	currentFrameNumber = 0;
	xRes = x;
	yRes = y;
	frames = numberOfFrames;

  FILE * pFile = fopen(filename, "rb");

  if (pFile == NULL)
	{
		std::cout << "Error: Can't open file " << filename << "\n";
		exit(1);
	}

  data1 = (float*) malloc(sizeof(float) * y * x * frames);
  if (data1 == NULL) {std::cout << "Memory error\n"; exit (2);}

  fread(data1, sizeof(float), y * x * frames, pFile);

	//scale the values to be between 0.0 and 1.0
	for (long i = 1; i < yRes * xRes * frames; ++i)
	{
		data1[i] = (data1[i] - min) / (max - min);
		if (data1[i] < 0.0) data1[i] = 0.0;
		if (data1[i] > 1.0) data1[i] = 1.0;
	}

	std::stringstream _min, _max;
	_min << min;
	_max << max;
	*minString = _min.str();
	*maxString = _max.str();
  
  fclose(pFile);
}

//This is the contructor to be used for two dimensional data
DataSourceRaw::DataSourceRaw(char* file1, char* file2, int x, int y, int numberOfFrames, float longestVector)
{
  type = TWO_DIMENSIONAL;

  currentFrameNumber = 0;
	xRes = x;
	yRes = y;
	frames = numberOfFrames;

  FILE * pFile = fopen(file1, "rb");

  if (pFile == NULL)
	{
		std::cout << "Error: Can't open file " << file1 << "\n";
		exit(1);
	}

  data1 = (float*) malloc(sizeof(float) * y * x * frames);
  if (data1 == NULL) {std::cout << "Memory error\n"; exit (2);}

  fread(data1, sizeof(float), y * x * frames, pFile);

  fclose(pFile);

  pFile = fopen(file2, "rb");

  if (pFile == NULL)
	{
		std::cout << "Error: Can't open file " << file2 << "\n";
		exit(1);
	}


  data2 = (float*) malloc(sizeof(float) * y * x * frames);
  if (data2 == NULL) {std::cout << "Memory error\n"; exit (2);}

  fread(data2, sizeof(float), y * x * frames, pFile);

  fclose(pFile);  

	//if no vector length is given, look for the longest one
  if (longestVector == 0.0)
  {
    float maxLength = 0.0;
    for (long i = 0; i < frames * xRes * yRes; ++i)
      if (maxLength < pow(data1[i],2) + pow(data2[i],2))
        maxLength = pow(data1[i],2) + pow(data2[i],2);
 
    scalingFactor = 1. / sqrt(maxLength);
  }
	else
		scalingFactor = 1. / longestVector;

	//scale by scalingFactor
	for (long i = 0; i < frames * xRes * yRes; ++i)
	{
		data1[i] *= scalingFactor;
		data2[i] *= scalingFactor;
	}

}

long DataSourceRaw::currentFrame(float* buffer)
{
  memcpy(buffer, &data1[currentFrameNumber * xRes * yRes], sizeof(float) * xRes * yRes);
	return currentFrameNumber;
}

long DataSourceRaw::currentFrame(float* buffer1, float* buffer2)
{
  memcpy(buffer1, &data1[currentFrameNumber * xRes * yRes], sizeof(float) * xRes * yRes);
  memcpy(buffer2, &data2[currentFrameNumber * xRes * yRes], sizeof(float) * xRes * yRes);
	return currentFrameNumber;
}

long DataSourceRaw::nextFrame(float* buffer)
{
	currentFrameNumber = (currentFrameNumber + 1) % frames;
  memcpy(buffer, &data1[currentFrameNumber * xRes * yRes], sizeof(float) * xRes * yRes);
	return currentFrameNumber;
}

long DataSourceRaw::nextFrame(float* buffer1, float* buffer2)
{
	currentFrameNumber = (currentFrameNumber + 1) % frames;
  memcpy(buffer1, &data1[currentFrameNumber * xRes * yRes], sizeof(float) * xRes * yRes);
  memcpy(buffer2, &data2[currentFrameNumber * xRes * yRes], sizeof(float) * xRes * yRes);
	return currentFrameNumber;
}

long DataSourceRaw::nextFrame()
{
	currentFrameNumber = (currentFrameNumber + 1) % frames;
	return currentFrameNumber;
}

long DataSourceRaw::prevFrame(float* buffer)
{
	currentFrameNumber = (currentFrameNumber + frames - 1) % frames;
  memcpy(buffer, &data1[currentFrameNumber * xRes * yRes], sizeof(float) * xRes * yRes);
	return currentFrameNumber;
}

long DataSourceRaw::prevFrame(float* buffer1, float* buffer2)
{
	currentFrameNumber = (currentFrameNumber + frames - 1) % frames;
  memcpy(buffer1, &data1[currentFrameNumber * xRes * yRes], sizeof(float) * xRes * yRes);
  memcpy(buffer2, &data2[currentFrameNumber * xRes * yRes], sizeof(float) * xRes * yRes);
	return currentFrameNumber;
}

long DataSourceRaw::prevFrame()
{
	currentFrameNumber = (currentFrameNumber + frames - 1) % frames;
	return currentFrameNumber;
}

long DataSourceRaw::firstFrame()
{
  currentFrameNumber = 0;
	return currentFrameNumber;
}

//bilinear interpolation between the points
//here scalingFactor is used to get back the original values for the streakline calculations
void DataSourceRaw::interpolate(float x, float y, float* vx, float* vy)
{
	float v1, v2, v3, v4, va, vb;
	v1 = data1[currentFrameNumber * xRes * yRes + (int) y * xRes + (int) x];
	v2 = data1[currentFrameNumber * xRes * yRes + (int) y * xRes + (int) x + 1];
	v3 = data1[currentFrameNumber * xRes * yRes + (int) (y + 1) * xRes + (int) x];
	v4 = data1[currentFrameNumber * xRes * yRes + (int) (y + 1) * xRes + (int) x + 1];
	va = linearInterpolation(x - (double)((int) x), v1, v2);
	vb = linearInterpolation(x - (double)((int) x), v3, v4);
	*vx = linearInterpolation(y - (double)((int) y), va, vb) / scalingFactor;

	v1 = data2[currentFrameNumber * xRes * yRes + (int) y * xRes + (int) x];
	v2 = data2[currentFrameNumber * xRes * yRes + (int) y * xRes + (int) x + 1];
	v3 = data2[currentFrameNumber * xRes * yRes + (int) (y + 1) * xRes + (int) x];
	v4 = data2[currentFrameNumber * xRes * yRes + (int) (y + 1) * xRes + (int) x + 1];
	va = linearInterpolation(x - (double)((int) x), v1, v2);
	vb = linearInterpolation(x - (double)((int) x), v3, v4);
	*vy = linearInterpolation(y - (double)((int) y), va, vb) / scalingFactor;
}

float DataSourceRaw::linearInterpolation(float h, float v1, float v2)
{
	return h*(v2 - v1) + v1;
}

