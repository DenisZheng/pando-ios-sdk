//  Copyright (c) 2015 Pando. All rights reserved.
//  PtotoBuf:   ProtocolBuffer.h
//
//  Create By ZhaoWenwu On 15/01/24.

#ifndef SUB_DEVICE_PROTOCOL_TOOL_H
#define SUB_DEVICE_PROTOCOL_TOOL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "platform_functions.h"
#include "pando_endian.h"

#define MAGIC_HEAD_SUB_DEVICE 0x34
#define PAYLOAD_TYPE_COMMAND 1
#define PAYLOAD_TYPE_EVENT	 2
#define PAYLOAD_TYPE_DATA	 3
#define PAYLOAD_TYPE_COMMAND_WITH_FILE 4

#define	TLV_TYPE_FLOAT64 1 
#define TLV_TYPE_FLOAT32 2 
#define	TLV_TYPE_INT8    3
#define	TLV_TYPE_INT16   4
#define	TLV_TYPE_INT32   5
#define TLV_TYPE_INT64   6 
#define	TLV_TYPE_UINT8   7
#define TLV_TYPE_UINT16  8
#define TLV_TYPE_UINT32  9
#define TLV_TYPE_UINT64 10
#define	TLV_TYPE_BYTES  11
#define	TLV_TYPE_URI    12
#define	TLV_TYPE_BOOL   13

#define DEV_HEADER_LEN (sizeof(struct device_header))

#pragma pack(1)

struct device_header
{
    uint8_t  magic;         /* 开始标志 (0x34) */
    uint8_t  crc;           /* 校验和 */
    uint16_t payload_type;  /* 载荷类型 */
    uint16_t payload_len;   /* 载荷长度 */
    uint16_t flags;         /* 标志位 */
    uint32_t frame_seq;     /* 帧序列 */
};

/*TLV信息区，包含count*/
struct TLV 
{
	uint16_t type;
	uint16_t length;
	uint8_t value[];
};

struct TLVs
{
	uint16_t count;
	//struct TLV tlv[];
};

/*命令，事件和数据具体的数据结构*/
struct pando_command
{
	uint16_t sub_device_id;   /* 子设备ID */
	uint16_t command_id;      /* 命令ID */
	uint16_t priority;        /* 优先级 */
	struct TLVs params[1];          /* 参数 */
};

struct pando_event
{
	uint16_t sub_device_id;   /* 子设备ID */
	uint16_t event_num;        /* 事件ID */
    uint16_t priority;
	struct TLVs params[1];          /* 参数 */
};

/*属性的定义*/
struct pando_property
{
	uint16_t sub_device_id;   /* 子设备ID */
	uint16_t property_num;	/* 属性编号 */
	struct TLVs params[1];          /* 参数 */
};

struct sub_device_buffer
{
	uint16_t buffer_length;
	uint8_t *buffer;
};

struct sub_device_base_params
{
	uint32_t event_sequence;
	uint32_t data_sequence;
	uint32_t command_sequence;
};
#pragma pack()


//初始化子设备模块
int init_sub_device(struct sub_device_base_params base_params);

//在创建数据包或者事件包前，先创建好参数的信息区, 同时添加第一个参数，待信息区被create_event等函数成功使用后，要将信息区delete
struct TLVs *create_params_block(uint16_t first_type, uint16_t first_length, void *first_value);

//多次调用直至添加完所有参数
int add_next_param(struct TLVs *params_block, uint16_t next_type, uint16_t next_length, void *next_value);

//创建事件包，返回缓冲区，
//数据发送完成后，要将返回的缓冲区delete掉
struct sub_device_buffer *create_event_package(uint16_t event_num, uint16_t flags, uint16_t priority, struct TLVs *event_params);
struct sub_device_buffer *create_command_package(uint16_t cmd_num, uint16_t flags, uint16_t priority, struct TLVs *cmd_params);

struct sub_device_buffer *create_data_package(uint16_t property_num, uint16_t flag, struct TLVs *data_params);
int add_next_property(struct sub_device_buffer *data_package, uint16_t property_num, struct TLVs *next_data_params);

struct sub_device_buffer *create_feedback_package();

//解析命令包,command_body传出command_id、参数个数等信息,返回第一个参数的指针，用于get_tlv_param获取参数
struct TLV *get_sub_device_command(struct sub_device_buffer *device_buffer, struct pando_command *command_body);

//get data's property id and property num with property_body, return tlv param block
struct TLV *get_sub_device_property(struct sub_device_buffer *device_buffer, struct pando_property *property_body);

//尚未实现,后续迭代时封装完善

uint16_t get_tlv_count(struct TLVs *params_block);

/* if value type is uri or bytes, value point to the start address of data.
   else, value will match the type, make sure (void *) has enough space to 
   memcpy
*/
struct TLV *get_tlv_param(struct TLV *params_in, uint16_t *type, uint16_t *length, void *value);

//删除子设备缓冲区，如果为参数创建过信息区，还需要删除信息区
void delete_device_package(struct sub_device_buffer *device_buffer);

void delete_params_block(struct TLVs *params_block);

uint16_t get_sub_device_payloadtype(struct sub_device_buffer *package);

int is_device_file_command(struct sub_device_buffer *device_buffer);


#ifdef __cplusplus
}
#endif
#endif



