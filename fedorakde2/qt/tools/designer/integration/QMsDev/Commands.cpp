// Commands.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include <afxdlgs.h>
#include "QMsDev.h"
#include "Commands.h"
#include "newqtprojectdialog.h"
#include "qmsdevtemplates.h"
#include <direct.h>
#include <process.h>
#include <windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static bool dontOpen = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CCommands

CCommands::CCommands()
{
    m_pApplication = NULL;
    m_pApplicationEventsObj = NULL;
    m_pDebuggerEventsObj = NULL;
}

CCommands::~CCommands()
{
    ASSERT (m_pApplication != NULL);
    m_pApplication->Release();
}

void CCommands::SetApplicationObject(IApplication* pApplication)
{
    // Diese Funktion geht davon aus, dass AddRef bereits auf pApplication angewendet wurde,
    //  was CDSAddIn durch den Aufruf von QueryInterface direkt vor dem Aufruf dieser
    //  Funktion bereits erledigt hat.
    m_pApplication = pApplication;

    // Ereignis-Handler für Anwendung erzeugen
    XApplicationEventsObj::CreateInstance(&m_pApplicationEventsObj);
    m_pApplicationEventsObj->AddRef();
    m_pApplicationEventsObj->Connect(m_pApplication);
    m_pApplicationEventsObj->m_pCommands = this;

    // Ereignis-Handler für Debugger erzeugen
    CComPtr<IDispatch> pDebugger;
    if (SUCCEEDED(m_pApplication->get_Debugger(&pDebugger)) 
	    && pDebugger != NULL)
    {
	XDebuggerEventsObj::CreateInstance(&m_pDebuggerEventsObj);
	m_pDebuggerEventsObj->AddRef();
	m_pDebuggerEventsObj->Connect(pDebugger);
	m_pDebuggerEventsObj->m_pCommands = this;
    }
}

void CCommands::UnadviseFromEvents()
{
    ASSERT (m_pApplicationEventsObj != NULL);
    m_pApplicationEventsObj->Disconnect(m_pApplication);
    m_pApplicationEventsObj->Release();
    m_pApplicationEventsObj = NULL;

    if (m_pDebuggerEventsObj != NULL)
    {
	// Da wir die Verbindung zu den Debugger-Ereignissen herstellen konnten, 
	//  sollte es auch möglich sein, erneut auf das Debugger-Objekt zuzugreifen, 
	//  um die Verbindung zu seinen Ereignissen zu trennen (daher das VERIFY_OK weiter unten -- siehe stdafx.h).
	CComPtr<IDispatch> pDebugger;
	VERIFY_OK(m_pApplication->get_Debugger(&pDebugger));
	ASSERT (pDebugger != NULL);
	m_pDebuggerEventsObj->Disconnect(pDebugger);
	m_pDebuggerEventsObj->Release();
	m_pDebuggerEventsObj = NULL;
    }
}


/////////////////////////////////////////////////////////////////////////////
// Ereignis-Handler

// ZU ERLEDIGEN: Füllen Sie die Implementierung für die Ereignisse aus, die Sie behandeln wollen
//  Verwenden Sie m_pCommands->GetApplicationObject(), um auf das Objekt
//  "Developer Studio Application" zuzugreifen

//Application-Ereignisse

HRESULT CCommands::XApplicationEvents::BeforeBuildStart()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return S_OK;
}

HRESULT CCommands::XApplicationEvents::BuildFinish(long nNumErrors, long nNumWarnings)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeApplicationShutDown()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentOpen(IDispatch* theDocument)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    dontOpen = TRUE;
    if ( !theDocument )
	return S_OK;

    CComQIPtr<ITextDocument, &IID_ITextDocument> pText(theDocument);
    if ( pText ) {
	CString file;
	CString filepath;
	CString filename;
	CString fileext;
	CComBSTR bszStr;
	pText->get_FullName(&bszStr);
	file = bszStr;
    	m_pCommands->splitFileName( file, filepath, filename, fileext );
	if ( fileext == "ui" ) {
	    DsSaveStatus saved;
	    pText->Close( CComVariant(dsSaveChangesNo), &saved );
	    m_pCommands->runDesigner( filepath + file );
	}
    }

    return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeDocumentClose(IDispatch* theDocument)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    dontOpen = TRUE;
    return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentSave(IDispatch* theDocument)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewDocument(IDispatch* theDocument)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowActivate(IDispatch* theWindow)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // only run designer if it was intended
    if ( dontOpen ) {
	dontOpen = FALSE;
	return S_OK;
    }
    
    CComQIPtr<IGenericWindow, &IID_IGenericWindow> pGenericWindow(theWindow);
    if ( !pGenericWindow )
	return S_OK;

    BSTR type;
    pGenericWindow->get_Type( &type );
    if ( CString(type) != "Text" )
	return S_OK;

    CComPtr<IDispatch> pDocument;
    pGenericWindow->get_Parent(&pDocument);
    CComQIPtr<ITextDocument, &IID_ITextDocument> pTextDocument(pDocument);
    if ( pTextDocument ) {
	CString file;
	CString filepath;
	CString filename;
	CString fileext;
	CComBSTR bszStr;
	pTextDocument->get_FullName(&bszStr);
	file = bszStr;

	m_pCommands->splitFileName( file, filepath, filename, fileext );
	if ( fileext == "ui" )
	    m_pCommands->runDesigner( filepath + file );
    }

    return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowDeactivate(IDispatch* theWindow)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceOpen()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceClose()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewWorkspace()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return S_OK;
}

