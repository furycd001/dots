/***************************************************************************
                          keyecandypage.cpp  -  description
                             -------------------
    begin                : Tue May 22 2001
    copyright            : (C) 2001 by Ralf Nolden
    email                : nolden@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qslider.h>
#include <qcolor.h>

#include <ksimpleconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <dcopclient.h>
#include <kipc.h>
#include <kapp.h>
#include <klistview.h>
#include <krun.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <ktrader.h>
#include <kglobalsettings.h>

#include <stdlib.h>

#include <kdebug.h>

#include "klocaleadv.h"

#include "keyecandypage.h"

KEyeCandyPage::KEyeCandyPage(QWidget *parent, const char *name ) : KEyeCandyPageDlg(parent,name) {

  kwinconf = new KConfig("kwinrc", false, true);
  kwineventconf = new KConfig("kwin.eventsrc", false, false);
  kcmdisplayconfig = new KConfig("kcmdisplayrc");
  kickerconf =  new KConfig("kickerrc",false, false);
  konquerorconf =  new KConfig("konquerorrc",false, false);
  konqiconconf = new KConfig("konqiconviewrc",false,false);
  kdesktopconf = new KConfig("kdesktoprc", false, false);
  prevImage=false;
  prevText=false;
  prevOther=false;

  px_eyecandySidebar->setPixmap(locate("data", "kpersonalizer/pics/step3.png"));

  klv_features->addColumn(i18n("Features"));
  klv_features->setFullWidth ();


   // Level 1
  desktop_wallpaper = new QCheckListItem(klv_features, i18n("Desktop wallpaper"),
                                 QCheckListItem::CheckBox);
  desktop_window_effects= new QCheckListItem(klv_features, i18n("Window moving/resizing effects"),
                                 QCheckListItem::CheckBox);
  desktop_window_moving_contents= new QCheckListItem(klv_features, i18n("Display contents in moving/resizing windows"),
                                 QCheckListItem::CheckBox);
  // Level 2
  backgrounds_konqueror= new QCheckListItem(klv_features, i18n("File manager background picture"),
                                 QCheckListItem::CheckBox);
  backgrounds_panel= new QCheckListItem(klv_features, i18n("Panel background picture"),
                                 QCheckListItem::CheckBox);
   // Level 3
  icon_zooming_panel = new QCheckListItem(klv_features, i18n("Panel Icon Zooming"),
                                    QCheckListItem::CheckBox);
  icon_effect_gamma = new QCheckListItem(klv_features, i18n("Icon Highlighting"),
                                    QCheckListItem::CheckBox);


  sound_scheme = new QCheckListItem(klv_features, i18n("Sound theme"),
                             QCheckListItem::CheckBox);

    // Level 4
    ///////////////////////////////////////////////////////////////////////////////////
    /// DEPENDEND ON RESOLUTION; DEFAULT: DON`T USE IN LEVELS
   icon_effect_size_desktop = new QCheckListItem(klv_features, i18n("Large Desktop Icons"),
                                      QCheckListItem::CheckBox);
   icon_effect_size_panel = new QCheckListItem(klv_features, i18n("Large Panel Icons"),
                                      QCheckListItem::CheckBox);
    /// DEPENDEND ON RESOLUTION; DEFAULT: DON`T USE IN LEVELS
    ///////////////////////////////////////////////////////////////////////////////////
    alpha_blending_panel = new QCheckListItem(klv_features, i18n("Smoothed icons (Panel)"),
                                      QCheckListItem::CheckBox);

   // Level 5
  preview_images = new QCheckListItem(klv_features, i18n("Image preview"),
                                    QCheckListItem::CheckBox);
   // Level 6
  animated_combo = new QCheckListItem(klv_features, i18n("Animated comboboxes"),
                               QCheckListItem::CheckBox);
   // Level 7
  antialiasing_fonts = new QCheckListItem(klv_features, i18n("Smoothed fonts "
                                         "(Antialiasing)"), QCheckListItem::CheckBox);
  fading_tooltips = new QCheckListItem(klv_features, i18n("Fading tooltips"), QCheckListItem::CheckBox);
   // Level 8
  alpha_blending_desktop  = new QCheckListItem(klv_features, i18n("Smoothed icons (Desktop)"),
                                    QCheckListItem::CheckBox);
  preview_text = new QCheckListItem(klv_features, i18n("Textfile previews"),
                                    QCheckListItem::CheckBox);
   // Level 9
  fading_menus= new QCheckListItem(klv_features, i18n("Fading menus"),
                                   QCheckListItem::CheckBox);
  preview_other = new QCheckListItem(klv_features, i18n("Other file previews"),
                                    QCheckListItem::CheckBox);

  klv_features->hide();

  getUserDefaults();  // get user's current settings
  setDefaults();          // set the default level 4 on the slider and checkboxes
}

KEyeCandyPage::~KEyeCandyPage(){
}
/** enables/disables the QCheckListItems in the klv_features
according to the level the slider moved. */
void KEyeCandyPage::slotEyeCandySliderMoved(int value){
       // Level 1
       desktop_wallpaper->setOn(false);
       desktop_window_effects->setOn(false);
       desktop_window_moving_contents->setOn(false);
        // Level 2
       backgrounds_konqueror->setOn(false);
       backgrounds_panel->setOn(false);
       // Level 3
       icon_effect_gamma->setOn(false);
       icon_zooming_panel->setOn(false);
       // Level 4
       alpha_blending_panel->setOn(false);
       icon_effect_size_desktop->setOn(false);
       icon_effect_size_panel->setOn(false);

       // Level 5
       preview_images->setOn(false);
       // Level 6
       animated_combo->setOn(false);
       // Level 7
       antialiasing_fonts->setOn(false);
       fading_tooltips->setOn(false);
       // Level 8
       alpha_blending_desktop->setOn(false);
       preview_text->setOn(false);
       // Level 9
       fading_menus->setOn(false);
       preview_other->setOn(false);
       sound_scheme->setOn(false);

   if( value >= 1){
         // Level 1
       desktop_wallpaper->setOn(true);
       desktop_window_effects->setOn(true);
       desktop_window_moving_contents->setOn(true);
   }
   if( value >= 2){
       // Level 2
       backgrounds_konqueror->setOn(true);
       backgrounds_panel->setOn(true);
   }
    if( value >= 3){
       // Level 3
       icon_effect_gamma->setOn(true);
       icon_zooming_panel->setOn(true);
   }
    if( value >= 4){
       // Level 4
       alpha_blending_panel->setOn(true);
       icon_effect_size_desktop->setOn(true);
       if(QApplication::desktop()->width() >=1024){
          icon_effect_size_desktop->setOn(true); // enable 48x48 icons by default if the desktop size is wider than 1024x768
       }
       if(QApplication::desktop()->width() >=1280){
          icon_effect_size_panel->setOn(true);
       }
   }
    if( value >= 5){
        // Level 5
       preview_images->setOn(true);
  }
    if( value >= 6){
        // Level 6
       animated_combo->setOn(true);
  }
    if( value >= 7){
       // Level 7
       antialiasing_fonts->setOn(true);
       fading_tooltips->setOn(true);
   }
    if( value >= 8){
       // Level 8
       alpha_blending_desktop->setOn(true);
       preview_text->setOn(true);
   }
    if( value >= 9){
       // Level 9
       fading_menus->setOn(true);
       preview_other->setOn(true);
       sound_scheme->setOn(true);
       if(QApplication::desktop()->width() >=1024)
        icon_effect_size_desktop->setOn(true); // enable 48x48 icons by default if the desktop size is wider than 1024x768
    }

}


