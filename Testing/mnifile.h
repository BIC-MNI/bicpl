/*
 * define MNI file stuff
 */

#ifndef NULL
#define NULL 0
#endif

#ifndef Boolean
#define Boolean int
#endif

#define MNIchar     0
#define MNIshort    1
#define MNIlong     2
#define MNIfloat    3
#define MNIdouble   4

#define BLOCK_SIZE                512
#define FRUIT_SALAD_WIDTH         12
#define FRUIT_SALAD_HEIGHT        7
#define SCX_SLICES                15
#define NAME_LENGTH               23
#define NUMBER_LENGTH             14
#define ID_LENGTH                 3
#define TIME_DATE_LENGTH          10
#define HEAD_CIRCUMFERENCE_LENGTH 10
#define SHORT_LENGTH              2
#define LONG_LENGTH               4
#define FLOAT_LENGTH              4
#define BYTE_LENGTH               1

#define MNI_NO_DUMP       0
#define MNI_DUMP_TABLE    1
#define MNI_DUMP_SHORT    2
#define MNI_DUMP_LONG     4
#define MNI_DUMP_HEADER   8

typedef unsigned char MNIbyte;

typedef struct {
   float Qsc;
   float CntImg;
   float Zpos;
   short Izpos;
   short Itype;
   short Isc;
   short Islice;
   short Iyoff;
   float Tsc;
   short Isea;
   float Samp;
   float OffGry;
   short Isctim;
   float Swid;
   float SclGry;
   short RatLin;
   float OffWht;
   short RatRan;
   float SclWht;
} ImageInfo;

typedef struct {
   MNIbyte Qsc[FLOAT_LENGTH];
   MNIbyte CntImg[FLOAT_LENGTH];
   MNIbyte Zpos[FLOAT_LENGTH];
   MNIbyte FillerBlock1[2];
   MNIbyte Izpos[BYTE_LENGTH];
   MNIbyte FillerBlock2[1];
   MNIbyte Itype[BYTE_LENGTH];
   MNIbyte Isc[BYTE_LENGTH];
   MNIbyte Islice[BYTE_LENGTH];
   MNIbyte FillerBlock3[1];
   MNIbyte Iyoff[BYTE_LENGTH];
   MNIbyte FillerBlock4[3];
   MNIbyte Tsc[FLOAT_LENGTH];
   MNIbyte Isea[SHORT_LENGTH];
   MNIbyte FillerBlock5[2];
   MNIbyte Samp[FLOAT_LENGTH];
   MNIbyte OffGry[FLOAT_LENGTH];
   MNIbyte Isctim[SHORT_LENGTH];
   MNIbyte FillerBlock6[2];
   MNIbyte Swid[FLOAT_LENGTH];
   MNIbyte SclGry[FLOAT_LENGTH];
   MNIbyte RatLin[SHORT_LENGTH];
   MNIbyte FillerBlock7[6];
   MNIbyte OffWht[FLOAT_LENGTH];
   MNIbyte RatRan[SHORT_LENGTH];
   MNIbyte FillerBlock8[6];
   MNIbyte SclWht[FLOAT_LENGTH];
   MNIbyte FillerBlock9[8];
} FruitSaladItems;

typedef union {
   MNIbyte Lines[FRUIT_SALAD_HEIGHT][FRUIT_SALAD_WIDTH];
   FruitSaladItems Items;
} FruitSaladStructure;

typedef struct {
   char *FileName;
   FILE *fp;
   char *PatientName;
   char *PatientNumber;
   char *CameraId;
   char *AcquisitionTime;
   char *AcquisitionDate;
   char *PhysicianName;
   char *StudyType;
   char *CollimatorType;
   char *Isotope;
   char *Dose;
   char *InjectionTime;
   int CommentOffset;
   int BlockCount;
   int ReconMask;
   int AdminOffset;
   int ScannerId;
   int DataOffset;
   int CountsOffset;
   int FrameCount;
   int GroupCount;
   int DelayTime;
   int TiltAngle;
   int AcquisitionStatus;
   char MatrixType;
   char *ScannerType;
   int ImageWidth;
   int ImageHeight;
   int PixelWidth;
   int ImageSize;
   int ImageBytes;
   int Slices;
   int Frames;
   ImageInfo *ImageParameters;
} MniInfo;

typedef struct {
   MNIbyte FrameCount[LONG_LENGTH];
   MNIbyte MatrixType[SHORT_LENGTH];
   MNIbyte CloseType[SHORT_LENGTH];
   MNIbyte FrameRate[LONG_LENGTH];
   MNIbyte FrameDuration[5];
} GroupInfo;

typedef struct {
   MNIbyte Count[SHORT_LENGTH];
   MNIbyte Start[SHORT_LENGTH];
   MNIbyte Increment[SHORT_LENGTH];
} CouchInfo;

