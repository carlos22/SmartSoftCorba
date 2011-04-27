//--------------------------------------------------------------------------
//  Copyright (C) 2009 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//--------------------------------------------------------------------------

#ifndef COLOR_HH_
#define COLOR_HH_

#include <stdlib.h>
#include <time.h>

class Color {
public:
	enum Colors {
		Red = 0,
		Green = 1,
		Blue = 2,
		DeepPink = 3,
		DeepSkyBlue = 4,
		Green4 = 5,
		DeepSkyBlue4 = 6,
		DarkOrange = 7,
		SlateBlue1 = 8,
		Cyan = 9
	};

private:
	int red;
	int green;
	int blue;

public:
	Color(int color);

	float getGlRed() const {
		return (float) red / 255;
	}

	float getGlGreen() const {
		return (float) green / 255;
	}

	float getGlBlue() const {
		return (float) blue / 255;
	}

	static void init_random() {
		srand(time(NULL));
	}

	static Color::Colors random() {
		return (Color::Colors) (rand() % 10);
	}

};

#endif /* COLOR_H_ */
