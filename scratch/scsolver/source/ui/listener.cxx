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


#include <listener.hxx>
#include <solver.hxx>
#include <dialog.hxx>
#include <unoglobal.hxx>
#include <global.hxx>
#include <msgdlg.hxx>
#include <xcalc.hxx>

#include <memory>
#include <iostream>

#include <com/sun/star/awt/XTextComponent.hpp>

#include <com/sun/star/beans/XPropertySet.hpp>

#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XNotifyingDispatch.hpp>

#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/sheet/XRangeSelection.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

using namespace std;

namespace scsolver {

//---------------------------------------------------------------------------
// Class RngSelListener

RngSelListener::RngSelListener( BaseDialog* pDlg, RngBtnListener* pBtn, 
		const rtl::OUString& sEditName )
{
	m_pDlg = pDlg;
	m_pBtn = pBtn;
	m_sEditName = sEditName;
}

RngSelListener::~RngSelListener() throw()
{
}

void RngSelListener::disposing( const lang::EventObject& oEvt ) throw ( RuntimeException )
{
	Debug( "RngSelListener::disposing" );
}
	
void RngSelListener::done( const sheet::RangeSelectionEvent& oEvt ) throw ( RuntimeException )
{
	// Since all instances of RngSelListener receive an event notification when
	// a cell selection is done, I need to make sure that this instance is the right 
	// instance for the event.
	if ( m_pBtn->isEventOwner() )
	{
		rtl::OUString sRange = oEvt.RangeDescriptor;	// Get cell range expression

		Reference< uno::XInterface > oRngEdit = m_pDlg->getWidgetByName( m_sEditName );
		Reference< awt::XTextComponent > xComp( oRngEdit, UNO_QUERY );
		rtl::OUString sRangeOld = xComp->getText();
		xComp->setText( sRange );

		if ( !m_pDlg->doneRangeSelection() )
			xComp->setText( sRangeOld );

		m_pDlg->setVisible( true );
		m_pBtn->setEventOwner( false );
	}
}

void RngSelListener::aborted( const sheet::RangeSelectionEvent& oEvt ) throw ( RuntimeException )
{
	m_pDlg->setVisible( true );
}


//---------------------------------------------------------------------------
// Class RngBtnListener

RngBtnListener::RngBtnListener( BaseDialog* pDlg, 
		Reference< sheet::XRangeSelection > xRngSel, const rtl::OUString& sEditName )
		
		: ActionListener( pDlg ),
		
