//--------------------------------------------------------------------------
//  Copyright (C) 2009 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//--------------------------------------------------------------------------

#include "data/ObjectSet.hh"

#include "gen/SmartKatanaTestClient.hh"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <armadillo.hh>

using namespace std;

ObjectSet* ObjectSet::instance = 0;

ObjectSet& ObjectSet::getInstance() {
	if (instance == 0) {
		instance = new ObjectSet();
	}

	return *instance;
}

ObjectSet::ObjectSet() {
	isModified = false;
	index = glGenLists(1);
}

ObjectSet::~ObjectSet() {

	clearAll();

	// delete it if it is not used any more
	glDeleteLists(index, 1);

}

void ObjectSet::draw() {

	if (isModified) {
		drawNewList();
	}

	// draw the display list
	glCallList(index);
}

void ObjectSet::clearObjects() {
	COMP->ViewerMutex.acquire();
	objects.clear();
	COMP->ViewerMutex.release();
}

void ObjectSet::clearPoints() {
	COMP->ViewerMutex.acquire();
	points.clear();
	COMP->ViewerMutex.release();
}

void ObjectSet::clearLines() {
	COMP->ViewerMutex.acquire();
	lines.clear();
	COMP->ViewerMutex.release();
}

void ObjectSet::clearAll() {
	clearObjects();
	clearPoints();
	clearLines();
}

void ObjectSet::addCube(const Cube& obj) {
	COMP->ViewerMutex.acquire();
	objects.push_back(obj);
	COMP->ViewerMutex.release();
	isModified = true;
}

void ObjectSet::addPoint(const Point& point) {
	COMP->ViewerMutex.acquire();
	points.push_back(point);
	COMP->ViewerMutex.release();
	isModified = true;
}

void ObjectSet::addLine(const Line& line) {
	COMP->ViewerMutex.acquire();
	lines.push_back(line);
	COMP->ViewerMutex.release();
	isModified = true;
}

void ObjectSet::addPoint(double x, double y, double z, Color color) {
	Point pt(x, y, z, color.getGlRed(), color.getGlGreen(), color.getGlBlue());
	addPoint(pt);
}

