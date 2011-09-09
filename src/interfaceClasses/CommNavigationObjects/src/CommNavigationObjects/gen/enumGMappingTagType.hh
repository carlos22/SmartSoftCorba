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

#ifndef _COMMNAVIGATIONOBJECTS_ENUM_HH_GMAPPINGTAGTYPE
#define _COMMNAVIGATIONOBJECTS_ENUM_HH_GMAPPINGTAGTYPE

#include <CommNavigationObjects/gen/enumGMappingTagTypeC.hh>
#include <string>
#include <ostream>

namespace CommNavigationObjects
{

class GMappingTagType
{
public:
	enum literal
	{

		SAVE_MAP = CommNavigationObjectsIDL::LITERAL_SAVE_MAP
	};

	GMappingTagType()
	{
	}

	GMappingTagType(literal t)
	{
		value = t;
	}

	operator literal()
	{
		return value;
	}

	GMappingTagType(CommNavigationObjectsIDL::ENUM_GMappingTagType t)
	{
		value = (literal) t;
	}

	bool operator ==(GMappingTagType& t)
	{
		return this->value == t.value;
	}

	CommNavigationObjectsIDL::ENUM_GMappingTagType get_value_idl()
	{
		return (CommNavigationObjectsIDL::ENUM_GMappingTagType) value;
	}

	literal get_value() const
	{
		return value;
	}

	std::string get_string() const
	{
		switch (value)
		{

		case SAVE_MAP:
			return "GMappingTagType::SAVE_MAP";
			break;

		default:
			return "UNKNOWN!";
			break;
		}
	}

private:
	literal value;
};

inline std::ostream &operator<<(std::ostream &os, const GMappingTagType &e)
{
	os << e.get_string();
	return os;
}

}
#endif
