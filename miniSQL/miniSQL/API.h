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
    // 对外的接口，解决所有的sql语句
	Recordinfo dealCmd(sqlcommand& sql);

    // 应catalogmanager要求，新增这些接口，供interpreter语法检查使用
	int AttrCount(std::string tablename);
	bool TableExists(std::string tablename);
	bool IndexExists(std::string indexname);
	bool AttrExists(std::string attrname,std::string tablename);
public:
    // 全部返回recordinfo可能比较浪费？

    // 先检查所有有索引的属性，然后对所有用索引查到的tuple取交
    // 再判断这些tuple是否满足剩下的条件
    Recordinfo select(sqlcommand& sql);

    // 先选择到要删除的tuple，同select
    // 然后删除record，再删除该表所有的index里面对应的tuple
    Recordinfo del(sqlcommand& sql);
    
    // 先检查table是否存在，然后检查primary key/unique有没有重复
    // 插入tuple，再插入所有的索引
    Recordinfo insert(sqlcommand& sql);

    // 创建表的同时创建索引
    Recordinfo createTable(sqlcommand& sql);

    // first check if it is a unique/primary
    // then check a index has been created
    // if created, return the index name
    // 创建结束之后，还要：
    // 在catalog里面更新记录
    // 插入该表现在所有的record
    Recordinfo createIndex(sqlcommand& sql, bool flag=false);

    // turn to Catalog find all index
    // drop index, drop table in Catalog, drop all records
    Recordinfo dropTable(sqlcommand& sql);
    
    Recordinfo dropIndex(sqlcommand& sql);
};


#endif