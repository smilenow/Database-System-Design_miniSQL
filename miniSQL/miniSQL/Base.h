//
//  base.h
//  miniSQL
//
//  Created by SmilENow on 10/13/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

// V2.0: Add AND(), append() methods for Resultinfo & Result & Row, by Xinyuan Lu
// Now they can perform some logical operation
// In fact, the core code of select/delete of API are now in this file

// v2.1, delete unused attribute, by Xinyuan Lu

#ifndef miniSQL_base_h
#define miniSQL_base_h

#include <string>
#include <vector>
#include <sstream>

#include <algorithm>

#define block_size  4096
#define max_block   1000
#define max_char_length 256

#define _string_type    1
#define _int_type       0
#define _float_type     -1

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
        // dname = that.dname;
        // AttrNum = that.AttrNum;
        RecordLength = that.RecordLength;
        // RecordNum = that.RecordNum;
        // size = that.size;
    };
    
    std::string name;       // 表名称
    // std::string dname;      // 隶属于哪个数据库
    // int AttrNum;            // 属性数量
    int RecordLength;       // 一个记录的长度
    // int RecordNum;          // 记录的数量
    // int size;               // 这个表的大小/长度
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
    void append(Result r){
        for(int i=0; i<r.rows.size();i++)
            rows.push_back(r.rows.at(i));
    }
    void AND(Result r){
        for(std::vector<Row>::iterator i=rows.begin(); i<rows.end(); i++){
            bool flag=false;
            for(std::vector<Row>::iterator j=r.rows.begin(); j<r.rows.end(); j++){
                if(std::equal((*i).cols.begin(), (*i).cols.end(), (*i).cols.begin())){
                    flag=true;
                    break;
                }
            }
            if(flag==false)
                rows.erase(i);
        }
    }
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
    
    void append(Recordinfo r){
        res.append(r.res);
        successful|=r.successful;
        num+=r.num;
        if(successful==false) message=message+" & "+r.message;
    }
    void AND(Recordinfo r){
        // first check
        if(!successful) return ;
        if(r.successful==false){
            message=message+" & "+r.message;
            successful=false;
            num=0;
            return ;
        }
        if(r.successful&&successful)
            res.AND(r.res);
        num=res.rows.size();
        if(num==0){
            message="Cannot find any tuple by intersection!";
            successful=false;
        }
    }
    
private:
    bool successful;    // 是否正确执行,获取对应的record
    std::string message;    // 错误信息
    Result res;     // 如果正确执行,获得的数据
    long num;       // Record的数量
};

//存放值的类
class Value{
private:                    // 用type来区别是哪种类型,由于没有写多态,所以开了三种类型来选择
    int type;               // 1 string 0 int -1 float
    std::string charKey;
    int intKey;
    float floatKey;
    bool Valid;
    
public:
    // 构造
    Value():type(0),intKey(0),Valid(false){};
    Value(int type):type(type),Valid(true){};
    Value(int type,std::string k):type(type),charKey(k),Valid(true){};
    Value(int type,int k):type(type),intKey(k),Valid(true){};
    Value(int type,float k):type(type),floatKey(k),Valid(true){};
    
    //获取类型属性和相对应的值
    int getType() const { return type; };
    int getIntKey() const { return intKey; };
    float getFloatKey() const { return floatKey; };
    bool getValid() const { return Valid; }
    std::string getCharKey() const { return charKey; };
    
    //设置对应的值
    void setKey(int k) { intKey = k; };
    void setKey(float k) { floatKey = k; };
    void setKey(std::string k) { charKey = k; };
    void setValid(bool v) { Valid = v; }
    
    //具体函数看.cpp,获取key的值,用string输出
    std::string getKey();
    void resetKey();
};

struct slot{
    int block_id;
    int offset;
    slot():block_id(-1),offset(-1){};
    slot(int bid,int offset):block_id(bid),offset(offset){};
    void reset(){ block_id=-1,offset=-1; }
};

#endif
