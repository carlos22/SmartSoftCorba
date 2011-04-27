// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004 Christian Schlegel
//
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft Communication Library".
//  It provides standardized patterns for communication between
//  different components.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
//  (partly based on joint work with Robert Wörz)
//
// --------------------------------------------------------------------------

#ifndef _SMARTPARAMETER_HH
#define _SMARTPARAMETER_HH

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>

#include "smartErrno.hh"

namespace CHS {
  /** Exception class to describe errors while processing parameters in SmartParameter.
   */
  class ParameterError : public std::domain_error
  {
  public:
    ParameterError(const std::string & reason)
      : domain_error(reason)
      { };
  };


  /** Parameter management.
   *
   *  Supports storage of parameters in separate parameter files.
   *  Parameter files can be read by this class which then provides
   *  simple access to all kinds of parameters. Parameters can also
   *  be extracted from environment variables or be defined by command
   *  line arguments.
   *
   *  The prefix is used to group parameters and can also be set to
   *  the empty string. A parameter therefore is always referenced
   *  by its prefix and its name.
   *
   *  Prefix and parameter name are case insensitive !
   *
   *  Demonstrated in @ref example7
   */
  class SmartParameter {

  private:
    //
    // associative map to maintain parameters
    //
    std::map<std::pair<std::string,std::string>,std::string> parameter;

    //
    // helper function to read until next unescaped newline
    //
    void getline(std::istream &is, std::string &line) const;

  public:
    // Initialization

    /// Constructor
    SmartParameter();

    /// Destructor
    virtual ~SmartParameter();


    //
    // Adding Parameters
    //

    /** Appends an entry to the parameter list.
     *
     *  If same entry already exists overwrite it with the new value.
     */
    void addEntry(std::string prefix, std::string name, const std::string &value);

    /** Appends command line arguments to the parameter list.
     *
     *  If same entry already exists overwrite it with the new value.
     * Format of command line parameter: -parameter=value
     */
    void addCommandLine(const std::string &prefix, int argc, char * const * argv);

    /** Appends environment variable to the parameter list.
     *
     *  @param prefix prefix of parameter name (prefix.name)
     *  @param env is the name of the environment variable.
     *
     *  If same entry already exists overwrite it with the new value.
     *  Throw exception if environment variable unknown.
     */
    void addEnv(const std::string &prefix, const std::string &env) throw(CHS::ParameterError);

    /** Appends the contents of a parameter file to the parameter list.
     *  Entries which already exist are replaced by the new values.
     *  Throw exception if something went wrong (file does not exist
     *  or contains syntax errors).
     *
     *  Looks first in the current directory and then in $SMART_ROOT/etc
     *
     *  The file extension normally used for parameter files is "ini".
     *  There is no maximum length per line, newlines may be escaped to
     *  concatenate successive lines. The syntax is as follows:
     *
     *  @code
     *  --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
     *     [smartsoft]
     *
     *     # -------------------------------------
     *     # laser server parameter
     *     # -------------------------------------
     *
     *     # laser device
     *     [laser]
     *
     *     type         SICK LMS200
     *     use          Yes
     *
     *     baudrate     57600          # 1200/9600/19200/38400/57600
     *     parity       NONE
     *     data         8
     *     device       /dev/cua2      # /dev/cur2  /dev/cua2
     *
     *     interval     0.167          # poll interval in seconds
     *
     *  --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
     *  @endcode
     *
     *  ALL ENTRIES ARE CASE INSENSITIVE !
     *
     *  The first line always contains the identification which is "[smartsoft]".
     *  For compatibility reasons we also accept "[bParamFile]" and "[smartParamFile]".
     *
     *  The first string in the line is the parameter name. Everything else following
     *  is the value excluding any comment. Comments are invoked by a "#" and end with
     *  the line.
     *
     *  Square brackets invoke the next group. If parameters are defined before any
     *  group is named, then their group name is simply "" (empty group name).
     *
     *  Truth values can be defined in many different formats:
     *    - true  :   yes | y | true  | t | + | 1 | on
     *    - false :   no  | n | false | f | - | 0 | off
     */
    void addFile(const std::string &filename) throw(CHS::ParameterError);

