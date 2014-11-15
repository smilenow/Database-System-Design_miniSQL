//
//  API.h
//  miniSQL
//
//  Created by Kael on 11/3/14.
//  Copyright (c) 2014 Xinyuan Lu. All rights reserved.
//

// v2.0, rewrite all the interfaces, by Xinyuan Lu

#ifndef __API__
#define __API__

#include "Base.h"
#include "IndexManager.h"
#include "RecordManager.h"
#include "CatalogManager.h"
#include "sqlcommand.h"

extern CatalogManager *catalogmanager;
extern RecordManager *recordmanager;
extern IndexManager *indexmanager;

class API{
public:
	Recordinfo dealCmd(sqlcommand& sql);
	int AttrCount(std::string tablename);
	bool TableExists(std::string tablename);
	bool IndexExists(std::string indexname);
	bool AttrExists(std::string attrname,std::string tablename);
public:
    // 全部返回recordinfo可能比较浪费？
    Recordinfo select(sqlcommand& sql);
    Recordinfo del(sqlcommand& sql);
    Recordinfo insert(sqlcommand& sql);
    Recordinfo createTable(sqlcommand& sql);
    Recordinfo createIndex(sqlcommand& sql, bool flag=false);
    Recordinfo dropTable(sqlcommand& sql);
    Recordinfo dropIndex(sqlcommand& sql);
    Recordinfo wrapTable();
    Recordinfo wrapAttribute();
};


#endif