// Debugger-Ereignis

HRESULT CCommands::XDebuggerEvents::BreakpointHit(IDispatch* pBreakpoint)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Little helpers
CString CCommands::getActiveFileName()
{
    CString file;
    CComPtr<IDispatch> pActiveDocument;
    m_pApplication->get_ActiveDocument(&pActiveDocument);
    if (pActiveDocument) {
	CComQIPtr<ITextDocument, &IID_ITextDocument> pText(pActiveDocument);
	if ( pText ) {
	    CComBSTR bszStr;
	    pText->get_FullName(&bszStr);
	    file = bszStr;
	}
    }
    return file;
}

void CCommands::splitFileName( CString &file, CString &filepath, CString &filetitle, CString &fileext )
{
    // cut file into filepath and file
    int pathpos = file.ReverseFind( '\\' );
    if ( pathpos != -1 ) {
	filepath = file.Left( pathpos +1 );
	file = file.Mid(pathpos + 1);
    }
    // cut file into filetitle and fileext (without dot)
    int extpos = file.ReverseFind( '.' );
    if ( extpos != -1 ) {
	filetitle = file.Left( extpos );
	fileext = file.Mid( extpos + 1 );	
    }
}

int CCommands::getActiveProject(CComQIPtr<IBuildProject, &IID_IBuildProject>& project )
{
    CComPtr<IDispatch> pDispProject;
    m_pApplication->get_ActiveProject(&pDispProject);
    project = CComQIPtr<IBuildProject, &IID_IBuildProject>(pDispProject);
    if ( !project ) {
	m_pApplication->PrintToOutputWindow( CComBSTR("NO ACTIVE PROJECT FOUND") );
	return S_FALSE;
    }

    return S_OK;
}

int CCommands::getConfigurations(CComQIPtr<IBuildProject, &IID_IBuildProject> project, CComQIPtr<IConfigurations, &IID_IConfigurations>& configs )
{
    project->get_Configurations( &configs );
    if ( !configs ) {
	m_pApplication->PrintToOutputWindow( CComBSTR("NO CONFIGURATIONS IN THIS PROJECT") );
	return S_FALSE;
    }
    return S_OK;
}

bool CCommands::getGlobalSettings( CString &libname )
{
    CFileFind qtLib;
    CString qtFile = "";
    CString qtThread = "";
    CString filever = "";
    CString threadver = "";
    CString libver;

    BOOL bWorking = qtLib.FindFile( _T(getenv("QTDIR") + CString("\\lib\\qt*.lib" ) ) );
    bool useThreads;

    while ( bWorking ) {
	bWorking = qtLib.FindNextFile();
	if ( !bWorking ) 
	    break;
	if ( (LPCTSTR)qtLib.GetFileName().Left(5).Compare( "qt-mt" )==0 ) {
	    qtThread = (LPCTSTR)qtLib.GetFileName();
	    threadver = qtThread.Mid( 5, 3 );
	} else if ( !(qtLib.GetFileName().Compare( "qtmain.lib" ) == 0) ) {
	    qtFile = (LPCTSTR)qtLib.GetFileName();
	    filever = qtFile.Mid( 2, 3 );
	}
	
	if ( threadver.Compare( filever ) > 0 && threadver.Compare( libver ) > 0 ) {
	    libname = qtThread;
	    libver = threadver;
	    useThreads = TRUE;
	} else if ( threadver.Compare( filever ) < 0 && filever.Compare( libver ) > 0 ) {
	    libname = qtFile;
	    libver = filever;
	    useThreads = FALSE;
	} else if ( threadver.Compare( filever ) == 0 && !(qtThread.Right(6).Compare("nc.lib")==0) && threadver.Compare( libver ) > 0 ) {
	    libname = qtThread;
	    libver = threadver;
	    useThreads = TRUE;
	} else if ( filever.Compare( libver ) > 0 ) {
	    libname = qtFile;
	    libver = filever;
	    useThreads = FALSE;
	} else if ( filever.Compare( libver ) == 0 && libname.Right(6).Compare("nc.lib")==0 ) {
	    libname = qtFile;
	    libver = filever;
	    useThreads = FALSE;
	}
    }
    return useThreads;
}
void CCommands::addSharedSettings( CComPtr<IConfiguration> pConfig )
{
    CString libname;
    bool useThreads = getGlobalSettings( libname );    
    
    const CComBSTR compiler("cl.exe");
    const CComBSTR linker("link.exe");
    LPCTSTR dllDefs;
    if ( useThreads )
	dllDefs = "/D QT_DLL /D QT_THREAD_SUPPORT";
    else
	dllDefs = "/D QT_DLL";

    const CComBSTR dllDefine( dllDefs );	
    const CComBSTR incPath(" /I$(QTDIR)\\include");
    const CComBSTR staticLib("$(QTDIR)\\lib\\qt.lib");
    CString sharedLibText = CString("$(QTDIR)\\lib\\") + libname;
    const CComBSTR sharedLib(sharedLibText + CString(" $(QTDIR)\\lib\\qtmain.lib") );
    const CComBSTR defLibs( "kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib" );
    const CComBSTR sysLibs( "kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib" );
    const CComBSTR threadLibD("/MLd");
    const CComBSTR threadLibR("/ML");
    const CComBSTR correctLibD("/MDd");
    const CComBSTR correctLibR("/MD");

    VERIFY_OK(pConfig->AddToolSettings( compiler, dllDefine, CComVariant(VARIANT_FALSE) ));
    VERIFY_OK(pConfig->AddToolSettings( compiler, incPath, CComVariant(VARIANT_FALSE) ));
    VERIFY_OK(pConfig->RemoveToolSettings( linker, defLibs, CComVariant(VARIANT_FALSE) ));
    VERIFY_OK(pConfig->AddToolSettings( linker, sysLibs, CComVariant(VARIANT_FALSE) ));    
    VERIFY_OK(pConfig->RemoveToolSettings( linker, staticLib, CComVariant(VARIANT_FALSE) ));
    VERIFY_OK(pConfig->AddToolSettings( linker, sharedLib, CComVariant(VARIANT_FALSE) ));
    VERIFY_OK(pConfig->RemoveToolSettings( compiler, threadLibD, CComVariant(VARIANT_FALSE) ));
    VERIFY_OK(pConfig->AddToolSettings( compiler, correctLibD, CComVariant(VARIANT_FALSE) ));
    VERIFY_OK(pConfig->RemoveToolSettings( compiler, threadLibR, CComVariant(VARIANT_FALSE) ));
    VERIFY_OK(pConfig->AddToolSettings( compiler, correctLibR, CComVariant(VARIANT_FALSE) ));
    m_pApplication->PrintToOutputWindow( CComBSTR("\t\tadded Qt shared library") );
}

