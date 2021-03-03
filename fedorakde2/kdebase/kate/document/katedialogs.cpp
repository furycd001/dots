#include "katedialogs.h"
#include <klocale.h>
#include <kdebug.h>
#include <qgroupbox.h>
#include <qvgroupbox.h>
#include <qhgroupbox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <kcharsets.h>
#include <kglobal.h>
#include <qmap.h>
#include <kmessagebox.h>
#include <kstddirs.h>

#include "katedialogs.moc"
/*******************************************************************************************************************
*                                        Context Editor                                                            *
*******************************************************************************************************************/

StyleChanger::StyleChanger( QWidget *parent )
  : QWidget(parent)
{
  QLabel *label;

  QGridLayout *glay = new QGridLayout( this, 4, 3, 0, KDialog::spacingHint() );
  CHECK_PTR(glay);
  glay->addColSpacing( 1, KDialog::spacingHint() ); // Looks better
  glay->setColStretch( 2, 10 );

  col = new KColorButton(this);
  CHECK_PTR(col);
  connect(col,SIGNAL(changed(const QColor &)),this,SLOT(changed()));
  label = new QLabel(col,i18n("Normal:"),this);
  CHECK_PTR(label);
  glay->addWidget(label,0,0);
  glay->addWidget(col,1,0);

  selCol = new KColorButton(this);
  CHECK_PTR(selCol);
  connect(selCol,SIGNAL(changed(const QColor &)),this,SLOT(changed()));
  label = new QLabel(selCol,i18n("Selected:"),this);
  CHECK_PTR(label);
  glay->addWidget(label,2,0);
  glay->addWidget(selCol,3,0);

  bold = new QCheckBox(i18n("Bold"),this);
  CHECK_PTR(bold);
  connect(bold,SIGNAL(clicked()),SLOT(changed()));
  glay->addWidget(bold,1,2);

  italic = new QCheckBox(i18n("Italic"),this);
  CHECK_PTR(italic);
  connect(italic,SIGNAL(clicked()),SLOT(changed()));
  glay->addWidget(italic,2,2);
}

void StyleChanger::setRef(ItemStyle *s) {

  style = s;
  col->setColor(style->col);
  selCol->setColor(style->selCol);
  bold->setChecked(style->bold);
  italic->setChecked(style->italic);

}

void StyleChanger::setEnabled(bool enable) {

  col->setEnabled(enable);
  selCol->setEnabled(enable);
  bold->setEnabled(enable);
  italic->setEnabled(enable);
}

void StyleChanger::changed() {

  if (style) {
    style->col = col->color();
    style->selCol = selCol->color();
    style->bold = bold->isChecked();
    style->italic = italic->isChecked();
  }
}

HighlightDialog::HighlightDialog( HlManager *hlManager, ItemStyleList *styleList,
                                  HlDataList *highlightDataList,
                                  int hlNumber, QWidget *parent,
                                  const char *name, bool modal )
  :KDialogBase(parent,name,modal,i18n("Highlight Settings"), Ok|Cancel, Ok)
{
  QVBox *page = makeVBoxMainWidget();
  content=new HighlightDialogPage(hlManager,styleList,highlightDataList,hlNumber,page);
}

void HighlightDialog::done(int r)
{
  kdDebug(13010)<<"HighlightDialod done"<<endl;
  content->saveData();
  KDialogBase::done(r);
}

HighlightDialogPage::HighlightDialogPage(HlManager *hlManager, ItemStyleList *styleList,
                              HlDataList* highlightDataList,
                              int hlNumber,QWidget *parent, const char *name)
   :QTabWidget(parent,name),defaultItemStyleList(styleList),hlData(0L)

