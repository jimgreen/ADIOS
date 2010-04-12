/* 
 * ADIOS is freely available under the terms of the BSD license described
 * in the COPYING file in the top level directory of this source distribution.
 *
 * Copyright (c) 2008 - 2009.  UT-BATTELLE, LLC. All rights reserved.
 */

#ifndef ADIOS_BP_V2_H
#define ADIOS_BP_V2_H
#include "stdint.h"
#include "unistd.h"
#include "adios_types.h"
#include "adios_transport_hooks.h"

struct adios_bp_buffer_struct_v2
{
    int f;             // the file handle
    uint64_t file_size;
    uint32_t version;

    char * allocated_buff_ptr;  // initial alloc for aligning on 8-byte boundary

    char * buff;
    uint64_t length;
    uint64_t offset;   // buffer_offset

    enum ADIOS_FLAG change_endianness;

    off_t file_offset;
    uint64_t end_of_pgs;          // where the last process group ends
    uint64_t pg_index_offset;     // process groups index starts
    uint64_t pg_size;             // process groups index size
    uint64_t vars_index_offset;   // vars index start
    uint64_t vars_size;           // vars index size
    uint64_t attrs_index_offset;  // attributes index start
    uint64_t attrs_size;          // attributes index size

    uint64_t read_pg_offset;
    uint64_t read_pg_size;
};

struct adios_index_process_group_struct_v2
{
    char * group_name;
    enum ADIOS_FLAG adios_host_language_fortran;
    uint32_t process_id;
    char * time_index_name;
    uint32_t time_index;
    uint64_t offset_in_file;

    struct adios_index_process_group_struct_v2 * next;
};

struct adios_index_characteristic_dims_struct_v2
{
    uint8_t count;
    uint64_t * dims;  // each 3 uint64_t represents one dimension (l, g, o)
};

struct adios_index_characteristic_struct_v2
{
    uint64_t offset;  // beginning of the var or attr entry
    void * min;
    void * max;
    struct adios_index_characteristic_dims_struct_v2 dims;
    void * value;
    uint16_t var_id;
    uint64_t payload_offset;   // beginning of the var or attr payload
    char * file_name;
};

struct adios_index_var_struct_v2
{
    uint16_t id;
    char * group_name;
    char * var_name;
    char * var_path;
    enum ADIOS_DATATYPES type;

    uint64_t characteristics_count;
    uint64_t characteristics_allocated;

    struct adios_index_characteristic_struct_v2 * characteristics;

    struct adios_index_var_struct_v2 * next;
};

struct adios_index_attribute_struct_v2
{
    uint16_t id;
    char * group_name;
    char * attr_name;
    char * attr_path;
    enum ADIOS_DATATYPES type;

    uint64_t characteristics_count;
    uint64_t characteristics_allocated;

    struct adios_index_characteristic_struct_v2 * characteristics;

    struct adios_index_attribute_struct_v2 * next;
};

struct adios_method_info_struct_v2
{
    enum ADIOS_IO_METHOD id;
    char * parameters;

    struct adios_method_info_struct_v2 * next;
};

struct adios_dimension_item_struct_v2
{
    uint64_t rank;
    uint16_t var_id;
    enum ADIOS_FLAG time_index;
};

struct adios_dimension_struct_v2
{
    struct adios_dimension_item_struct_v2 dimension;
    struct adios_dimension_item_struct_v2 global_dimension;
    struct adios_dimension_item_struct_v2 local_offset;
    struct adios_dimension_struct_v2 * next;
};

struct adios_process_group_header_struct_v2
{
    enum ADIOS_FLAG host_language_fortran;
    char * name;
    uint16_t coord_var_id;
    char * time_index_name;
    uint32_t time_index;
    uint8_t methods_count;
    struct adios_method_info_struct_v2 * methods;
};

struct adios_vars_header_struct_v2
{
    uint16_t count;
    uint64_t length;
};

struct adios_attributes_header_struct_v2
{
    uint16_t count;
    uint64_t length;
};

struct adios_var_header_struct_v2
{
    uint16_t id;
    char * name;
    char * path;
    enum ADIOS_DATATYPES type;
    enum ADIOS_FLAG is_dim;
    struct adios_dimension_struct_v2 * dims;
    struct adios_index_characteristic_struct_v2 characteristics;
    uint64_t payload_size;
};

struct adios_var_payload_struct_v2
{
    void * payload;
};

struct adios_attribute_struct_v2
{
    uint16_t id;
    char * name;
    char * path;

