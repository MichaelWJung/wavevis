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

#include "vector.h"

vector::vector()
{
  x = 0; y = 0; z = 0;
}

vector::vector(double initialX, double initialY, double initialZ)
{
  x = initialX;
  y = initialY;
  z = initialZ;
}

vector vector::operator+ (vector param)
{
  vector result (x + param.x, y + param.y, z + param.z);
  return result;
}

vector vector::operator- (vector param)
{
  vector result (x - param.x, y - param.y, z - param.z);
  return result;
}

//cross product
vector vector::operator& (vector param)
{
  vector result;
  result.x = y * param.z - z * param.y;
  result.y = z * param.x - x * param.z;
  result.z = x * param.y - y * param.x;
  return result;
}

vector vector::operator* (double value)
{
  vector result;
  result.x = x * value;
  result.y = y * value;
  result.z = z * value;
  return result;
}

void vector::operator = (vector param)
{
  x = param.x;
  y = param.y;
  z = param.z;
}

void vector::normalize()
{
  double length = sqrt(x*x + y*y + z*z);
  x /= length;
  y /= length;
  z /= length;
}

void vector::out()
{
  std::cout << x << " " << y << " " << z << std::endl;
}
