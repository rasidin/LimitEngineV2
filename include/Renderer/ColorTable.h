/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_ColorTable.h
 @brief Color Table for form
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2013/09/02 Created by minseob
 ***********************************************************/

#ifndef _LE_COLORTABLE_H_
#define _LE_COLORTABLE_H_

#include "LE_CColorRGBA.h"

#define COLORTABLE_MAINMENU_TOP cColorRGBA(uint8(140), uint8(140), uint8(140), uint8(255))
#define COLORTABLE_MAINMENU_BOTTOM cColorRGBA(uint8(100), uint8(100), uint8(100), uint8(255))
#define COLORTABLE_MAINMENU COLORTABLE_MAINMENU_TOP, COLORTABLE_MAINMENU_TOP, COLORTABLE_MAINMENU_BOTTOM, COLORTABLE_MAINMENU_BOTTOM

#endif // _LE_COLORTABLE_H_