void CCommands::addStaticSettings( CComPtr<IConfiguration> pConfig )
{
    CString libname;
    bool useThreads = getGlobalSettings( libname );    

    const CComBSTR compiler("cl.exe");
    const CComBSTR linker("link.exe");
    LPCTSTR dllDefs;
    if ( useThreads )
	dllDefs = "/D QT_DLL /D QT_THREAD_SUPPORT";
    else
	dllDefs = "/D QT_DLL";

    const CComBSTR dllDefine( dllDefs );	
    const CComBSTR incPath(" /I$(QTDIR)\\include");
    const CComBSTR staticLib("$(QTDIR)\\lib\\qt.lib");
    CString sharedLibText = CString("$(QTDIR)\\lib\\") + libname;
    const CComBSTR sharedLib(sharedLibText + CString(" $(QTDIR)\\lib\\qtmain.lib") );
    const CComBSTR defLibs( "kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib" );
    const CComBSTR sysLibs( "kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib" );

    VERIFY_OK(pConfig->RemoveToolSettings( compiler, dllDefine, CComVariant(VARIANT_FALSE) ));
    VERIFY_OK(pConfig->AddToolSettings( compiler, incPath, CComVariant(VARIANT_FALSE) ));
    VERIFY_OK(pConfig->RemoveToolSettings( linker, defLibs, CComVariant(VARIANT_FALSE) ));
    VERIFY_OK(pConfig->AddToolSettings( linker, sysLibs, CComVariant(VARIANT_FALSE) ));    
    VERIFY_OK(pConfig->RemoveToolSettings( linker, sharedLib, CComVariant(VARIANT_FALSE) ));
    VERIFY_OK(pConfig->AddToolSettings( linker, staticLib, CComVariant(VARIANT_FALSE) ));
    m_pApplication->PrintToOutputWindow( CComBSTR("\t\tadded Qt static library") );
}

