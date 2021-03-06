//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.8.0
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
// This file is generated once. Modify this file to your needs. 
// If you want the toolchain to re-generate this file, please 
// delete it before running the code generator.
//--------------------------------------------------------------------------

#ifndef _COMM_COMMSAMPLEOBJECTS_COMMSAMPLEPRINT_HH
#define _COMM_COMMSAMPLEOBJECTS_COMMSAMPLEPRINT_HH

#include <string>
#include <iostream>
#include <iomanip>

// include files of classes

// include header-files of included communication objects
#include <CommSampleObjects/commSampleTime.hh>

// include enums


// include client-side file generated by IDL compiler
#include "CommSampleObjects/gen/smartCommSamplePrintC.hh"

namespace CommSampleObjects
{

class CommSamplePrint
{
protected:
	CommSampleObjectsIDL::CommSamplePrint idl_CommSamplePrint;

public:
	CommSamplePrint()
	{
	}

	CommSamplePrint(const CommSampleObjectsIDL::CommSamplePrint &obj) :
		idl_CommSamplePrint(obj)
	{
	}

	virtual ~CommSamplePrint()
	{
	}

	void get(CORBA::Any &a) const;
	void set(const CORBA::Any &a);

	inline const CommSampleObjectsIDL::CommSamplePrint &get() const
	{
		return idl_CommSamplePrint;
	}

	inline void set(const CommSampleObjectsIDL::CommSamplePrint &obj)
	{
		idl_CommSamplePrint = obj;
	}

	static inline std::string identifier(void)
	{
		return "CommSampleObjects::CommSamplePrint";
	}

	//
	// user interfaces
	void get(unsigned int&, unsigned int&, unsigned int&,std::string& text) const;
	void set(unsigned int, unsigned int, unsigned int,std::string);
	void print(std::ostream &os = std::cout) const;
};

}
#endif