{

  // defaults =========================================================

  QFrame *page1 = new QFrame(this);
  addTab(page1,i18n("&Defaults"));
  QGridLayout *grid = new QGridLayout(page1, 1, 1);

  QVGroupBox *dvbox1 = new QVGroupBox( i18n("Default Item Styles"), page1 );
  /*QLabel *label = */new QLabel( i18n("Item:"), dvbox1 );
  QComboBox *styleCombo = new QComboBox( false, dvbox1 );
  defaultStyleChanger = new StyleChanger( dvbox1 );
  for( int i = 0; i < hlManager->defaultStyles(); i++ ) {
    styleCombo->insertItem(hlManager->defaultStyleName(i));
  }
  connect(styleCombo, SIGNAL(activated(int)), this, SLOT(defaultChanged(int)));
  grid->addWidget(dvbox1, 0,0);

  defaultChanged(0);

  // highlight modes =====================================================

  QFrame *page2 = new QFrame(this);
  addTab(page2,i18n("&Highlight Modes"));
  //grid = new QGridLayout(page2,2,2);
  QVBoxLayout *bl=new QVBoxLayout(page2);
  bl->setAutoAdd(true);
  QHGroupBox *hbox1 = new QHGroupBox( i18n("Config Select"), page2 );
  hbox1->layout()->setMargin(5);
  QVBox *vbox1=new QVBox(hbox1);
//  grid->addMultiCellWidget(vbox1,0,0,0,1);
  QVGroupBox *vbox2 = new QVGroupBox( i18n("Item Style"), page2 );
//  grid->addWidget(vbox2,1,0);
  QVGroupBox *vbox3 = new QVGroupBox( i18n("Highlight Auto Select"), hbox1 );
  //grid->addWidget(vbox3,1,1);

  QLabel *label = new QLabel( i18n("Highlight:"), vbox1 );
  hlCombo = new QComboBox( false, vbox1 );
  QHBox *modHl = new QHBox(vbox1);
//  connect(new QPushButton(i18n("New"),modHl),SIGNAL(clicked()),this,SLOT(hlNew()));
//  connect(new QPushButton(i18n("Edit"),modHl),SIGNAL(clicked()),this,SLOT(hlEdit()));
  connect( hlCombo, SIGNAL(activated(int)),
           this, SLOT(hlChanged(int)) );
  for( int i = 0; i < hlManager->highlights(); i++) {
    hlCombo->insertItem(hlManager->hlName(i));
  }
  hlCombo->setCurrentItem(hlNumber);


  label = new QLabel( i18n("Item:"), vbox2 );
  itemCombo = new QComboBox( false, vbox2 );
  connect( itemCombo, SIGNAL(activated(int)), this, SLOT(itemChanged(int)) );

  label = new QLabel( i18n("File Extensions:"), vbox3 );
  wildcards  = new QLineEdit( vbox3 );
  label = new QLabel( i18n("Mime Types:"), vbox3 );
  mimetypes = new QLineEdit( vbox3 );


  styleDefault = new QCheckBox(i18n("Default"), vbox2 );
  connect(styleDefault,SIGNAL(clicked()),SLOT(changed()));
  styleChanger = new StyleChanger( vbox2 );

  hlDataList = highlightDataList;
  hlChanged(hlNumber);
}


void HighlightDialogPage::defaultChanged(int z)
{
  defaultStyleChanger->setRef(defaultItemStyleList->at(z));
}


void HighlightDialogPage::hlChanged(int z)
{
  writeback();

  hlData = hlDataList->at(z);

  wildcards->setText(hlData->wildcards);
  mimetypes->setText(hlData->mimetypes);

  itemCombo->clear();
  for (ItemData *itemData = hlData->itemDataList.first(); itemData != 0L;
    itemData = hlData->itemDataList.next()) {
    kdDebug(13010) << itemData->name << endl;
    itemCombo->insertItem(i18n(itemData->name.latin1()));
  }

  itemChanged(0);
}

void HighlightDialogPage::itemChanged(int z)
{
  itemData = hlData->itemDataList.at(z);

  styleDefault->setChecked(itemData->defStyle);
  styleChanger->setRef(itemData);
}

void HighlightDialogPage::changed()
{
  itemData->defStyle = styleDefault->isChecked();
}

void HighlightDialogPage::writeback() {
  if (hlData) {
    hlData->wildcards = wildcards->text();
    hlData->mimetypes = mimetypes->text();
  }
}

void HighlightDialogPage::saveData() {
  kdDebug(13010)<<"HighlightDialogPage::saveData()"<<endl;
  writeback();
}


void HighlightDialogPage::hlEdit() {
  HlEditDialog diag(0,0,"hlEdit", true,hlData);
  diag.show();
}

void HighlightDialogPage::hlNew() {
  HlEditDialog diag(0,0,"hlEdit",true,0);
  diag.show();
}