//---------------------------DESKTOP--------------------------------------------------
/** This should be self-explanatory, enabling/disabling the default desktop wallpaper. Level 0 disables,
Level 1 enables this (and all levels above). */
void KEyeCandyPage::enableDesktopWallpaper(bool enable, bool user){
	kdesktopconf->setGroup("Desktop0");

	if( st_UserWallpaper.WallpaperMode == "NoWallpaper")
	deskbgimage="default_blue.jpg";

	if(enable && !user){
		// if the user has a different mode than the default of NoMulti, we don't change anyting on that.
		if( st_UserWallpaper.MultiWallpaperMode == "NoMulti" )
			kdesktopconf->writeEntry("MultiWallpaperMode", "NoMulti");
		// if the wallpaper is the new default one, set mode to scaled to leave user settings untouched
		if( deskbgimage == "default_blue.jpg"){
			kdesktopconf->writeEntry("WallpaperMode", "Scaled");
			//here we change the kdesktop font color to white as it fits better
			// to the default_blue.jpg blue background
			kdesktopconf->setGroup("FMSettings");
			kdesktopconf->writeEntry("NormalTextColor", QColor("#FFFFFF") );
			kdesktopconf->setGroup("Desktop0");
		}
		else{
			kdesktopconf->writeEntry("WallpaperMode", st_UserWallpaper.WallpaperMode );
		}
		// write the bg image name, this is the user's image if he already set that on desktop0
		kdesktopconf->writeEntry("Wallpaper", deskbgimage);
		kdesktopconf->setGroup("Background Common");
		// when the user set his desktop to *not* use common desktop and no wallpaper = he can have set
		// different color schemes for his desktops, we set the common desktop again to set the new
		// default wallpaper on *all* desktops.
		if(!st_UserWallpaper.CommonDesktop && (st_UserWallpaper.WallpaperMode == "NoWallpaper") )
			kdesktopconf->writeEntry("CommonDesktop", true);
		// the user set his desktop *not* to use common desktop, but *has* set a wallpaper = multiple
		// desktops with different wallpapers.
		if(!st_UserWallpaper.CommonDesktop && (!(st_UserWallpaper.WallpaperMode == "NoWallpaper")) )
			kdesktopconf->writeEntry("CommonDesktop", false);
	}
	else{
		kdesktopconf->setGroup("Desktop0");
		kdesktopconf->writeEntry("WallpaperMode", "NoWallpaper");
		kdesktopconf->setGroup("FMSettings");
		kdesktopconf->writeEntry("NormalTextColor", desktopTextColor); //restore the user's color
		kdesktopconf->setGroup("Background Common");
		// only set this to the user's setting. the default is true anyway
		if(st_UserWallpaper.WallpaperMode == "NoWallpaper")
			kdesktopconf->writeEntry("CommonDesktop", st_UserWallpaper.CommonDesktop);
		else
			kdesktopconf->writeEntry("CommonDesktop", true);
	}
	if(user){
		// reset everything
		kdesktopconf->setGroup("Desktop0");
		kdesktopconf->writeEntry("MultiWallpaperMode", st_UserWallpaper.MultiWallpaperMode);
		kdesktopconf->writeEntry("WallpaperMode", st_UserWallpaper.WallpaperMode);
		kdesktopconf->writeEntry("Wallpaper", st_UserWallpaper.Wallpaper);
		kdesktopconf->setGroup("Background Common");
		kdesktopconf->writeEntry("CommonDesktop", st_UserWallpaper.CommonDesktop);
		kdesktopconf->setGroup("FMSettings");
		kdesktopconf->writeEntry("NormalTextColor", desktopTextColor); //restore the user's color
	}
}

