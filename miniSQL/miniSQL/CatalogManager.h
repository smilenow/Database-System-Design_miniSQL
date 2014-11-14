//
//  catalogmanager.h
//  test
//
//  Created by Sophie on 11/9/14.
//  Copyright (c) 2014 Sophie. All rights reserved.
//

#ifndef __miniSQL__CatalogManager__
#define __miniSQL__CatalogManager__

#include <stdio.h>
#include <vector>
#include "block.h"
#include "Base.h"
#include "sqlcommand.h"
#include "BufferManager.h"
#define Unused 0x00
#define used 0x01
#define namesize 64

extern BufferManager *buffermanager;

//tablename,attr#,pkattr,recordlength
//char(64)+int+char(64)+int

std::string getStrEle(std::string s);
std::string _memcpy(int i,AttrCatalogBlock* nowblock);
std::string _memcpy(int i,TableCatalogBlock* nowblock);
std::string _memcpy(int i,IndexCatalogBlock* nowblock);

class CatalogManager{
public:
    CatalogManager(){};
    virtual ~CatalogManager(){};
    // std::string getStrEle(IndexCatalogBlock &tblock, int j,int p,int tuple_Len);
    // std::string getStrEle(AttrCatalogBlock &tblock, int j,int p,int tuple_Len);
    // std::string getStrEle(TableCatalogBlock &tblock, int j,int p,int tuple_Len);
    bool TableExists(std::string tablename);
    bool IndexExists(std::string index);
    bool AttrExists(std::string attrname,std::string tablename);
    //std::vector<std::string> > getAttrType(std::string tablename);
    int getAttrType(std::string tablename,std::string attrname);
    int getCharLength(std::string tablename,std::string attrname);
    int getAttrType(std::string tablename, int attrindex);
    int getCharLength(std::string tablename, int attrindex);
    int AttrCount(std::string tablename);
    bool hasIndex(std::string tablename, std::string attrname);
    bool isPK(std::string tablename, int attrindex);
    bool isPK(std::string tablename, std::string attrname);
    bool isUnique(std::string tablename, int attrindex);
    bool isUnique(std::string tablename, std::string attrname);
    std::string pkOnTable(std::string tablename);
    void insertAttr(std::string tablename,std::string attrn,std::string attrp,std::string attrt,int attrindex);
    void insertIndex(sqlcommand &sql);
    bool dropIndex(std::string indexname);
    void insertTable(sqlcommand &sql);
    bool dropTable(std::string tablename);
    std::string getAttrName(std::string tablename,int i);
    std::string getIndexName(std::string tablename,std::string attrname);
    int recordSize(std::string table);
    Table getTable(std::string tablename);
    std::vector<int> getAllAttrType(std::string tablename);
    };

#endif
