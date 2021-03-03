/***************************************************************************
                          mimehdrline.cc  -  description
                             -------------------
    begin                : Wed Oct 11 2000
    copyright            : (C) 2000 by Sven Carstens
    email                : s.carstens@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <time.h>
#include <iostream>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include <qtextcodec.h>

#include <config.h>
#include "mimehdrline.h"
#include "rfcdecoder.h"
using namespace std;


const char *wdays[] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

const char *months[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

mimeHdrLine::mimeHdrLine ():
mimeValue ((const char *) NULL), mimeLabel ((const char *) NULL)
{
}

mimeHdrLine::mimeHdrLine (const QCString & aLabel, const QCString & aValue):
mimeValue (aValue),
mimeLabel (aLabel)
{
}

mimeHdrLine::mimeHdrLine (mimeHdrLine * aHdrLine):
mimeValue (aHdrLine->mimeValue), mimeLabel (aHdrLine->mimeLabel)
{
}

mimeHdrLine::~mimeHdrLine ()
{
}

int
mimeHdrLine::appendStr (const char *inCStr)
{
  int retVal = 0;
  int skip;
  char *aCStr = (char *) inCStr;

  if (aCStr)
  {
    skip = skipWS (aCStr);
    if (skip && !mimeLabel.isEmpty ())
    {
      if (skip > 0)
      {
        mimeValue += QCString (aCStr, skip + 1);
        aCStr += skip;
        retVal += skip;
        skip = parseFullLine (aCStr);
        mimeValue += QCString (aCStr, skip + 1);
        retVal += skip;
        aCStr += skip;
      }
    }
    else
    {
      if (mimeLabel.isEmpty ())
        return setStr (aCStr);
    }
  }
  return retVal;
}

/** parse a Line into the class
move input ptr accordingly
and report characters slurped */
int
mimeHdrLine::setStr (const char *inCStr)
{
  int retVal = 0;
  char *aCStr = (char *) inCStr;
//  char *begin = aCStr;  
  mimeLabel = QCString ((const char *) NULL);
  mimeValue = QCString ((const char *) NULL);

  if (aCStr)
  {
    // can't have spaces on normal lines
    if (!skipWS (aCStr))
    {
      int label = 0, advance;
      while ((advance = parseWord (&aCStr[label])))
      {
        label += advance;
      }
      if (label && aCStr[label - 1] != ':')
        retVal = 0;
      else
      {
        mimeLabel = QCString (aCStr, label);  //length including zero
        retVal += label;
        aCStr += label;
      }
    }
    if (retVal)
    {
      int skip;
      skip = skipWS (aCStr);
      if (skip < 0)
        skip *= -1;
      aCStr += skip;
      retVal += skip;
      skip = parseFullLine (aCStr);
      mimeValue = QCString (aCStr, skip + 1);
      retVal += skip;
      aCStr += skip;
    }
    else
    {
      //Skip malformed line
      while (*aCStr && *aCStr != '\r' && *aCStr != '\n')
      {
        retVal--;
        aCStr++;
      }
      if (*aCStr == '\r')
      {
        retVal--;
        aCStr++;
      }
      if (*aCStr == '\n')
      {
        retVal--;
        aCStr++;
      }
    }
  }
  else
  {
    //debug
  }
  return retVal;
}

/** slurp one word*/
int
mimeHdrLine::parseWord (const char *inCStr)
{
  int retVal = 0;
  char *aCStr = (char *) inCStr;

  if (aCStr && *aCStr)
  {
    if (*aCStr == '"')
      return mimeHdrLine::parseQuoted ('"', '"', aCStr);
    else
      return mimeHdrLine::parseHalfWord (aCStr);
  }
  else
  {
    //debug();
  }
  return retVal;
}

/** slurp one word*/
int
mimeHdrLine::parseQuoted (char startQuote, char endQuote, const char *inCStr)
{
  char *aCStr = (char *) inCStr;
  int retVal = 0;

  if (aCStr && *aCStr)
  {
    if (*aCStr == startQuote)
    {
      aCStr++;
      retVal++;
    }
    else
      return 0;
    while (*aCStr && *aCStr != endQuote)
    {
      //skip over backticks
      if (*aCStr == '\\')
      {
        aCStr++;
        retVal++;
      }
      //eat this
      aCStr++;
      retVal++;
    }
    if (*aCStr == endQuote)
    {
      aCStr++;
      retVal++;
    }
  }
  else
  {
    //debug();
  }
  return retVal;
}

