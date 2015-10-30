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
// sstRec03Int.cpp    17.10.15  Re.    17.10.15  Re.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sstRec03Lib.h"
#include "sstRec03LibInt.h"

//=============================================================================
sstRec03InternCls::sstRec03InternCls(dREC03RECSIZTYP dRecSize)
{
    int iStat = 0;
    dQuantity = 0;
    ucStorage = 0;
    dActStored = 0;
    pFilHdl = NULL;
    bFileNotDelete = 0;  // Default: File will be deleted

    this->poRecMemSysKey = new (sstRec03CargoKeyInternCls);
    this->poRecMemUsrKey = new (sstRec03CargoKeyInternCls);
    this->poVector = new sstRec03VectSysCls();
    this->poHeader = new sstRec03HeaderCls();

    iStat = this->AddCargoSys( 0, sizeof(sstRec03HeaderCls), (char*) "SYS", this->poRecMemSysKey);
    iStat = this->AddCargoSys( 0, dRecSize, (char*) "USR", this->poRecMemUsrKey);
    assert(iStat >= 0);

}
//=============================================================================
sstRec03InternCls::~sstRec03InternCls()
{

  delete (this->poHeader);
  delete (this->poVector);
  delete (this->poRecMemUsrKey);
  delete (this->poRecMemSysKey);

    if(ucStorage) {
      puts("freeing storage");
      free(ucStorage);
    }

    if (this->pFilHdl != 0)
    {
        // Close File
        int iStat= fclose( this->pFilHdl);
        assert(iStat >= 0);

        if(!this->bFileNotDelete)
        {
            // Remove file from disk
            iStat = remove ( this->cDatnam);
            assert(iStat >= 0);
        }
    }
}
//=============================================================================
int sstRec03InternCls::WritNewInt(int iKey, void* element, dREC03RECNUMTYP *index)
{

  if ( iKey != 0) return -1;

  if(this->pFilHdl != NULL)
  {
    // Jump to end of file and write record
    fseek  (this->pFilHdl, (this->dActStored) * this->poVector->GetSize(), SEEK_SET);
    fwrite (element, this->poVector->GetSize(), 1, this->pFilHdl);

  }
  else
  {
      if(dActStored >= dQuantity) // Enough space left?
        inflate(100);
      // Copy element into storage,
      // starting at next empty space:
      memcpy(&(ucStorage[dActStored * this->poVector->GetSize()]),
              element,
              this->poVector->GetSize());
  }

  dActStored++;
  *index = dActStored;

  return 0;
}
//=============================================================================
int sstRec03InternCls::WritNewVector(int iKey, void* vRecAdr, dREC03RECNUMTYP *dRecNo)
{
  // Set actual date into DateNew field
  this->poHeader->SetNewDate();
  this->poHeader->SetChangeDate();

  // write record into vector memory
  int iStat = this->poVector->WrtCargo( 0, this->poRecMemSysKey, this->poHeader);

  // write record into vector memory
  iStat = this->poVector->WrtCargo( 0, this->poRecMemUsrKey, vRecAdr);

  // Write new record into intern sstRec memory
  iStat = this->WritNewInt( iKey, this->poVector->GetAdr(), dRecNo);
  return iStat;
}

