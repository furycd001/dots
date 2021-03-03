// stdafx.h : Include-Datei f�r Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die h�ufig benutzt, aber
//      in unregelm��igen Abst�nden ge�ndert werden.
//

#if !defined(AFX_STDAFX_H__687335F2_9F10_486C_AA5B_E2255E3E0A12__INCLUDED_)
#define AFX_STDAFX_H__687335F2_9F10_486C_AA5B_E2255E3E0A12__INCLUDED_

#define VC_EXTRALEAN		// Selten benutzte Teile der Windows-Header nicht einbinden

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxdisp.h>

#include <atlbase.h>
//Sie k�nnen eine Klasse von CComModule ableiten und diese benutzen, um etwas zu �berlagern,
//Sie sollten jedoch den Namen von _Module nicht �ndern
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
// Testlauf-Unterst�tzung

// Verwenden Sie VERIFY_OK bei allen Aufrufen an Developer Studio-Objekte, 
//  die den Wert S_OK zur�ckgeben sollen.
// In DEBUG-Builds Ihres Add-Ins zeigt VERIFY_OK ein ASSERT-Dialogfeld an,
//  wenn der Ausdruck einen anderen Wert als S_OK f�r HRESULT zur�ckgibt. Gibt HRESULT
//  einen "Erfolgscode" zur�ck, zeigt das Feld ASSERT diesen Wert f�r HRESULT an. Gibt HRESULT 
//  einen "Fehlercode" zur�ck, zeigt das Feld ASSERT diesen Wert f�r HRESULT an sowie die 
//  Zeichenfolge mit der Fehlerbeschreibung, bereitgestellt von dem Objekt, das den Fehler ausgel�st hat.
// In RETAIL-Builds Ihres Add-Ins wertet VERIFY_OK lediglich den Ausdruck aus
//  und ignoriert den zur�ckgegebenen Wert f�r HRESULT.

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
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_STDAFX_H__687335F2_9F10_486C_AA5B_E2255E3E0A12__INCLUDED)
