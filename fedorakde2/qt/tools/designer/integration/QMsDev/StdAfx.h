// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#if !defined(AFX_STDAFX_H__687335F2_9F10_486C_AA5B_E2255E3E0A12__INCLUDED_)
#define AFX_STDAFX_H__687335F2_9F10_486C_AA5B_E2255E3E0A12__INCLUDED_

#define VC_EXTRALEAN		// Selten benutzte Teile der Windows-Header nicht einbinden

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxdisp.h>

#include <atlbase.h>
//Sie können eine Klasse von CComModule ableiten und diese benutzen, um etwas zu überlagern,
//Sie sollten jedoch den Namen von _Module nicht ändern
extern CComModule _Module;
#include <atlcom.h>

// Developer Studio Objektmodell
#include <ObjModel\addauto.h>
#include <ObjModel\appdefs.h>
#include <ObjModel\appauto.h>
#include <ObjModel\blddefs.h>
#include <ObjModel\bldauto.h>
#include <ObjModel\textdefs.h>
#include <ObjModel\textauto.h>
#include <ObjModel\dbgdefs.h>
#include <ObjModel\dbgauto.h>

/////////////////////////////////////////////////////////////////////////////
// Testlauf-Unterstützung

// Verwenden Sie VERIFY_OK bei allen Aufrufen an Developer Studio-Objekte, 
//  die den Wert S_OK zurückgeben sollen.
// In DEBUG-Builds Ihres Add-Ins zeigt VERIFY_OK ein ASSERT-Dialogfeld an,
//  wenn der Ausdruck einen anderen Wert als S_OK für HRESULT zurückgibt. Gibt HRESULT
//  einen "Erfolgscode" zurück, zeigt das Feld ASSERT diesen Wert für HRESULT an. Gibt HRESULT 
//  einen "Fehlercode" zurück, zeigt das Feld ASSERT diesen Wert für HRESULT an sowie die 
//  Zeichenfolge mit der Fehlerbeschreibung, bereitgestellt von dem Objekt, das den Fehler ausgelöst hat.
// In RETAIL-Builds Ihres Add-Ins wertet VERIFY_OK lediglich den Ausdruck aus
//  und ignoriert den zurückgegebenen Wert für HRESULT.

void GetLastErrorDescription(CComBSTR& bstr);		// In QMsDev.cpp definiert

#ifdef _DEBUG
#define VERIFY_OK(f) \
	{ \
		HRESULT hr = (f); \
		if (hr != S_OK) \
		{ \
			if (FAILED(hr)) \
			{ \
				CComBSTR bstr; \
				GetLastErrorDescription(bstr); \
				_RPTF2(_CRT_ASSERT, "Object call returned %lx\n\n%S", hr, (BSTR) bstr); \
			} \
			else \
				_RPTF1(_CRT_ASSERT, "Object call returned %lx", hr); \
		} \
	}

#else //_DEBUG

#define VERIFY_OK(f) (f);

#endif //_DEBUG

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_STDAFX_H__687335F2_9F10_486C_AA5B_E2255E3E0A12__INCLUDED)
