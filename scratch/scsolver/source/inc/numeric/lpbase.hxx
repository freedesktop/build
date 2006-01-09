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


#ifndef _LPBASE_HXX_
#define _LPBASE_HXX_

#include <numeric/matrix.hxx>

namespace scsolver { namespace numeric { namespace opres { namespace lp {

class Model;

class ConstraintError : public std::exception
{
public:
	virtual const char* what() const throw();
};

class AssertionWrong : public std::exception
{
public:
	virtual const char* what() const throw();
};

class ModelInfeasible : public std::exception
{
public:
	virtual const char* what() const throw();
};

class BaseAlgorithm
{
public:
	BaseAlgorithm();
	virtual ~BaseAlgorithm() = 0;

	virtual void solve() = 0;

	Model* getModel() const { return m_pModel; }
	void setModel( Model* p ) { m_pModel = p; }
	
	Matrix getSolution() const { return m_Solution; }
	void setSolution( const Matrix& );

private:
	Model* m_pModel;
	Matrix m_Solution;
};

}}}}

#endif
