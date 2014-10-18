//
//  base.h
//  miniSQL
//
//  Created by SmilENow on 10/13/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#ifndef miniSQL_base_h
#define miniSQL_base_h

#include <string>
#include <vector>
#include <sstream>

#define block_size  4096
#define max_block   1000
#define max_char_length 256

class Attribute{
public:
    Attribute():PK(false),UN(false),NN(false),ID(false){};
    
    std::string name;       // 名称
    int datatype;           // 数据类型 (0 int) (-1 float) (正数 char[])
    int length;             // 数据长度
    bool PK,UN,NN,ID;       // PK 主键 UN 唯一 NN 非空 ID 是否有索引
    
    std::string findtype(){
        if (datatype == 0) return "INT";
        if (datatype == -1) return "FLOAT";
        std::stringstream ss;
        ss << datatype;
        return ("Char[" + ss.str() + "]");
    }
};

class Table{
public:
    Table(){};
    Table(const Table& that){
        name = that.name;
        dname = that.dname;
        AttrNum = that.AttrNum;
        RecordLength = that.RecordLength;
        RecordNum = that.RecordNum;
        size = that.size;
    };
    
    std::string name;       // 表名称
    std::string dname;      // 隶属于哪个数据库
    int AttrNum;            // 属性数量
    int RecordLength;       // 一个记录的长度
    int RecordNum;          // 记录的数量
    int size;               // 这个表的大小/长度
    std::vector<Attribute> AttrList;    // 属性列表
};

class TableNum{
public:
    TableNum(){};
    TableNum(int num):num(num){};
    
    int num;
};


// Record返回信息 , 由行列组成, vector<vector<> >
class Row{
public:
    std::vector<std::string> cols; // columns
};
class Result{
public:
    std::vector<Row> rows;  // rows
};
class indexinfo{
};
//
class Recordinfo{
public:
    Recordinfo(){};
    Recordinfo(bool successful,std::string message,Result res,long num):successful(successful),message(message),res(res),num(num){};
    bool getSucc() { return successful; };
    std::string getMessage() { return message; };
    Result getRes() { return res; };
    long getNum(){ return num; };
    
private:
    bool successful;    // 是否正确执行,获取对应的record
    std::string message;    // 错误信息
    Result res;     // 如果正确执行,获得的数据
    long num;       // Record的数量
};

#endif