		m_xRngSel( xRngSel ),
		m_pRngSelListener( NULL ),
		m_sEditName( sEditName ),
		m_bEventOwner( false ),
		m_bSingleCell( false )
{
	registerRngSelListener();
}

RngBtnListener::~RngBtnListener() throw()
{
	if ( m_xRngSel.is() )
	{
		Debug( "removing selection listener" );
		
		// It appears that by explicitly removing the selection listener
		// its instance is automatically delete'd, which means a manual delete 
		// of the listener after the line below will cause a crash.
		m_xRngSel->removeRangeSelectionListener( m_pRngSelListener );
	}
	else
		Debug( "m_xRngSel == NULL!" );
}

void RngBtnListener::disposing( const lang::EventObject& oEvt )
		throw ( RuntimeException )
{
}
	
void RngBtnListener::actionPerformed( const awt::ActionEvent& oActionEvt ) 
	throw ( RuntimeException )
{
	if ( m_xRngSel != NULL )
	{
		Debug( "Do range selection" );
	
		uno::Sequence< beans::PropertyValue > aProp( 3 );
		uno::Any aValue;
		aValue <<= ascii( "Please select a range" );
		aProp[0].Name = ascii( "Title" );
		aProp[0].Value = aValue;
		aProp[1].Name = ascii( "CloseOnMouseRelease" );
		aValue <<= static_cast<sal_Bool>(false);
		aProp[1].Value = aValue;
// 		aProp[2].Name = ascii( "SingleCellMode" );
// 		aValue <<= m_bSingleCell;
// 		aProp[2].Value = aValue;
		setEventOwner( true );

		m_xRngSel->startRangeSelection( aProp );
		
		// Do NOT set the dialog invisible before starting range selection!
		getDialog()->setVisible( false );

		if ( getDialog()->getDialogName().equals( ascii( "ConstEditDialog" ) ) )
		{
			SolverDialog* pMainDlg = getDialog()->getSolverImpl()->getMainDialog();
			pMainDlg->setVisible( false );
		}
	}
	else
		Debug( "Range selection interface NULL" );
}

void RngBtnListener::registerRngSelListener()
{
	if ( m_pRngSelListener == NULL && m_xRngSel != NULL )
	{
		Debug( "Register range selection listener" );
		
		m_pRngSelListener = new RngSelListener( getDialog(), this, m_sEditName );
		m_xRngSel->addRangeSelectionListener( m_pRngSelListener );
	}
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Class SolveBtnListener

SolveBtnListener::SolveBtnListener( SolverDialog* pDlg ) : ActionListener( pDlg )
{
}

SolveBtnListener::~SolveBtnListener() throw()
{
}

void SolveBtnListener::disposing( const lang::EventObject& oEvt )
	throw ( RuntimeException )
{
}

void SolveBtnListener::actionPerformed( const awt::ActionEvent& oEvt )
	throw ( RuntimeException )
{
	getDialog()->getSolverImpl()->solveModel();
}


//---------------------------------------------------------------------------
// Class CloseBtnListener

CloseBtnListener::CloseBtnListener( SolverDialog* pDlg ) : ActionListener( pDlg )
{
}

CloseBtnListener::~CloseBtnListener() throw()
{
}

void CloseBtnListener::disposing( const lang::EventObject& oEvt )
	throw ( RuntimeException )
{
}
	
void CloseBtnListener::actionPerformed( const awt::ActionEvent& oEvt )
	throw ( RuntimeException )
{
	getDialog()->setVisible( false );
}


//---------------------------------------------------------------------------
// Class SaveBtnListener

SaveBtnListener::SaveBtnListener( SolverDialog* pDlg ) : ActionListener( pDlg )
{
}

SaveBtnListener::~SaveBtnListener() throw()
{
}

void SaveBtnListener::disposing( const lang::EventObject& oEvt )
	throw ( RuntimeException )
{
}
	
void SaveBtnListener::actionPerformed( const awt::ActionEvent& oEvt )
	throw ( RuntimeException )
{
	getDialog()->getSolverImpl()->getMainDialog()->saveModelToDocument();
}


//---------------------------------------------------------------------------
// Class LoadBtnListener

LoadBtnListener::LoadBtnListener( SolverDialog* pDlg ) : ActionListener( pDlg )
{
}

LoadBtnListener::~LoadBtnListener() throw()
{
}

void LoadBtnListener::disposing( const lang::EventObject& oEvt )
	throw ( RuntimeException )
{
}
	
void LoadBtnListener::actionPerformed( const awt::ActionEvent& oEvt )
	throw ( RuntimeException )
{
	getDialog()->getSolverImpl()->getMainDialog()->loadModelFromDocument();
}


//---------------------------------------------------------------------------
// Class ResetBtnListener

ResetBtnListener::ResetBtnListener( SolverDialog* pDlg ) : ActionListener( pDlg ),
	m_pDlg( NULL )
{
}

ResetBtnListener::~ResetBtnListener() throw()
{
}

void ResetBtnListener::disposing( const lang::EventObject& oEvt )
	throw ( RuntimeException )
{
}
	
void ResetBtnListener::actionPerformed( const awt::ActionEvent& oEvt )
	throw ( RuntimeException )
{
	getDialog()->getSolverImpl()->getMainDialog()->reset();
}


//---------------------------------------------------------------------------
// Class ConstEditBtnListener

ConstEditBtnListener::ConstEditBtnListener( SolverDialog* pDlg, ConstButtonType eType ) :
		ActionListener( pDlg )
{
	m_eBtnType = eType;
}

ConstEditBtnListener::~ConstEditBtnListener() throw()
{
}

void ConstEditBtnListener::disposing( const lang::EventObject& oEvt )
	throw ( RuntimeException )
{
}
	
void ConstEditBtnListener::actionPerformed( const awt::ActionEvent& oEvt )
	throw ( RuntimeException )
{
	BaseDialog* pDlg = getDialog();
	
	ConstButtonType eType = getButtonType();
	SolverDialog* pMainDlg = pDlg->getSolverImpl()->getMainDialog();
	switch( eType )
	{
		case CONST_ADD:
		case CONST_CHANGE:
			{
				ConstEditDialog* pCE = pMainDlg->getConstEditDialog();
				if ( pCE != NULL )
				{
					if ( eType == CONST_CHANGE )
					{
						// Set the selected constraint to the dialog.
						sal_uInt32 nSel = pMainDlg->getSelectedConstraintPos();
						
						rtl::OUString sLeft, sRight;
						Equality eEq;
						pMainDlg->getConstraint( nSel, sLeft, sRight, eEq );
						pCE->setLeftCellReference( sLeft );
						pCE->setRightCellReference( sRight );
						pCE->setEquality( eEq );
						
						pCE->setChangeMode( true );
						pCE->setConstraintId( nSel );
					}
					else
					{
						assert( eType == CONST_ADD );
						pCE->setChangeMode( false );
					}
					
					pCE->setVisible( true );
				}
				else
					assert( !"ConstEditDialog is NULL" );
			}
			break;
			
		case CONST_DELETE:
			{
				// Delete the selected constraint, and disable the "Change" and
				// "Delete" buttons.
			
				sal_Int16 nSel = pMainDlg->getSelectedConstraintPos();
				pMainDlg->removeConstraint( nSel );
				pMainDlg->enableWidget( ascii( "btnConstChange" ), false );
				pMainDlg->enableWidget( ascii( "btnConstDelete" ), false );
			}
			break;
	
		default:
			assert( !"Wrong button type!" );
	}
}

//---------------------------------------------------------------------------
// Class ConstListBoxListener

ConstListBoxListener::ConstListBoxListener( SolverDialog* pDlg ) : ItemListener( pDlg )
{
}

void ConstListBoxListener::itemStateChanged( const awt::ItemEvent& oEvt )
	throw ( RuntimeException )
{
	Debug( "ConstListBoxListener::itemStateChanged" );
	
	getDialog()->enableWidget( ascii( "btnConstChange" ) );
	getDialog()->enableWidget( ascii( "btnConstDelete" ) );
}


//---------------------------------------------------------------------------
// Class MaxRadioBtnListener

MaxRadioBtnListener::MaxRadioBtnListener( SolverDialog* pDlg ) : ItemListener( pDlg )
{
}

MaxRadioBtnListener::~MaxRadioBtnListener() throw()
{
}

void MaxRadioBtnListener::disposing( const lang::EventObject& oEvt ) 
		throw ( RuntimeException )
{
}

void MaxRadioBtnListener::itemStateChanged( const awt::ItemEvent& oEvt )
	throw ( RuntimeException )
{
	Debug( "MaxRadioBtnListener::itemStateChanged" );
}



WindowFocusListener::WindowFocusListener( BaseDialog* pDlg ) : FocusListener( pDlg )
{
}

void WindowFocusListener::focusGained( const awt::FocusEvent& oEvt ) throw( RuntimeException )
{
	Debug( "focusGained" );
}

void WindowFocusListener::focusLost( const awt::FocusEvent& oEvt ) throw( RuntimeException )
{
	Debug( "focusLost" );
}


WindowMouseListener::WindowMouseListener( BaseDialog* pDlg ) : MouseListener( pDlg )
{
}

WindowMouseListener::~WindowMouseListener() throw()
{
}

void WindowMouseListener::mousePressed( const awt::MouseEvent& oEvt ) 
	throw( RuntimeException )
{
	Debug( "mousePressed" );
}


OKCancelBtnListener::OKCancelBtnListener( BaseDialog* pDlg, const rtl::OUString& sMode ) : 
		ActionListener( pDlg )
{
	m_sMode = sMode;
}

OKCancelBtnListener::~OKCancelBtnListener() throw()
{
}

void OKCancelBtnListener::disposing( const lang::EventObject& oEvt )
	throw ( RuntimeException )
{
	Debug( "CancelBtnListener::disposing" );
}

void OKCancelBtnListener::actionPerformed( const awt::ActionEvent& oEvt )
	throw ( RuntimeException )
{	
	ConstEditDialog* pDlg = getDialog()->getSolverImpl()->getMainDialog()->getConstEditDialog();
	
	if ( m_sMode.equals( ascii ( "OK" ) ) )
	{
		Debug( "OK pressed" );
		
		rtl::OUString  sLeft = pDlg->getLeftCellReference();
		rtl::OUString sRight = pDlg->getRightCellReference();
		Equality         eEq = pDlg->getEquality();
		
		SolverImpl* p = getDialog()->getSolverImpl();
		SolverDialog* pMainDlg = p->getMainDialog();
		if ( pDlg->isChangeMode() )
			pMainDlg->editConstraint( pDlg->getConstraintId(), sLeft, sRight, eEq );
		else
			pMainDlg->setConstraint( sLeft, sRight, eEq );
	}
	
	if ( m_sMode.equals( ascii( "Cancel" ) ) )
		Debug( "Cancel pressed" );

	pDlg->setVisible( false );
	pDlg->reset();
}


}
