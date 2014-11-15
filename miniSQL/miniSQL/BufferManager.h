//
//  buffermanager.h
//  miniSQL
//
//  Created by Kael on 10/18/14.
//  Copyright (c) 2014 Xinyuan Lu. All rights reserved.
//

#ifndef __BUFFERMANAGER__
#define __BUFFERMANAGER__

#include "Block.h"
#include "Base.h"
#include <string>
#include <vector>
#include <map>

#define Buffer_Capacity 200

#define DB 1	// DataBlock
#define IB 2	// IndexBlock
#define ICB 3	// IndexCatalogBlock
#define TCB 4	// TableCatalogBlock
#define ACB 5	// AttributeCatalogBlock

#define max_name_length 128		// suppose the maximum of all the names is 256

class BufferManager{
public:
	BufferManager();			// 同时也创建三个文件夹
	~BufferManager(){
		write_all();
	}

	Block* newBlock(int type, std::string tablename, int NodeType=-100, int AttrType=-100, int valuecharlen=0);
	// todo: catalog_block分开
	// 读block可不分开，但是必须要指明是哪个manager
	// 返回后需要dynamic_cast
	Block* getBlock(int type, std::string tablename, int bid);
	// storeBlock第一个参数可以去掉，只给出block就可以
	bool storeBlock(std::string tablename, Block *block);
	// 为了防止产生歧义，getblocknumber使用fname(file)做变量名
	// 事实上只有index的fname相对特殊
	int get_block_number(int type, std::string fname);
    void delete_blocks(std::string tablename);

    // for catalog manager
    int getTableCatalogBlocksNumber();
    int getAttrCatalogBlocksNumber();
    int getIndexCatalogBlocksNumber();
    TableCatalogBlock* getTableCatalogBlocks(int block_id);
    AttrCatalogBlock* getAttrCatalogBlocks(int block_id);
    IndexCatalogBlock* getIndexCatalogBlocks(int block_id);
    void storeTableCatalogBlocks(int block_id, TableCatalogBlock* nowblock);
    void storeAttrCatalogBlocks(int block_id, AttrCatalogBlock* nowblock);
    void storeIndexCatalogBlocks(int block_id, IndexCatalogBlock* nowblock);
    TableCatalogBlock* newTableCatalogBlocks();
    AttrCatalogBlock* newAttrCatalogBlocks();
    IndexCatalogBlock* newIndexCatalogBlocks();

	std::vector<IndexBlock> load_tree(std::string indexname);
	bool store_tree(std::string indexname, std::vector<IndexBlock> tree);
	bool delete_tree(std::string indexname);

public:
	bool write_recordblock(int block_n);
	bool write_catalogblock(int block_n);
	bool write_indexblock(int block_n);

	int write_block();
	void write_all();
	// void create_data(std::string tablename);
	// void create_catalog(std::string tablename);
	// void create_index(std::string tablename, std::string indexname);
	bool is_full();
	static void* clock_LRU(void *arg);
	void LRU();
	void pin_block(int block_n);
	void unpin_block(int block_n);
	int find_type(Block* block);
	void load_block(int block_n, int type, std::string tablename, int bid);
	int get_available_block();
public:
	// 当前valid的block个数
	int block_number;

	// if the pin
	// but if explicitly required, it should write inspite the pin_bit？
	static bool pin_bit[Buffer_Capacity];
    static bool reference_bit[Buffer_Capacity];
	static int reference_bit_count;
	// filename存放对应的block的filename
	// map并不适合这个结构，但一开始写的是map，后来修改了结构，没有把map改掉了
	std::map<int, std::string> filename;
	// 存放各种block
	Block* buffer[Buffer_Capacity];
};

#endif