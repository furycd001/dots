// QMsDev.cpp : Legt die Initialisierungsroutinen für die DLL fest.
//

#include "stdafx.h"
#include <initguid.h>
#include "QMsDev.h"
#include "DSAddIn.h"
#include "Commands.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_DSAddIn, CDSAddIn)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQMsDevApp

class CQMsDevApp : public CWinApp
{
public:
	CQMsDevApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CQMsDevApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CQMsDevApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassenassistenten eingefügt und entfernt..
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CQMsDevApp

BEGIN_MESSAGE_MAP(CQMsDevApp, CWinApp)
	//{{AFX_MSG_MAP(CQMsDevApp)
		// HINWEIS - Hier werden Mapping-Makros vom Klassenassistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Das einzige CQMsDevApp-Objekt

CQMsDevApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CQMsDevApp Konstruktion

CQMsDevApp::CQMsDevApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
}

/////////////////////////////////////////////////////////////////////////////
// CQMsDevApp Initialisierung

BOOL CQMsDevApp::InitInstance()
{
	_Module.Init(ObjectMap, m_hInstance);

	return CWinApp::InitInstance();
}

int CQMsDevApp::ExitInstance()
{
	_Module.Term();

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Spezielle, für Inproc-Server benötigte Einsprungpunkte

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _Module.GetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

// Sie können regsvr32.exe verwenden, indem Sie DllRegisterServer exportieren
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT hRes = S_OK;
	
	// Registriert Objekt, Typelib und alle Schnittstellen in Typelib
	hRes = _Module.RegisterServer(TRUE);
	if (FAILED(hRes))
		return hRes;

	// Beschreibung dieses AddIn-Objekts in seinem eigenen
	//  /Description Teilschlüssel registrieren.
	// ZU ERLEDIGEN:  Wenn Sie diesem Modul weitere AddIns hinzufügen, müssen Sie
	//  zur Registrierung all Ihrer Beschreibungen jede Beschreibung
	//  im CLSID-Eintrag der Registrierung jedes Add-In-Objekts registrieren:
	// HKEY_CLASSES_ROOT\Clsid\{add-in  HKEY_CLASSES_ROOT\Clsid\{add-in CLSID}\Description="Add-In-Beschreibung"
	_ATL_OBJMAP_ENTRY* pEntry = _Module.m_pObjMap;
	CRegKey key;
	LONG lRes = key.Open(HKEY_CLASSES_ROOT, _T("CLSID"));
	if (lRes == ERROR_SUCCESS)
	{
		USES_CONVERSION;
		LPOLESTR lpOleStr;
		StringFromCLSID(*pEntry->pclsid, &lpOleStr);
		LPTSTR lpsz = OLE2T(lpOleStr);

		lRes = key.Open(key, lpsz);
		if (lRes == ERROR_SUCCESS)
		{
			CString strDescription;
			strDescription.LoadString(IDS_QMSDEV_DESCRIPTION);
			key.SetKeyValue(_T("Description"), strDescription);
		}
		CoTaskMemFree(lpOleStr);
	}
	if (lRes != ERROR_SUCCESS)
		hRes = HRESULT_FROM_WIN32(lRes);

	return hRes;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Entfernt Einträge aus der Systemregistrierung

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hRes = S_OK;
	_Module.UnregisterServer();
	return hRes;
}


/////////////////////////////////////////////////////////////////////////////
// Testlauf-Unterstützung

// GetLastErrorDescription wird in der Implementierung des Makros VERIFY_OK
//  verwendet, das in stdafx.h definiert ist.

void GetLastErrorDescription(CComBSTR& bstr)
{
	CComPtr<IErrorInfo> pErrorInfo;
	if (GetErrorInfo(0, &pErrorInfo) == S_OK)
		pErrorInfo->GetDescription(&bstr);
}
