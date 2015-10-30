/**********************************************************************
 *
 * sstTemplateLib - cpp template library for sst
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
// sstRec03CargoMem.cpp    17.10.15  Re.    17.10.15  Re.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sstRec03Lib.h"
#include "sstRec03LibInt.h"

//=============================================================================
sstRec03CargoMemCls::sstRec03CargoMemCls()
{
  memset( &(*this), 0, sizeof((*this)));
}
//=============================================================================
sstRec03CargoMemCls::~sstRec03CargoMemCls()
{

}
//=============================================================================
int sstRec03CargoMemCls::GetOffset()
{
  return this->iOffset;
}
//=============================================================================
void sstRec03CargoMemCls::SetOffset(int iTmpOffset)
{
  this->iOffset = iTmpOffset;
}
//=============================================================================
int sstRec03CargoMemCls::GetCargoSize()
{
  return this->iSize;
}
//=============================================================================
void sstRec03CargoMemCls::SetCargoSize(int iTmpSize)
{
  this->iSize = iTmpSize;
}
//=============================================================================
char* sstRec03CargoMemCls::GetCargoSysNam()
{
  return this->cNam;
}
//=============================================================================
void sstRec03CargoMemCls::SetCargoSysNam(char *cTmpNam)
{
  strncpy(this->cNam,cTmpNam,4);
}
//=============================================================================
void* sstRec03CargoMemCls::GetCargoAdr()
{
  return this->vCargoAdr;
}
//=============================================================================
void sstRec03CargoMemCls::SetCargoAdr(void *vTmpCargoAdr)
{
  this->vCargoAdr = vTmpCargoAdr;
}
//=============================================================================