/** this function enables/disables the window effects for Shading, Minimize and Restore. The contents in moving/resized windows is set in enableWindowContens(bool ) */
void KEyeCandyPage::enableDesktopWindowEffects(bool enable,bool restore){
// see /kdebase/kcontrol/kwm module, KAdvancedConfig class. Used are:
// -Animate minimize and restore
// -Animate shade
// -Enable Hover
//-Enable move/resize on maximised windows
  kwinconf->setGroup( "Windows" );
  if(!restore){
    kwinconf->writeEntry("AnimateMinimize", enable );
    kwinconf->writeEntry("AnimateShade", enable );
    kwinconf->writeEntry("MoveResizeMaximizedWindows",enable);
    kwinconf->writeEntry("ShadeHover", enable );
  }
  else{
    kwinconf->writeEntry("AnimateMinimize", b_AnimateMinimize );
    kwinconf->writeEntry("AnimateShade", b_AnimateShade );
    kwinconf->writeEntry("MoveResizeMaximizedWindows",b_MoveResizeMaximizedWindows);
    kwinconf->writeEntry("ShadeHover", b_ShadeHover);
  }
}
/** enable/disable window moving with contents shown */
void KEyeCandyPage::enableDesktopWindowMovingContents(bool enable, bool restore){
// see /kdebase/kcontrol/kwm module, KAdvancedConfig class. Used are:
// -Display content in moving window
// -Display content in resizing window

    kwinconf->setGroup( "Windows" );
    if (enable){
        kwinconf->writeEntry("ResizeMode","Opaque");
        kwinconf->writeEntry("MoveMode","Opaque");
    }
    else{
        kwinconf->writeEntry("ResizeMode","Transparent");
        kwinconf->writeEntry("MoveMode","Transparent");
    }
    if(restore){
        kwinconf->writeEntry("ResizeMode",s_ResizeMode);
        kwinconf->writeEntry("MoveMode",s_MoveMode);
    }

}
//---------------------------DESKTOP--------------------------------------------------



