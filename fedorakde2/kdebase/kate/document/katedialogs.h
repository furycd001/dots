#ifndef KATEDIALOGS_H
#define KATEDIALOGS_H
#include <kdialog.h>
#include <kdialogbase.h>
#include "katesyntaxdocument.h"
#include  "katehighlight.h"
#include <klistview.h>
#include <qtabwidget.h>
#include <kcolorbutton.h>

class QWidgetStack;
class QVBox;
class  KListView;
class QListViewItem;
struct syntaxContextData;
class QCheckBox;
//class ItemFont;
#define HlEUnknown 0
#define HlEContext 1
#define HlEItem 2
 //--------


class StyleChanger : public QWidget {
    Q_OBJECT
  public:
    StyleChanger(QWidget *parent );
    void setRef(ItemStyle *);
    void setEnabled(bool);
  protected slots:
    void changed();
  protected:
    ItemStyle *style;
    KColorButton *col;
    KColorButton *selCol;
    QCheckBox *bold;
    QCheckBox *italic;
};

class HighlightDialogPage : public QTabWidget
{
    Q_OBJECT
  public:
    HighlightDialogPage(HlManager *, ItemStyleList *, HlDataList *, int hlNumber,
                    QWidget *parent=0, const char *name=0);
    void saveData();

  protected slots:
    void defaultChanged(int);

    void hlChanged(int);
    void itemChanged(int);
    void changed();
    void hlEdit();
    void hlNew();
  protected:
    StyleChanger *defaultStyleChanger;
    ItemStyleList *defaultItemStyleList;

    void writeback();
    QComboBox *itemCombo, *hlCombo;
    QLineEdit *wildcards;
    QLineEdit *mimetypes;
    QCheckBox *styleDefault;
    StyleChanger *styleChanger;

    HlDataList *hlDataList;
    HlData *hlData;
    ItemData *itemData;
};

class ItemInfo
{
  public:
    ItemInfo():trans_i18n(),length(0){};
    ItemInfo(QString _trans,int _length):trans_i18n(_trans),length(_length){};
    QString trans_i18n;
    int length;
};

class HighlightDialog : public KDialogBase
{
  Q_OBJECT
  public:
    HighlightDialog( HlManager *hlManager, ItemStyleList *styleList,
                                  HlDataList *highlightDataList,
                                  int hlNumber, QWidget *parent,
                                  const char *name=0, bool modal=true );
  private:
    HighlightDialogPage *content;
  protected:
    virtual void done(int r);
};

class HlEditDialog : public KDialogBase
{
    Q_OBJECT
  public:
    HlEditDialog(HlManager *,QWidget *parent=0, const char *name=0, bool modal=true, HlData *data=0);
  private:
    class QWidgetStack *stack;
    class QVBox *contextOptions, *itemOptions;
    class KListView *contextList;
    class QListViewItem *currentItem;
    void initContextOptions(class QVBox *co);
    void initItemOptions(class QVBox *co);
    void loadFromDocument(HlData *hl);
    void showContext();
    void showItem();

    QListViewItem *addContextItem(QListViewItem *_parent,QListViewItem *prev,struct syntaxContextData *data);
    void insertTranslationList(QString tag, QString trans,int length);
    void newDocument();

    class QLineEdit *ContextDescr;
    class QComboBox *ContextAttribute;
    class QComboBox *ContextLineEnd;

    class QComboBox *ItemType;
    class QComboBox *ItemContext;
    class QLineEdit *ItemParameter;
    class QComboBox *ItemAttribute;

    class QMap<int,QString> id2tag;
    class QMap<int,ItemInfo> id2info;
    class QMap<QString,int> tag2id;
    int transTableCnt;
  protected slots:
    void currentSelectionChanged ( QListViewItem * );
    void contextDescrChanged(const QString&);
    void contextLineEndChanged(int);
    void contextAttributeChanged(int);
    void contextAddNew();

    void ItemTypeChanged(int id);
    void ItemParameterChanged(const QString& name);
    void ItemAttributeChanged(int attr);
    void ItemContextChanged(int cont);
    void ItemAddNew();
};

#endif
