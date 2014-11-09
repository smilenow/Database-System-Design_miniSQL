
#ifndef __miniSQL__CatalogManager__
#define __miniSQL__CatalogManager__

#include <stdio.h>
#include <iostream>
#include "Block.h"
#include "Base.h"
#include "sqlcommand.h"
#define Unused 0x00
#define used 0x01
#define namesize 64
const static int contentsize = block_size-sizeof(Block)-sizeof(int);

//table:name, attr#, indexontable
//char(64)+int+int*#+char(64)*#+int*#+char(64)
class TableCatalogBlock:public Block{
public:
    TableCatalogBlock():Block(){};
public:
    char content[contentsize];
    int nowcontentsize;
};
//tablen, attrn, attrp, attrt, attrindex
class AttrCatalogBlock:public Block{
public:
    AttrCatalogBlock():Block(){};
   
public:
    char content[contentsize];
    int nowcontentsize;
};
//index:table, attrname
//char(64)+char(64)+char(64)
class IndexCatalogBlock:public Block{
public:
    IndexCatalogBlock():Block(){};
public:
    char content[contentsize];
    int nowcontentsize;
};

class CatalogManager{
public:
	std::string getAttrName(std::string tablename,int i);
	bool dropTable(std::string tablename);
	bool dropIndex(std::string indexname);
	std::string getIndexName(std::string tablename, std::string attrname);
	int recordSize(std::string table);
public:
	CatalogManager(){};
	virtual ~CatalogManager(){};
	bool TableExists(std::string tablename);
	bool IndexExists(std::string index);
	bool AttrExists(std::string attrname,std::string tablename);
	//std::vector<std::string> > getAttrType(std::string tablename);
	int getAttrType(std::string tablename,std::string attrname);
	std::vector<int> getAllAttrType(std::string tablename);
	int AttrCount(std::string tablename);
	bool hasIndex(std::string tablename, std::string attrname);
	bool isPK(std::string tablename, int attrindex);
	bool isPK(std::string tablename, std::string attrname);
	bool isUnique(std::string tablename, int attrindex);
	bool isUnique(std::string tablename, std::string attrname);
	std::string pkOnTable(std::string tablename);
	void insertIndex(sqlcommand &sql);
	void insertAttr(std::string tablename,std::string attrn,std::string attrp,std::string attrt,int attrindex);
	void insertTable(sqlcommand &sql);
	std::string getStrEle(IndexCatalogBlock &tblock, int j,int p,int tuple_Len);
	std::string getStrEle(AttrCatalogBlock &tblock, int j,int p,int tuple_Len);
	std::string getStrEle(TableCatalogBlock &tblock, int j,int p,int tuple_Len);
};
#endif