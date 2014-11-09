//
//  CatalogManager.cpp
//
//  Created by Sophie on 11/9/14.
//  Copyright (c) 2014 Sophie. All rights reserved.
//


#include "CatalogManager.h"
#include <vector>
#include <iostream>
//int main(){
//    return 0;
//}

// std::string getStrEle(AttrCatalogBlock &tblock, int j,int p,int tuple_Len){
//     std::string str = "";
//     char ch[namesize];
//     memcpy(ch, tblock.content+j*tuple_Len+p, namesize*sizeof(char));
//     //p += namesize*sizeof(char);

//     // 去掉补全的\0
//     int pos = 0;
//     while (ch[pos]!='\0'){
//         pos++;
//         if (pos >= namesize) break;
//     }
//     for (int j=pos;j<namesize;j++) str += ch[j];
//     str += "\0";
//     return str;
// }

// std::string getStrEle(IndexCatalogBlock &tblock, int j,int p,int tuple_Len){
//     std::string str = "";
//     char ch[namesize];
//     memcpy(ch, tblock.content+j*tuple_Len+p, namesize*sizeof(char));
//     //p += namesize*sizeof(char);

//     // 去掉补全的\0
//     int pos = 0;
//     while (ch[pos]!='\0'){
//         pos++;
//         if (pos >= namesize) break;
//     }
//     for (int j=pos;j<namesize;j++) str += ch[j];
//     str += "\0";
//     return str;
// }

// std::string getStrEle(TableCatalogBlock &tblock, int j,int p,int tuple_Len){
//     std::string str = "";
//     char ch[namesize];
//     memcpy(ch, tblock.content+j*tuple_Len+p, namesize*sizeof(char));
//     //p += namesize*sizeof(char);

//     // 去掉补全的\0
//     int pos = 0;
//     while (ch[pos]!='\0'){
//         pos++;
//         if (pos >= namesize) break;
//     }
//     for (int j=pos;j<namesize;j++) str += ch[j];
//     str += "\0";
//     return str;
// }
std::string getStrEle(std::string ch){
    //int pos = 0;
    std::string str="";
//    while (ch[pos]!='\0'){
//        pos++;
//        if (pos >= namesize) break;
//    }
//    for (int j=pos;j<namesize;j++) str += ch[j];
//    str += "\0";
    return str;
}
int CatalogManager::getAttrType(std::string tablename,std::string attrname){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3;
    int blockLen = (contentsize) / tupleLen;
    int t = 0,size;
    size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    std::string s2;
                    memcpy(&s2, nowblock.content+j*tupleLen+1+namesize*sizeof(char), sizeof(char)*namesize);
                    s2=getStrEle(s2);
                    if(s2==attrname)
                        memcpy(&t, nowblock.content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int), sizeof(int));
                }
            }
    }
    return t;
}
///////////////////
int CatalogManager::getAttrType(std::string tablename, int attrindex){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3;
    int blockLen = (contentsize) / tupleLen;
    int t = 0;
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    int ii;
                    memcpy(&ii, nowblock.content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int)*2, sizeof(int));
                    if(ii==attrindex)
                        memcpy(&t, nowblock.content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int), sizeof(int));
                }
            }
    }
    return t;
}

std::vector<int> CatalogManager::getAllAttrType(std::string tablename){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3;
    int blockLen = (contentsize) / tupleLen;
    std::vector<int> type;
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    //std::string s2=getStrEle(nowblock,j,1+namesize*sizeof(char),tupleLen);
                    int t;
                    memcpy(&t, nowblock.content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int), sizeof(int));
                    type.push_back(t);
                }
            }
    }
    return type;
}