//---------------------------ALPHA-BLENDING--------------------------------------------------
/** enable alpha blending for kicker icons in Level 4.  */
void KEyeCandyPage::enableAlphaBlendingPanel(bool enable){

  KGlobal::config()->setGroup("PanelIcons");
  KGlobal::config()->writeEntry("AlphaBlending", enable, true, true);

}
/** Enables desktop alpha blending for icons.  Enable this in Level 8 */
void KEyeCandyPage::enableAlphaBlendingDesktop(bool enable){

  KGlobal::config()->setGroup("DesktopIcons");
  KGlobal::config()->writeEntry("AlphaBlending", enable, true, true);

}
//---------------------------ALPHA-BLENDING--------------------------------------------------



//---------------------------BACKGROUNDS--------------------------------------------------
/** Here, the background tiles/wallpapers for Konqueror and Kicker are set to the default values. Enabled in Level 2. */
void KEyeCandyPage::enableBackgroundsPanel(bool enable){



// TODO: come up with a good background
}
/** Here, the background tiles/wallpapers for Konqueror and Kicker are set to the default values. Enabled in Level 2. */
void KEyeCandyPage::enableBackgroundsKonqueror(bool enable){
  konquerorconf->setGroup("Settings");
  if(enable){
      if(konqbgimage.isEmpty())
        konqbgimage="kenwimer.png";
      konquerorconf->writeEntry("BgImage", konqbgimage);
  }
  else
      konquerorconf->writeEntry("BgImage", "");
}
//----------------------------BACKGROUNDS-------------------------------------------------




//----------------------------ICON STUFF-------------------------------------------------
/** Level 0-2 disable this, Level 3 and above enable this.  */
void KEyeCandyPage::enableIconZoomingPanel(bool enable){
// Kicker Icon zooming feature. See /kdebase/kcontrol/kicker, LookAndFeelTab
    kickerconf->setGroup("buttons");
    kickerconf->writeEntry("EnableIconZoom", enable);
}
/** enable Icon highlighting,  Level 3 */
void KEyeCandyPage::enableIconEffectGamma(bool enable, bool user){
  if(enable){
    KGlobal::config()->setGroup("DesktopIcons");
    KGlobal::config()->writeEntry("ActiveEffect", "togamma", true, true);
    KGlobal::config()->writeEntry("ActiveValue", "0.7", true, true);
    KGlobal::config()->setGroup("PanelIcons");
    KGlobal::config()->writeEntry("ActiveEffect", "togamma", true, true);
    KGlobal::config()->writeEntry("ActiveValue", "0.7", true, true);
  }
  else{
    if(user){
      KGlobal::config()->setGroup("DesktopIcons");
      KGlobal::config()->writeEntry("ActiveEffect", st_UserGamma.EffectDesktop, true, true);
      KGlobal::config()->writeEntry("ActiveValue", st_UserGamma.ValueDesktop, true, true);
      KGlobal::config()->setGroup("PanelIcons");
      KGlobal::config()->writeEntry("ActiveEffect", st_UserGamma.EffectPanel, true, true);
      KGlobal::config()->writeEntry("ActiveValue", st_UserGamma.ValuePanel, true, true);
    }
    else{
      KGlobal::config()->setGroup("DesktopIcons");
      KGlobal::config()->writeEntry("ActiveEffect", "none", true, true);
      KGlobal::config()->setGroup("PanelIcons");
      KGlobal::config()->writeEntry("ActiveEffect", "none", true, true);
    }
  }
}

