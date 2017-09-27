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

#include "datasourcerawringbuffer.h"

DataSourceRawRingBuffer::~DataSourceRawRingBuffer()
{
	free(data1);
	free(data2);
	fclose(pFile1);
}

//standard constructor for a height or slope file
DataSourceRawRingBuffer::DataSourceRawRingBuffer(char* filename, int x, int y, int numberOfFrames, int size)
{
  type = ONE_DIMENSIONAL;

	scalingEnabled = false;

	xRes = x;
	yRes = y;
	bufferSize = size;
	currentFramePosition = 0;
	currentFrameNumber = 0;
	frames = numberOfFrames;

	pFile1 = fopen(filename, "rb");

  if (pFile1 == NULL)
	{
		std::cout << "Error: Can't open file " << filename << "\n";
		exit(1);
	}
}

//This one scales according to the two given float values. Values < min or > max will be set to
//0.0 or 1.0 accordingly
DataSourceRawRingBuffer::DataSourceRawRingBuffer(char* filename, int x, int y, int numberOfFrames, float min, float max, int size)
{
  type = ONE_DIMENSIONAL;

	scalingEnabled = true;
	minValue = min;
	maxValue = max;

	xRes = x;
	yRes = y;
	bufferSize = size;
	currentFramePosition = 0;
	currentFrameNumber = 0;
	frames = numberOfFrames;
	
	pFile1 = fopen(filename, "rb");

  if (pFile1 == NULL)
	{
		std::cout << "Error: Can't open file " << filename << "\n";
		exit(1);
	}
}

//This is the contructor to be used for two dimensional data
DataSourceRawRingBuffer::DataSourceRawRingBuffer(char* file1, char* file2, int x, int y, int numberOfFrames, float longestVector, int size)
{
  type = TWO_DIMENSIONAL;

	xRes = x;
	yRes = y;
	bufferSize = size;
	currentFramePosition = 0;
	currentFrameNumber = 0;
	frames = numberOfFrames;

  pFile1 = fopen(file1, "rb");

  if (pFile1 == NULL)
	{
		std::cout << "Error: Can't open file " << file1 << "\n";
		exit(0);
	}

  pFile2 = fopen(file2, "rb");

  if (pFile2 == NULL)
	{
		std::cout << "Error: Can't open file " << file2 << "\n";
		exit(0);
	}

	scalingFactor = 1. / longestVector;
}

//if this data source is put into use, fill the buffer
//if no longer needed, free the memory
void DataSourceRawRingBuffer::inUse(bool i)
{
	_inUse = i;
	if (i)
		load();
	else
	{
		free(data1);
		free(data2);
	}
}

//if new data has to be loaded into the buffer load one frame and scale it
void DataSourceRawRingBuffer::idle(int a)
{
	if(_inUse)
	{
		if (lastFrameInBuffer - currentFrameNumber + 1 < bufferSize && ++lastFrameInBuffer < frames)
		{
			long j;
			switch (type)
			{
				case ONE_DIMENSIONAL:
					fseek(pFile1, sizeof(float) * ((lastFrameInBuffer + 1)%frames) * xRes * yRes, SEEK_SET);
					j = ((currentFramePosition + lastFrameInBuffer - currentFrameNumber) % bufferSize) * xRes * yRes;
					fread(&data1[j], sizeof(float), xRes * yRes, pFile1);
					if (scalingEnabled)
						for (long i = 0; i < xRes * yRes; ++i)
						{
							data1[j+i] = (data1[j+i] - minValue) / (maxValue - minValue);
							if (data1[j+i] < 0.0) data1[j+i] = 0.0;
							if (data1[j+i] > 1.0) data1[j+i] = 1.0;
						}
					break;
				case TWO_DIMENSIONAL:
					fseek(pFile1, sizeof(float) * ((lastFrameInBuffer + 1)%frames) * xRes * yRes, SEEK_SET);
					fseek(pFile2, sizeof(float) * ((lastFrameInBuffer + 1)%frames) * xRes * yRes, SEEK_SET);
					j = ((currentFramePosition + lastFrameInBuffer - currentFrameNumber) % bufferSize) * xRes * yRes;
					fread(&data1[j], sizeof(float), xRes * yRes, pFile1);
					fread(&data2[j], sizeof(float), xRes * yRes, pFile2);
					for (long i = 0; i < xRes * yRes; ++i)
					{
						data1[j+i] *= scalingFactor;
						data2[j+i] *= scalingFactor;
					}
					break;
			}
		}
	}
}

