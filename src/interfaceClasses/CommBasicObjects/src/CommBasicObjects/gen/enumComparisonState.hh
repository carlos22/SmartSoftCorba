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

#ifndef _COMMBASICOBJECTS_ENUM_HH_COMPARISONSTATE
#define _COMMBASICOBJECTS_ENUM_HH_COMPARISONSTATE

#include <CommBasicObjects/gen/enumComparisonStateC.hh>
#include <string>
#include <ostream>

namespace CommBasicObjects
{

class ComparisonState
{
public:
	enum literal
	{

		GREATER = CommBasicObjectsIDL::LITERAL_GREATER,

		INBETWEEN = CommBasicObjectsIDL::LITERAL_INBETWEEN,

		LOWER = CommBasicObjectsIDL::LITERAL_LOWER,

		UNKNOWN = CommBasicObjectsIDL::LITERAL_UNKNOWN
	};

	ComparisonState()
	{
	}

	ComparisonState(literal t)
	{
		value = t;
	}

	operator literal()
	{
		return value;
	}

	ComparisonState(CommBasicObjectsIDL::ENUM_ComparisonState t)
	{
		value = (literal) t;
	}

	bool operator ==(ComparisonState& t)
	{
		return this->value == t.value;
	}

	CommBasicObjectsIDL::ENUM_ComparisonState get_value_idl()
	{
		return (CommBasicObjectsIDL::ENUM_ComparisonState) value;
	}

	literal get_value() const
	{
		return value;
	}

	std::string get_string() const
	{
		switch (value)
		{

		case GREATER:
			return "ComparisonState::GREATER";
			break;

		case INBETWEEN:
			return "ComparisonState::INBETWEEN";
			break;

		case LOWER:
			return "ComparisonState::LOWER";
			break;

		case UNKNOWN:
			return "ComparisonState::UNKNOWN";
			break;

		default:
			return "UNKNOWN!";
			break;
		}
	}

private:
	literal value;
};

inline std::ostream &operator<<(std::ostream &os, const ComparisonState &e)
{
	os << e.get_string();
	return os;
}

}
#endif
