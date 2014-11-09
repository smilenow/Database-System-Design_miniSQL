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
	CatalogManager *cm=new CatalogManager();
	return cm->AttrCount(tablename);
}
bool API::TableExists(std::string tablename){
	CatalogManager *cm=new CatalogManager();
	return cm->TableExists(tablename);
}
bool API::IndexExists(std::string indexname){
	CatalogManager *cm=new CatalogManager();
	return cm->IndexExists(indexname);
}
bool API::AttrExists(std::string attrname,std::string tablename){
	CatalogManager *cm=new CatalogManager();
	return cm->AttrExists(attrname, tablename);
}

// 以下所有的都需要错误输入的检查？
Recordinfo API::select(sqlcommand& sql){
	// 先到catalog查看是否有index
	// 如果有就用，然后作intersection
	// 如果没有，直接交给record？
	// 可以减少数据传递的overhead
	CatalogManager *cm=new CatalogManager();
	RecordManager *rm=new RecordManager();
	bool flag=false;
    std::vector<std::vector<std::string> >::iterator conditioni;
	Recordinfo result;
    
	int IndexAttrType=0x7fff;

	for(conditioni=sql.conditions.begin(); conditioni<sql.conditions.end(); conditioni++){
		flag=cm->hasIndex(sql.tablename, (*conditioni).at(0));
		if(flag) {
			// 为什么catalog的getattrtype()被注释掉了？
			IndexAttrType=cm->getAttrType(sql.tablename, (*conditioni).at(0));
			break;
		}
	}
    
    // 需要商量
    Table table;
    table.name=sql.tablename;
    table.RecordLength=cm->recordSize(sql.tablename);
    for(int i=0; i<cm->AttrCount(sql.tablename); i++){
        //        table.AttrList();
    }
    
	if(!flag) {
		std::vector<slot> slots;
		result=rm->Select_Record(sql, table, false, slots);
	}
	else{
		Value *v;
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
		IndexManager *im=new IndexManager();
        
        std::vector<slot> s;
		if((*conditioni).at(1)=="<") s=im->_FindSmall((*conditioni).at(0), *v);
        else if((*conditioni).at(1)=="<=") s=im->_FindSmallEqual((*conditioni).at(0), *v);
        else if((*conditioni).at(1)==">") s=im->_FindBigger((*conditioni).at(0), *v);
        else if((*conditioni).at(1)==">=") s=im->_FindBiggerEqual((*conditioni).at(0), *v);
        else if((*conditioni).at(1)=="!=") s=im->_FindNotEqual((*conditioni).at(0), *v);
        result=rm->Select_Record(sql, table, true, s);
		// 然后开始剩下的暴力获取
        sql.conditions.erase(conditioni);
        Recordinfo result2=rm->Select_Record(sql, table, false, s);
        // and
        result.AND(result2);
		delete v;
		delete im;
	}
	delete cm;
	delete rm;
    return result;
}

Recordinfo API::del(sqlcommand& sql){
    CatalogManager *cm=new CatalogManager();
    RecordManager *rm=new RecordManager();
    bool flag=false;
    //	int n=-1;	// 记录第几个是index
    std::vector<std::vector<std::string> >::iterator conditioni;
    Recordinfo result;
    
    int IndexAttrType=0x7fff;
    
    for(conditioni=sql.conditions.begin(); conditioni<sql.conditions.end(); conditioni++){
        flag=cm->hasIndex(sql.tablename, (*conditioni).at(0));
        if(flag) {
            // 为什么catalog的getattrtype()被注释掉了？
            IndexAttrType=cm->getAttrType(sql.tablename, (*conditioni).at(0));
            break;
        }
    }
    
    // 需要商量
    Table table;
    table.name=sql.tablename;
    table.RecordLength=cm->recordSize(sql.tablename);
    for(int i=0; i<cm->AttrCount(sql.tablename); i++){
//        table.AttrList();
    }
    
    if(!flag) {
        std::vector<slot> slots;
        result=rm->Delete_Record(sql, table, false, slots);
    }
    else{
        Value *v;
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
        IndexManager *im=new IndexManager();
        
        std::vector<slot> s;
        if((*conditioni).at(1)=="<") s=im->_FindSmall((*conditioni).at(0), *v);
        else if((*conditioni).at(1)=="<=") s=im->_FindSmallEqual((*conditioni).at(0), *v);
        else if((*conditioni).at(1)==">") s=im->_FindBigger((*conditioni).at(0), *v);
        else if((*conditioni).at(1)==">=") s=im->_FindBiggerEqual((*conditioni).at(0), *v);
        else if((*conditioni).at(1)=="!=") s=im->_FindNotEqual((*conditioni).at(0), *v);
        result=rm->Delete_Record(sql, table, true, s);
        // 然后开始剩下的暴力获取
        sql.conditions.erase(conditioni);
        Recordinfo result2=rm->Select_Record(sql, table, false, s);
        // and
        result.AND(result2);
        delete v;
        delete im;
    }
    delete cm;
    delete rm;
    return result;
}

