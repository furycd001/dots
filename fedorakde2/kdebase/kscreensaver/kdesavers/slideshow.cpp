/* Slide Show Screen Saver
 *  (C) 1999 Stefan Taferner <taferner@kde.org>
 *
 * This code is under GPL
 *
 * 2001/03/04 Converted to libkscreensaver by Martin R. Jones.
 */


#include <qdir.h>
#include <qpushbutton.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qfile.h>
#include <qcolor.h>
#include <qpaintdevicemetrics.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qslider.h>

#include <kconfig.h>
#include <kglobal.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <kmessagebox.h>
#include <kimageio.h>
#include <kfiledialog.h>
#include <kstddirs.h>

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "slideshow.h"
#include "slideshow.moc"

// libkscreensaver interface
extern "C"
{
    const char *kss_applicationName = "kslideshow.kss";
    const char *kss_description = I18N_NOOP( "KSlideshow" );
    const char *kss_version = "2.2.0";

    KScreenSaver *kss_create( WId id )
    {
        return new kSlideShowSaver( id );
    }

    QDialog *kss_setup()
    {
        return new kSlideShowSetup();
    }
}


//=============================================================================
//  Class kSlideShowSaver
//=============================================================================
kSlideShowSaver::kSlideShowSaver( WId id ): KScreenSaver(id)
{
  time_t t;

  time(&t);
  srand((unsigned int)t);

  KImageIO::registerFormats();

  blank();

  mEffect = NULL;
  mNumEffects = 0;
  mIntArray = NULL;
  registerEffects();

  readConfig();
  initNextScreen();

  mFileIdx = 0;
  mColorContext = QColor::enterAllocContext();

  mEffectRunning = false;
  loadNextImage();
  createNextScreen();

  mTimer.start(10, true);
  connect(&mTimer, SIGNAL(timeout()), SLOT(slotTimeout()));
}