int CatalogManager::AttrCount(std::string tablename){
    int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    int size = buffermanager->getTableCatalogBlocksNumber();
    //std::vector<int> blocks = getTableCatalogBlocks();
    for(int i=0;i<size;i++){
        TableCatalogBlock nowblock = buffermanager->getTableCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    int c;
                    memcpy(&c, nowblock.content+j*tupleLen+1+sizeof(char)*namesize, sizeof(int));
                    return c;
                }
            }
    }
    return -1;
}
bool CatalogManager::TableExists(std::string tablename){
    int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    int size = buffermanager->getTableCatalogBlocksNumber();
    //std::vector<int> blocks = getTableCatalogBlocks();
    for(int i=0;i<size;i++){
        TableCatalogBlock nowblock = buffermanager->getTableCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    return true;
                }
            }
    }
    return false;
}
bool CatalogManager::IndexExists(std::string indexname){
    int tupleLen = namesize*sizeof(char)*3;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    int size = buffermanager->getIndexCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        IndexCatalogBlock nowblock = buffermanager->getIndexCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                //std string::s2=getStrEle(nowblock,j,1+namesize*sizeof(char),tupleLen);
                //std string::s3=getStrEle(nowblock,j,1+namesize*sizeof(char)*2,tupleLen);
                if(s1==indexname) {
                    return true;
                    
                }
            }
    }
    return false;
}
bool CatalogManager::AttrExists(std::string attrname,std::string tablename){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3;
    int blockLen = (contentsize) / tupleLen;
    
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    std::string s2;
                    memcpy(&s2, nowblock.content+j*tupleLen+1+sizeof(char)*namesize, sizeof(char)*namesize);
                    s2=getStrEle(s2);
                    if(s2==attrname) {
                        return true;
                    }
                }
            }
    }
    return false;
}
bool CatalogManager::hasIndex(std::string tablename, std::string attrname){
    int tupleLen = namesize*sizeof(char)*3;
    int blockLen = (contentsize) / tupleLen;
    int size = buffermanager->getIndexCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        IndexCatalogBlock nowblock = buffermanager->getIndexCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                //std string::s1=getStrEle(nowblock,j,1,tupleLen);
                std::string s2;
                memcpy(&s2, nowblock.content+j*tupleLen+1+sizeof(char)*namesize, sizeof(char)*namesize);
                s2=getStrEle(s2);
                std::string s3;
                memcpy(&s3, nowblock.content+j*tupleLen+1+sizeof(char)*namesize*2, sizeof(char)*namesize);
                s3=getStrEle(s3);
                if(s2==tablename) {
                    if(s3==attrname) {
                        return true;
                    }
                }
            }
    }
    return false;
}
bool CatalogManager::isPK(std::string tablename, std::string attrname){
    std::string s=pkOnTable(tablename);
    return s==attrname;
}
bool CatalogManager::isPK(std::string tablename, int attrindex){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3;
    int blockLen = (contentsize) / tupleLen;
    
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    int ii;
                    memcpy(&ii, nowblock.content+j*tupleLen+1+namesize*sizeof(char)*2+sizeof(int)*2, sizeof(int));
                    if(ii==attrindex) {
                        int p;
                        memcpy(&p, nowblock.content+j*tupleLen+1+namesize*sizeof(char)*2, sizeof(int));
                        if(p==2)  return true;
                        else return false;
                    }
                }
            }
    }
    return false;
}
bool CatalogManager::isUnique(std::string tablename, int attrindex){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3;
    int blockLen = (contentsize) / tupleLen;
    
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    int ii;
                    memcpy(&ii, nowblock.content+j*tupleLen+1+namesize*sizeof(char)*2+sizeof(int)*2, sizeof(int));
                    if(ii==attrindex) {
                        int p;
                        memcpy(&p, nowblock.content+j*tupleLen+1+namesize*sizeof(char)*2, sizeof(int));
                        if(p==1)  return true;
                        else return false;
                    }
                }
            }
    }
    return false;
    
}
bool CatalogManager::isUnique(std::string tablename, std::string attrname){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3;
    int blockLen = (contentsize) / tupleLen;
    
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    std::string s2;
                    memcpy(&s2, nowblock.content+j*tupleLen+1+sizeof(char)*namesize, sizeof(char)*namesize);
                    s2=getStrEle(s2);
                    if(s2==attrname) {
                        int p;
                        memcpy(&p, nowblock.content+j*tupleLen+1+namesize*sizeof(char)*2, sizeof(int));
                        if(p==1)  return true;
                        else return false;
                    }
                }
            }
    }
    return false;
    
}
std::string CatalogManager::getAttrName(std::string tablename,int i){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3;
    int blockLen = (contentsize) / tupleLen;
    int ii=0,size;
    std::string s2;
    size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    if (ii==i) {
                        memcpy(&s2, nowblock.content+j*tupleLen+1+sizeof(char)*namesize, sizeof(char)*namesize);
                        s2=getStrEle(s2);
                    }
                    ii++;
                }
            }
    }
    return s2;
}
int CatalogManager::recordSize(std::string tablename){
    int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    int size = buffermanager->getTableCatalogBlocksNumber();
    //std::vector<int> blocks = getTableCatalogBlocks();
    for(int i=0;i<size;i++){
        TableCatalogBlock nowblock = buffermanager->getTableCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    int c;
                    memcpy(&c, nowblock.content+j*tupleLen+1+namesize*sizeof(char)*2+sizeof(int), sizeof(int));
                    return c;
                }
            }
    }
    return -1;
    
}
bool CatalogManager::dropIndex(std::string indexname){
    //indexcatalogblock
    int size = buffermanager->getIndexCatalogBlocksNumber();
    for (int i=0;i<size;i++){
        IndexCatalogBlock nowblock = buffermanager->getIndexCatalogBlocks(i);
        int tupleLen = namesize*sizeof(char)*3;
        int blockLen = (contentsize) / tupleLen;
        for (int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==indexname) {
                    nowblock.content[j*tupleLen]=Unused;
                    nowblock.is_dirty = true;
                    nowblock.nowcontentsize -= tupleLen;
                    //storeIndexCatalogBlocks(i,nowblock);
                    return true;
                }
            }
    }
    return false;
}
std::string CatalogManager::getIndexName(std::string tablename,std::string attrname){
    //indexcatalogblock
    int size = buffermanager->getIndexCatalogBlocksNumber();
    for (int i=0;i<size;i++){
        IndexCatalogBlock nowblock = buffermanager->getIndexCatalogBlocks(i);
        int tupleLen = namesize*sizeof(char)*3;
        int blockLen = (contentsize) / tupleLen;
        for (int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1+sizeof(char)*namesize, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    std::string s2;
                    memcpy(&s2, nowblock.content+j*tupleLen+1+sizeof(char)*namesize*2, sizeof(char)*namesize);
                    s2=getStrEle(s2);
                    if(s2==attrname) {
                        std::string s3;
                        memcpy(&s3, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                        s3=getStrEle(s3);
                        return s3;
                    }
                }
            }
    }
    return "Index not found";
}
bool CatalogManager::dropTable(std::string tablename){
    int size;
    int flag1=false,flag2=false,flag3=false;
    //tablecatalogblock
    size = buffermanager->getTableCatalogBlocksNumber();
    for (int i=0;i<size;i++){
        TableCatalogBlock nowblock = buffermanager->getTableCatalogBlocks(i);
        int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2;
        int blockLen = (contentsize) / tupleLen;
        for (int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    nowblock.content[j*tupleLen]=Unused;
                    nowblock.is_dirty = true;
                    nowblock.nowcontentsize -= tupleLen;
                    flag1=true;
                }
            }
        //storeTableCatalogBlocks(i,nowblock);
    }
    //attrcatalogblock
    size = buffermanager->getAttrCatalogBlocksNumber();
    for (int i=0;i<size;i++){
        AttrCatalogBlock nowblock = buffermanager->getAttrCatalogBlocks(i);
        int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3;
        int blockLen = (contentsize) / tupleLen;
        for (int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    nowblock.content[j*tupleLen]=Unused;
                    nowblock.is_dirty = true;
                    nowblock.nowcontentsize -= tupleLen;
                    flag2=true;
                }
            }
        //storeAttrCatalogBlocks(i,nowblock);
    }
    //indexcatalogblock
    size = buffermanager->getIndexCatalogBlocksNumber();
    for (int i=0;i<size;i++){
        IndexCatalogBlock nowblock = buffermanager->getIndexCatalogBlocks(i);
        int tupleLen = namesize*sizeof(char)*3;
        int blockLen = (contentsize) / tupleLen;
        for (int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1+namesize*sizeof(char), sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    nowblock.content[j*tupleLen]=Unused;
                    nowblock.is_dirty = true;
                    nowblock.nowcontentsize -= tupleLen;
                    flag3=true;
                }
            }
        //storeIndexCatalogBlocks(i,nowblock);
    }
    return (flag1&&flag2&&flag3);
}
std::string CatalogManager::pkOnTable(std::string tablename){
    int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    
    int size = buffermanager->getTableCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        TableCatalogBlock nowblock = buffermanager->getTableCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1, sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    std::string s2;
                    memcpy(&s2, nowblock.content+j*tupleLen+1+namesize*sizeof(char)+sizeof(int), sizeof(char)*namesize);
                    s2=getStrEle(s2);
                    if(s2!="") return s2;
                    else return "No primary key on this table!";
                }
            }
    }
    return "Table not found in CatalogBlocks!";
}

