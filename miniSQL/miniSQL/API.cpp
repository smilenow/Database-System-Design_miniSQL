//
//  API.cpp
//  miniSQL
//
//  Created by Kael on 11/3/14.
//  Copyright (c) 2014 Xinyuan Lu. All rights reserved.
//

// v2.0, see comment in "APT.h"

#include "API.h"

Recordinfo API::dealCmd(sqlcommand& sql){
	// 使用get方法更好，这里先直接调用public
	switch(sql.sqlType){
		case 0:
		return select(sql);
		break;
		case 1:
		return del(sql);
		break;
		case 2:
		return insert(sql);
		break;
		case 3:
		return createTable(sql);
		break;
		case 4:
		return createIndex(sql);
		break;
		case 5:
		return dropTable(sql);
		break;
		case 6:
		return dropIndex(sql);
		break;
	}
	return Recordinfo();
}

int API::AttrCount(std::string tablename){
	return catalogmanager->AttrCount(tablename);
}
bool API::TableExists(std::string tablename){
	return catalogmanager->TableExists(tablename);
}
bool API::IndexExists(std::string indexname){
	return catalogmanager->IndexExists(indexname);
}
bool API::AttrExists(std::string attrname,std::string tablename){
	return catalogmanager->AttrExists(attrname, tablename);
}

// 以下所有的都需要错误输入的检查？
Recordinfo API::select(sqlcommand& sql){
	// 先到catalog查看是否有index
	// 如果有就用，然后作intersection
	// 如果没有，直接交给record？
	// 可以减少数据传递的overhead
	bool flag=false;
    std::vector<std::vector<std::string> >::iterator conditioni;
	Recordinfo result;
    
	int IndexAttrType=0x7fff;

	for(conditioni=sql.conditions.begin(); conditioni<sql.conditions.end(); conditioni++){
		flag=catalogmanager->hasIndex(sql.tablename, (*conditioni).at(0));
		if(flag) {
			IndexAttrType=catalogmanager->getAttrType(sql.tablename, (*conditioni).at(0));
			break;
		}
	}
    
    // 需要商量
    Table table=catalogmanager->getTable(sql.tablename);
    
	if(!flag) {
		std::vector<slot> slots;
		result=recordmanager->Select_Record(sql, table, false, slots);
	}
	else{
		Value *v=NULL;
		switch(IndexAttrType){
			case 0:
			v=new Value(IndexAttrType, std::stoi((*conditioni).at(2)));
			break;
			case 1:
			v=new Value(IndexAttrType, (*conditioni).at(2));
			break;
			case -1:
			v=new Value(IndexAttrType, std::stof((*conditioni).at(2)));
			break;
		}
		IndexManager *indexmanager=new IndexManager();
        
        std::vector<slot> s;
		if((*conditioni).at(1)=="<") s=indexmanager->_FindSmall((*conditioni).at(0), *v);
        else if((*conditioni).at(1)=="<=") s=indexmanager->_FindSmallEqual((*conditioni).at(0), *v);
        else if((*conditioni).at(1)==">") s=indexmanager->_FindBigger((*conditioni).at(0), *v);
        else if((*conditioni).at(1)==">=") s=indexmanager->_FindBiggerEqual((*conditioni).at(0), *v);
        else if((*conditioni).at(1)=="!=") s=indexmanager->_FindNotEqual((*conditioni).at(0), *v);
        else if((*conditioni).at(1)=="==") s.push_back(indexmanager->select((*conditioni).at(0), *v));
        if(s.size()==0 || (s.size()==1 && s[0].block_id==-1)){
            delete v;
            return Recordinfo(false, "Cannot find the record!", Result(), 0);
        }
        result=recordmanager->Select_Record(sql, table, true, s);
		// 然后开始剩下的暴力获取
        sql.conditions.erase(conditioni);
        Recordinfo result2=recordmanager->Select_Record(sql, table, false, s);
        // and
        result.AND(result2);
		delete v;
	}
    return result;
}