void CCommands::addMOC( CComQIPtr<IBuildProject, &IID_IBuildProject> pProject, CString file )
{
    CString fileext;
    CString filename;
    CString filepath;
    CString mocfile;
    CString fileToMoc;
    const CString moccommand = "%qtdir%\\bin\\moc.exe ";

    splitFileName( file, filepath, filename, fileext );

    bool specialFile = FALSE;
    if ( !fileext.IsEmpty() && fileext[0] == 'c' ) {
	fileToMoc = filepath + filename + ".moc";
	mocfile = filepath + filename + ".moc";
	specialFile = TRUE;
    } else {
	fileToMoc = filepath + file;
	mocfile = filepath + "moc_"+ filename + ".cpp";
    }

    // Add the files to the project
    if ( pProject->AddFile( CComBSTR(fileToMoc), CComVariant(VARIANT_TRUE) ) == S_OK )
        m_pApplication->PrintToOutputWindow( CComBSTR("\t\tadded "+file) );
    if ( pProject->AddFile( CComBSTR(mocfile), CComVariant(VARIANT_TRUE) ) == S_OK )
	m_pApplication->PrintToOutputWindow( CComBSTR("\t\tadded "+mocfile) );

    // Get the list of configurations in the active project
    CComQIPtr<IConfigurations, &IID_IConfigurations> pConfigs;
    if ( getConfigurations( pProject, pConfigs ) != S_OK ) {
	m_pApplication->PrintToOutputWindow( CComBSTR("FAILED TO ADD MOC!") );
	return;
    }

    if ( !fileext.IsEmpty() && fileext[0] == 'c' ) {
	fileToMoc = filepath + filename + ".moc";
	mocfile = "$(InputDir)\\$(InputName).moc";
	specialFile = TRUE;
    } else {
	fileToMoc = filepath + file;
	mocfile = "$(InputDir)\\moc_$(InputName).cpp";
    }

    // Add the moc step to the file
    long cCount;
    VERIFY_OK( pConfigs->get_Count(&cCount));
    for (long c = 0; c < cCount; c++ ) {
	CComVariant Varc = c+1;
	CComPtr<IConfiguration> pConfig;
	VERIFY_OK(pConfigs->Item(Varc, &pConfig));
	VERIFY_OK(pConfig->AddCustomBuildStepToFile(CComBSTR(fileToMoc), CComBSTR(moccommand+"$(InputDir)\\$(InputName)."+fileext+" -o "+mocfile), 
					  CComBSTR(mocfile), CComBSTR("Moc'ing $(InputName)."+fileext+" ..."), 
					  CComVariant(VARIANT_FALSE)));
	m_pApplication->PrintToOutputWindow( CComBSTR("\t\tadded MOC preprocessor") );
    }
    // CANTDO: add dependency to .moc-file
    if ( specialFile ) {
//	    VERIFY_OK(pConfig->AddFileDependency( CComBSTR(mocfile), CComBSTR(filepath+file)));
    }
}

void CCommands::addUIC( CComQIPtr<IBuildProject, &IID_IBuildProject> pProject, CString file )
{
    CString fileext;
    CString filename;
    CString filepath;

    splitFileName( file, filepath, filename, fileext );

    const CString uiFile(filepath + file);
    const CString impFile(filepath + filename + ".cpp");
    const CString decFile(filepath + filename + ".h");
    const CString incFile( filename+".h" );
    const CString mocFile(filepath + "moc_" + filename + ".cpp");
    const CString uiccommand("%qtdir%\\bin\\uic.exe ");
    const CString moccommand("%qtdir%\\bin\\moc.exe ");

    // Add the file and the all output to the project
    if ( pProject->AddFile( CComBSTR(uiFile), CComVariant(VARIANT_TRUE) ) == S_OK )
	m_pApplication->PrintToOutputWindow( CComBSTR("\t\tadded "+uiFile) );
    if ( pProject->AddFile( CComBSTR(impFile), CComVariant(VARIANT_TRUE) ) == S_OK )
	m_pApplication->PrintToOutputWindow( CComBSTR("\t\tadded "+impFile) );
    if (pProject->AddFile( CComBSTR(decFile), CComVariant(VARIANT_TRUE) ) == S_OK )
	m_pApplication->PrintToOutputWindow( CComBSTR("\t\tadded "+decFile) );
    if (pProject->AddFile( CComBSTR(mocFile), CComVariant(VARIANT_TRUE) ) == S_OK )
	m_pApplication->PrintToOutputWindow( CComBSTR("\t\tadded "+mocFile) );

    // Get the list of configurations in the active project
    CComQIPtr<IConfigurations, &IID_IConfigurations> pConfigs;
    if ( getConfigurations( pProject, pConfigs ) != S_OK ) {
	m_pApplication->PrintToOutputWindow( CComBSTR("FAILED TO ADD UIC!") );
    }
    // Add the uic step to the file
    long cCount;
    VERIFY_OK( pConfigs->get_Count(&cCount));
    for (long c = 0; c < cCount; c++ ) {
	CComVariant Varc = c+1;
	CComPtr<IConfiguration> pConfig;
	VERIFY_OK(pConfigs->Item(Varc, &pConfig));
	CComBSTR command = uiccommand+"$(InputPath) -o $(InputDir)\\$(InputName).h\n" +
			   uiccommand+"$(InputPath) -i $(InputName).h -o $(InputDir)\\$(InputName).cpp\n" + 
			   moccommand+"$(InputDir)\\$(InputName).h -o $(InputDir)\\moc_$(InputName).cpp";
	CComBSTR output = "$(InputDir)\\$(InputName).h\n$(InputDir)\\$(InputName).cpp\n$(InputDir)\\moc_$(InputName).cpp";
	VERIFY_OK(pConfig->AddCustomBuildStepToFile(CComBSTR(uiFile), command, output, CComBSTR("Uic'ing $(InputName).ui ..."), 
	    CComVariant(VARIANT_FALSE)));
	m_pApplication->PrintToOutputWindow( CComBSTR("\t\tadded UIC preprocessor step") );
    }
}

CString CCommands::replaceTemplateStrings( const CString& t, const CString& classheader, 
					   const CString& classname, const CString& instance, 
					   const CString& instcall, const CString& projekt,
					   const CString& runapp)
{
    CString r = t;
    r.Replace( "$QMSDEVCLASSHEADER", classheader );
    r.Replace( "$QMSDEVCLASSNAME", classname );
    r.Replace( "$QMSDEVINSTANCE", instance );
    r.Replace( "$QMSDEVINSTCALL", instcall );
    r.Replace( "$QMSDEVPROJECTNAME", projekt );
    r.Replace( "$QMSDEVRUNAPP", runapp );
    return r;
}

