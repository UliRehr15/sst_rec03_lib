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
// sstRec03LibInt.h   17.10.15  Re.   17.10.15  Re.
//
// Intern Datastructures and Prototypes for system "sstRec03Lib"
//

#ifndef   _SST_REC03_LIB_INT_HEADER
#define   _SST_REC03_LIB_INT_HEADER

/**
 * @defgroup sstRecord03InternLib sstRecord03InternLib: Intern cpp sst record library
 *
 * Intern cpp sst record library <BR>
 *
 */


// Defines ---------------------------------------------------------------------

/**
 * @brief Maximal File Name Length
 * @ingroup sstRecord03InternLib
 */
#define dREC03FILNAMMAXLEN 260   /**< Maximal File Name Length    */


// Structures and Classes ------------------------------------------------------

//==============================================================================
/**
* @brief sstRecMem Internal Header Class with System Data for every record <BR>
*
* With Delete Flag, Mark Flag, Version String, New Date and so on. <BR>
* Stored as cargo class inside record.
*
* Changed: 12.10.15  Re.
*
* @ingroup sstRecord03InternLib
*
* @author Re.
*
* @date 12.10.15
*/
// ----------------------------------------------------------------------------
class sstRec03HeaderCls
{
  public:   // Public functions
     sstRec03HeaderCls();  // Constructor
     //==============================================================================
     /**
     * @brief // Get Size of full record size with all cargo packets <BR>
     * dRecordSize = oRemMemHeader.getRecSize();
     *
     * @retval  Record Size
     */
     // ----------------------------------------------------------------------------
     dREC03RECSIZTYP getRecSize() const;
     //==============================================================================
     /**
     * @brief // Set Size of full Record with all cargo packets <BR>
     * oRecMem.setRecSize( dRecSize);
     *
     * @param dRecSize [in] Record Size
     *
     */
     // ----------------------------------------------------------------------------
     void setRecSize(const dREC03RECSIZTYP &dRecSize);
     //==============================================================================
     /**
     * @brief return adress of version string
     *
     * @return adress of version string
     */
     // ----------------------------------------------------------------------------
     char* GetVersStr();
     //==============================================================================
     /**
     * @brief set version string
     *
     * @param cNam [in] adress of version string
     */
     // ----------------------------------------------------------------------------
     void SetVersStr(char *cNam);
     //==============================================================================
     /**
     * @brief set version string
     */
     // ----------------------------------------------------------------------------
     void SetNewDate();
     //==============================================================================
     /**
     * @brief set version string
     */
     // ----------------------------------------------------------------------------
     void SetChangeDate();
     //==============================================================================
     /**
     * @brief set record deleted
     */
     // ----------------------------------------------------------------------------
     void RecSetDeleted();
     //==============================================================================
     /**
     * @brief set record marked
     */
     // ----------------------------------------------------------------------------
     void RecSetMarked();
     //==============================================================================
     /**
     * @brief set record undeleted
     */
     // ----------------------------------------------------------------------------
     void RecSetUndeleted();
     //==============================================================================
     /**
     * @brief set record unmarked
     */
     // ----------------------------------------------------------------------------
     void RecSetUnmarked();
     //==============================================================================
     /**
     * @brief get record Delete Status
     */
     // ----------------------------------------------------------------------------
     bool RecGetDeleteStatus();
     //==============================================================================
     /**
     * @brief get record Mark Status
     */
     // ----------------------------------------------------------------------------
     bool RecGetMarkStatus();


private:  // Private functions
     char cVersionstring[10];    /**< Version String, for exampe sstRec02 */
     dREC03RECSIZTYP dRecSize;  /**< Size of every Record */
     char cRecChgDateTime[18];    /**< Write Change Date, for exampe 151012 */
     char cRecNewDateTime[18];    /**< Write New Date, for exampe 151012 */
     bool bDel;    /**< Delete Flag */
     bool bMark;   /**< Mark Flag */
};
//==============================================================================
/**
* @brief System data for every cargo packet
*
* More Comment
*
* Changed: 19.02.10  Re.
*
* @ingroup sstRecord03InternLib
*
* @author Re.
*
* @date 19.02.10
*/
// ----------------------------------------------------------------------------
class sstRec03CargoMemCls
{
  public:   // Öffentliche Funktionen
     sstRec03CargoMemCls();  // Konstruktor
     ~sstRec03CargoMemCls();  // Destruktor
     //==============================================================================
     /**
     * @brief return Offset
     *
     * @return Offset
     */
     // ----------------------------------------------------------------------------
     int GetOffset();
     //==============================================================================
     /**
     * @brief Set Offset
     *
     * @param iOffset [in] Offset
     */
     // ----------------------------------------------------------------------------
     void SetOffset(int iOffset);
     //==============================================================================
     /**
     * @brief return Cargo Size
     *
     * @return Cargo Size
     */
     // ----------------------------------------------------------------------------
     int GetCargoSize();
     //==============================================================================
     /**
     * @brief Set Cargo Size
     *
     * @param iSize [in] Cargo Size
     */
     // ----------------------------------------------------------------------------
     void SetCargoSize(int iSize);
     //==============================================================================
     /**
     * @brief return adress of cargo name char
     *
     * @return adress of cargo name char
     */
     // ----------------------------------------------------------------------------
     char* GetCargoSysNam();
     //==============================================================================
     /**
     * @brief set cargo name
     *
     * @param cNam [in] adress of cargo name char
     */
     // ----------------------------------------------------------------------------
     void SetCargoSysNam(char *cNam);
     //==============================================================================
     /**
     * @brief get cargo adress
     *
     * @return cargo adress
     */
     // ----------------------------------------------------------------------------
     void* GetCargoAdr();
     //==============================================================================
     /**
     * @brief set cargo adress
     *
     * @param vCargoAdr [in] adress of cargo
     */
     // ----------------------------------------------------------------------------
     void SetCargoAdr(void *vCargoAdr);
     //==============================================================================
  private:  // Private Funktionen
     int iOffset;           /**< Offset from Start of vector */
     int iSize;             /**< Size of cargo sys*/
     char cNam[4];          /**< Name of cargo sys */
     void *vCargoAdr;        /**< Adress in vector memory */
};


