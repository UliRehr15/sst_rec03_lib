
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
// sstRec03TreeKeyIntern.cpp    20.10.15  Re.    20.10.15  Re.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sstRec03Lib.h"
#include "sstRec03LibInt.h"

//==============================================================================
sstRec03TreeHeaderCls::sstRec03TreeHeaderCls()
{
  dRoot = 0;
  iNodeOfs = 0;
  iOffset = 0;
  iSize = 0;
  eTyp = sstRecTyp_No;

  this->poDataKey = new (sstRec03CargoKeyInternCls );
}
//==============================================================================
