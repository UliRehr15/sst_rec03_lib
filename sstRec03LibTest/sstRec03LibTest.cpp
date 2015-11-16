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
// sst_rec03_lib_test.cpp    13.09.15  Re.    13.09.15  Re.
//
// test frame for sstRec03Lib

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sstRec03Lib.h>

#include "sstRec03LibTest.h"

#define BUFSIZE 100

int main()
{

//=============================================================================
  int iStat = 0;

  // Do some intern Tests
  iStat = sstRec03_DoSomeInternTests ( 0);
  assert(iStat >= 0);

  {

    // Define new RecMem table object in heap for integer records
    int iTestVal = 0;
    sstRec03Cls oRecMem_Int(sizeof(iTestVal));
    dREC03RECNUMTYP dRecNo = 0;

    // standard errors

    // Read record 2 from RecMem
    iStat = oRecMem_Int.Read(0,2,&iTestVal);
    assert(iStat < 0);  // Error: Memory is empty

    // Write record 2 into RecMem
    iStat = oRecMem_Int.Writ(0,&iTestVal,2);
    assert(iStat < 0);  // Error: Memory is empty

    // write 100 Integer records in RecMem
    for(int j = 1; j <= 100; j++)
    {
      oRecMem_Int.WritNew(0,&j,&dRecNo);
    }

    // Write record at position 2 into sstRec memory
    int iValue=6;
    iStat = oRecMem_Int.Writ(0, &iValue, 2);
    assert(iStat >= 0);

    FILE* file = fopen("sstRec03LibTest.cpp", "r");
    assert(file);
    // Holds 100-character strings:
    sstRec03Cls oRecMem_Str(sizeof(char) * BUFSIZE);
    dRecNo = 1;
    char buf[BUFSIZE];
    while(fgets(buf, BUFSIZE, file))
    {
        oRecMem_Str.WritNew(0,buf,&dRecNo);
    }
    fclose(file);

    for(dREC03RECNUMTYP k = 1; k <= oRecMem_Int.count(); k++)
    {
      int iVal=0;
      oRecMem_Int.Read(0,k,&iVal);
      printf("oRecMem_Int.Read(%d) = %d\n", k, iVal);
    }

    iValue = 0;
    oRecMem_Int.Read( 0, 2, &iValue);
    assert(iValue==6);  // Return Value for record 2 should be 6

    for(dREC03RECNUMTYP i = 1; i <= oRecMem_Str.count(); i++)
    {
        oRecMem_Str.Read(0,i,buf);
        printf("oRecMem_Str.Read(%d) = %s", i, buf);
    }
    putchar('\n');
    }

//=============================================================================
    // Test with storing in file
    {
      sstRec03Cls intStash(sizeof(int));

      int iStat = intStash.NewFile( 0, (char*)"TestInt");
      assert (iStat >= 0);

      dREC03RECNUMTYP dRecNo = 0;
      for(int j = 1; j <= 100; j++)
        intStash.WritNew(0,&j,&dRecNo);
      iStat = intStash.SetStoreFile(0);
    }
// Open file TestInt.rec and read integer values, then delete file
    {
      sstRec03Cls *poRecMem_Int;
      poRecMem_Int = new sstRec03Cls(sizeof(int));
      int iStat = poRecMem_Int->OpenFile(0,(char*)"TestInt");
      assert (iStat >= 0);

      for(dREC03RECNUMTYP k = 1; k <= poRecMem_Int->count(); k++)
      { int iVal=0;
        poRecMem_Int->Read(0,k,&iVal);
        assert (iVal==k);
        printf("poRecMem_Int->Read(%d) = %d\n", k, iVal);
      }

    putchar('\n');
    delete(poRecMem_Int);
    }
    //=== testing cargo system ====================================================
    {
      int iValue;  // Value to store inside sstRecMem
      sstRec03Cls oRecMem_Int(sizeof(iValue));
      dREC03RECNUMTYP dRecNo = 0;

      // Create two empty cargo keys
      sstRec03CargoKeyCls oTestCargo1Key;
      sstRec03CargoKeyCls oTestCargo2Key;

      // Create two cargo packet objects
      // This records should be stored beside user data
      sstRec03TestRec1Cls oTestRec1;
      sstRec03TestRec2Cls oTestRec2;

        // Add cargo system to vector memory and get new identification key
        iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec1), (char*)"ABCD", &oTestCargo1Key);
        assert(iStat==0);

        // Add second cargo system to vector memory and get new identification key
        iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec2), (char*)"WXYZ", &oTestCargo2Key);
        assert(iStat==0);

        // write cargo packet 1 to vector Memory
        oTestRec1.iValue = 10;
        strncpy(oTestRec1.cVal,"ABC",5);
        iStat = oRecMem_Int.WrtCargo ( 0, &oTestCargo1Key, &oTestRec1);

        // write cargo packet 2 to vector Memory
        oTestRec2.dValue = 3.3;
        strncpy(oTestRec2.cVal,"ABC DEF",10);
        iStat = oRecMem_Int.WrtCargo ( 0, &oTestCargo2Key, &oTestRec2);

        // Write 10 records into sstRecMem
        // All get packets with 10/ABC and 3.3/ABC DEF
        for(int j = 1; j <= 10; j++)
        {
          oRecMem_Int.WritNew(0,&j,&dRecNo);
        }

        // write cargo packet 1 to vector Memory
        oTestRec1.iValue = 20;
        strncpy(oTestRec1.cVal,"DEF",5);
        iStat = oRecMem_Int.WrtCargo ( 0, &oTestCargo1Key, &oTestRec1);

        // write cargo packet 2 to vector Memory
        oTestRec2.dValue = 5.5;
        strncpy(oTestRec2.cVal,"GHI JKL",10);
        iStat = oRecMem_Int.WrtCargo ( 0, &oTestCargo2Key, &oTestRec2);

        // Write 10 more records into sstRecMem
        // All get packets with 20/DEF and 5.5/GHI JKL
        for(int j = 11; j <= 20; j++)
        {
          oRecMem_Int.WritNew(0,&j,&dRecNo);
        }

        // Set Status deleted for record 4
        iStat = oRecMem_Int.RecSetDeleted(0,4);
        // Read record 4, Error= Is deleted, Returned record is 0
        iValue = 5;
        iStat = oRecMem_Int.Read(0,4,&iValue);
        assert(iStat == -3);
        assert(iValue == 0);

        // Read record 5, has packet 10/ABC and 3.3/ABC DEF
        oRecMem_Int.Read(0,5,&iValue);
        assert(iValue == 5);
        oRecMem_Int.RedCargo(0,&oTestCargo1Key,&oTestRec1);
        assert(oTestRec1.iValue == 10);
        oRecMem_Int.RedCargo(0,&oTestCargo2Key,&oTestRec2);
        iStat = strncmp((char*)"ABC DEF", oTestRec2.cVal, 10);
        assert(iStat == 0);

        // Read record 15, has packet 20/DEF and 5.5/GHI JKL
        oRecMem_Int.Read(0,15,&iValue);
        assert(iValue == 15);
        oRecMem_Int.RedCargo(0,&oTestCargo1Key,&oTestRec1);
        assert(oTestRec1.iValue == 20);
        oRecMem_Int.RedCargo(0,&oTestCargo2Key,&oTestRec2);
        iStat = strncmp((char*)"GHI JKL", oTestRec2.cVal, 10);
        assert(iStat == 0);

    }
    //=== testing cargo system: Error Behavior ================================
    {
      int iValue;  // Value to store inside sstRecMem
      sstRec03Cls oRecMem_Int(sizeof(iValue));  // new sstRecMem for iValue
      dREC03RECNUMTYP dRecNo = 0;

      // Create two empty cargo keys
      sstRec03CargoKeyCls oTestCargo1Key;
      sstRec03CargoKeyCls oTestCargo2Key;

      // Create two cargo packet objects
      sstRec03TestRec1Cls oTestRec1;
      sstRec03TestRec2Cls oTestRec2;

      // Add cargo system to vector memory and get new identification key
      // Error, size of cargo packet is empty
      iStat = oRecMem_Int.AddCargoSys( 0, 0, (char*)"ABC", &oTestCargo1Key);
      assert(iStat == -2);

      // Add cargo system to vector memory and get new identification key
      // Error, name of cargo packet is not length = 3
      iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec1), (char*)"ABCDE", &oTestCargo1Key);
      assert(iStat == -3);

      // Add cargo system to vector memory and get new identification key
      iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec1), (char*)"ABCD", &oTestCargo1Key);

      // Add cargo system to vector memory and get new identification key
      // Error, cargo key is not empty
      iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec2), (char*)"ABCD", &oTestCargo1Key);
      assert(iStat == -4);

      // Add cargo system to vector memory and get new identification key
      // Error, new cargo name is not unique
      iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec2), (char*)"ABCD", &oTestCargo2Key);
      assert(iStat == -5);

      // Write record into RecMem
      oRecMem_Int.WritNew(0,&iValue,&dRecNo);

      // Add cargo system to vector memory and get new identification key
      // Error, sstRecMem is not empty
      iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec2), (char*)"ABCD", &oTestCargo2Key);
      assert(iStat == -10);


    }


    {

    //--- Testing binary tree sorting --------------------------------------------
    sstRec03TreeKeyCls oTriKey_CC;  // new tree identification object for sorting chars
    sstRec03TreeKeyCls oTriKey_I2;  // new tree identification object for sorting ints
    dREC03RECNUMTYP dRecNo = 0;
    sstRec03TestRec1Cls oTstRec11;  // Test Record 1 Write
    sstRec03TestRec1Cls oTstRec12;  // Test Record 1 Read
    sstRec03Cls oTestRecMem1(sizeof(oTstRec11));  // new sstRecMem Table for Test Record 1

      // Initialize new Sorttree 1 in RecMem for test Record 1
      // Sorting value should be cVal
      iStat = oTestRecMem1.TreIni ( 0, &oTstRec11, &oTstRec11.cVal, sizeof(oTstRec11.cVal), sstRecTyp_CC, &oTriKey_CC);
      assert(iStat == 0);
      // Initialize new Sorttree 2 in RecMem for test Record 1
      // Sorting value should be iValue
      iStat = oTestRecMem1.TreIni ( 0, &oTstRec11, &oTstRec11.iValue, sizeof(oTstRec11.iValue), sstRecTyp_I2, &oTriKey_I2);
      assert(iStat == 0);

      // insert test record 1 (int,char)
      iStat = oTstRec11.SetAllValues(20,(char*)"dscr");
      iStat = oTestRecMem1.WritNew( 0, &oTstRec11, &dRecNo);

      // insert test record 2 (int,char)
      iStat = oTstRec11.SetAllValues(30,(char*)"cscr");
      iStat = oTestRecMem1.WritNew( 0, &oTstRec11, &dRecNo);

      // insert test record 3 (int,char)
      iStat = oTstRec11.SetAllValues(10,(char*)"escr");
      iStat = oTestRecMem1.WritNew( 0, &oTstRec11, &dRecNo);

      // insert test record 4 (int,char)
      iStat = oTstRec11.SetAllValues(20,(char*)"ascr");
      iStat = oTestRecMem1.WritNew( 0, &oTstRec11, &dRecNo);

      // insert test record 5 (int,char)
      iStat = oTstRec11.SetAllValues(15,(char*)"cscr");
      iStat = oTestRecMem1.WritNew( 0, &oTstRec11, &dRecNo);

      // Full rebuild of sorting tree 1 oTriKey_CC (chars) in RecMem
      iStat = oTestRecMem1.TreBld ( 0, &oTriKey_CC);
      assert(iStat == 0);

      // Full rebuild of sorting tree 2 oTriKey_I2 (ints) in RecMem
      iStat = oTestRecMem1.TreBld ( 0, &oTriKey_I2);
      assert(iStat == 0);

      // Tree 1: Sort order 4,2,5,1,3
      // Tree 2: Sort order 3,5,1,4,2

      dREC03RECNUMTYP dRecNo1 = 0;

      // Delete next record with given value from tree(s)
      char cValue[5] = "cscr";
      // iStat = oTestRecMem1.TreDel ( 0, &oTriKey_CC, (char*)"cscr", &dRecNo1);
      iStat = oTestRecMem1.TreDelValue ( 0, &oTriKey_CC, cValue, &dRecNo1);
      assert(iStat == 1);
      assert(dRecNo1 == 2);

      // Tree 1: Sort order 4,5,1,3

      // Traverse throuh given tree 1 from small to great
      // Return next greater/equal record and number for given record
      // Start with record 0: return record and record number with smallest value

      dRecNo1 = 0;
      iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
      iStat = strncmp((char*)"ascr", oTstRec12.cVal, 5);
      if(iStat != 0) assert(0);

// Record 2 is deleted from tree


      // seach record with next greater value to dRecNo and return record and record number
      //      iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
      //      iStat = strncmp((char*)"cscr", oTstRec12.cVal, 5);
      //      if(iStat != 0) assert(0);

      iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
      iStat = strncmp((char*)"cscr", oTstRec12.cVal, 5);
      if(iStat != 0) assert(0);

      iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
      iStat = strncmp((char*)"dscr", oTstRec12.cVal, 5);
      if(iStat != 0) assert(0);

      iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
      iStat = strncmp((char*)"escr", oTstRec12.cVal, 5);
      if(iStat != 0) assert(0);

      // Traverse throuh given tree 1 from small to great
      // Return next greater/equal record number for given record
      // Start with record 0

      dRecNo1 = 0;
      // return record number for smallest value for tree oTriKey_CC
      iStat = oTestRecMem1.TreSeaFrst ( 0, &oTriKey_CC, &dRecNo1);

      // Result should be 4 for value = ascr
      assert(dRecNo1==4);

      dREC03RECNUMTYP sRecNo2 = 0;

      // Return record number for next greater/equal value

// Record 2 is deleted from Tree

      //      iStat = oTestRecMem1.TreSeaNxtGE ( 0, &oTriKey_CC, dRecNo1, &sRecNo2);
      //      dRecNo1 = sRecNo2;
      //      assert(dRecNo1==2);

      iStat = oTestRecMem1.TreSeaNxtGE ( 0, &oTriKey_CC, dRecNo1, &sRecNo2);
      dRecNo1 = sRecNo2;
      assert(dRecNo1==5);

      iStat = oTestRecMem1.TreSeaNxtGE ( 0, &oTriKey_CC, dRecNo1, &sRecNo2);
      dRecNo1 = sRecNo2;
      assert(dRecNo1==1);

      iStat = oTestRecMem1.TreSeaNxtGE ( 0, &oTriKey_CC, dRecNo1, &sRecNo2);
      dRecNo1 = sRecNo2;
      assert(dRecNo1==3);

      iStat = oTestRecMem1.TreSeaNxtGE ( 0, &oTriKey_CC, dRecNo1, &sRecNo2);
      assert(sRecNo2==0);

      // Tree 2: Sort order 3,5,1,4,2

      // Traverse throuh given tree 2 from small to great
      // Return next greater/equal record and number for given record
      // Start with record 0
      dRecNo1 = 0;
      iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
      assert(oTstRec12.iValue == 10);
      assert(dRecNo1==3);

      iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
      assert(oTstRec12.iValue == 15);
      assert(dRecNo1==5);

      iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
      assert(oTstRec12.iValue == 20);
      assert(dRecNo1==1);

      iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
      assert(oTstRec12.iValue == 20);
      assert(dRecNo1==4);

      iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
      assert(oTstRec12.iValue == 30);
      assert(dRecNo1==2);


      dRecNo1 = 0;
      // return record number for smallest value for tree oTriKey_CC
      iStat = oTestRecMem1.TreSeaFrst ( 0, &oTriKey_I2, &dRecNo1);

      // Result should be 4 for value = ascr
      assert(dRecNo1==3);

      sRecNo2 = 0;

      // Return record number for next greater/equal value
      iStat = oTestRecMem1.TreSeaNxtGE ( 0, &oTriKey_I2, dRecNo1, &sRecNo2);
      dRecNo1 = sRecNo2;
      assert(dRecNo1==5);
      iStat = oTestRecMem1.TreSeaNxtGE ( 0, &oTriKey_I2, dRecNo1, &sRecNo2);
      dRecNo1 = sRecNo2;
      assert(dRecNo1==1);
      iStat = oTestRecMem1.TreSeaNxtGE ( 0, &oTriKey_I2, dRecNo1, &sRecNo2);
      dRecNo1 = sRecNo2;
      assert(dRecNo1==4);
      iStat = oTestRecMem1.TreSeaNxtGE ( 0, &oTriKey_I2, dRecNo1, &sRecNo2);
      dRecNo1 = sRecNo2;
      assert(dRecNo1==2);
      iStat = oTestRecMem1.TreSeaNxtGE ( 0, &oTriKey_I2, dRecNo1, &sRecNo2);
      assert(sRecNo2==0);

    }
  {
    //--- Testing binary tree sorting (build tree on the fly) --------------------

    sstRec03TreeKeyCls oTriKey_CC;  // new tree identification object for sorting chars
    sstRec03TreeKeyCls oTriKey_I2;  // new tree identification object for sorting ints
    dREC03RECNUMTYP dRecNo = 0;
    sstRec03TestRec1Cls oTstRec11;
    sstRec03TestRec1Cls oTstRec12;
    sstRec03Cls oTestRecMem1(sizeof(oTstRec11));  // new sstRecMem Table for TstRec11 records

    // Initialize new Sorttree for RecMem
    // Sorting value should be cVal
    iStat = oTestRecMem1.TreIni ( 0, &oTstRec11, &oTstRec11.cVal, sizeof(oTstRec11.cVal), sstRecTyp_CC, &oTriKey_CC);
    assert(iStat == 0);
    // Sorting value should be iValue
    iStat = oTestRecMem1.TreIni ( 0, &oTstRec11, &oTstRec11.iValue, sizeof(oTstRec11.iValue), sstRecTyp_I2, &oTriKey_I2);
    assert(iStat == 0);

    // insert test record (int,char) and update trees
    iStat = oTstRec11.SetAllValues(20,(char*)"dscr");
    iStat = oTestRecMem1.TreWriteNew ( 0, &oTstRec11, &dRecNo);

    // insert test record (int,char) and update trees
    iStat = oTstRec11.SetAllValues(30,(char*)"cscr");
    iStat = oTestRecMem1.TreWriteNew ( 0, &oTstRec11, &dRecNo);

    // insert test record (int,char) and update trees
    iStat = oTstRec11.SetAllValues(10,(char*)"escr");
    iStat = oTestRecMem1.TreWriteNew ( 0, &oTstRec11, &dRecNo);

    // insert test record (int,char) and update trees
    iStat = oTstRec11.SetAllValues(40,(char*)"ascr");
    iStat = oTestRecMem1.TreWriteNew ( 0, &oTstRec11, &dRecNo);

    // Traverse throuh given tree 1 from small to great
    // Return next greater/equal record and number for given record
    // Start with record 0

    dREC03RECNUMTYP dRecNo1 = 0;
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"ascr", oTstRec12.cVal, 5);
    assert(iStat == 0);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"cscr", oTstRec12.cVal, 5);
    assert(iStat == 0);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"dscr", oTstRec12.cVal, 5);
    assert(iStat == 0);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"escr", oTstRec12.cVal, 5);
    assert(iStat == 0);

    // Traverse throuh given tree 2 from small to great
    // Tree 2: 3,1,2,4
    // Return next greater/equal record and number for given record
    // Start with record 0

    dRecNo1 = 0;
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
    assert(oTstRec12.iValue == 10);
    assert(dRecNo1==3);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
    assert(oTstRec12.iValue == 20);
    assert(dRecNo1==1);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
    assert(oTstRec12.iValue == 30);
    assert(dRecNo1==2);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
    assert(oTstRec12.iValue == 40);
    assert(dRecNo1==4);
  }
  {
//--- Rewrite record and update tree(s), search value in tree

    sstRec03TreeKeyCls oTriKey_CC;  // new tree identification object for sorting chars
    sstRec03TreeKeyCls oTriKey_I2;  // new tree identification object for sorting ints
    dREC03RECNUMTYP dRecNo = 0;
    sstRec03TestRec1Cls oTstRec11;
    sstRec03TestRec1Cls oTstRec12;
    sstRec03Cls oTestRecMem1(sizeof(oTstRec11));  // new sstRecMem Table for TstRec11 records

    // Initialize new Sorttree for RecMem
    // Sorting value should be cVal
    iStat = oTestRecMem1.TreIni ( 0, &oTstRec11, &oTstRec11.cVal, sizeof(oTstRec11.cVal), sstRecTyp_CC, &oTriKey_CC);
    assert(iStat == 0);
    // Sorting value should be iValue
    iStat = oTestRecMem1.TreIni ( 0, &oTstRec11, &oTstRec11.iValue, sizeof(oTstRec11.iValue), sstRecTyp_I2, &oTriKey_I2);
    assert(iStat == 0);

    // insert test record (int,char) and update trees
    iStat = oTstRec11.SetAllValues(20,(char*)"dscr");
    iStat = oTestRecMem1.TreWriteNew ( 0, &oTstRec11, &dRecNo);

    // insert test record (int,char) and update trees
    iStat = oTstRec11.SetAllValues(30,(char*)"cscr");
    iStat = oTestRecMem1.TreWriteNew ( 0, &oTstRec11, &dRecNo);

    // insert test record (int,char) and update trees
    iStat = oTstRec11.SetAllValues(10,(char*)"escr");
    iStat = oTestRecMem1.TreWriteNew ( 0, &oTstRec11, &dRecNo);

    // insert test record (int,char) and update trees
    iStat = oTstRec11.SetAllValues(40,(char*)"ascr");
    iStat = oTestRecMem1.TreWriteNew ( 0, &oTstRec11, &dRecNo);

    // Find record with exact search value
    iStat = oTestRecMem1.TreSeaEQ ( 0, &oTriKey_CC, (char*)"bbcr", &dRecNo);

    // Traverse throuh given tree 1 from small to great
    // Tree 1: 4,2,1,3
    // Return next greater/equal record and number for given record
    // Start with record 0

    dREC03RECNUMTYP dRecNo1 = 0;
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"ascr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==4);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"cscr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==2);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"dscr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==1);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"escr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==3);

    // Traverse throuh given tree 2 from small to great
    // Tree 2: 3,1,2,4
    // Return next greater/equal record and number for given record
    // Start with record 0

    dRecNo1 = 0;
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
    assert(oTstRec12.iValue == 10);
    assert(dRecNo1==3);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
    assert(oTstRec12.iValue == 20);
    assert(dRecNo1==1);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
    assert(oTstRec12.iValue == 30);
    assert(dRecNo1==2);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
    assert(oTstRec12.iValue == 40);
    assert(dRecNo1==4);

    // Tree 1: 4,2,1,3

    // Rewrite record at position 4 with big value and update tree(s)
    iStat = oTstRec11.SetAllValues(30,(char*)"xscr");
    iStat = oTestRecMem1.TreWritAtPos ( 0 , &oTstRec11, 4);

    // Traverse throuh given tree 1 from small to great
    // Now Tree 1 order: 2,1,3,4

    dRecNo1 = 0;
    //    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    //    iStat = strncmp((char*)"ascr", oTstRec12.cVal, 5);
    //    assert(iStat == 0);
    //    assert(dRecNo1==4);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"cscr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==2);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"dscr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==1);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"escr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==3);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"xscr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==4);

    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    assert(iStat == -2);  // Nothing found
    iStat = strncmp((char*)"", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1 == 0);