//==============================================================================
/**
* @brief vector storage with all defined cargo packets
*
* More Comment
*
* Changed: 01.10.15  Re.
*
* @ingroup sstRecord03InternLib
*
* @author Re.
*
* @date 01.10.15
*/
// ----------------------------------------------------------------------------
class sstRec03VectSysCls
{
  public:   // Öffentliche Funktionen
    //=============================================================================
    /**
    * @brief constructor for vector memory
    *
    * @return Errorstate
    *
    * @retval   = 0: OK
    * @retval   < 0: Unspecified Error
    */
    // ----------------------------------------------------------------------------
     sstRec03VectSysCls();
     ~sstRec03VectSysCls();
     //=============================================================================
     /**
     * @brief // Add cargo system to vector memory and get new identification key <BR>
     * iStat = oVector.AddCargoSys( iKey, uiSize, *cCargoNam, *oCargoKey);
     *
     * @param iKey      [in]  For the moment 0
     * @param uiSize    [in]  Size of cargo record
     * @param cCargoNam [in]  Name of cargo system (4 Letters)
     * @param oCargoKey [out] key of new cargo object
     *
     * @return Errorstate
     *
     * @retval   =  0: OK
     * @retval   = -1: Wrong Key
     * @retval   = -2: Cargo Packet must not be empty
     * @retval   = -3: Cargo Name has to be length 3
     * @retval   = -4: Cargo Key should be empty
     * @retval   = -5: Cargo Name should be unique
     * @retval   <  0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int AddCargoSys( int             iKey,
                      unsigned int    uiSize,
                      char           *cCargoNam,
                      sstRec03CargoKeyInternCls *oCargoKey);
     //=============================================================================
     /**
     * @brief write record to vector Memory
     *
     * @param iKey      [in]  For the moment 0
     * @param oDataKey  [in]  adress of cargo record
     * @param vCargoAdr [in]  adress of cargo record
     *
     * @return Errorstate
     *
     * @retval   =  0: OK
     * @retval   = -1: Wrong Key
     * @retval   = -2: Cargo key not OK for RecMem object
     * @retval    < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int WrtCargo ( int              iKey,
                    sstRec03CargoKeyInternCls *oDataKey,
                    void            *vCargoAdr);

     //=============================================================================
     /**
     * @brief Read record from vector memory
     *
     * @param iKey      [in]  For the moment 0
     * @param oDataKey  [in]  Adress of cargo record
     * @param vCargoAdr [in]  Adress of cargo record
     *
     * @return Errorstate
     *
     * @retval   =  0: OK
     * @retval   = -1: Wrong Key
     * @retval   = -2: Cargo key not OK for RecMem object
     * @retval   <  0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int RedCargo ( int              iKey,
                    sstRec03CargoKeyInternCls *oDataKey,
                    void            *vCargoAdr);

     //=============================================================================
     /**
     * @brief Calculate new position in memory
     *
     * @param BasPtr [in]  Basis pointer
     * @param IdxPtr [out] Result pointer = Basis + offset
     * @param Offs   [in]  Offset
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     void CalcSetPos ( void   *BasPtr,
                       void  **IdxPtr,
                       long    Offs);

     //=============================================================================
     /**
     * @brief Get size of vector
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     unsigned long GetSize() const;

     //=============================================================================
     /**
     * @brief Get adress of vector
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     void* GetAdr() const;
     //=============================================================================
     /**
     * @brief Get Adress of cargo system
     *
     * @param iKey      [in]     For the moment 0
     * @param oDataKey  [in out] Key of cargo object
     * @param vCargoAdr [out]    Adress of cargo system
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int GetCargoAdr (int               iKey,
                      sstRec03CargoKeyInternCls  *oDataKey,
                      void            **vCargoAdr);
     //=============================================================================
     /**
     * @brief Get offset of cargo system
     *
     * @param iKey     [in]     For the moment 0
     * @param oDataKey [in out] Key of cargo object
     * @param iOffset  [out]    offset of cargo object
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int GetOffset (int                iKey,
                    sstRec03CargoKeyInternCls   *oDataKey,
                    int               *iOffset);
     //=============================================================================
     /**
     * @brief reset memory of vector
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     void ResetMem(int iKey);

  private:
    unsigned int              uiNumCargoSys;  /**< Number of Cargo Systems */
    sstRec03CargoMemCls *poMemAdr;        /**< Field of of Cargo Systems */
    void            *vVectorAdr;     /**< main memory vector with all cargo values */
    dREC03RECSIZTYP    dVectorSize;  /**< size of main memory vector */
};
//==============================================================================
/**
* @brief Intern key object for cargo packet
*
* More Comment
*
* Changed: 19.02.10  Re.
*
* @ingroup sstRecord03InternLib
*
* @author Re.
*
* @date 19.02.10
*/
//==============================================================================
class sstRec03CargoKeyInternCls
{
  public:   // Öffentliche Funktionen
     sstRec03CargoKeyInternCls();  // Konstruktor
     ~sstRec03CargoKeyInternCls();  // Destruktor
     int  iKey;           /**< Key of cargo sys */
     char cNam[4];        /**< Name of cargo sys*/
     sstRec03VectSysCls *poCargoAdr;  /**< Adress of cargo object  */
  private:  // Private Funktionen
     // int iDummy;
};
//==============================================================================
/**
* @brief intern sstRecMem class
*
* Intern class for sstRecordLib <BR>
*
* Changed: 04.08.15  Re.
*
* @ingroup sstRecord03InternLib
*
* @author Re.
*
* @date 09.07.15
*/
// ----------------------------------------------------------------------------
class sstRec03InternCls
{
  public:   // Public functions
    //==============================================================================
    /**
    * @brief constructor with size of one record
    *
    * @param Size [in] Size of one record
    */
    // ----------------------------------------------------------------------------
     sstRec03InternCls(dREC03RECSIZTYP Size);  // Constructor
    ~sstRec03InternCls();  // Destructor
     //==============================================================================
     /**
     * @brief // Write new record into intern sstRec memory <BR>
     * iStat = oRecMem.WritNewInt( iKey, vRecAdr, dRecNo);
     *
     * @param iKey    [in]  For the moment 0
     * @param vRecAdr [in]  Record to store
     * @param dRecNo  [out] New index number
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int WritNewInt(int iKey, void* vRecAdr, dREC03RECNUMTYP *dRecNo);
     //==============================================================================
     /**
     * @brief // Write new record with sstRec vector <BR>
     * iStat = oRecMem.WritNewVector( iKey, vRecAdr, dRecNo);
     *
     * @param iKey    [in]  For the moment 0
     * @param vRecAdr [in]  Record to store
     * @param dRecNo  [out] New index number
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int WritNewVector(int iKey, void* vRecAdr, dREC03RECNUMTYP *dRecNo);
     //==============================================================================
     /**
     * @brief // Write Record at position in intern sstRec Memory  <BR>
     * iStat = oRecMem.WritInt( iKey, *vRecAdr,  dRecNo);
     *
     * @param iKey    [in] For the moment 0
     * @param vRecAdr [in] Adress of record
     * @param dRecNo  [in] target position to write record
     *
     * @return Errorstate
     *
     * @retval   =  0: OK
     * @retval   = -1: Wrong Key
     * @retval   = -2: Wrong Record positon
     * @retval   <  0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int WritInt(int iKey, void* vRecAdr, dREC03RECNUMTYP dRecNo);
     //==============================================================================
     /**
     * @brief // Write Record at position with sstRec vector  <BR>
     * iStat = oRecMem.WritVector( iKey, *vRecAdr, dRecNo);
     *
     * @param iKey    [in] For the moment 0
     * @param vRecAdr [in] Adress of record
     * @param dRecNo  [in] target position to write record
     *
     * @return Errorstate
     *
     * @retval   =  0: OK
     * @retval   = -1: Wrong Key
     * @retval   = -2: Wrong Record positon
     * @retval   <  0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int WritVector(int iKey, void* vRecAdr, dREC03RECNUMTYP dRecNo);
     //==============================================================================
     /**
     * @brief // Read record from sstRec memory with Record number <BR>
     * iStat = oRecMem.ReadInt( iKey, dRecNo, *vRecAdr);
     *
     * @param iKey    [in] For the moment 0
     * @param dRecNo  [in] record number to read
     * @param vRecAdr [out read record
     *
     * @return Errorstate
     *
     * @retval   =  0: OK
     * @retval   = -1: Wrong Key
     * @retval   = -2: Wrong Record positon
     * @retval   <  0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int ReadInt(int iKey, dREC03RECNUMTYP dRecNo, void *vRecAdr);
     //==============================================================================
     /**
     * @brief // Read record from sstRec memory with Record number and vector <BR>
     * iStat = oRecMem.ReadVector( iKey, dRecNo, *vRecAdr);
     *
     * @param iKey    [in] For the moment 0
     * @param dRecNo  [in] record number to read
     * @param vRecAdr [out read record
     *
     * @return Errorstate
     *
     * @retval   =  0: OK
     * @retval   = -1: Wrong Key
     * @retval   = -2: Wrong Record positon
     * @retval   <  0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int ReadVector(int iKey, dREC03RECNUMTYP dRecNo, void *vRecAdr);
     //==============================================================================
     /**
     * @brief return number of stored records in sstRec memory
     *
     * @return number of records, which are stored
     */
     // ----------------------------------------------------------------------------
     dREC03RECNUMTYP count();
     //==============================================================================
     /**
     * @brief Open existing or new file of record storing
     *
     * @param iKey    [in]  For the moment 0
     * @param cSysNam [in]  File Name
     *
     * @return Errorstate
     *
     * @retval   =  0: OK
     * @retval   = -1: Wrong Key
     * @retval   = -2: File already open
     * @retval   = -3: Filename empty
     * @retval   = -4: sstRec not empty
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int OpenFile(int   iKey,
                  char *cSysNam);
     //==============================================================================
     /**
     * @brief Open new file of record storing
     *
     * @param iKey    [in]  For the moment 0
     * @param cSysNam [in]  File Name
     *
     * @return Errorstate
     *
     * @retval   =  0: OK
     * @retval   = -1: Wrong Key
     * @retval   = -2: File already open
     * @retval   = -3: Filename empty
     * @retval   = -4: sstRec not empty
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int NewFile(int   iKey,
                 char *cSysNam);
     //==============================================================================
     /**
     * @brief Store record data in file
     *
     * @param iKey  [in]  For the moment 0
     *
     * @return Errorstate
     *
     * @retval   =  0: OK
     * @retval   = -1: Wrong Key
     * @retval   = -2: No File open
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int SetStoreFile(int iKey);
     //==============================================================================
     /**
     * @brief // Add cargo system to sstRec memory and get new identification key <BR>
     * iStat = oRecMem.AddCargoSys( iKey, uiSize, *cCargoNam, *oCargoKey);
     *
     * @param iKey      [in]  For the moment 0
     * @param uiSize    [in]  Size of cargo record
     * @param cCargoNam [in]  Name of cargo system (4 Letters)
     * @param oCargoKey [out] key of new cargo object
     *
     * @return Errorstate
     *
     * @retval   =   0: OK
     * @retval   =  -1: Wrong Key
     * @retval   =  -2: Cargo Packet must not be empty
     * @retval   =  -3: Cargo Name has to be length 3
     * @retval   =  -4: Cargo Key should be empty
     * @retval   =  -5: Cargo Name should be unique
     * @retval   = -10: sstRecMem is not empty
     * @retval   <   0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int AddCargoSys( int                  iKey,
                      unsigned int         uiSize,
                      char                *cCargoNam,
                      sstRec03CargoKeyInternCls *oCargoKey);
     //=============================================================================
     /**
     * @brief write record to vector Memory
     *
     * @param iKey      [in]  For the moment 0
     * @param oDataKey  [in]  adress of cargo record
     * @param vCargoAdr [in]  adress of cargo record
     *
     * @return Errorstate
     *
     * @retval   =  0: OK
     * @retval   = -1: Wrong Key
     * @retval   = -2: Cargo key not OK for RecMem object
     * @retval   <  0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int WrtCargo ( int              iKey,
                    sstRec03CargoKeyInternCls *oDataKey,
                    void            *vCargoAdr);
     //=============================================================================
     /**
     * @brief Read record from vector memory
     *
     * @param iKey      [in]  For the moment 0
     * @param oDataKey  [in]  Identification Key
     * @param vCargoAdr [in]  Adress of cargo record
     *
     * @return Errorstate
     *
     * @retval   =  0: OK
     * @retval   = -1: Wrong Key
     * @retval   = -2: Cargo key not OK for RecMem object
     * @retval   <  0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int RedCargo ( int              iKey,
                    sstRec03CargoKeyInternCls *oDataKey,
                    void            *vCargoAdr);
     //==============================================================================
     /**
     * @brief set date/time in new string of header object
     */
     // ----------------------------------------------------------------------------
     void SetNewDate();
     //==============================================================================
     /**
     * @brief set date/time in change string of header object
     */
     // ----------------------------------------------------------------------------
     void SetChangeDate();
     //==============================================================================
     /**
     * @brief // Set record state as deleted  for record dRecNo in RecMem  <BR>
     * iStat = oRecMem.RecSetDeleted ( iKey,  RecNo);
     *
     * @param iKey   [in]  For the moment 0
     * @param dRecNo [in]  record number
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int RecSetDeleted ( int               iKey,
                         dREC03RECNUMTYP   dRecNo);
     //==============================================================================
     /**
     * @brief // Set record state as marked  for record dRecNo in RecMem  <BR>
     * iStat = oRecMem.RecSetMarked ( iKey,  RecNo);
     *
     * @param iKey   [in]  For the moment 0
     * @param dRecNo [in]  record number
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int RecSetMarked( int               iKey,
                        dREC03RECNUMTYP   dRecNo);
     //==============================================================================
     /**
     * @brief // Set record state as undeleted  for record dRecNo in RecMem  <BR>
     * iStat = oRecMem.RecSetUndeleted ( iKey,  RecNo);
     *
     * @param iKey   [in]  For the moment 0
     * @param dRecNo [in]  record number
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int RecSetUndeleted( int               iKey,
                           dREC03RECNUMTYP   dRecNo);
     //==============================================================================
     /**
     * @brief // Set record state as unmarked for record dRecNo in RecMem  <BR>
     * iStat = oRecMem.RecSetUnmarked ( iKey,  RecNo);
     *
     * @param iKey   [in]  For the moment 0
     * @param dRecNo [in]  record number
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
     int RecSetUnmarked( int               iKey,
                          dREC03RECNUMTYP   dRecNo);
     //==============================================================================
     /**
     * @brief // Get delete state of record dRecNo  <BR>
     * iStat = oRecMem.RecGetDeleteState ( iKey,  RecNo);
     *
     * @param iKey   [in]  For the moment 0
     * @param dRecNo [in]  record number
     *
     * @return Delete State
     *
     * @retval   = true:  Record is deleted
     * @retval   = false: Record is undeleted
     */
     // ----------------------------------------------------------------------------
     bool RecGetDeleteStatus( int               iKey,
                              dREC03RECNUMTYP   dRecNo);
     //==============================================================================
     /**
     * @brief // Get mark state of record dRecNo <BR>
     * iStat = oRecMem.RecGetMarkState ( iKey,  RecNo);
     *
     * @param iKey   [in]  For the moment 0
     * @param dRecNo [in]  record number
     *
     * @return Mark State
     *
     * @retval   = true : Record is marked
     * @retval   = false: Record is unmarked
     */
     // ----------------------------------------------------------------------------
     bool RecGetMarkStatus( int               iKey,
                            dREC03RECNUMTYP   dRecNo);
     //==============================================================================

