/*
 * PersonDetectionVisualization.h
 *
 *  Created on: Jul 6, 2011
 *      Author: wopfner
 */

#ifndef PERSONDETECTIONVISUALIZATION_HH_
#define PERSONDETECTIONVISUALIZATION_HH_

#include "AbstractVisualization.hh"
#include <CommPersonDetectionObjects/commDetectedPerson.hh>

#include <vector>

class PersonDetectionVisualization: public AbstractVisualization
{
public:
	PersonDetectionVisualization(CDisplayWindow3D& window3D, const std::string& identifier);
	virtual ~PersonDetectionVisualization();

	void displayPersons(std::vector<CommPersonDetectionObjects::CommDetectedPerson>& persons);
	void clear();
};

#endif /* PERSONDETECTIONVISUALIZATION_HH_ */