Recordinfo API::del(sqlcommand& sql){
    bool flag=false;
    //	int n=-1;	// 记录第几个是index
    std::vector<std::vector<std::string> >::iterator conditioni;
    Recordinfo result;
    
    int IndexAttrType=0x7fff;
    
    for(conditioni=sql.conditions.begin(); conditioni<sql.conditions.end(); conditioni++){
        flag=catalogmanager->hasIndex(sql.tablename, (*conditioni).at(0));
        if(flag) {
            IndexAttrType=catalogmanager->getAttrType(sql.tablename, (*conditioni).at(0));
            break;
        }
    }
    
    // 需要商量
    Table table=catalogmanager->getTable(sql.tablename);
    
    if(!flag) {
        std::vector<slot> slots;
        result=recordmanager->Delete_Record(sql, table, false, slots);
    }
    else{
        Value *v=NULL;
        switch(IndexAttrType){
            case 0:v=new Value(IndexAttrType, std::stoi((*conditioni).at(2)));break;
            case 1:v=new Value(IndexAttrType, (*conditioni).at(2));break;
            case -1:v=new Value(IndexAttrType, std::stof((*conditioni).at(2)));break;
        }
        
        std::vector<slot> s;
        if((*conditioni).at(1)=="<") s=indexmanager->_FindSmall((*conditioni).at(0), *v);
        else if((*conditioni).at(1)=="<=") s=indexmanager->_FindSmallEqual((*conditioni).at(0), *v);
        else if((*conditioni).at(1)==">") s=indexmanager->_FindBigger((*conditioni).at(0), *v);
        else if((*conditioni).at(1)==">=") s=indexmanager->_FindBiggerEqual((*conditioni).at(0), *v);
        else if((*conditioni).at(1)=="!=") s=indexmanager->_FindNotEqual((*conditioni).at(0), *v);
        else if((*conditioni).at(1)=="==") s.push_back(indexmanager->select((*conditioni).at(0), *v));
        if(s.size()==0 || (s.size()==1 && s[0].block_id==-1)){
            delete v;
            return Recordinfo(false, "Cannot find the record!", Result(), 0);
        }
        result=recordmanager->Delete_Record(sql, table, true, s);
        // 然后开始剩下的暴力获取
        sql.conditions.erase(conditioni);
        Recordinfo result2=recordmanager->Select_Record(sql, table, false, s);
        // and
        result.AND(result2);
        delete v;
    }
    return result;
}

