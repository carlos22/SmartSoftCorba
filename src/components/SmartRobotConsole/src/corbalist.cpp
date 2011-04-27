// --------------------------------------------------------------------------
//
//  Copyright (C) 2011 Matthias Lutz
//
//        schlegel@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft robot console component".
//
//  This file is based on the TAO nslist.cpp which comes with the
//  license below.
//
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
//  Copyright and Licensing Information for ACE(TM), TAO(TM), CIAO(TM), and
//  CoSMIC(TM)
//
//   [1]ACE(TM), [2]TAO(TM), [3]CIAO(TM), and [4]CoSMIC(TM) (henceforth
//   referred to as "DOC software") are copyrighted by [5]Douglas C.
//   Schmidt and his [6]research group at [7]Washington University,
//   [8]University of California, Irvine, and [9]Vanderbilt University,
//   Copyright (c) 1993-2007, all rights reserved. Since DOC software is
//   open-source, freely available software, you are free to use, modify,
//   copy, and distribute--perpetually and irrevocably--the DOC software
//   source code and object code produced from the source, as well as copy
//   and distribute modified versions of this software. You must, however,
//   include this copyright statement along with any code built using DOC
//   software that you release. No copyright statement needs to be provided
//   if you just ship binary executables of your software products.
//
//   You can use DOC software in commercial and/or binary software releases
//   and are under no obligation to redistribute any of your source code
//   that is built using DOC software. Note, however, that you may not do
//   anything to the DOC software code, such as copyrighting it yourself or
//   claiming authorship of the DOC software code, that will prevent DOC
//   software from being distributed freely using an open-source
//   development model. You needn't inform anyone that you're using DOC
//   software in your software, though we encourage you to let [10]us know
//   so we can promote your project in the [11]DOC software success
//   stories.
//
//   The [12]ACE, [13]TAO, [14]CIAO, and [15]CoSMIC web sites are
//   maintained by the [16]DOC Group at the [17]Institute for Software
//   Integrated Systems (ISIS) and the [18]Center for Distributed Object
//   Computing of Washington University, St. Louis for the development of
//   open-source software as part of the open-source software community. By
//   submitting comments, suggestions, code, code snippets, techniques
//   (including that of usage) and algorithms (collectively
//   ``Submissions''), submitters acknowledge that they have the right to
//   do so, that any such Submissions are given freely and unreservedly,
//   and that they waive any claims to copyright or ownership. In addition,
//   submitters acknowledge that any such Submission might become part of
//   the copyright maintained on the overall body of code that comprises
//   the DOC software. By making a Submission, submitter agree to these
//   terms. Moreover, submitters acknowledge that the incorporation or
//   modification of such Submissions is entirely at the discretion of the
//   moderators of the open-source DOC software projects or their
//   designees.
//
//   Submissions are provided by the submitter ``as is'' with no warranties
//   whatsoever, including any warranty of merchantability, noninfringement
//   of third party intellectual property, or fitness for any particular
//   purpose. In no event shall the submitter be liable for any direct,
//   indirect, special, exemplary, punitive, or consequential damages,
//   including without limitation, lost profits, even if advised of the
//   possibility of such damages. Likewise, DOC software is provided as is
//   with no warranties of any kind, including the warranties of design,
//   merchantability, and fitness for a particular purpose,
//   noninfringement, or arising from a course of dealing, usage or trade
//   practice. Washington University, UC Irvine, Vanderbilt University,
//   their employees, and students shall have no liability with respect to
//   the infringement of copyrights, trade secrets or any patents by DOC
//   software or any part thereof. Moreover, in no event will Washington
//   University, UC Irvine, or Vanderbilt University, their employees, or
//   students be liable for any lost revenue or profits or other special,
//   indirect and consequential damages.
//
//   DOC software is provided with no support and without any obligation on
//   the part of Washington University, UC Irvine, Vanderbilt University,
//   their employees, or students to assist in its use, correction,
//   modification, or enhancement. A [19]number of companies around the
//   world provide commercial support for DOC software, however.
//
//   DOC software is Y2K-compliant, as long as the underlying OS platform
//   is Y2K-compliant. Likewise, DOC software is compliant with the new US
//   daylight savings rule passed by Congress as "The Energy Policy Act of
//   2005," which established new daylight savings times (DST) rules for
//   the United States that expand DST as of March 2007. Since DOC software
//   obtains time/date and calendaring information from operating systems
//   users will not be affected by the new DST rules as long as they
//   upgrade their operating systems accordingly.
//
//   The names ACE(TM), TAO(TM), CIAO(TM), CoSMIC(TM), Washington
//   University, UC Irvine, and Vanderbilt University, may not be used to
//   endorse or promote products or services derived from this source
//   without express written permission from Washington University, UC
//   Irvine, or Vanderbilt University. Further, products or services
//   derived from this source may not be called ACE(TM), TAO(TM), CIAO(TM),
//   or CoSMIC(TM) nor may the name Washington University, UC Irvine, or
//   Vanderbilt University appear in their names, without express written
//   permission from Washington University, UC Irvine, and Vanderbilt
//   University.
//
//   If you have any suggestions, additions, comments, or questions, please
//   let [20]me know.
//
//   [21]Douglas C. Schmidt
//     _________________________________________________________________
//
//   Back to the [22]ACE home page.
//
//References
//
//   1. http://www.cs.wustl.edu/~schmidt/ACE.html
//   2. http://www.cs.wustl.edu/~schmidt/TAO.html
//   3. http://www.dre.vanderbilt.edu/CIAO/
//   4. http://www.dre.vanderbilt.edu/cosmic/
//   5. http://www.dre.vanderbilt.edu/~schmidt/
//   6. http://www.cs.wustl.edu/~schmidt/ACE-members.html
//   7. http://www.wustl.edu/
//   8. http://www.uci.edu/
//   9. http://www.vanderbilt.edu/
//  10. mailto:doc_group@cs.wustl.edu
//  11. http://www.cs.wustl.edu/~schmidt/ACE-users.html
//  12. http://www.cs.wustl.edu/~schmidt/ACE.html
//  13. http://www.cs.wustl.edu/~schmidt/TAO.html
//  14. http://www.dre.vanderbilt.edu/CIAO/
//  15. http://www.dre.vanderbilt.edu/cosmic/
//  16. http://www.dre.vanderbilt.edu/
//  17. http://www.isis.vanderbilt.edu/
//  18. http://www.cs.wustl.edu/~schmidt/doc-center.html
//  19. http://www.cs.wustl.edu/~schmidt/commercial-support.html
//  20. mailto:d.schmidt@vanderbilt.edu
//  21. http://www.dre.vanderbilt.edu/~schmidt/
//  22. http://www.cs.wustl.edu/ACE.html
// --------------------------------------------------------------------------