void CCommands::runDesigner( const CString &file )
{
    CString path;
    CString command;

    path = getenv("QTDIR");
    if ( path.IsEmpty() ) {
	// Get the designer location from the registry
	CRegKey key;
	char* value = new char[256];
	unsigned long length;
	if (key.Open(HKEY_CURRENT_USER, "Software\\Trolltech\\Qt\\Qt GUI Designer") == ERROR_SUCCESS) {
	    length = 255;
	    key.QueryValue( value, "PathToExe", &length );
	    path = value;
	    length = 255;
	    key.QueryValue( value, "NameOfExe", &length );
	    command = value;
	    key.Close();
	} else {
	    ::MessageBox(NULL, "Can't locate Qt GUI Designer", 
			       "Error", MB_OK | MB_ICONINFORMATION);
	    return;
	}
	delete[] value;
    } else {
	command = "designer.exe";
	path+="\\bin";
    }

    // Run the designer with options -client and "file"

    if ( spawnl(_P_NOWAIT, path+"\\"+command, command, "-client", file, 0 ) == -1 ) {
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));
	::MessageBox(NULL, "Failed to run Qt GUI Designer: "+command, 
			   "Start Designer", MB_OK | MB_ICONINFORMATION);
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
    } else {
	m_pApplication->PrintToOutputWindow( CComBSTR("Qt Designer started...") );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Ccommands-Methoden

STDMETHODIMP CCommands::QMsDevStartDesigner() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString file;
    CString filepath;
    CString filename;
    CString fileext;

    // Get the active file
    file = getActiveFileName();
    splitFileName( file, filepath, filename, fileext );

    // Check if we can use the file
    if ( file.IsEmpty() || fileext != "ui" )
	file = "NewDialog.ui";

    runDesigner( filepath + file );

    return S_OK;
}

STDMETHODIMP CCommands::QMsDevUseQt()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    m_pApplication->PrintToOutputWindow( CComBSTR("Adding Qt support to project") );
    // Check for active Project
    CComQIPtr<IBuildProject, &IID_IBuildProject> pProject;
    if ( getActiveProject( pProject ) != S_OK )
	return S_FALSE;

    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));
    int result = ::MessageBox(NULL, "Do you want to use Qt in a shared library?", "Question", MB_YESNOCANCEL | MB_ICONQUESTION );
    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

    if ( result == IDCANCEL )
	return S_OK;

    // TODO:Get the highest qt library version in $(QTDIR)\lib

    // Get the list of configurations in the active project
    CComQIPtr<IConfigurations, &IID_IConfigurations> pConfigs;
    if ( getConfigurations( pProject, pConfigs ) != S_OK )
	return S_FALSE;

    // Add the specific settings to compiler and linker
    
    long cCount;
    VERIFY_OK( pConfigs->get_Count(&cCount));
    for (long c = 0; c < cCount; c++ ) {
	CComVariant Varc = c+1;
	CComPtr<IConfiguration> pConfig;
	VERIFY_OK(pConfigs->Item(Varc, &pConfig));
	if ( result == IDYES )
	    addSharedSettings( pConfig );
	else
	    addStaticSettings( pConfig );
    }

    return S_OK;
    m_pApplication->PrintToOutputWindow( CComBSTR("Finished!\n") );
}

STDMETHODIMP CCommands::QMsDevAddMOCStep()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

    // Check for active Project
    CComQIPtr<IBuildProject, &IID_IBuildProject> pProject;
    if ( getActiveProject( pProject ) != S_OK ) {
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));
	::MessageBox(NULL, "Can't find an active project!", "QMsDev", MB_OK | MB_ICONINFORMATION );
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_FALSE;
    }

    CString file;
    CString fileext;
    CString filename;
    CString filepath;

    file = getActiveFileName();

    if ( file.IsEmpty() ) {
	CFileDialog fd( TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
	    "Header files (*.h; *.hxx; *.hpp)|*.h; *.hxx; *.hpp|"
	    "Implementation files (*.c; *.cpp; *.cxx)|*.c; *.cpp; *.cxx|"
	    "C++ files (*.h; *.hxx; *.hpp; *.c; *.cpp; *.cxx)|*.h; *.hxx; *.hpp; *.c; *.cpp; *.cxx|"
	    "All Files (*.*)|*.*||", NULL);
	int result = fd.DoModal();
        
	if ( result == IDCANCEL ) {
	    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	    return S_OK;
	}
	file = fd.GetPathName();
    }

    splitFileName( file, filepath, filename, fileext );

    m_pApplication->PrintToOutputWindow( CComBSTR("Add MOC buildstep for "+file+"...") );
    addMOC( pProject, filepath+file );
    m_pApplication->PrintToOutputWindow( CComBSTR("Finished!\n") );
    
    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

    return S_OK;
}