Recordinfo API::insert(sqlcommand& sql){
	// 为什么insert也要有sqlcommand????
    // 插入index
    
    Recordinfo result;
    Table table=catalogmanager->getTable(sql.tablename);
    int block_id;
    int record_id;
    
    // check primary
    std::string primarykey=catalogmanager->pkOnTable(sql.tablename);
    if(primarykey=="Table not found in CatalogBlocks!"){
        result=Recordinfo(false, primarykey, Result(), 0);
        return result;
    }
    else{
        for(int i=0; i<sql.colValue.size(); i++){
            if(catalogmanager->isUnique(sql.tablename, i) || catalogmanager->isPK(sql.tablename, i)){
                std::string attrname=catalogmanager->getAttrName(sql.tablename, i);
                // 如果有索引就用索引，并且更新索引
                if(catalogmanager->hasIndex(sql.tablename, attrname)){
                    std::string indexname=catalogmanager->getIndexName(sql.tablename, attrname);
                    int AttrType=catalogmanager->getAttrType(sql.tablename, primarykey);
                    Value *v=NULL;
                    switch(AttrType){
                        case 0:v=new Value(AttrType, std::stoi((sql.colValue).at(i)));break;
                        case 1:v=new Value(AttrType, (sql.colValue).at(i));break;
                        case -1:v=new Value(AttrType, std::stof(sql.colValue.at(i)));break;
                    }
                    slot s=indexmanager->select(indexname, *v);
                    if(s.block_id!=-1){
                        Recordinfo result=Recordinfo(false, "The unique key value has existed!", Result(), 0);
                        delete v;
                        return result;
                    }
                    delete v;
                }
                // 没有索引直接查询
                sqlcommand tempsql=sql;
                tempsql.sqlType=0;
                tempsql.conditions={{attrname, "=", sql.colValue[i]}};
                tempsql.selectInfo={"*"};
                Recordinfo r=recordmanager->Select_Record(tempsql, table, 0, std::vector<slot>());
                if(r.successful){
                    result=Recordinfo(false, "The unique key value has existed!", Result(), 0);
                    return result;
                }
            }
        }
        
        result=recordmanager->Insert_Record(sql, table, block_id, record_id);
        //插入索引
        slot s=slot(block_id, record_id);
        for(int i=0; i<sql.colValue.size(); i++){
            std::string attrname=catalogmanager->getAttrName(sql.tablename, i);
            if(catalogmanager->hasIndex(sql.tablename, attrname)){
                std::string indexname=catalogmanager->getIndexName(sql.tablename, attrname);
                int AttrType=catalogmanager->getAttrType(sql.tablename, primarykey);
                Value *v=NULL;
                switch(AttrType){
                    case 0:v=new Value(AttrType, std::stoi((sql.colValue).at(i)));break;
                    case 1:v=new Value(AttrType, (sql.colValue).at(i));break;
                    case -1:v=new Value(AttrType, std::stof(sql.colValue.at(i)));break;
                }
                indexmanager->_insert(indexname, *v, s);
            }
        }
        // 解决可能的返回错误？
        return result;
    }
}

Recordinfo API::createTable(sqlcommand& sql){
    catalogmanager->insertTable(sql);
    std::string pk;
    std::string tablename=sql.createTableInfo[0].at(0);
    if((pk=catalogmanager->pkOnTable(tablename))!="No primary key on this table!"){
        // 可以创建空的index吧？
        indexmanager->CreateIndex(tablename+"$"+pk, catalogmanager->getAttrType(tablename, pk), std::vector<Value>(0), std::vector<slot>(0));
        
        sqlcommand tempsql=sqlcommand();
        tempsql.sqlType=4;
        tempsql.indexname=tablename+"$"+pk;
        tempsql.setCreateIndexInfo(tablename, pk);
        catalogmanager->insertIndex(tempsql);
    }
    return Recordinfo(); // further improve
}

Recordinfo API::createIndex(sqlcommand& sql){
	std::string tablename=sql.createIndexInfo[1];
    std::string indexname=sql.createIndexInfo[0];
    int attrtype=catalogmanager->getAttrType(tablename, sql.createIndexInfo[2]);
    indexmanager->CreateIndex(sql.indexname, attrtype, std::vector<Value>(), std::vector<slot>());
    
    catalogmanager->insertIndex(sql);
        return Recordinfo(); // further improve
}

// turn to Catalog find all index and
// drop index
// dropCatalog
// drop all records
Recordinfo API::dropTable(sqlcommand& sql){
    std::string tablename=sql.tablename;
    
    for(int i=0; i<catalogmanager->AttrCount(tablename); i++){
        std::string attrname=catalogmanager->getAttrName(tablename, i);
        if(catalogmanager->hasIndex(tablename, attrname)){
            indexmanager->DropIndex(catalogmanager->getIndexName(tablename, attrname));
            catalogmanager->dropIndex(catalogmanager->getIndexName(tablename, attrname));
        }
    }
    catalogmanager->dropTable(sql.tablename);
        return Recordinfo(); // further improve
}

Recordinfo API::dropIndex(sqlcommand& sql){
    catalogmanager->dropIndex(sql.indexname);
    indexmanager->DropIndex(sql.indexname);
        return Recordinfo(); // further improve
    // catalog
}