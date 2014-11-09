//
//  main.cpp
//  miniSQL
//
//  Created by SmilENow on 11/9/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#include <iostream>
#include "Base.h"
#include "Block.h"
#include "sqlcommand.h"
#include "API.h"
#include "BufferManager.h"
#include "CatalogManager.h"
#include "RecordManager.h"
#include "IndexManager.h"

CatalogManager *catalogmanager = new CatalogManager();
RecordManager *recordmanager = new RecordManager();
IndexManager *indexmanager = new IndexManager();
BufferManager *buffermanager = new BufferManager();

int main(int argc, const char * argv[]) {
    return 0;
}
