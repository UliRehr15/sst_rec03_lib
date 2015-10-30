/**********************************************************************
 *
 * sst_rec_lib - library for working with data records
 * Hosted on github
 *
 * Copyright (C) 2015 Uli Rehr
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/
// sstRec03Header.cpp    17.10.15  Re.    17.10.15  Re.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "sstRec03Lib.h"
#include "sstRec03LibInt.h"

//=============================================================================
sstRec03HeaderCls::sstRec03HeaderCls()
{
  strncpy(cVersionstring,(char*)"sstRec02",10);    /**< Version String, for exampe sstRec02 */
  dRecSize = 0;  /**< Size of every Record */
  memset( cRecChgDateTime, 0, 18);    /**< Write Change Date, for exampe 151012 */
  memset( cRecNewDateTime, 0, 18);    /**< Write New Date, for exampe 151012 */
  bDel = 0;    /**< Delete Flag */
  bMark = 0;   /**< Mark Flag */
}
//=============================================================================
dREC03RECSIZTYP sstRec03HeaderCls::getRecSize() const
{
  return this->dRecSize;
}
//=============================================================================
void sstRec03HeaderCls::setRecSize(const dREC03RECSIZTYP &dTmpValue)
{
  this->dRecSize = dTmpValue;
}
//=============================================================================
char* sstRec03HeaderCls::GetVersStr()
{
  return this->cVersionstring;
}
//=============================================================================
void sstRec03HeaderCls::SetVersStr(char *cTmpNam)
{
  strncpy(this->cVersionstring,cTmpNam,10);
}
//=============================================================================
void sstRec03HeaderCls::SetNewDate()
{
  time_t     now = time(0);
  struct tm  tstruct;
  char       buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%Y%m%d.%X", &tstruct);
  strncpy(this->cRecNewDateTime,buf,18);
}
//=============================================================================
void sstRec03HeaderCls::SetChangeDate()
{
  time_t     now = time(0);
  struct tm  tstruct;
  char       buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%Y%m%d.%X", &tstruct);
  strncpy(this->cRecChgDateTime,buf,18);
}
//==============================================================================
void sstRec03HeaderCls::RecSetDeleted()
{
  this->bDel = true;
}
//==============================================================================
void sstRec03HeaderCls::RecSetMarked()
{
  this->bMark = true;
}
//==============================================================================
void sstRec03HeaderCls::RecSetUndeleted()
{
  this->bDel = false;
}
//==============================================================================
void sstRec03HeaderCls::RecSetUnmarked()
{
  this->bMark = false;
}
//==============================================================================
bool sstRec03HeaderCls::RecGetDeleteStatus()
{
  return this->bDel;
}
//==============================================================================
bool sstRec03HeaderCls::RecGetMarkStatus()
{
  return this->bMark;
}
//==============================================================================