  private:  // Private functions
     //==============================================================================
     /**
     * @brief Shortstory
     *
     * @param iKey [in] For the moment 0
     *
     * @return Errorstate
     *
     * @retval   = 0: OK
     * @retval   < 0: Unspecified Error
     */
     // ----------------------------------------------------------------------------
    void inflate(int increase);
    //==============================================================================
    // dREC03RECSIZTYP dUsrSize;     /**< Size of each user record */
    dREC03RECNUMTYP dQuantity;     /**< Number of storage spaces */
    dREC03RECNUMTYP dActStored;   /**< Number of stored records */
    unsigned char* ucStorage;       /**< Dynamically allocated array of bytes */
    FILE*          pFilHdl;        /**< File Handle: If not NULL, store in file   */
    bool           bFileNotDelete;  /**< Do File not delete   */
    char cDatnam[dREC03FILNAMMAXLEN]; /**< Filename for storing record data   */
    sstRec03HeaderCls *poHeader;  /**< Intern Header            */
    sstRec03VectSysCls *poVector;  /**< Intern memory space for vector            */
    sstRec03CargoKeyInternCls *poRecMemUsrKey;   /**< Identification Key for Header Cargo */
    sstRec03CargoKeyInternCls *poRecMemSysKey;   /**< Identification Kea for User Data Cargo */
};
//==============================================================================
// iStat = Test_VectorSys_Stack ( iKey);
int Test_VectorSys_Stack (int iKey);

// iStat = Test_VectorSys_Heap ( iKey);
int Test_VectorSys_Heap (int iKey);
//==============================================================================

#endif

// --------------------------------------------------------------- File End ----

