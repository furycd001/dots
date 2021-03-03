; CLW-Datei enthält Informationen für den MFC-Klassen-Assistenten

[General Info]
Version=1
ODLFile=QMsDev.odl
ClassCount=3
Class1=CQMsDevApp
LastClass=GetStringDialog
NewFileInclude2=#include "QMsDev.h"
ResourceCount=4
NewFileInclude1=#include "stdafx.h"
LastTemplate=CDialog
Resource1=IDR_TOOLBAR_MEDIUM
Resource2=IDD_NEWQTPROJECT_DIALOG
Class2=NewQtProjectDialog
Resource3=IDR_TOOLBAR_LARGE
Class3=GetStringDialog
Resource4=IDD_GETSTRING_DIALOG

[CLS:CQMsDevApp]
Type=0
HeaderFile=QMsDev.h
ImplementationFile=QMsDev.cpp
Filter=N
LastObject=CQMsDevApp

[TB:IDR_TOOLBAR_MEDIUM]
Type=1
Class=?
Command1=ID_BUTTON32786
Command2=ID_BUTTON32785
Command3=ID_BUTTON32791
Command4=ID_BUTTON32777
Command5=ID_BUTTON32778
Command6=ID_BUTTON32779
Command7=ID_BUTTON32780
CommandCount=7

[TB:IDR_TOOLBAR_LARGE]
Type=1
Class=?
Command1=ID_BUTTON32789
Command2=ID_BUTTON32788
Command3=ID_BUTTON32792
Command4=ID_BUTTON32781
Command5=ID_BUTTON32782
Command6=ID_BUTTON32783
Command7=ID_BUTTON32784
CommandCount=7

[DLG:IDD_NEWQTPROJECT_DIALOG]
Type=1
Class=NewQtProjectDialog
ControlCount=16
Control1=IDC_PROJECTNAME,edit,1350631552
Control2=IDC_PROJECTLOCATION,edit,1350631552
Control3=IDC_PROJECTLOOKUP,button,1342242816
Control4=IDC_QTSHARED,button,1342308361
Control5=IDC_QTSTATIC,button,1342177289
Control6=IDC_APPDIALOG,button,1342308361
Control7=IDC_APPMAIN,button,1342177289
Control8=IDC_APPMDI,button,1342242819
Control9=IDCANCEL,button,1342242816
Control10=IDOK,button,1342242817
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,button,1342177287
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,button,1342177287
Control16=IDC_STATIC,button,1342177287

[CLS:NewQtProjectDialog]
Type=0
HeaderFile=NewQtProjectDialog.h
ImplementationFile=NewQtProjectDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_BUTTON32777
VirtualFilter=dWC

[DLG:IDD_GETSTRING_DIALOG]
Type=1
Class=GetStringDialog
ControlCount=4
Control1=IDC_NAME,edit,1350631552
Control2=IDCANCEL,button,1342242816
Control3=IDOK,button,1342242817
Control4=IDC_STATIC,button,1342177287

[CLS:GetStringDialog]
Type=0
HeaderFile=GetStringDialog.h
ImplementationFile=GetStringDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=GetStringDialog