HlEditDialog::HlEditDialog(HlManager *,QWidget *parent, const char *name, bool modal,HlData *data)
  :KDialogBase(KDialogBase::Swallow, i18n("Highlight Conditions"), Ok|Cancel, Ok, parent, name, modal)
{
  currentItem=0;
    transTableCnt=0;
  QHBox *wid=new QHBox(this);
  QVBox *lbox=new QVBox(wid);
    contextList=new KListView(lbox);
    contextList->setRootIsDecorated(true);
    contextList->addColumn(i18n("Syntax structure"));
    contextList->setSorting(-1);
    QHBox *bbox=new QHBox(lbox);
    QPushButton *addContext=new QPushButton(i18n("New Context"),bbox);
    QPushButton *addItem=new QPushButton(i18n("New Item"),bbox);
    QVGroupBox *opt  = new QVGroupBox( i18n("Options"), wid);
    stack=new QWidgetStack(opt);
    initContextOptions(contextOptions=new QVBox(stack));
    stack->addWidget(contextOptions,HlEContext);
    initItemOptions(itemOptions=new QVBox(stack));
    stack->addWidget(itemOptions,HlEItem);
    stack->raiseWidget(HlEContext);
    setMainWidget(wid);
    if (data!=0) loadFromDocument(data);
    else newDocument();
    connect(contextList,SIGNAL(currentChanged( QListViewItem*)),this,SLOT(currentSelectionChanged ( QListViewItem * )));
    connect(addContext,SIGNAL(clicked()),this,SLOT(contextAddNew()));
    connect(addItem,SIGNAL(clicked()),this,SLOT(ItemAddNew()));
    }

void HlEditDialog::newDocument()
{
  KStandardDirs *dirs = KGlobal::dirs();
  QStringList list=dirs->findAllResources("data","kate/syntax/syntax.template",false,true);
  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
      {
        HlData data("","",*it);
        loadFromDocument(&data);
        return;
      }
  KMessageBox::error(this,i18n("Can't find template file"));
}


void HlEditDialog::initContextOptions(QVBox *co)
{
  if( co!=0)
    {
        QHBox *tmp = new QHBox(co);
        (void) new QLabel(i18n("Description:"),tmp);
        ContextDescr=new QLineEdit(tmp);
        tmp= new QHBox(co);
        (void) new QLabel(i18n("Attribute:"),tmp);
        ContextAttribute=new QComboBox(tmp);
        tmp= new QHBox(co);
        (void) new QLabel(i18n("LineEnd:"),tmp);
        ContextLineEnd = new QComboBox(tmp);
        connect(ContextDescr,SIGNAL(textChanged(const QString&)),this,SLOT(contextDescrChanged(const QString&)));
        connect(ContextLineEnd,SIGNAL(activated(int)),this,SLOT(contextLineEndChanged(int)));
        connect(ContextAttribute,SIGNAL(activated(int)),this,SLOT(contextAttributeChanged(int)));
    }
   else
     kdDebug(13010)<<"initContextOptions: Widget is 0"<<endl;
}


void HlEditDialog::insertTranslationList(QString tag, QString trans,int length)
  {
    ItemInfo data(trans,length);
    id2tag.insert(transTableCnt,tag);
    id2info.insert(transTableCnt,data);
    tag2id.insert(tag,transTableCnt);
    transTableCnt++;
  }


void HlEditDialog::initItemOptions(QVBox *co)
{
  if (co!=0)
    {
        QHBox *tmp = new QHBox(co);
        (void) new QLabel(i18n("Type:"),tmp);
        ItemType = new QComboBox(tmp);
        tmp= new QHBox(co);
        (void) new QLabel(i18n("Parameter:"),tmp);
        ItemParameter=  new QLineEdit(tmp);
        tmp= new QHBox(co);
        (void) new QLabel(i18n("Attribute:"),tmp);
        ItemAttribute= new QComboBox(tmp);
        (void) new QLabel(i18n("Context switch:"),tmp);
        ItemContext = new QComboBox(tmp);
        co->setSpacing(15);
        QPushButton *delItem=new QPushButton(i18n("Delete this item"),co);

	/* init translation lists */
	insertTranslationList("CharDetect","CharDetect",1);
        insertTranslationList("2CharDetect","2CharDetect",2);
	insertTranslationList("RangeDetect","RangeDetect",2);
	insertTranslationList("StringDetect","StringDetect",-1);
	insertTranslationList("AnyChar","AnyChar",-1);
	insertTranslationList("RegExpr","RegExpr",-1);
	insertTranslationList("Int","Int",0);
	insertTranslationList("Float","Float",0);
	insertTranslationList("keyword","keyword",0);
	insertTranslationList("dataType","dataType",0);
        ItemType->clear();
        for (int i=0; i<transTableCnt; i++) ItemType->insertItem(id2info[i].trans_i18n);
        connect(ItemType,SIGNAL(activated(int)),this,SLOT(ItemTypeChanged(int)));
        connect(ItemParameter,SIGNAL(textChanged(const QString&)),this,SLOT(ItemParameterChanged(const QString&)));
        connect(ItemAttribute,SIGNAL(activated(int)),this,SLOT(ItemAttributeChanged(int)));
        connect(ItemContext,SIGNAL(activated(int)),this,SLOT(ItemContextChanged(int)));
    }
  else
    kdDebug(13010)<<"initItemOptions: Widget is 0"<<endl;
}

