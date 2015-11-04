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
    this->iTriAnz = 0; // Number of defined trees is zero
    this->poTre = NULL;

    this->poRecMemSysKey = new (sstRec03CargoKeyInternCls);
    this->poRecMemUsrKey = new (sstRec03CargoKeyInternCls);
    this->poVector = new sstRec03VectSysCls();
    this->poHeader = new sstRec03HeaderCls();

    iStat = this->AddCargoSys( 0, sizeof(sstRec03HeaderCls), (char*) "SYST", this->poRecMemSysKey);
    iStat = this->AddCargoSys( 0, dRecSize, (char*) "USER", this->poRecMemUsrKey);
    assert(iStat >= 0);

}
//=============================================================================
sstRec03InternCls::~sstRec03InternCls()
{

  // First delete all defined intern tree key
  for (int ii=1;ii<=this->iTriAnz;ii++)
  {
     delete this->poTre[ii-1].poDataKey;
  }
  // next free tree array in heap
  free(this->poTre);

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
//=============================================================================
int sstRec03InternCls::TreIni ( int                iKey,
                                void              *DsAdr,
                                void              *CompAdr,
                                int                CompSiz,
                                sstRec03CompTyp_enum   CompTyp,
                                sstRec03TreeKeyCls     *oTre)
//-----------------------------------------------------------------------------
{
  int iStat = 0;
  sstRec03TreeNodeCls oBaum;
  void *Adr = NULL;

//.............................................................................
  if (iKey != 0) return -1;

  // this->Tre = new (sstRec03TreeHeaderCls);

  this->iTriAnz++;

  // === Handle cargo array memory
  if (this->iTriAnz == 1)  // first cargo
  {
    iStat = sizeof(sstRec03TreeHeaderCls);
    Adr = calloc( iStat,1);
    if (Adr <= NULL) assert(0);
    this->poTre = (sstRec03TreeHeaderCls*) Adr;
    // this->poMemAdr[this->uiNumCargoSys-1].SetOffset(0);
  }
  else
  {  // next cargo memory
    iStat = this->iTriAnz * sizeof(sstRec03TreeHeaderCls);
    Adr = realloc(this->poTre , iStat);
    if (Adr <= NULL) assert(0);
    this->poTre = (sstRec03TreeHeaderCls*) Adr;
    // this->poMemAdr[this->uiNumCargoSys-1].SetOffset(this->dVectorSize);
  }




  oTre->iTriNo = this->iTriAnz;  // Set extern tree identification key

  // Old Size of Vector is offset to Tre Stru
  this->poTre[this->iTriAnz-1].iAdrOfs = this->poVector->GetSize();

  // find next Tree name for Cargo System: TR01, TR02 ...
  char cLocTreeCargoName[dREC03CARGONAMMAXLEN+1];
  char cLocStr[3];
  strncpy(cLocTreeCargoName,(char*)"TR",dREC03CARGONAMMAXLEN+1);
  int n;
  n = sprintf (cLocStr, "%02d", this->iTriAnz);
  assert(n==2);
  strncat(cLocTreeCargoName,cLocStr,dREC03CARGONAMMAXLEN+1);

  this->poTre[this->iTriAnz-1].poDataKey = new(sstRec03CargoKeyInternCls);

  // define cargo system for new tree
  iStat = this->AddCargoSys( 0, sizeof(sstRec03TreeNodeCls), cLocTreeCargoName, this->poTre[this->iTriAnz-1].poDataKey);
  assert(iStat == 0);

  oBaum.dLeft_LT = 0;
  oBaum.dRight_GE = 0;
  iStat = poVector->WrtCargo(0,this->poTre[this->iTriAnz-1].poDataKey,&oBaum);

  // Default is entry of tree at record 1
  this->poTre[this->iTriAnz-1].dRoot = 1;

  // Offset of compare value from start of record
  this->poTre[this->iTriAnz-1].iOffset = (char*)CompAdr - (char*)DsAdr;

  // Size of compare value
  this->poTre[this->iTriAnz-1].iSize = CompSiz;

  // Type of compare value
  this->poTre[this->iTriAnz-1].eTyp  = CompTyp;

  return iStat;
}
//=============================================================================
int sstRec03InternCls::TreBld ( int              iKey,
                                sstRec03TreeKeyCls   *oTre)
//.............................................................................
{
  void *Adr1 = NULL;        // adress of compare value in vector
  dREC03RECNUMTYP  Root = 0;

  dREC03RECNUMTYP  ii = 0;
  int   iStat = 0;
//.............................................................................
  if (iKey != 0) return -1;

  // Root = NIL;
  Root = 0;

  for( ii=1; ii<=this->dActStored; ii++)
  {
    // record record from RecMem into vector
    iStat = this->ReadInt(  0, ii, this->poVector->GetAdr());

    // Get adress of user data
    void *vUsrAdr = NULL;
    iStat = this->poVector->GetCargoAdr(0,this->poRecMemUsrKey,&vUsrAdr);
    // Get Adress of compare value in user data
    this->poVector->CalcSetPos( vUsrAdr, &Adr1, this->poTre[oTre->iTriNo-1].iOffset);

    // Insert new record ii in tree otre with root record
    // Root = DSiTreIns2( 0, oTre, Root , ii, Adr1);
    Root = DSiTreInsert( 0, &this->poTre[oTre->iTriNo-1], Root , ii, Adr1);
  }
  return iStat;
}
//=============================================================================
dREC03RECNUMTYP sstRec03InternCls::DSiTreInsert( int                     iKey,
                                                 sstRec03TreeHeaderCls  *oTre,
                                                 dREC03RECNUMTYP         DsNrAlt,
                                                 dREC03RECNUMTYP         DsNrNeu,
                                                 void                   *Adr1)
//.............................................................................
{

  void        *LocDs = NULL;       // adress of local data memory
  sstRec03TreeNodeCls *Baum2;      // Tree node data in record 2
  void        *Adr2 = NULL;        // Compare value 2
  int          AltGroesser = 0;

  int   iStat = 0;
//.............................................................................

  if (iKey != 0) return -1;

  if ( DsNrAlt == 0)  // exist no stored record
  {
    DsNrAlt = DsNrNeu;
  }
  else
  {

    // LocDs = malloc ( this->oVector->GetSize());
    LocDs = calloc ( this->poVector->GetSize(),1);
    assert(LocDs);

    // Read record from RecMem into local memory
    iStat = this->ReadInt( 0, DsNrAlt, LocDs);

    // Adressen der Baumstrukturen in den Zwischenspeichern ermitteln
    // Get Adress from tree node data in local memory
    // TreVerw = (sstRec03TreeHeaderCls*) this->Tre;
    this->poVector->CalcSetPos(LocDs, &Adr2, oTre->iAdrOfs);
    Baum2 = (sstRec03TreeNodeCls*) Adr2;

    // Get Adress of compare value in local memory
    int iUsrOfs = 0;
    this->poVector->GetOffset(0,this->poRecMemUsrKey,&iUsrOfs);
    this->poVector->CalcSetPos(LocDs, &Adr2, (iUsrOfs + oTre->iOffset));

    // Is true, if Compare Value Adr2 is greater than Adr1
    AltGroesser = DSiVarCompGT ( 0, &oTre->eTyp, Adr2, Adr1);

    if ( AltGroesser)
    {
      Baum2->dLeft_LT = DSiTreInsert ( 0, oTre, Baum2->dLeft_LT , DsNrNeu, Adr1);
    }
    else
    {
      Baum2->dRight_GE = DSiTreInsert ( 0, oTre, Baum2->dRight_GE, DsNrNeu, Adr1);
    }

    // Write record in local memory back to RecMem
    iStat = this->WritInt( 0, LocDs, DsNrAlt);

    free ( LocDs);

  }
  // Fatal Errors goes to an assert
  if (iStat < 0)
  {
    // Expression (iRet >= 0) has to be fullfilled
    assert(0);
  }

  return DsNrAlt;
}
//=============================================================================
int sstRec03InternCls::TreReadNxtGE (int              iKey,
                                    sstRec03TreeKeyCls   *oTre,
                                    void            *DSatz,
                                    dREC03RECNUMTYP *SNr)
//-----------------------------------------------------------------------------
{
  dREC03RECNUMTYP SNr1 = 0;
  dREC03RECNUMTYP SNr2 = 0;
  int iRet  = 0;
  int iStat = 0;
//-----------------------------------------------------------------------------
  if ( iKey != 0) return -1;

  SNr1 = *SNr;

  if (SNr1 == 0)
  {
    // Get record with smallest value from tree oTre
    iStat = TreSeaFrst ( 0, oTre, &SNr2);
  }
  else
  {
    // Get for record SNr1 next greater record number in tree oTre
    iStat = TreSeaNxtGT ( 0, oTre, SNr1, &SNr2);
  }

  // Datensatz an absoluter Position lesen.
  // Read record SNr from RecMem
  *SNr = SNr2;
  iStat = this->ReadVector(0,*SNr,DSatz);

// Heavy Errors goes to an assert
  if (iRet < 0)
  {
    // Expression (iRet >= 0) has to be fullfilled
    assert("Error in function: xxx");
  }

  // Small Errors will given back
  iRet = iStat;

  return iRet;
}
//=============================================================================
int sstRec03InternCls::TreSeaFrst ( int        iKey,
                             sstRec03TreeKeyCls    *oTre,
                             dREC03RECNUMTYP  *SNr)
//-----------------------------------------------------------------------------
{
  dREC03RECNUMTYP      IntSNr = 0;        // Local Record number
  void         *IntSAdr;                  // Local record memory
  sstRec03TreeNodeCls     SBaum;              // Local tree node object

  int iRet;
  int iStat;
//.............................................................................
  if (iKey != 0) return -1;
  iRet = 0;
  // iStat = 0;

  // Is RecMem empty, no Record number is result: Error
  if (this->dActStored <= 0)
  {
    *SNr = 0;
    return -2;
  }

  // Is only one record in RecMem, this is result
  if (this->dActStored == 1)
  {
    *SNr = 1;
    return 0;
  }

  // Get Entry record number in Tree oTre
  // This is first result number
  IntSNr = this->poTre->dRoot;

  // Start local record memory
  IntSAdr = malloc(this->poVector->GetSize());
  assert(IntSAdr);

  SBaum.dLeft_LT = IntSNr;

  while ( SBaum.dLeft_LT > 0)
  {
    IntSNr = SBaum.dLeft_LT;
    *SNr   = IntSNr;
    // Return tree node data for record IntSNr of tree oTre
    iStat = DSiTreDatGet ( 0, oTre, IntSNr, IntSAdr,  &SBaum);
  }

  free (IntSAdr);

  // Fatal Errors goes to an assert
  if (iRet < 0)
  {
    // Expression (iRet >= 0) has to be fullfilled
    assert(0);
  }

  // Small Errors will given back
  iRet = iStat;

  return iRet;
}
//=============================================================================
int sstRec03InternCls::TreSeaNxtGT ( int               eKey,
                                     sstRec03TreeKeyCls    *oTre,
                                     dREC03RECNUMTYP   SNr1,
                                     dREC03RECNUMTYP  *SNr2)
{
  dREC03RECNUMTYP  SNr = 0;

  int iKey;
  int iStat;
//.............................................................................
  if ( eKey != 0) return -1;
  if ( SNr1 == 0) return -2;

  // Get Base of Tree
  SNr = this->poTre[oTre->iTriNo-1].dRoot;

  *SNr2 = 0;  // Default is Result empty

  // In Tree oTre for Record 1 seach next greater Record 2
  iKey  = 0;
  iStat = DSiTreSeaNxtGT ( &iKey, oTre, SNr, SNr1, SNr2);

  return iStat;
}
//=============================================================================
int sstRec03InternCls::DSiVarCompGT (int               iKey,
                                     sstRec03CompTyp_enum *cTyp,
                                     void             *AdrAlt,
                                     void             *AdrNeu)
//-----------------------------------------------------------------------------
{
  sstRec03CompValueCls ValSet;
  sstRec03CompValueCls CompValSet;

  int AltGroesser;
//-----------------------------------------------------------------------------
  if ( iKey != 0) return -1;

  // Set type and adress for compare value in ValSet
  DSiCompValue ( 0, AdrAlt, cTyp, &ValSet);
  // Set type and adress for compare value in CompValSet
  DSiCompValue ( 0, AdrNeu, cTyp, &CompValSet);

    AltGroesser = 0;
    switch (*cTyp)
    {
      // Comp2 ist größer Comp1
      case sstRecTyp_CC:  // Char
        if ( strcmp( ValSet.CComp, CompValSet.CComp) > 0 ) AltGroesser = 1;
        break;
      case sstRecTyp_I2:  // Int
        if ( *ValSet.IComp > *CompValSet.IComp) AltGroesser = 1;
        break;
      case sstRecTyp_I4:  // Long Int
        if ( *ValSet.LComp > *CompValSet.LComp) AltGroesser = 1;
        break;
      case sstRecTyp_UI:  // Unsigned Int
        if ( *ValSet.UIComp > *CompValSet.UIComp) AltGroesser = 1;
        break;
      case sstRecTyp_UL:  // Unsigned Long Int
        if ( *ValSet.ULComp > *CompValSet.ULComp) AltGroesser = 1;
        break;
      case sstRecTyp_R4:  // float
        if ( *ValSet.RComp > *CompValSet.RComp) AltGroesser = 1;
        break;
      case sstRecTyp_D8:  // Double
        if ( *ValSet.DComp > *CompValSet.DComp) AltGroesser = 1;
        break;
      default:
        assert(0);
        break;
    }

  return AltGroesser;
}
//=============================================================================
int sstRec03InternCls::DSiCompValue (int                 iKey,
                                     void               *ValueAdr,
                                     sstRec03CompTyp_enum   *CompTyp,
                                     sstRec03CompValueCls       *CompValue)
//-----------------------------------------------------------------------------
{
  int iRet  = 0;
  int iStat = 0;
//-----------------------------------------------------------------------------
  if ( iKey != 0) return -1;

  // reset all compare type values
  memset(CompValue,0,sizeof(sstRec03CompValueCls));

  switch (*CompTyp)
  {
    case sstRecTyp_CC:  // Char
      CompValue->CComp = (char*) ValueAdr;
      break;
    case sstRecTyp_I2:  // Int
      CompValue->IComp = (int*) ValueAdr;
      break;
    case sstRecTyp_I4:   // Long Int
      CompValue->LComp = (long*) ValueAdr;
      break;
    case sstRecTyp_UI:  // Unsigned Int
      CompValue->UIComp = (unsigned int*) ValueAdr;
      break;
    case sstRecTyp_UL:   // Unsigned Long Int
      CompValue->ULComp = (unsigned long*) ValueAdr;
      break;
    case sstRecTyp_R4:   // Float
      CompValue->RComp = (float*) ValueAdr;
      break;
    case sstRecTyp_D8:   // Double
      CompValue->DComp = (double*) ValueAdr;
      break;
    default:
      iRet = -1;
      break;
  }
    // Heavy Errors goes to an assert
  if (iRet < 0)
  {
    // Expression (iRet >= 0) has to be fullfilled
    assert("Error in DSi_CompValue: Cast-Error");
  }

  // Small Errors will given back
  iRet = iStat;

  return iRet;
}
//=============================================================================
int sstRec03InternCls::DSiTreSeaNxtGT ( int                *iKey,
                                        sstRec03TreeKeyCls      *oTre,
                                        dREC03RECNUMTYP     SNr,
                                        dREC03RECNUMTYP     SNr1,
                                        dREC03RECNUMTYP    *SNr2)
{
  void       *SNr_Adr;      // Local record in heap
  sstRec03TreeNodeCls   SBaum;  // local tree node object

  int iRet = 0;
  int iStat = 0;
//.............................................................................
  if ( *iKey < 0 || *iKey > 2) return -1;
  if ( *iKey == 2) return 0;

  if ( SNr != 0)  // if not empty
  {
    // Get heap memory for compare record
    SNr_Adr = malloc ( this->poVector->GetSize());
    assert(SNr_Adr);

    // Read record dRecNo into SNr_Adr Memory and return tree node data of tree oTre
    iStat = DSiTreDatGet ( 0, oTre, SNr, SNr_Adr,  &SBaum);

    // Traverse in Tree oTre to next greater record
    iStat = DSiTreSeaNxtGT ( iKey, oTre,  SBaum.dLeft_LT, SNr1, SNr2);

    // Action !!
    // the later action first, so this does not run immedialety
    if ( *iKey == 1)
    {
      *SNr2 = SNr;
      *iKey = 2;
    }

    if ( SNr == SNr1)
    {
      *iKey = 1;
    }

    // Traverse in Tree oTre to next greater record
    iStat = DSiTreSeaNxtGT ( iKey, oTre,  SBaum.dRight_GE, SNr1, SNr2);

    // free heap for local record
    free ( SNr_Adr);
}

  // Fatal Errors goes to an assert
  if (iRet < 0)
  {
    // Expression (iRet >= 0) has to be fullfilled
    assert(0);
  }

  // Small Errors will given back
  iRet = iStat;

  return iRet;
}
//=============================================================================
int sstRec03InternCls::DSiTreDatGet ( int                iKey,
                                      sstRec03TreeKeyCls     *oTre,
                                      dREC03RECNUMTYP    SNr,
                                      void              *DSatz,
                                      sstRec03TreeNodeCls   *TreDat)
//.............................................................................
{
  int iRet = 0;
  int iStat = 0;
//.............................................................................
  if (iKey != 0) return -1;

  // istat = 0;
  if (SNr <= 0) return -1;
  if (SNr > this->dActStored) return -1;

  // Read record from RecMem into given memory
  iStat = this->ReadInt(0,SNr,DSatz);

  // Get Tree node data from read record
  iStat = DSiTreAdrGet ( 0 , oTre, DSatz, TreDat);

  // Fatal Errors goes to an assert
  if (iRet < 0)
  {
    // Expression (iRet >= 0) has to be fullfilled
    assert(0);
  }

  // Small Errors will given back
  iRet = iStat;

  return iRet;
}
//=============================================================================
int sstRec03InternCls::DSiTreDatSet ( int                iKey,
                                      sstRec03TreeKeyCls     *oTre,
                                      dREC03RECNUMTYP    SNr,
                                      void              *DSatz,
                                      sstRec03TreeNodeCls   *TreDat)
{
  int iRet = 0;
  int iStat = 0;
//.............................................................................
  if (iKey != 0) return -1;

  if (SNr <= 0) return -1;
  if (SNr > this->dActStored) return -1;

  // Write tree node data into given record
  iStat = DSiTreAdrSet ( 0 , oTre, TreDat);

  // Write given record into RecMem at position
  iStat = this->WritInt(0,DSatz,SNr);

  // Fatal Errors goes to an assert
  if (iRet < 0)
  {
    // Expression (iRet >= 0) has to be fullfilled
    assert(0);
  }

  // Small Errors will given back
  iRet = iStat;

  return iRet;
}
//=============================================================================
int sstRec03InternCls::DSiTreAdrGet ( int                 iKey,
                                      sstRec03TreeKeyCls      *oTre,
                                      void               *AktDs,
                                      sstRec03TreeNodeCls    *RetBaum)
//.............................................................................
{
  sstRec03TreeHeaderCls         *TreVerw;
  sstRec03TreeNodeCls    *Baum;
  void               *BaumAdr;

  int iret;
  // int istat;
//.............................................................................
  if (iKey != 0) return -1;
  iret = 0;

  // Get tree adress in tree array
  TreVerw = (sstRec03TreeHeaderCls*) this->poTre;

  int iTreAdrOfs = 0;
  this->poVector->GetOffset(0,TreVerw[oTre->iTriNo-1].poDataKey,&iTreAdrOfs);
  CalcSetPos ( AktDs, &BaumAdr, iTreAdrOfs);

  // Adr are tree node data
  Baum = (sstRec03TreeNodeCls*) BaumAdr;

  // Copy Tree Data from vector to tree data
  *RetBaum = *Baum;

  return iret;

}
//=============================================================================
int sstRec03InternCls::DSiTreAdrSet ( int               iKey,
                                      sstRec03TreeKeyCls    *oTre,
                                      sstRec03TreeNodeCls  *SetBaum)
{
  sstRec03TreeHeaderCls  *TreVerw;

  int iret;
//.............................................................................
  if (iKey != 0) return -1;
  iret = 0;

  // Get tree adr in tree array
  TreVerw = (sstRec03TreeHeaderCls*) this->poTre;

  this->poVector->WrtCargo ( 0, TreVerw[oTre->iTriNo-1].poDataKey, SetBaum);

  return iret;
}
//=============================================================================
void sstRec03InternCls::CalcSetPos ( void   *BasPtr,
                                     void  **IdxPtr,
                                     long    Offs) const
//-----------------------------------------------------------------------------
{
  unsigned char *xxx;  // smallest possible type

//.............................................................................
  // Calculate pointer offset
  xxx = (unsigned char*) BasPtr;
  xxx = xxx + Offs;
  (*IdxPtr) = (void*) xxx;
}
//=============================================================================
int sstRec03InternCls::TreWriteNew ( int              iKey,
                                     void            *vRecAdr,
                                     dREC03RECNUMTYP *dRecNo)
//.............................................................................
{
  dREC03RECNUMTYP   DsNr;
  void             *vCompValueAdr;
  int               iTreNo;
  dREC03RECNUMTYP   dRoot;

  int iStat = 0;
//.............................................................................
  if (iKey != 0) return -1;

  // write new record into record memory thru vector buffer
  iStat = this->WritNewVector( 0, vRecAdr, &DsNr);

  assert(DsNr > 0);
  assert(iStat >= 0);

  // Get Adress of user data in vector buffer
  void *vUsrRecAdr = NULL;
  iStat = this->poVector->GetCargoAdr( 0, this->poRecMemUsrKey, &vUsrRecAdr);

  // Update new record in all trees
  for(iTreNo=1; iTreNo <= this->iTriAnz; iTreNo++)
  {
    if (DsNr == 1) dRoot = 0;
    else dRoot = 1;

    // Get Adress of compare value in user data of vector
    this->poVector->CalcSetPos( vUsrRecAdr, &vCompValueAdr, this->poTre[iTreNo-1].iOffset);

    // Insert new record in tree
    dRoot = this->DSiTreInsert( 0, &this->poTre[iTreNo-1], dRoot , DsNr, vCompValueAdr);
  }

  *dRecNo = this->dActStored;

  return iStat;
}
//=============================================================================
