#ifndef SC_VBA_FONT_HXX
#define SC_VBA_FONT_HXX

#include <cppuhelper/implbase1.hxx>

#include <org/openoffice/vba/XFont.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include "vbahelper.hxx"
#include "vbapalette.hxx"

class ScTableSheetsObj;

typedef  ::cppu::WeakImplHelper1< oo::vba::XFont > ScVbaFont_BASE;

class ScVbaFont : public ScVbaFont_BASE 
{
    css::uno::Reference< css::beans::XPropertySet > mxFont;
    ScVbaPalette mPalette;
    SfxItemSet* mpDataSet;
public:
	ScVbaFont( const ScVbaPalette& dPalette, css::uno::Reference< css::beans::XPropertySet > xPropertySet, SfxItemSet* pDataSet = NULL ) throw ( css::uno::RuntimeException );
	virtual ~ScVbaFont() {}

	// Attributes
    virtual css::uno::Any SAL_CALL getSize() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setSize( const css::uno::Any& _size ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getStandardFontSize() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setStandardFontSize( const css::uno::Any& _standardfontsize ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getStandardFont() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setStandardFont( const css::uno::Any& _standardfont ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getFontStyle() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setFontStyle( const css::uno::Any& _fontstyle ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getColorIndex() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setColorIndex( const css::uno::Any& _colorindex ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getBold() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setBold( const css::uno::Any& _bold ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getUnderline() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setUnderline( const css::uno::Any& _underline ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getStrikethrough() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setStrikethrough( const css::uno::Any& _strikethrough ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getShadow() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setShadow( const css::uno::Any& _shadow ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getItalic() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setItalic( const css::uno::Any& _italic ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getSubscript() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setSubscript( const css::uno::Any& _subscript ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getSuperscript() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setSuperscript( const css::uno::Any& _superscript ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getName() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setName( const css::uno::Any& _name ) throw (css::uno::RuntimeException);
	
};



#endif /* SC_VBA_FONT_HXX */