//=============================================================================
int sstRec03InternCls::WritInt(int iKey, void* vRecAdr, dREC03RECNUMTYP index)
{
  if ( iKey != 0) return -1;
  if (index <= 0 || index > dActStored) return -2;

  if(this->pFilHdl != NULL)
  {
    // Jump to end of file and write record
    fseek  (this->pFilHdl, index * this->poVector->GetSize(), SEEK_SET);
    fwrite (vRecAdr, this->poVector->GetSize(), 1, this->pFilHdl);

  }
  else
  {
      // Copy element into storage,
      // starting at next empty space:
      memcpy(&(ucStorage[(index-1) * this->poVector->GetSize()]), vRecAdr, this->poVector->GetSize());
  }

  return 0;
}
//=============================================================================
int sstRec03InternCls::WritVector(int iKey, void* vRecAdr, dREC03RECNUMTYP dRecNo)
{

  sstRec03HeaderCls *poLocHeader = NULL;
  int iStat = 0;

  // Read record from sstRec memory with Record number into vector
  iStat = this->ReadInt( iKey, dRecNo, this->poVector->GetAdr());
  if (iStat < 0) return iStat;

  // show header cargo of vector data
  iStat = this->poVector->GetCargoAdr(0,this->poRecMemSysKey,(void**) &poLocHeader);

  // Write Change Data in Header Cargo of vector
  poLocHeader->SetChangeDate();

  // write user record into vector memory
  iStat = this->poVector->WrtCargo( 0, this->poRecMemUsrKey, vRecAdr);

  // Write vector Record at position in intern sstRec Memory
  iStat = this->WritInt( iKey, this->poVector->GetAdr(), dRecNo);
  return iStat;
}
//=============================================================================
int sstRec03InternCls::ReadInt(int iKey, dREC03RECNUMTYP index, void *vAdr)
{
    if ( iKey != 0) return -1;
    if(index <= 0 || index > dActStored) return -2;

  if(this->pFilHdl != 0)
  {
    fseek (this->pFilHdl, (index-1) * this->poVector->GetSize(), SEEK_SET);
    fread (vAdr, this->poVector->GetSize(), 1, this->pFilHdl);
  }
  else
  {
      void *vLocAdr = NULL;

      // if(index >= next || index < 0)
      if(index > dActStored)
        return -2;  // Not out of bounds?

      // Produce pointer to desired element:
      vLocAdr = (void*) &(ucStorage[(index-1) * this->poVector->GetSize()]);

      // copy one record data to given record adress
      memcpy( vAdr, vLocAdr, this->poVector->GetSize());
  }

  return 0;
}
//=============================================================================
int sstRec03InternCls::ReadVector(int iKey, dREC03RECNUMTYP dRecNo, void *vRecAdr)
{
  if ( iKey != 0) return -1;

  // Read record from sstRec memory with Record number
    int iStat = this->ReadInt( iKey, dRecNo, this->poVector->GetAdr());
    if (iStat < 0) return -2;

    sstRec03HeaderCls *poLocHeader = NULL;
    iStat = this->poVector->GetCargoAdr(0,this->poRecMemSysKey,(void**) &poLocHeader);

    if (poLocHeader->RecGetDeleteStatus())
    {
      this->poVector->ResetMem(0);
      // Read record from vector memory
      iStat = this->poVector->RedCargo( 0, this->poRecMemUsrKey, vRecAdr);
      return -3;
    }

    // Read record from vector memory
    iStat = this->poVector->RedCargo( 0, this->poRecMemUsrKey, vRecAdr);

    // Read Header Data from vector and test
    sstRec03HeaderCls oHeaderData;
    if (iStat >= 0) iStat = this->poVector->RedCargo( 0, this->poRecMemSysKey, &oHeaderData);
    iStat = strncmp((char*)"sstRec02", oHeaderData.GetVersStr(), 10);
    assert(iStat==0);
    assert(oHeaderData.getRecSize() == this->poVector->GetSize());

    return iStat;
}
//=============================================================================
dREC03RECNUMTYP sstRec03InternCls::count() {
  return dActStored; // Number of actual stored records
}
//==============================================================================
int sstRec03InternCls::OpenFile(int   iKey,
                                char *cSysNam)
{
  if ( iKey != 0) return -1;
  if (this->pFilHdl != NULL) return -2;
  if (strnlen(cSysNam,dREC03FILNAMMAXLEN) <= 0) return -3;
  if (this->dActStored > 0) return -4;

  strncpy(cDatnam, cSysNam, dREC03FILNAMMAXLEN);
  strncat(cDatnam, ".rec", dREC03FILNAMMAXLEN);

  // Reoopen existing or open new file for binary Reading/Writing
  this->pFilHdl = fopen( cDatnam, "a+b");
  assert( this->pFilHdl != NULL);

  fseek (this->pFilHdl, 0, SEEK_END);
  long lSize = ftell(this->pFilHdl);

  // Calculate number of existing records in file
  this->dActStored = lSize / this->poVector->GetSize();

  return 0;
}
//==============================================================================
int sstRec03InternCls::NewFile(int   iKey,
                               char *cSysNam)
{
  if ( iKey != 0) return -1;
  if (this->pFilHdl != NULL) return -2;
  if (strnlen(cSysNam,dREC03FILNAMMAXLEN) <= 0) return -3;
  if (this->dActStored > 0) return -4;

  strncpy(cDatnam, cSysNam, dREC03FILNAMMAXLEN);
  strncat(cDatnam, ".rec", dREC03FILNAMMAXLEN);

  // Open new file for binary Reading/Writing
  this->pFilHdl = fopen( cDatnam, "w+b");
  assert( this->pFilHdl != NULL);

  return 0;
}
//==============================================================================
int sstRec03InternCls::SetStoreFile(int iKey)
{
  if ( iKey != 0) return -1;
  this->bFileNotDelete = 1;
  return 0;
}
//=============================================================================
void sstRec03InternCls::inflate(int increase) {
  void* v = realloc(ucStorage, (dQuantity+increase) * this->poVector->GetSize());
  assert(v);  // Was it successful?
  ucStorage = (unsigned char*)v;
  dQuantity += increase;
}
//=============================================================================
int sstRec03InternCls::AddCargoSys( int                  iKey,
                              unsigned int         uiSize,
                              char                *cCargoNam,
                              sstRec03CargoKeyInternCls *oCargoKey)
{
    int iRet  = 0;
    int iStat = 0;
  //-----------------------------------------------------------------------------
    if ( iKey != 0) return -1;

    if (this->count()!= 0) return -10;

    iStat = this->poVector->AddCargoSys(iKey,uiSize,cCargoNam,oCargoKey);

    this->poHeader->setRecSize(this->poVector->GetSize());

    assert(iRet >= 0);

    // Small Errors will given back
    iRet = iStat;

    return iRet;
}

