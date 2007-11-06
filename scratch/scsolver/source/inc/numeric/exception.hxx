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

#ifndef _SCSOLVER_NUMERIC_EXCEPTION_HXX_
#define _SCSOLVER_NUMERIC_EXCEPTION_HXX_

#include <exception>

namespace scsolver { namespace numeric {

class ConstraintError : public ::std::exception
{
public:
	virtual const char* what() const throw();
};

class AssertionWrong : public ::std::exception
{
public:
	virtual const char* what() const throw();
};

class ModelInfeasible : public ::std::exception
{
public:
	virtual const char* what() const throw();
};

class MaxIterationReached : public ::std::exception
{
public:
	virtual const char* what() const throw();
};

class IterationTimedOut : public ::std::exception
{
public:
	virtual const char* what() const throw();
};

/**
 * This exception is thrown when the caller attempts to retrieve a boundary
 * value of a decision variable when it's not bounded.
 */
class UnboundedException : public ::std::exception
{
public:
    virtual const char * what() const throw();
};

}}


#endif
