/*
Copyright (C) 2014 Daniel Gastler

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef __MYEXCEPTION_H__
#define __MYEXCEPTION_H__

#include <exception> //for inheritance from std::exception
  
#include <string.h> //for memcpy, strlen.

#include <string> //std::string

#include <stdlib.h> // for malloc

#include <stdio.h>

//For SYS_gettid()
//#define _GNU_SOURCE          /* See feature_test_macros(7) */
#include <unistd.h>
#include <sys/syscall.h>    /* For SYS_xxx definitions */

//Backtrace code
#include <execinfo.h>

//abi demangling
#include <cxxabi.h>


//Error strings
#define DescriptionError "Description allocation failed.\n"

namespace FedException
{
  

  //Macro for derived exception classes
#define FedExceptionClass( ClassName , ClassDescription )		\
  class ClassName : public FedException::exBase {			\
  public:								\
    ClassName() throw() {Init();}					\
    ClassName(const ClassName & rh) throw() {Init();Copy(rh);}		\
    ClassName & operator=(const ClassName & rh) throw()			\
      {Init();Copy(rh);return *this;}					\
    ~ClassName() throw() {}						\
    const char * what() const throw() {return whatname;}		\
  private:								\
    void Init()								\
    {									\
      strcpy(whatname,ClassDescription);				\
    }									\
    char whatname[sizeof(ClassDescription)];				\
  };
  
  
  class exBase : public std::exception
    {
    public:
      //destructor
      virtual ~exBase() throw();
 
      //Returns a stack trace of where the exception happened
      const char * StackTrace() const throw();
      
      //Append info to the description message
      void Append(const char * buffer) throw();
      void Append(std::string str){Append(str.c_str());};
      const char * Description() const throw();

      //Return the what string
      virtual const char * what() const throw()  = 0;

      pid_t GetPID(){return PID;};

    protected:
      //Constructor is protected so only derived classes can call it
      exBase() throw() ;
      //Copy function for base class internals (called by derived)
      void Copy(const exBase & rh) throw();
	    
    private:
      //Copy constructor/ assignment operators are private and not implemented
      exBase(const exBase & rh) throw();
      exBase & operator=(const exBase & rh) throw();

      //Stack trace code
      void GenerateStackTrace() throw();
      void AppendStackLine(const char * line) throw(); //adds a eol char if possible

      //Description internals
      size_t descriptionSize;
      size_t descriptionUsed;
      char * descriptionBuffer;

      //Stack trace internals
      size_t stackSize;
      size_t stackUsed;
      char * stackBuffer;

      pid_t PID;
    };


//Derived exceptions


  FedExceptionClass(EventSizeError,"Event size > Total Data Size\n");
  FedExceptionClass(BadHeaderMagicWord,"Wrong magic word in header\n");
  FedExceptionClass(ReadBlockHeaderInputError,"Input out of range for block header read function\n");
  FedExceptionClass(BadBlockHeaderValue,"Block header value out of range");
  FedExceptionClass(PreParseError,"Something went wrong in PreParse");
  FedExceptionClass(GetMaskedFieldFailed,"Get masked field failed");
  FedExceptionClass(PreParseInputError,"Input for number of words for preparse out of range");
  FedExceptionClass(OutOfRange,"Input is out of range");
  FedExceptionClass(DataFormatError,"Value read in data out of bounds");
  FedExceptionClass(NULLPointer,"Pointer is NULL");
  FedExceptionClass(EndOfFile,"End of File");
  FedExceptionClass(BadAllocation,"Bad Allocation");
  FedExceptionClass(FileReadError,"Error reading file");
  FedExceptionClass(BadCRC,"CRC bad in data");

}



#endif
