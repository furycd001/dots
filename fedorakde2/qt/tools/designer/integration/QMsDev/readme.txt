==============================================================================
  The QMsDev AddIn - Qt integration for the Microsoft(tm) Visual Studio(tm)
==============================================================================


1. Building the DLL
===================

Add the qmsdev.dsp file to any workspace in your Visual C++(tm) IDE. Make sure
the release configuration is selected and run Build (F7). This will create the
qmsdev.dll file in the release subdirectory.


2. Installation
===============

Move the qmsdev.dll file into the AddIns subdirectory of your Visual Studio(tm)
installation:

Example:
C:\Program Files\Microsoft Visual Studio\Common\MSDev98\AddIns

Open the "Customize"-Dialog in the Visual Studio(tm) ( Extras -> Customize ) 
and open the "Add-Ins and Macrofiles"-Tab. Check the list item "QMsDev AddIn" 
and close the dialog. This should load the AddIn and add a new toolbar.
Use the "Customize"-Dialog to change the layout of the toolbar. The plugin
will add these commands to your Visual Studio(tm):

- New Qt Project       - A small application wizard
- Generate Qt Project  - Runs the TMAKE tool with a .pro-file
- New Qt Dialog        - Add an empty Qt Dialog to the active project
- Qt GUI Designer      - Run the Qt Designer
- Use Qt               - Add the Qt libraries to the active project
- Add MOC              - Add the MOC precompiler to the active file
- Add UIC              - Add the UIC precompiler to the active file

Moreover, the plugin will start the Qt Designer each time you open a .ui file
by doubleclicking on the file in workspace overview.


3. De-Installation
==================

Delete the qmsdev.dll file from the addins subdirectory.


4. Known problems
=================

- Adding MOC step to a cpp-file. This will add a .moc file to your project
and add the necessary command to the custom build steps. As the Visual Studio
COM interface does not provide a function to add a dependency for this file,
you will have to add this manually in order to have the moc-file generated
each time you change the cpp-file. Open the settings dialog for the moc-file, 
and add the dependency to the cpp-file manually in the "Custom Build-Step"-Tab.