/** No descriptions */
void KEyeCandyPage::enableIconEffectSizePanel(bool enable){
  QByteArray data;
  QDataStream stream( data, IO_WriteOnly );

  if(enable)
    stream << 3;
  else
    stream << panelsize;

  kapp->dcopClient()->send( "kicker", "Panel", "setPanelSize(int)",data);
}
/** No descriptions */
void KEyeCandyPage::enableIconEffectSizeDesktop(bool enable){

  if( enable )  // use 48x48 icons
  {
    KGlobal::config()->setGroup("DesktopIcons");
    KGlobal::config()->writeEntry("Size", 48, true, true);
  }
  else{
    KGlobal::config()->setGroup("DesktopIcons");
    KGlobal::config()->writeEntry("Size", desktopiconsize, true, true);
  }
}
//----------------------------ICON STUFF-------------------------------------------------


//----------------------------STYLE EFFECTS-------------------------------------------------
/** Enable fading tooltips in Level 7 */
void KEyeCandyPage::enableFadingToolTips(bool enable){

    kcmdisplayconfig->setGroup("KDE");
    kcmdisplayconfig->writeEntry( "EffectFadeTooltip", enable,true, true );

}
/** enables/disables fading menus which are off by default in KDE. Enable this in Level 9 */
void KEyeCandyPage::enableFadingMenus(bool enable){

    kcmdisplayconfig->setGroup("KDE");
    kcmdisplayconfig->writeEntry("EffectFadeMenu", enable,true, true);

}
/** Enable animated combo boxes, see styles kcontrol module. Enable in Level 6 (disabled by default anyway, so doesn't need to be
disabled in levels below 4) */
void KEyeCandyPage::enableAnimatedCombo(bool enable){

    kcmdisplayconfig->setGroup("KDE");
    kcmdisplayconfig->writeEntry("EffectAnimateCombo", enable, true, true);

}
//----------------------------STYLE EFFECTS-------------------------------------------------


//----------------------------PREVIEWS-------------------------------------------------
/** enables desktop/konqueror image previews, level 5 */
void KEyeCandyPage::enablePreviewImages(bool enable){
  prevImage=enable;
}
/** enables text preview in konq/kdesktop. Enable in Level 8 */
void KEyeCandyPage::enablePreviewText(bool enable){
  prevText=enable;
}
/** enables all other file previews that are available besides text and image preview. Enable in Level 9. */
void KEyeCandyPage::enablePreviewOther(bool enable){
  prevOther=enable;
}
/** as the preview entries in the rc files (konqiconviewrc and kdesktoprc) are a string list and
this list would be overwritten by the three different possibilities, we just set bool values
and ask them here, set the according string list here. */
void KEyeCandyPage::enablePreview(bool currSettings){
  QStringList previews;

  if(prevOther){
    KTrader::OfferList plugins = KTrader::self()->query("ThumbCreator");
    for (KTrader::OfferList::ConstIterator it = plugins.begin(); it != plugins.end(); ++it)
    {
          previews.append((*it)->desktopEntryName());
          kdDebug() << "Adding Preview:" << (*it)->desktopEntryName() << endl;
    }
  }
  if(prevImage)
    previews.append("imagethumbnail");
  if(prevText)
    previews.append("textthumbnail");
  if(prevOther){  // remove text/image if not checked
    if(!prevImage)
      previews.remove("imagethumbnail");
    if(!prevText)
      previews.remove("textthumbnail");
  }
  if(prevOther)
    previews.append("audio/");
  kdesktopconf->setGroup("Desktop Icons");
  kdesktopconf->writeEntry("Preview", currSettings ? previews: kdesktop_prev);

  konqiconconf->setGroup("Settings");
  konqiconconf->writeEntry("Preview", currSettings ? previews:konq_prev);
}
//----------------------------PREVIEWS-------------------------------------------------



