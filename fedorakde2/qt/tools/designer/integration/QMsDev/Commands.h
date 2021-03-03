// Commands.h : Header-Datei
//

#if !defined(AFX_COMMANDS_H__BCA8D1B1_12D4_456D_B34A_73C08840E419__INCLUDED_)
#define AFX_COMMANDS_H__BCA8D1B1_12D4_456D_B34A_73C08840E419__INCLUDED_

#include "QMsDevTypes.h"

class CCommands : 
	public CComDualImpl<ICommands, &IID_ICommands, &LIBID_QMsDev>, 
	public CComObjectRoot,
	public CComCoClass<CCommands, &CLSID_Commands>
{
protected:
	IApplication* m_pApplication;
	
	CString getActiveFileName();
	int getActiveProject(CComQIPtr<IBuildProject, &IID_IBuildProject>&);
	int getConfigurations(CComQIPtr<IBuildProject, &IID_IBuildProject>, CComQIPtr<IConfigurations, &IID_IConfigurations>&);
	void addSharedSettings( CComPtr<IConfiguration> );
	void addStaticSettings( CComPtr<IConfiguration> );
	bool getGlobalSettings( CString &qtLibName );
	void addMOC( CComQIPtr<IBuildProject, &IID_IBuildProject> pProject, CString file );
	void addUIC( CComQIPtr<IBuildProject, &IID_IBuildProject> pProject, CString file );
	CString replaceTemplateStrings( const CString& t, const CString& classheader, 
					const CString& classname, const CString& instance, 
					const CString& instcall, const CString& projekt, 
					const CString& runapp = "return app.exec();" );

public:
	CCommands();
	~CCommands();

	void splitFileName( CString &file, CString &filepath, CString &filetitle, CString& fileext );
	void runDesigner( const CString& file );

	void SetApplicationObject(IApplication* m_pApplication);
	IApplication* GetApplicationObject() { return m_pApplication; }
	void UnadviseFromEvents();

	BEGIN_COM_MAP(CCommands)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ICommands)
	END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CCommands)

protected:
	// Diese Klassenvorlage wird als Basisklasse für die
	//  Ereignis-Handler-Objekte "Application" und "Debugger" verwendet,
	//  die weiter unten deklariert werden.
	template <class IEvents, const IID* piidEvents, const GUID* plibid,
		class XEvents, const CLSID* pClsidEvents>
	class XEventHandler :
		public CComDualImpl<IEvents, piidEvents, plibid>,
		public CComObjectRoot,
		public CComCoClass<XEvents, pClsidEvents>
	{
	public:
		BEGIN_COM_MAP(XEvents)
			COM_INTERFACE_ENTRY(IDispatch)
			COM_INTERFACE_ENTRY_IID(*piidEvents, IEvents)
		END_COM_MAP()
		DECLARE_NOT_AGGREGATABLE(XEvents)
		void Connect(IUnknown* pUnk)
		{ VERIFY(SUCCEEDED(AtlAdvise(pUnk, this, *piidEvents, &m_dwAdvise))); }
		void Disconnect(IUnknown* pUnk)
		{ AtlUnadvise(pUnk, *piidEvents, m_dwAdvise); }

		CCommands* m_pCommands;

	protected:
		DWORD m_dwAdvise;
	};

	// Dieses Objekt behandelt vom Objekt Application ausgelöste Ereignisse
	class XApplicationEvents : public XEventHandler<IApplicationEvents, 
		&IID_IApplicationEvents, &LIBID_QMsDev, 
		XApplicationEvents, &CLSID_ApplicationEvents>
	{
	public:
		// IApplicationEvents-Methoden
		STDMETHOD(BeforeBuildStart)(THIS);
		STDMETHOD(BuildFinish)(THIS_ long nNumErrors, long nNumWarnings);
		STDMETHOD(BeforeApplicationShutDown)(THIS);
		STDMETHOD(DocumentOpen)(THIS_ IDispatch * theDocument);
		STDMETHOD(BeforeDocumentClose)(THIS_ IDispatch * theDocument);
		STDMETHOD(DocumentSave)(THIS_ IDispatch * theDocument);
		STDMETHOD(NewDocument)(THIS_ IDispatch * theDocument);
		STDMETHOD(WindowActivate)(THIS_ IDispatch * theWindow);
		STDMETHOD(WindowDeactivate)(THIS_ IDispatch * theWindow);
		STDMETHOD(WorkspaceOpen)(THIS);
		STDMETHOD(WorkspaceClose)(THIS);
		STDMETHOD(NewWorkspace)(THIS);

	};
	typedef CComObject<XApplicationEvents> XApplicationEventsObj;
	XApplicationEventsObj* m_pApplicationEventsObj;

	// Dieses Objekt behandelt vom Objekt Application ausgelöste Ereignisse
	class XDebuggerEvents : public XEventHandler<IDebuggerEvents, 
		&IID_IDebuggerEvents, &LIBID_QMsDev, 
		XDebuggerEvents, &CLSID_DebuggerEvents>
	{
	public:
		// IDebuggerEvents-Methode
	    STDMETHOD(BreakpointHit)(THIS_ IDispatch * pBreakpoint);
	};
	typedef CComObject<XDebuggerEvents> XDebuggerEventsObj;
	XDebuggerEventsObj* m_pDebuggerEventsObj;

public:
// ICommands methods
	STDMETHOD(QMsDevNewQtProject)();
	STDMETHOD(QMsDevGenerateQtProject)();
	STDMETHOD(QMsDevAddUICStep)();
	STDMETHOD(QMsDevAddMOCStep)();
	STDMETHOD(QMsDevUseQt)();
	STDMETHOD(QMsDevStartDesigner)(THIS);
	STDMETHOD(QMsDevNewQtDialog)();
};

typedef CComObject<CCommands> CCommandsObj;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_COMMANDS_H__BCA8D1B1_12D4_456D_B34A_73C08840E419__INCLUDED)
