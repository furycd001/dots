// DSAddIn.h : Header-Datei
//

#if !defined(AFX_DSADDIN_H__A7A23BC1_13F2_488C_B5F5_57B23FAA215B__INCLUDED_)
#define AFX_DSADDIN_H__A7A23BC1_13F2_488C_B5F5_57B23FAA215B__INCLUDED_

#include "commands.h"

// {6CED40CF-FF72-4369-8AA4-6A917FEF55DD}
DEFINE_GUID(CLSID_DSAddIn,
0x6ced40cf, 0xff72, 0x4369, 0x8a, 0xa4, 0x6a, 0x91, 0x7f, 0xef, 0x55, 0xdd);

/////////////////////////////////////////////////////////////////////////////
// CDSAddIn

class CDSAddIn : 
	public IDSAddIn,
	public CComObjectRoot,
	public CComCoClass<CDSAddIn, &CLSID_DSAddIn>
{
public:
	DECLARE_REGISTRY(CDSAddIn, "QMsDev.DSAddIn.1",
		"QMSDEV Developer Studio Add-in", IDS_QMSDEV_LONGNAME,
		THREADFLAGS_BOTH)

	CDSAddIn() {}
	BEGIN_COM_MAP(CDSAddIn)
		COM_INTERFACE_ENTRY(IDSAddIn)
	END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CDSAddIn)

// IDSAddIns
public:
	STDMETHOD(OnConnection)(THIS_ IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection);
	STDMETHOD(OnDisconnection)(THIS_ VARIANT_BOOL bLastTime);

protected:
	CCommandsObj* m_pCommands;
	DWORD m_dwCookie;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_DSADDIN_H__A7A23BC1_13F2_488C_B5F5_57B23FAA215B__INCLUDED)