/** slurp one alphanumerical word without continuation*/
int
mimeHdrLine::parseAlphaNum (const char *inCStr)
{
  int retVal = 0;
  char *aCStr = (char *) inCStr;

  if (aCStr)
  {
    while (*aCStr && isalnum (*aCStr))
    {
      //skip over backticks
      if (*aCStr == '\\')
      {
        aCStr++;
        retVal++;
      }
      //eat this
      aCStr++;
      retVal++;
    }
  }
  else
  {
    //debug();
  }
  return retVal;
}

int
mimeHdrLine::parseHalfWord (const char *inCStr)
{
  int retVal = 0;
  char *aCStr = (char *) inCStr;

  if (aCStr && *aCStr)
  {
    if (isalnum (*aCStr))
      return mimeHdrLine::parseAlphaNum (aCStr);
    //skip over backticks
    if (*aCStr == '\\')
    {
      aCStr++;
      retVal++;
    }
    else if (!isspace (*aCStr))
    {
      //eat this
      aCStr++;
      retVal++;
    }
  }
  else
  {
    //debug();
  }
  return retVal;
}

/** slurp one line without continuation*/
int
mimeHdrLine::parseHalfLine (const char *inCStr)
{
  int retVal = 0;
  char *aCStr = (char *) inCStr;

  if (aCStr)
  {
    while (*aCStr && *aCStr != '\n')
    {
      //skip over backticks
      if (*aCStr == '\\')
      {
        aCStr++;
        retVal++;
      }
      //eat this
      aCStr++;
      retVal++;
    }
    if (*aCStr == '\n')
    {
      aCStr++;
      retVal++;
    }
  }
  else
  {
    //debug();
  }
  return retVal;
}

/** skip all white space characters including continuation*/
int
mimeHdrLine::skipWS (const char *inCStr)
{
  int retVal = 0;
  char *aCStr = (char *) inCStr;

  if (aCStr && *aCStr)
  {
    while (*aCStr == ' ' || *aCStr == '\t')
    {
      aCStr++;
      retVal++;
    }
    //check out for continuation lines
    if (*aCStr == '\r')
    {
      aCStr++;
      retVal++;
    }
    if (*aCStr++ == '\n')
      if (*aCStr == '\t' || *aCStr == ' ')
      {
        int skip = mimeHdrLine::skipWS (aCStr);
        if (skip < 0)
          skip *= -1;
        retVal += 1 + skip;
      }
      else
      {
        retVal = -retVal - 1;
      }
  }
  else
  {
    //debug();
  }
  return retVal;
}

/** parses continuated lines */
int
mimeHdrLine::parseFullLine (const char *inCStr)
{
  int retVal = 0;
  char *aCStr = (char *) inCStr;
  int skip;

  if (aCStr)
  {
    //skip leading white space
    skip = skipWS (aCStr);
    if (skip > 0)
    {
      aCStr += skip;
      retVal += skip;
    }
    while (*aCStr)
    {
      int advance;

      if ((advance = parseHalfLine (aCStr)))
      {
        retVal += advance;
        aCStr += advance;
      }
      else if ((advance = skipWS (aCStr)))
      {
        if (advance > 0)
        {
          retVal += advance;
          aCStr += advance;
        }
        else
        {
          retVal -= advance;
          break;
        }
      }
      else
        break;
    }
  }
  else
  {
    //debug();
  }
  return retVal;
}

/** parses continuated lines */
int
mimeHdrLine::parseSeparator (char separator, const char *inCStr)
{
  char *aCStr = (char *) inCStr;
  int retVal = 0;
  int skip;

  if (aCStr)
  {
    //skip leading white space
    skip = skipWS (aCStr);
    if (skip > 0)
    {
      aCStr += skip;
      retVal += skip;
    }
    while (*aCStr)
    {
      int advance;

      if (*aCStr != separator)
      {
        if ((advance = mimeHdrLine::parseWord (aCStr)))
        {
          retVal += advance;
          aCStr += advance;
        }
        else if ((advance = mimeHdrLine::skipWS (aCStr)))
        {
          if (advance > 0)
          {
            retVal += advance;
            aCStr += advance;
          }
          else
          {
            retVal -= advance;
            break;
          }
        }
        else
          break;
      }
      else
      {
        //include separator in result
        retVal++;
        aCStr++;
        break;
      }
    }
  }
  else
  {
    //debug();
  }
  return retVal;
}

