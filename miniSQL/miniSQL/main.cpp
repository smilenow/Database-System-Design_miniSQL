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
API *api=new API();
void printResult(Result res){
    Row row;
    //Result::iterator iter;
    std::vector<Row>::iterator iter;
    for(iter = res.rows.begin(); iter != res.rows.end(); iter++)  {
        printf("|");
        for(std::vector<std::string>::iterator it = (*iter).cols.begin(); it != (*iter).cols.end(); it++)
            printf(" %s\t",(char *)(*it).c_str());
        printf("|\n");
    }
}

void printRecordInfo(std::string s,Recordinfo info){
    if(info.getSucc()){
        printf("%s succeeded!%s\n",s.c_str(),info.getMessage().c_str());
        printResult(info.getRes());
    }
    else{
        printf("%s failed!%s\n",s.c_str(),info.getMessage().c_str());
    }
}
int main(int argc, const char * argv[]) {
    sqlcommand sql;
    //create
    sql.sqlType=3;
    std::string n[2]={"a","b"};
    int p[2]={1,0};
    int t[2]={0,0};
    sql.setCreateTableInfo("table",2,n,p,t);
    api->dealCmd(sql);
    
    //insert
    sql.clear();
    sql.sqlType=2;
    std::string s="1";
    sql.setcolValue(s);
    s="2";
    sql.setcolValue(s);
    sql.tablename="table";
    api->dealCmd(sql);
//delete
    // sql.sqlType=1;
   // api->dealCmd(sql);
    
    //select
    sql.sqlType=0;
    sql.setSelectInfo("*");
    sql.tablename="table";
    //sql.setconditions("a", "=", "1");
    printRecordInfo("Select",api->select(sql));
    sql.sqlType=5;
    api->dealCmd(sql);
//    sql.setconditions(<#std::string attrname#>, <#std::string op#>, <#std::string s#>)
    
    sql.clear();
    sql.sqlType=2;
    s="1";
    sql.setcolValue(s);
    s="2";
    sql.setcolValue(s);
    sql.tablename="table";
    api->dealCmd(sql);
    printf("over");
    return 0;
}
