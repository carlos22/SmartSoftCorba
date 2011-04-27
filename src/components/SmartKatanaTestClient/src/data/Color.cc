//--------------------------------------------------------------------------
//  Copyright (C) 2009 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//--------------------------------------------------------------------------

#include "data/Color.hh"

Color::Color(int color)
{
	switch (color)
	{
		case Color::Blue:
			red = 0;
			green = 0;
			blue = 255;
			break;

		case Color::DeepSkyBlue:
			red = 0;
			green = 191;
			blue = 255;
			break;

		case Color::DeepSkyBlue4:
			red = 0;
			green = 104;
			blue = 139;
			break;

		case Color::SlateBlue1:
			red = 131;
			green = 111;
			blue = 255;
			break;

		case Color::Cyan:
			red = 0;
			green = 255;
			blue = 255;
			break;

		case Color::Red:
			red = 255;
			green = 0;
			blue = 0;
			break;

		case Color::Green:
			red = 0;
			green = 255;
			blue = 0;
			break;

		case Color::Green4:
			red = 0;
			green = 139;
			blue = 0;
			break;

		case Color::DarkOrange:
			red = 255;
			green = 140;
			blue = 0;
			break;

		case Color::DeepPink:
			red = 255;
			green = 20;
			blue = 147;
			break;
	}
}