#include <ace/SString.h>
#include <orbsvcs/CosNamingC.h>
#include <tao/Endpoint.h>
#include <tao/Profile.h>
#include <tao/Stub.h>
#include <tao/ORB_Constants.h>
#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdio.h>
#include <ace/Argv_Type_Converter.h>
#include <ace/OS_NS_ctype.h>
#include<iostream>
#include <vector>


class port_s{
	public:
		std::string typ;
		std::string serviceName;
        std::string CommObj;
};



class component_s{
	public:
		std::string componentName;
		std::vector<port_s> ports;
};


std::vector<component_s> components_list;



  bool
    showIOR = false,    // Default decodes endpoints
    showCtxIOR = false, // Default no displaying naming context ior
    noLoops = false;    // Default draw loopback arrows
  const char
    *myTree = "|",      // Default string to draw tree "tram-lines"
    *myNode = "+";      // Default string to draw tree node end-points
  int
    sizeMyTree,         // Initialised by main to strlen (myTree)
    sizeMyNode,         // Initialised by main to strlen (myNode)
    maxDepth= 0;        // Limit to display depth (default unlimited)

void list_context (const CosNaming::NamingContext_ptr,
                     int level);

 //==========================================================================
  class NestedNamingContexts
  {
  public:
    static void add (const CosNaming::NamingContext_ptr nc)
    {
      (void) new NestedNamingContexts( nc ); // This is not a leak (see constructor)
    }

    static void remove ()
    {
      const NestedNamingContexts *const pThisOne= pBottom;
      delete pThisOne; // i.e. delete pBottom; Attempt to stop over-optimisation by BORLAND
    }

    static int hasBeenSeen (const CosNaming::NamingContext_ptr nc)
    {
      int level= 1;
      for (const NestedNamingContexts *pMyNode= pBottom;
           pMyNode;
           ++level, pMyNode= pMyNode->pNext)
        {
          if (pMyNode->pnc->_is_equivalent (nc))
            return level; // Loops backwards this number of levels
        }

      return 0; // Not seen before
    }

  private:
    static const NestedNamingContexts
      *pBottom; // Our current lowest level
    const CosNaming::NamingContext_ptr
      pnc; // This level's Naming Context
    const NestedNamingContexts
      *const pNext; // Next highest level

    NestedNamingContexts (const CosNaming::NamingContext_ptr nc)
      :pnc(nc), pNext(pBottom) // Adds the old list to this!
    {
      this->pBottom= this; // Doesn't leak this (it's the new start)
    }

    ~NestedNamingContexts ()
    {
      this->pBottom= this->pNext; // this node removed from list.
    }

    // Outlaw copying
    NestedNamingContexts (const NestedNamingContexts &);
    NestedNamingContexts &operator= (const NestedNamingContexts &);
  };
  const NestedNamingContexts *NestedNamingContexts::pBottom= 0;

  //==========================================================================
  void
  get_tag_name (CORBA::ULong tag, ACE_CString& tag_string)
  {
    if (tag == IOP::TAG_INTERNET_IOP)
      tag_string = "IIOP";
    else if (tag == TAO_TAG_UIOP_PROFILE)
      tag_string = "UIOP";
    else if (tag == TAO_TAG_SHMEM_PROFILE)
      tag_string = "SHMEM";
#ifdef TAO_TAG_DIOP_PROFILE
    else if (tag == TAO_TAG_DIOP_PROFILE)
      tag_string = "GIOP over UDP";
#endif /* TAO_TAG_DIOP_PROFILE */
    else
      {
        char buffer[32]= {'\0'};
        ACE_OS::sprintf( buffer, "%08x (%u)", tag, tag );
        (tag_string = "Unknown tag: ") += buffer;
      }
  }


 //==========================================================================
  // Display NS entries from a finite list.
  void
  show_chunk (const CosNaming::NamingContext_ptr nc,
              const CosNaming::BindingList &bl,
              int level)
  {
    for (CORBA::ULong i = 0; i < bl.length (); ++i)
      {


	//ACE_DEBUG ((LM_DEBUG, "level: %d",level));
        if(level == 2)
	{
	  //ACE_DEBUG ((LM_DEBUG, "\nComponent Name: %s \n",bl[i].binding_name[0].id.in ()));
	  component_s comp;
	  comp.componentName = bl[i].binding_name[0].id.in ();
	  components_list.push_back(comp);
        }
        if(level == 3)
	{
	  //ACE_DEBUG ((LM_DEBUG, "\nPortType: %s \n",bl[i].binding_name[0].id.in ()));
	  port_s port;
	  port.typ = bl[i].binding_name[0].id.in ();
	  components_list.back().ports.push_back(port);
        }
        if(level == 4)
	{
	  //ACE_DEBUG ((LM_DEBUG, "\nService Name: %s \n",bl[i].binding_name[0].id.in ()));
	  components_list.back().ports.back().serviceName = bl[i].binding_name[0].id.in ();
        }
        if(level == 5)
        {
          //ACE_DEBUG ((LM_DEBUG, "\nObj Name: %s \n",bl[i].binding_name[0].id.in ()));
          components_list.back().ports.back().CommObj = bl[i].binding_name[0].id.in ();
          port_s port;
	  port.typ = "empty";
	  components_list.back().ports.push_back(port);
        }

	/*
        int count;
        for (count= 0; count < level-1; ++count)
          ACE_DEBUG ((LM_DEBUG, "%s ", myTree));
        ACE_DEBUG ((LM_DEBUG, "%s %s ", myNode,
                   bl[i].binding_name[0].id.in ()));


        if (bl[i].binding_name[0].kind[0])
          ACE_DEBUG ((LM_DEBUG,
                     " (Kind: %s)",
                     bl[i].binding_name[0].kind.in ()));
        */
        CosNaming::Name Name;
        Name.length (1);
        Name[0].id =
          CORBA::string_dup (bl[i].binding_name[0].id);
        Name[0].kind =
          CORBA::string_dup (bl[i].binding_name[0].kind);

        CORBA::Object_var obj = nc->resolve (Name);

        // If this is a context node, follow it down to the next level...
        if (bl[i].binding_type == CosNaming::ncontext)
          {
            //ACE_DEBUG ((LM_DEBUG, ": Naming context"));

            CosNaming::NamingContext_var xc;
            try
              {
                xc = CosNaming::NamingContext::_narrow (obj.in ());
              }
            catch (const CORBA::OBJECT_NOT_EXIST& not_used)
              {
                 ACE_UNUSED_ARG (not_used);
                 xc= 0;
                ACE_DEBUG ((LM_DEBUG, " {Destroyed}"));
              }

            if (const int backwards= NestedNamingContexts::hasBeenSeen (xc.in ()))
              {
                ACE_DEBUG ((LM_DEBUG, " (Binding Loop)\n"));
                if (!noLoops)
                  {
                    int count;
                    for (count= 0; count < (level - backwards); ++count)
                      ACE_DEBUG ((LM_DEBUG, "%s ", myTree));
                    ACE_DEBUG ((LM_DEBUG, "^"));
                    int chars;
                    while (++count < level)
                      for (chars= 0; chars <= sizeMyTree; ++chars)
                        ACE_DEBUG ((LM_DEBUG, "-"));
                    for (chars= 0; chars < sizeMyNode; ++chars)
                      ACE_DEBUG ((LM_DEBUG, "-"));
                    ACE_DEBUG ((LM_DEBUG, "^\n"));
                  }
              }
            else
             {
                if (maxDepth != level)
                  {
                    //ACE_DEBUG ((LM_DEBUG, "\n"));
                    if (xc.in ())
                      {
                        list_context (xc.in (), level + 1);
                      }
                  }
                else
                  ACE_DEBUG ((LM_DEBUG, " {Max depth reached}\n"));
              }
          }
        // Mark this node as a reference
        else
          {
            //ACE_DEBUG ((LM_DEBUG, ": Object Reference"));
            //if (CORBA::is_nil (obj.in ()))
            //  ACE_DEBUG ((LM_DEBUG, " {Null}"));
            //ACE_DEBUG ((LM_DEBUG, "\n"));
          }
      }
  }


  //==========================================================================
  void
  list_context (const CosNaming::NamingContext_ptr nc,
                const int level)
  {
    CosNaming::BindingIterator_var it;
    CosNaming::BindingList_var bl;
    const CORBA::ULong CHUNK = 100;

    NestedNamingContexts::add (nc);
    nc->list (CHUNK, bl, it);

    show_chunk (nc, bl.in (), level);


    NestedNamingContexts::remove ();
  }

