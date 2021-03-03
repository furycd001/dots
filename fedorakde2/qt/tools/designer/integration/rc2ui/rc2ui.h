#ifndef RC2UI_H
#define RC2UI_H

#include <qtextstream.h>
#include <qfile.h>
#include <qstringlist.h>

class RC2UI
{
public:
    RC2UI( QTextStream* input );
    ~RC2UI();

    bool parse();
    bool parse ( QStringList& get );

protected:
    enum WidgetType { IDWidget, IDPushButton, IDLabel, IDCheckBox, IDRadioButton, 
		      IDGroupBox, IDLineEdit, IDMultiLineEdit, IDIconView, IDListView,
		      IDProgressBar, IDTabWidget, IDSpinBox, IDSlider, IDComboBox, 
		      IDListBox, IDScrollBar, IDCustom, IDUnknown = 0x0100 };
    bool makeDialog();
    bool makeBitmap();
    bool makeStringTable();
    bool makeAccelerator();
    bool makeCursor();
    bool makeHTML();
    bool makeIcon();
    bool makeVersion();

    QString line;
    QTextStream *in;
    QStringList target;

    void indent();
    void undent();

    QString stripQM( const QString& string );
    QString parseNext( QString& arg, char sep = ',' );
    QStringList splitStyles( const QString& styles, char sep = '|' );
    void wi();

    void writeClass( const QString& name );
    void writeCString( const QString& name, const QString& value );
    void writeString( const QString& name, const QString& value );
    void writeRect( const QString& name, int x, int y, int w, int h );
    void writeFont( const QString& family, int pointsize );
    void writeBool( const QString& name, bool value );
    void writeNumber( const QString& name, int value );
    void writeEnum( const QString& name, const QString& value );
    void writeSet( const QString& name, const QString& value );

    void writeStyles( const QStringList styles, bool isFrame );
private:
    int indentation;
    bool writeToFile;
    QTextStream* out;

    QString useName( const QString& );

    QStringList usedNames;
};

#endif //
