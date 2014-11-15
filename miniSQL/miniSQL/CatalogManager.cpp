//
//  CatalogManager.cpp
//
//  Created by Sophie on 11/9/14.
//  Copyright (c) 2014 Sophie. All rights reserved.
//


#include "CatalogManager.h"
#include <vector>
#include <iostream>

std::string getStrEle(std::string ch){
    int pos = 0;
    std::string str="";
    while (ch[pos]!='\0'){
        pos++;
        if (pos >= namesize) break;
    }
    for (int j=pos;j<namesize;j++) str += ch[j];
    str += "\0";
    return str;
}

int CatalogManager::getAttrType(std::string tablename,std::string attrname){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
    int blockLen = (contentsize) / tupleLen;
    int t = 0,size;
    size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                char s2[namesize];
                memcpy(&s2, nowblock->content+j*tupleLen+1, sizeof(char)*namesize);
                for (int i=0;i<namesize;i++) if(s2[i]=='\0') break; else s1+=s2[i];
                //s1=getStrEle(s1);
                if(s1==tablename) {
                    std::string s2=_memcpy(j*tupleLen+1+sizeof(char)*namesize, nowblock);
                    if(s2==attrname){
                        memcpy(&t, nowblock->content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int), sizeof(int));
                        if (t>0) return 1;
                        else return t;
                    }
                }
            }
    }
    if(t>0) t=1;
    return t;
}

int CatalogManager::getCharLength(std::string tablename,std::string attrname){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
    int blockLen = (contentsize) / tupleLen;
    int t = 0,size;
    size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                char s2[namesize];
                memcpy(&s2, nowblock->content+j*tupleLen+1, sizeof(char)*namesize);
                for (int i=0;i<namesize;i++) if(s2[i]=='\0') break; else s1+=s2[i];
                //s1=getStrEle(s1);
                if(s1==tablename) {
                    std::string s2=_memcpy(j*tupleLen+1+sizeof(char)*namesize, nowblock);
                    if(s2==attrname)
                        memcpy(&t, nowblock->content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int), sizeof(int));
                }
            }
    }
    
    return t;
}
std::string _memcpy(int i,AttrCatalogBlock* nowblock){
    std::string s2="";
    char s1[namesize];
    memcpy(&s1, nowblock->content+i, sizeof(char)*namesize);
    for (int i=0;i<namesize;i++) if(s1[i]=='\0') break;else s2+=s1[i];
    //s2=getStrEle(s2);
    return s2;
}
std::string _memcpy(int i,TableCatalogBlock* nowblock){
    std::string s2="";
    char s1[namesize];
    memcpy(&s1, nowblock->content+i, sizeof(char)*namesize);
    for (int i=0;i<namesize;i++) if(s1[i]=='\0') break;else s2+=s1[i];
    //    s2=getStrEle(s2);
    return s2;
}
std::string _memcpy(int i,IndexCatalogBlock* nowblock){
    std::string s2="";
    char s1[namesize];
    memcpy(&s1, nowblock->content+i, sizeof(char)*namesize);
    for (int i=0;i<namesize;i++) if(s1[i]=='\0') break;else s2+=s1[i];
    //  s2=getStrEle(s2);
    return s2;
}
///////////////////
int CatalogManager::getAttrType(std::string tablename, int attrindex){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
    int blockLen = (contentsize) / tupleLen;
    int t = 0;
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                
                std::string s1;
                s1=_memcpy(j*tupleLen+1,nowblock);
                if(s1==tablename) {
                    int ii;
                    memcpy(&ii, nowblock->content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int)*2, sizeof(int));
                    if(ii==attrindex)
                        memcpy(&t, nowblock->content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int), sizeof(int));
                }
            }
    }
    if(t>0) t=1;
    return t;
}

