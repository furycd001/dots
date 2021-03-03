#ifndef __khc_view_h__
#define __khc_view_h__

#include <khtml_part.h>

#include "khc_navigator.h"

class KHCView : public KHTMLPart
{
    Q_OBJECT
public:
    KHCView( QWidget *parentWidget, const char *widgetName,
             QObject *parent, const char *name, KHTMLPart::GUIProfile prof );

    virtual bool openURL( const KURL &url );

    virtual void saveState( QDataStream &stream );
    virtual void restoreState( QDataStream &stream );

    QString title() const { return m_title; }

public slots:
    void showGlossaryEntry( const khcNavigatorWidget::GlossaryEntry &entry );

private slots:
    void setTitle( const QString &title );

private:
    void showAboutPage();
 
    QString langLookup( const QString &fname );

    enum State { Docu, About, GlossEntry };

    khcNavigatorWidget::GlossaryEntry m_glossEntry;
    int m_state;
    QString m_title;
};

#endif
/**
 * vim:et
 */
