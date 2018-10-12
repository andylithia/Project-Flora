#ifndef _DS3231_H
#define _DS3231_H
#include "BCG.h"
#include "stm32f0xx.h"
#define DS3231_BASE_ADR				B8(11010000)

#define DS3231_SET_HDIG(x) (x<<4)&B8(01110000)
#define DS3231_GET_HDIG(x) (x>>4)&B8(00001111)
#define DS3231_SET_LDIG(x) (x)&B8(00001111)
#define DS3231_GET_LDIG(x) (x)&B8(00001111)

#define DS3231_ADR_SEC				0x00
#define DS3231_ADR_MIN				0x01
#define DS3231_ADR_HOUR				0x02
#define DS3231_ADR_DAY				0x03
#define DS3231_ADR_DATE				0x04
#define DS3231_ADR_C_MONTH			0x05
#define DS3231_ADR_YEAR				0x06
#define DS3231_ADR_ALRM1_SE			0x07
#define DS3231_ADR_ALRM1_MIN		0x08
#define DS3231_ADR_ALRM1_HOUR		0x09
#define DS3231_ADR_ALRM1_DAY_DATE	0x0A
#define DS3231_ADR_ALRM2_MIN		0x0B
#define DS3231_ADR_ALRM2_HOUR		0x0C
#define DS3231_ADR_ALRM2_DAY_DATE	0x0D
#define DS3231_ADR_CTRL1			0x0E
#define DS3231_ADR_CTRL2_STAT		0x0F
#define DS3231_ADR_AOFFSET			0x10
#define DS3231_ADR_TEMP_H			0x11
#define DS3231_ADR_TEMP_L			0x12

#define DS3231_EOSCN			BIT(7)
#define DS3231_BBSQW			BIT(6)
#define DS3231_CONV				BIT(5)
#define DS3231_RS2				BIT(4)
#define DS3231_RS1				BIT(3)
#define DS3231_INTCN			BIT(2)
#define DS3231_A2IE				BIT(1)
#define DS3231_A1IE				BIT(0)

#define DS3231_OSF				BIT(7)
#define DS3231_EN32KHZ			BIT(3)
#define DS3231_BSY				BIT(2)
#define DS3231_A2F				BIT(1)
#define DS3231_A1F				BIT(0)

#define DS3231_HOUR_TYPE_12		1
#define DS3231_HOUR_TYPE_24		0

#define DS3231_YEAR_MAX			99

typedef enum { NA=0, \
		SEC = 1, MIN = 2, HOUR = 4, DAY = 8,\
		DATE = 16, C_MON = 32, MONTH = 64, YEAR = 128\
	} DS3231_TSECT_E;

typedef enum {
		NONE = 0, ALRM1 = 1, ALRM2 = 2
	} DS3231_ALARM_ID_E;

typedef union {
	uint8_t HOUR_12;
	uint8_t HOUR_24;
} DS3231_HOUR_UNION_t;

typedef struct {
	uint8_t HOUR_TYPE;			// 12/24n 1bit
	uint8_t AMn_PM;				// AMn/PM 1bit
	DS3231_HOUR_UNION_t HOUR;	// HRH 1/2bit, HRL 4bit
} DS3231_HOUR_t;

typedef union {
	uint8_t DAY;				// DAY 1-7 4bit
	uint8_t DATE;				// DATEH 2bit, DATEL 4bit
} DS3231_DYDT_UNION_t;

typedef struct {
	uint8_t	SEC;				// SECH 3bit, SECL 4bit
	uint8_t MIN;				// MINH 3bit, MINL 4bit
	DS3231_HOUR_t HOUR;
	uint8_t DAY;				// DAY 3bit (1-7)
	uint8_t DATE;				// DATEH 2bit, DATEL 4bit
	uint8_t CENTURY;			// CENTURY 1bit
	uint8_t MONTH;				// MONTHH 1bit, MONTHL 4bit
	uint8_t YEAR;				// YEARH 4bit, YEARL 4bit
} DS3231_time_t;

typedef struct{
	uint8_t	M;					// MODE[4:1]
	uint8_t SEC;				// SECH 3bit, SECL 4bit
	uint8_t MIN;				// MINH 3bit, MINL 4bit
	DS3231_HOUR_t HOUR;
	uint8_t DYDTn;				// DAY/DATEn 1bit
	DS3231_DYDT_UNION_t DYDT;
} DS3231_alarm_t;

typedef struct {
	uint8_t CTRL1;
	uint8_t CTRL2_STAT;
	int8_t AOFFSET;				// AGING OFFSET SIGNED 8bit
	uint16_t TEMP;				// TEMP LEFT ALIGNED SIGNED 10bit
} DS3231_control_t;

#define LYEAR_CHECK(YEAR) (((YEAR)%4==0&&(YEAR)%100!= 0)||(YEAR)%400==0)


#endif /* _DS3231_H */
