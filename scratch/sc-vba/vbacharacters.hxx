#ifndef SC_VBA_CHARACTERS_HXX
#define SC_VBA_CHARACTERS_HXX

#include <cppuhelper/implbase1.hxx>

#include <org/openoffice/vba/XCharacters.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/text/XSimpleText.hpp>

#include "vbahelper.hxx"
#include "vbapalette.hxx"
typedef ::cppu::WeakImplHelper1< oo::vba::XCharacters > ScVbaCharacters_BASE;

class ScVbaCharacters : public ScVbaCharacters_BASE
{
private:
	css::uno::Reference< css::text::XTextRange > m_xTextRange;
	css::uno::Reference< css::text::XSimpleText > m_xSimpleText;
	css::uno::Reference< css::uno::XComponentContext > m_xContext;
	ScVbaPalette m_aPalette;
	sal_Int16 nLength;
	sal_Int16 nStart;
public:
	ScVbaCharacters( const css::uno::Reference< css::uno::XComponentContext >& xContext,  const ScVbaPalette& dPalette, const css::uno::Reference< css::text::XSimpleText >& xRange, const css::uno::Any& Start, const css::uno::Any& Length  ) throw ( css::lang::IllegalArgumentException );

	virtual ~ScVbaCharacters() {}
	// Attributes
	virtual ::rtl::OUString SAL_CALL getCaption() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setCaption( const ::rtl::OUString& _caption ) throw (css::uno::RuntimeException);
	virtual ::sal_Int32 SAL_CALL getCount() throw (css::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getText() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setText( const ::rtl::OUString& _text ) throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XFont > SAL_CALL getFont() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setFont( const css::uno::Reference< oo::vba::XFont >& _font ) throw (css::uno::RuntimeException);

	// Methods
	virtual void SAL_CALL Insert( const ::rtl::OUString& String ) throw (css::uno::RuntimeException);
	virtual void SAL_CALL Delete(  ) throw (css::uno::RuntimeException);



};

#endif /* SC_VBA_CHARACTER_HXX */

