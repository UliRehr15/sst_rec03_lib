/**********************************************************************
 *
 * sst_rec_lib - library for working with data records
 * Hosted on github
 *
 * Copyright (C) 2015 Hans Ulrich Rehr
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/
// sstRec03.cpp    17.10.15  Re.    17.10.15  Re.
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sstRec03Lib.h"
#include "sstRec03LibInt.h"

//==============================================================================
sstRec03Cls::sstRec03Cls(dREC03RECSIZTYP Size) {
  poRec01Intern = new sstRec03InternCls(Size);
}
//==============================================================================
sstRec03Cls::~sstRec03Cls() {
    delete(poRec01Intern);
}
//==============================================================================
int sstRec03Cls::WritNew(int iKey, void* vRecAdr, dREC03RECNUMTYP *pdRecNo) {

    return poRec01Intern->WritNewVector( iKey, vRecAdr, pdRecNo);
}
//==============================================================================
int sstRec03Cls::Writ(int iKey, void* vRecAdr, dREC03RECNUMTYP dRecNo)
{
  return poRec01Intern->WritVector( iKey, vRecAdr, dRecNo);
}

//==============================================================================
int sstRec03Cls::Read(int iKey, dREC03RECNUMTYP dRecNo, void *vRecAdr) {
    int iStat = poRec01Intern->ReadVector( iKey, dRecNo, vRecAdr);
    return iStat;
}
//==============================================================================
dREC03RECNUMTYP sstRec03Cls::count() {
    return poRec01Intern->count();
}
//==============================================================================
int sstRec03Cls::OpenFile(int   iKey,
                    char *cSysNam)
{
  if ( iKey != 0) return -1;

  int iStat = 0;
  iStat = this->poRec01Intern->OpenFile(iKey,cSysNam);
  return iStat;
}
//==============================================================================
int sstRec03Cls::NewFile(int   iKey,
                   char *cSysNam)
{
  if ( iKey != 0) return -1;

  int iStat = 0;
  iStat = this->poRec01Intern->NewFile(iKey,cSysNam);
  return iStat;
}
//==============================================================================
int sstRec03Cls::SetStoreFile(int iKey)
{
  if ( iKey != 0) return -1;
  int iStat = this->poRec01Intern->SetStoreFile(iKey);
  return iStat;
}
//==============================================================================
int sstRec03Cls::AddCargoSys( int                  iKey,
                              unsigned int         uiSize,
                              char                *cCargoNam,
                              sstRec03CargoKeyCls *oCargoKey)
//-----------------------------------------------------------------------------
{
    int iRet  = 0;
    int iStat = 0;
    //-----------------------------------------------------------------------------
    if ( iKey != 0) return -1;

    iStat = this->poRec01Intern->AddCargoSys(iKey,uiSize,cCargoNam, oCargoKey->poCargoKeyIntern);

    // Fatal Errors goes to an assert
    assert(iRet >= 0);

    // Small Errors will given back
    iRet = iStat;

    return iRet;
}

//==============================================================================
int sstRec03Cls::WrtCargo ( int              iKey,
                            sstRec03CargoKeyCls *oDataKey,
                            void            *vCargoAdr)
//------------------------------------------------------------------------------
{
    int iRet  = 0;
    int iStat = 0;
    //-----------------------------------------------------------------------------
    if ( iKey != 0) return -1;

    iStat = this->poRec01Intern->WrtCargo(iKey,oDataKey->poCargoKeyIntern,vCargoAdr);

    // Fatal Errors goes to an assert
    assert(iRet >= 0);

    // Small Errors will given back
    iRet = iStat;

    return iRet;
}
//==============================================================================
int sstRec03Cls::RedCargo ( int              iKey,
                            sstRec03CargoKeyCls *oDataKey,
                            void            *vCargoAdr)
{
    int iRet  = 0;
    int iStat = 0;
  //-----------------------------------------------------------------------------
    if ( iKey != 0) return -1;

    iStat = this->poRec01Intern->RedCargo( iKey, oDataKey->poCargoKeyIntern, vCargoAdr);

    assert(iRet >= 0);

    // Small Errors will given back
    iRet = iStat;

    return iRet;
}
//==============================================================================
int sstRec03Cls::RecSetDeleted ( int               iKey,
                                 dREC03RECNUMTYP   dRecNo)
{
  return this->poRec01Intern->RecSetDeleted( iKey, dRecNo);
}
//==============================================================================
int sstRec03Cls::RecSetMarked( int               iKey,
                                dREC03RECNUMTYP   dRecNo)
{
  return this->poRec01Intern->RecSetMarked( iKey, dRecNo);
}
//==============================================================================
int sstRec03Cls::RecSetUndeleted( int               iKey,
                                   dREC03RECNUMTYP   dRecNo)
{
  return this->poRec01Intern->RecSetUndeleted( iKey, dRecNo);
}
//==============================================================================
int sstRec03Cls::RecSetUnmarked( int               iKey,
                                  dREC03RECNUMTYP   dRecNo)
{
  return this->poRec01Intern->RecSetUnmarked( iKey, dRecNo);
}
//==============================================================================
bool sstRec03Cls::RecGetDeleteStatus( int               iKey,
                                      dREC03RECNUMTYP   dRecNo)
{
  return this->poRec01Intern->RecGetDeleteStatus( iKey, dRecNo);
}
//==============================================================================
bool sstRec03Cls::RecGetMarkStatus( int               iKey,
                                    dREC03RECNUMTYP   dRecNo)
{
  return this->poRec01Intern->RecGetMarkStatus( iKey, dRecNo);
}
//==============================================================================
int sstRec03Cls::TreIni ( int              iKey,
             void            *DsAdr,
             void            *CompAdr,
             int              CompSiz,
             sstRec03CompTyp_enum     CompTyp,
             sstRec03TreeKeyCls   *oTre)
{
  return this->poRec01Intern->TreIni(iKey,DsAdr,CompAdr,CompSiz,CompTyp,oTre);
}
//==============================================================================
int sstRec03Cls::TreBld ( int              iKey,
             sstRec03TreeKeyCls   *oTre)
{
  return this->poRec01Intern->TreBld(iKey,oTre);
}

//==============================================================================
int sstRec03Cls::TreReadNxtGE (int              iKey,
                 sstRec03TreeKeyCls   *oTre,
                 void            *DSatz,
                 dREC03RECNUMTYP   *SNr)
{
  return this->poRec01Intern->TreReadNxtGE(iKey,oTre,DSatz,SNr);
}
//==============================================================================
int sstRec03Cls::TreSeaFrst ( int            iKey,
                 sstRec03TreeKeyCls *oTre,
                 dREC03RECNUMTYP  *SNr)
{
  return this->poRec01Intern->TreSeaFrst(iKey,oTre,SNr);
}
//==============================================================================
int sstRec03Cls::TreSeaNxtGT ( int             iKey,
                  sstRec03TreeKeyCls  *oTre,
                  dREC03RECNUMTYP    SNr1,
                  dREC03RECNUMTYP   *SNr2)
{
  return this->poRec01Intern->TreSeaNxtGT(iKey,oTre,SNr1,SNr2);
}
//==============================================================================
int sstRec03Cls::TreWriteNew( int               iKey,
                            void            *vRecAdr,
                            dREC03RECNUMTYP *dRecNo)
{
  return this->poRec01Intern->TreWriteNew( iKey, vRecAdr, dRecNo);
}
//==============================================================================
