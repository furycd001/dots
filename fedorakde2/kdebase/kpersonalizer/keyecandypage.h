/***************************************************************************
                          keyecandypage.h  -  description
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

#ifndef KEYECANDYPAGE_H
#define KEYECANDYPAGE_H

#include "keyecandypagedlg.h"

class QCheckListItem;
class QColor;
/**Abstract class for the eyecandy page. Applies  the accoring eyecandy settings
  *@author Ralf Nolden
  */

class KEyeCandyPage : public KEyeCandyPageDlg  {
   Q_OBJECT
public: 
	KEyeCandyPage(QWidget *parent=0, const char *name=0);
	~KEyeCandyPage();

//---------------------------------------------------------------------------------------------------------
  /** This should be self-explanatory, enabling/disabling the default desktop wallpaper. Level 0 disables,
Level 1 enables this (and all levels above). */
  void enableDesktopWallpaper(bool enable, bool user=false);
  /** this function enables/disables the window effects for Shading, Minimize and Restore. The contents in moving/resized windows is set in enableWindowContens(bool ) */
  void enableDesktopWindowEffects(bool enable, bool restore= false);
  /** enable/disable window moving with contents shown */
  void enableDesktopWindowMovingContents( bool enable,bool restore= false);
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
  /** enable alpha blending for kicker icons in Level 4.  */
  void enableAlphaBlendingPanel(bool enable);
  /** Enables desktop alpha blending for icons.  Enable this in Level 8 */
  void enableAlphaBlendingDesktop(bool enable);
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
  /** Level 0-2 disable this, Level 3 and above enable this.  */
  void enableIconZoomingPanel(bool enable);
  /** enable Icon highlighting,  Level 3 */
  void enableIconEffectGamma(bool enable, bool user);
  /** No descriptions */
  void enableIconEffectSizeDesktop(bool enable);
  /** No descriptions */
  void enableIconEffectSizePanel(bool enable);
//---------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------
  /** Here, the background tiles/wallpapers for Kicker are set to the default values. Enabled in Level 2. */
  void enableBackgroundsPanel(bool enable);
  /** Here, the background tiles/wallpapers for Konqueror are set to the default values. Enabled in Level 2. */
  void enableBackgroundsKonqueror(bool enable);
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
  /** enables all other file previews that are available besides text and image preview. Enable in Level 9. */
  void enablePreviewOther(bool enable);
  /** enables text preview in konq/kdesktop. Enable in Level 8 */
  void enablePreviewText(bool enable);
  /** enables desktop/konqueror image previews, level 5 */
  void enablePreviewImages(bool enable);
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
  /** Enable fading tooltips in Level 7 */
  void enableFadingToolTips(bool enable);
  /** enables/disables fading menus which are off by default in KDE. Enable this in Level 9 */
  void enableFadingMenus(bool enable);
  /** Enable animated combo boxes, see styles kcontrol module. Enable in Level 4 (disabled by default anyway, so doesn't need to be
        disabled in levels below 4) */
  void enableAnimatedCombo(bool enable);
//---------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------
  /** Enable Antialiased fonts. Maybe a check if the chard can do this with xdpyinfo | grep RENDER here would bring up if
        the user can do this or not. Enable in Level 7. */
  void enableAntialiasingFonts(bool enable);
  /** Enables the default KDE sound scheme in Level 3 */
  void enableSoundScheme(bool enable);
//---------------------------------------------------------------------------------------------------------


  /** save funtion to enable/disable the according settings that are made in the QCheckListItems of the
Eyecandy page by default. If currSettings is false, the user's default settings will be restored*/
  void save(bool currSettings=true);
  /** sets the slider to the default value of Level 4 (KDE Default) and the checklistboxes on 
that belong to this level */
  void setDefaults();
  /** as the preview entries in the rc files (konqiconviewrc and kdesktoprc) are a string list and
this list would be overwritten by the three different possibilities, we just set bool values 
and ask them here, set the according string list here.  If currSettings is true, take the chosen ones, else take the user's ones*/
  void enablePreview(bool currSettings);
  /** calls all enable functions with the state of the checkboxes. This is needed for save() only,
as in case the user quits, we have to set these states again in saveUserDefaults to what they were
prior to running kpersonalizer */
  void saveCheckState(bool currSettings);
  /** retrieves the user's local values. In case he doesn't have these set, use the default values of KDE, level 4. */
  void getUserDefaults();
  /** Set back the user settings for IconEffect gamma */

  public slots:
  /** enables/disables the QCheckListItems in the klv_features
according to the level the slider moved. */
  void slotEyeCandySliderMoved(int value);
  /** No descriptions */
  void slotEyeCandyShowDetails(bool details);

  private:
  // DEFAULT VALUES SET BY USER
  int panelsize; // kicker panelsize 0,1,2,3 before the big icons are set to reset that
  int desktopiconsize;
  bool b_EffectFadeMenu, b_EffectAnimateCombo, b_EffectFadeTooltip, b_EnableIconZoom, b_AlphaBlendingPanel,
  b_AlphaBlendingDesktop, b_AnimateMinimize, b_AnimateShade, b_MoveResizeMaximizedWindows,
  b_ShadeHover, b_AntiAliasing;

  QString konqbgimage, s_ResizeMode, s_MoveMode, deskbgimage;
  QStringList konq_prev, kdesktop_prev;
  QColor desktopTextColor;

	struct st_Gamma{
		QString EffectDesktop;
		QString EffectPanel;
		QString ValueDesktop;
		QString ValuePanel;
	} st_UserGamma;
  struct st_Wallpaper{
		bool CommonDesktop;
		QString MultiWallpaperMode;
		QString WallpaperMode;
		QString Wallpaper;
	} st_UserWallpaper;
  // DEFAULT VALLUES SET BY USER (END)

  KConfig* kwinconf;
  KConfig* kwineventconf;
  KConfig* kcmdisplayconfig;
  KConfig* kickerconf;
  KConfig* konquerorconf;
  KConfig* konqiconconf;
  KConfig* kdesktopconf;

  bool prevImage, prevText, prevOther;

  QCheckListItem* alpha_blending_desktop;
  QCheckListItem* alpha_blending_panel;

  QCheckListItem* animated_combo;

  QCheckListItem* antialiasing_fonts;

  QCheckListItem* backgrounds_konqueror;
  QCheckListItem* backgrounds_panel;

  QCheckListItem* desktop_wallpaper;
  QCheckListItem* desktop_window_effects;
  QCheckListItem* desktop_window_moving_contents;

  QCheckListItem* icon_effect_gamma;
  QCheckListItem* icon_effect_size_desktop;
  QCheckListItem* icon_effect_size_panel;

  QCheckListItem* icon_zooming_panel;

  QCheckListItem* fading_menus;
  QCheckListItem* fading_tooltips;

  QCheckListItem* preview_text;
  QCheckListItem* preview_images;
  QCheckListItem* preview_other;

  QCheckListItem* sound_scheme;

};

#endif
