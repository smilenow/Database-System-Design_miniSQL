//
//  RecordManager.cpp
//  miniSQL
//
//  Created by SmilENow on 10/25/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#include "RecordManager.h"

void RecordManager::ReturnRes(Row &tuple, Result &results, std::vector<int> ColToReturn){
    Row nowrow;
    for (auto &i : ColToReturn)
        nowrow.cols.push_back(tuple.cols[i]);
    results.rows.push_back(nowrow);
}

bool RecordManager::check(Row &tuple, std::vector<Attribute> &attrs, std::vector<std::vector<std::string> > &conditions){
    int pos,cond_type;
    
    for (auto &i:conditions){
        pos=0;
        for (int j=0;j<attrs.size();j++){
            if (!i[0].compare(attrs[j].name)){
                pos = j;
                break;
            }
        }
        // change i[1] op into int type : == > >= < <= <>
        if (!i[1].compare("==")) cond_type = 1;
        else if (!i[1].compare(">")) cond_type = 2;
        else if (!i[1].compare(">=")) cond_type = 3;
        else if (!i[1].compare("<")) cond_type = 4;
        else if (!i[1].compare("<=")) cond_type = 5;
        else cond_type=6;
        //
        switch (attrs[pos].datatype){
            case 0:{                // int
                int v1 = atoi(tuple.cols[pos].c_str());
                int v2 = atoi(i[2].c_str());
                switch (cond_type){
                    case 1: if (v1!=v2) return false; break;
                    case 2: if (v1<=v2) return false; break;
                    case 3: if (v1<v2) return false; break;
                    case 4: if (v1>=v2) return false; break;
                    case 5: if (v1>v2) return false; break;
                    case 6: if (v1==v2) return false; break;
                    default: break;
                }
            }
            break;
            case -1:{               // float
                float v1 = atof(tuple.cols[pos].c_str());
                float v2 = atof(i[2].c_str());
                switch (cond_type){
                    case 1: if (v1!=v2) return false; break;
                    case 2: if (v1<=v2) return false; break;
                    case 3: if (v1<v2) return false; break;
                    case 4: if (v1>=v2) return false; break;
                    case 5: if (v1>v2) return false; break;
                    case 6: if (v1==v2) return false; break;
                    default: break;
                }
            }
            break;
            case 1:{                // string
                std::string v1 = tuple.cols[pos];
                std::string v2 = i[2];
                switch (cond_type){
                    case 1: if (v1!=v2) return false; break;
                    case 2: if (v1<=v2) return false; break;
                    case 3: if (v1<v2) return false; break;
                    case 4: if (v1>=v2) return false; break;
                    case 5: if (v1>v2) return false; break;
                    case 6: if (v1==v2) return false; break;
                    default: break;
                }
            }
            break;
        }
    }
    return true;
}

Recordinfo RecordManager::write(RecordBlock &rblock, int j, int tuple_Len, std::vector<Attribute> &attrs, std::vector<std::string> &colValue){
    
    int p=1; // j*tuple_Len 记录使用，所以从1开始
    std::string str;
    Result results;
    
    for (int i=0;i<attrs.size();i++){
        switch (attrs[i].datatype){
            case 0:{                // int
                int x = atoi(colValue[i].c_str());
                memcpy(rblock.content+j*tuple_Len+p, &x, sizeof(int));
                p += sizeof(int);
            }
            break;
            case -1:{               // float
                float x = atof(colValue[i].c_str());
                memcpy(rblock.content+j*tuple_Len+p, &x, sizeof(float));
                p += sizeof(float);
            }
            break;
            case 1:{
                str = colValue[i];
                if ((str.length())>attrs[i].length)
                    return Recordinfo(false, "Insertion failed. And "+attrs[i].name+" is illegal in length.", results, 0);
                // 如果string长度少于表定义的长度,补全
                if ((str.length())<attrs[i].length){
                    for (int k=0;k<attrs[i].length-(int)str.length();k++)
                        str.insert(0, "\0");
                }
                memcpy(rblock.content+j*tuple_Len+p, str.c_str(), attrs[i].length+1);
                p += attrs[i].length+1;
            }
            break;
            default: break;
        }
    }
    rblock.content[j*tuple_Len]=used;
    rblock.is_dirty = true;
    return Recordinfo(true,"Insertion succeeded",results,1);
}