//tablename,attr#,pkattr,recordlength
void CatalogManager::insertTable(sqlcommand &sql){
    std::vector<std::vector<std::string> > info=sql.getCreateTableInfo();
    int num = atoi(info[0][1].c_str());
    std::string tablename = info[0][0];
    if(tablename.length()<namesize)
        for (int k=0;k<namesize-(int)tablename.length();k++)
            tablename.insert(0, "\0");
    for(int ii=0;ii<num;ii++){
        if(info[1][ii].length()<namesize)
            for (int k=0;k<namesize-(int)info[1][ii].length();k++)
                info[1][ii].insert(0, "\0");
        insertAttr(tablename,info[1][ii],info[2][ii],info[3][ii],ii);
    }
    int length=0;
    for(int ii=0;ii<num;ii++){
        if(info[3][ii]=="0") length+=sizeof(int);
        else if(info[3][ii]=="-1") length+=sizeof(float);
        else length+=atoi(info[3][ii].c_str())*sizeof(char);
    }
    //find primary key
    std::string pkattr="";
    int ii=0;
    for(std::vector<std::string>::iterator it = info[2].begin(); it != info[2].end(); it++){
        if(*it=="2"){pkattr=info[1][ii];break;}
        ii++;
    }
    if(pkattr.length()<namesize)
        for (int k=0;k<namesize-(int)pkattr.length();k++)
            pkattr.insert(0, "\0");
    
    //int tupleLen = namesize*sizeof(char)+num*(sizeof(int)+sizeof(char)*namesize);
    int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    int size = buffermanager->getTableCatalogBlocksNumber();
    int i;
    for(i=0;i<size;i++){
        TableCatalogBlock nowblock = buffermanager->getTableCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++){
            if(nowblock.content[j*tupleLen]==Unused){
                // if(tablename.length()<namesize)
                //     for (int k=0;k<namesize-(int)tablename.length();k++)
                //         tablename.insert(0, "\0");
                memcpy(nowblock.content+j*tupleLen+1, tablename.c_str(), sizeof(char)*namesize);
                memcpy(nowblock.content+j*tupleLen+1+sizeof(char)*namesize*2,&num,sizeof(int));
                memcpy(nowblock.content+j*tupleLen+1+sizeof(char)*namesize, pkattr.c_str(), sizeof(char)*namesize);
                memcpy(nowblock.content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int),&length,sizeof(int));
                nowblock.content[j*tupleLen]=used;
                nowblock.is_dirty = true;
                nowblock.nowcontentsize += tupleLen;
                //storeTableCatalogBlocks(i,nowblock);
                return;
            }
        }
    }
    
    TableCatalogBlock nowblock = buffermanager->newTableCatalogBlocks();
    memcpy(nowblock.content+1, tablename.c_str(), sizeof(char)*namesize);
    memcpy(nowblock.content+1+sizeof(char)*namesize,&num,sizeof(int));
    memcpy(nowblock.content+1+sizeof(char)*namesize, pkattr.c_str(), sizeof(char)*namesize);
    memcpy(nowblock.content+1+sizeof(char)*namesize*2+sizeof(int),&length,sizeof(int));
    nowblock.nowcontentsize += tupleLen;
    nowblock.is_dirty = true;
    nowblock.content[0] = used;
    //storeTableCatalogBlocks(i,nowblock);
    return;
    
}