//load back the previous frame and scale it
void DataSourceRawRingBuffer::oneFrameBack()
{
	long j;
	switch (type)
	{
		case ONE_DIMENSIONAL:
			fseek(pFile1, sizeof(float) * ((currentFrameNumber + frames - 1)%frames) * xRes * yRes, SEEK_SET);
			j = ((currentFramePosition + bufferSize - 1) % bufferSize) * xRes * yRes;
			fread(&data1[j], sizeof(float), xRes * yRes, pFile1);
			if (scalingEnabled)
				for (long i = 0; i < xRes * yRes; ++i)
				{
					data1[j+i] = (data1[j+i] - minValue) / (maxValue - minValue);
					if (data1[j+i] < 0.0) data1[j+i] = 0.0;
					if (data1[j+i] > 1.0) data1[j+i] = 1.0;
				}
			break;
		case TWO_DIMENSIONAL:
			fseek(pFile1, sizeof(float) * ((currentFrameNumber + frames - 1)%frames) * xRes * yRes, SEEK_SET);
			fseek(pFile2, sizeof(float) * ((currentFrameNumber + frames - 1)%frames) * xRes * yRes, SEEK_SET);
			j = ((currentFramePosition + bufferSize - 1) % bufferSize) * xRes * yRes;
			fread(&data1[j], sizeof(float), xRes * yRes, pFile1);
			fread(&data2[j], sizeof(float), xRes * yRes, pFile2);
			for (long i = 0; i < xRes * yRes; ++i)
			{
				data1[j+i] *= scalingFactor;
				data2[j+i] *= scalingFactor;
			}
			break;
	}
	if (currentFrameNumber == 0)
		lastFrameInBuffer  = frames - 1;
	if (lastFrameInBuffer - currentFrameNumber + 1 >= bufferSize)
		lastFrameInBuffer--;
}

//fill the buffer with data
void DataSourceRawRingBuffer::load()
{
	//allocate memory for the buffers
	data1 = (float*) malloc(sizeof(float) * xRes * yRes * bufferSize);
  if (data1 == NULL) {std::cout << "Memory error\n"; exit (2);}

	if (type == TWO_DIMENSIONAL)
	{
		data2 = (float*) malloc(sizeof(float) * xRes * yRes * bufferSize);
		if (data2 == NULL) {std::cout << "Memory error\n"; exit (2);}
	}

	//set file handlers to the position of the current frame in the file
	fseek(pFile1, sizeof(float) * currentFrameNumber * xRes * yRes, SEEK_SET);

	if (type == TWO_DIMENSIONAL)
		fseek(pFile2, sizeof(float) * currentFrameNumber * xRes * yRes, SEEK_SET);

	//fill the buffer
	//fill only partially if there is not enough data left in the file
	long toBeRead = bufferSize;
	if (toBeRead + currentFrameNumber > frames) toBeRead = frames - currentFrameNumber;
	fread(data1, sizeof(float), toBeRead * xRes * yRes, pFile1);
	if (type == TWO_DIMENSIONAL)
		fread(data2, sizeof(float), toBeRead * xRes * yRes, pFile2);

	//scale data
	switch (type)
	{
		case ONE_DIMENSIONAL:
			if (scalingEnabled)
				for (long i = 0; i < bufferSize * xRes * yRes; ++i)
				{
					data1[i] = (data1[i] - minValue) / (maxValue - minValue);
					if (data1[i] < 0.0) data1[i] = 0.0;
					if (data1[i] > 1.0) data1[i] = 1.0;
				}
			break;
		case TWO_DIMENSIONAL:
			for (long i = 0; i < bufferSize * xRes * yRes; ++i)
			{
				data1[i] *= scalingFactor;
				data2[i] *= scalingFactor;
			}
			break;
	}
	lastFrameInBuffer = currentFrameNumber + toBeRead - 1;
	currentFramePosition = 0;
}

long DataSourceRawRingBuffer::currentFrame(float* buffer)
{
	if (currentFrameNumber <= lastFrameInBuffer)
  	memcpy(buffer, &data1[currentFramePosition * xRes * yRes], sizeof(float) * xRes * yRes);
	return currentFrameNumber;
}

long DataSourceRawRingBuffer::currentFrame(float* buffer1, float* buffer2)
{
	if (currentFrameNumber <= lastFrameInBuffer)
  {
		memcpy(buffer1, &data1[currentFramePosition * xRes * yRes], sizeof(float) * xRes * yRes);
  	memcpy(buffer2, &data2[currentFramePosition * xRes * yRes], sizeof(float) * xRes * yRes);
	}
	return currentFrameNumber;
}