//----------------------------OTHER STUFF-------------------------------------------------
/** Enables the default KDE sound scheme in Level 3 */
void KEyeCandyPage::enableSoundScheme(bool enable){
    kwineventconf->setGroup("desktop1");
    kwineventconf->writeEntry("presentation",  enable ? 1:0 );
    kwineventconf->setGroup("desktop2");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("desktop3");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("desktop4");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("desktop5");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("desktop6");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("desktop7");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("desktop8");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );

    kwineventconf->setGroup("new");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("close");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );

    kwineventconf->setGroup("transnew");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("transdelete");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );

    kwineventconf->setGroup("iconify");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("deiconify");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("maximize");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("unmaximize");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("shadeup");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("shadedown");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("sticky");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
    kwineventconf->setGroup("unsticky");
    kwineventconf->writeEntry("presentation", enable ? 1:0 );
}
/** Enable Antialiased fonts. Maybe a check if the chard can do this with xdpyinfo | grep RENDER here would bring up if
the user can do this or not. Enable in Level 7. */
void KEyeCandyPage::enableAntialiasingFonts(bool enable){
    KGlobal::config()->setGroup("KDE");
    KGlobal::config()->writeEntry( "AntiAliasing", enable,true, true);
}
//----------------------------OTHER STUFF-------------------------------------------------





/** save funtion to enable/disable the according settings that are made in the QCheckListItems of the
Eyecandy page. */
void KEyeCandyPage::save(bool currSettings){
    kdDebug() << "KEyeCandyPage::save()" << endl;
  // save like we want. Just set the checkboxes matching to either how they are set in the dialog (currSettings=true, default)
  // or, if false, take the settings we got in getUserDefaults()
  saveCheckState(currSettings);
///////////////////////////////////////////
/// restart kwin  for window effects
  kwineventconf->sync();
  kapp->dcopClient()->send("knotify", "Notify", "reconfigure()", "");
  kwinconf->sync();
  kapp->dcopClient()->send("kwin*", "", "reconfigure()", "");
///////////////////////////////////////////


///////////////////////////////////////////
// set the display options (style effects)
  kcmdisplayconfig->sync();
  KIPC::sendMessageAll(KIPC::SettingsChanged);
  QApplication::syncX();
///////////////////////////////////////////

///////////////////////////////////////////
// kicker stuff: Iconzooming etc.
  kickerconf->sync();
  kapp->dcopClient()->send( "kicker", "Panel", "configure()", "" );
///////////////////////////////////////////


///////////////////////////////////////////
// Icon stuff: Alphablending etc.
  KGlobal::config()->sync();
    for (int i=0; i<KIcon::LastGroup; i++)
    {
	    KIPC::sendMessageAll(KIPC::IconChanged, i);
    }
///////////////////////////////////////////

///////////////////////////////////////////
// konquerorconfig for background image. restarting konqueror not needed, no konqueror started yet
// preview stuff
  konquerorconf->sync();  // background image saving
  konqiconconf->sync();
  kdesktopconf->sync();
  // unfortunately, the konqiconview does not re-read the configuration to restructure the previews and the background picture
  kapp->dcopClient()->send( "konqueror*", "KonquerorIface", "reparseConfiguration()", "" );
  kapp->dcopClient()->send( "kdesktop", "KDesktopIface", "configure()", "" );
  kapp->dcopClient()->send( "kdesktop", "KBackgroundIface", "configure()", "" );
  kapp->dcopClient()->send( "kdesktop", "KDesktopIface", "lineupIcons()", "" ); //lin

////////////////////////////////////////////

}

/** No descriptions */
void KEyeCandyPage::slotEyeCandyShowDetails(bool details){
 if(details){
    klv_features->show();
  }
  else{
    klv_features->hide();
  }
}