void RecordManager::getOneTuple(RecordBlock &rblock, int j, int tuple_Len, std::vector<Attribute> &attrs, Row &tuple){
    
    int p=1;
    std::stringstream ss;
    char ch[max_char_length];
    std::string str;
    
    for (auto &i:attrs){
        switch (i.datatype){
            case 0:{                // int
                int x;
                memcpy(&x, rblock.content+j*tuple_Len+p, sizeof(int));
                p += sizeof(int);
                ss << x;
                tuple.cols.push_back(ss.str());
            }
            break;
            case -1:{               // float
                float x;
                memcpy(&x, rblock.content+j*tuple_Len+p, sizeof(float));
                p += sizeof(float);
                ss << x;
                tuple.cols.push_back(ss.str());
            }
            break;
            case 1:{                // string
                memcpy(ch, rblock.content+j*tuple_Len+p, i.length+1);
                p += i.length+1;
                str = "";
                //
                // 去掉补全的\0
                int pos = 0;
                while (ch[pos]!='\0'){
                    pos++;
                    if (pos >= i.length) break;
                }
                for (int j=pos;j<i.length;j++) str += ch[j];
                str += "\0";
                tuple.cols.push_back(str);
            }
            break;
            default: break;
        }
    }
}

Recordinfo RecordManager::Select_Record(sqlcommand &sql, Table &table, bool indexflag, std::vector<slot> slots){
    
    std::vector<Attribute> attrs = table.AttrList;              // 表的属性
    std::string TableName = table.name;                         // 表的名字
    std::vector<std::vector<std::string> > conditions = sql.getconditions();    // 查询条件
    std::vector<std::string> what_to_select = sql.GetSelectInfo();  // 需要查询的内容
    std::vector<int> what_to_select_indexof_attrs;                  // 需要查找的内容对应在attrs的index
    std::vector<int> blocks = buffermanager->getTableblock(TableName);  // 获取table下的block
    
    int tupleLen = table.RecordLength + 1;                      // 每个记录长度
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    long num=0;
    bool successful = false;
    
    // 如果是select *, 那就补全
    if (what_to_select[0] == "*"){
        what_to_select.clear();
        for (auto &i:attrs) what_to_select.push_back(i.name);
    }
    
    // 记录查询内容在attrs整个表中的下标
    what_to_select_indexof_attrs.clear();
    for (auto &i:what_to_select){
        for (int j=0;j<attrs.size();j++)
            if (!i.compare(attrs[j].name)) what_to_select_indexof_attrs.push_back(j);
    }
    
    Row row; // 每一行
    Result res; // 最后的结果
    
    // 保存查找属性作为第一行的结果
    row.cols = what_to_select;
    res.rows.push_back(row);
    
    // 判断有没有where
    bool Exist_Where = false;
    
    if (indexflag){
        int block_id,record_id;
        for (auto &i:slots){
            record_id = i.offset;
            block_id = i.block_id;
            RecordBlock &nowblock = buffermanager->getBlocks(DB,TableName,block_id);
            if (nowblock.content[record_id*tupleLen]==used){
                Row nowtuple;
                getOneTuple(nowblock, record_id, tupleLen, attrs, nowtuple);
                if (!Exist_Where || (Exist_Where && check(nowtuple, attrs, conditions))){
                    ReturnRes(nowtuple, res, what_to_select_indexof_attrs);
                    num++;
                    successful = true;
                }
            }
        }
    }
    else {
        if (conditions[0].size()) Exist_Where = true;
        for (int i=0;i<blocks.size();i++){
            RecordBlock &nowblock = buffermanager->getBlocks(DB,TableName,i);
            for (int j=0;j<blockLen;j++)
                if (nowblock.content[j*tupleLen]==used){
                    Row nowtuple;
                    getOneTuple(nowblock, j, tupleLen, attrs, nowtuple);
                    if (!Exist_Where || (Exist_Where && check(nowtuple, attrs, conditions))){
                        ReturnRes(nowtuple, res, what_to_select_indexof_attrs);
                        num++;
                        successful = true;
                    }
                }
        }
    }
    
    std::string message = "Selection succeeded.";
    if (num==0) {successful=false; message="The result is null.";}
    return Recordinfo(successful, message, res, num);
}

