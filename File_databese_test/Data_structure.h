#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct db_metadata;
struct field;


//BLOCK STRUCTURE-------------------------------------------
struct block {//(8 bytes)

	block();
	//block(db_metadata);

	int next;
	int is_empty;

};

//DATABASE METADATA STRUCTURE--------------------------------
struct db_metadata { //(12 bytes)

	db_metadata();
	db_metadata(int db_size_, int block_size_);

	int db_size;
	int cant_blocks;
	int block_size;

	db_metadata fill_dbm(string, db_metadata);

	int get_freeblock_index(db_metadata);
	int get_table_index(db_metadata dbm, char[20]);


	void create_database(string, db_metadata);
	void create_table(db_metadata);

	void open_database(string);

	void drop_database();

	void insert_records_part2(int,db_metadata);
	void list_systables(db_metadata dbm);
	void select(db_metadata dbm);
	void show_table_columns(int index);
	void show_all_records(int index, db_metadata dbm,int);
	void  drop_table(db_metadata dbm);
	void insert_records_part1(db_metadata dbm);
	void drop_table_helper(int table_index, db_metadata dbm, char table_name_[20],int);
	void delete_record(db_metadata dbm);
	void delete_record_helper(int index, db_metadata dbm, char [10]);

	void update_record(db_metadata dbm);
	void update_record_helper(int index, db_metadata dbm, char chosen_column[10]);
	
	void  select_specific_column(db_metadata dbm);
	void select_specific_column_helper(int index, db_metadata dbm, char chosen_column[10]);
};

//TABLE STRUCTURE---------------------------------------------
struct table {

	table();
	table(char[20]);

	char table_name[20];
	int vector_size;
	vector<field>fields;


};

//FIELDS STRUCTURE--------------------------------------------
struct field {

	char field_name[10];
	char field_type[10];


};
struct records_field {
	char field_type[10];
};

struct sys_table_metadata {
	char table_name[20];
	int table_count;
};


struct sys_table_field {
	char table_name[20];
	int index;

};
struct records_metadata {
	records_metadata();
	char records_separator[10];
	int records_count;


};