    /** Append the contents of a parameter file to the parameter list.
     *
     *  The name of the parameter file is not directly given, but extracted from 
     *  the argument list. So you can start multiple instances of the same
     *  program each with a different parameter set.
     *
     *  The command line arguments are given by \a argc and \a argv and the
     *  string \a argname gives the name of the argument which contains
     *  the actual filename.
     *
     *  If \a allow_stdin is \c true (default) and the value of argument \a argname 
     *  is "stdin", the method will read the configuration data from stdin. 
     *
     *  @author Boris Kluge <kluge@faw.uni-ulm.de>
     */
    void addFile(int argc, char * const * argv, const std::string &argname, bool allow_stdin = true) throw(CHS::ParameterError);

    /** Append the contents of a parameter file to the parameter list.
     *  The stream is assumed to be connected to a parameter file.
     */
    void addFile(std::istream &is) throw(CHS::ParameterError);

    //
    // helper methods
    //

    /// print all currently known parameters with their values
    void print(void);

    //
    // Retrieving parameters out of parameter list
    //

    /** Retrieve double casted value of parameter name
     *
     * throw "ParameterError" if parameter not known or conversion fails
     */
    double getDouble(std::string prefix, std::string parameter) const throw(CHS::ParameterError);

    /** Retrieve double casted value of parameter name
     *
     * leave \a d alone and return false,
     * if parameter not known or conversion fails
     */
    template<class Float>
    inline bool getDouble(const std::string &prefix, const std::string &parameter, Float &d) const throw();

    /** Retrieve int casted value of parameter name
     *
     *  throw "ParameterError" if parameter not known or conversion fails
     */
    int getInt(std::string prefix, std::string parameter) const throw(CHS::ParameterError);

    /** Retrieve int casted value of parameter name
     *
     *  leave \a i alone and return false,
     *  if parameter not known or conversion fails
     */
    template<class Integer>
    inline bool getInt(const std::string &prefix, const std::string &parameter, Integer &i) const throw();

    /** Retrieve truth value of parameter name.
     *
     * throw "ParameterError" if parameter not known or conversion fails
     */
    bool getTruthValue(std::string prefix, std::string parameter) const throw(CHS::ParameterError);

    /** Retrieve truth value of parameter name.
     *
     * leave \a b alone and return false,
     * if parameter not known or conversion fails
     */
    bool getTruthValue(const std::string &prefix, const std::string &parameter, bool &b) const throw();

    /** Retrieve string value of parameter name
     *
     *  throw "ParameterError" if parameter not known
     */
    std::string getString(std::string prefix, std::string parameter) const throw(CHS::ParameterError);
    
    /** Retrieve string value of parameter name
     *
     *  leave \a s alone and return false if parameter not known
     */
    bool getString(const std::string &prefix, const std::string &parameter, std::string &s) const throw();
    
    /** @internal return the begin Iterator from our map.
     *
     *  subject to change without notice.
     */
    std::map<std::pair<std::string,std::string>,std::string>::const_iterator
    begin();

    /** @internal return the end Iterator from our map.
     *
     *  subject to change without notice.
     */
    std::map<std::pair<std::string,std::string>,std::string>::const_iterator
    end();

  };

  template<class Float>
  inline bool CHS::SmartParameter::getDouble(const std::string &prefix, const std::string &param, Float &d) const throw()
  {
    try {
      const Float tmp = getDouble(prefix, param);
      d = tmp;
      return true;
    } catch(CHS::ParameterError e) {
      std::cerr << e.what() << std::endl;
      return false;
    }
  }

  template<class Integer>
  inline bool CHS::SmartParameter::getInt(const std::string &prefix, const std::string &param, Integer &i) const throw()
  {
    try {
      const Integer tmp = getInt(prefix, param);
      i = tmp;
      return true;
    } catch(CHS::ParameterError e) {
      std::cerr << e.what() << std::endl;
      return false;
    }
  }
}


#endif // _SMARTPARAMETER_HH

