#include "notepad.h" // this plugin applies to a notepad part
#include <qmultilineedit.h>
#include "plugin_spellcheck.h"
#include <kaction.h>
#include <kinstance.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>

PluginSpellCheck::PluginSpellCheck( QObject* parent, const char* name )
    : Plugin( parent, name )
{
    (void) new KAction( "&Select current line (plugin)", 0, this, SLOT(slotSpellCheck()),
                        actionCollection(), "spellcheck" );
}

PluginSpellCheck::~PluginSpellCheck()
{
}

void PluginSpellCheck::slotSpellCheck()
{
    kdDebug() << "Plugin parent : " << parent()->name() << " (" << parent()->className() << ")" << endl;
    // The parent is assumed to be a NotepadPart
    if ( !parent()->inherits("NotepadPart") )
       KMessageBox::error(0L,"You just called the spell-check action on a wrong part (not NotepadPart)");
    else
    {
         NotepadPart * part = (NotepadPart *) parent();
         QMultiLineEdit * widget = (QMultiLineEdit *) part->widget();
         widget->selectAll(); //selects current line !
    }
}

KPluginFactory::KPluginFactory( QObject* parent, const char* name )
  : KLibFactory( parent, name )
{
  s_instance = new KInstance("KPluginFactory");
}

KPluginFactory::~KPluginFactory()
{
  delete s_instance;
}

QObject* KPluginFactory::createObject( QObject* parent, const char* name, const char*, const QStringList & )
{
    return new PluginSpellCheck( parent, name );
}

extern "C"
{
  void* init_libspellcheckplugin()
  {
    return new KPluginFactory;
  }

}

KInstance* KPluginFactory::s_instance = 0L;

#include <plugin_spellcheck.moc>