int CatalogManager::getCharLength(std::string tablename, int attrindex){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
    int blockLen = (contentsize) / tupleLen;
    int t = 0;
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                
                std::string s1;
                s1=_memcpy(j*tupleLen+1,nowblock);
                if(s1==tablename) {
                    int ii;
                    memcpy(&ii, nowblock->content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int)*2, sizeof(int));
                    if(ii==attrindex)
                        memcpy(&t, nowblock->content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int), sizeof(int));
                }
            }
    }
    return t;
}
std::vector<int> CatalogManager::getAllAttrType(std::string tablename){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
    int blockLen = (contentsize) / tupleLen;
    std::vector<int> type;
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
                if(s1==tablename) {
                    //std::string s2=getStrEle(nowblock,j,1+namesize*sizeof(char),tupleLen);
                    int t;
                    memcpy(&t, nowblock->content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int), sizeof(int));
                    if(t>0) t=1;
                    type.push_back(t);
                }
            }
    }
    return type;
}

int CatalogManager::AttrCount(std::string tablename){
    int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2+1;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    int size = buffermanager->getTableCatalogBlocksNumber();
    //std::vector<int> blocks = getTableCatalogBlocks();
    for(int i=0;i<size;i++){
        TableCatalogBlock* nowblock = buffermanager->getTableCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
                if(s1==tablename) {
                    int c;
                    memcpy(&c, nowblock->content+j*tupleLen+1+sizeof(char)*namesize, sizeof(int));
                    return c;
                }
            }
    }
    return -1;
}
bool CatalogManager::TableExists(std::string tablename){
    int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2+1;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    int size = buffermanager->getTableCatalogBlocksNumber();
    //std::vector<int> blocks = getTableCatalogBlocks();
    for(int i=0;i<size;i++){
        
        TableCatalogBlock* nowblock = buffermanager->getTableCatalogBlocks(i);//???
        // printf("bf3\n");
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1,s2;
                s2=tablename;
                // printf("bf4\n");
                s1=_memcpy(j*tupleLen+1, nowblock);
                // printf("bf5\n");
               // printf("%lu  %lu",s2.length(),s1.length());
                if(s1==s2) {
                    return true;
                }
            }
    }
    return false;
}
bool CatalogManager::IndexExists(std::string indexname){
    int tupleLen = namesize*sizeof(char)*3+1;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    int size = buffermanager->getIndexCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        IndexCatalogBlock* nowblock = buffermanager->getIndexCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
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
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
    int blockLen = (contentsize) / tupleLen;
    
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
                if(s1==tablename) {
                    std::string s2;
                    s2=_memcpy(j*tupleLen+1+sizeof(char)*namesize, nowblock);
                    if(s2==attrname) {
                        return true;
                    }
                }
            }
    }
    return false;
}
bool CatalogManager::hasIndex(std::string tablename, std::string attrname){
    int tupleLen = namesize*sizeof(char)*3+1;
    int blockLen = (contentsize) / tupleLen;
    int size = buffermanager->getIndexCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        IndexCatalogBlock* nowblock = buffermanager->getIndexCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                //std string::s1=getStrEle(nowblock,j,1,tupleLen);
                std::string s2;
                s2=_memcpy(j*tupleLen+1+sizeof(char)*namesize, nowblock);
                std::string s3;
                s3=_memcpy(j*tupleLen+1+sizeof(char)*namesize*2, nowblock);
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
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
    int blockLen = (contentsize) / tupleLen;
    
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
                if(s1==tablename) {
                    int ii;
                    memcpy(&ii, nowblock->content+j*tupleLen+1+namesize*sizeof(char)*2+sizeof(int)*2, sizeof(int));
                    if(ii==attrindex) {
                        int p;
                        memcpy(&p, nowblock->content+j*tupleLen+1+namesize*sizeof(char)*2, sizeof(int));
                        if(p==1)  return true;
                        else return false;
                    }
                }
            }
    }
    return false;
}
bool CatalogManager::isUnique(std::string tablename, int attrindex){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
    int blockLen = (contentsize) / tupleLen;
    
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
                if(s1==tablename) {
                    int ii;
                    memcpy(&ii, nowblock->content+j*tupleLen+1+namesize*sizeof(char)*2+sizeof(int)*2, sizeof(int));
                    if(ii==attrindex) {
                        int p;
                        memcpy(&p, nowblock->content+j*tupleLen+1+namesize*sizeof(char)*2, sizeof(int));
                        if(p==2)  return true;
                        else return false;
                    }
                }
            }
    }
    return false;
    
}
bool CatalogManager::isUnique(std::string tablename, std::string attrname){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
    int blockLen = (contentsize) / tupleLen;
    
    int size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
                if(s1==tablename) {
                    std::string s2;
                    s2=_memcpy(j*tupleLen+1+sizeof(char)*namesize, nowblock);
                    if(s2==attrname) {
                        int p;
                        memcpy(&p, nowblock->content+j*tupleLen+1+namesize*sizeof(char)*2, sizeof(int));
                        if(p==2)  return true;
                        else return false;
                    }
                }
            }
    }
    return false;
    
}
std::string CatalogManager::getAttrName(std::string tablename,int index){
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
    int blockLen = (contentsize) / tupleLen;
    int ii=0,size;
    std::string s2;
    size = buffermanager->getAttrCatalogBlocksNumber();
    bool flag = false;
    for(int i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
                if(s1==tablename) {
                    if (ii==index) {
                        s2=_memcpy(j*tupleLen+1+sizeof(char)*namesize, nowblock);
                        flag = true;
                        break;
                    }
                    ii++;
                }
            }
        if (flag) break;
    }
    return s2;
}
int CatalogManager::recordSize(std::string tablename){
    int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2+1;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    int size = buffermanager->getTableCatalogBlocksNumber();
    //std::vector<int> blocks = getTableCatalogBlocks();
    for(int i=0;i<size;i++){
        TableCatalogBlock* nowblock = buffermanager->getTableCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
                
                if(s1==tablename) {
                    int c;
                    memcpy(&c, nowblock->content+j*tupleLen+1+namesize*sizeof(char)*2+sizeof(int), sizeof(int));
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
        IndexCatalogBlock* nowblock = buffermanager->getIndexCatalogBlocks(i);
        int tupleLen = namesize*sizeof(char)*3+1;
        int blockLen = (contentsize) / tupleLen;
        for (int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
                
                if(s1==indexname) {
                    nowblock->content[j*tupleLen]=Unused;
                    nowblock->is_dirty = true;
                    nowblock->nowcontentsize -= tupleLen;
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
        IndexCatalogBlock* nowblock = buffermanager->getIndexCatalogBlocks(i);
        int tupleLen = namesize*sizeof(char)*3+1;
        int blockLen = (contentsize) / tupleLen;
        for (int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1+sizeof(char)*namesize, nowblock);
                
                if(s1==tablename) {
                    std::string s2;
                    s2=_memcpy(j*tupleLen+1+sizeof(char)*namesize*2, nowblock);
                    
                    if(s2==attrname) {
                        std::string s3;
                        s3=_memcpy(j*tupleLen+1, nowblock);
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
        TableCatalogBlock* nowblock = buffermanager->getTableCatalogBlocks(i);
        int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2+1;
        int blockLen = (contentsize) / tupleLen;
        for (int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
                
                if(s1==tablename) {
                    nowblock->content[j*tupleLen]=Unused;
                    nowblock->is_dirty = true;
                    nowblock->nowcontentsize -= tupleLen;
                    flag1=true;
                }
            }
        //storeTableCatalogBlocks(i,nowblock);
    }
    //attrcatalogblock
    size = buffermanager->getAttrCatalogBlocksNumber();
    for (int i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);
        int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
        int blockLen = (contentsize) / tupleLen;
        for (int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
                
                if(s1==tablename) {
                    nowblock->content[j*tupleLen]=Unused;
                    nowblock->is_dirty = true;
                    nowblock->nowcontentsize -= tupleLen;
                    flag2=true;
                }
            }
        //storeAttrCatalogBlocks(i,nowblock);
    }
    //indexcatalogblock
    size = buffermanager->getIndexCatalogBlocksNumber();
    for (int i=0;i<size;i++){
        IndexCatalogBlock* nowblock = buffermanager->getIndexCatalogBlocks(i);
        int tupleLen = namesize*sizeof(char)*3+1;
        int blockLen = (contentsize) / tupleLen;
        for (int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1+namesize*sizeof(char), nowblock);
                
                if(s1==tablename) {
                    nowblock->content[j*tupleLen]=Unused;
                    nowblock->is_dirty = true;
                    nowblock->nowcontentsize -= tupleLen;
                    flag3=true;
                }
            }
        //storeIndexCatalogBlocks(i,nowblock);
    }
    return (flag1&&flag2&&flag3);
}
std::string CatalogManager::pkOnTable(std::string tablename){
    int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2+1;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    int size = buffermanager->getTableCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        TableCatalogBlock* nowblock = buffermanager->getTableCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1="";
                char s2[namesize];
                memcpy(&s2, nowblock->content+j*tupleLen+1, sizeof(char)*namesize);
                for (int i=0;i<namesize;i++)
                    if (s2[i]=='\0') break; else s1+=s2[i];
                    if(s1==tablename) {
                        std::string s2;
                        s2=_memcpy(j*tupleLen+1+namesize*sizeof(char)+sizeof(int), nowblock);
                    if(s2!="") return s2;
                    else return "No primary key on this table!";
                    // }
                }
            }
    }    return "Table not found in CatalogBlocks!";
}

//tablename,attr#,pkattr,recordlength
void CatalogManager::insertTable(sqlcommand &sql){
    // printf("insertTable\n");
    std::vector<std::vector<std::string> > info=sql.getCreateTableInfo();
    int num = atoi(info[0][1].c_str());
    std::string tablename = info[0][0];
    if(tablename.length()<namesize)
        for (int k=0;k<namesize-(int)tablename.length();k++)
            tablename.insert(0, "\0");
    for(int ii=0;ii<num;ii++){
        if(info[ii+1][0].length()<namesize)
            for (int k=0;k<namesize-(int)info[ii+1][0].length();k++)
                info[ii+1][0].insert(0, "\0");
        insertAttr(tablename,info[ii+1][0],info[ii+1][1],info[ii+1][2],ii);
    }//n p t
    int length=0;
    for(int ii=0;ii<num;ii++){
        if(info[ii+1][2]=="0") length+=sizeof(int);
        else if(info[ii+1][2]=="-1") length+=sizeof(float);
        else length+=atoi(info[ii+1][2].c_str())*sizeof(char);
    }
    //find primary key
    std::string pkattr="";
    int ii=0;
    while(ii<num){
        if(info[ii+1][1]=="1"){pkattr=info[ii+1][0];break;}
        ii++;
    }
    if(pkattr.length()<namesize)
        for (int k=0;k<namesize-(int)pkattr.length();k++)
            pkattr.insert(0, "\0");
    //int tupleLen = namesize*sizeof(char)+num*(sizeof(int)+sizeof(char)*namesize);
    int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2+1;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    int size = buffermanager->getTableCatalogBlocksNumber();
    int i;
    for(i=0;i<size;i++){
        TableCatalogBlock* nowblock = buffermanager->getTableCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++){
            if(nowblock->content[j*tupleLen]==Unused){
                // if(tablename.length()<namesize)
                //     for (int k=0;k<namesize-(int)tablename.length();k++)
                //         tablename.insert(0, "\0");
                memcpy(nowblock->content+j*tupleLen+1, tablename.c_str(), sizeof(char)*namesize);
                memcpy(nowblock->content+j*tupleLen+1+sizeof(char)*namesize,&num,sizeof(int));
                memcpy(nowblock->content+j*tupleLen+1+sizeof(char)*namesize+sizeof(int), pkattr.c_str(), sizeof(char)*namesize);
                memcpy(nowblock->content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int),&length,sizeof(int));
                nowblock->content[j*tupleLen]=used;
                nowblock->is_dirty = true;
                nowblock->nowcontentsize += tupleLen;
                //buffermanager->storeTableCatalogBlocks(i,nowblock);
                return;
            }
        }
    }
    
    TableCatalogBlock* nowblock = buffermanager->newTableCatalogBlocks();
    memcpy(nowblock->content+1, tablename.c_str(), sizeof(char)*namesize);
    memcpy(nowblock->content+1+sizeof(char)*namesize,&num,sizeof(int));
    memcpy(nowblock->content+1+sizeof(char)*namesize+sizeof(int), pkattr.c_str(), sizeof(char)*namesize);
    memcpy(nowblock->content+1+sizeof(char)*namesize*2+sizeof(int),&length,sizeof(int));
    nowblock->nowcontentsize += tupleLen;
    nowblock->is_dirty = true;
    nowblock->content[0] = used;
    //buffermanager->storeTableCatalogBlocks(i,nowblock);
    return;
    
}