//----------------------------------------------------------------------------
kSlideShowSaver::~kSlideShowSaver()
{
  mTimer.stop();
  if (mPainter.isActive()) mPainter.end();
  QColor::leaveAllocContext();
  QColor::destroyAllocContext(mColorContext);
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::initNextScreen()
{
  QPaintDeviceMetrics metric(this);
  int w, h;

  w = width();
  h = height();
  mNextScreen = QPixmap(w, h, metric.depth());
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::readConfig()
{
  KConfig *config = KGlobal::config();
  config->setGroup("Settings");
  mShowRandom = config->readBoolEntry("ShowRandom", true);
  mZoomImages = config->readBoolEntry("ZoomImages", false);
  mPrintName = config->readBoolEntry("PrintName", true);
  mDirectory = config->readEntry("Directory", KGlobal::dirs()->saveLocation("data", "kslideshow/pics", false));
  mDelay = config->readNumEntry("Delay", 10) * 1000;

  loadDirectory();
  // loadFileList("slideshow.list");
}


//----------------------------------------------------------------------------
void kSlideShowSaver::registerEffects()
{
  int i = 0;

  mEffectList = new EffectMethod[64];
  mEffectList[i++] = &kSlideShowSaver::effectChessboard;
  mEffectList[i++] = &kSlideShowSaver::effectMultiCircleOut;
  mEffectList[i++] = &kSlideShowSaver::effectSpiralIn;
  mEffectList[i++] = &kSlideShowSaver::effectSweep;
  mEffectList[i++] = &kSlideShowSaver::effectMeltdown;
  mEffectList[i++] = &kSlideShowSaver::effectCircleOut;
  mEffectList[i++] = &kSlideShowSaver::effectBlobs;
  mEffectList[i++] = &kSlideShowSaver::effectHorizLines;
  mEffectList[i++] = &kSlideShowSaver::effectVertLines;
  mEffectList[i++] = &kSlideShowSaver::effectRandom;
  mEffectList[i++] = &kSlideShowSaver::effectGrowing;
  mEffectList[i++] = &kSlideShowSaver::effectIncomingEdges;

  mNumEffects = i;
  // mNumEffects = 1;  //...for testing
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectMultiCircleOut(bool aInit)
{
  int x, y, i;
  double alpha;
  static QPointArray pa(4);

  if (aInit)
  {
    startPainter();
    mw = width();
    mh = height();
    mx = mw;
    my = mh>>1;
    pa.setPoint(0, mw>>1, mh>>1);
    pa.setPoint(3, mw>>1, mh>>1);
    mfy = sqrt((double)mw*mw + mh*mh) / 2;
    mi  = rand()%15 + 2;
    mfd = M_PI*2/mi;
    mAlpha = mfd;
    mwait = 10 * mi;
    mfx = M_PI/32;  // divisor must be powers of 8
  }

  if (mAlpha < 0)
  {
    mPainter.end();
    showNextScreen();
    return -1;
  }

  for (alpha=mAlpha, i=mi; i>=0; i--, alpha+=mfd)
  {
    x = (mw>>1) + (int)(mfy * cos(-alpha));
    y = (mh>>1) + (int)(mfy * sin(-alpha));

    mx = (mw>>1) + (int)(mfy * cos(-alpha + mfx));
    my = (mh>>1) + (int)(mfy * sin(-alpha + mfx));

    pa.setPoint(1, x, y);
    pa.setPoint(2, mx, my);

    mPainter.drawPolygon(pa);
  }
  mAlpha -= mfx;

  return mwait;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectSpiralIn(bool aInit)
{
  if (aInit)
  {
    startPainter();
    mw = width();
    mh = height();
    mix = mw / 8;
    miy = mh / 8;
    mx0 = 0;
    mx1 = mw - mix;
    my0 = miy;
    my1 = mh - miy;
    mdx = mix;
    mdy = 0;
    mi = 0;
    mj = 16 * 16;
    mx = 0;
    my = 0;
  }

  if (mi==0 && mx0>=mx1)
  {
    mPainter.end();
    showNextScreen();
    return -1;
  }

  if (mi==0 && mx>=mx1) // switch to: down on right side
  {
    mi = 1;
    mdx = 0;
    mdy = miy;
    mx1 -= mix;
  }
  else if (mi==1 && my>=my1) // switch to: right to left on bottom side
  {
    mi = 2;
    mdx = -mix;
    mdy = 0;
    my1 -= miy;
  }
  else if (mi==2 && mx<=mx0) // switch to: up on left side
  {
    mi = 3;
    mdx = 0;
    mdy = -miy;
    mx0 += mix;
  }
  else if (mi==3 && my<=my0) // switch to: left to right on top side
  {
    mi = 0;
    mdx = mix;
    mdy = 0;
    my0 += miy;
  }

  bitBlt(this, mx, my, &mNextScreen, mx, my, mix, miy, CopyROP, true);

  mx += mdx;
  my += mdy;
  mj--;

  return 8;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectMeltdown(bool aInit)
{
  int i, x, y;
  bool done;

  if (aInit)
  {
    if (mIntArray) delete mIntArray;
    mw = width();
    mh = height();
    mdx = 4;
    mdy = 16;
    mix = mw / mdx;
    mIntArray = new int[mix];
    for (i=mix-1; i>=0; i--)
      mIntArray[i] = 0;
  }

  done = true;
  for (i=0,x=0; i<mix; i++,x+=mdx)
  {
    y = mIntArray[i];
    if (y >= mh) continue;
    done = false;
    if ((rand()&15) < 6) continue;
    bitBlt(this, x, y+mdy, this, x, y, mdx, mh-y-mdy, CopyROP, true);
    bitBlt(this, x, y, &mNextScreen, x, y, mdx, mdy, CopyROP, true);
    mIntArray[i] += mdy;
  }

  if (done)
  {
    delete mIntArray;
    mIntArray = NULL;
    return -1;
  }

  return 15;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectCircleOut(bool aInit)
{
  int x, y;
  static QPointArray pa(4);

  if (aInit)
  {
    startPainter();
    mw = width();
    mh = height();
    mx = mw;
    my = mh>>1;
    mAlpha = 2*M_PI;
    pa.setPoint(0, mw>>1, mh>>1);
    pa.setPoint(3, mw>>1, mh>>1);
    mfx = M_PI/16;  // divisor must be powers of 8
    mfy = sqrt((double)mw*mw + mh*mh) / 2;
  }

  if (mAlpha < 0)
  {
    mPainter.end();
    showNextScreen();
    return -1;
  }

  x = mx;
  y = my;
  mx = (mw>>1) + (int)(mfy * cos(mAlpha));
  my = (mh>>1) + (int)(mfy * sin(mAlpha));
  mAlpha -= mfx;

  pa.setPoint(1, x, y);
  pa.setPoint(2, mx, my);

  mPainter.drawPolygon(pa);

  return 20;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectSweep(bool aInit)
{
  int w, h, x, y, i;

  if (aInit)
  {
    // subtype: 0=sweep right to left, 1=sweep left to right
    //          2=sweep bottom to top, 3=sweep top to bottom
    mSubType = rand() % 4;
    mw  = width();
    mh  = height();
    mdx = (mSubType==1 ? 16 : -16);
    mdy = (mSubType==3 ? 16 : -16);
    mx  = (mSubType==1 ? 0 : mw);
    my  = (mSubType==3 ? 0 : mh);
  }

  if (mSubType==0 || mSubType==1)
  {
    // horizontal sweep
    if ((mSubType==0 && mx < -64) ||
	(mSubType==1 && mx > mw+64))
    {
       return -1;
    }
    for (w=2,i=4,x=mx; i>0; i--, w<<=1, x-=mdx)
    {
      bitBlt(this, x, 0, &mNextScreen, x, 0, w, mh, CopyROP, true);
    }
    mx += mdx;
  }
  else
  {
    // vertical sweep
    if ((mSubType==2 && my < -64) ||
	(mSubType==3 && my > mh+64))
    {
      return -1;
    }
    for (h=2,i=4,y=my; i>0; i--, h<<=1, y-=mdy)
    {
      bitBlt(this, 0, y, &mNextScreen, 0, y, mw, h, CopyROP, true);
    }
    my += mdy;
  }

  return 20;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectBlobs(bool aInit)
{
  int r;

  if (aInit)
  {
    startPainter();
    mAlpha = M_PI * 2;
    mw = width();
    mh = height();
    mi = 150;
  }

  if (mi <= 0)
  {
    mPainter.end();
    showNextScreen();
    return -1;
  }

  mx = rand() % mw;
  my = rand() % mh;
  r = (rand() % 200) + 50;

  mPainter.drawEllipse(mx-r, my-r, r, r);
  mi--;

  return 10;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectRandom(bool /*aInit*/)
{
  int x, y, i, w, h, fact, sz;

  fact = (rand() % 3) + 1;

  w = width() >> fact;
  h = height() >> fact;
  sz = 1 << fact;

  for (i = (w*h)<<1; i > 0; i--)
  {
    x = (rand() % w) << fact;
    y = (rand() % h) << fact;
    bitBlt(this, x, y, &mNextScreen, x, y, sz, sz, CopyROP, true);
  }
  showNextScreen();

  return -1;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectGrowing(bool aInit)
{
  if (aInit)
  {
    mw = width();
    mh = height();
    mx = mw >> 1;
    my = mh >> 1;
    mi = 0;
    mfx = mx / 100.0;
    mfy = my / 100.0;
  }

  mx = (mw>>1) - (int)(mi * mfx);
  my = (mh>>1) - (int)(mi * mfy);
  mi++;

  if (mx<0 || my<0)
  {
    showNextScreen();
    return -1;
  }

  bitBlt(this, mx, my, &mNextScreen, mx, my,
	 mw - (mx<<1), mh - (my<<1), CopyROP, true);

  return 20;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectChessboard(bool aInit)
{
  int y;

  if (aInit)
  {
    mw  = width();
    mh  = height();
    mdx = 8;         // width of one tile
    mdy = 8;         // height of one tile
    mj  = (mw+mdx-1)/mdx; // number of tiles
    mx  = mj*mdx;    // shrinking x-offset from screen border
    mix = 0;         // growing x-offset from screen border
    miy = 0;         // 0 or mdy for growing tiling effect
    my  = mj&1 ? 0 : mdy; // 0 or mdy for shrinking tiling effect
    mwait = 800 / mj; // timeout between effects
  }

  if (mix >= mw)
  {
    showNextScreen();
    return -1;
  }

  mix += mdx;
  mx  -= mdx;
  miy = miy ? 0 : mdy;
  my  = my ? 0 : mdy;

  for (y=0; y<mw; y+=(mdy<<1))
  {
    bitBlt(this, mix, y+miy, &mNextScreen, mix, y+miy, 
	   mdx, mdy, CopyROP, true);
    bitBlt(this, mx, y+my, &mNextScreen, mx, y+my, 
	   mdx, mdy, CopyROP, true);
  }

  return mwait;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectIncomingEdges(bool aInit)
{
  int x1, y1;

  if (aInit)
  {
    mw = width();
    mh = height();
    mix = mw >> 1;
    miy = mh >> 1;
    mfx = mix / 100.0;
    mfy = miy / 100.0;
    mi = 0;
    mSubType = rand() & 1;
  }

  mx = (int)(mfx * mi);
  my = (int)(mfy * mi);

  if (mx>mix || my>miy)
  {
    showNextScreen();
    return -1;
  }

  x1 = mw - mx;
  y1 = mh - my;
  mi++;

  if (mSubType)
  {
    // moving image edges
    bitBlt(this,  0,  0, &mNextScreen, mix-mx, miy-my, mx, my, CopyROP, true);
    bitBlt(this, x1,  0, &mNextScreen, mix, miy-my, mx, my, CopyROP, true);
    bitBlt(this,  0, y1, &mNextScreen, mix-mx, miy, mx, my, CopyROP, true);
    bitBlt(this, x1, y1, &mNextScreen, mix, miy, mx, my, CopyROP, true);
  }
  else
  {
    // fixed image edges
    bitBlt(this,  0,  0, &mNextScreen,  0,  0, mx, my, CopyROP, true);
    bitBlt(this, x1,  0, &mNextScreen, x1,  0, mx, my, CopyROP, true);
    bitBlt(this,  0, y1, &mNextScreen,  0, y1, mx, my, CopyROP, true);
    bitBlt(this, x1, y1, &mNextScreen, x1, y1, mx, my, CopyROP, true);
  }
  return 20;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectHorizLines(bool aInit)
{
  static int iyPos[] = { 0, 4, 2, 6, 1, 5, 3, 7, -1 };
  int y;

  if (aInit)
  {
    mw = width();
    mh = height();
    mi = 0;
  }

  if (iyPos[mi] < 0) return -1;

  for (y=iyPos[mi]; y<mh; y+=8)
  {
    bitBlt(this, 0, y, &mNextScreen, 0, y, mw, 1, CopyROP, true);
  }

  mi++;
  if (iyPos[mi] >= 0) return 160;
  return -1;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectVertLines(bool aInit)
{
  static int ixPos[] = { 0, 4, 2, 6, 1, 5, 3, 7, -1 };
  int x;

  if (aInit)
  {
    mw = width();
    mh = height();
    mi = 0;
  }

  if (ixPos[mi] < 0) return -1;

  for (x=ixPos[mi]; x<mw; x+=8)
  {
    bitBlt(this, x, 0, &mNextScreen, x, 0, 1, mh, CopyROP, true);
  }

  mi++;
  if (ixPos[mi] >= 0) return 160;
  return -1;
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::startPainter(Qt::PenStyle aPen)
{
  QBrush brush;
  brush.setPixmap(mNextScreen);
  if (mPainter.isActive()) mPainter.end();
  mPainter.begin(this);
  mPainter.setBrush(brush);
  mPainter.setPen(aPen);
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::restart()
{
  mEffectRunning = false;
  mEffect = NULL;
  blank();
  slotTimeout();
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::slotTimeout()
{
  int tmout = -1;
  int i;

  if (mEffectRunning)
  {
    tmout = (this->*mEffect)(false);
  }
  else
  {
    loadNextImage();
    createNextScreen();

    if (mNumEffects > 1) i = rand() % mNumEffects;
    else i = 0;

    mEffect = mEffectList[i];
    mEffectRunning = true;
    tmout = (this->*mEffect)(true);
  }
  if (tmout <= 0)
  {
    tmout = mDelay;
    mEffectRunning = false;
  }
  mTimer.start(tmout, true);
}


//----------------------------------------------------------------------------
void kSlideShowSaver::showNextScreen()
{
  bitBlt(this, 0, 0, &mNextScreen, 0, 0,
	 mNextScreen.width(), mNextScreen.height(), CopyROP, true);
}


//----------------------------------------------------------------------------
void kSlideShowSaver::createNextScreen()
{
  QPainter p;
  int ww, wh, iw, ih, x, y;
  double fx, fy;

  if (mNextScreen.size() != size())
    mNextScreen.resize(size());

  mNextScreen.fill(black);

  ww = mNextScreen.width();
  wh = mNextScreen.height();
  iw = mImage.width();
  ih = mImage.height();

  p.begin(&mNextScreen);

  if (mFileList.isEmpty())
  {
    p.setPen(QColor("white"));
    p.drawText(20 + (rand() % (ww>>1)), 20 + (rand() % (wh>>1)),
	       i18n("No images found"));
  }
  else
  {
    if (mZoomImages)
    {
      fx = (double)ww / iw;
      fy = (double)wh / ih;
      if (fx > fy) fx = fy;
      if (fx > 2) fx = 2;
      iw = (int)(iw * fx);
      ih = (int)(ih * fx);
      QImage scaledImg = mImage.smoothScale(iw, ih);

      x = (ww - iw) >> 1;
      y = (wh - ih) >> 1;

      p.drawImage(x, y, scaledImg);
    }
    else
    {
      x = (ww - iw) >> 1;
      y = (wh - ih) >> 1;

      // bitBlt(&mNextScreen, x, y, &mImage, 0, 0, iw, ih, CopyROP, false);
      p.drawImage(x, y, mImage);
    }

    if (mPrintName)
    {
      p.setPen(QColor("black"));
      for (x=9; x<=11; x++)
	for (y=21; y>=19; y--)
	  p.drawText(x, wh-y, mImageName);
      p.setPen(QColor("white"));
      p.drawText(10, wh-20, mImageName);
    }
  }

  p.end();
}


//----------------------------------------------------------------------------
void kSlideShowSaver::loadNextImage()
{
  QString fname, fpath;
  int num, i, j;

  if (mShowRandom)
  {
    num = mRandomList.count();
    if (num <= 0)
    {
      mRandomList = mFileList;
      num = mRandomList.count();
    }
    if (num <= 0) return;
    mFileIdx = rand() % num;
    fname = mRandomList[mFileIdx];
    mRandomList.remove(fname);
  }
  else
  {
    num = mFileList.count();
    if (mFileIdx >= num) mFileIdx = 0;
    fname = mFileList[mFileIdx];
  }

  if (!mDirectory.isEmpty()) fpath = mDirectory + '/' + fname;
  else fpath = fname;

  if (!mImage.load(fpath))
  {
    printf(i18n("Failed to load image \"%1\"\n").arg(fpath).local8Bit());
    mFileList.remove(fname);
    mRandomList.remove(fname);
    if (!mFileList.isEmpty()) loadNextImage();
    return;
  }
  mFileIdx++;

  i = fname.findRev('.');
  if (i < 0) i = 32767;
  j = fname.findRev('/') + 1;
  if (j < 0) j = 0;
  mImageName = fname.mid(j, i-j);
}


//----------------------------------------------------------------------------
void kSlideShowSaver::loadDirectory()
{
  QDir dir(mDirectory);

  mFileIdx = 0;
  mFileList.clear();
  mFileList = dir.entryList();
  mFileList.remove(".");
  mFileList.remove("..");
}


//----------------------------------------------------------------------------
void kSlideShowSaver::loadFileList(const QCString& aFileName)
{
  QFile file(aFileName);
  QString fname;

  mFileList.clear();
  mFileIdx = 0;

  if (!file.open(IO_ReadOnly)) return;

  while (!file.atEnd())
  {
    if (file.readLine(fname, 256) > 0)
    {
      fname = fname.stripWhiteSpace();
      if (fname.isEmpty() || fname[0] == '#') continue;
      mFileList.append(fname);
    }
  }
  file.close();
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::blank()
{
   erase();
}


//=============================================================================
//  Class kSlideShowSetup
//=============================================================================
kSlideShowSetup::kSlideShowSetup(QWidget *aParent, const char *aName):
  QDialog(aParent, aName, TRUE )
{
  setCaption(i18n("Setup Slide Show Screen Saver"));

  QPushButton *button;
  mSaver = NULL;

  QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);
  QHBoxLayout *tl1 = new QHBoxLayout;
  tl->addLayout(tl1);
  QVBoxLayout *tl11 = new QVBoxLayout(5);
  tl1->addLayout(tl11);

  mCbxZoom = new QCheckBox(i18n("Zoom pictures"), this);
  connect(mCbxZoom, SIGNAL(clicked()), SLOT(writeSettings()));
  tl11->addWidget(mCbxZoom);

  mCbxRandom = new QCheckBox(i18n("Random play"), this);
  connect(mCbxRandom, SIGNAL(clicked()), SLOT(writeSettings()));
  tl11->addWidget(mCbxRandom);

  mCbxShowName = new QCheckBox(i18n("Show names"), this);
  connect(mCbxShowName, SIGNAL(clicked()), SLOT(writeSettings()));
  tl11->addWidget(mCbxShowName);
  tl11->addSpacing(5);

  mLblDelay = new QLabel(i18n("Delay:"), this);
  mLblDelay->setMaximumWidth(32767);
  tl11->addWidget(mLblDelay);

  mDelay = new QSlider(1, 60, 10, 1, QSlider::Horizontal, this);
  mDelay->setMinimumSize(90, 20);
  mDelay->setTracking(false);
  mDelay->setTickmarks(QSlider::Right);
  mDelay->setTickInterval(10);
  connect(mDelay, SIGNAL(valueChanged(int)), SLOT(slotDelay(int)));
  tl11->addWidget(mDelay);
  tl11->addSpacing(5);

  tl11->addStretch(1000);

  mPreview = new QWidget(this);
  mPreview->setFixedSize(220, 170);
  mPreview->setBackgroundColor(black);
  mPreview->show();    // otherwise saver does not get correct size
  mSaver = new kSlideShowSaver(mPreview->winId());
  tl1->addWidget(mPreview);

  mCboDir = new QComboBox(false, this);
  mCboDir->insertItem(i18n("Select..."));
  mCboDir->setMaxCount(21);
  mCboDir->setSizeLimit(20);
  mCboDir->setMaximumWidth(32767);
  connect(mCboDir, SIGNAL(activated(int)), SLOT(slotDirSelected(int)));
  tl->addWidget(mCboDir);
  //  tl->addSpacing(5);

  KButtonBox *bbox = new KButtonBox(this);	
  button = bbox->addButton(i18n("About"));
  connect(button, SIGNAL(clicked()), SLOT(slotAbout()));
  bbox->addStretch(1);

  button = bbox->addButton(i18n("OK"));	
  connect(button, SIGNAL(clicked()), SLOT(slotOkPressed()));

  button = bbox->addButton(i18n("Cancel"));
  connect(button, SIGNAL(clicked()), SLOT(reject()));
  bbox->layout();
  tl->addWidget(bbox);

  tl->freeze();

  readSettings();
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::readSettings()
{
  int i, num, cur;
  QString key, value, curDir;
  KConfig *config = KGlobal::config();

  config->setGroup("Settings");
  mCbxRandom->setChecked(config->readBoolEntry("ShowRandom", true));
  mCbxZoom->setChecked(config->readBoolEntry("ZoomImages", false));
  mCbxShowName->setChecked(config->readBoolEntry("PrintName", true));
  num = config->readNumEntry("Delay", 20);
  mDelay->setValue(num);

  QString str = i18n("Delay: %1 seconds").arg(num);
  mLblDelay->setText(str);

  curDir = config->readEntry("Directory");

  config->setGroup("Slide Show");
  num = config->readNumEntry("Count", 0);
  for (i=0, cur=-1; i<num; i++)
  {
    key = QString::fromLatin1("Dir%1").arg(i);
    value = config->readEntry(key);
    if (!value.isEmpty())
    {
      if (cur < 0 && value == curDir) cur = i;
      mCboDir->insertItem(value);
    }
  }
  if (cur < 0 && !curDir.isEmpty())
  {
    mCboDir->insertItem(curDir);
    mCboDir->setCurrentItem(mCboDir->count());
  }
  else mCboDir->setCurrentItem(cur+1);
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::writeSettings()
{
  int i, num;
  QString key, value;
  KConfig *config = KGlobal::config();

  config->setGroup("Settings");
  config->writeEntry("ShowRandom", mCbxRandom->isChecked());
  config->writeEntry("ZoomImages", mCbxZoom->isChecked());
  config->writeEntry("PrintName",  mCbxShowName->isChecked());
  config->writeEntry("Delay", mDelay->value());

  if (mCboDir->currentItem() >= 1)
    config->writeEntry("Directory", mCboDir->currentText());

  config->setGroup("Slide Show");

  num = mCboDir->count() - 1;
  if (num > 20) num = 20;
  config->writeEntry("Count", num);

  for (i=0; i<num; i++)
  {
    key = QString::fromLatin1("Dir%1").arg(i);
    value = mCboDir->text(i+1);
    config->writeEntry(key, value);
  }

  config->sync();

  if (mSaver)
  {
    mSaver->readConfig();
    mSaver->restart();
  }
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::slotDelay(int x)
{
  writeSettings();

  QString str = i18n("Delay: %1 seconds").arg(x);
  mLblDelay->setText(str);
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::slotDirSelected(int aIdx)
{
  if (aIdx <= 0)
  {
    QString dirName;
    dirName = KFileDialog::getExistingDirectory(QDir::homeDirPath(), this,
		   i18n("Choose Images Directory") );
    if (dirName.isEmpty()) return;
    mCboDir->insertItem(dirName, 1);
    mCboDir->setCurrentItem(1);
  }
  writeSettings();
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::slotOkPressed()
{
  writeSettings();
  accept();
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::slotAbout()
{
  KMessageBox::about(this,
     i18n("SlideShow Version 1.1\n\n"
			"Copyright (c) 1999 by\n"
			"Stefan Taferner <taferner@kde.org>\n"));
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::minSize(QWidget* aWidget)
{
  aWidget->setFixedSize(aWidget->sizeHint());
}