QCString
mimeHdrLine::getDateStr (struct tm * timeStruct, int gmt_off)
{
  char retVal[256];
  int off_sec = 0;
  int off_min = 0;
  int off_hour = 0;
  char sign = '+';

  if (gmt_off)
  {
    if (gmt_off < 0)
      sign = '-';
    off_sec = gmt_off % 60;
    gmt_off = gmt_off / 60;
    off_min = gmt_off % 60;
    gmt_off = gmt_off / 60;
    off_hour = abs (gmt_off);
  }
  else
    sign = 0x00;

  if (timeStruct)
  {
    snprintf (retVal, 255, "%s, %2d %s %d %d%d:%d%d:%d%d %c%2.2d%2.2d",
              wdays[timeStruct->tm_wday % 7], timeStruct->tm_mday,
              months[timeStruct->tm_mon % 12], timeStruct->tm_year + 1900,
              timeStruct->tm_hour / 10, timeStruct->tm_hour % 10,
              timeStruct->tm_min / 10, timeStruct->tm_min % 10,
              timeStruct->tm_sec / 10, timeStruct->tm_sec % 10, sign,
              off_hour, off_min);
//    snprintf(retVal,255,"%04d",timeStruct->tm_year);
  }
//    strftime(retVal,255,"%Y",timeStruct);
//    strftime(retVal,255,"%a, %e %b %Y %T zone",timeStruct);
  else
    retVal[0] = 0x00;

  return QCString (retVal);
}