Recordinfo API::insert(sqlcommand& sql){
	// 为什么insert也要有sqlcommand????
    // 插入index
    RecordManager *rm=new RecordManager();
    IndexManager *im=new IndexManager();
    CatalogManager *cm=new CatalogManager();
    
    Recordinfo result;
    Table table;
    table.name=sql.tablename;
    int block_id;
    int record_id;
    
    // check primary
    std::string primarykey=cm->pkOnTable(sql.tablename);
    if(primarykey=="Table not found in CatalogBlocks!"){
        result=Recordinfo(false, primarykey, Result(), 0);
    }
    else{
//        if(primarykey=="No primary key on this table!"){
//            result=rm->Insert_Record(sql, table, block_id, record_id);
//        }
        for(int i=0; i<sql.colValue.size(); i++){
            if(cm->isUnique(sql.tablename, i)){
                std::string attrname=cm->getAttrName(i);
                if(cm->hasIndex(tablename, attrname)){
                    im->select();
                }
                sqlcommand tempsql=sql;
                tempsql.
            }
            
        }
        
        // catalog manager接口
        im->select(primarykey, );
        
        result=rm->Insert_Record(sql, table, block_id, record_id);
        // 向索引插入value(if needed)
        
        Recordinfo indexresult;
    }
    
    delete rm;
    delete im;
    delete cm;
    return result;
}

Recordinfo API::createTable(sqlcommand& sql){
    CatalogManager *cm=new CatalogManager();
    IndexManager *im=new IndexManager();
    cm->insertTable(sql);
    std::string pk;
    std::string tablename=sql.createTableInfo[0].at(0);
    if((pk=cm->pkOnTable(tablename))!="No primary key on this table!"){
        // 可以创建空的index吧？
        im->CreateIndex(tablename, cm->getAttrType(tablename, pk), std::vector<Value>(), std::vector<slot>());
    }
    delete im;
    delete cm;
    return Recordinfo(); // further improve
}

Recordinfo API::createIndex(sqlcommand& sql){
    CatalogManager *cm=new CatalogManager();
	IndexManager *im=new IndexManager();
	std::string tablename=sql.createIndexInfo[1];
    std::string indexname=sql.createIndexInfo[0];
    int attrtype=cm->getAttrType(tablename, sql.createIndexInfo[2]);
    im->CreateIndex(sql.indexname, attrtype, std::vector<Value>(), std::vector<slot>());
    
    cm->insertIndex(sql);
    delete cm;
    delete im;
}

// turn to Catalog find all index and
// drop index
// dropCatalog
// drop all records
Recordinfo API::dropTable(sqlcommand& sql){
	IndexManager *im=new IndexManager();
    CatalogManager *cm=new CatalogManager();
    
    std::string tablename=sql.tablename;
    
    for(int i=0; i<cm->AttrCount(tablename); i++){
        std::string attrname=cm->getAttrName(i);
        if(cm->hasIndex(tablename, attrname)){
            im->DropIndex(cm->getIndexName(attrname));
            cm->dropIndex(cm->getIndexName(attrname));
        }
    }
    cm->dropTable(sql.tablename);
    delete cm;
    delete im;
}

Recordinfo API::dropIndex(sqlcommand& sql){
    IndexManager *im=new IndexManager();
    CatalogManager *cm=new CatalogManager();
    cm->dropTable(sql.indexname);
    im->DropIndex(sql.indexname);
    // catalog
    delete cm;
    delete im;
}