// AddInMod.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "QMsDev.h"
#include "DSAddIn.h"
#include "Commands.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Dieser Code wird beim ersten Laden des Add-Ins und beim Starten der Anwendung aufgerufen
//  jeder nachfolgenden Developer Studio-Sitzung
STDMETHODIMP CDSAddIn::OnConnection(IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// An uns übergebene Info speichern
	IApplication* pApplication = NULL;
	if (FAILED(pApp->QueryInterface(IID_IApplication, (void**) &pApplication))
		|| pApplication == NULL)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}

	m_dwCookie = dwCookie;

	// Befehlsverteilung erzeugen, Rückmeldung an DevStudio
	CCommandsObj::CreateInstance(&m_pCommands);
	m_pCommands->AddRef();

	// Das obige QueryInterface hat AddRef auf das Objekt Application angewendet. Es
	//  wird im Destruktor von CCommand freigegeben.
	m_pCommands->SetApplicationObject(pApplication);

	// (siehe Definition von VERIFY_OK in stdafx.h)

	VERIFY_OK(pApplication->SetAddInInfo((long) AfxGetInstanceHandle(),
		(LPDISPATCH) m_pCommands, IDR_TOOLBAR_MEDIUM, IDR_TOOLBAR_LARGE, m_dwCookie));

	// DevStudio über die implementierten Befehle informieren

	VARIANT_BOOL bRet;
	CString strCmdString;

	LPCTSTR szNewQtProject = _T("New Qt Project");
	strCmdString.LoadString(IDS_NEWQTPROJECT_STRING);
	VERIFY_OK(pApplication->AddCommand(CComBSTR(szNewQtProject + strCmdString), CComBSTR(_T("QMsDevNewQtProject")), 0, m_dwCookie, &bRet));
#if 1
	LPCTSTR szGenerateQtProject = _T("Generate Qt Project");
	strCmdString.LoadString(IDS_GENERATEQTPROJECT_STRING);
	VERIFY_OK(pApplication->AddCommand(CComBSTR(szGenerateQtProject + strCmdString), CComBSTR(_T("QMsDevGenerateQtProject")), 1, m_dwCookie, &bRet));
#endif
	LPCTSTR szNewQtDialog = _T("New Qt Dialog");
	strCmdString.LoadString(IDS_NEWQTDIALOG_STRING);
	VERIFY_OK(pApplication->AddCommand(CComBSTR(szNewQtDialog + strCmdString), CComBSTR(_T("QMsDevNewQtDialog")), 2, m_dwCookie, &bRet));

	LPCTSTR szOpenDesigner = _T("Open Qt GUI Designer");
	strCmdString.LoadString(IDS_OPENDESIGNER_STRING);
	VERIFY_OK(pApplication->AddCommand(CComBSTR(szOpenDesigner + strCmdString), CComBSTR(_T("QMsDevStartDesigner")), 3, m_dwCookie, &bRet));

#ifndef QT_NON_COMMERCIAL
	LPCTSTR szUseQt = _T("Use Qt");
  	strCmdString.LoadString(IDS_USEQT_STRING);
  	VERIFY_OK(pApplication->AddCommand(CComBSTR(szUseQt + strCmdString), CComBSTR(_T("QMsDevUseQt")), 4, m_dwCookie, &bRet));
#endif

	LPCTSTR szAddMOCStep = _T("Add MOC step");
	strCmdString.LoadString(IDS_ADDMOCSTEP_STRING);
	VERIFY_OK(pApplication->AddCommand(CComBSTR(szAddMOCStep + strCmdString), CComBSTR(_T("QMsDevAddMOCStep")), 5 , m_dwCookie, &bRet));

	LPCTSTR szAddUICStep = _T("Add UIC step");
	strCmdString.LoadString(IDS_ADDUICSTEP_STRING);
	VERIFY_OK(pApplication->AddCommand(CComBSTR(szAddUICStep + strCmdString), CComBSTR(_T("QMsDevAddUICStep")), 6, m_dwCookie, &bRet));

	if (bRet == VARIANT_FALSE)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}

	if (bFirstTime == VARIANT_TRUE)
	{
		VERIFY_OK(pApplication->
			AddCommandBarButton(dsGlyph, CComBSTR(szNewQtProject), m_dwCookie));
#if 1
		VERIFY_OK(pApplication->
			AddCommandBarButton(dsGlyph, CComBSTR(szGenerateQtProject), m_dwCookie));
#endif
		VERIFY_OK(pApplication->
			AddCommandBarButton(dsGlyph, CComBSTR(szNewQtDialog), m_dwCookie));
		VERIFY_OK(pApplication->
			AddCommandBarButton(dsGlyph, CComBSTR(szOpenDesigner), m_dwCookie));
#ifndef QT_NON_COMMERCIAL
		VERIFY_OK(pApplication->
  			AddCommandBarButton(dsGlyph, CComBSTR(szUseQt), m_dwCookie));
#endif
		VERIFY_OK(pApplication->
			AddCommandBarButton(dsGlyph, CComBSTR(szAddMOCStep), m_dwCookie));
		VERIFY_OK(pApplication->
			AddCommandBarButton(dsGlyph, CComBSTR(szAddUICStep), m_dwCookie));
	}

	*OnConnection = VARIANT_TRUE;
	return S_OK;
}

// Dieser Code wird bei Shut-Down-Vorgängen und beim Entfernen des Add-Ins aus dem Speicher aufgerufen
STDMETHODIMP CDSAddIn::OnDisconnection(VARIANT_BOOL bLastTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pCommands->UnadviseFromEvents();
	m_pCommands->Release();
	m_pCommands = NULL;

	// ZU ERLEDIGEN: Hier alle anfallenden Bereinigungsarbeiten durchführen

	return S_OK;
}