//tablen, attrn, attrp, attrt, attrindex
void CatalogManager::insertAttr(std::string tablename,std::string attrn,std::string attrp,std::string attrt,int attrindex){
    
    int tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
    int blockLen = (contentsize) / tupleLen;
    int size = buffermanager->getAttrCatalogBlocksNumber();
    
    int i;
    for(i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++){
            if(nowblock->content[j*tupleLen]==Unused){
                int tmp1=atoi(attrp.c_str());
                int tmp2=atoi(attrt.c_str());
                
                memcpy(nowblock->content+j*tupleLen+1, tablename.c_str(), sizeof(char)*namesize);
                memcpy(nowblock->content+j*tupleLen+1+sizeof(char)*namesize, attrn.c_str(), sizeof(char)*namesize);
                memcpy(nowblock->content+j*tupleLen+1+sizeof(char)*namesize*2, &tmp1, sizeof(int));
                memcpy(nowblock->content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int),&tmp2,sizeof(int));
                memcpy(nowblock->content+j*tupleLen+1+sizeof(char)*namesize*2+sizeof(int)*2,&attrindex,sizeof(int));
                
                nowblock->content[j*tupleLen]=used;
                nowblock->is_dirty = true;
                nowblock->nowcontentsize += tupleLen;
                //buffermanager->storeAttrCatalogBlocks(i,nowblock);
                return;
            }
        }
    }
    
    AttrCatalogBlock* nowblock = buffermanager->newAttrCatalogBlocks();
    int tmp1=atoi(attrp.c_str());
    int tmp2=atoi(attrt.c_str());
    memcpy(nowblock->content+1, tablename.c_str(), sizeof(char)*namesize);
    memcpy(nowblock->content+1+sizeof(char)*namesize, attrn.c_str(), sizeof(char)*namesize);
    memcpy(nowblock->content+1+sizeof(char)*namesize*2, &tmp1, sizeof(int));
    memcpy(nowblock->content+1+sizeof(char)*namesize*2+sizeof(int),&tmp2,sizeof(int));
    memcpy(nowblock->content+1+sizeof(char)*namesize*2+sizeof(int)*2,&attrindex,sizeof(int));
    
    nowblock->nowcontentsize += tupleLen;
    nowblock->is_dirty = true;
    nowblock->content[0] = used;
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
    int tupleLen = namesize*sizeof(char)*3+1;
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    
    int size = buffermanager->getIndexCatalogBlocksNumber();
    int i;
    for(i=0;i<size;i++){
        IndexCatalogBlock* nowblock = buffermanager->getIndexCatalogBlocks(i);//???
        for(int j=0;j<blockLen;j++){
            if(nowblock->content[j*tupleLen]==Unused){
                memcpy(nowblock->content+j*tupleLen+1, indexn.c_str(), sizeof(char)*namesize);
                memcpy(nowblock->content+j*tupleLen+1+sizeof(char)*namesize, tablen.c_str(), sizeof(char)*namesize);
                memcpy(nowblock->content+j*tupleLen+1+sizeof(char)*namesize*2, attrn.c_str(),sizeof(char)*namesize);
                nowblock->content[j*tupleLen]=used;
                nowblock->is_dirty = true;
                nowblock->nowcontentsize += tupleLen;
                //buffermanager->storeIndexCatalogBlocks(i,nowblock);
                return;
            }
        }
    }
    
    IndexCatalogBlock*   nowblock = buffermanager->newIndexCatalogBlocks();
    memcpy(nowblock->content+1, indexn.c_str(), sizeof(char)*namesize);
    memcpy(nowblock->content+1+sizeof(char)*namesize, tablen.c_str(), sizeof(char)*namesize);
    memcpy(nowblock->content+1+sizeof(char)*namesize*2, attrn.c_str(),sizeof(char)*namesize);
    nowblock->content[0]=used;
    nowblock->is_dirty = true;
    nowblock->nowcontentsize += tupleLen;
    //storeIndexCatalogBlocks(i,nowblock);
    return;
}