Recordinfo RecordManager::Delete_Record(sqlcommand &sql, Table &table, bool indexflag, std::vector<slot> slots){

    std::vector<Attribute> attrs = table.AttrList;              // 表的属性
    std::string TableName = table.name;                         // 表的名字
    std::vector<std::vector<std::string> > conditions = sql.getconditions();    // 查询条件
    std::vector<std::string> what_to_select = sql.GetSelectInfo();  // 需要查询的内容
    std::vector<int> what_to_select_indexof_attrs;                  // 需要查找的内容对应在attrs的index
    std::vector<int> blocks = buffermanager->getTableblock(TableName);  // 获取table下的block
    
    int tupleLen = table.RecordLength + 1;                      // 每个记录长度
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    long num=0;
    bool successful = false;
    
    Row row; // 每一行
    Result res; // 最后的结果
    
    // 判断有没有where
    bool Exist_Where = false;
    
    if (indexflag){
        int block_id,record_id;
        for (auto &i : slots){
            record_id = i.offset;
            block_id = i.block_id;
            RecordBlock &nowblock = buffermanager->getBlocks(DB,TableName,block_id);
            if (nowblock.content[record_id*tupleLen]==used){
                Row nowtuple;
                getOneTuple(nowblock, record_id, tupleLen, attrs, nowtuple);
                if (!Exist_Where || (Exist_Where && check(nowtuple, attrs, conditions))){
                    nowblock.content[record_id*tupleLen]=Unused;
                    nowblock.is_dirty = true;
                    nowblock.nowcontentsize -= tupleLen;
                    successful = true;
                    num++;
                }
            }
            buffermanager->storeBlocks(TableName,i,nowblock);
        }
    }
    else {
        if (conditions[0].size()) Exist_Where = true;
        for (int i=0;i<blocks.size();i++){
            RecordBlock &nowblock = buffermanager->getBlocks(TableName,i);
            for (int j=0;j<blockLen;j++)
                if (nowblock.content[j*tupleLen]==used){
                    Row nowtuple;
                    getOneTuple(nowblock, j, tupleLen, attrs, nowtuple);
                    if (!Exist_Where || (Exist_Where && check(nowtuple, attrs, conditions))){
                        nowblock.content[j*tupleLen]=Unused;
                        nowblock.is_dirty = true;
                        nowblock.nowcontentsize -= tupleLen;
                        num++;
                        successful = true;
                    }
                }
            buffermanager->storeBlocks(TableName,nowblock);
        }
    }
    
    std::stringstream ss;
    ss << num;
    std::string message;
    if (num==0) message = "There is no record in deletion.";
    else if (num==1) message = "There is 1 record in deletion";
    else message = "There are " + ss.str() + " records in deletion";
    return Recordinfo(successful, message, res, num);
}

Recordinfo RecordManager::Insert_Record(sqlcommand &sql, Table &table, int &block_id, int &record_id){
    
    std::vector<Attribute> attrs = table.AttrList;              // 表的属性
    std::string TableName = table.name;                         // 表的名字
    std::vector<std::string> colValue = sql.getcolValue();      // 需要插入的record值
    std::vector<int> blocks = buffermanager->getTableblock(TableName);  // 获取该table下的block
    Recordinfo nowrinfo;                                        // 当前写入记录的返回结果
    
    int tupleLen = table.RecordLength + 1;                      // 每个记录长度
    int blockLen = (contentsize) / tupleLen;       // 一个block最多放多少个记录
    
    int i;
    for (i=0;i<blocks.size();i++){
        RecordBlock & nowblock = buffermanager->getBlocks(TableName,i);
        for (int j=0;j<blockLen;j++){
            if (nowblock.content[j*tupleLen]==Unused){          // 找到没有使用过的tuple可以插入记录
                nowrinfo = write(nowblock, j, tupleLen, attrs, colValue);
                if (nowrinfo.getSucc()){                        // 成功写入
                    nowblock.content[j*tupleLen]=used;
                    nowblock.is_dirty = true;
                    nowblock.nowcontentsize += tupleLen;
                    buffermanager->storeBlocks(TableName,nowblock);
                    block_id = i;
                    record_id = j;
                    return nowrinfo;
                }
                else return nowrinfo;                       // 这个else有待商榷
            }
        }
    }
    
    // 循环完代表写不下了
    RecordBlock nowblock(i,TableName.c_str());
    nowrinfo = write(nowblock, 0, tupleLen, attrs, colValue);
    if (nowrinfo.getSucc()){
        nowblock.nowcontentsize += tupleLen;
        nowblock.is_dirty = true;
        nowblock.content[0] = used;
        block_id = i;
        record_id = 0;
        buffermanager->storeBlocks(TableName,i,nowblock);
        return nowrinfo;
    }
    
    Result res;
    return Recordinfo(false, "Insert Unsucceeded.", res, 0);
}