long DataSourceRawRingBuffer::nextFrame(float* buffer)
{
	currentFrameNumber = (currentFrameNumber + 1) % frames;
	currentFramePosition = (currentFramePosition + 1) % bufferSize;
	if (currentFrameNumber == 0) lastFrameInBuffer = -1;
	//if current frame is not in buffer, reload until it is
	while (currentFrameNumber > lastFrameInBuffer)
		idle(0);
	memcpy(buffer, &data1[currentFramePosition * xRes * yRes], sizeof(float) * xRes * yRes);
	return currentFrameNumber;
}

long DataSourceRawRingBuffer::nextFrame(float* buffer1, float* buffer2)
{
	currentFrameNumber = (currentFrameNumber + 1) % frames;
	currentFramePosition = (currentFramePosition + 1) % bufferSize;
	if (currentFrameNumber == 0) lastFrameInBuffer = -1;
	//if current frame is not in buffer, reload until it is
	while (currentFrameNumber > lastFrameInBuffer)
		idle(0);
  memcpy(buffer1, &data1[currentFramePosition * xRes * yRes], sizeof(float) * xRes * yRes);
  memcpy(buffer2, &data2[currentFramePosition * xRes * yRes], sizeof(float) * xRes * yRes);
	return currentFrameNumber;
}

long DataSourceRawRingBuffer::nextFrame()
{
	currentFrameNumber = (currentFrameNumber + 1) % frames;
	currentFramePosition = (currentFramePosition + 1) % bufferSize;
	while (_inUse && currentFrameNumber > lastFrameInBuffer)
		idle(0);
	return currentFrameNumber;
}

long DataSourceRawRingBuffer::prevFrame(float* buffer)
{
	oneFrameBack();
	currentFrameNumber = (currentFrameNumber + frames - 1) % frames;
	currentFramePosition = (currentFramePosition + bufferSize - 1) % bufferSize;
	memcpy(buffer, &data1[currentFramePosition * xRes * yRes], sizeof(float) * xRes * yRes);
	return currentFrameNumber;
}

long DataSourceRawRingBuffer::prevFrame(float* buffer1, float* buffer2)
{
	oneFrameBack();
	currentFrameNumber = (currentFrameNumber + frames - 1) % frames;
	currentFramePosition = (currentFramePosition + bufferSize - 1) % bufferSize;
  memcpy(buffer1, &data1[currentFramePosition * xRes * yRes], sizeof(float) * xRes * yRes);
  memcpy(buffer2, &data2[currentFramePosition * xRes * yRes], sizeof(float) * xRes * yRes);
	return currentFrameNumber;
}

long DataSourceRawRingBuffer::prevFrame()
{
	currentFrameNumber = (currentFrameNumber + frames - 1) % frames;
	return currentFrameNumber;
}

long DataSourceRawRingBuffer::firstFrame()
{
  currentFrameNumber = 0;
	if (data1 != NULL)
		free(data1);
	if (data2 != NULL)
		free(data2);
	load();
	return currentFrameNumber;
}

//bilinear interpolation between the points
//here scalingFactor is used to get back the original values for the streakline calculations
void DataSourceRawRingBuffer::interpolate(float x, float y, float* vx, float* vy)
{
	float v1, v2, v3, v4, va, vb;
	v1 = data1[currentFramePosition * xRes * yRes + (int) y * xRes + (int) x];
	v2 = data1[currentFramePosition * xRes * yRes + (int) y * xRes + (int) x + 1];
	v3 = data1[currentFramePosition * xRes * yRes + (int) (y + 1) * xRes + (int) x];
	v4 = data1[currentFramePosition * xRes * yRes + (int) (y + 1) * xRes + (int) x + 1];
	va = linearInterpolation(x - (double)((int) x), v1, v2);
	vb = linearInterpolation(x - (double)((int) x), v3, v4);
	*vx = linearInterpolation(y - (double)((int) y), va, vb) / scalingFactor;

	v1 = data2[currentFramePosition * xRes * yRes + (int) y * xRes + (int) x];
	v2 = data2[currentFramePosition * xRes * yRes + (int) y * xRes + (int) x + 1];
	v3 = data2[currentFramePosition * xRes * yRes + (int) (y + 1) * xRes + (int) x];
	v4 = data2[currentFramePosition * xRes * yRes + (int) (y + 1) * xRes + (int) x + 1];
	va = linearInterpolation(x - (double)((int) x), v1, v2);
	vb = linearInterpolation(x - (double)((int) x), v3, v4);
	*vy = linearInterpolation(y - (double)((int) y), va, vb) / scalingFactor;
}

float DataSourceRawRingBuffer::linearInterpolation(float h, float v1, float v2)
{
	return h*(v2 - v1) + v1;
}