typedef struct {
   MNIbyte PatientName[NAME_LENGTH];                         /*   1 */
   MNIbyte PatientNumber[NUMBER_LENGTH];                     /*  24 */
   MNIbyte Frgg[SHORT_LENGTH];                               /*  38 */
   MNIbyte CameraId[ID_LENGTH];                              /*  40 */
   MNIbyte AcquisitionTime[TIME_DATE_LENGTH];                /*  43 */
   MNIbyte AcquisitionDate[TIME_DATE_LENGTH];                /*  53 */
   MNIbyte HeadCircumference[HEAD_CIRCUMFERENCE_LENGTH];     /*  63 */
   MNIbyte PhysicianName[NAME_LENGTH];                       /*  73 */
   MNIbyte StudyType[NUMBER_LENGTH];                         /*  96 */
   MNIbyte View[10];                                         /* 110 */
   MNIbyte CollimatorFill[BYTE_LENGTH];                      /* 120 */
   MNIbyte CollimatorType[4];                                /* 121 */
   MNIbyte CollimatorEnd[2];                                 /* 124 */
   MNIbyte FillerBlock1[ID_LENGTH];                          /* 127 */
   MNIbyte Isotope[NUMBER_LENGTH];                           /* 130 */
   MNIbyte Dose[NUMBER_LENGTH];                              /* 144 */
   MNIbyte InjectionTime[TIME_DATE_LENGTH];                  /* 158 */
   MNIbyte FillerBlock2[18];                                 /* 168 */
   MNIbyte Ismod[ID_LENGTH];                                 /* 186 */
   MNIbyte CollectionType[ID_LENGTH];                        /* 189 */
   MNIbyte FillerBlock3[BYTE_LENGTH];                        /* 192 */
   MNIbyte CommentOffset[SHORT_LENGTH];                      /* 193 */
   MNIbyte BlockCount[SHORT_LENGTH];                         /* 195 */
   MNIbyte ReconMask[SHORT_LENGTH];                          /* 197 */
   MNIbyte AdminOffset[SHORT_LENGTH];                        /* 199 */
   MNIbyte ScannerId[SHORT_LENGTH];                          /* 201 */
   MNIbyte FillerBlock4[10];                                 /* 203 */
   MNIbyte DataOffset[SHORT_LENGTH];                         /* 213 */
   MNIbyte CountsOffset[SHORT_LENGTH];                       /* 215 */
   MNIbyte FillerBlock5[14];                                 /* 217 */
   MNIbyte FrameCount[SHORT_LENGTH];                         /* 231 */
   MNIbyte GroupCount[SHORT_LENGTH];                         /* 233 */
   GroupInfo Groups[10];                                     /* 235 */
   MNIbyte ReconControl[24];                                 /* 405 */
   MNIbyte FillerBlock6[26];                                 /* 429 */
   MNIbyte PrecPoints[BYTE_LENGTH];                          /* 455 */
   MNIbyte CycleCount[BYTE_LENGTH];                          /* 456 */
   MNIbyte PrecRads[BYTE_LENGTH];                            /* 457 */
   MNIbyte CmtrlPts[BYTE_LENGTH];                            /* 458 */
   MNIbyte FillerBlock7[6];                                  /* 459 */
   MNIbyte SeMask[BYTE_LENGTH];                              /* 465 */
   MNIbyte DelayMask[BYTE_LENGTH];                           /* 466 */
   MNIbyte DelayTime[SHORT_LENGTH];                          /* 467 */
   CouchInfo CouchSets[5];                                   /* 469 */
   MNIbyte DiskUnit[3];                                      /* 499 */
   MNIbyte FillerBlock[BYTE_LENGTH];                         /* 502 */
   MNIbyte TiltAngle[SHORT_LENGTH];                          /* 503 */
   MNIbyte AcquisitionStatus[SHORT_LENGTH];                  /* 505 */
   MNIbyte FillerBlock8[4];                                  /* 507 */
   MNIbyte OverflowStop[BYTE_LENGTH];                        /* 511 */
   MNIbyte SetCount[BYTE_LENGTH];                            /* 512 */
} MniHeader;

/*
 * Function prototypes
 */

MniInfo *open_mni_file(char *mni_filename, Boolean dump_only, int dump_type);
void read_mni_parameters(MniInfo *MniFile,
                         int     *slices,
                         int     slice_count,
                         int     *frames,
                         int     frame_count,
                         Boolean dump_only,
                         int     dump_type);
void close_mni_file(MniInfo *MniFile);
void get_mni_slice(MniInfo *MniFile,
                   int slice,
                   int frame,
                   void *out_image,
                   int precision,
                   Boolean scale);
