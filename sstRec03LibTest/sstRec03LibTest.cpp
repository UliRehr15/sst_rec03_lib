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
// sst_rec02_lib_test.cpp    13.09.15  Re.    13.09.15  Re.
//
// test frame for sstRec02Lib

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sstRec03Lib.h>

#include "sstRec03LibTest.h"

#define BUFSIZE 100

int main() {

//=============================================================================
    int iStat = 0;
    // Do some intern Tests
    iStat = sstRec03_DoSomeInternTests ( 0);
    assert(iStat >= 0);

    {

    sstRec03Cls oRecMem_Int(sizeof(int));
    dREC03RECNUMTYP index = 0;

    // standard errors
    iStat = oRecMem_Int.Read(0,2,&index);
    assert(iStat < 0);  // Error: Memory is empty

    iStat = oRecMem_Int.Writ(0,&index,2);
    assert(iStat < 0);  // Error: Memory is empty

    for(int j = 1; j <= 100; j++)
    {
      oRecMem_Int.WritNew(0,&j,&index);
    }

    // Write record at position 2 into sstRec memory
    int iValue=6;
    iStat = oRecMem_Int.Writ(0, &iValue, 2);
    assert(iStat >= 0);

    FILE* file = fopen("sstRec03LibTest.cpp", "r");
    assert(file);
    // Holds 100-character strings:
    sstRec03Cls oRecMem_Str(sizeof(char) * BUFSIZE);
    index = 1;
    char buf[BUFSIZE];
    while(fgets(buf, BUFSIZE, file))
    {
        oRecMem_Str.WritNew(0,buf,&index);
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

      dREC03RECNUMTYP index = 0;
      for(int j = 1; j <= 100; j++)
        intStash.WritNew(0,&j,&index);
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
      sstRec03TestRec1Cls oTestRec1;
      sstRec03TestRec2Cls oTestRec2;

        // Add cargo system to vector memory and get new identification key
        iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec1), (char*)"ABC", &oTestCargo1Key);

        // Add second cargo system to vector memory and get new identification key
        iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec2), (char*)"XYZ", &oTestCargo2Key);

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
      iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec1), (char*)"ABCD", &oTestCargo1Key);
      assert(iStat == -3);

      // Add cargo system to vector memory and get new identification key
      iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec1), (char*)"ABC", &oTestCargo1Key);

      // Add cargo system to vector memory and get new identification key
      // Error, cargo key is not empty
      iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec2), (char*)"ABC", &oTestCargo1Key);
      assert(iStat == -4);

      // Add cargo system to vector memory and get new identification key
      // Error, new cargo name is not unique
      iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec2), (char*)"ABC", &oTestCargo2Key);
      assert(iStat == -5);

      // Write record into RecMem
      oRecMem_Int.WritNew(0,&iValue,&dRecNo);

      // Add cargo system to vector memory and get new identification key
      // Error, sstRecMem is not empty
      iStat = oRecMem_Int.AddCargoSys( 0, sizeof(oTestRec2), (char*)"ABC", &oTestCargo2Key);
      assert(iStat == -10);


    }
  return 0;
}
//=============================================================================
