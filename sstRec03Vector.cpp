/**********************************************************************
 *
 * sst_rec02_lib - library for working with data records
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
// sstRec03Vector.cpp    17.10.15  Re.    17.10.15  Re.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sstRec03Lib.h"
#include "sstRec03LibInt.h"

//=============================================================================
sstRec03VectSysCls::sstRec03VectSysCls()
{
  this->dVectorSize = 0;
  this->vVectorAdr = NULL;
  this->uiNumCargoSys = 0;
  this->poMemAdr = NULL;
}
//=============================================================================
sstRec03VectSysCls::~sstRec03VectSysCls()
{
  if (this->poMemAdr != NULL) free(this->poMemAdr);
  if (this->vVectorAdr != NULL) free(this->vVectorAdr);
}
//=============================================================================
int sstRec03VectSysCls::AddCargoSys( int             iKey,
                                     unsigned int    uiSize,
                                     char           *cCargoNam,
                                     sstRec03CargoKeyInternCls *oCargoKey)
//-----------------------------------------------------------------------------
{
  int iStat = 0;

  if ( iKey != 0) return -1;  // Wrong Key
  if ( uiSize <= 0) return -2;  // Cargo Packet must not be empty
  iStat = strlen(cCargoNam);  // Cargo Name has to be length 3
  if(iStat !=3) return -3;
  if (oCargoKey->iKey != 0) return -4;   // Cargo Key should be emtpy

  // is new cargo sys name unique?
  for (unsigned int ii=1; ii<=this->uiNumCargoSys; ii++)
  {
    iStat = strncmp(this->poMemAdr[ii-1].GetCargoSysNam(), cCargoNam, 4);
    if (iStat == 0) return -5;
  }

  int iCargoOffs = 0;
  void *Adr = NULL;

  this->uiNumCargoSys++;

  // === Handle cargo array memory
  if (uiNumCargoSys == 1)  // first cargo
  {
    iStat = sizeof(sstRec03CargoMemCls);
    Adr = calloc( iStat,1);
    if (Adr <= NULL) assert(0);
    this->poMemAdr = (sstRec03CargoMemCls*) Adr;
    this->poMemAdr[this->uiNumCargoSys-1].SetOffset(0);
  }
  else
  {  // next cargo memory
    iStat = this->uiNumCargoSys * sizeof(sstRec03CargoMemCls);
    Adr = realloc(this->poMemAdr , iStat);
    if (Adr <= NULL) assert(0);
    this->poMemAdr = (sstRec03CargoMemCls*) Adr;
    this->poMemAdr[this->uiNumCargoSys-1].SetOffset(this->dVectorSize);
  }

  this->poMemAdr[this->uiNumCargoSys-1].SetCargoSize( uiSize);
  this->poMemAdr[this->uiNumCargoSys-1].SetCargoSysNam(cCargoNam);

  // === Handle Vector memory
  this->dVectorSize = this->dVectorSize + uiSize;
  if (uiNumCargoSys == 1)
  {
    Adr = calloc( this->dVectorSize,1);
    if (Adr <= NULL) assert(0);
  }
  else
  {
    Adr = realloc(this->vVectorAdr ,this->dVectorSize);
    if (Adr <= NULL) assert(0);

  }
  this->vVectorAdr = Adr;

  // Recalculate all cargo adresses in vector
  iCargoOffs = 0;
  for (unsigned int ii=1;ii<=uiNumCargoSys;ii++)
  {
    this->CalcSetPos ( this->vVectorAdr, &Adr, iCargoOffs);
    this->poMemAdr[ii-1].SetCargoAdr(Adr);
    iCargoOffs = iCargoOffs + this->poMemAdr[ii-1].GetCargoSize();
  }

  // set cargo key
  oCargoKey->iKey = uiNumCargoSys;
  strncpy(oCargoKey->cNam,cCargoNam,4);
  oCargoKey->poCargoAdr = this;

  return 0;
}
//=============================================================================
int sstRec03VectSysCls::WrtCargo ( int              iKey,
                                   sstRec03CargoKeyInternCls *oDataKey,
                                   void            *CargoAdr)
{
  sstRec03CargoMemCls *oCargoArrayRec;
  int iStat = 0;
  if ( iKey != 0) return -1;

  // Check Adress of Cargo System
  if (oDataKey->poCargoAdr != this) return -2;

  // Check Name of Cargo System
  iStat = strncmp( oDataKey->cNam, this->poMemAdr[oDataKey->iKey-1].GetCargoSysNam(),4);
  if (iStat != 0) return -3;

  // Copy Cargo into Vector at cargo position
  unsigned long ulSize = this->poMemAdr[oDataKey->iKey-1].GetCargoSize();

  oCargoArrayRec = this->poMemAdr;

  for(int ii=1;ii<=oDataKey->iKey-1;ii++)
  {
    oCargoArrayRec++;
  }

  memcpy(this->poMemAdr[oDataKey->iKey-1].GetCargoAdr(), CargoAdr, ulSize);

  return 0;
}
//=============================================================================
int sstRec03VectSysCls::RedCargo ( int              iKey,
                                   sstRec03CargoKeyInternCls *oDataKey,
                                   void            *CargoAdr)
{
  int iStat = 0;

  if ( iKey != 0) return -1;

  if (oDataKey->poCargoAdr != this) return -2;

  iStat = strncmp( oDataKey->cNam, this->poMemAdr[oDataKey->iKey-1].GetCargoSysNam(),4);
  if (iStat != 0) return -3;

  memcpy( CargoAdr, this->poMemAdr[oDataKey->iKey-1].GetCargoAdr(), this->poMemAdr[oDataKey->iKey-1].GetCargoSize());

  return 0;
}
//=============================================================================
void sstRec03VectSysCls::CalcSetPos ( void   *BasPtr,
                                      void  **IdxPtr,
                                      long    Offs)
//-----------------------------------------------------------------------------
{
  unsigned char *xxx;
//.............................................................................
  xxx = (unsigned char*) BasPtr;
  xxx = xxx + Offs;
  (*IdxPtr) = (void*) xxx;
}
//=============================================================================
unsigned long sstRec03VectSysCls::GetSize() const
{
  return this->dVectorSize;
}
//=============================================================================

void* sstRec03VectSysCls::GetAdr() const
{
  return this->vVectorAdr;
}

//=============================================================================
int sstRec03VectSysCls::GetCargoAdr(int                iKey,
                                  sstRec03CargoKeyInternCls   *oDataKey,
                                  void             **vCargoAdr)
{
  int iStat = 0;

  if ( iKey != 0) return -1;

  if (oDataKey->poCargoAdr != this) return -2;

  iStat = strncmp( oDataKey->cNam, this->poMemAdr[oDataKey->iKey-1].GetCargoSysNam(),4);
  if (iStat != 0) return -3;

  *vCargoAdr = this->poMemAdr[oDataKey->iKey-1].GetCargoAdr();
  return 0;
}

//=============================================================================
int sstRec03VectSysCls::GetOffset (int                iKey,
                                 sstRec03CargoKeyInternCls   *oDataKey,
                                 int               *iOffset)
{
  int iStat = 0;

  if ( iKey != 0) return -1;

  if (oDataKey->poCargoAdr != this) return -2;

  iStat = strncmp( oDataKey->cNam, this->poMemAdr[oDataKey->iKey-1].GetCargoSysNam(),4);
  if (iStat != 0) return -3;

  *iOffset = this->poMemAdr[oDataKey->iKey-1].GetOffset();
  return 0;
}
//=============================================================================
void sstRec03VectSysCls::ResetMem(int iKey)
{
  if ( iKey == 0) memset( this->vVectorAdr, 0, this->dVectorSize);
}
//=============================================================================


