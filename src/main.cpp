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
#include "config.h"
#include "wave.h"
#include "datasource.h"
#include "datasourceraw.h"
#include "datasourcerawringbuffer.h"
#include "colormap.h"
#include "colormapasc.h"

namespace
{
	std::list<waveConfig::sourceTableEntry>::iterator idleFuncIterator;
	bool idleFuncIteratorSet = false;
};

void parseconfig(waveConfig* config, char* filename)
{
	std::ifstream file;
	std::string line;
	std::string tag;
	std::string value;
	int k,i,c;
  double d;

	//used to temporarily store values read from the config file
	bool buffered = false;
	int bufferSize = 50;
	waveConfig::sourceTableEntry s;
  char file1[1024];
  char file2[1024];
  waveConfig::cameraPoint p;
	std::string min, max;
	float minValue, maxValue;

	file.open(filename);
	file.is_open();

	if (file.fail())
  {
 		std::cout << "Error: Could not open config file " << filename << std::endl;
    exit(1);
  }

	//loop through config file
	while (!file.eof()) {
	    getline(file, line);

		//It's not a comment nor a new line
		if (line[0] != '#' && line[0] != '\0') {
			i = 0;

			//get tag and value
			while (line[i]!= '\t') {
				i++;
			}
			tag = line.substr(0,i);
			k = ++i;

			while (line[i]!= '\0') {
				i++;
			}
			value = line.substr(k,i-k);
      std::istringstream valuestream(value);

			//put the values to the config object
      if (tag == "RES_X")
			{
        valuestream >> c;
				config->xRes(c);
			}

      if (tag == "RES_Y")
			{
        valuestream >> c;
				config->yRes(c);
			}

      if (tag == "FRAMES")
			{
        valuestream >> c;
				config->frames(c);
			}

      if (tag == "FRAMES_PER_SEC")
      {
        valuestream >> c;
        config->framesPerSec(c);
      }

			if (tag == "BUFFERED")
			{
				valuestream >> c;
				switch (c)
				{
					case 1: buffered = true; break;
					case 0: buffered = false; break;
					default: break;
				}
			}

			if (tag == "BUFFER_SIZE")
        valuestream >> bufferSize;

      if (tag == "SCALING")
      {
        valuestream >> d;
        config->scaling(d);
      }

      if (tag == "GROUND_Z")
      {
        valuestream >> d;
        config->ground(d);
      }

      if (tag == "BG_RED")
      {
        valuestream >> d;
        config->bgred(d);
      }

      if (tag == "BG_GREEN")
      {
        valuestream >> d;
        config->bggreen(d);
      }

      if (tag == "BG_BLUE")
      {
        valuestream >> d;
        config->bgblue(d);
      }

			if (tag == "FILE_HEIGHT")
			{
				char* file = new char[1024];
				for (int j = 0; j <= i-k; ++j)
				{
          if (j != i-k)
						file[j] = value[j];
					else
						file[j] = '\0';
				}
				if (buffered)
					s.data = new DataSourceRawRingBuffer(file, config->xRes(), config->yRes(), config->frames(), bufferSize);
				else
					s.data = new DataSourceRaw(file, config->xRes(), config->yRes(), config->frames());
				s.type = HEIGHT;
				s.colormap = NULL;
				config->dataList.push_back(s);
				delete[] file;
			}

			if (tag == "FILE_SLOPE_X")
			{
				char* file = new char[1024];
				for (int j = 0; j <= i-k; ++j)
				{
          if (j != i-k)
						file[j] = value[j];
					else
						file[j] = '\0';
				}

				if (buffered)
					s.data = new DataSourceRawRingBuffer(file, config->xRes(), config->yRes(), config->frames(), bufferSize);
				else
					s.data = new DataSourceRaw(file, config->xRes(), config->yRes(), config->frames());
				s.type = SLOPE_X;
				s.colormap = NULL;
				config->dataList.push_back(s);
				delete[] file;
			}

			if (tag == "FILE_SLOPE_Y")
			{
				char* file = new char[1024];
				for (int j = 0; j <= i-k; ++j)
				{
          if (j != i-k)
						file[j] = value[j];
					else
						file[j] = '\0';
				}

				if (buffered)
					s.data = new DataSourceRawRingBuffer(file, config->xRes(), config->yRes(), config->frames(), bufferSize);
				else
					s.data = new DataSourceRaw(file, config->xRes(), config->yRes(), config->frames());
				s.type = SLOPE_Y;
				s.colormap = NULL;
				config->dataList.push_back(s);
				delete[] file;
			}

			if (tag == "OVERLAY_BEGIN")
			{
				//set default values for the overlay setup
				s.data = NULL;
				s.colormap = NULL;
				s.type = FIELD_1D;
				minValue = 1.0;
				maxValue = 0.0;
				min.clear();
				max.clear();
			}

			if (tag == "OVERLAY_END")
      {
        switch (s.type)
				{
					case FIELD_2D:
						if (buffered)
							s.data = new DataSourceRawRingBuffer(file1, file2, config->xRes(), config->yRes(), config->frames(), maxValue, bufferSize);
						else
							s.data = new DataSourceRaw(file1, file2, config->xRes(), config->yRes(), config->frames(), maxValue);
						break;
					case FIELD_1D:
						std::string _min, _max;
						if (buffered)
						{
							//min / max entered manually?
							if (minValue < maxValue)
							{
									s.data = new DataSourceRawRingBuffer(file1, config->xRes(), config->yRes(), config->frames(), minValue, maxValue, bufferSize);
									std::stringstream __min, __max;
									__min << minValue;
									__max << maxValue;
									min = __min.str();
									max = __max.str();
							}
							else
							{
									s.data = new DataSourceRawRingBuffer(file1, config->xRes(), config->yRes(), config->frames(), 0, 50, bufferSize);
									std::stringstream __min, __max;
									__min << 0;
									__max << 50;
									min = __min.str();
									max = __max.str();
							}
						}
						else
						{
							//min / max entered manually?
							if (minValue < maxValue)
								s.data = new DataSourceRaw(file1, config->xRes(), config->yRes(), config->frames(), minValue, maxValue, &_min, &_max);
							else
								s.data = new DataSourceRaw(file1, config->xRes(), config->yRes(), config->frames(), &_min, &_max);
						}

						//if tags have not been entered manually use the default ones
						if (min.length() == 0)
							min = _min;
						if (max.length() == 0)
							max = _max;

						s.colormap->minTag(min);
						s.colormap->maxTag(max);	
				}
				//put into the data source list
				config->dataList.push_back(s);
			}

			if (tag == "OVERLAY_TYPE")
			{
				valuestream >> c;
				switch (c)
				{
					case 1: s.type = FIELD_1D; break;
					case 2: s.type = FIELD_2D; break;
					default: break;
				}
			}

			if (tag == "OVERLAY_FILE")
			{
				for (int j = 0; j <= i-k; ++j)
				{
          if (j != i-k)
						file1[j] = value[j];
					else
						file1[j] = '\0';
				}
			}

			if (tag == "OVERLAY_COLORMAP")
			{
				char* file = new char[1024];
				for (int j = 0; j <= i-k; ++j)
				{
          if (j != i-k)
						file[j] = value[j];
					else
						file[j] = '\0';
				}

			  s.colormap = new colorMapAsc(file);
				delete[] file;
			}

			if (tag == "OVERLAY_VALUE_MIN")
				valuestream >> minValue;

			if (tag == "OVERLAY_VALUE_MAX")
				valuestream >> maxValue;

			if (tag == "OVERLAY_TAG_MIN")
				min = value;

			if (tag == "OVERLAY_TAG_MAX")
				max = value;

			if (tag == "OVERLAY_FILE1")
			{
				for (int j = 0; j <= i-k; ++j)
				{
          if (j != i-k)
						file1[j] = value[j];
					else
						file1[j] = '\0';
				}
			}

			if (tag == "OVERLAY_FILE2")
			{
				for (int j = 0; j <= i-k; ++j)
				{
          if (j != i-k)
						file2[j] = value[j];
					else
						file2[j] = '\0';
				}
			}

			if (tag == "ANIMATION")
			{
				valuestream >> c;
				switch (c)
				{
					case 1: config->animationEnabled(true); break;
					case 0: config->animationEnabled(false); break;
					default: break;
				}
			}

      if (tag == "FLIGHT_ENABLED")
			{
				valuestream >> c;
				switch (c)
				{
					case 1: config->flightEnabled(true); config->freeCamera(false); break;
					case 0: config->flightEnabled(false); config->freeCamera(true); break;
					default: break;
				}
			}

			if (tag == "CAMERA_BEGIN")
			{
				p.x = p.y = p.z = p.tx = p.ty = 0;
        p.duration = 1000;
			}

			if (tag == "CAMERA_END")
				config->cameraFlight.push_back(p);

                  
      if (tag == "CAMERA_X")
        valuestream >> p.x;     
      if (tag == "CAMERA_Y")
        valuestream >> p.y;
      if (tag == "CAMERA_Z")
        valuestream >> p.z;
      if (tag == "CAMERA_TARGET_X")
        valuestream >> p.tx;
      if (tag == "CAMERA_TARGET_Y")
        valuestream >> p.ty;
      if (tag == "FLIGHT_DURATION")
        valuestream >> p.duration;
		}
	}
	file.close();
}

void idle(waveConfig* config)
{
	if (idleFuncIteratorSet)
	{
		idleFuncIterator++;
		if (idleFuncIterator == config->dataList.end())
		{
			idleFuncIterator = config->dataList.begin();
		}
		idleFuncIterator->data->idle(0);
	}
	else
	{
		idleFuncIterator = config->dataList.begin();
		idleFuncIteratorSet = true;
	}
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cout << "usage: wave <configfile>\ni.e.: wave wave.cfg\n";
		exit (1);
	}

	//init glut display system
	int n = 1;
	char** a = NULL;
	glutInit(&n, a);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (800,600);
	glutInitWindowPosition (20, 20);

	//create config object and pass it to a wave object
  waveConfig* config = new waveConfig();
	parseconfig(config, argv[1]);
	config->idleFunc(&idle);
  wave* wave1 = new wave(config);

  glutMainLoop();
}