//tablen, attrn, attrp, attrt, attrindex
void CatalogManager::insertAttr(std::string tablename,std::string attrn,std::string attrp,std::string attrt,int attrindex){
    
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3;
    int blockLen = (contentsize) / tupleLen;
    int size = buffermanager->getAttrCatalogBlocksNumber();
    int i;
    for(i=0;i<size;i++){
        AttrCatalogBlock nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++){
            if(nowblock.content[j*tupleLen]==Unused){
                int tmp1=atoi(attrp.c_str());
                int tmp2=atoi(attrt.c_str());
                memcpy(nowblock.content+j*tupleLen+1, tablename.c_str(), sizeof(char)*namesize);
                memcpy(nowblock.content+j*tupleLen+sizeof(char)*namesize, attrn.c_str(), sizeof(char)*namesize);
                memcpy(nowblock.content+j*tupleLen+1+sizeof(char)*namesize*2, &tmp1, sizeof(int));
                memcpy(nowblock.content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int),&tmp2,sizeof(int));
                memcpy(nowblock.content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int)*2,&attrindex,sizeof(int));
                
                nowblock.content[j*tupleLen]=used;
                nowblock.is_dirty = true;
                nowblock.nowcontentsize += tupleLen;
                //storeAttrCatalogBlocks(i,nowblock);
                return;
            }
        }
    }
    
    AttrCatalogBlock nowblock = buffermanager->newAttrCatalogBlocks();
    int tmp1=atoi(attrp.c_str());
    int tmp2=atoi(attrt.c_str());
    memcpy(nowblock.content+1, tablename.c_str(), sizeof(char)*namesize);
    memcpy(nowblock.content+sizeof(char)*namesize, attrn.c_str(), sizeof(char)*namesize);
    memcpy(nowblock.content+1+sizeof(char)*namesize*2, &tmp1, sizeof(int));
    memcpy(nowblock.content+1+sizeof(char)*namesize*2+sizeof(int),&tmp2,sizeof(int));
    memcpy(nowblock.content+1+sizeof(char)*namesize*2+sizeof(int)*2,&attrindex,sizeof(int));
    
    nowblock.nowcontentsize += tupleLen;
    nowblock.is_dirty = true;
    nowblock.content[0] = used;
    //storeAttrCatalogBlocks(i,nowblock);
    return;
}
//indexname, tablename, attrname
void CatalogManager::insertIndex(sqlcommand &sql){
    std::vector<std::string> info=sql.getCreateIndexInfo();
    std::string indexn=info[0];
    std::string tablen=info[1];
    std::string attrn=info[2];
    
    if(indexn.length()<namesize)
        for (int k=0;k<namesize-(int)indexn.length();k++)
            indexn.insert(0, "\0");
    if(tablen.length()<namesize)
        for (int k=0;k<namesize-(int)tablen.length();k++)
            indexn.insert(0, "\0");
    if(attrn.length()<namesize)
        for (int k=0;k<namesize-(int)attrn.length();k++)
            indexn.insert(0, "\0");
    
    //int tupleLen = namesize*sizeof(char)+num*(sizeof(int)+sizeof(char)*namesize);
    int tupleLen = namesize*sizeof(char)*3;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    
    int size = buffermanager->getIndexCatalogBlocksNumber();
    int i;
    for(i=0;i<size;i++){
        IndexCatalogBlock nowblock = buffermanager->getIndexCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++){
            if(nowblock.content[j*tupleLen]==Unused){
                memcpy(nowblock.content+j*tupleLen+1, indexn.c_str(), sizeof(char)*namesize);
                memcpy(nowblock.content+j*tupleLen+1+sizeof(char)*namesize, tablen.c_str(), sizeof(char)*namesize);
                memcpy(nowblock.content+j*tupleLen+1+sizeof(char)*namesize*2, attrn.c_str(),sizeof(char)*namesize);
                nowblock.content[j*tupleLen]=used;
                nowblock.is_dirty = true;
                nowblock.nowcontentsize += tupleLen;
                //storeIndexCatalogBlocks(i,nowblock);
                return;
            }
        }
    }
    
    IndexCatalogBlock nowblock = buffermanager->newIndexCatalogBlocks();
    memcpy(nowblock.content+1, indexn.c_str(), sizeof(char)*namesize);
    memcpy(nowblock.content+1+sizeof(char)*namesize, tablen.c_str(), sizeof(char)*namesize);
    memcpy(nowblock.content+1+sizeof(char)*namesize*2, attrn.c_str(),sizeof(char)*namesize);
    nowblock.content[0]=used;
    nowblock.is_dirty = true;
    nowblock.nowcontentsize += tupleLen;
    //storeIndexCatalogBlocks(i,nowblock);
    return;
}


