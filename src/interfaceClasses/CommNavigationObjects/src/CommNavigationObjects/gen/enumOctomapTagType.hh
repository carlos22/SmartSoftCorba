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

#ifndef _COMMNAVIGATIONOBJECTS_ENUM_HH_OCTOMAPTAGTYPE
#define _COMMNAVIGATIONOBJECTS_ENUM_HH_OCTOMAPTAGTYPE

#include <CommNavigationObjects/gen/enumOctomapTagTypeC.hh>
#include <string>
#include <ostream>

namespace CommNavigationObjects
{

class OctomapTagType
{
public:
	enum literal
	{

		RESET = CommNavigationObjectsIDL::LITERAL_RESET
	};

	OctomapTagType()
	{
	}

	OctomapTagType(literal t)
	{
		value = t;
	}

	operator literal()
	{
		return value;
	}

	OctomapTagType(CommNavigationObjectsIDL::ENUM_OctomapTagType t)
	{
		value = (literal) t;
	}

	bool operator ==(OctomapTagType& t)
	{
		return this->value == t.value;
	}

	CommNavigationObjectsIDL::ENUM_OctomapTagType get_value_idl()
	{
		return (CommNavigationObjectsIDL::ENUM_OctomapTagType) value;
	}

	literal get_value() const
	{
		return value;
	}

	std::string get_string() const
	{
		switch (value)
		{

		case RESET:
			return "OctomapTagType::RESET";
			break;

		default:
			return "UNKNOWN!";
			break;
		}
	}

private:
	literal value;
};

inline std::ostream &operator<<(std::ostream &os, const OctomapTagType &e)
{
	os << e.get_string();
	return os;
}

}
#endif
