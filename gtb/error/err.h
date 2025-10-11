#ifndef _ERR_H
#define _ERR_H

/*
 * A generic error class that is thrown by tools
 * and applications.
 *
 * USAGE:
 *   throw CErr(...)
 *   will write a debug message the the debug console.
 *   if you wish to remove it, #define NO_ERR_DEBUG_OUTPUT
 *
 * Purpose:
 *   Ease error handling
 *   Help with debug
 */

#include <gtb/common.hpp>
#include <iostream>
#include <string>
#include <stdio.h>

GTB_BEGIN_NAMESPACE

void DbgOut(const std::string& msg);

class CErr
{
public:
  CErr(const CErr& err) : 
    m_description(err.m_description),
    m_reason(err.m_reason) 
  {
  }

  CErr(int a_reason, std::string a_description = "") :
    m_description(a_description),
    m_reason(a_reason) 
  {
//    DbgOut(m_description);
  }

  CErr(std::string a_description) :
    m_description(a_description),
    m_reason(-1) 
  {
//    DbgOut(m_description);
  }

    const char* c_description() const { return m_description.c_str(); }
	const std::string& description() const { return m_description; }

    operator int() const { return m_reason; }
    operator const std::string& () const { return m_description; }

    friend std::ostream& operator << (std::ostream& s, const CErr& v);
protected:
  std::string m_description;
  int m_reason;
};

GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/error/err.inl>
#endif

#endif // _ERR_H