int
mimeHdrLine::parseDate (const char *inCStr, struct tm *fillTime, int *gmt_off)
{
  char *aCStr = (char *) inCStr;
  int retVal = 0;
  int state = 0;
  int skip;
  bool flagnumeric;
  long number;
  int ct_offset = 0;

  if (!fillTime)
    return 0;

  fillTime->tm_wday = 1;        //if we set this to 0 one year will be gone
  fillTime->tm_yday = 0;
  fillTime->tm_isdst = 0;
  fillTime->tm_year = 0;
#ifdef HAVE_TM_ZONE
  //not all systems have this field
  fillTime->tm_zone = 0;
  fillTime->tm_gmtoff = 0;
#endif

  /*          29 Aug 1996 14 : 04 : 52 CDT   */
  /* states: 1  2   3    4  5 6  7 8  9   10 */

  state = 1;

  skip = skipWS (aCStr);
  if (skip < 0)
    return 0;
  retVal += skip;
  aCStr += skip;

  while ((skip = parseWord (aCStr)))
  {
    flagnumeric = isdigit (*aCStr) || (*aCStr == '+') || (*aCStr == '-');
    number = atoi (aCStr);

//    qDebug("mimeHdrLine::parseDate - state %d - remaining [%d] '%s'",state,skip,aCStr);
    switch (state)
    {
    case 1:
      if (!flagnumeric)
        //skip day of week if any
        if (aCStr[skip] == ',')
        {
          state--;
          skip++;
          break;                //try next one
        }
        else
          return 0;

      fillTime->tm_mday = number;
      break;
    case 2:
      if (!qstrnicmp (aCStr, "Jan", 3))
      {
        fillTime->tm_mon = 0;
        break;
      }
      if (!qstrnicmp (aCStr, "Feb", 3))
      {
        fillTime->tm_mon = 1;
        break;
      }
      if (!qstrnicmp (aCStr, "Mar", 3))
      {
        fillTime->tm_mon = 2;
        break;
      }
      if (!qstrnicmp (aCStr, "Apr", 3))
      {
        fillTime->tm_mon = 3;
        break;
      }
      if (!qstrnicmp (aCStr, "May", 3))
      {
        fillTime->tm_mon = 4;
        break;
      }
      if (!qstrnicmp (aCStr, "Jun", 3))
      {
        fillTime->tm_mon = 5;
        break;
      }
      if (!qstrnicmp (aCStr, "Jul", 3))
      {
        fillTime->tm_mon = 6;
        break;
      }
      if (!qstrnicmp (aCStr, "Aug", 3))
      {
        fillTime->tm_mon = 7;
        break;
      }
      if (!qstrnicmp (aCStr, "Sep", 3))
      {
        fillTime->tm_mon = 8;
        break;
      }
      if (!qstrnicmp (aCStr, "Oct", 3))
      {
        fillTime->tm_mon = 9;
        break;
      }
      if (!qstrnicmp (aCStr, "Nov", 3))
      {
        fillTime->tm_mon = 10;
        break;
      }
      if (!qstrnicmp (aCStr, "Dec", 3))
      {
        fillTime->tm_mon = 11;
        break;
      }
      return 0;
    case 3:
      if (!flagnumeric)
        return 0;
      if (number < 50)
        number += 2000;
      if (number < 999)
        number += 1900;
      fillTime->tm_year = number - 1900;
      break;
    case 4:
      if (!flagnumeric)
        return 0;
      fillTime->tm_hour = number;
      break;
    case 5:
      if (*aCStr == ':')
        break;
      return 0;
    case 6:
      if (!flagnumeric)
        return 0;
      fillTime->tm_min = number;
      break;
    case 8:
      if (!flagnumeric)
        return 0;
      fillTime->tm_sec = number;
      break;
    case 7:
      if (*aCStr == ':')
        break;
      fillTime->tm_sec = 0;
      state = 9;
    case 9:
      if (flagnumeric)
      {                         /* happiness */
        if (number >= 0)
          ct_offset = (number / 100) * 60 + (number % 100);
        else
          ct_offset = -(((-number) / 100) * 60 + ((-number) % 100));
//          if (!qstrnicmp(aCStr,"-0000",5)) known = 1;
        break;
      }
      if (!qstrnicmp (aCStr, "UT", 2))
      {
        ct_offset = 0;
        break;
      }
      if (!qstrnicmp (aCStr, "GMT", 3))
      {
        ct_offset = 0;
        break;
      }
      /* XXX: GMT+nnnn? */

      if (!qstrnicmp (aCStr, "BST", 3))
      {
        ct_offset = 60;
        break;
      }
      if (!qstrnicmp (aCStr, "CDT", 3))
      {
        ct_offset = -300;
        break;
      }
      if (!qstrnicmp (aCStr, "CET", 3))
      {
        ct_offset = 60;
        break;
      }
      if (!qstrnicmp (aCStr, "CST", 3))
      {
        ct_offset = -360;
        break;
      }
      if (!qstrnicmp (aCStr, "EDT", 3))
      {
        ct_offset = -240;
        break;
      }
      if (!qstrnicmp (aCStr, "EET", 3))
      {
        ct_offset = 120;
        break;
      }
      if (!qstrnicmp (aCStr, "EST", 3))
      {
        ct_offset = -300;
        break;
      }
      if (!qstrnicmp (aCStr, "HKT", 3))
      {
        ct_offset = 480;
        break;
      }
      if (!qstrnicmp (aCStr, "IST", 3))
      {
        ct_offset = 120;
        break;
      }
      if (!qstrnicmp (aCStr, "JST", 3))
      {
        ct_offset = 540;
        break;
      }
      if (!qstrnicmp (aCStr, "MDT", 3))
      {
        ct_offset = -360;
        break;
      }
      if (!qstrnicmp (aCStr, "MET", 3))
      {
        ct_offset = 60;
        break;
      }
      if (!qstrnicmp (aCStr, "METDST", 6))
      {
        ct_offset = 120;
        break;
      }
      if (!qstrnicmp (aCStr, "MST", 3))
      {
        ct_offset = -420;
        break;
      }
      if (!qstrnicmp (aCStr, "PDT", 3))
      {
        ct_offset = -420;
        break;
      }
      if (!qstrnicmp (aCStr, "PST", 3))
      {
        ct_offset = -480;
        break;
      }
      return 0;
    case 10:
      if (!qstrnicmp (aCStr, "DST", 3))
        ct_offset += 60;
      break;
    }
    if (state < 10)
      ++state;

    // imap internal date is seperated by '-' not ' '
    if (aCStr[skip] == '-')
      skip++;
    aCStr += skip;
    retVal += skip;

    skip = skipWS (aCStr);
    if (skip < 0)
      break;
    retVal += skip;
    aCStr += skip;
  }

#ifdef HAVE_TM_ZONE
  fillTime->tm_gmtoff = ct_offset * 60;
#endif

  if (gmt_off)
    *gmt_off = ct_offset * 60;
  //sanitize the day of week and day of year values
  {
    time_t myTime;
    struct tm tempStruct, *staticStruct;

    memcpy ((void *) &tempStruct, (void *) fillTime, sizeof (struct tm));
    myTime = mktime (&tempStruct);

    staticStruct = localtime (&myTime);
    fillTime->tm_wday = staticStruct->tm_wday;
    fillTime->tm_yday = staticStruct->tm_yday;
  }

  return retVal;
}

/** return the label */

const QCString
mimeHdrLine::getLabel ()
{
  return mimeLabel;
}

/** return the value */
const QCString
mimeHdrLine::getValue ()
{
  return mimeValue;
}

QCString
mimeHdrLine::truncateLine (QCString aLine, unsigned int truncate)
{
  int cutHere;
  QCString retVal;

  while (aLine.length () > truncate)
  {
    cutHere = aLine.findRev (' ', truncate);
    if (cutHere < 1)
      cutHere = aLine.findRev ('\t', truncate);
    if (cutHere < 1)
      cutHere = aLine.find (' ', 1);
    if (cutHere < 1)
      cutHere = aLine.find ('\t', 1);
    if (cutHere < 1)
    {
      cerr << "cant truncate line" << endl;
      break;
    }
    else
    {
      retVal += aLine.left (cutHere) + '\n';
      aLine = aLine.right (aLine.length () - cutHere);
    }
  }
  retVal += aLine;

  return retVal;
}
