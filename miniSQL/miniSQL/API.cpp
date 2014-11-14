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
    bool indexflag=false;
    std::vector<std::vector<std::string> >::iterator conditioni;
	Recordinfo result;
    std::vector<slot> slots;
    Table table=catalogmanager->getTable(sql.tablename);

	int IndexAttrType=0x7fff;

	for(conditioni=sql.conditions.begin(); conditioni<sql.conditions.end(); conditioni++){
		flag=catalogmanager->hasIndex(sql.tablename, (*conditioni).at(0));
		if(flag) {
            indexflag=true;
            std::string indexname=catalogmanager->getIndexName(sql.tablename, (*conditioni).at(0));
			IndexAttrType=catalogmanager->getAttrType(sql.tablename, (*conditioni).at(0));
            Value v;
            switch(IndexAttrType){
                case 0:v=Value(IndexAttrType, std::stoi((*conditioni).at(2)));break;
                case 1:v=Value(IndexAttrType, (*conditioni).at(2));break;
                case -1:v=Value(IndexAttrType, std::stof((*conditioni).at(2)));break;
            }
            // search
            std::vector<slot> s;
            if((*conditioni).at(1)=="<") s=indexmanager->_FindSmall(indexname, v);
            else if((*conditioni).at(1)=="<=") s=indexmanager->_FindSmallEqual(indexname, v);
            else if((*conditioni).at(1)==">") s=indexmanager->_FindBigger(indexname, v);
            else if((*conditioni).at(1)==">=") s=indexmanager->_FindBiggerEqual(indexname, v);
            else if((*conditioni).at(1)=="<>") s=indexmanager->_FindNotEqual(indexname, v);
            else if((*conditioni).at(1)=="=") s.push_back(indexmanager->select(indexname, v));
            // AND
            if(conditioni==sql.conditions.begin())
                slots=s;
            else{
                for(std::vector<slot>::iterator i=slots.begin(); i<slots.end(); i++){
                    bool f=false;
                    for(int j=0; j<s.size(); j++) if(i->block_id==s[j].block_id && i->offset==slots[j].offset) f=true;
                    if(f==false)slots.erase(i);
                }
            }
            // erase conditioni?????
		}
	}
	if(!indexflag) {
		std::vector<slot> s;
		result=recordmanager->Select_Record(sql, table, false, s);
	}
    else{
        result=recordmanager->Select_Record(sql, table, true, slots);
        
    }
    return result;
}

Recordinfo API::del(sqlcommand& sql){
    bool flag=false;
    bool indexflag=false;
    std::vector<std::vector<std::string> >::iterator conditioni;
    Recordinfo result;
    std::vector<slot> slots;
    Table table=catalogmanager->getTable(sql.tablename);
    // For RecordManager
//    sql.selectInfo.push_back("*");
    
    int IndexAttrType=0x7fff;
    
    for(conditioni=sql.conditions.begin(); conditioni<sql.conditions.end(); conditioni++){
        flag=catalogmanager->hasIndex(sql.tablename, (*conditioni).at(0));
        if(flag) {
            indexflag=true;
            std::string indexname=catalogmanager->getIndexName(sql.tablename, (*conditioni).at(0));
            IndexAttrType=catalogmanager->getAttrType(sql.tablename, (*conditioni).at(0));
            Value v;
            switch(IndexAttrType){
                case 0:v=Value(IndexAttrType, std::stoi((*conditioni).at(2)));break;
                case 1:v=Value(IndexAttrType, (*conditioni).at(2));break;
                case -1:v=Value(IndexAttrType, std::stof((*conditioni).at(2)));break;
            }
            // search
            std::vector<slot> s;
            if((*conditioni).at(1)=="<") s=indexmanager->_FindSmall(indexname, v);
            else if((*conditioni).at(1)=="<=") s=indexmanager->_FindSmallEqual(indexname, v);
            else if((*conditioni).at(1)==">") s=indexmanager->_FindBigger(indexname, v);
            else if((*conditioni).at(1)==">=") s=indexmanager->_FindBiggerEqual(indexname, v);
            else if((*conditioni).at(1)=="<>") s=indexmanager->_FindNotEqual(indexname, v);
            else if((*conditioni).at(1)=="=") s.push_back(indexmanager->select(indexname, v));
            // AND
            if(conditioni==sql.conditions.begin())
                slots=s;
            else{
                for(std::vector<slot>::iterator i=slots.begin(); i<slots.end(); i++){
                    bool f=false;
                    for(int j=0; j<s.size(); j++) if(i->block_id==s[j].block_id && i->offset==slots[j].offset) f=true;
                    if(f==false)slots.erase(i);
                }
            }
            // erase conditioni?????
        }
    }
    
    if(!indexflag) {
        std::vector<slot> s;
        result=recordmanager->Delete_Record(sql, table, false, s);
    }
    else{
        result=recordmanager->Delete_Record(sql, table, true, slots);
        // delete record in index
        for(int i=0; i<catalogmanager->AttrCount(sql.tablename); i++){
            std::string attrname=catalogmanager->getAttrName(sql.tablename, i);
            std::string indexname=catalogmanager->getIndexName(sql.tablename, attrname);
            int indexattrtype=catalogmanager->getAttrType(sql.tablename, attrname);
            for(int j=1; j<result.res.rows.size(); j++){
                Value v;
                switch(indexattrtype){
                    case 0:v=Value(indexattrtype, std::stoi(result.res.rows.at(j).cols.at(i)));break;
                    case 1:v=Value(indexattrtype, result.res.rows.at(j).cols.at(i));break;
                    case -1:v=Value(indexattrtype, std::stoi(result.res.rows.at(j).cols.at(i)));break;
                }
                indexmanager->_delete(indexname, v);
            }
        }
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
                        Recordinfo result=Recordinfo(false, "The primary key value has existed!", Result(), 0);
                        delete v;
                        return result;
                    }
                    delete v;
                    continue;
                }
                // 没有索引直接查询
                sqlcommand tempsql=sql;
                tempsql.sqlType=0;
                tempsql.conditions={{attrname, "=", sql.colValue[i]}};
                tempsql.selectInfo={"*"};
                std::vector<slot> tempslot=std::vector<slot>();
                Recordinfo r=recordmanager->Select_Record(tempsql, table, 0, tempslot);
                if(r.num!=0){
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
    Recordinfo result=Recordinfo(true, " ", Result(), 0);
    std::string tablename=sql.tablename;
    
    for(int i=0; i<catalogmanager->AttrCount(tablename); i++){
        if(sql.createTableInfo[i+1].at(2)=="1"){
            std::string attrname=sql.createTableInfo[i+1].at(0);
            std::string indexname=tablename+"$"+attrname;
            sqlcommand tempsql=sqlcommand();
            tempsql.sqlType=4;
            tempsql.createIndexInfo.push_back(indexname);
            tempsql.createIndexInfo.push_back(tablename);
            tempsql.createIndexInfo.push_back(attrname);
        }
    }
    return result;
}

