

#ifndef	__AVENC_TYPES_H__
#define	__AVENC_TYPES_H__

typedef	enum
{
	AVENC_AUDIO = 0,
	AVENC_IDR,
	AVENC_PSLICE,
	AVENC_FRAME_TYPE_CNT,
}AVENC_FRAME_TYPE;

typedef	enum
{
	AVENC_DATA_TYPE_AUDIO = 0,
	AVENC_DATA_TYPE_IDR,
	AVENC_DATA_TYPE_PSLICE,
	AVENC_DATA_TYPE_BUFFER_AUDIO,
	AVENC_DATA_TYPE_BUFFER_IDR,
	AVENC_DATA_TYPE_BUFFER_PSLICE,
	AVENC_DATA_TYPE_CNT,
}AVENC_DATA_TYPE;

// frame size limited
#define AVENC_FRAME_CIF_SIZE (64*1024)
#define AVENC_FRAME_D1_SIZE (256*1024)

#define AVENC_GOP_TIME (2) // seconds

#endif	//__AVENC_TYPES_H__