Table CatalogManager::getTable(std::string tablename){
    Table t;
    std::vector<Attribute> attrlist;
    //tablename,attr#,pkattr,recordlength
    t.name=tablename;
    int tupleLen = namesize*sizeof(char)*2+sizeof(int)*2+1;
    int blockLen = (contentsize) / tupleLen;
    int size = buffermanager->getTableCatalogBlocksNumber();
    bool flag = false;
    for(int i=0;i<size;i++){
        TableCatalogBlock* nowblock = buffermanager->getTableCatalogBlocks(i);
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1, nowblock);
                if(s1==tablename) {
                    int a,b;
                    memcpy(&a, nowblock->content+j*tupleLen+1+namesize*sizeof(char)*2+sizeof(int), sizeof(int));
                    t.RecordLength=a;
                    //std::string pk;//table里需不需要存primary key？
//                    pk=_memcpy(j*tupleLen+1+namesize*sizeof(char)+sizeof(int), nowblock);
                    
                    memcpy(&b, nowblock->content+j*tupleLen+1+namesize*sizeof(char), sizeof(int));
                    t.AttrNum=b;
                    flag = true;
                    break;
                }
            }
        if (flag) break;
    }
    
    //tablen, attrn, attrp, attrt, index
    tupleLen = 2*sizeof(char)*namesize+sizeof(int)*3+1;
    blockLen = (contentsize) / tupleLen;
    int ii=0;
    size = buffermanager->getAttrCatalogBlocksNumber();
    for(int i=0;i<size;i++){
        AttrCatalogBlock* nowblock = buffermanager->getAttrCatalogBlocks(i);
        for(int j=0;j<blockLen;j++)
            if (nowblock->content[j*tupleLen]==used){
                std::string s1;
                s1=_memcpy(j*tupleLen+1,nowblock);
                
                int l;
                memcpy(&l, nowblock->content+j*tupleLen+1+namesize*sizeof(char)*2+sizeof(int)*2, sizeof(int));
                if(s1==tablename&&l==ii) {
                    Attribute attr;
                    std::string s1;
                    attr.name=_memcpy(j*tupleLen+1+namesize*sizeof(char), nowblock);
                    //                    attr.name=getStrEle(s1);
                    int p;
                    memcpy(&p, nowblock->content+j*tupleLen+1+namesize*sizeof(char)*2, sizeof(int));
                    switch(p){
                        case 1: attr.PK=true;break;
                        case 2: attr.UN=true;break;
                    }
                    memcpy(&attr.length, nowblock->content+j*tupleLen+1+namesize*sizeof(char)*2+sizeof(int), sizeof(int));
                   // int l;
                  //  if(attr.datatype>0) l=getCharLength(tablename, attr.name);
                    if(attr.length>0) attr.datatype=1;
                    else attr.datatype=attr.length;
                    attrlist.push_back(attr);
                    ii++;
                     if(ii==t.AttrNum) break;
                }
            }
                            if(ii==t.AttrNum) break;
    }
    t.AttrList=attrlist;
    return t;
}