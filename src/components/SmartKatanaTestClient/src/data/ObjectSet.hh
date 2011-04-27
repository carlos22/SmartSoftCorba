//--------------------------------------------------------------------------
//  Copyright (C) 2009 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//--------------------------------------------------------------------------

#ifndef OBJECTSET_HH_
#define OBJECTSET_HH_


#include <vector>
#include "GL/glut.h"

#include "data/structs.hh"
#include "data/Color.hh"

#include <CommBasicObjects/commPose3d.hh>

using namespace std;

class ObjectSet
{

	private:
		static ObjectSet* instance;

		// create one display list
		GLuint index;

		bool isModified;
		vector<Cube> objects;
		vector<Point> points;
		vector<Line> lines;


	public:
		static ObjectSet& getInstance();

	public:
		void draw();

		void drawBaseCoord(float lineLenght = 0.2);
		void drawCoord(const CommBasicObjects::CommPose3d &pose, float lineLenght = 0.2);
		void drawCoord(const arma::mat &mat_pose, float lineLenght);

		void addCube(const Cube& obj);
		void addPoint(const Point& point);
		void addLine(const Line& line);

		void addPoint(double x, double y, double z, Color color);
		void addBoundingBox(const CommBasicObjects::CommPose3d &pose, double x, double y, double z, Color color);

		void clearObjects();
		void clearPoints();
		void clearLines();
		void clearAll();


	private:
		ObjectSet();
		~ObjectSet();

		void drawNewList();
};

#endif /* OBJECTSET_H_ */
