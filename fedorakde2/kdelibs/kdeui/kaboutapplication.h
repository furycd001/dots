/*
 * This file is part of the KDE Libraries
 * Copyright (C) 2000 Waldo Bastian (bastian@kde.org) and
 * Espen Sand (espen@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#ifndef _KABOUT_APPLICATION_H_
#define _KABOUT_APPLICATION_H_

#include <kaboutdata.h>
#include <kaboutdialog.h>

/**
 * This class provides the standard "About Application" dialog box that
 * is used by @ref KHelpMenu. It uses the information of the global
 * @ref KAboutData that is specified at the start of you program in
 * main(). Normally you should not use this class directly but
 * rather the @ref KHelpMenu class or even better just subclass your
 * toplevel window from @ref KMainWindow. If you do the latter, the help
 * menu and thereby this dialog box is available through the
 * @ref KMainWindow::helpMenu() function.
 *
 * @short Standard "About Application" dialog box.
 * @author Waldo Bastian (bastian@kde.org) and Espen Sand (espen@kde.org)
 * @version $Id
 */

class KAboutApplication : public KAboutDialog
{
  public:
    /**
     * Constructor. Creates a fully featured "About Application" dialog box.
     * Note that this dialog is made modeless in the @ref KHelpMenu class so
     * the users may expect a modeless dialog.
     *
     * @param parent The parent of the dialog box. You should use the
     *        toplevel window so that the dialog becomes centered.
     * @param name Internal name of the widget. This name is not used in the
     *        caption.
     * @param modal If false, this widget will be modeless and must be
     *        made visible using @ref QWidget::show(). Otherwise it will be
     *        modal and must be made visible using @ref QWidget::exec().
     */
    KAboutApplication( QWidget *parent=0, const char *name=0, bool modal=true );

    /**
     * Constructor. Mostly does the same stuff as the above constructor, except
     * that it can take a custom KAboutData object instead of the one specified
     * in your main() function. This is especially useful for applications
     * which are implemented as (dynamically loaded) libraries, e.g. panel
     * applets.
     *
     * @param aboutData A pointer to a @ref KAboutData object which data
     *        will be used for filling the dialog.
     * @param parent The parent of the dialog box. You should use the
     *        toplevel window so that the dialog becomes centered.
     * @param name Internal name of the widget. This name is not used in the
     *        caption.
     * @param modal If false, this widget will be modeless and must be
     *        made visible using @ref QWidget::show(). Otherwise it will be
     *        modal and must be made visible using @ref QWidget::exec().
     */
    KAboutApplication( const KAboutData *aboutData, QWidget *parent=0, const char *name=0, bool modal=true );

/*
 FIXME: The two constructors should be replaced with the following  after the lib freeze:

    KAboutApplication( const KAboutData *aboutData=0, QWidget *parent=0, const char *name=0, bool modal=true );

 This will make buildDialog() obsolete as well (Frerich).
*/
  protected:
    void buildDialog( const KAboutData *aboutData );
};


#endif