//    dRecNo1 = 0;
//    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
//    assert(oTstRec12.iValue == 10);
//    assert(dRecNo1==3);

//    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
//    assert(oTstRec12.iValue == 20);
//    assert(dRecNo1==1);

//    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
//    assert(oTstRec12.iValue == 30);
//    assert(dRecNo1==2);

//    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_I2, &oTstRec12, &dRecNo1);
//    assert(oTstRec12.iValue == 40);
//    assert(dRecNo1==4);

  }
//=============================================================================
  // Tree Tests with equal values
  {
    sstRec03TreeKeyCls oTriKey_CC;  // new tree identification object for sorting chars
    sstRec03TreeKeyCls oTriKey_I2;  // new tree identification object for sorting ints
    dREC03RECNUMTYP dRecNo = 0;
    sstRec03TestRec1Cls oTstRec11;
    sstRec03TestRec1Cls oTstRec12;
    sstRec03Cls oTestRecMem1(sizeof(oTstRec11));  // new sstRecMem Table for TstRec11 records

    // Initialize new Sorttree for RecMem
    // Sorting value should be cVal
    iStat = oTestRecMem1.TreIni ( 0, &oTstRec11, &oTstRec11.cVal, sizeof(oTstRec11.cVal), sstRecTyp_CC, &oTriKey_CC);
    assert(iStat == 0);
    // Sorting value should be iValue
    iStat = oTestRecMem1.TreIni ( 0, &oTstRec11, &oTstRec11.iValue, sizeof(oTstRec11.iValue), sstRecTyp_I2, &oTriKey_I2);
    assert(iStat == 0);

    // insert test record (int,char) and update trees
    iStat = oTstRec11.SetAllValues(20,(char*)"dscr");
    iStat = oTestRecMem1.TreWriteNew ( 0, &oTstRec11, &dRecNo);
    iStat = oTestRecMem1.TreWriteNew ( 0, &oTstRec11, &dRecNo);
    iStat = oTestRecMem1.TreWriteNew ( 0, &oTstRec11, &dRecNo);
    iStat = oTestRecMem1.TreWriteNew ( 0, &oTstRec11, &dRecNo);

    dREC03RECNUMTYP dRecNo1 = 0;
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"dscr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==1);
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"dscr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==2);
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"dscr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==3);
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"dscr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==4);
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    assert(iStat == -2);
    iStat = strncmp((char*)"", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==0);

    // Delete record 3 from all trees
    iStat = oTestRecMem1.TreDelNumber ( 0, 3);

    dRecNo1 = 0;
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"dscr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==1);
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"dscr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==2);
//    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
//    iStat = strncmp((char*)"dscr", oTstRec12.cVal, 5);
//    assert(iStat == 0);
//    assert(dRecNo1==3);
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    iStat = strncmp((char*)"dscr", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==4);
    iStat = oTestRecMem1.TreReadNxtGE ( 0, &oTriKey_CC, &oTstRec12, &dRecNo1);
    assert(iStat == -2);
    iStat = strncmp((char*)"", oTstRec12.cVal, 5);
    assert(iStat == 0);
    assert(dRecNo1==0);

  }
  return 0;
}
//=============================================================================