//==============================================================================
int sstRec03InternCls::WrtCargo ( int              iKey,
                            sstRec03CargoKeyInternCls *oDataKey,
                            void            *vCargoAdr)
{
    int iRet  = 0;
    int iStat = 0;
  //-----------------------------------------------------------------------------
    if ( iKey != 0) return -1;

    iStat = this->poVector->WrtCargo(0,oDataKey,vCargoAdr);

    assert(iRet >= 0);

    // Small Errors will given back
    iRet = iStat;

    return iRet;
}
//==============================================================================
int sstRec03InternCls::RedCargo ( int              iKey,
                            sstRec03CargoKeyInternCls *oDataKey,
                            void            *vCargoAdr)
{
    int iRet  = 0;
    int iStat = 0;
  //-----------------------------------------------------------------------------
    if ( iKey != 0) return -1;

    iStat = this->poVector->RedCargo(0,oDataKey,vCargoAdr);

    assert(iRet >= 0);

    // Small Errors will given back
    iRet = iStat;

    return iRet;
}
//==============================================================================
void sstRec03InternCls::SetNewDate()
{
  this->poHeader->SetNewDate();
}
//==============================================================================
void sstRec03InternCls::SetChangeDate()
{
  this->poHeader->SetChangeDate();
}
//==============================================================================
int sstRec03InternCls::RecSetDeleted( int               iKey,
                                       dREC03RECNUMTYP   dRecNo)
{
  if ( iKey != 0) return -1;
  if (dRecNo < 1 || dRecNo > this->dActStored) return -2;

  sstRec03HeaderCls *poLocHeader = NULL;
  int iStat = 0;

  // Read record from sstRec memory with Record number
  iStat = this->ReadInt( iKey, dRecNo, this->poVector->GetAdr());

  if (iStat >= 0) iStat = this->poVector->GetCargoAdr(0,this->poRecMemSysKey,(void**) &poLocHeader);

  if (iStat >= 0) poLocHeader->RecSetDeleted();

  // Write record from vector to sstRec memory with Record number
  if (iStat >= 0) iStat = this->WritInt( iKey, this->poVector->GetAdr(),dRecNo);

  return iStat;
}
//==============================================================================
int sstRec03InternCls::RecSetMarked( int               iKey,
                                      dREC03RECNUMTYP   dRecNo)
{
  if ( iKey != 0) return -1;
  if (dRecNo < 1 || dRecNo > this->dActStored) return -2;

  sstRec03HeaderCls *poLocHeader = NULL;
  int iStat = 0;

  // Read record from sstRec memory with Record number
  iStat = this->ReadInt( iKey, dRecNo, this->poVector->GetAdr());

  if (iStat >= 0) iStat = this->poVector->GetCargoAdr(0,this->poRecMemSysKey,(void**) &poLocHeader);

  if (iStat >= 0) poLocHeader->RecSetMarked();

  // Write record from vector to sstRec memory with Record number
  if (iStat >= 0) iStat = this->WritInt( iKey, this->poVector->GetAdr(),dRecNo);

  return 0;
}
//==============================================================================
int sstRec03InternCls::RecSetUndeleted( int               iKey,
                                         dREC03RECNUMTYP   dRecNo)
{
  if ( iKey != 0) return -1;
  if (dRecNo < 1 || dRecNo > this->dActStored) return -2;

  sstRec03HeaderCls *poLocHeader = NULL;
  int iStat = 0;

  // Read record from sstRec memory with Record number
  iStat = this->ReadInt( iKey, dRecNo, this->poVector->GetAdr());

  if (iStat >= 0) iStat = this->poVector->GetCargoAdr(0,this->poRecMemSysKey,(void**) &poLocHeader);

  if (iStat >= 0) poLocHeader->RecSetUndeleted();

  // Write record from vector to sstRec memory with Record number
  if (iStat >= 0) iStat = this->WritInt( iKey, this->poVector->GetAdr(),dRecNo);

  return 0;
}
//==============================================================================
int sstRec03InternCls::RecSetUnmarked( int               iKey,
                                        dREC03RECNUMTYP   dRecNo)
{
  if ( iKey != 0) return -1;
  if (dRecNo < 1 || dRecNo > this->dActStored) return -2;

  sstRec03HeaderCls *poLocHeader = NULL;
  int iStat = 0;

  // Read record from sstRec memory with Record number
  iStat = this->ReadInt( iKey, dRecNo, this->poVector->GetAdr());

  if (iStat >= 0) iStat = this->poVector->GetCargoAdr(0,this->poRecMemSysKey,(void**) &poLocHeader);

  if (iStat >= 0) poLocHeader->RecSetUnmarked();

  // Write record from vector to sstRec memory with Record number
  if (iStat >= 0) iStat = this->WritInt( iKey, this->poVector->GetAdr(),dRecNo);

  return 0;
}
//==============================================================================
bool sstRec03InternCls::RecGetDeleteStatus( int               iKey,
                                            dREC03RECNUMTYP   dRecNo)
{
  if ( iKey != 0) return -1;
  if (dRecNo < 1 || dRecNo > this->dActStored) return 0;

  sstRec03HeaderCls *poLocHeader = NULL;
  int iStat = 0;

  // Read record from sstRec memory with Record number
  iStat = this->ReadInt( iKey, dRecNo, this->poVector->GetAdr());

  if (iStat >= 0) iStat = this->poVector->GetCargoAdr(0,this->poRecMemSysKey,(void**) &poLocHeader);

  return poLocHeader->RecGetDeleteStatus();
}
//==============================================================================
bool sstRec03InternCls::RecGetMarkStatus( int               iKey,
                                          dREC03RECNUMTYP   dRecNo)
{
  if ( iKey != 0) return -1;
  if (dRecNo < 1 || dRecNo > this->dActStored) return 0;

  sstRec03HeaderCls *poLocHeader = NULL;
  int iStat = 0;

  // Read record from sstRec memory with Record number
  iStat = this->ReadInt( iKey, dRecNo, this->poVector->GetAdr());

  if (iStat >= 0) iStat = this->poVector->GetCargoAdr(0,this->poRecMemSysKey,(void**) &poLocHeader);

  return poLocHeader->RecGetMarkStatus();
}
//==============================================================================