STDMETHODIMP CCommands::QMsDevAddUICStep()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

    CString file;
    CString fileext;
    CString filename;
    CString filepath;

    // Check for active Project
    CComQIPtr<IBuildProject, &IID_IBuildProject> pProject;
    if ( getActiveProject( pProject ) != S_OK ) {
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));
	::MessageBox(NULL, "Can't find an active project!", "QMsDev", MB_OK | MB_ICONINFORMATION );
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_FALSE;
    }

    file = getActiveFileName();
    splitFileName( file, filepath, filename, fileext );

    if ( file.IsEmpty() || fileext != "ui" ) {
	CFileDialog fd( TRUE, NULL, NULL, OFN_HIDEREADONLY, 
	    "User Interface File (*.ui)|*.ui|"
	    "All Files (*.*)|*.*||", NULL);
	int result = fd.DoModal();
	if ( result == IDCANCEL ) {
	    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	    return S_OK;
	}

	file = fd.GetPathName();
	splitFileName( file, filepath, filename, fileext );
    }

    m_pApplication->PrintToOutputWindow( CComBSTR("Add UIC buildstep for "+file+"...") );
    addUIC( pProject, filepath + file );
    m_pApplication->PrintToOutputWindow( CComBSTR("Finished!\n") );
    
    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
    return S_OK;
}

STDMETHODIMP CCommands::QMsDevGenerateQtProject()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

    CString file;
    CString filepath;
    CString filename;
    CString fileext;

    CFileDialog fd( TRUE, NULL, NULL, OFN_HIDEREADONLY, 
	"Qt Project (*.pro)|*.pro|"
	"All Files (*.*)|*.*||", NULL);
    int result = fd.DoModal();
    if ( result == IDCANCEL ) {
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_OK;
    }

    file = fd.GetPathName();
    splitFileName( file, filepath, filename, fileext );

    chdir( filepath );
    CString contents;
    CString tFile = "vcapp.t";
    try {
	CStdioFile file( filepath + file, CFile::modeRead );
	CString line;
	BOOL eof;
	do {
	    eof = !file.ReadString( line );
	    if ( eof )
		break;
	    if ( line.Find( "TEMPLATE" ) != -1 ) {
		if ( ( line.Find( "lib" ) != -1 ) || 
		    ( line.Find( "vclib" ) != -1 ) )
		    tFile = "vclib.t";
		break;
	    }
	} while ( !eof );
    } 
    catch ( CFileException* e ) {
	char err[256];
	e->GetErrorMessage( (char*)&err, 255, NULL );
	::MessageBox( NULL, err, "Error", MB_OK );
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_FALSE;
    }

    m_pApplication->PrintToOutputWindow( CComBSTR("Running tmake...") );
    if ( system( "tmake "+file+" -o "+filename+".dsp"+" -t "+tFile + " CONFIG+=thread") )
	m_pApplication->PrintToOutputWindow( CComBSTR("FAILED TO RUN TMAKE!") );
    else
	::MessageBox(NULL, "Created Developer Studio Project for Qt Project "+file+"\n"
			   "Add the new project file to your current workspace,\n"
			   "or open it in an empty workspace.", 
			   "Open Qt Project", MB_OK | MB_ICONINFORMATION );

    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
    return S_OK;
}