/** sets the slider to the default value of Level 4 (KDE Default) and the checklistboxes on
that belong to this level */
void KEyeCandyPage::setDefaults(){

  // reset slider
  sld_effects->setValue( 4 );

  prevImage=false;
  prevText=false;
  prevOther=false;

  // reset checkboxes
  desktop_wallpaper->setOn(true);
  desktop_window_effects->setOn(true);
  desktop_window_moving_contents->setOn(true);
  // Level 2
  backgrounds_konqueror->setOn(true);
  backgrounds_panel->setOn(true);
  // Level 3
  icon_effect_gamma->setOn(true);
  icon_zooming_panel->setOn(true);

  // Level 4
  alpha_blending_panel->setOn(true);
  if(QApplication::desktop()->width() >=1024){
     icon_effect_size_desktop->setOn(true); // enable 48x48 icons by default if the desktop size is wider than 1024x768
  }
  if(QApplication::desktop()->width() >=1280){
     icon_effect_size_panel->setOn(true);
  }
}

/** retrieves the user's local values. In case he doesn't have these set, use the default values of KDE, level 4. */
void KEyeCandyPage::getUserDefaults(){
//
  QByteArray replydata;
  QByteArray data;
  QCString replytype;
  kapp->dcopClient()->call( "kicker", "Panel", "panelSize()",data, replytype, replydata);
  QDataStream stream( replydata, IO_ReadOnly );
  stream >> panelsize;

//  , b_AnimateMinimize, b_AnimateShade, b_MoveResizeMaximizedWindows, b_ShadeHover,

    // Wallpaper-User-Defaults
    kdesktopconf->setGroup("FMSettings");
    QColor tempcolor=KGlobalSettings::textColor();
    desktopTextColor = kdesktopconf->readColorEntry("NormalTextColor", &tempcolor );
    kdesktopconf->setGroup("Background Common");
    st_UserWallpaper.CommonDesktop = kdesktopconf->readBoolEntry("CommonDesktop", true);
    kdesktopconf->setGroup("Desktop0"); // we only need to set one desktop
    st_UserWallpaper.MultiWallpaperMode = kdesktopconf->readEntry("MultiWallpaperMode", "NoMulti");
    st_UserWallpaper.WallpaperMode = kdesktopconf->readEntry("WallpaperMode", "Scaled");
    st_UserWallpaper.Wallpaper = kdesktopconf->readEntry("Wallpaper", "NoWallpaper");
    deskbgimage = kdesktopconf->readEntry("Wallpaper", "default_blue.jpg");
    // Wallpaper-User-Defaults (END)
    KGlobal::config()->setGroup("KDE");
    b_AntiAliasing=KGlobal::config()->readBoolEntry( "AntiAliasing", false);
    KGlobal::config()->setGroup("PanelIcons");
    b_AlphaBlendingDesktop=KGlobal::config()->readBoolEntry("AlphaBlending", false);
    st_UserGamma.EffectPanel=KGlobal::config()->readEntry("ActiveEffect", "none");
    st_UserGamma.ValuePanel=KGlobal::config()->readEntry("ActiveValue", "0.7");
    KGlobal::config()->setGroup("DesktopIcons");
    b_AlphaBlendingPanel=KGlobal::config()->readBoolEntry("AlphaBlending", false);
    st_UserGamma.EffectDesktop=KGlobal::config()->readEntry("ActiveEffect", "none");
    st_UserGamma.ValueDesktop=KGlobal::config()->readEntry("ActiveValue", "0.7");
    desktopiconsize=KGlobal::config()->readNumEntry("Size", 32);
    kcmdisplayconfig->setGroup("KDE");
    b_EffectFadeTooltip=kcmdisplayconfig->readBoolEntry( "EffectFadeTooltip", false );

    kcmdisplayconfig->setGroup("KDE");
    b_EffectFadeMenu=kcmdisplayconfig->readBoolEntry("EffectFadeMenu", false);

    kcmdisplayconfig->setGroup("KDE");
    b_EffectAnimateCombo=kcmdisplayconfig->readBoolEntry("EffectAnimateCombo", false);

    kickerconf->setGroup("buttons");
    b_EnableIconZoom=kickerconf->readBoolEntry("EnableIconZoom", true);

    konquerorconf->setGroup("Settings");
    konqbgimage=konquerorconf->readEntry("BgImage", "");

    kdesktopconf->setGroup("Desktop Icons");
    kdesktop_prev=kdesktopconf->readListEntry("Preview");
    konqiconconf->setGroup("Settings");
    konq_prev=konqiconconf->readListEntry("Preview");
    kwinconf->setGroup( "Windows" );
    s_ResizeMode=kwinconf->readEntry("ResizeMode", "Transparent");
    s_MoveMode=kwinconf->readEntry("MoveMode", "Opaque");

    b_AnimateMinimize=kwinconf->readBoolEntry("AnimateMinimize", true );
    b_AnimateShade=kwinconf->readBoolEntry("AnimateShade", true );
    b_MoveResizeMaximizedWindows=kwinconf->readBoolEntry("MoveResizeMaximizedWindows",true);
    b_ShadeHover = kwinconf->readBoolEntry("ShadeHover", false);
}
/** calls all enable functions with the state of the checkboxes. This is needed for save() only,
as in case the user quits, we have to set these states again in saveUserDefaults to what they were
prior to running kpersonalizer */
void KEyeCandyPage::saveCheckState(bool currSettings){ // currSettings= true -> take the checkboxes, otherwise take user values set
  if(currSettings){
      enableDesktopWallpaper(desktop_wallpaper->isOn());
      enableDesktopWindowEffects(desktop_window_effects->isOn(), false);
      enableDesktopWindowMovingContents(desktop_window_moving_contents->isOn(), false);

      enableAlphaBlendingPanel(alpha_blending_panel->isOn());
      enableAlphaBlendingDesktop(alpha_blending_desktop->isOn());

      enableBackgroundsPanel(backgrounds_panel->isOn());
      enableBackgroundsKonqueror(backgrounds_konqueror->isOn());

      enableIconZoomingPanel(icon_zooming_panel->isOn());
      enableIconEffectGamma(icon_effect_gamma->isOn(), false);
      enableIconEffectSizePanel(icon_effect_size_panel->isOn());
      enableIconEffectSizeDesktop(icon_effect_size_desktop->isOn());

      enableFadingToolTips(fading_tooltips->isOn());
      enableFadingMenus(fading_menus->isOn());

      enableAnimatedCombo(animated_combo->isOn());

      enablePreviewImages(preview_images->isOn());
      enablePreviewText(preview_text->isOn());
      enablePreviewOther(preview_other->isOn());
      enablePreview(true);

      enableSoundScheme(sound_scheme->isOn());

      enableAntialiasingFonts(antialiasing_fonts->isOn());
  }

//b_AnimateMinimize, b_AnimateShade, b_MoveResizeMaximizedWindows, b_ShadeHover,
   else{ //  user's settings
     // restore functions
      enableDesktopWallpaper(false, true);
      enableAlphaBlendingPanel(b_AlphaBlendingPanel);
      enableAlphaBlendingDesktop(b_AlphaBlendingDesktop);
      enableBackgroundsKonqueror(konqbgimage.isEmpty());  // if empty
      enableIconZoomingPanel(b_EnableIconZoom);
      enableFadingToolTips(b_EffectFadeTooltip);
      enableFadingMenus(b_EffectFadeMenu);
      enableAnimatedCombo(b_EffectAnimateCombo);
      enablePreview(false);
      enableAntialiasingFonts(b_AntiAliasing);
      enableDesktopWindowMovingContents(false, true);  // the first parameter is equal to this call
      enableDesktopWindowEffects(false, true);
      enableIconEffectSizePanel(false);
      enableIconEffectSizeDesktop(false);
      enableIconEffectGamma(false, true);

      // misses functionality at all
      enableBackgroundsPanel(false);

      // NOT YET DONE
      enableSoundScheme(false);


  }
}
#include "keyecandypage.moc"
