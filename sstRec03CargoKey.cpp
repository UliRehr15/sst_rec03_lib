/**********************************************************************
 *
 * sstRec02Lib - cpp template library for sst
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
// sstRec03CargoKey.cpp    17.10.15  Re.    17.10.15  Re.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sstRec03Lib.h"
#include "sstRec03LibInt.h"

//=============================================================================
sstRec03CargoKeyCls::sstRec03CargoKeyCls()
{
    this->poCargoKeyIntern = new(sstRec03CargoKeyInternCls);
}
//=============================================================================
sstRec03CargoKeyCls::~sstRec03CargoKeyCls()
{
    delete(this->poCargoKeyIntern);
}
//=============================================================================
