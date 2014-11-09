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
	return cm->AttrCount(tablename);
}
bool API::TableExists(std::string tablename){
	return cm->TableExists(tablename);
}
bool API::IndexExists(std::string indexname){
	return cm->IndexExists(indexname);
}
bool API::AttrExists(std::string attrname,std::string tablename){
	return cm->AttrExists(attrname, tablename);
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
		flag=cm->hasIndex(sql.tablename, (*conditioni).at(0));
		if(flag) {
			IndexAttrType=cm->getAttrType(sql.tablename, (*conditioni).at(0));
			break;
		}
	}
    
    // 需要商量
    Table table=cm->getTable(sql.tablename);
    
	if(!flag) {
		std::vector<slot> slots;
		result=rm->Select_Record(sql, table, false, slots);
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
        flag=cm->hasIndex(sql.tablename, (*conditioni).at(0));
        if(flag) {
            IndexAttrType=cm->getAttrType(sql.tablename, (*conditioni).at(0));
            break;
        }
    }
    
    // 需要商量
    Table table=cm->getTable(sql.tablename);
    
    if(!flag) {
        std::vector<slot> slots;
        result=rm->Delete_Record(sql, table, false, slots);
    }
    else{
        Value *v=NULL;
        switch(IndexAttrType){
            case 0:v=new Value(IndexAttrType, std::stoi((*conditioni).at(2)));break;
            case 1:v=new Value(IndexAttrType, (*conditioni).at(2));break;
            case -1:v=new Value(IndexAttrType, std::stof((*conditioni).at(2)));break;
        }
        
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
    }
    return result;
}

Recordinfo API::insert(sqlcommand& sql){
	// 为什么insert也要有sqlcommand????
    // 插入index
    
    Recordinfo result;
    Table table=cm->getTable(sql.tablename);
    int block_id;
    int record_id;
    
    // check primary
    std::string primarykey=cm->pkOnTable(sql.tablename);
    if(primarykey=="Table not found in CatalogBlocks!"){
        result=Recordinfo(false, primarykey, Result(), 0);
        return result;
    }
    else{
        for(int i=0; i<sql.colValue.size(); i++){
            if(cm->isUnique(sql.tablename, i) || cm->isPK(sql.tablename, i)){
                std::string attrname=cm->getAttrName(sql.tablename, i);
                // 如果有索引就用索引，并且更新索引
                if(cm->hasIndex(sql.tablename, attrname)){
                    std::string indexname=cm->getIndexName(sql.tablename, attrname);
                    int AttrType=cm->getAttrType(sql.tablename, primarykey);
                    Value *v=NULL;
                    switch(AttrType){
                        case 0:v=new Value(AttrType, std::stoi((sql.colValue).at(2)));break;
                        case 1:v=new Value(AttrType, (sql.colValue).at(2));break;
                        case -1:v=new Value(AttrType, std::stof(sql.colValue.at(2)));break;
                    }
                    slot s=im->select(indexname, *v);
                    if(s.block_id==-1){
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
                Recordinfo r=rm->Select_Record(tempsql, table, 0, std::vector<slot>());
                if(r.successful){
                    result=Recordinfo(false, "The unique key value has existed!", Result(), 0);
                    return result;
                }
            }
        }
        
        result=rm->Insert_Record(sql, table, block_id, record_id);
        //插入索引
        slot s=slot(block_id, record_id);
        for(int i=0; i<sql.colValue.size(); i++){
            std::string attrname=cm->getAttrName(sql.tablename, i);
            if(cm->hasIndex(sql.tablename, attrname)){
                std::string indexname=cm->getIndexName(sql.tablename, attrname);
                int AttrType=cm->getAttrType(sql.tablename, primarykey);
                Value *v=NULL;
                switch(AttrType){
                    case 0:v=new Value(AttrType, std::stoi((sql.colValue).at(2)));break;
                    case 1:v=new Value(AttrType, (sql.colValue).at(2));break;
                    case -1:v=new Value(AttrType, std::stof(sql.colValue.at(2)));break;
                }
                im->_insert(indexname, *v, s);
            }
        }
        // 解决可能的返回错误？
        return result;
    }
}

Recordinfo API::createTable(sqlcommand& sql){
    cm->insertTable(sql);
    std::string pk;
    std::string tablename=sql.createTableInfo[0].at(0);
    if((pk=cm->pkOnTable(tablename))!="No primary key on this table!"){
        // 可以创建空的index吧？
        im->CreateIndex(tablename+"$"+pk, cm->getAttrType(tablename, pk), std::vector<Value>(), std::vector<slot>());
    }
    return Recordinfo(); // further improve
}

Recordinfo API::createIndex(sqlcommand& sql){
	std::string tablename=sql.createIndexInfo[1];
    std::string indexname=sql.createIndexInfo[0];
    int attrtype=cm->getAttrType(tablename, sql.createIndexInfo[2]);
    im->CreateIndex(sql.indexname, attrtype, std::vector<Value>(), std::vector<slot>());
    
    cm->insertIndex(sql);
        return Recordinfo(); // further improve
}

// turn to Catalog find all index and
// drop index
// dropCatalog
// drop all records
Recordinfo API::dropTable(sqlcommand& sql){
    std::string tablename=sql.tablename;
    
    for(int i=0; i<cm->AttrCount(tablename); i++){
        std::string attrname=cm->getAttrName(tablename, i);
        if(cm->hasIndex(tablename, attrname)){
            im->DropIndex(cm->getIndexName(tablename, attrname));
            cm->dropIndex(cm->getIndexName(tablename, attrname));
        }
    }
    cm->dropTable(sql.tablename);
        return Recordinfo(); // further improve
}

Recordinfo API::dropIndex(sqlcommand& sql){
    cm->dropTable(sql.indexname);
    im->DropIndex(sql.indexname);
        return Recordinfo(); // further improve
    // catalog
}