void HlEditDialog::loadFromDocument(HlData *hl)
{
  struct syntaxContextData *data;
  QListViewItem *last=0,*lastsub=0;

  HlManager::self()->syntax->setIdentifier(hl->identifier);
  data=HlManager::self()->syntax->getGroupInfo("highlighting","context");
  int i=0;
  if (data)
    {
      while (HlManager::self()->syntax->nextGroup(data)) //<context tags>
        {
        kdDebug(13010)<< "Adding context to list"<<endl;
          last= new QListViewItem(contextList,last,
                 HlManager::self()->syntax->groupData(data,QString("name")),
                 QString("%1").arg(i),
                 HlManager::self()->syntax->groupData(data,QString("attribute")),
                 HlManager::self()->syntax->groupData(data,QString("lineEndContext")));
          i++;
          lastsub=0;
          while (HlManager::self()->syntax->nextItem(data))
              {
                kdDebug(13010)<< "Adding item to list"<<endl;
                lastsub=addContextItem(last,lastsub,data);
              }


	 }
       if (data) HlManager::self()->syntax->freeGroupInfo(data);
   }
  ContextAttribute->clear();
  ItemAttribute->clear();
  data=HlManager::self()->syntax->getGroupInfo("highlighting","itemData");
  while (HlManager::self()->syntax->nextGroup(data))
    {
        ContextAttribute->insertItem(HlManager::self()->syntax->groupData(data,QString("name")));
        ItemAttribute->insertItem(HlManager::self()->syntax->groupData(data,QString("name")));
    }
  if (data) HlManager::self()->syntax->freeGroupInfo(data);
}

QListViewItem *HlEditDialog::addContextItem(QListViewItem *_parent,QListViewItem *prev,struct syntaxContextData *data)
  {

                kdDebug(13010)<<HlManager::self()->syntax->groupItemData(data,QString("name")) << endl;

                QString dataname=HlManager::self()->syntax->groupItemData(data,QString(""));
                QString attr=(HlManager::self()->syntax->groupItemData(data,QString("attribute")));
                QString context=(HlManager::self()->syntax->groupItemData(data,QString("context")));
		char chr;
                if (! HlManager::self()->syntax->groupItemData(data,QString("char")).isEmpty())
		  chr= (HlManager::self()->syntax->groupItemData(data,QString("char")).latin1())[0];
		else
                  chr=0;
		QString stringdata=HlManager::self()->syntax->groupItemData(data,QString("String"));
                char chr1;
                if (! HlManager::self()->syntax->groupItemData(data,QString("char1")).isEmpty())
		  chr1= (HlManager::self()->syntax->groupItemData(data,QString("char1")).latin1())[0];
		else
                  chr1=0;
		bool insensitive=(HlManager::self()->syntax->groupItemData(data,QString("insensitive"))==QString("TRUE"));
                QString param("");
                if ((dataname=="keyword") || (dataname=="dataType")) param=dataname;
                  else if (dataname=="CharDetect") param=chr;
                    else if ((dataname=="2CharDetect") || (dataname=="RangeDetect")) param=QString("%1%2").arg(chr).arg(chr1);
                      else if ((dataname=="StringDetect") || (dataname=="AnyChar") || (dataname=="RegExpr")) param=stringdata;
                        else                     kdDebug(13010)<<"***********************************"<<endl<<"Unknown entry for Context:"<<dataname<<endl;
                kdDebug(13010)<<dataname << endl;
                return new QListViewItem(_parent,prev,i18n(dataname.latin1())+" "+param,dataname,param,attr,context);
 }


