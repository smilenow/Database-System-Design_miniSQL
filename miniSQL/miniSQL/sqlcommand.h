
// sqlcommander:
// std::vector<std::vector<std::string> > getconditions()
// 	attrname operation string
// std::vector<std::string> getSelectInfo()
#ifndef __miniSQL__sqlcmd__
#define __miniSQL__sqlcmd__

#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

class sqlcommand{
public:
	int sqlType;//各数字代表意义见下
	std::string tablename;
	//0:select & 1:delete
	//std::string selectTablen;
	std::vector<std::vector<std::string> > conditions;
	std::vector<std::string> selectInfo;
	//2:insert
	//std::string insertTablen;
	std::vector<std::string> colValue;
	//3:create table
	//首行是tablename，之后每行分别是属性名、数据类型(0:int +:char -1:float)和列约束(0:普通 1:PR 2:UN)
	std::vector<std::vector<std::string> > createTableInfo;
	//4:create index
	std::vector<std::string> createIndexInfo;
	//5:drop table & 6:drop index
	//std::string dropTablen;
	std::string indexname;
	



	sqlcommand(){};
	int getSqlType() {return sqlType;}
	std::string getTablen()	{return tablename;}
	std::string getIndexn()	{return indexname;}
	std::vector<std::vector<std::string> > getconditions(){
		std::vector<std::vector<std::string> >::iterator iter;
		return conditions;
	}
	std::vector<std::string> GetSelectInfo(){
		return selectInfo;
	}
	std::vector<std::string> getcolValue(){
		return colValue;
	}
	std::vector<std::vector<std::string> > getCreateTableInfo(){
		std::vector<std::vector<std::string> >::iterator iter;
		return createTableInfo;
	}
	//std::string getDropTablen()	{return tablename;}
	//std::string getDropIndexn()	{return dropIndexn;}
	std::vector<std::string> getCreateIndexInfo()	{
		return createIndexInfo;
	}

	void setSelectInfo(std::string s){
		selectInfo.push_back(s);
	}
	void setcolValue(std::string s){
		colValue.push_back(s);
	}
	void clear(){
		//printf("cleared");
		conditions.clear();
		colValue.clear();
		selectInfo.clear();
		createTableInfo.clear();
		createIndexInfo.clear();
	};
	void setconditions(std::string attrname,std::string op,std::string s){
		std::vector<std::string> v;
		v.push_back(attrname);
		v.push_back(op);
		v.push_back(s);

		conditions.push_back(v);
	}
	//tablename,attraneme,attrproperty,attrtype
	void setCreateTableInfo(std::string s,std::string n[],int p[],int t[]){
		std::vector<std::string> v;
		v.push_back(s);
		createTableInfo.push_back(v);
		v.clear();
		int i = 0;
		while(n[i]!="")	{
			std::vector<std::string> v;
			char s1[2],s2[3];
			sprintf(s1, "%d", p[i]);
			sprintf(s2, "%d", t[i]);
			v.push_back(n[i]);
			v.push_back(s1);
			v.push_back(s2);
			createTableInfo.push_back(v);
			i++;
		}
		std::stringstream ss;
		ss<<i;
		std::string str;
		ss>>str;
		createTableInfo[0].push_back(str);
	}
	void setCreateIndexInfo(std::string tablen,std::string attrn){
		createIndexInfo.push_back(indexname);
		createIndexInfo.push_back(tablen);
		createIndexInfo.push_back(attrn);
	}
	//void setDropTablen(std::string tablen) {tablename=tablen;}
	//void setDropIndexn(std::string indexn) {dropIndexn=indexn;}
	~sqlcommand(){};
};




#endif