    enum ADIOS_FLAG is_var;

    uint16_t var_id;

    enum ADIOS_DATATYPES type;
    uint32_t length;
    void * value;
};

void adios_shared_buffer_free_v2 (struct adios_bp_buffer_struct_v2 * b);
void adios_buffer_struct_init_v2 (struct adios_bp_buffer_struct_v2 * b);
void adios_buffer_struct_clear_v2 (struct adios_bp_buffer_struct_v2 * b);

void * adios_dupe_data_scalar (enum ADIOS_DATATYPES type, void * in);

// buff must be 4 bytes
int adios_parse_version_v2 (struct adios_bp_buffer_struct_v2 * b
                        ,uint32_t * version
                        );

// buff must be 16 bytes
int adios_parse_index_offsets_v2 (struct adios_bp_buffer_struct_v2 * b);

int adios_parse_process_group_index_v2 (struct adios_bp_buffer_struct_v2 * b
                         ,struct adios_index_process_group_struct_v2 ** pg_root
                         );

int adios_parse_vars_index_v2 (struct adios_bp_buffer_struct_v2 * b
                              ,struct adios_index_var_struct_v2 ** vars_root
                              );
int adios_parse_attributes_index_v2 (struct adios_bp_buffer_struct_v2 * b
                          ,struct adios_index_attribute_struct_v2 ** attrs_root
                          );

int adios_parse_process_group_header_v2 (struct adios_bp_buffer_struct_v2 * b
                     ,struct adios_process_group_header_struct_v2 * pg_header
                     );

int adios_parse_vars_header_v2 (struct adios_bp_buffer_struct_v2 * b
                               ,struct adios_vars_header_struct_v2 * vars_header
                               );

int adios_parse_var_data_header_v2 (struct adios_bp_buffer_struct_v2 * b
                                ,struct adios_var_header_struct_v2 * var_header
                                );

int adios_parse_var_data_payload_v2 (struct adios_bp_buffer_struct_v2 * b
                              ,struct adios_var_header_struct_v2 * var_header
                              ,struct adios_var_payload_struct_v2 * var_payload
                              ,uint64_t payload_size
                              );

int adios_parse_attributes_header_v2 (struct adios_bp_buffer_struct_v2 * b
                       ,struct adios_attributes_header_struct_v2 * attrs_header
                       );

int adios_parse_attribute_v2 (struct adios_bp_buffer_struct_v2 * b
                             ,struct adios_attribute_struct_v2 * attribute
                             );
int adios_clear_process_group_header_v2 (
                     struct adios_process_group_header_struct_v2 * pg_header);
int adios_clear_var_header_v2 (struct adios_var_header_struct_v2 * var_header);
int adios_clear_attribute_v2 (struct adios_attribute_struct_v2 * attribute);

// ****************************************************************************
// functions from adios_posix.c used for file reading

void adios_init_buffer_read_version_v2 (struct adios_bp_buffer_struct_v2 * b);
void adios_posix_read_version_v2 (struct adios_bp_buffer_struct_v2 * b);

void adios_init_buffer_read_index_offsets_v2 ( struct adios_bp_buffer_struct_v2 * b);
void adios_posix_read_index_offsets_v2 (struct adios_bp_buffer_struct_v2 * b);

void adios_init_buffer_read_process_group_index_v2 ( struct adios_bp_buffer_struct_v2 * b);
void adios_posix_read_process_group_index_v2 (struct adios_bp_buffer_struct_v2 * b);

void adios_init_buffer_read_process_group_v2 (struct adios_bp_buffer_struct_v2 * b);
void adios_init_buffer_read_vars_index_v2 (struct adios_bp_buffer_struct_v2 * b);
void adios_posix_read_vars_index_v2 (struct adios_bp_buffer_struct_v2 * b);

void adios_init_buffer_read_attributes_index_v2
                                        (struct adios_bp_buffer_struct_v2 * b);
void adios_posix_read_attributes_index_v2 (struct adios_bp_buffer_struct_v2 * b);

void adios_init_buffer_read_procss_group_v2 (struct adios_bp_buffer_struct_v2 * b);
uint64_t adios_posix_read_process_group_v2 (struct adios_bp_buffer_struct_v2 * b);

int adios_posix_open_read_internal_v2 (const char * filename
                                   ,const char * base_path
                                   ,struct adios_bp_buffer_struct_v2 * b
                                   );
void adios_posix_close_internal_v2 (struct adios_bp_buffer_struct_v2 * b);

#endif