STDMETHODIMP CCommands::QMsDevNewQtProject()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

    NewQtProjectDialog dialog;
    if ( dialog.DoModal() == IDCANCEL ) {
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_FALSE;
    }

    if ( mkdir( dialog.m_location ) < 0 ) {
	::MessageBox(NULL, "Couldn't create Directory\n"+dialog.m_location, "New Qt Project", MB_OK | MB_ICONINFORMATION );
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_FALSE;
    }

    m_pApplication->PrintToOutputWindow( CComBSTR("Creating Qt Project \"" + dialog.m_name + "\"...") );

    CString filename = dialog.m_location;

    if ( filename[ filename.GetLength() -1 ] != '\\' )
	filename+="\\";
    filename += dialog.m_name + ".dsp";
    if ( m_pApplication->AddProject( CComBSTR(filename), CComBSTR(dialog.m_location), CComBSTR("Application"), VARIANT_TRUE ) != S_OK ) {
	CComBSTR err;
	GetLastErrorDescription( err );
	::MessageBox(NULL, (LPCTSTR)err.m_str, "New Qt Project", MB_OK | MB_ICONINFORMATION );
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_FALSE;
    }

    // Get project object we just created
    CComQIPtr<IBuildProject, &IID_IBuildProject> pProject;
    if ( getActiveProject( pProject ) != S_OK ) {
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));
	::MessageBox(NULL, "Project creation failed!", "QMsDev", MB_OK | MB_ICONINFORMATION );
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_FALSE;
    }
    // Get the list of configurations
    CComQIPtr<IConfigurations, &IID_IConfigurations> pConfigs;
    if ( getConfigurations( pProject, pConfigs ) != S_OK ) {
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_FALSE;
    }

    long cCount;
    VERIFY_OK( pConfigs->get_Count(&cCount));

    for (long c = 0; c < cCount; c++ ) {
	CComVariant Varc = c+1;
	CComPtr<IConfiguration> pConfig;
	VERIFY_OK(pConfigs->Item(Varc, &pConfig));
	BSTR bstr;
	pConfig->get_Name(&bstr);
#ifndef QT_NON_COMMERCIAL
	if ( dialog.m_shared ) {
#endif
	    m_pApplication->PrintToOutputWindow( CComBSTR("\tadding Qt shared library to "+ CString(bstr) + "...") );
	    addSharedSettings( pConfig );
#ifndef QT_NON_COMMERCIAL
	} else {
	    m_pApplication->PrintToOutputWindow( CComBSTR("\tadding Qt static library to "+ CString(bstr) + "...") );
	    addStaticSettings( pConfig );
	}
#endif      
   }

    CString projectName = dialog.m_name;
    dialog.m_name.MakeLower();
    CString classheader;
    CString classname;
    CString instancename;
    CString instancecall;
    CString runapp;
    if ( dialog.m_dialog ) {
	classheader = dialog.m_name + "dialog";
	classname = projectName + "Dialog";
	instancename = "dialog";
	instancecall = "( 0, 0, TRUE )";
	runapp = "dialog.exec();\n\n\treturn 0;";
    } else {
	classheader = dialog.m_name + "window";
	classname = projectName + "Window";
	instancename = "window";
	instancecall = "";
	runapp = "window.show();\n\n\treturn app.exec();";
    }

    CString baseDir = dialog.m_location + "\\";
    m_pApplication->PrintToOutputWindow( CComBSTR("\tcreating basic files...") );
    
    // Write files and replace $MSDEV... strings with project-specific stuff
    try {
	CStdioFile mainFile( baseDir + "main.cpp", CFile::modeCreate | CFile::modeWrite );

	mainFile.WriteString( replaceTemplateStrings(main_cpp, classheader, 
						     classname, instancename, instancecall, projectName, runapp) );
	mainFile.Close();

	CStdioFile infoFile( baseDir + "readme.txt", CFile::modeCreate | CFile::modeWrite );
	CString readme( readme_txt );

	if ( dialog.m_dialog ) {
	    readme.Replace( "$QMSDEVUITYPE", "Dialog interface" );
	    readme.Replace( "$QMSDEVFILELIST", classheader+"base.ui\n\tA dialog with basic buttons.\n"
					       "\tUse the Qt GUI Designer change the layout.\n" );
	    readme.Replace( "$QMSDEVQTFILELIST", classheader+"base.h\n"+classheader+"base.cpp\nmoc_"+
						 classheader+"base.cpp\nmoc_"+classheader+".cpp\n" );
	} else if ( dialog.m_mdi ) {
	    readme.Replace( "$QMSDEVUITYPE", "Multi Document interface (MDI)" );
	    readme.Replace( "$QMSDEVFILELIST", "" );
	    readme.Replace( "$QMSDEVQTFILELIST", "moc_"+classheader+".cpp\n" );
	} else {
	    readme.Replace( "$QMSDEVUITYPE", "Basic interface" );
	    readme.Replace( "$QMSDEVFILELIST", "" );
	    readme.Replace( "$QMSDEVQTFILELIST", "moc_"+classheader+".cpp\n" );
	}

	infoFile.WriteString( replaceTemplateStrings(readme, classheader, 
						     classname, instancename, instancecall, projectName) );
	infoFile.Close();

	if ( dialog.m_dialog ) {
	    CString uiFileName(dialog.m_name+"dialogbase.ui");
	    m_pApplication->PrintToOutputWindow( CComBSTR("\tcreating basic dialog UI...") );
	    
	    CStdioFile uiFile( baseDir + uiFileName, CFile::modeCreate | CFile::modeWrite );
	    uiFile.WriteString( replaceTemplateStrings(dialogbase_ui, classheader, 
						       classname, instancename, instancecall, projectName) );
	    uiFile.Close();

	    CString hFileName(classheader+".h");
	    CString iFileName(classheader+".cpp");
	    m_pApplication->PrintToOutputWindow( CComBSTR("\tcreating basic dialog implementation...") );
	    CStdioFile hFile( baseDir + hFileName, CFile::modeCreate | CFile::modeWrite );
	    hFile.WriteString( replaceTemplateStrings(dialog_h, classheader, 
						      classname, instancename, instancecall, projectName) );
	    hFile.Close();

	    CStdioFile iFile( baseDir + iFileName, CFile::modeCreate | CFile::modeWrite );
	    iFile.WriteString( replaceTemplateStrings(dialog_cpp, classheader, 
						      classname, instancename, instancecall, projectName) );
	    iFile.Close();

	    m_pApplication->PrintToOutputWindow( CComBSTR("\tadding files...") );
	    pProject->AddFile( CComBSTR(uiFileName), CComVariant(VARIANT_TRUE) );
	    pProject->AddFile( CComBSTR(hFileName), CComVariant(VARIANT_TRUE) );
	    pProject->AddFile( CComBSTR(iFileName), CComVariant(VARIANT_TRUE) );
	    m_pApplication->PrintToOutputWindow( CComBSTR("\tadding build steps...") );
	    addUIC( pProject, uiFileName );
	    addMOC( pProject, hFileName );
	    runDesigner( baseDir + uiFileName );
	} else {
	    CString hFileName;
	    CString iFileName;
	    hFileName = classheader+".h";
	    iFileName = classheader+".cpp";

	    CStdioFile hFile( baseDir + hFileName, CFile::modeCreate | CFile::modeWrite );
	    CStdioFile iFile( baseDir + iFileName, CFile::modeCreate | CFile::modeWrite );
	    if ( dialog.m_mdi ) {
		m_pApplication->PrintToOutputWindow( CComBSTR("\tcreating MDI interface...") );
		hFile.WriteString( replaceTemplateStrings(mdi_h, classheader, 
							  classname, instancename, instancecall, projectName) );
		iFile.WriteString( replaceTemplateStrings(mdi_cpp, classheader, 
							  classname, instancename, instancecall, projectName) );
	    } else {
		m_pApplication->PrintToOutputWindow( CComBSTR("\tcreating basic main window...") );
		hFile.WriteString( replaceTemplateStrings(window_h, classheader, 
							  classname, instancename, instancecall, projectName) );
		iFile.WriteString( replaceTemplateStrings(window_cpp, classheader, 
							  classname, instancename, instancecall, projectName) );
	    }
	    hFile.Close();
	    iFile.Close();

	    m_pApplication->PrintToOutputWindow( CComBSTR("\tadding files...") );
	    pProject->AddFile( CComBSTR(hFileName), CComVariant(VARIANT_TRUE) );
	    pProject->AddFile( CComBSTR(iFileName), CComVariant(VARIANT_TRUE) );
	    m_pApplication->PrintToOutputWindow( CComBSTR("\tadding build steps...") );
	    addMOC( pProject, hFileName);
	}
    }
    catch ( CFileException* e )
    {
	char err[256];
	e->GetErrorMessage( (char*)&err, 255, NULL );
	::MessageBox( NULL, err, "Error", MB_OK );
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_FALSE;
    }
    
    pProject->AddFile( CComBSTR("main.cpp"), CComVariant(VARIANT_TRUE) );
    pProject->AddFile( CComBSTR("readme.txt"), CComVariant(VARIANT_TRUE) );
    
    m_pApplication->PrintToOutputWindow( CComBSTR(dialog.m_name+" finished!\n") );

    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
    return S_OK;
}

