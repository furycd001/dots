/**
 * @libdoc Framework for KDE graphical components
 *
 * This library implements the framework for KDE parts, which are
 * elaborate widgets with a user-interface defined in terms of actions
 * (menu items, toolbar icons...). See @ref KParts::Part.
 *
 * The library also provides a framework for applications that want to
 * use parts. Such applications need to inherit their main window
 * from @ref KParts::MainWindow and provide a so-called shell GUI, 
 * which provides a basic skeleton GUI with part-independent functionality/actions.
 *
 * Some KParts applications won't be specific to a given part, but expect
 * to be able to embed, for instance, all types of viewers out there. For this
 * the basic functionality of any viewer has been implemented in @ref
 * KParts::ReadOnlyPart, which viewer-like parts should inherit from.
 * The same applies to @ref KParts::ReadWritePart, which is for editor-like parts.
 *
 * You can add actions to an existing KParts app from "outside", defining
 * the code for those actions in a shared library. This mechanism is
 * obviously called plugins, and implemented by @ref KParts::Plugin.
 */