Recordinfo API::createIndex(sqlcommand& sql){
	std::string tablename=sql.createIndexInfo[1];
    std::string indexname=sql.createIndexInfo[0];
    std::string attrname=sql.createIndexInfo[2];
//    if(catalogmanager->attrname)
    if(!catalogmanager->isUnique(tablename, attrname)){
        Recordinfo result=Recordinfo(false, "You can only create an index on unique/primary attribute!", Result(), 0);
        return result;
    }
    int attrtype=catalogmanager->getAttrType(tablename, sql.createIndexInfo[2]);
    catalogmanager->insertIndex(sql);
    int valuecharlen=0;
    if(attrtype>0)
        valuecharlen=catalogmanager->getCharLength(tablename, sql.createIndexInfo[2]);
    indexmanager->CreateIndex(sql.indexname, attrtype, std::vector<Value>(), std::vector<slot>(), valuecharlen);
    // insert all records to index
    sqlcommand tempsql=sqlcommand();
    tempsql.sqlType=0;
    tempsql.selectInfo=std::vector<std::string>();
    tempsql.selectInfo.push_back(sql.createIndexInfo[2]);
    tempsql.tablename=tablename;
    tempsql.conditions=std::vector<std::vector<std::string> >();
    Table temptable=catalogmanager->getTable(tablename);
    std::vector<slot> tempslot=std::vector<slot>();
    Recordinfo result=recordmanager->Select_Record(tempsql, temptable, 0, tempslot);
    for(int i=0;i<tempslot.size();i++){
        Value v;
        switch(attrtype){
            case 0:v=Value(attrtype, std::stoi(result.res.rows[i+1].cols[0]));break;
            case 1:v=Value(attrtype, result.res.rows[i+1].cols[0]);break;
            case -1:v=Value(attrtype, std::stof(result.res.rows[i+1].cols[0]));break;
        }
        indexmanager->_insert(indexname, v, tempslot[i]);
    }
    
    return Recordinfo(1, "", Result(), 0); // further improve
}

// turn to Catalog find all index and
// drop index
// dropCatalog
// drop all records
Recordinfo API::dropTable(sqlcommand& sql){
    std::string tablename=sql.tablename;
    
    // delete all records
    sqlcommand tempsql=sqlcommand();
    tempsql.sqlType=1;
    tempsql.conditions=std::vector<std::vector<std::string> >();
    tempsql.tablename=sql.tablename;
    Table temptable=catalogmanager->getTable(sql.tablename);
    recordmanager->Delete_Record(tempsql, temptable, 0, std::vector<slot>());
    
    // delete all indexes
    for(int i=0; i<catalogmanager->AttrCount(tablename); i++){
        std::string attrname=catalogmanager->getAttrName(tablename, i);
        if(catalogmanager->hasIndex(tablename, attrname)){
            indexmanager->DropIndex(catalogmanager->getIndexName(tablename, attrname));
            catalogmanager->dropIndex(catalogmanager->getIndexName(tablename, attrname));
        }
    }
    // delete catalog
    catalogmanager->dropTable(sql.tablename);
    return Recordinfo(1, "", Result(), 0); // further improve
}

Recordinfo API::dropIndex(sqlcommand& sql){
    catalogmanager->dropIndex(sql.indexname);
    indexmanager->DropIndex(sql.indexname);
    return Recordinfo(1, "", Result(), 0); // further improve
    // catalog
}