STDMETHODIMP CCommands::QMsDevNewQtDialog()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    
    CComQIPtr<IBuildProject, &IID_IBuildProject> pProject;
    if ( getActiveProject( pProject ) != S_OK ) {
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));
	if ( ::MessageBox( NULL, "There is no active project.\nDo you want to create a new Qt Project?", 
	    "New Dialog", MB_YESNOCANCEL | MB_ICONQUESTION ) == IDYES )
	    QMsDevNewQtProject();

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_FALSE;
    }

    CComBSTR fp;
    pProject->get_FullName( &fp );
    CString file(fp);
    CString filepath;
    CString filename;
    CString fileext;
    CString classname;
    
    splitFileName( file, filepath, filename, fileext );

    // TODO: ask for classname
    CFileDialog fd( FALSE, "ui", "NewDialog.ui", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, 
	"User Interface File (*.ui)|*.ui|"
	"All Files (*.*)|*.*||", NULL);
    fd.m_ofn.lpstrInitialDir = filepath;
    int result = fd.DoModal();
    if ( result == IDCANCEL ) {
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_OK;
    }    

    file = fd.GetPathName();

    splitFileName( file, filepath, filename, fileext );

    classname = filename;
    filename.MakeLower();

    m_pApplication->PrintToOutputWindow( CComBSTR("Adding new dialog \"" + classname + "\"...") );
    m_pApplication->PrintToOutputWindow( CComBSTR("\tcreating dialog...") );

    file = filename+".ui";
    file.MakeLower();
    CStdioFile uiFile( filepath + file , CFile::modeCreate | CFile::modeWrite );
    uiFile.WriteString( replaceTemplateStrings(dialogbase_ui, "", 
					       classname, "", "", classname) );
    uiFile.Close();

    m_pApplication->PrintToOutputWindow( CComBSTR("\tadding files...") );
    addUIC( pProject, filepath+file );

    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));
    if ( ::MessageBox( NULL, "Do you want me to add a basic implementation for your dialog?", 
	"Question", MB_YESNOCANCEL | MB_ICONQUESTION ) == IDYES ) {
	m_pApplication->PrintToOutputWindow( CComBSTR("\tcreating implementation...") );
	int error = system( "uic -subdecl "+classname+" "+filename+".h "+filepath+filename+".ui -o "+filepath+filename+"impl.h" );
	error +=    system( "uic -subimpl "+classname+" "+filename+"impl.h "+filepath+filename+".ui -o "+filepath+filename+"impl.cpp" );
	if ( error ) {
	    ::MessageBox( NULL, "Failed to create subclass implementation!", "Error", MB_OK );
	    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	    return S_FALSE;
	}
	m_pApplication->PrintToOutputWindow( CComBSTR("\tadding implementation files...") );
	pProject->AddFile( CComBSTR(filepath+filename+"impl.h"), CComVariant(VARIANT_TRUE));
	addMOC( pProject, filepath+filename+"impl.h" );
	pProject->AddFile( CComBSTR(filepath+filename+"impl.cpp"), CComVariant(VARIANT_TRUE));
    }

    m_pApplication->PrintToOutputWindow( CComBSTR("New Dialog \"" + classname + "\" finished!") );

    VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
    return S_OK;
}
