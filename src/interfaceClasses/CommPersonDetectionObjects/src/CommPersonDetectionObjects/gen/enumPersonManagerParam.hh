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

#ifndef _COMMPERSONDETECTIONOBJECTS_ENUM_HH_PERSONMANAGERPARAM
#define _COMMPERSONDETECTIONOBJECTS_ENUM_HH_PERSONMANAGERPARAM

#include <CommPersonDetectionObjects/gen/enumPersonManagerParamC.hh>
#include <string>
#include <ostream>

namespace CommPersonDetectionObjects
{

class PersonManagerParam
{
public:
	enum literal
	{

		ACTIVATE_MANAGER = CommPersonDetectionObjectsIDL::LITERAL_ACTIVATE_MANAGER,

		DEACTIVATE_MANAGER = CommPersonDetectionObjectsIDL::LITERAL_DEACTIVATE_MANAGER,

		NONE_MANAGER_PARAM = CommPersonDetectionObjectsIDL::LITERAL_NONE_MANAGER_PARAM,

		RESET_MANAGER = CommPersonDetectionObjectsIDL::LITERAL_RESET_MANAGER
	};

	PersonManagerParam()
	{
	}

	PersonManagerParam(literal t)
	{
		value = t;
	}

	operator literal()
	{
		return value;
	}

	PersonManagerParam(CommPersonDetectionObjectsIDL::ENUM_PersonManagerParam t)
	{
		value = (literal) t;
	}

	bool operator ==(PersonManagerParam& t)
	{
		return this->value == t.value;
	}

	CommPersonDetectionObjectsIDL::ENUM_PersonManagerParam get_value_idl()
	{
		return (CommPersonDetectionObjectsIDL::ENUM_PersonManagerParam) value;
	}

	literal get_value() const
	{
		return value;
	}

	std::string get_string() const
	{
		switch (value)
		{

		case ACTIVATE_MANAGER:
			return "PersonManagerParam::ACTIVATE_MANAGER";
			break;

		case DEACTIVATE_MANAGER:
			return "PersonManagerParam::DEACTIVATE_MANAGER";
			break;

		case NONE_MANAGER_PARAM:
			return "PersonManagerParam::NONE_MANAGER_PARAM";
			break;

		case RESET_MANAGER:
			return "PersonManagerParam::RESET_MANAGER";
			break;

		default:
			return "UNKNOWN!";
			break;
		}
	}

private:
	literal value;
};

inline std::ostream &operator<<(std::ostream &os, const PersonManagerParam &e)
{
	os << e.get_string();
	return os;
}

}
#endif