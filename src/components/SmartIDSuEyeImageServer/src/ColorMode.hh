//--------------------------------------------------------------------------
//
//  Copyright (C) 2011 Manuel Wopfner
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "IDS uEye Video Server component".
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//--------------------------------------------------------------------------

#ifndef COLORMODE_H_
#define COLORMODE_H_

#include <string>
#include <iostream>

// define configures ueye.h
#define __LINUX__

#include <ueye.h>

#include <CommVisionObjects/gen/enumFormatType.hh>

class ColorMode
{
public:

	class NotSupported: std::exception
	{
	public:
		NotSupported(const std::string& msg) :
			message(msg)
		{
		}
		virtual ~NotSupported() throw()
		{
		}

		/** Returns a C-style character string describing the general cause
		 *  of the current error.  */
		virtual const char* what() const throw()
		{
			return message.c_str();
		}

	private:
		std::string message;
	};

	enum literal
	{
		MONO8 = IS_CM_MONO8,
		MONO12 = IS_CM_MONO12,
		MONO16 = IS_CM_MONO16,

		BAYER_RG8 = IS_CM_BAYER_RG8,
		BAYER_RG12 = IS_CM_BAYER_RG12,
		BAYER_RG16 = IS_CM_BAYER_RG16,

		BGR555_PACKED = IS_CM_BGR555_PACKED,
		BGR565_PACKED = IS_CM_BGR565_PACKED,

		RGB8_PACKED = IS_CM_RGB8_PACKED,
		BGR8_PACKED = IS_CM_BGR8_PACKED,
		RGBA8_PACKED = IS_CM_RGBA8_PACKED,
		BGRA8_PACKED = IS_CM_BGRA8_PACKED,
		RGBY8_PACKED = IS_CM_RGBY8_PACKED,
		BGRY8_PACKED = IS_CM_BGRY8_PACKED,
		RGB10V2_PACKED = IS_CM_RGB10V2_PACKED,
		BGR10V2_PACKED = IS_CM_BGR10V2_PACKED,

		UYVY_PACKED = IS_CM_UYVY_PACKED,
		UYVY_MONO_PACKED = IS_CM_UYVY_MONO_PACKED,
		UYVY_BAYER_PACKED = IS_CM_UYVY_BAYER_PACKED,
		CBYCRY_PACKED = IS_CM_CBYCRY_PACKED,

		UNKNOWN
	};

	ColorMode()
	{
		value = ColorMode::UNKNOWN;
	}

	ColorMode(literal t)
	{
		value = t;
	}

	ColorMode(int t)
	{
		value = (literal) t;
	}

	operator literal()
	{
		return value;
	}

	bool operator ==(ColorMode& t)
	{
		return this->value == t.value;
	}

	int getBits()
	{
		switch (value)
		{
		case RGBA8_PACKED:
		case BGRA8_PACKED:
		case RGBY8_PACKED:
		case BGRY8_PACKED:
		case RGB10V2_PACKED:
		case BGR10V2_PACKED:
			return 32;

		case RGB8_PACKED:
		case BGR8_PACKED:
			return 24;

		case MONO12:
		case MONO16:
		case BAYER_RG12:
		case BAYER_RG16:
		case BGR555_PACKED:
		case BGR565_PACKED:
		case UYVY_PACKED:
		case UYVY_MONO_PACKED:
		case UYVY_BAYER_PACKED:
		case CBYCRY_PACKED:
			return 16;

		case MONO8:
		case BAYER_RG8:
			return 8;

		default:
			std::cerr << "[ColorMode::getBits]: Type " << toString() << " not supported!\n";
			throw NotSupported("Type " + toString() + " not supported!");
		}
	}

	CommVisionObjects::FormatType getVisionFormat() const
	{
		switch (value)
		{

		case MONO8:
			return CommVisionObjects::FormatType::GREY;
			break;
		case RGB8_PACKED:
			return CommVisionObjects::FormatType::RGB24;
			break;
		case RGBA8_PACKED:
			return CommVisionObjects::FormatType::RGB32;
			break;
		case UYVY_PACKED:
			return CommVisionObjects::FormatType::UYVY;
			break;
		default:
			std::cerr << "[ColorMode::getVisionFormat]: Type " << toString() << " not supported!\n";
			throw NotSupported("Type " + toString() + " not supported!");
		}
	}

	std::string toString() const
	{
		switch (value)
		{
		case MONO8:
			return "ColorMode::MONO8";
			break;
		case MONO12:
			return "ColorMode::MONO12";
			break;
		case MONO16:
			return "ColorMode::MONO16";
			break;

		case BAYER_RG8:
			return "ColorMode::BAYER_RG8";
			break;
		case BAYER_RG12:
			return "ColorMode::BAYER_RG12";
			break;
		case BAYER_RG16:
			return "ColorMode::BAYER_RG16";
			break;

		case BGR555_PACKED:
			return "ColorMode::BGR555_PACKED";
			break;
		case BGR565_PACKED:
			return "ColorMode::BGR565_PACKED";
			break;

		case RGB8_PACKED:
			return "ColorMode::RGB8_PACKED";
			break;
		case BGR8_PACKED:
			return "ColorMode::BGR8_PACKED";
			break;
		case RGBA8_PACKED:
			return "ColorMode::RGBA8_PACKED";
			break;
		case BGRA8_PACKED:
			return "ColorMode::BGRA8_PACKED";
			break;
		case RGBY8_PACKED:
			return "ColorMode::RGBY8_PACKED";
			break;
		case BGRY8_PACKED:
			return "ColorMode::BGRY8_PACKED";
			break;
		case RGB10V2_PACKED:
			return "ColorMode::RGB10V2_PACKED";
			break;
		case BGR10V2_PACKED:
			return "ColorMode::BGR10V2_PACKED";
			break;

		case UYVY_PACKED:
			return "ColorMode::UYVY_PACKED";
			break;
		case UYVY_MONO_PACKED:
			return "ColorMode::UYVY_MONO_PACKED";
			break;
		case UYVY_BAYER_PACKED:
			return "ColorMode::UYVY_BAYER_PACKED";
			break;
		case CBYCRY_PACKED:
			return "ColorMode::CBYCRY_PACKED";
			break;

		default:
			return "ColorMode::UNKNOWN";
			break;
		}
	}

private:
	literal value;
};

inline std::ostream &operator<<(std::ostream &os, const ColorMode &m)
{
	os << m.toString();
	return os;
}

#endif /* COLORMODE_H_ */
