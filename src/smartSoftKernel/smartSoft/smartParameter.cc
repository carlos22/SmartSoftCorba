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

#include <sstream>

#include "smartParameter.hh"

CHS::SmartParameter::SmartParameter()
{
  parameter.clear();
}

CHS::SmartParameter::~SmartParameter()
{
}

std::map<std::pair<std::string,std::string>,std::string>::const_iterator
CHS::SmartParameter::begin()
{
  return parameter.begin();
}

std::map<std::pair<std::string,std::string>,std::string>::const_iterator
CHS::SmartParameter::end()
{
  return parameter.end();
}


void CHS::SmartParameter::addEntry(std::string prefix, std::string param, const std::string &value)
{
  transform (param.begin(),  param.end(),  param.begin(),  ::tolower);
  transform (prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  std::pair<std::string,std::string> p(prefix,param);

  parameter[p] = value;
}

void CHS::SmartParameter::addCommandLine(const std::string &prefix, int argc, char * const * argv)
{
  std::string a;
  unsigned int p;
  int i;

  std::string param, value;

  if (argc < 1) {
    // no additional command line arguments

    return;
  } else {
    // check additional command line arguments

    i = 1;
    argc--;

    while((argc--) && (argv[i])) {
      // convert from c to c++ string style
      a = argv[i];

      // command line argument must begin with '-'
      if (a[0] != '-') {
        i++;
        continue;
      }

      // remove leading '-'
      a.erase(0,1);

      p = a.find("=");
      if (p != std::string::npos) {
        // valid <parameter=value> command line argument
        param = a.substr(0,p);
        value = a.substr(p+1,std::string::npos);

        this->addEntry(prefix,param,value);
      }

      // process next argument
      i++;
    }
  }
}

void CHS::SmartParameter::addEnv(const std::string &prefix, const std::string &env) throw(CHS::ParameterError)
{
  const char *value;

  value = getenv(env.c_str());

  if (value) {
    std::string v = value;

    this->addEntry(prefix,env,v);
  } else {
    throw CHS::ParameterError("Parameter not found in environment");
  }
}

void CHS::SmartParameter::addFile(const std::string &filename) throw(CHS::ParameterError)
{
  std::string name1, name2;
  const char *env;
  std::ifstream t;

  //
  // open file
  //
  name1 = "";
  name1.append("./");
  name1.append(filename);
  env = getenv("SMART_ROOT");
  if (env) {
    name2 = "";
    name2.append(env);
    name2.append("/etc/");
    name2.append(filename);
  } else {
    name2 = "";
  }

  t.open(name1.c_str());
  if (!t.is_open()) {
    t.open(name2.c_str());
    if (!t.is_open()) {
      std::ostringstream oss;
      oss << "SmartParameter: error: couldn't find parameter file <" << filename << ">" << std::endl;
      std::cerr << oss.str();
      throw ParameterError(oss.str());
    }
  }

  //
  // read and parse the file
  //
  addFile(t);
}

void CHS::SmartParameter::addFile(int argc, char *const *argv, const std::string &argname, bool allow_stdin) throw(CHS::ParameterError)
{
  SmartParameter tmp_param;
  std::string filename;
  tmp_param.addCommandLine("cmdline",argc,argv);
  if(tmp_param.getString("cmdline",argname,filename))
  {
    if(allow_stdin && (filename=="stdin"))
    {
      addFile(std::cin);
    }
    else
    {
      std::ifstream ifs;
      ifs.open(filename.c_str());
      if(ifs.good())
      {
        addFile(ifs);
      }
      else
      {
        std::ostringstream oss;
        oss << "SmartParameter: error: couldn't open parameter file <" << filename << ">" << std::endl;
        throw ParameterError(oss.str());
      }
    }
  }
  else
  {
    std::ostringstream oss;
    oss << "SmartParameter: error: couldn't find command line parameter \"" << argname << "\"." << std::endl;
    throw ParameterError(oss.str());
  }
}

void CHS::SmartParameter::addFile(std::istream &is) throw(CHS::ParameterError)
{
//  char         content[512];
  std::string  line;
  unsigned int index;
  unsigned int start;

  std::string prefix;
  std::string param;
  std::string value;

  //
  // read first line
  //
  if (is.eof()) {
    std::cerr << "SmartParameter: error: empty file" << std::endl;
    throw ParameterError("SmartParameter: error: empty file");
  }

//  is.getline(content,512);
//  line = content;
  this->getline(is,line);

  if ((line != "[smartsoft]") && (line != "[bParamFile]") && (line != "[smartParamFile]")) {
    std::cerr << "SmartParameter: error: seems not to be a parameter file" << std::endl;
    throw ParameterError( "SmartParameter: error: seems not to be a parameter file");
  }

  while (!is.eof()) {

//    is.getline(content,512);
//    line = content;
    this->getline(is,line);

    //
    //
    // interpret a single line
    //
    //
    index = 0;

    // skip leading white space
    while (isspace(line[index])) index++;

    // comment or blank line => skip this line
    if ((index >= line.size()) || (line.at(index)==';') || (line.at(index)=='#')) continue;

    //
    // group prefix
    //
    if (line.at(index)=='[') {
      index++;
      start = index;
      while ((index < line.size())  && (line.at(index)!=';')  && (line.at(index)!='#')  &&
             (line.at(index)!=' ')  && (line.at(index)!='\t') && (line.at(index)!='\n') &&
             (line.at(index)!='\r') && (line.at(index)!=']')) index++;

      prefix = line.substr(start,index-start);

      continue;
    }

    //
    // no group prefix, therefore we expect the parameter name
    //
    start = index;

    while ((index < line.size())  && (line.at(index)!=';')  && (line.at(index)!='#')  &&
           (line.at(index)!=' ')  && (line.at(index)!='\t') && (line.at(index)!='\n') &&
           (line.at(index)!='\r')) index++;

    param = line.substr(start,index-start);

    if (!isspace(line[index])) {
      //
      // line terminates without giving a value => value is set to empty string
      //
      std::string empty;

      this->addEntry(prefix,param,empty);

      continue;
    }

    //
    // found parameter name which is followed by a value
    //

    // skip white space in front of value
    while (isspace(line[index])) index++;

    start = index;

    // do not accept blank as delimiter since then one could not have multiple word
    // values
    while ((index < line.size())  && (line.at(index)!=';')  && (line.at(index)!='#') &&
           (line.at(index)!='\n') && (line.at(index)!='\r')) index++;

    // now remove white space at the end since we read until we reached a hard delimiter
    index--;
    while (isspace(line[index])) index--;

    value = line.substr(start,index-start+1);

    this->addEntry(prefix,param,value);
  }
}

void CHS::SmartParameter::print(void)
{
  std::map<std::pair<std::string,std::string>,std::string>::iterator i;

  for (i=parameter.begin();i!=parameter.end();++i) {
    std::cout << "<" << (*i).first.first << "><" << (*i).first.second << "><" << (*i).second << ">" << std::endl;
  }
}

double CHS::SmartParameter::getDouble(std::string prefix, std::string param) const throw(ParameterError)
{
  transform (param.begin(),  param.end(),  param.begin(),  ::tolower);
  transform (prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  std::pair<std::string,std::string> p(prefix,param);

  std::map<std::pair<std::string,std::string>,std::string>::const_iterator i;
  i = parameter.find(p);

  if (i == parameter.end()) {
    std::ostringstream oss;
    oss << "Parameter not found: double [" << prefix  << "] " << param;
    throw ParameterError(oss.str());
  }

  const char *value = (*i).second.c_str();
  char *end;
  const double d = strtod(value, &end);

  if (end == value) {
    std::ostringstream oss;
    oss << "Conversion failed: double [" << prefix  << "] " << param;
    throw ParameterError(oss.str());
  }

  return d;
}

int CHS::SmartParameter::getInt(std::string prefix, std::string param) const throw(ParameterError)
{
  transform (param.begin(),  param.end(),  param.begin(),  ::tolower);
  transform (prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  std::pair<std::string,std::string> p(prefix,param);

  std::map<std::pair<std::string,std::string>,std::string>::const_iterator i;
  i = parameter.find(p);

  if (i == parameter.end()) {
    std::ostringstream oss;
    oss << "Parameter not found: integer [" << prefix  << "] " << param;
    throw ParameterError(oss.str());
  }

  const char *value = (*i).second.c_str();
  char *end;
  const long int l = strtol(value, &end, 0); // accept hex ("0x...") and oct ("0...") representation

  if (end == value) {
    std::ostringstream oss;
    oss << "Conversion failed: integer [" << prefix  << "] " << param;
    throw ParameterError(oss.str());
  }

  return l;
}

bool CHS::SmartParameter::getTruthValue(std::string prefix, std::string param) const throw(ParameterError)
{
  std::string result;

  transform (param.begin(),  param.end(),  param.begin(),  ::tolower);
  transform (prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  std::pair<std::string,std::string> p(prefix,param);

  std::map<std::pair<std::string,std::string>,std::string>::const_iterator i;
  i = parameter.find(p);

  if (i == parameter.end()) {
    std::ostringstream oss;
    oss << "Parameter not found: boolean value [" << prefix  << "] " << param;
    throw ParameterError(oss.str());
  }

  result = (*i).second;
  transform (result.begin(), result.end(), result.begin(), ::tolower);

  if (result=="yes" || result=="true" || result=="y" || result=="t" || result=="on" || result=="+") {
    return true;
  } else if (result=="no" || result=="false" || result=="n" || result=="f" || result=="off" || result=="-") {
    return false;
  } else if (atoi(result.c_str())) {
    return true;
  } else {
    return false;
  }
}

bool CHS::SmartParameter::getTruthValue(const std::string &prefix, const std::string &param, bool &b) const throw()
{
  try {
    const bool tmp = getTruthValue(prefix, param);
    b = tmp;
    return true;
  } catch(CHS::ParameterError e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

std::string CHS::SmartParameter::getString(std::string prefix, std::string param) const throw(ParameterError)
{
  transform (param.begin(),  param.end(),  param.begin(),  ::tolower);
  transform (prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  std::pair<std::string,std::string> p(prefix,param);

  std::map<std::pair<std::string,std::string>,std::string>::const_iterator i;
  i = parameter.find(p);

  if (i == parameter.end()) {
    std::ostringstream oss;
    oss << "Parameter not found: string [" << prefix  << "] " << param;
    throw ParameterError(oss.str());
  }
  return ((*i).second);
}

bool CHS::SmartParameter::getString(const std::string &prefix, const std::string &param, std::string &s) const throw()
{
  try {
    const std::string tmp = getString(prefix, param);
    s = tmp;
    return true;
  } catch(CHS::ParameterError e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

void CHS::SmartParameter::getline(std::istream &is, std::string &line) const
{
  std::ostringstream oss;
  bool escaped = false; // "true" means last character was a backslash 
                        // which has not yet been appended to the buffer
  char ch;
  while(!is.get(ch).eof())
  {
    if(ch=='\\')
    {
      if(escaped)
      {
        oss.put(ch); // write the previous backslash, not the current.
      }
      escaped = true; // do not append backslash to buffer until next character is read
    }
    else
    {
      if(!escaped)
      {
        if(ch=='\n') break; // done.
        oss.put(ch);
      }
      else
      {
        if(ch!='\n') 
        {
          oss.put('\\');
          oss.put(ch);
        }
        // else ignore newline
      }
      escaped = false; // next character isn't escaped
    }
  }
  line = oss.str();
}