//============================================================================

std::vector< std::pair<std::string,std::string> > get_listOfComponentsWithState(){
	std::vector< std::pair<std::string,std::string> > list_of_state_clients;

  	for(unsigned int i =0; i<components_list.size(); i++)
	{
		//std::cout<<"\n===============================\n";
		//std::cout<<"Component : "<<components_list[i].componentName<<std::endl;
		for(unsigned int j = 0;j<components_list[i].ports.size();j++)
		  {
//			if(components_list[i].ports[j].serviceName == "state" || components_list[i].ports[j].serviceName == "stateServer")
			if(components_list[i].ports[j].CommObj == "CHS::smartStateRequest")
			{

				std::pair <std::string,std::string> tmp;
				tmp.first = components_list[i].componentName;
				tmp.second = components_list[i].ports[j].serviceName;
				list_of_state_clients.push_back(tmp);
			}
	  		//std::cout<<"Porttype: "<<components_list[i].ports[j].typ<<std::endl;
	  		//std::cout<<"Servicename: "<<components_list[i].ports[j].serviceName<<std::endl;
	  		//std::cout<<"Com obj: "<<components_list[i].ports[j].CommObj<<std::endl;
	   		//std::cout<<"\n";
		  }
  	}
        return list_of_state_clients;
}



int get_list (std::vector< std::pair<std::string,std::string> > &list, CHS::SmartComponent * component)
{
	CORBA::ORB_var orb_1 = component->orb;
  try
    {

      components_list.clear();

      // Initialise the lengths of the myTree and myNode draw strings.
      sizeMyTree= ACE_OS::strlen (myTree);
      sizeMyNode= ACE_OS::strlen (myNode);

      // Contact the name service
      CORBA::Object_var obj;
      obj = orb_1->resolve_initial_references ("NameService");

      CosNaming::NamingContext_var root_nc =
        CosNaming::NamingContext::_narrow (obj.in ());
      if (CORBA::is_nil (root_nc.in ()))
        {
          ACE_DEBUG ((LM_DEBUG,
                      "Error: nil root naming context\n"));
          return 1;
        }

      CORBA::String_var str =
        orb_1->object_to_string (root_nc.in ());
          list_context (root_nc.in (), 1);
    }
  catch (const CORBA::Exception& ex)
    {
      ex._tao_print_exception ("Exception in nslist");
      return -1;
    }


  std::vector< std::pair<std::string,std::string> > list_of_state_clients;

  list_of_state_clients = get_listOfComponentsWithState();

  list = list_of_state_clients;

  /*std::cout<<"Components_with_State:"<<std::endl;
  for(unsigned int i =0; i<list_of_state_clients.size(); i++)
  {
	  std::cout<<list_of_state_clients[i]<<std::endl;
  }*/

  return 0;
}
