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

#ifndef _COMMOBJECTRECOGNITIONOBJECTS_ENUM_HH_SIMPLEOBJECTSTATE
#define _COMMOBJECTRECOGNITIONOBJECTS_ENUM_HH_SIMPLEOBJECTSTATE

#include <CommObjectRecognitionObjects/gen/enumSimpleObjectStateC.hh>
#include <string>
#include <ostream>

namespace CommObjectRecognitionObjects
{

class SimpleObjectState
{
public:
	enum literal
	{

		OBJECT_FOUND = CommObjectRecognitionObjectsIDL::LITERAL_OBJECT_FOUND,

		OBJECT_NOT_FOUND = CommObjectRecognitionObjectsIDL::LITERAL_OBJECT_NOT_FOUND,

		UNKNOWN = CommObjectRecognitionObjectsIDL::LITERAL_UNKNOWN
	};

	SimpleObjectState()
	{
	}

	SimpleObjectState(literal t)
	{
		value = t;
	}

	operator literal()
	{
		return value;
	}

	SimpleObjectState(CommObjectRecognitionObjectsIDL::ENUM_SimpleObjectState t)
	{
		value = (literal) t;
	}

	bool operator ==(SimpleObjectState& t)
	{
		return this->value == t.value;
	}

	CommObjectRecognitionObjectsIDL::ENUM_SimpleObjectState get_value_idl()
	{
		return (CommObjectRecognitionObjectsIDL::ENUM_SimpleObjectState) value;
	}

	literal get_value() const
	{
		return value;
	}

	std::string get_string() const
	{
		switch (value)
		{

		case OBJECT_FOUND:
			return "SimpleObjectState::OBJECT_FOUND";
			break;

		case OBJECT_NOT_FOUND:
			return "SimpleObjectState::OBJECT_NOT_FOUND";
			break;

		case UNKNOWN:
			return "SimpleObjectState::UNKNOWN";
			break;

		default:
			return "UNKNOWN!";
			break;
		}
	}

private:
	literal value;
};

inline std::ostream &operator<<(std::ostream &os, const SimpleObjectState &e)
{
	os << e.get_string();
	return os;
}

}
#endif