void ObjectSet::addBoundingBox(const CommBasicObjects::CommPose3d &pose, double x, double y, double z, Color color) {
	arma::mat mat_pose = pose.getHomogeneousMatrix(1);

	arma::vec vec1(4);
	arma::vec vec2(4);
	vec1[3] = 1;
	vec2[3] = 1;

	// line 1
	vec1[0] = -x / 2;
	vec1[1] = -y / 2;
	vec1[2] = -z / 2;
	vec1 = mat_pose * vec1;

	vec2[0] = x / 2;
	vec2[1] = -y / 2;
	vec2[2] = -z / 2;
	vec2 = mat_pose * vec2;

	Line l1(Point(vec1[0], vec1[1], vec1[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()), Point(vec2[0],
			vec2[1], vec2[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()));
	addLine(l1);

	// line 2
	vec1[0] = x / 2;
	vec1[1] = -y / 2;
	vec1[2] = -z / 2;
	vec1 = mat_pose * vec1;

	vec2[0] = x / 2;
	vec2[1] = y / 2;
	vec2[2] = -z / 2;
	vec2 = mat_pose * vec2;

	Line l2(Point(vec1[0], vec1[1], vec1[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()), Point(vec2[0],
			vec2[1], vec2[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()));
	addLine(l2);

	// line 3
	vec1[0] = -x / 2;
	vec1[1] = y / 2;
	vec1[2] = -z / 2;
	vec1 = mat_pose * vec1;

	vec2[0] = x / 2;
	vec2[1] = y / 2;
	vec2[2] = -z / 2;
	vec2 = mat_pose * vec2;

	Line l3(Point(vec1[0], vec1[1], vec1[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()), Point(vec2[0],
			vec2[1], vec2[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()));
	addLine(l3);

	// line 4
	vec1[0] = -x / 2;
	vec1[1] = -y / 2;
	vec1[2] = -z / 2;
	vec1 = mat_pose * vec1;

	vec2[0] = -x / 2;
	vec2[1] = y / 2;
	vec2[2] = -z / 2;
	vec2 = mat_pose * vec2;

	Line l4(Point(vec1[0], vec1[1], vec1[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()), Point(vec2[0],
			vec2[1], vec2[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()));
	addLine(l4);

	// line 5
	vec1[0] = -x / 2;
	vec1[1] = -y / 2;
	vec1[2] = z / 2;
	vec1 = mat_pose * vec1;

	vec2[0] = x / 2;
	vec2[1] = -y / 2;
	vec2[2] = z / 2;
	vec2 = mat_pose * vec2;

	Line l5(Point(vec1[0], vec1[1], vec1[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()), Point(vec2[0],
			vec2[1], vec2[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()));
	addLine(l5);

	// line 6
	vec1[0] = x / 2;
	vec1[1] = -y / 2;
	vec1[2] = z / 2;
	vec1 = mat_pose * vec1;

	vec2[0] = x / 2;
	vec2[1] = y / 2;
	vec2[2] = z / 2;
	vec2 = mat_pose * vec2;

	Line l6(Point(vec1[0], vec1[1], vec1[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()), Point(vec2[0],
			vec2[1], vec2[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()));
	addLine(l6);

	// line 7
	vec1[0] = -x / 2;
	vec1[1] = y / 2;
	vec1[2] = z / 2;
	vec1 = mat_pose * vec1;

	vec2[0] = x / 2;
	vec2[1] = y / 2;
	vec2[2] = z / 2;
	vec2 = mat_pose * vec2;

	Line l7(Point(vec1[0], vec1[1], vec1[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()), Point(vec2[0],
			vec2[1], vec2[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()));
	addLine(l7);

	// line 8
	vec1[0] = -x / 2;
	vec1[1] = -y / 2;
	vec1[2] = z / 2;
	vec1 = mat_pose * vec1;

	vec2[0] = -x / 2;
	vec2[1] = y / 2;
	vec2[2] = z / 2;
	vec2 = mat_pose * vec2;

	Line l8(Point(vec1[0], vec1[1], vec1[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()), Point(vec2[0],
			vec2[1], vec2[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()));
	addLine(l8);

	// line 9
	vec1[0] = -x / 2;
	vec1[1] = -y / 2;
	vec1[2] = -z / 2;
	vec1 = mat_pose * vec1;

	vec2[0] = -x / 2;
	vec2[1] = -y / 2;
	vec2[2] = z / 2;
	vec2 = mat_pose * vec2;

	Line l9(Point(vec1[0], vec1[1], vec1[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()), Point(vec2[0],
			vec2[1], vec2[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()));
	addLine(l9);

	// line 10
	vec1[0] = x / 2;
	vec1[1] = -y / 2;
	vec1[2] = -z / 2;
	vec1 = mat_pose * vec1;

	vec2[0] = x / 2;
	vec2[1] = -y / 2;
	vec2[2] = z / 2;
	vec2 = mat_pose * vec2;

	Line l10(Point(vec1[0], vec1[1], vec1[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()), Point(vec2[0],
			vec2[1], vec2[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()));
	addLine(l10);

	// line 11
	vec1[0] = x / 2;
	vec1[1] = y / 2;
	vec1[2] = -z / 2;
	vec1 = mat_pose * vec1;

	vec2[0] = x / 2;
	vec2[1] = y / 2;
	vec2[2] = z / 2;
	vec2 = mat_pose * vec2;

	Line l11(Point(vec1[0], vec1[1], vec1[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()), Point(vec2[0],
			vec2[1], vec2[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()));
	addLine(l11);

	// line 12
	vec1[0] = -x / 2;
	vec1[1] = y / 2;
	vec1[2] = -z / 2;
	vec1 = mat_pose * vec1;

	vec2[0] = -x / 2;
	vec2[1] = y / 2;
	vec2[2] = z / 2;
	vec2 = mat_pose * vec2;

	Line l12(Point(vec1[0], vec1[1], vec1[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()), Point(vec2[0],
			vec2[1], vec2[2], color.getGlRed(), color.getGlGreen(), color.getGlBlue()));
	addLine(l12);
}

void ObjectSet::drawBaseCoord(float lineLenght) {
	Line xAxis(Point(0, 0, 0, 1, 0, 0), Point(lineLenght, 0, 0, 1, 0, 0));
	addLine(xAxis);

	Line yAxis(Point(0, 0, 0, 0, 1, 0), Point(0, lineLenght, 0, 0, 1, 0));
	addLine(yAxis);

	Line zAxis(Point(0, 0, 0, 0, 0, 1), Point(0, 0, lineLenght, 0, 0, 1));
	addLine(zAxis);
}

void ObjectSet::drawCoord(const CommBasicObjects::CommPose3d &pose, float lineLenght) {

	drawCoord(pose.getHomogeneousMatrix(1), lineLenght);
}

void ObjectSet::drawCoord(const arma::mat &mat_pose, float lineLenght) {

	arma::vec vec_o(4);
	vec_o.zeros();
	vec_o[3] = 1;

	arma::vec vec_x = vec_o;
	vec_x[0] = lineLenght;

	arma::vec vec_y = vec_o;
	vec_y[1] = lineLenght;

	arma::vec vec_z = vec_o;
	vec_z[2] = lineLenght;

	vec_o = mat_pose * vec_o;
	vec_x = mat_pose * vec_x;
	vec_y = mat_pose * vec_y;
	vec_z = mat_pose * vec_z;

	Line xAxis(Point(vec_o[0], vec_o[1], vec_o[2], 1, 0, 0), Point(vec_x[0], vec_x[1], vec_x[2], 1, 0, 0));
	addLine(xAxis);

	Line yAxis(Point(vec_o[0], vec_o[1], vec_o[2], 0, 1, 0), Point(vec_y[0], vec_y[1], vec_y[2], 0, 1, 0));
	addLine(yAxis);

	Line zAxis(Point(vec_o[0], vec_o[1], vec_o[2], 0, 0, 1), Point(vec_z[0], vec_z[1], vec_z[2], 0, 0, 1));
	addLine(zAxis);
}

void ObjectSet::drawNewList() {
	COMP->ViewerMutex.acquire();
	glNewList(index, GL_COMPILE);
	{
		glDisable(GL_LIGHTING);

		// Points
		glPushMatrix();
		{
			glBegin(GL_POINTS);
			vector<Point>::iterator iter;
			for (iter = points.begin(); iter != points.end(); iter++) {
				Point& pt = *(iter);
				glColor3f(pt.r, pt.g, pt.b);
				glVertex3f(pt.x, pt.y, pt.z);

			}
			glEnd();
		}
		glPopMatrix();

		// Lines
		glPushMatrix();
		{
			vector<Line>::iterator iter;
			for (iter = lines.begin(); iter != lines.end(); iter++) {
				glBegin(GL_LINES);
				{
					Line& line = *(iter);

					glColor3f(line.begin.r, line.begin.g, line.begin.b);
					glVertex3f(line.begin.x, line.begin.y, line.begin.z);

					glColor3f(line.end.r, line.end.g, line.end.b);
					glVertex3f(line.end.x, line.end.y, line.end.z);
				}
				glEnd();
			}
		}
		glPopMatrix();

		glEnable(GL_LIGHTING);

		// Objects
		vector<Cube>::iterator iter;
		for (iter = objects.begin(); iter != objects.end(); iter++) {
			Cube& obj = *(iter);

			glPushMatrix();
			{
				glColor3f(obj.r, obj.g, obj.b);
				glTranslatef(obj.x, obj.y, obj.z);
				glutWireCube(obj.size);
			}
			glPopMatrix();
		}

	}
	glEndList();
	COMP->ViewerMutex.release();

	isModified = false;
}

