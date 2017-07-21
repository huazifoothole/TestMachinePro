#ifndef __PUBLIC_HEADER__
#define __PUBLIC_HEADER__

#define __DEBUG__	1

#ifndef __LINUX__
#include <windows.h>
#endif

//#pragma pack (1)

#ifdef __LINUX__
//??????
typedef void				VOID;
typedef char				CHAR;
typedef unsigned char		BYTE;
typedef short				WORD;
typedef unsigned short		USHORT;
typedef int					INT;
typedef unsigned int		UINT;
typedef long				LONG;
typedef unsigned long		DWORD;

//??????
typedef void*				LPVOID;
typedef bool*				PBOOL;
typedef char*				PCHAR;
typedef unsigned char*		PBYTE;
typedef short*				PWORD;
typedef unsigned short*		PUSHORT;
typedef int*				PINT;
typedef unsigned int*		PUINT;
typedef long*				PLONG;
typedef unsigned long*		PDWORD;
typedef const char*			LPCSTR;
typedef char*				LPSTR;
typedef	BYTE*				LPBYTE;

typedef bool				BOOL;
#define false		0
#define true		!false
#define FALSE		false
#define TRUE		true
//??????
/*#ifndef BOOL
#define bool		int
#define false		0
#define true		!false

#define BOOL		bool
#define FALSE		false
#define TRUE		true
#endif*/

/*
        //??????
        #define VOID		void
        #define CHAR		char
        #define BYTE		unsigned char
        #define WORD		short
        #define USHORT		unsigned short
        #define INT			int
        #define UINT		unsigned int
        #define LONG		long
        #define DWORD		unsigned long

        //??????
        #define LPVOID		void*
        #define PBOOL		bool*
        #define PCHAR		char*
        #define PBYTE		unsigned char*
        #define PWORD		short*
        #define PUSHORT		unsigned short*
        #define PINT		int*
        #define PUINT		unsigned int*
        #define PLONG		long*
        #define PDWORD		unsigned long*
        #define LPCSTR		const char*
*/
#endif


//????????
#ifndef IN
        #define IN
        #define OUT
#endif

typedef void*	HANDLE;

//??????
#define ZERO_INT			0
#define ZERO_FLOAT			0.0
#define BYTE_LIMIT			0xff
#define SHORT_LIMIT			0xffff
#define LONG_LIMIT			0xffffffff
#define BYTE_WIDTH			8
#define SHORT_WIDTH			16
#define INT_WIDTH			32
#define KB					1024
#define MB					(KB*KB)
#define INVALID_VALUE		-1
#define INVALID_PORT		-1	//?????


//????????
#ifndef NO_ERROR
#define NO_ERROR			0
#endif

#define E_PARAM_ERROR		-1	//????
#define E_NO_DEVICE			-2	//????
#define E_IO_ERROR			-3	//IO??
#define E_READ_FAILED		-4	//???
#define E_WRITE_FAILED		-5	//???
#define E_TIME_OUTS			-6	//??
#define	E_NO_DRIVER			-7	//????
#define E_COMM_ERROR		-8	//????
#define E_INVALID_HANDLE	-9	//?????
#define E_NO_MEMORY			-10	//?????
#define E_VER_ERROR			-11	//????
#define E_NOT_ENOUGH		-12	//???????
#define E_NOT_EXISTS		-13	//?????
#define E_INVALID_VALUE		-14	//????
#define E_NOT_SUPPORT		-15	//??????
#define E_FILE_ERROR		-16	//????
#define E_NULL_POINTER		-17	//???
#define E_DEVICE_BUSY		-18	//???
#define E_INVALID_DATA		-19	//????
#define E_GENERAL_ERROR		-20	//????



inline void FreeMemory(BYTE** puchData)
{
        if (NULL!=puchData && NULL!=*puchData)
        {
                delete [] *puchData;
                *puchData =NULL;
        }
}

inline LONG AllocMemory(void** ppData, LONG lLeng)
{
        if (NULL ==ppData)
                return E_NULL_POINTER;

        *ppData =new BYTE [lLeng];
        if (NULL ==*ppData)
                return E_NO_MEMORY;

        memset(*ppData, 0, lLeng);

        return NO_ERROR;
}

#define VERSION_STRING_LENG				64		//????????
#define DESCTRIPTION_STRING_LENG		128		//????????
//?????????
typedef struct Sversion
{
        char	szVersion[VERSION_STRING_LENG];
        char	szDateTime[VERSION_STRING_LENG];
        char	szDescription[DESCTRIPTION_STRING_LENG];
}Sversion;


//???????
typedef struct TDate
{
        BYTE	uchYear;		//?
        BYTE	uchMonth;	//?
        BYTE	uchDay;		//?
}TDate;

typedef struct TTime
{
        BYTE	uchHour;	//??
        BYTE	uchMinute;	//?
        BYTE	uchSecond;	//?
}TTime;

typedef struct TPoint
{
        INT nXPos;
        INT nYPos;
}TPoint;

// typedef struct  TRange
// {
// 	DWORD dwMin;
// 	DWORD dwMax;
// }TRange;

typedef struct TDateTime
{
        TDate	tDate;
        BYTE	uchDays;	//??
        TTime	tTime;
}TDateTime;

#endif
