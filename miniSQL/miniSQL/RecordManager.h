//
//  RecordManager.h
//  miniSQL
//
//  Created by SmilENow on 10/25/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#ifndef __miniSQL__RecordManager__
#define __miniSQL__RecordManager__

#include <stdio.h>
#include <sstream>
#include <cmath>
#include <map>
#include "Base.h"
#include "Block.h"
#include "BufferManager.h"
#include "sqlcommand.h"

#define Unused 0x00
#define used 0x01


extern BufferManager *buffermanager;
// WARNING!
// message: 这两个接口请务必给好，具体可以看cpp里面我需要的东西，当然如果有方便的方式或者需要更改对应函数变量就直接说好了


class RecordManager{
public:
    RecordManager(){ NextCanUse.clear(); NowNextCanUse.clear(); };
    virtual ~RecordManager(){};
    // 查找
    Recordinfo Select_Record(sqlcommand& sql, Table &table, bool indexflag, std::vector<slot>& slots);
    // 删除
    Recordinfo Delete_Record(sqlcommand& sql, Table &table, bool indexflag, std::vector<slot> slots);
    // 插入
    Recordinfo Insert_Record(sqlcommand& sql, Table &table, int &block_id, int &record_id);
public:
    // 判断是否满足约束条件 == > >= < <= <>
    bool check(Row& tuple, std::vector<Attribute>& attrs, std::vector<std::vector<std::string> >& conditions);
    // 写入一条记录
    Recordinfo write(RecordBlock& rblock,int j,int tuple_Len, std::vector<Attribute>& attrs, std::vector<std::string>& colValue);
    // 拿出每一条记录
    void getOneTuple(RecordBlock& rblock,int j,int tuple_Len, std::vector<Attribute>& attrs, Row& tuple);
    // 按要求返回结果
    void ReturnRes(Row& tuple, Result& results, std::vector<int> ColToReturn);
public:
    // 记录表的下一个可用位置,提高插入效率,从 O(sigma(i^2)) [i from 0 to n]   -->   O(n) , 用于insert和delete
    std::map<std::string,std::vector<slot>> NextCanUse;
    std::vector<slot> NowNextCanUse;
};

#endif /* defined(__miniSQL__RecordManager__) */
