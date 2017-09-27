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

// This is the colormap base class. If you want to use another color map
// than the one that comes with this program you can create a child class
// and pass an instance of that class to the config object.

#ifndef _COLORMAP_H_
#define _COLORMAP_H_

#include "glutwindow.h"

class colorMap
{
	protected:
	//The tags for minimum and maximum value to be shown in the legend
	std::string _minTag;
	std::string _maxTag;

	public:
	virtual void lookUp(float value, float* buffer) = 0;
	virtual std::string minTag() = 0;
	virtual void minTag(std::string i) = 0;
	virtual std::string maxTag() = 0;
	virtual void maxTag(std::string i) = 0;
};

#endif
