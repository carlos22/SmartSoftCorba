//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.4
// The SmartSoft Toolchain has been developed by:
//  
// ZAFH Servicerobotic Ulm
// Christian Schlegel (schlegel@hs-ulm.de)
// University of Applied Sciences
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// smart-robotics.sourceforge.net
// 
// Please do not modify this file. It will be re-generated
// running the code generator.
//--------------------------------------------------------------------------

#ifndef _COMMOBJECTRECOGNITIONOBJECTS_ENUM_HH_OBJECTRECOGNITIONSENSOR
#define _COMMOBJECTRECOGNITIONOBJECTS_ENUM_HH_OBJECTRECOGNITIONSENSOR

#include <CommObjectRecognitionObjects/gen/enumObjectRecognitionSensorC.hh>
#include <string>
#include <ostream>

namespace CommObjectRecognitionObjects
{

class ObjectRecognitionSensor
{
public:
	enum literal
	{

		KINECT = CommObjectRecognitionObjectsIDL::LITERAL_KINECT,

		MANIPULATOR_CAMERA = CommObjectRecognitionObjectsIDL::LITERAL_MANIPULATOR_CAMERA,

		MANIPULATOR_LASER = CommObjectRecognitionObjectsIDL::LITERAL_MANIPULATOR_LASER
	};

	ObjectRecognitionSensor()
	{
	}

	ObjectRecognitionSensor(literal t)
	{
		value = t;
	}

	operator literal()
	{
		return value;
	}

	ObjectRecognitionSensor(
			CommObjectRecognitionObjectsIDL::ENUM_ObjectRecognitionSensor t)
	{
		value = (literal) t;
	}

	bool operator ==(ObjectRecognitionSensor& t)
	{
		return this->value == t.value;
	}

	CommObjectRecognitionObjectsIDL::ENUM_ObjectRecognitionSensor get_value_idl()
	{
		return (CommObjectRecognitionObjectsIDL::ENUM_ObjectRecognitionSensor) value;
	}

	literal get_value() const
	{
		return value;
	}

	std::string get_string() const
	{
		switch (value)
		{

		case KINECT:
			return "ObjectRecognitionSensor::KINECT";
			break;

		case MANIPULATOR_CAMERA:
			return "ObjectRecognitionSensor::MANIPULATOR_CAMERA";
			break;

		case MANIPULATOR_LASER:
			return "ObjectRecognitionSensor::MANIPULATOR_LASER";
			break;

		default:
			return "UNKNOWN!";
			break;
		}
	}

private:
	literal value;
};

inline std::ostream &operator<<(std::ostream &os,
		const ObjectRecognitionSensor &e)
{
	os << e.get_string();
	return os;
}

}
#endif
