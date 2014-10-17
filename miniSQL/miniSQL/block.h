//
//  block.h
//  miniSQL
//
//  Created by SmilENow on 10/13/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#ifndef __miniSQL__block__
#define __miniSQL__block__

// 缓冲数据块
// 当收到 BufferManager 传回的块后
// 用 getContent 得到内部数据(字符串数组)的指针
// 用 getSize 得到内部数据的有效大小/长度
// 主要用在 IndexManager, BufferManger, RecordManager 中


/*

 文件中 块结构 定义如下:
 [Head][Content]
 Head_Size 64
 Block_Size/Content_Size 4096
 
 Head结构 如果疏漏,请小伙伴们指出
 Table_Name     char*       32      该块表名
 Offset         int         4       偏移量
 Next_Offset    int         4       下一块偏移
 Content_Size   int         4       该块的有效长度/用来判断能否继续插入
 Is_Valid       bool        1       删除标志
 Is_Index       bool        1       index标志位
 
 Content 结构
 Content        char*       4096    4k大小的block,用于存储数据
 
*/

#include <stdio.h>
#include <string>
#include <vector>

#define block_size  4096
#define head_size   64

class Block{
public:                     // 块头数据
    char TableName[32];     // 表名
    int NextOffset;         // 下一块在文件中的偏移量
    int ContentSize;        // 该块的有效长度
    bool Is_Valid;          // 删除标志
    bool Is_Index;          // index标志位
    
public:                         // 块数据
    char Content[block_size];   // 块数据
    
public:                     // 其他
    int Offset;             // 该block在文件中的偏移量
    bool Is_Dirty;          // 脏数据标志位
    
public:
    Block();            // 初始化块,先把空间给开够,无论它用不用,这样就可以满足课程理论的设计要求了
    Block(bool index);  // 为索引初始化块
    void dirty();       // 设置为脏数据,同时注意如果替换的时候必须要把脏数据写回memory
    char* getContent(); // 获取数据信息
    int getContentSize();      // 获取块内部的数据长度,以便getContent时候的操作
    void debug(bool requireContent = false);       // 用于测试
};

#endif /* defined(__miniSQL__block__) */