void HlEditDialog::currentSelectionChanged ( QListViewItem *it)
  {
     kdDebug(13010)<<"Update data view"<<endl<<"Depth:"<<it->depth()<<endl;
     currentItem=it;
     if (it->depth()==0) showContext();
        else showItem();
  }


/****************************************************************************/
/*                              CONTEXTS                                    */
/****************************************************************************/


void HlEditDialog::showContext()
  {
    stack->raiseWidget(HlEContext);
    ContextDescr->setText(currentItem->text(0));
    ContextAttribute->setCurrentItem(currentItem->text(2).toInt());
    ContextLineEnd->clear();
    for (QListViewItem *it=contextList->firstChild();it;it=it->nextSibling())
        ContextLineEnd->insertItem(it->text(0));
    ContextLineEnd->setCurrentItem(currentItem->text(3).toInt());
//    ContextAttribute->setText(currentItem->text(1));
//    ContextLineEnd->setText(currentItem->text(2));
  }

void HlEditDialog::contextDescrChanged(const QString& name)
  {
    if (currentItem)
      {
        currentItem->setText(0,name);
        ContextLineEnd->changeItem(name,currentItem->text(3).toInt());
      }
  }

void HlEditDialog::contextAttributeChanged(int id)
{
  if (currentItem)
     {
     currentItem->setText(2,QString("%1").arg(id));
     }
}

void HlEditDialog::contextLineEndChanged(int id)
{
  kdDebug(13010)<<"contextLineEndChanged"<<endl;
  if (currentItem)
     {
     currentItem->setText(3,QString("%1").arg(id));
     }
}

void HlEditDialog::contextAddNew()
{
  QListViewItem *it=contextList->firstChild();
  for (;it->nextSibling()!=0;it=it->nextSibling());
  it=new QListViewItem(contextList,it,i18n("New Context"),QString("%1").arg(it->text(1).toInt()),"0","0");
  contextList->setSelected(it,true);
}

/****************************************************************************/
/*                              ITEMS                                       */
/****************************************************************************/

void HlEditDialog::showItem()
  {
    stack->raiseWidget(HlEItem);
    ItemContext->clear();
    for (QListViewItem *it=contextList->firstChild();it;it=it->nextSibling())
        ItemContext->insertItem(it->text(0));
    ItemContext->setCurrentItem(currentItem->text(4).toInt());
    ItemAttribute->setCurrentItem(currentItem->text(3).toInt());
    QMap<QString,int>::Iterator iter=tag2id.find(currentItem->text(1));
    if (iter==tag2id.end())
      kdDebug(13010)<<"Oops, unknown itemtype in showItem: "<<currentItem->text(1)<<endl;
    else
      {
        ItemType->setCurrentItem(*iter);
        if (id2info[*iter].length==0) ItemParameter->hide();
          else
           {
             ItemParameter->setMaxLength(id2info[*iter].length);
             ItemParameter->show();
             ItemParameter->setText(currentItem->text(2));
           }
      }

  }

void HlEditDialog::ItemTypeChanged(int id)
{
  if (currentItem)
     {
     currentItem->setText(1,id2tag[id]);
     ItemParameter->setMaxLength(id2info[id].length);
     ItemParameterChanged(ItemParameter->text());
     }
}

void HlEditDialog::ItemParameterChanged(const QString& name)
{
  if (currentItem)
    {
      currentItem->setText(2,name);
      currentItem->setText(0,id2info[ItemType->currentItem()].trans_i18n+" "+currentItem->text(2));
    }
}

void HlEditDialog::ItemAttributeChanged(int attr)
{
   if (currentItem)
     {
       currentItem->setText(3,QString("%1").arg(attr));
     }
}

void HlEditDialog::ItemContextChanged(int cont)
{
   if (currentItem)
     {
       currentItem->setText(4,QString("%1").arg(cont));
     }
}

void HlEditDialog::ItemAddNew()
{
  QListViewItem *it;
  if (currentItem)
    {
      if (currentItem->depth()==0) it=currentItem->firstChild();
        else
          it=currentItem;
      if (it) for (;it->nextSibling();it=it->nextSibling());
      (void) new QListViewItem(it ? it->parent() : currentItem,it,"StringDetect "+i18n("New Item"),"StringDetect",i18n("New Item"),0,it ? it->parent()->text(1) : currentItem->text(1));
    }
}
