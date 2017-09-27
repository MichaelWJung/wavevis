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

#include "colormapasc.h"
#include "glutwindow.h"

colorMapAsc::colorMapAsc(char* filename)
{
  FILE * pFile = fopen(filename, "r");

  if (pFile == NULL)
	{
		std::cout << "Error: Can't open file " << filename << "\n";
	}

  int j = -1;

	//count the number of lines
  while (feof(pFile) == 0)
  {
    fscanf(pFile, "%*f %*f %*f");
    ++j;
  }

  if (j == -1)
  {
    std::cout << "Error: no color map available / incorrect color map in file " << filename << std::endl;
  }

  numberOfColors = j;
  map = (float*) malloc(sizeof(float) * j * 3);

  rewind(pFile);
  
  for (int i = 0; i < j; ++i)
  {
    fscanf(pFile, "%f %f %f", &map[3 * i], &map[3 * i + 1], &map[3 * i + 2]);
  }

  fclose(pFile);
}

colorMapAsc::~colorMapAsc()
{
	free(map);
}

//value lies between 0.0 and 1.0
//writes the color into the buffer array
void colorMapAsc::lookUp(float value, float* buffer)
{
  int n = (int) (value * numberOfColors);
  if (n >= numberOfColors) n = numberOfColors - 1;
  if (n < 0) n = 0;
	buffer[0] = map[3 * n];
	buffer[1] = map[3 * n + 1];
	buffer[2] = map[3 * n + 2];
}

//set / get tags
void colorMapAsc::minTag(std::string string)
{
	_minTag = string;
}

std::string colorMapAsc::minTag()
{
	return _minTag;
}

void colorMapAsc::maxTag(std::string string)
{
	_maxTag = string;
}

std::string colorMapAsc::maxTag()
{
	return _maxTag;
}