Table CatalogManager::getTable(std::string tablename){
    Table t;
    std::vector<Attribute> attrlist;
    //tablename,attr#,pkattr,recordlength
    t.name=tablename;
    int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2;
    int blockLen = (contentsize) / tupleLen;
    int size = buffermanager->getTableCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        TableCatalogBlock nowblock = buffermanager->getTableCatalogBlocks(i);
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1+namesize*sizeof(char), sizeof(char)*namesize);
                s1=getStrEle(s1);
                if(s1==tablename) {
                    memcpy(&t.RecordLength, nowblock.content+j*tupleLen+1+namesize*sizeof(char)*2+sizeof(int), sizeof(int));
                    std::string pk;//table里需不需要存primary key？
                    memcpy(&pk, nowblock.content+j*tupleLen+1+namesize*sizeof(char)+sizeof(int), sizeof(char)*namesize);
                    pk=getStrEle(pk);
                    memcpy(&t.AttrNum, nowblock.content+j*tupleLen+1+namesize*sizeof(char), sizeof(int));
                }
            }
    }
    
    //tablen, attrn, attrp, attrt, attrindex
    tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3;
    blockLen = (contentsize) / tupleLen;
    int ii=0;
    size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock nowblock = buffermanager->getAttrCatalogBlocks(i);
        for(int j=0;j<blockLen;j++)
            if (nowblock.content[j*tupleLen]==used){
                std::string s1;
                memcpy(&s1, nowblock.content+j*tupleLen+1+namesize*sizeof(char), sizeof(char)*namesize);
                s1=getStrEle(s1);
                int j;
                memcpy(&j, nowblock.content+j*tupleLen+1+namesize*sizeof(char)*2+sizeof(int)*2, sizeof(int));
                if(s1==tablename&&j==ii) {
                    Attribute attr;
                    std::string s1;
                    memcpy(&s1, nowblock.content+j*tupleLen+1+namesize*sizeof(char), sizeof(char)*namesize);
                    attr.name=getStrEle(s1);
                    int p;
                    memcpy(&p, nowblock.content+j*tupleLen+1+namesize*sizeof(char)*2, sizeof(int));
                    switch(p){
                        case 1: attr.PK=true;break;
                        case 2: attr.UN=true;break;
                    }
                    memcpy(&attr.datatype, nowblock.content+j*tupleLen+1+namesize*sizeof(char)*2+sizeof(int), sizeof(int));
                    //                    switch(attr.datatype){
                    //                        case 0: attr.length=sizeof(int);break;
                    //                        case -1: attr.length=sizeof(float);break;
                    //                        default: attr.length=sizeof(char)*attr.datatype;break;
                    //                    }
                    attrlist.push_back(attr);
                    ii++;
                }
            }
    }
    t.AttrList=attrlist;
    return t;
}