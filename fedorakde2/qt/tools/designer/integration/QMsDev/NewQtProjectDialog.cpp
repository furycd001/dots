// NewQtProjectDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "QMsDev.h"
#include <afxdlgs.h>
#include <direct.h>
#include "NewQtProjectDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld NewQtProjectDialog 


NewQtProjectDialog::NewQtProjectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NewQtProjectDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(NewQtProjectDialog)
	m_mdi = FALSE;
	m_dialog = TRUE;
#if defined(QT_NON_COMMERCIAL)
	m_shared = TRUE;
#endif
	m_name = _T("NewProject");
	m_location = _T("");
	//}}AFX_DATA_INIT	
}

void NewQtProjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
#ifndef QT_NON_COMMERCIAL
	//{{AFX_DATA_MAP(NewQtProjectDialog)
	DDX_Control(pDX, IDC_PROJECTLOCATION, c_location);
	DDX_Control(pDX, IDC_PROJECTNAME, c_name);
	DDX_Control(pDX, IDC_APPMDI, c_mdi);
	DDX_Check(pDX, IDC_APPMDI, m_mdi);
	DDX_Text(pDX, IDC_PROJECTLOCATION, m_location);
	DDX_Text(pDX, IDC_PROJECTNAME, m_name);
	DDX_Check(pDX, IDC_APPDIALOG, m_dialog);
	DDX_Check(pDX, IDC_QTSHARED, m_shared);
	//}}AFX_DATA_MAP
#else
	//{{AFX_DATA_MAP(NewQtProjectDialog)
	DDX_Control(pDX, IDC_PROJECTLOCATION, c_location);
	DDX_Control(pDX, IDC_PROJECTNAME, c_name);
	DDX_Control(pDX, IDC_APPMDI, c_mdi);
	DDX_Check(pDX, IDC_APPMDI, m_mdi);
	DDX_Text(pDX, IDC_PROJECTLOCATION, m_location);
	DDX_Text(pDX, IDC_PROJECTNAME, m_name);
	DDX_Check(pDX, IDC_APPDIALOG, m_dialog);
	//}}AFX_DATA_MAP
#endif
}


BEGIN_MESSAGE_MAP(NewQtProjectDialog, CDialog)
#if defined(QT_NON_COMMERCIAL)
	//{{AFX_MSG_MAP(NewQtProjectDialog)
	ON_BN_CLICKED(IDC_QTSHARED, OnQtShared)
	ON_BN_CLICKED(IDC_QTSTATIC, OnQtStatic)
	ON_BN_CLICKED(IDC_PROJECTLOOKUP, OnProjectLookup)
	ON_BN_CLICKED(IDC_APPDIALOG, OnAppDialog)
	ON_BN_CLICKED(IDC_APPMAIN, OnAppMain)
	ON_EN_CHANGE(IDC_PROJECTNAME, OnProjectNameChange)
	//}}AFX_MSG_MAP
#else
	//{{AFX_MSG_MAP(NewQtProjectDialog)
	ON_BN_CLICKED(IDC_PROJECTLOOKUP, OnProjectLookup)
	ON_BN_CLICKED(IDC_APPDIALOG, OnAppDialog)
	ON_BN_CLICKED(IDC_APPMAIN, OnAppMain)
	ON_EN_CHANGE(IDC_PROJECTNAME, OnProjectNameChange)
	//}}AFX_MSG_MAP
#endif
END_MESSAGE_MAP()

BOOL NewQtProjectDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    c_mdi.EnableWindow( !m_dialog );
    char cwd[256];
    m_location = _T(getcwd((char*)&cwd, 256 ));
    if ( m_location.GetAt( m_location.GetLength()-1 ) != '\\' )
	c_location.SetWindowText( m_location+ "\\" + m_name );
    else 
	c_location.SetWindowText( m_location+m_name );

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten NewQtProjectDialog 

#if defined(QT_NON_COMMERCIAL)

void NewQtProjectDialog::OnQtShared()
{
    m_shared = TRUE;
}

void NewQtProjectDialog::OnQtStatic()
{
    m_shared = FALSE;
}
#endif

void NewQtProjectDialog::OnProjectLookup() 
{
    CFileDialog fd( TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL, NULL );
    if ( fd.DoModal() == IDOK ) {
	m_location = fd.GetPathName();
	int endpath = m_location.ReverseFind( '\\' );
	if ( endpath != -1 )
	    m_location = m_location.Left( endpath );

	if ( m_location.GetAt( m_location.GetLength()-1 ) != '\\' )
	    m_location+='\\';

	CString name;
	c_name.GetWindowText( name );

	c_location.SetWindowText( m_location + name );
    }
}

void NewQtProjectDialog::OnProjectNameChange()
{
    CString location, name;
    c_location.GetWindowText( location );
    int endpath = location.ReverseFind( '\\' );
    if ( endpath != -1 )
	location = location.Left( endpath );

    c_name.GetWindowText( name );

    if ( location.GetAt( location.GetLength()-1 ) != '\\' )
	location+='\\';

    c_location.SetWindowText( location + name );
}

void NewQtProjectDialog::OnAppDialog() 
{
    m_dialog = TRUE;
    c_mdi.EnableWindow( FALSE );
}

void NewQtProjectDialog::OnAppMain() 
{
    m_dialog = FALSE;
    c_mdi.EnableWindow( TRUE );
}


