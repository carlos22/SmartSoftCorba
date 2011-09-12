//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.3
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

#ifndef _COMMNAVIGATIONOBJECTS_ENUM_HH_CDLGOALEVENTTYPE
#define _COMMNAVIGATIONOBJECTS_ENUM_HH_CDLGOALEVENTTYPE

#include <CommNavigationObjects/gen/enumCdlGoalEventTypeC.hh>
#include <string>
#include <ostream>

namespace CommNavigationObjects
{

class CdlGoalEventType
{
public:
	enum literal
	{

		CDL_GOAL_NOT_REACHED = CommNavigationObjectsIDL::LITERAL_CDL_GOAL_NOT_REACHED,

		CDL_GOAL_REACHED = CommNavigationObjectsIDL::LITERAL_CDL_GOAL_REACHED,

		CDL_GOAL_UNKNOWN = CommNavigationObjectsIDL::LITERAL_CDL_GOAL_UNKNOWN
	};

	CdlGoalEventType()
	{
	}

	CdlGoalEventType(literal t)
	{
		value = t;
	}

	operator literal()
	{
		return value;
	}

	CdlGoalEventType(CommNavigationObjectsIDL::ENUM_CdlGoalEventType t)
	{
		value = (literal) t;
	}

	bool operator ==(CdlGoalEventType& t)
	{
		return this->value == t.value;
	}

	CommNavigationObjectsIDL::ENUM_CdlGoalEventType get_value_idl()
	{
		return (CommNavigationObjectsIDL::ENUM_CdlGoalEventType) value;
	}

	literal get_value() const
	{
		return value;
	}

	std::string get_string() const
	{
		switch (value)
		{

		case CDL_GOAL_NOT_REACHED:
			return "CdlGoalEventType::CDL_GOAL_NOT_REACHED";
			break;

		case CDL_GOAL_REACHED:
			return "CdlGoalEventType::CDL_GOAL_REACHED";
			break;

		case CDL_GOAL_UNKNOWN:
			return "CdlGoalEventType::CDL_GOAL_UNKNOWN";
			break;

		default:
			return "UNKNOWN!";
			break;
		}
	}

private:
	literal value;
};

inline std::ostream &operator<<(std::ostream &os, const CdlGoalEventType &e)
{
	os << e.get_string();
	return os;
}

}
#endif