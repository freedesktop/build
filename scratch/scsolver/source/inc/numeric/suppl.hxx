/*************************************************************************
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Kohei Yoshida.
 *    1039 Kingsway Dr., Apex, NC 27502, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/


#ifndef _SUPPL_HXX_
#define _SUPPL_HXX_

#ifndef TESTCASE
#include <cppuhelper/implementationentry.hxx>
#include <global.hxx>
#endif

#ifdef TESTCASE
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

typedef long sal_Int32;
typedef unsigned long sal_uInt32;
typedef char sal_Char;

namespace {

std::string repeatString( const sal_Char* str, unsigned int nNum )
{
	std::ostringstream os;
	for ( unsigned int i = 0; i < nNum; ++i )
		os << str;
	return os.str();
}

template<typename T>
void printVector( const std::vector<T>& a )
{
	sal_uInt32 size = a.size();
	if ( size > 0 )
		for ( sal_uInt32 i = 0; i < size; ++i )
			std::cout << a.at( i ) << " ";
	else
		std::cout << "[empty] ";
}

}
#endif

#endif
