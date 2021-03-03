#if !defined(AFX_NEWQTPROJECTDIALOG_H__68FB6822_A864_4F5B_B2F9_3B4E84D3E1E1__INCLUDED_)
#define AFX_NEWQTPROJECTDIALOG_H__68FB6822_A864_4F5B_B2F9_3B4E84D3E1E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewQtProjectDialog.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld NewQtProjectDialog 

class NewQtProjectDialog : public CDialog
{
// Konstruktion
public:
	BOOL m_dialog;
	BOOL m_shared;
	NewQtProjectDialog(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(NewQtProjectDialog)
	enum { IDD = IDD_NEWQTPROJECT_DIALOG };
	CEdit	c_location;
	CEdit	c_name;
	CButton	c_mdi;
	BOOL	m_mdi;
	CString	m_location;
	CString	m_name;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(NewQtProjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	BOOL OnInitDialog();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(NewQtProjectDialog)
	afx_msg void OnQtShared();
	afx_msg void OnQtStatic();
	afx_msg void OnProjectLookup();
	afx_msg void OnProjectNameChange();
	afx_msg void OnAppDialog();
	afx_msg void OnAppMain();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_NEWQTPROJECTDIALOG_H__68FB6822_A864_4F5B_B2F9_3B4E84D3E1E1__INCLUDED_

