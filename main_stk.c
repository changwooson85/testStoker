/*****************************************************************************/
/* 1.System Name  : STKinf (LTS's STK interface processing Server)           */
/* 2.Program ID   : main_stk.c                                               */
/* 3.Description  : main program of STKinf                                   */
/* 4.In/Out Table : None                                                     */
/* 5.Functions    :                                                          */
/*    main       - STKinf's main function                                    */
/*    readConfig - Configuration file Loading                                */
/*    waitConnection - Message receive waiting                               */
/*    stkMsgThread - Message  thread function                               */
/*    freeThreadInfo - thread funtion resource free                          */
/*    signalHandler - system shut down resource free                         */
/*    stk_recv - STK client message recv function                            */
/*    stk_rAssociateUnit - Logical connect function                          */
/*    stk_rAssociateUnit_hton - Endian change function                       */
/*    stk_rAssociateUnit_ntoh - Endian change function                       */
/*    stk_rAssociateUnitErrReply - Logical connect error function            */
/*    stk_rClose - STK client connection close function                      */
/*    stk_rClose_hton - Endian change function                               */
/*    stk_rClose_ntoh - Endian change function                               */
/*    stk_rConnect - STK client connection function                          */
/*    stk_rConnect_hton - Endian change function                             */
/*    stk_rConnect_ntoh - Endian change function                             */
/*    stk_rDisassociateUnit - Logical disconnect function                    */
/*    stk_rDisassociateUnit_hton - Endian change function                    */
/*    stk_rDisassociateUnit_ntoh - Endian change function                    */
/*    stk_rDisassociateUnitErrReply - Logcial disconnect error function      */
/*    stk_rDisplayMsg - Teltag write function                                */
/*    stk_rDisplayMsg_hton - Endian change function                          */
/*    stk_rDisplayMsg_ntoh - Endian change function                          */
/*    stk_rDisplayMsgErrReply - Teltag write error function                  */
/*    stk_rLogicalToPhysicalUnit - Logical To Physical function(teltag)      */
/*    stk_rLogicalToPhysicalSensor - Logical To Physical function            */
/*    stk_rListUnitAtIrt - Teltag info read function                         */
/*    stk_rListUnitAtIrt_hton - Endian change function                       */
/*    stk_rListUnitAtIrt_ntoh - Endian change function                       */
/*    stk_rListUnitAtIrtErrReply - Teltag info read error function           */
/*    stk_rPhysicalToLogicalSensor - IrtName request function                */
/*    stk_rPhysicalToLogicalSensor_hton - Endian change function             */
/*    stk_rPhysicalToLogicalSensor_ntoh - Endian change function             */
/*    stk_rReadMemory - Logical info request function                        */
/*    stk_rReadMemory_hton - Endian change function                          */
/*    stk_rReadMemory_ntoh - Endian change function                          */
/*    stk_rReadMemoryErrReply - Logical info request error function          */
/*    GetBcrIDByLogicalID - Cassete ID DB query function                     */
/*    GetBcrIPByIrt - BCR IP DB query function                               */
/*    GetIrtNameByIrt - IrtName DB query function                            */
/*    GetLogicalIDByBcrID - Logical ID DB query function                     */
/*    GetLotInfo - LOT info DB query function                                */
/*    GetStkTypeByIP - STK type DB query function                            */
/*    GetTagIDByBcrID - Teltag ID DB query function                          */
/*    GetBcrIDByTagID - Bcr ID DB query function                             */
/*    GetCurrentHistoryByBcrID - Current cst inout history DB query function */
/*    InsertBcrTagMapping - Bcr Tag mapping info DB insert function          */
/*    getErrorReceiver - Error message receiver file read                    */ 
/*    bcr_connect - STK BCR module connect function                          */
/*    bcr_SendRecv - STK BCR send and recv module function                   */
/*    hht_connect - HHTinf connect module function                           */
/*    hht_SendRecv - HHTinf send and recv module function                    */
/*    hht_sendErrMsg - HHTinf Error Message send module function             */
/*    rid_connect - Ridian server connect module function                    */
/*    rid_close - Ridian server close module function                        */
/*    rid_SendRecv - Ridian server send and recv module function             */
/*    lts_inputRequest - LTSsvr LOT/Reticle input process function           */
/*    lts_outputRequest - LTSsvr LOT/Reticle output process function         */
/*    lts_rAssociateUnit - LTSsvr Reticle connect function                   */
/*    lts_rDisassociateUnit - LTSsvr Reticle disconnect function             */
/*    lts_SendRecv - LTSsvr send and recv module function                    */
/*    stk_RecvLogSvr - STK recv message LOGsvr write                         */
/*    stk_SendLogSvr - STK send message LOGsvr write                         */
/*    stk_MakeSendMsg - STK make send message function                       */
/*    bigToLitts - Endian change function                                    */
/*    bigToLittl - Endian change function                                    */ 
/*    msgVerify - MSG Verify function                                        */                 
/* 6.Notification :                                                          */
/*   Ver.  ID/Name   Organization    Date       Comment                      */
/*   1.0  CHOI.H.G   GYURICOM       2005.03.21  Original Prototype           */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/* Application Include Files                                                 */
/*---------------------------------------------------------------------------*/
#include "msgstruct.h"
#include "eDB.h"
/*---------------------------------------------------------------------------*/
/* Constants, Macro Declaration                                              */
/*---------------------------------------------------------------------------*/
#define SERVER_NAME	        "STKinf"
#define CONFIG_FILE         "STKinf.conf"

#define LOTPODTYPE          1
#define RETICLEPODTYPE      2
#define RETICLEBARETYPE     3 
#define HEADERSIZE          3
#define TYPEBYTE            2
#define LENGTHSIZE          2
#define BCRLEN              7
#define READLEN             16

#define UNKNOWN             "ZZZ-UNKNOWN"

#define ERROR               0
#define INFO                3
#define DEBUG               5

/*---------------------------------------------------------------------------*/
/* Local Function Prototype Declaration                                      */
/*---------------------------------------------------------------------------*/
void * stkMsgThread(void * arg);
void * bcrMsgThread(void *arg);
void * bcrWaitThread(void *arg);

void freeThreadInfo(void *arg);
void signalHandler(int sig);
void waitConnection(pthread_attr_t *, int ,char *, char *, int , char *);
int createBcrWaitThread(pthread_attr_t *, unsigned int , int , char *);

int readConfig(int *, int *, int *, char *, char *, char *, char *, char *);
int stk_recv(int , char *, char *, char *);
int stk_rAssociateUnit(int , int *, char *, char *, int , char *);
int stk_rAssociateUnit_hton(rGenRequest *);
int stk_rAssociateUnit_ntoh(rGenReply *);
int stk_rAssociateUnitErrReply(int , char *, char *, int , char *);
int stk_rClose(int , char *, char *, char *);
int stk_rClose_hton(rSimpleRequest *);
int stk_rClose_ntoh(rSimpleReply *);
int stk_rConnect(int , char *, char *, char *);
int stk_rConnect_hton(rConnectRequest *);
int stk_rConnect_ntoh(rConnectReply *);
int stk_rDisassociateUnit(int , int *, char *, char *, int , char *);
int stk_rDisassociateUnit_hton(rGenRequest *);
int stk_rDisassociateUnit_ntoh(rGenReply *);
int stk_rDisassociateUnitErrReply(int , char *, char *, int , char *);
int stk_rDisplayMsg(int , int *, char *, char *, int , char *);
int stk_rDisplayMsg_hton(rPostLineRequest *);
int stk_rDisplayMsg_ntoh(rSimpleReply *);
int stk_rDisplayMsgErrReply(int , char *, char *, int , char *);
int stk_rLogicalToPhysicalUnit(int , int *, char *, char *, char *, char *);
int stk_rListUnitAtIrt(int , int *, char *, char *, int , char *);
int stk_rListUnitAtIrt_hton(rQuerySensorRequest *);
int stk_rListUnitAtIrt_ntoh(rQuerySensorReply *);
int stk_rListUnitAtIrtErrReply(int , char *, char *, int , char *);
int stk_rLogicalToPhysicalSensor(int *, char *, char *, char *);
int stk_rPhysicalToLogicalSensor(int , char *, char *, char *);
int stk_rPhysicalToLogicalSensor_hton(rGenRequest *);
int stk_rPhysicalToLogicalSensor_ntoh(rGenReply *);
int stk_rReadMemory(int , char *, char *, int ,char *, char *);
int stk_rReadMemory_hton(rReadRAMRequest *);
int stk_rReadMemory_ntoh( rReadRAMReply *);
int stk_rReadMemoryErrReply(int , char *, char *, int ,char *, char *);

int GetBcrIDByLogicalID(char *, char *, char *);
int GetNextCleanData(char *, char *, char *, char *);
int GetBcrIPByIrt(char *, char *, char *, char *, char *);
int GetIrtNameByIrt(char *, char *, char *);
int GetLogicalIDByBcrID(char *, char *, char *);
int GetLotInfo(char *, char *, char *);
int GetStkTypeByIP(char *, char *, char *);
int GetTagIDByBcrID(char *, char *, char *);
int GetBcrIDByTagID(char *, char *, char *);
int GetBcrInfoByBcrIP(char *, char *, char *, char *, char *, char *);
int GetLocationByBcrID(char *, char* , char *, char *);
int GetCurrentHistoryByBcrID(char *, char* , char* , char *);
int getErrorReceiver(char *, char *);
int InsertBcrTagMapping(char *, char *, char *, char *);

int bcr_connect(char *);
int bcr_SendRecv(char *, char *,char *, char *);

int rid_connect(char *);
char rid_SendRecv(int *, char *, unsigned short int, char *, unsigned short int, char *);
int rid_close(int , char *);

int hht_connect();
int hht_SendRecv(char *, char *, char *, char *);
int hht_sendErrMsg(char *, char *, char *);

int lts_inputRequest(int , char *, char *, char *, char *, char *);
int lts_outputRequest(int , char *, char *, char *, char *, char *);
int lts_rAssociateUnit(int , char *, char *, char *, char *, char *);
int lts_rDisassociateUnit(int , char *, char *, char *, char *, char *);
int lts_SendRecv(int , char *, char *, char *, char *, char *);

int stk_RecvLogSvr(void *, char , char *, char *, int);
int stk_SendLogSvr(void *, char , char *, char *, int);
int stk_MakeSendMsg(void *, void * , char, char *);

int getRecipe(char *, char *);
//ASML 용
int getCstName(char *, char *, char *);

unsigned short bigToLitts(unsigned short);
unsigned int bigToLittl(unsigned int);
int msgVerify(char);

/* GLOBAL 변수 선언 */
char ltsFile[256]={0,};
char logFile[256]={0,};
char RIDSERVER_IP[20]={0,};
int  RIDSERVER_PORT = 0;
char HHTINF_IP[20]={0,};
int  HHTINF_PORT = 0;

int  MOD_PORT = 0;
int  RETRY = 2;
int  gLotParallelMaint = 1;		    /* 병행유지, 전환완료 플래그 */
int  gReticleParallelMaint = 1;		/* 병행유지, 전환완료 플래그 */




/*****************************************************************************/
/* 1. Function Name: getCstName                                              */
/* 2. Description  : Logical ID Query                                        */
/* 3. Parameters   : char* bcrID     - Cassete ID                            */
/*                   char* cstName   - cstName                               */
/*                   char* errmsg    - error message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int getCstName(char *bcrID, char *cstName, char *errmsg)
{
    int ret_i;
    int l_length = -1;
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};
    
    pthread_mutex_lock(&msg_mtx);	
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        "SELECT RTRIM(CST_ID) CST_ID, RTRIM(CST_NAME) CST_NAME FROM LTSCST WHERE CST_ID=:v1");

    strcpy( ga_bindframe_stt.bind_str[0], bcrID );
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
   
    if( ret_i > 0 ){
        if(getSubstr(resultSet, "^CST_NAME=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(cstName, tmp_str);   
            } else {
                sprintf(errmsg, "ERROR: CSTID[%s] CST_NAME is null",bcrID);
                cstName[0] = NULL;
                return true;
            }
		}
	} else {
        sprintf(errmsg, "ERROR: GetCstName CSTID[%s]::%s", bcrID, resultSet);
        return false;
    }
     return true;
}

int getRecipe (char *lotID, char *recipeID )
{

  char resultSet[BUFSIZ];
  char tmp_str[MAX_ITEMS];
  char Next_EQ[10];
  char Next_STK[10];
  char tmp_RCP[20];
   char errmsg[BUFSIZ]={0,};

  int ret;

  memset(resultSet,0x00, BUFSIZ);
  memset(tmp_str, 0x00, MAX_ITEMS);
  memset(Next_EQ, 0x00, 10);
  memset(Next_STK, 0x00, 10);
  memset(tmp_RCP, 0x00,20);

  /*Next STK 조회 */
        pthread_mutex_lock(&msg_mtx);
        memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
        memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
        memset( resultSet, 0x00, sizeof(resultSet) );

        sprintf(ga_sqlframe_stt.sqlstat_str,
           "SELECT SUBSTR(A.EQ_ID,1,6) AS NEXT_EQ  \
            FROM \
            LSUSER.LS_EQ_LOT_SEQ_NEW A, MWIPLOTSTS B \
            WHERE A.FACILITY = 'AFB1' \
            AND B.LOT_id = :v1 \
            AND A.LOT_NO = B.LOT_ID \
            AND A.ROUTE = B.FLOW \
            AND A.OPER = B.OPER");
        
           
        strcpy( ga_bindframe_stt.bind_str[0], lotID );
        ret = eDB_query( SQL_COMMAND, (char *)0, 1 );
        memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
        pthread_mutex_unlock(&msg_mtx);

        if( ret > 0 ){
            if( getSubstr(resultSet, "^NEXT_EQ=", "^", tmp_str) == SUCCESS ){
                if(tmp_str[0] != '-')
            	    strcpy(Next_EQ, tmp_str);
                 else
            	    Next_EQ[0] = NULL;
            }
         } else {
                Next_EQ[0] = NULL;
                Next_STK[0] = NULL;
         }

        pthread_mutex_lock(&msg_mtx);
        memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
        memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
        memset( resultSet, 0x00, sizeof(resultSet) );

        sprintf(ga_sqlframe_stt.sqlstat_str,
          "SELECT RTRIM(RES_CMF_25) AS NEXT_STK \
           FROM \
           MRASRESDEF \
           WHERE \
           FACTORY = 'AFB1' \
           AND RES_ID = :v1 ");
           
        strcpy( ga_bindframe_stt.bind_str[0], Next_EQ );
        ret = eDB_query( SQL_COMMAND, (char *)0, 1 );
        memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
        pthread_mutex_unlock(&msg_mtx);

        if( ret > 0 ){
            if( getSubstr(resultSet, "^NEXT_STK=", "^", tmp_str) == SUCCESS ){
                if(tmp_str[0] != '-')
            	    strcpy(Next_STK, tmp_str);
                 else
            	    Next_STK[0] = NULL;
            }
         } else {
                Next_STK[0] = NULL;
         }

        sprintf(errmsg, "DEBUG: LOT[%s] Next EQ[%s] Next STK[%s]", lotID, Next_EQ, Next_STK);
        logMessage(DEBUG, errmsg);


      /*Next STK 조회 */
      
      
      
        /*Update Recipe 가 존재 한다면 Update Recipe Return */
        pthread_mutex_lock(&msg_mtx);
        memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
        memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
        memset( resultSet, 0x00, sizeof(resultSet) );

        sprintf(ga_sqlframe_stt.sqlstat_str,
           "SELECT RECIPE FROM MRCPLOTRCP WHERE LOT_ID = :v1");
           
        strcpy( ga_bindframe_stt.bind_str[0], lotID );
        ret = eDB_query( SQL_COMMAND, (char *)0, 1 );
        memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
        pthread_mutex_unlock(&msg_mtx);

        if( ret > 0 ){
            if( getSubstr(resultSet, "^RECIPE=", "^", tmp_str) == SUCCESS ){
                if(tmp_str[0] != '-')
            	    strcpy(tmp_RCP, tmp_str);
                 else
            	    tmp_RCP[0] = NULL;
            }
         } else {
                tmp_RCP[0] = NULL;
         }

         if(tmp_RCP[0] != NULL){
            /*Update Recipe 가 존재 한다면 Update Recipe Return */     
                if (Next_STK[0] != NULL ){
                     sprintf (recipeID,"%.20s-%.6s",tmp_RCP, Next_STK);
                } else {
                     sprintf (recipeID,"%.20s",tmp_RCP);
                }
                 return 1;
         } 
        	
        /*비공통 Recipe 가 존재 한다면 비공통 Recipe Return */
        pthread_mutex_lock(&msg_mtx);
        memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
        memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
        memset( resultSet, 0x00, sizeof(resultSet) );

        sprintf(ga_sqlframe_stt.sqlstat_str,
                "SELECT B.RECIPE RECIPE FROM MWIPLOTSTS A, MRCPMFODEF B \
                 WHERE A.LOT_ID = :v1 \
                 AND A.FACTORY =B.FACTORY \
                 AND B.OPT_LEVEL = '1' \
                 AND A.MAT_ID = B.MAT_ID \
                 AND A.FLOW = B.FLOW \
                 AND A.OPER = B.OPER");
           
        strcpy( ga_bindframe_stt.bind_str[0], lotID );
        ret = eDB_query( SQL_COMMAND, (char *)0, 1 );
        memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
        pthread_mutex_unlock(&msg_mtx);

        if( ret > 0 ){
            if( getSubstr(resultSet, "^RECIPE=", "^", tmp_str) == SUCCESS ){
                if(tmp_str[0] != '-')
            	    strcpy(tmp_RCP, tmp_str);
                 else
            	    tmp_RCP[0] = NULL;
            }
         } else {
                tmp_RCP[0] = NULL;
         }

         if(tmp_RCP[0] != NULL){
            /*비공통 Recipe 가 존재 한다면 비공통 Recipe Return */     
                if (Next_STK[0] != NULL ){
                     sprintf (recipeID,"%.20s-%.6s",tmp_RCP, Next_STK);
                } else {
                     sprintf (recipeID,"%.20s",tmp_RCP);
                }
                 return 2;
         } 

        /*공통 Recipe 가 존재 한다면 공통 Recipe Return */
        pthread_mutex_lock(&msg_mtx);
        memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
        memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
        memset( resultSet, 0x00, sizeof(resultSet) );

        sprintf(ga_sqlframe_stt.sqlstat_str,
                "SELECT B.RECIPE RECIPE FROM MWIPLOTSTS A, MRCPMFODEF B \
                 WHERE A.LOT_ID = :v1 \
                 AND A.FACTORY =B.FACTORY \
                 AND B.OPT_LEVEL = '2' \
                 AND B.MAT_ID = ' ' \
                 AND A.FLOW = B.FLOW \
                 AND A.OPER = B.OPER");

        strcpy( ga_bindframe_stt.bind_str[0], lotID );
        ret = eDB_query( SQL_COMMAND, (char *)0, 1 );
        memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
        pthread_mutex_unlock(&msg_mtx);

        if( ret > 0 ){
            if( getSubstr(resultSet, "^RECIPE=", "^", tmp_str) == SUCCESS ){
                if(tmp_str[0] != '-')
            	    strcpy(tmp_RCP, tmp_str);
                 else
            	    tmp_RCP[0] = NULL;
            }
         } else {
                tmp_RCP[0] = NULL;
         }

         if(tmp_RCP[0] != NULL){
            /*Update Recipe 가 존재 한다면 비공통 Recipe Return */     
                if (Next_STK[0] != NULL ){
                     sprintf (recipeID,"%.20s-%.6s",tmp_RCP, Next_STK);
                } else {
                     sprintf (recipeID,"%.20s",tmp_RCP);
                }
                 return 3;
         } 
         
         return 0;
}

/*****************************************************************************/
/* 1.Function Name: main                                                     */
/* 2.Description  : HHTinf's main function                                   */
/* 3.Parameters   : None                                                     */
/* 4.Return Value : None                                                     */
/*****************************************************************************/
void main()
{
	pthread_attr_t attr;		/* 글로벌 쓰레드 속성   */
	int serv_smq;				/* 서버큐 디스크립터    */
	int ltssvr_smq;				/* LTSsvr큐 디스크립터  */
	int logsvr_smq;				/* LOGsvr큐 디스크립터  */
	int s_port = 0;				/* 서버 리슨 포트       */
	int l_queue = 0;			/* 서버 리슨 큐 사이즈  */
	int t_max = 0;			    /* 쓰레드 최대 갯수     */

	char pid_file[BUFSIZ + 1];	/* pid 파일이름         */
	char lts_file[BUFSIZ + 1];	/* smq 파일이름(LTSsvr) */
	char msg_file[BUFSIZ + 1];	/* smq 파일이름(LOGsvr) */
	char log_file[BUFSIZ + 1];	/* LOG 파일이름         */
	char svr_msg[BUFSIZ + 1];	/* 데몬 메세지          */

	/* 환경설정 파일 로드 */
	if ( readConfig(&s_port, &l_queue, &t_max,
					pid_file, lts_file, log_file, msg_file, svr_msg) == false ) {
		logMessage(ERROR, svr_msg);
		exit(1);
	}

	/* 데몬 프로세스로 변경 */
	if ( setDaemon(pid_file, svr_msg) == false ) {
		logMessage(ERROR, svr_msg);
		exit(1);
	}

	/* 데몬 프로세스 초기화 */
	if ( initDaemon(&attr, log_file, svr_msg) == false ) {
		logMessage(ERROR, svr_msg);
		exit(1);
	}
    
    signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, signalHandler);
	
	/* 데몬 소켓 초기화 */
	if ( (serv_smq = initSocket_inet(s_port, l_queue, svr_msg) ) == false ) {
		logMessage(ERROR, svr_msg);
		exit(1);
	}
    /* Health check thread create for L4 */
    if(createHealthThread(&attr, s_port+5, l_queue, svr_msg) != 0)
    {
    	logMessage(ERROR, svr_msg);
        exit(1);
    }
    if(createBcrWaitThread(&attr, s_port+3, l_queue, svr_msg) != 0){
        logMessage(ERROR, svr_msg);
        exit(1);
    }
    /* Log file check thread create */
    if(createLogFileChangeThread(&attr, log_file, svr_msg) != 0)
    {
    	logMessage(ERROR, svr_msg);
        exit(1);
    }
	sprintf(svr_msg, "INFO : %s 프로세스가 기동하였습니다", SERVER_NAME);
	logMessage(ERROR, svr_msg);
    
	/* 클라이언 요청 처리 */
	waitConnection(&attr, serv_smq, lts_file, msg_file, t_max, svr_msg);
	logMessage(ERROR, svr_msg);
    
    eDB_disconnect();    		
    sprintf(svr_msg,"INFO : FCCM DB 연결 해제 성공!!");
	logDaemonMsg(ERROR, svr_msg);
	
	/* 데몬 자원 해제 */
	freeDaemon(serv_smq);
	exit(0);
}

/*****************************************************************************/
/* 1.Function Name: readConfig                                               */
/* 2.Description  : STKinf's Configuration Setting                           */
/* 3.Parameters   : int *sport    - 서버 리슨 포트                           */
/*                  int *lqueue   - 서버 리슨 큐                             */
/*                  int *tmax     - 쓰레드 최대 갯수                         */
/*                  char *pfile   - pid 파일                                 */
/*                  char *sfile   - smq 파일 (LTSsvr 송신)                   */
/*                  char *lfile   - error log 파일                           */
/*                  char *mfile   - msg 파일 (LOGsvr 송신)                   */
/*                  char *msg     - 처리결과 메세지                          */
/* 4.Return Value : true  - 성공                                             */
/*                  false - 실패                                             */
/*****************************************************************************/
int readConfig(int *sport, int *lqueue, int *tmax,
			   char *pfile, char *sfile, char *lfile, char *mfile, char *msg)
{
	FILE *fp = NULL;
	char *ldenv = NULL;
	char *token = NULL;
    char *tail = NULL;
	char fconf[BUFSIZ + 1];
	char tmp[BUFSIZ + 1];
    char lp[BUFSIZ + 1];
    char dbuser[10];
    char dbpasswd[10];
    char dbcon[10]; 
    
    int ret_i;
    
	size_t rc = 0;
	
	if ( (ldenv = getenv("FCCM_DB_CONF")) == NULL ) {
    	sprintf(msg, "ERROR: DB 환경변수 [%s]가 정의되지 않았습니다", "FCCM_DB_CONF");
        return false;
    }

    getNthItem( ldenv, 1, ",", dbuser );    
    getNthItem( ldenv, 2, ",", dbpasswd );    
    getNthItem( ldenv, 3, ",", dbcon );     
    
    ret_i = eDB_connect_allocation( dbuser, dbpasswd, dbcon );
    if( ret_i == FAIL )
    {
        sprintf( msg, "ERROR: DB Connect Error occurred!!::%s",ga_sqlframe_stt.result_str);
        return false;
    }

	/* 환경파일 FULL PATH 설정 */
    if ( (ldenv = getenv("LTSSERVER_CONF")) == NULL ) {
    	sprintf(msg, "ERROR: 환경변수 [%s]가 정의되지 않았습니다", "LTSSERVER_CONF");
        return false;
    }
    strcat(strcat(strcpy(fconf, ldenv), "/"), CONFIG_FILE);
    
    
    /* 환경파일 열기 */
    if ( (fp = fopen(fconf, "r")) == NULL ) {
    	sprintf(msg, "ERROR: 환경파일 [%s]을 읽을 수 없습니다", CONFIG_FILE);
    	return false;
    }

	/* 환경파일 로드 */
	while (fgets(lp, BUFSIZ + 1, fp) != NULL) {
		/* '#'로 시작하는 문자열 무시 */
		if (*lp == '#') continue;

		/* 문자열 복사 */
        trimEx(lp, tmp);

		/* 환경변수 잘라내기 */
        token = strtok(lp, "=");

        /* 환경변수 오류 검사 및 환경변수값 저장 */
        if (token == NULL) {
        	sprintf(msg, "ERROR: 환경파일 구성이 잘못되었습니다");
        	return false;
		}
		else if (strcmp("STKinf.listen.port", token) == 0) {
			*sport = (int)strtol(&tmp[strlen(token) + 1], &tail, 0);
		}
		else if (strcmp("STKinf.listen.queue", token) == 0) {
			*lqueue = (int)strtol(&tmp[strlen(token) + 1], &tail, 0);
		}
		else if (strcmp("STKinf.thread.max", token) == 0) {
			*tmax = (int)strtol(&tmp[strlen(token) + 1], &tail, 0);
		}
		else if (strcmp("STKinf.self.pidfile", token) == 0) {
			strcpy(pfile, &tmp[strlen(token) + 1]);
		}
		else if (strcmp("STKinf.socket.timeout", token) == 0) {
			timeout = (int)strtol(&tmp[strlen(token) + 1], &tail, 0);
		}
		else if (strcmp("STKinf.self.smqfile", token) == 0) {
			strcpy(sfile, &tmp[strlen(token) + 1]);
			strcpy(ltsFile,&tmp[strlen(token) + 1]);
		}
		else if (strcmp("STKinf.self.logfile", token) == 0) {
			strcpy(lfile, &tmp[strlen(token) + 1]);
		}
		else if (strcmp("STKinf.remote.smqfile", token) == 0) {
			strcpy(mfile, &tmp[strlen(token) + 1]);
			strcpy(logFile, &tmp[strlen(token) + 1]);
		}
    	else if (strcmp("STKinf.lot.parallel.flag", token) == 0) {
    		gLotParallelMaint = (int)strtol(&tmp[strlen(token) + 1], &tail, 0);
    		if(gLotParallelMaint == 0){
    		    printf("INFO : LOT 전환모드로 STKinf start\n");
    		} else if(gLotParallelMaint == 1){
    		    printf("INFO : LOT 병행모드로 STKinf start\n");
    		} else {
    		    sprintf(msg, "ERROR: STKinf.lot.parallel.flag에 [%d]값이 셋팅되었습니다",gLotParallelMaint);
    		    return false;
    		}
    	}
    	else if (strcmp("STKinf.reticle.parallel.flag", token) == 0) {
    		gReticleParallelMaint = (int)strtol(&tmp[strlen(token) + 1], &tail, 0);
    		if(gReticleParallelMaint == 0){
    		    printf("INFO : Reticle 전환모드로 STKinf start\n");
    		} else if(gReticleParallelMaint == 1){
    		    printf("INFO : Reticle 병행모드로 STKinf start\n");
    		} else {
    		    sprintf(msg, "ERROR: STKinf.reticle.parallel.flag에 [%d]값이 셋팅되었습니다",gLotParallelMaint);
    		    return false;
    		}
    	}
    	else if (strcmp("STKinf.ridian.ip", token) == 0) {
    		strcpy(RIDSERVER_IP, &tmp[strlen(token) + 1]);
    	} 
    	else if (strcmp("STKinf.ridian.port", token) == 0) {
    		RIDSERVER_PORT = (int)strtol(&tmp[strlen(token) + 1], &tail, 0);
    	}
    	else if (strcmp("STKinf.HHTinf.ip", token) == 0) {
    		strcpy(HHTINF_IP, &tmp[strlen(token) + 1]);
    	} 
    	else if (strcmp("STKinf.HHTinf.port", token) == 0) {
    		HHTINF_PORT = (int)strtol(&tmp[strlen(token) + 1], &tail, 0);
    	}
    	else if (strcmp("STKinf.retry", token) == 0) {
    		RETRY = (int)strtol(&tmp[strlen(token) + 1], &tail, 0);
    	} 
    	else if (strcmp("STKinf.bcr.port", token) == 0) {
    		MOD_PORT = (int)strtol(&tmp[strlen(token) + 1], &tail, 0);
    	}
    	else if (strcmp("STKinf.superbiser.id", token) == 0) {
		}
    	else if (strcmp("STKinf.log.level", token) == 0) {
    		gLogLevel = (int)strtol(&tmp[strlen(token) + 1], &tail, 0);
    		if(gLogLevel < 0){
    		    gLogLevel = 9;
    		}
    	} 
		else {
			sprintf(msg, "ERROR: 지원하지 않는 환경변수 [%s]가 사용되었습니다", token);
			return false;
		}
	}

	/* 환경파일 닫기 */
	fclose(fp);

	/* 처리결과 반환 */
	return true;
}

void signalHandler(int sig)
{
	eDB_disconnect();
	logMessage(ERROR, "INFO : STKinf 프로세스가 종료되었습니다");
	exit(0);
}

/*****************************************************************************/
/* 1.Function Name: waitConnection                                           */
/* 2.Description  : Message Thread Create by Client Request                  */
/* 3.Parameters   : pthread_attr_t *attr - 쓰레드 속성                       */
/*                  char *lts_file       - LTSsvr socket file name           */
/*                  char *msg_file       - LOGsvr socket file name           */
/*                  int tmax             - 쓰레드 최대 갯수                  */
/*                  char *msg            - error message                     */
/* 4.Return Value : None                                                     */
/*****************************************************************************/
void waitConnection(pthread_attr_t *attr, int sockfd,
					char *lts_file, char *msg_file, int tmax, char *msg)
{
    MSG_THREAD_INFO *tinfo;		/* 메세지 쓰레드 정보 */
    
	/* 쓰레드 갯수 초기화 */
	thread_cnt = 0;

	char *stkIP = NULL;
    char stkName[12]={0,};
    
	/* 수신메세지 요청을 처리 */
	while (1) {
        if ( (tinfo = calloc(1, sizeof(*tinfo))) == NULL ) {
			sprintf(msg, "ERROR: 쓰레드 정보를 위한 메모리 할당에 실패하였습니다");
			logMessage(ERROR, msg);
			continue;
		}
		tinfo->clnt_addr_len = sizeof(tinfo->clnt_addr);

		/* 요청 대기 */
		if ( (tinfo->clnt_sockfd = accept(sockfd, (struct sockaddr *)&tinfo->clnt_addr,
					  &tinfo->clnt_addr_len)) == false) {
			sprintf(msg, "ERROR: 클라이언트로부터 요청을 받을 수 없습니다 INFO:%s",strerror(errno));
			logMessage(ERROR, msg);
			close(tinfo->clnt_sockfd);
			free(tinfo);
			continue;
		}

		setNonblockSocket(tinfo->clnt_sockfd);
		
        stkIP = NULL;
        memset(stkName, 0x00, sizeof(stkName));
        stkIP = inet_ntoa(((struct sockaddr_in*)(&tinfo->clnt_addr))->sin_addr);
        sprintf(msg, "INFO : STKIP[%s] client connect request", stkIP);
        logMessage(INFO, msg);
        
        sprintf(msg, "INFO : STKIP[%s] new thread create start",stkIP);
        logMessage(INFO, msg);
        
		/* 요청을 처리할 쓰레드 생성 */
		while (thread_cnt >= tmax) usleep(10000);
		if ( pthread_create(&tinfo->msg_tid, attr, stkMsgThread, (void *)tinfo) != 0 ) {
			sprintf(msg, "ERROR: STKIP[%s] new thread create fail",stkIP);
			logMessage(ERROR, msg);
			close(tinfo->clnt_sockfd);
			free(tinfo);
			continue;
		}

		/* 쓰레드 갯수 증가 */
		pthread_mutex_lock(&cnt_mtx);
		++thread_cnt;
		pthread_mutex_unlock(&cnt_mtx);
	}
}

/*****************************************************************************/
/* 1.Function Name: bcrWaitConnection                                        */
/* 2.Description  : Message Thread Create by Client Request                  */
/* 3.Parameters   : pthread_attr_t *attr - 쓰레드 속성                       */
/*                  char *lts_file       - LTSsvr socket file name           */
/*                  char *msg_file       - LOGsvr socket file name           */
/*                  int tmax             - 쓰레드 최대 갯수                  */
/*                  char *msg            - error message                     */
/* 4.Return Value : int                                                      */
/*****************************************************************************/
int createBcrWaitThread(pthread_attr_t *attr, unsigned int sport, int l_queue, char *msg)
{
    BCRTHREADINFO *tinfo;     /* 메세지 쓰레드 정보 */

    /* 수신메세지 요청을 처리 */
    if ( (tinfo = calloc(1, sizeof(*tinfo))) == NULL ) {
        sprintf(msg, "FATAL: BCR 쓰레드 정보를 위한 메모리 할당에 실패하였습니다");
        return -1;
    }
    memset(tinfo, 0x00, sizeof(*tinfo));
	tinfo->attr = *attr;
	/* Create Listen Socket */
    if ( (tinfo->sock = initSocket_inet(sport, l_queue, msg) ) == false ) {
    	free(tinfo);
        return -1;
    }
    
    /* 요청을 처리할 쓰레드 생성 */
    if ( pthread_create(&tinfo->msg_tid, attr, bcrWaitThread, (void *)tinfo) != 0 ) {
        sprintf(msg, "ERROR: BCR 정보를 리슨할 쓰레드를 생성할 수 없습니다");
        return -1;
    }
    
    return 0;
    
	
}

/*****************************************************************************/
/* 1.Function Name: bcrWaitThread                                             */
/* 2.Description  : Listen to L4 Health Check Connection                     */
/* 3.Parameters   : MSG_THREAD_INFO *tinfo - 메세지 쓰레드 정보              */
/* 4.Return Value : None                                                     */
/*****************************************************************************/
void *bcrWaitThread(void *arg)
{
    BCRTHREADINFO *tinfo = (BCRTHREADINFO *)arg;
    MSG_THREAD_INFO *cinfo = NULL;;
    char *bcrIP = NULL;
    char msg[BUFSIZ]={0,};
    pthread_attr_t attr;
    
  	/* 요청 대기 */
    while(1)
    {
        
        if ( (cinfo = calloc(1, sizeof(*cinfo))) == NULL ) {
            sprintf(msg, "FATAL: BCR 클라이언트 쓰레드 정보를 위한 메모리 할당에 실패하였습니다");
            logMessage(ERROR, msg);
            sleep(1);
            continue;
        }
        memset(cinfo, 0x00, sizeof(*cinfo));
        
        cinfo->clnt_addr_len = sizeof(cinfo->clnt_addr);
        if ( (cinfo->clnt_sockfd = accept(tinfo->sock, (struct sockaddr *)&cinfo->clnt_addr,
					  &cinfo->clnt_addr_len)) == false) {
			sprintf(msg, "ERROR: BCR 클라이언트로부터 요청을 받을 수 없습니다 ERRMSG:%s",strerror(errno));
			logMessage(ERROR, msg);
			close(cinfo->clnt_sockfd);
			free(cinfo);
			continue;
		}
        bcrIP = inet_ntoa(((struct sockaddr_in*)(&cinfo->clnt_addr))->sin_addr);
        sprintf(msg, "INFO : BCRIP[%s] client connect request", bcrIP);
        logMessage(INFO, msg);
        
        sprintf(msg, "INFO : BCRIP[%s] new thread create start",bcrIP);
        logMessage(INFO, msg);
        
		setNonblockSocket(cinfo->clnt_sockfd);
		
		if ( pthread_create(&cinfo->msg_tid, &tinfo->attr, bcrMsgThread, (void *)cinfo) != 0 ) {
			sprintf(msg, "ERROR: BCRIP[%s] new thread create fail",bcrIP);
			logMessage(ERROR, msg);
			close(cinfo->clnt_sockfd);
			free(cinfo);
			continue;
		}
		sprintf(msg, "INFO : BCRIP[%s] new thread create success",bcrIP);
        logMessage(INFO, msg);
    }
    close(tinfo->sock);
	free(tinfo);
}

/*****************************************************************************/
/* 1. Function Name: freeThreadInfo                                          */
/* 2. Description  : Message Thread Information Free                         */
/* 3. Parameters   : int *idx - 쓰레드 index                                 */
/* 4. Return Value : None                                                    */
/*****************************************************************************/
void freeThreadInfo(void *arg)
{
	MSG_THREAD_INFO *tinfo = (MSG_THREAD_INFO *)arg;

	/* 쓰레드 소켓 해제 */
	if (close(tinfo->clnt_sockfd) == -1) {
		logMessage(ERROR, "ERROR: 클라이언트 소켓 해제를 실패하였습니다");
	}
    logMessage(DEBUG, "DEBUG: stocker socket close success and thread destroy");
	/* 구조체 메모리 해제 */
	free(tinfo);
}

/*****************************************************************************/
/* 1.Function Name: bcrMsgThread                                            */
/* 2.Description  : Client Message Handling                                  */
/* 3.Parameters   : int *idx - 쓰레드 index                                  */
/* 4.Return Value : None                                                     */
/*****************************************************************************/
void * bcrMsgThread(void * arg)
{
    MSG_THREAD_INFO *tinfo = (MSG_THREAD_INFO *)arg;
    
    int n_bcrRecv;
    int ret,ltsresult;
    int stkType;
    char errmsg[BUFSIZ]={0,};
    char cstID[12]={0,};
    char ltsSendBuf[BUFSIZ]={0,};
    char ltsRecvBuf[BUFSIZ]={0,};
    char *bcrIP = NULL;
    char stkName[24]={0,};
    char bcrName[24]={0,};
    char portName[24]={0,};
    char logicalID[24]={0,};
    char tmpPortName[24]={0,};
    char portType[2]={0,};
    char location[24]={0,};
    
    struct timeval waittime;
    fd_set r_set;
    
    sleep(1);
    waittime.tv_sec =  10;
    waittime.tv_usec = 0;
    
    pthread_cleanup_push(freeThreadInfo, tinfo);
    bcrIP = inet_ntoa(((struct sockaddr_in*)(&tinfo->clnt_addr))->sin_addr);
    
    if(GetBcrInfoByBcrIP(bcrIP, stkName, bcrName, portName, portType, errmsg) == false){
        logMessage(ERROR, errmsg);
    } else {
        if(portType[0] != 'O'){
            sprintf(errmsg, "ERROR: STK[%s] PORT[%s] BCR[%s] IP[%s] port type is not output",stkName, portName, bcrName, bcrIP);
            logMessage(ERROR, errmsg);
        } else {
            stkType = GetStkTypeByStkName(stkName, errmsg);
            if(stkType == false){
                logMessage(ERROR, errmsg);
            } else {
                FD_ZERO(&r_set);
                FD_SET(tinfo->clnt_sockfd,&r_set);
                ret = select(tinfo->clnt_sockfd + 1, &r_set, NULL, NULL, &waittime);
                
                if(ret == -1){
                    sprintf(errmsg,"ERROR: STK[%s] PORT[%s] BCR[%s] IP[%s] select func error",stkName, portName, bcrName, bcrIP);
                    logMessage(ERROR, errmsg);
                } else if( ret == 0){
                    sprintf(errmsg,"ERROR: STK[%s] PORT[%s] BCR[%s] IP[%s] recv timeout", stkName, portName, bcrName, bcrIP);
                    logMessage(ERROR, errmsg);
                } else {
                    if(FD_ISSET(tinfo->clnt_sockfd, &r_set)) {
                        n_bcrRecv = read(tinfo->clnt_sockfd, cstID, sizeof(cstID));
                        if(n_bcrRecv == 7 && strlen(cstID) == 6){
                            if(GetLogicalIDByBcrID(cstID, logicalID, errmsg) == false){
                            } else {
                                if(logicalID[0] == NULL){
                                    sprintf(errmsg, "ERROR: STK[%s] PORT[%s] BCR[%s] IP[%s] CST[%s] empty cstID can't output",
                                                        stkName, portName, bcrName, bcrIP, cstID);
                                    logMessage(ERROR, errmsg);                                                    
                                } else {
                                    if(GetLocationByBcrID(cstID, location, tmpPortName, errmsg) == true){
                                        if(location[0] == NULL){
                                            logMessage(ERROR, errmsg);
                                        } else {
                                            if(lts_outputRequest(stkType, stkName, cstID, logicalID, portName, errmsg) == false){
                                                logMessage(ERROR, errmsg);
                                            }
                                        }
                                    }
                                }
                            }
                        } else {
                            sprintf(errmsg, "ERROR: STK[%s] PORT[%s] BCR[%s] IP[%s] RECV[%s] wrong data recv",
                                                stkName, portName, bcrName, bcrIP, cstID);
                            logMessage(ERROR, errmsg);                                            
                        }
                    }
                }
            }
        }
    }
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    sprintf(errmsg, "DEBUG: STK[%s] PORT[%s] BCR[%s] BCRIP[%s] destroy thread", stkName,portName, bcrName, bcrIP);
    logMessage(DEBUG, errmsg);
	/* 자원 해제 핸들러 실행 */
	pthread_cleanup_pop(1);
}

/*****************************************************************************/
/* 1.Function Name: stkMsgThread                                            */
/* 2.Description  : Client Message Handling                                  */
/* 3.Parameters   : int *idx - 쓰레드 index                                  */
/* 4.Return Value : None                                                     */
/*****************************************************************************/
void * stkMsgThread(void * arg)
{
    MSG_THREAD_INFO *tinfo = (MSG_THREAD_INFO *)arg;
    
    int  stkType;
    int  n_stkrecv;
    char recvBuf[BUFSIZ]={0,};
    char errmsg[BUFSIZ]={0,};
    char lotInfo[32*6]={0,};
    char useTag;
    char *stkIP;
    char stkName[15]={0,};
    int  endFlag = 0;
    int  ridsock = -1;
           
    pthread_cleanup_push(freeThreadInfo, tinfo);
    
    sleep(1);
    stkIP = inet_ntoa(((struct sockaddr_in*)(&tinfo->clnt_addr))->sin_addr);
    
    memset(stkName, 0x00, sizeof(stkName));
    memset(errmsg,  0x00, BUFSIZ);
    
    while(1){
        if(endFlag == 1){
            sprintf(errmsg,"INFO : STK[%s] is close request", stkName);
            logMessage(INFO, errmsg);
            break;
        }
        if (tinfo->clnt_sockfd < 0 || endFlag == 1) {
            sprintf(errmsg,"ERROR: STK[%s] is disconnect",stkName);
            logMessage(ERROR, errmsg);
            break;
        }
        memset(recvBuf, 0x00, BUFSIZ);
        n_stkrecv = stk_recv(tinfo->clnt_sockfd, recvBuf, stkName, errmsg);
        if( n_stkrecv < 0 ){
            sprintf(errmsg,"ERROR: STK[%s] is recv error", stkName);
            logMessage(ERROR, errmsg);
            endFlag = 1;
            break;
    	} else if(n_stkrecv == 0){
    		sprintf(errmsg,"ERROR: STK[%s] is recv time out",stkName);
            logMessage(ERROR, errmsg);
    		endFlag = 1;
    		break;
    	} else {
        	switch(recvBuf[TYPEBYTE]){
            	case msgTypeConnectRequest :
                {
                    if(stk_rConnect(tinfo->clnt_sockfd, recvBuf, stkName, errmsg) == false){
                        sprintf(errmsg, "ERROR: STK[%s] stk_rConnect error",stkName);
                        logMessage(ERROR, errmsg);
                        endFlag = 1;
                        break;
                    } else {
                        stkType = GetStkTypeByStkName(stkName, errmsg);
                        if(stkType == false){
                            logMessage(ERROR, errmsg);
                            endFlag = 1;
                            break;
                        } else {
                            if(stkType == LOTPODTYPE && gLotParallelMaint == 1){
                                if(ridsock > -1){
                                    close(ridsock);
                                }
                                ridsock = rid_connect(stkName);
                                if(ridsock == false){
                                    ridsock = rid_connect(stkName);
                                    if(ridsock == false){
                                        sprintf(errmsg, "ERROR: STK[%s] ridian reconnect fail errno[%d]",stkName,errno);
                                        logMessage(INFO, errmsg);
                                        ridsock = -1;
                                        endFlag = 1;
                                        break;
                                    } else {
                                        sprintf(errmsg, "INFO : STK[%s] ridian reconnect success sock[%d]",stkName,ridsock);
                                        logMessage(INFO, errmsg);
                                        break;
                                    }
                                }
                                break;
                            } else if(stkType != LOTPODTYPE && gReticleParallelMaint == 1){
                                if(ridsock > -1){
                                    close(ridsock);
                                }
                                ridsock = rid_connect(stkName);
                                if(ridsock == false){
                                    ridsock = rid_connect(stkName);
                                    if(ridsock == false){
                                        sprintf(errmsg, "ERROR: STK[%s] ridian reconnect fail errno[%d]",stkName,errno);
                                        logMessage(INFO, errmsg);
                                        ridsock = -1;
                                        endFlag = 1;
                                        break;
                                    } else {
                                        sprintf(errmsg, "INFO : STK[%s] ridian reconnect success sock[%d]",stkName,ridsock);
                                        logMessage(INFO, errmsg);
                                        break;
                                    }
                                }
                                break;
                            }                            
                        }
                    }
                    break;
                }
                case msgTypeCloseRequest :
                {
                    if(stk_rClose(tinfo->clnt_sockfd, recvBuf, stkName, errmsg) == false){
                        sprintf(errmsg, "ERROR: STK[%s] stk_rClose error",stkName);
                        logMessage(ERROR, errmsg);
                        endFlag = 1;
                        break;
                    }else {
                        sprintf(errmsg,"INFO : STK[%s] is normal disconnect ok...",stkName);
                        logMessage(ERROR, errmsg);
                        endFlag = 1;
                        break;
                    }
                    if(stkType == LOTPODTYPE && gLotParallelMaint == 1){
                        close(ridsock);
                        ridsock = -1;
                        sprintf(errmsg, "DEBUG: STK[%s] ridian socket close success", stkName);
                        logMessage(DEBUG, errmsg);
                        break;
                    } else if(stkType != LOTPODTYPE && gReticleParallelMaint == 1){
                        close(ridsock);
                        ridsock = -1;
                        sprintf(errmsg, "DEBUG: STK[%s] ridian socket close success", stkName);
                        logMessage(DEBUG, errmsg);
                        break;
                    }
                    break;
                }
                case msgTypePTLSensor :
                {
                    if(stk_rPhysicalToLogicalSensor(tinfo->clnt_sockfd, recvBuf, stkName, errmsg) == false){
                        sprintf(errmsg, "ERROR: STK[%s] stk_rPhysicalToLogicalSensor error",stkName);
                        logMessage(ERROR, errmsg);
                        endFlag = 1;
                    } 
                    break;
                }            
                case msgTypeQuerySensorLoc :
                {    
                    if(stk_rListUnitAtIrt(tinfo->clnt_sockfd, &ridsock, recvBuf, stkName, stkType, errmsg) == false) {
                        sprintf(errmsg, "ERROR: STK[%s] stk_rListUnitAtIrt ridian socket fail",stkName);
                        logMessage(ERROR, errmsg);
                        if(stkType == LOTPODTYPE && gLotParallelMaint == 1){
                            if(ridsock > -1){
                                close(ridsock); 
                                ridsock = -1;   
                                sprintf(errmsg, "DEBUG: STK[%s] ridian socket close success", stkName);
                                logMessage(DEBUG, errmsg);                              
                            }
                        } else if(stkType != LOTPODTYPE && gReticleParallelMaint == 1){
                            if(ridsock > -1){
                                close(ridsock);
                                ridsock = -1; 
                                sprintf(errmsg, "DEBUG: STK[%s] ridian socket close success", stkName);
                                logMessage(DEBUG, errmsg);                                
                            }
                        }
                        endFlag = 1;
                    }
                    break;
                }
                case msgTypeReadMemory :
                {
                    if(stk_rReadMemory(tinfo->clnt_sockfd, recvBuf, stkName, stkType, lotInfo, errmsg) == false){
                        sprintf(errmsg, "ERROR: STK[%s] stk_rReadMemory error",stkName);
                        logMessage(ERROR, errmsg);
                        endFlag = 1;
                    }
                    break;
                }
                
                case msgTypeAssociateUnit :
                {            
                    if(stk_rAssociateUnit(tinfo->clnt_sockfd, &ridsock, recvBuf, stkName, stkType, errmsg) == false){
                        sprintf(errmsg, "ERROR: STK[%s] stk_rAssociateUnit ridian socket fail",stkName);
                        logMessage(ERROR, errmsg);
                        if(stkType == LOTPODTYPE && gLotParallelMaint == 1){
                            if(ridsock != -1){
                                close(ridsock);
                                ridsock = -1;   
                                sprintf(errmsg, "DEBUG: STK[%s] ridian socket close success", stkName);
                                logMessage(DEBUG, errmsg);                                
                            }
                        } else if(stkType != LOTPODTYPE && gReticleParallelMaint == 1){
                            if(ridsock != -1){
                                close(ridsock);
                                ridsock = -1;  
                                sprintf(errmsg, "DEBUG: STK[%s] ridian socket close success", stkName);
                                logMessage(DEBUG, errmsg);                                 
                            }
                        }
                        endFlag = 1;
                    }
                    break;
                }
                case msgTypeDisassociateUnit :
                {
                    if(stk_rDisassociateUnit(tinfo->clnt_sockfd, &ridsock, recvBuf, stkName, stkType, errmsg) == false){
                        sprintf(errmsg, "ERROR: STK[%s] stk_rDisassociateUnit ridian socket fail",stkName);
                        logMessage(ERROR, errmsg);
                        if(stkType == LOTPODTYPE && gLotParallelMaint == 1){
                            if(ridsock != -1){
                                close(ridsock); 
                                ridsock = -1;  
                                sprintf(errmsg, "DEBUG: STK[%s] ridian socket close success", stkName);
                                logMessage(DEBUG, errmsg);                                
                            }
                        } else if(stkType != LOTPODTYPE && gReticleParallelMaint == 1){
                            if(ridsock != -1){
                                close(ridsock);  
                                ridsock = -1; 
                                sprintf(errmsg, "DEBUG: STK[%s] ridian socket close success", stkName);
                                logMessage(DEBUG, errmsg);                                
                            }
                        }
                        endFlag = 1;
                    }
                    break;
                }
    
                case msgTypeDisplayMsg :
                {
                    if(stk_rDisplayMsg(tinfo->clnt_sockfd, &ridsock, recvBuf, stkName, stkType, errmsg) == false){
                        sprintf(errmsg, "ERROR: STK[%s] stk_rDisplayMsg ridian socket fail",stkName);
                        logMessage(ERROR, errmsg);
                        if(stkType == LOTPODTYPE && gLotParallelMaint == 1){
                            if(ridsock != -1){
                                close(ridsock);  
                                ridsock = -1;   
                                sprintf(errmsg, "DEBUG: STK[%s] ridian socket close success", stkName);
                                logMessage(DEBUG, errmsg);                              
                            }
                        } else if(stkType != LOTPODTYPE && gReticleParallelMaint == 1){
                            if(ridsock != -1){
                                close(ridsock); 
                                sprintf(errmsg, "DEBUG: STK[%s] ridian socket close success", stkName);
                                logMessage(DEBUG, errmsg); 
                                ridsock = -1;                                 
                            }
                        }
                        endFlag = 1;
                    }
                    break;
                }
                
                default :
                {
                    endFlag = 1;
                    sprintf(errmsg, "ERROR: STK[%s] unknwon message recv type[%d]", stkIP, recvBuf[TYPEBYTE]);
                    logMessage(ERROR, errmsg);
                    break;
                }
            }
        }
    }
    if(ridsock != -1){
        close(ridsock);
        sprintf(errmsg, "DEBUG: STK[%s] ridian socket close success and thread destroy", stkName);
        logMessage(DEBUG, errmsg);
    }
    
    pthread_mutex_lock(&cnt_mtx);
	--thread_cnt;
	pthread_mutex_unlock(&cnt_mtx);
	sprintf(errmsg, "DEBUG: STK[%s] thread destroy thread cnt[%d] decrease", stkName, thread_cnt);
    logMessage(DEBUG, errmsg);
	/* 자원 해제 핸들러 중복 실행 방지 */
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    sprintf(errmsg, "DEBUG: STK[%s] 자원 해제 핸들러 중복 실행 방지", stkName);
    logMessage(DEBUG, errmsg);
	/* 자원 해제 핸들러 실행 */
	pthread_cleanup_pop(1);
    
	sprintf(errmsg, "DEBUG: STK[%s] all socket close success and thread destroy", stkName);
    logMessage(DEBUG, errmsg);
}

/*****************************************************************************/
/* 1. Function Name: stk_rConnect                                            */
/* 2. Description  : STK connect 요청 처리                                   */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rConnect(int csock, char *recvBuf, char *stkName, char *errmsg) 
{
    rConnectRequest treq, *req;
    req = &treq;
    memset(req, 0x00, sizeof(rConnectRequest));
    memcpy(req, recvBuf, sizeof(rConnectRequest));
    
    memcpy(stkName, req->name,1);
    memcpy(&stkName[1], &req->name[2], 5);

    sprintf(errmsg,"INFO : STK[%s] rConnect start ", stkName);
    logMessage(INFO, errmsg);
    
    stk_rConnect_hton(req);
    if(stk_RecvLogSvr((void*)req ,req->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg,"ERROR: STK[%s] rConnect recv log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    rConnectReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rConnectReply));
    rep->msgLen	    = sizeof(rConnectReply);
    rep->msgLen     = bigToLitts(rep->msgLen);
	rep->msgType	= msgTypeConnectRequest;
	rep->result	    = bigToLitts(0);
	rep->major	    = bigToLitts(2);
	rep->minor	    = bigToLitts(5);
	rep->byteOrder  = req->byteOrder;
	rep->bitOrder	= req->bitOrder;
	rep->point	    = bigToLitts(0);
	
	if(write(csock,(void*)rep, sizeof(rConnectReply)) <= 0){
	    sprintf(errmsg,"ERROR: STK[%s] disconnet network...",stkName);
	    logMessage(ERROR, errmsg);
	    return false;
	}
	stk_rConnect_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
	    sprintf(errmsg,"ERROR: STK[%s] rConnect send log transfer fail",stkName);
        logMessage(ERROR, errmsg);
    }
    sprintf(errmsg,"INFO : STK[%s] rConnect end  RESULT[%d]", 
                                                    stkName, rep->result);                                                    
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: stk_rPhysicalToLogicalSensor                            */
/* 2. Description  : STK IRT name 요청 처리                                  */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rPhysicalToLogicalSensor(int csock, char *recvBuf, char *stkName, char *errmsg)
{
    char irtName[30]={0,};
    rGenRequest treq, *req;
    req = &treq;
    memset(req, 0x00, sizeof(rGenRequest));
    memcpy(req, recvBuf, sizeof(rGenRequest));
    
    rGenReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rGenReply));
    
    sprintf(errmsg,"INFO : STK[%s] rPhysicalToLogicalSensor start  IRTID[%s]", 
                                                    stkName, req->physicalID);
    logMessage(INFO, errmsg);
    
    stk_rPhysicalToLogicalSensor_hton(req);
    if(stk_RecvLogSvr((void*)req ,req->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rPhysicalToLogicalSensor recv log transfer fail", stkName);
        logMessage(INFO, errmsg);
    }
    
    if(GetIrtNameByIrt(req->physicalID, irtName, errmsg) < 0){
        logMessage(ERROR, errmsg);
        rep->msgLen	    = bigToLitts(sizeof(rGenReply));
    	rep->msgType	= msgTypePTLSensor;
    	rep->result	    = bigToLittl(11);
    	strcpy(rep->physicalID,req->physicalID);
    	strcpy(rep->logicalName,"");
        
        if(write(csock, rep, sizeof(rGenReply)) <= 0){
            sprintf(errmsg,"ERROR: STK[%s] disconnet network",stkName);
    	    logMessage(ERROR, errmsg);
    	    return false;
    	}
    	stk_rPhysicalToLogicalSensor_ntoh(rep);
    	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
            sprintf(errmsg, "ERROR: STK[%s] rPhysicalToLogicalSensor send log transfer fail", stkName);
            logMessage(ERROR, errmsg);
        }
        
        sprintf(errmsg,"INFO : STK[%s] rPhysicalToLogicalSensor end  RESULT[%d], IRTID[%s], PORTID[%s]", 
                                                        stkName, rep->result, rep->physicalID, rep->logicalName);
        logMessage(INFO, errmsg);
        return false;
    }
    
    rep->msgLen	    = bigToLitts(sizeof(rGenReply));
	rep->msgType	= msgTypePTLSensor;
	rep->result	    = bigToLittl(0);
	strcpy(rep->physicalID,req->physicalID);
	strcpy(rep->logicalName,irtName);
    
    if(write(csock, rep, sizeof(rGenReply)) <= 0){
        sprintf(errmsg,"ERROR: STK[%s] disconnet network",stkName);
	    logMessage(ERROR, errmsg);
	    return false;
	}
	stk_rPhysicalToLogicalSensor_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rPhysicalToLogicalSensor send log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    sprintf(errmsg,"INFO : STK[%s] rPhysicalToLogicalSensor end  RESULT[%d], IRTID[%s], PORTID[%s]", 
                                            stkName, rep->result, rep->physicalID, rep->logicalName);
    logMessage(INFO, errmsg);
    return true;
} 

/*****************************************************************************/
/* 1. Function Name: stk_rLogicalToPhysicalSensor                            */
/* 2. Description  : STK IRT name 요청 처리                                  */
/* 3. Parameters   : int   ridsock   - Ridian 소켓 디스크립터                */
/*                   char* portName  - STK portName                          */
/*                   char* stkName   - STK name                              */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rLogicalToPhysicalSensor(int *ridsock, char *portName, char *stkName, char *errmsg)
{
    int reqLen;
    rGenRequest treq, *req;
    req = &treq;
    memset(req, 0x00, sizeof(rGenRequest));
    
    reqLen = sizeof(rGenRequest);
    req->msgLen = sizeof(rGenRequest);
    req->msgType = msgTypeLTPSensor;
    memcpy(req->logicalName, portName, strlen(portName));
        
    rGenReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rGenReply));
    
    sprintf(errmsg,"INFO : STK[%s] stk_rLogicalToPhysicalSensor start  PORTID[%s]", 
                                                            stkName, req->logicalName);
    logMessage(INFO, errmsg);
    
    if(rid_SendRecv(ridsock, (char*)req, reqLen, (char*)rep, sizeof(rGenReply), stkName)=='K'){
        if(rep->result == 0){
        } else {
            sprintf(errmsg, "ERROR: STK[%s] stk_rLogicalToPhysicalSensor PORTID[%s] is not existe", stkName, req->logicalName);
            return 1;
        }
    } else {
        memset(rep,0x00,sizeof(rGenReply));
        sprintf(errmsg, "ERROR: STK[%s] stk_rLogicalToPhysicalSensor ridian send and recv fail.", stkName);
        return false;
    }
    
    sprintf(errmsg,"INFO : STK[%s] rPhysicalToLogicalSensor end  RESULT[%d], IRTID[%s], PORTID[%s]", 
                                            stkName, rep->result, rep->physicalID, rep->logicalName);
    logMessage(INFO, errmsg);
    return true;
} 

/*****************************************************************************/
/* 1. Function Name: stk_rLogicalToPhysicalUnit                              */
/* 2. Description  : tag ID 요청                                             */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   int   ridsock   - ridian 소켓 디스크립터                */
/*                   char* tagID     - Teltag ID                             */ 
/*                   char* logicalID - Logical ID                            */
/*                   char* stkName   - STK name                              */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rLogicalToPhysicalUnit(int csock, int *ridsock, char *tagID, char *logicalID, char *stkName, char *errmsg)
{
    rGenRequest treq, *req;
    req = &treq;
    memset(req, 0x00, sizeof(rGenRequest));
    unsigned short int reqLen;
    
    rGenReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rGenReply));
    
    sprintf(errmsg,"INFO : STK[%s] rLogicalToPhysicalUnit start  LOGICALID[%s]", 
                                                    stkName, logicalID);
    logMessage(INFO, errmsg);
    
    req->msgLen = sizeof(rGenRequest);
    req->msgType = msgTypeLTPUnit;
    req->itemType = 0;
    memcpy(req->logicalName, logicalID, strlen(logicalID));
    reqLen = sizeof(rGenRequest);
    
    if(rid_SendRecv(ridsock, (char*)req, reqLen, (char*)rep, sizeof(rGenReply), stkName)=='K'){
        if(rep->result == 0){
            memcpy(tagID, rep->physicalID, strlen(rep->physicalID));
        } else {
            tagID[0] == NULL;
        }
    } else {
        sprintf(errmsg, "ERROR: STK[%s] rLogicalToPhysicalUnit ridian send and recv fail.", stkName);
        return false;
    }
    
    sprintf(errmsg,"INFO : STK[%s] rLogicalToPhysicalUnit end  RESULT[%d], TAGID[%s], LOGICALID[%s]", 
                                            stkName, rep->result, rep->physicalID, rep->logicalName);
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: stk_rListUnitAtIrt                                      */
/* 2. Description  : STK Teltag read 요청 처리                               */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   int   ridsock   - ridian server 소켓 디스크립터         */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   int stkType     - STK type                              */
/*                   PortTable *pt   - STK PORT 정보                         */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rListUnitAtIrt(int csock, int *ridsock, char *recvBuf, char *stkName, int stkType, char *errmsg)
{
    char tag, bcr, useTag;                  /* TelTag, Barcode가 붙어있는지 확인하는 flag 변수*/
    int  bcrResult,ridResult;               /* 고정식 BCR과 통신 결과 변수 */
    char bcrIP[20]={0,};                    /* 고정식 BCR IP 담을 변수 */
    char lotID[24]={0,};                    /* LOT ID 담을 변수 */
    char barcodeID[12]={0,};                /* Barcode ID를 담을 변수 */
    char irtName[30]={0,};                  /* IRT Name을 담을 변수 */
    char irtID[12]={0,};                    /* IRT ID를 담을 변수 */
    char teltag[12]={0,};                   /* TelTag를 담을 변수 */
    char portType[2]={0,};                  /* Port Type을 담을 변수 */
    char tmpTagID[12]={0,};                 /* Temp Tag ID */
	char cstName[31]={0,};                    /* LOT ID 담을 변수 */

	int i_Ret = 0;

    unsigned short int reqLen, repLen;      /* send 구조체 길이, recv 구조체 길이 */

    tag = 'N';                              /* TelTag는 안쓴다고 초기화시킨다. */
    
    rQuerySensorRequest treq, *req;
    rQuerySensorReply trep, *rep;
    rep = &trep;
    req = & treq;
    
    memset(req, 0x00, sizeof(rQuerySensorRequest));
    memset(rep, 0x00, sizeof(rQuerySensorReply));
    memcpy(req, recvBuf, sizeof(rQuerySensorRequest));
        
    reqLen = sizeof(rQuerySensorRequest);
    repLen = sizeof(rQuerySensorReply);  
        
    sprintf(errmsg,"INFO : STK[%s] rListUnitAtIrt start  PORTID[%s]", stkName, req->nameList.name);
    logMessage(INFO, errmsg);
    
    stk_rListUnitAtIrt_hton(req);
    if(stk_RecvLogSvr((void*)req ,req->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rListUnitAtIrt recv log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }

    strcpy(irtName,req->nameList.name);

    if(GetBcrIPByIrt(irtName, irtID, portType, bcrIP, errmsg) < 0){
        logMessage(ERROR, errmsg);
        stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
        return true;
    }
    if(stkType == LOTPODTYPE){
        if(gLotParallelMaint == 1){
            ridResult = stk_rLogicalToPhysicalSensor(ridsock, irtName, stkName, errmsg);
            if(ridResult == false){
                logMessage(ERROR, errmsg);
                stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                return false;
            } else if(ridResult == 1){
                logMessage(ERROR, errmsg);
                return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
            }
        }
    } else {
        if(gReticleParallelMaint == 1){
            ridResult = stk_rLogicalToPhysicalSensor(ridsock, irtName, stkName, errmsg);
            if(ridResult == false){
                logMessage(ERROR, errmsg);
                stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                return false;
            } else if(ridResult == 1){
                logMessage(ERROR, errmsg);
                return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
            }
        }
    }
    
    /* 바코드 리딩 시도 */
    bcrResult = bcr_SendRecv(bcrIP, barcodeID, stkName, errmsg);

	/*ASML 의 Type Pod 의 경우 Reticle ID 가 반대로 들어감 */
	if ( 0 == memcmp (stkName, "CRST", 4) &&  barcodeID[0] == 'R' )
	{
		i_Ret = getCstName (barcodeID, cstName, errmsg);

        //요청번호 : CSD240724000033
        //구현 일자 및 담당자 : 2024/07/31 강규 수석
        //구현 내용 : ASML 판정 기준 변경
        //현재 : Pod Name 에 ASML 포함일 경우
        //변경 : Pod Name 에 ASML 포함일 경우 + Barcode 앞 2자가 RY / RW 일 경우
		if ( 0 == memcmp (cstName, "ASML",4) || 0 == memcmp (barcodeID, "RY",2) || 0 == memcmp (barcodeID, "RW",2))
		{
            sprintf(errmsg, "ASML INTERLOCK STK[%s] CST_ID[%s] CST_NAME[%s]", stkName, barcodeID, cstName);
            logMessage(ERROR, errmsg);
            stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
            return true;
		}
	}

    if(bcrResult == 0){
        bcr = 'Y';

        if ( 0 == memcmp (stkName, "CPST", 4))
        {
            if(GetLogicalIDByBcrIDEmpty(barcodeID, lotID, errmsg) == false){
                logMessage(ERROR, errmsg);
                return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
            }
        }
        else
        {
            if(GetLogicalIDByBcrID(barcodeID, lotID, errmsg) == false){
                logMessage(ERROR, errmsg);
                return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
            }
        }
    } else {
        if(stkType == LOTPODTYPE){
            if(gLotParallelMaint == 0){
                sprintf(errmsg, "ERROR: STK[%s] BCRIP[%s] read fail", stkName, bcrIP);
                logMessage(ERROR, errmsg);
                stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                return true;
            }
        } else {
            sprintf(errmsg, "ERROR: STK[%s] BCRIP[%s] read fail", stkName, bcrIP);
            logMessage(ERROR, errmsg);
            return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
        }
        bcr = 'N';
    }
    ridResult = rid_SendRecv(ridsock, (char*)req, reqLen, (char*)rep, repLen, stkName);
    if(ridResult == 'K'){
        if(rep->result != 0){
            stk_rListUnitAtIrt_ntoh(rep);
            if(write(csock, rep, sizeof(rQuerySensorReply)) <= 0){
                sprintf(errmsg,"ERROR: STK[%s] disconnet network",stkName);
        	    logMessage(ERROR, errmsg);
        	    return false;
        	}

        	stk_rListUnitAtIrt_ntoh(rep);
        	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
                sprintf(errmsg,"ERROR: STK[%s] rListUnitAtIrt send log transfer fail", stkName);
                logMessage(ERROR, errmsg);
            }
            
            sprintf(errmsg,"INFO : STK[%s] rListUnitAtIrt end  RESULT[%d], TAGID[%s], LOGICALID[%s]", 
                             stkName, rep->result,teltag, lotID);
            logMessage(INFO, errmsg);
            return true;
        } else {
            if(rep->responseMsg.unitID[0] == NULL){
            } else if(rep->responseMsg.unitID[0] >=48 && rep->responseMsg.unitID[0] <=57){
            } else if(rep->responseMsg.unitID[0] >=65 && rep->responseMsg.unitID[0] <=90){
            } else if(rep->responseMsg.unitID[0] >=97 && rep->responseMsg.unitID[0] <=122){
            } else {
                sprintf(errmsg,"ERROR: STK[%s] ridian TAGID reading fail unknown tagID", stkName);
                logMessage(ERROR, errmsg);
                return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
            }
            if(gReticleParallelMaint == 1 && stkType != LOTPODTYPE){
                if(strlen(rep->responseMsg.unitID) == 0 || rep->responseMsg.unitID[0]==' '){
                    sprintf(errmsg,"ERROR: STK[%s] ridian TAGID reading fail", stkName);
                    logMessage(ERROR, errmsg);
                    return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                }
            } else if(gLotParallelMaint == 1 && stkType == LOTPODTYPE){
                if(strlen(rep->responseMsg.unitID) == 0 || rep->responseMsg.unitID[0]==' '){
                    sprintf(errmsg,"ERROR: STK[%s] ridian TAGID reading fail", stkName);
                    logMessage(ERROR, errmsg);
                    return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                }
            }
        }

        if(strlen(rep->responseMsg.unitID) == 0 || rep->responseMsg.unitID[0]==' '){
            tag = 'N';
        } else {
            tag = 'Y';
            /* Tag ID 복사 */
            memcpy(teltag, rep->responseMsg.unitID, strlen(rep->responseMsg.unitID));
            /* 바코드가 있을 경우 */
            if(bcr == 'Y'){
                if(gReticleParallelMaint == 1 && stkType != LOTPODTYPE){
                    if(GetTagIDByBcrID(barcodeID, tmpTagID, errmsg) == false){
                        if(InsertBcrTagMapping(barcodeID, teltag, stkName, errmsg) == false){
                            logMessage(ERROR, errmsg);
                            return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                        }
                    } else {
                        if(strcmp(teltag, tmpTagID) != 0){
                            sprintf(errmsg, "ERROR: STK[%s] LTSBCRTAG table wrong info CSTID[%s] DBTAG[%s] READTAG[%s]",
                                                                        stkName, barcodeID, tmpTagID,teltag);
                            logMessage(ERROR, errmsg);
                            return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                        }
                    }
                }
                if(strlen(rep->responseMsg.unitName) == 0 && strlen(lotID) > 0){
                    /* 읽은 Teltag와 bcr의 정보가 틀릴때 루틴 */
                    if(lts_rDisassociateUnit(stkType,stkName,barcodeID,lotID,irtName,errmsg) == false){
                        logMessage(ERROR, errmsg);
                        return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                    }
                    memset(lotID, 0x00, sizeof(lotID));
                } else if(strlen(rep->responseMsg.unitName) > 0 && strlen(lotID) == 0){
                    if(lts_rAssociateUnit(stkType,stkName,barcodeID,rep->responseMsg.unitName,irtName,errmsg) == false){
                        logMessage(ERROR, errmsg);
                        return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                    }
                    memset(lotID, 0x00, sizeof(lotID));
                    memcpy(lotID, rep->responseMsg.unitName, strlen(rep->responseMsg.unitName));
                } else if(strlen(rep->responseMsg.unitName) > 0 && strlen(lotID) > 0){
                    if(memcmp(lotID,rep->responseMsg.unitName,strlen(lotID)) != 0){
                        if(lts_rDisassociateUnit(stkType,stkName,barcodeID,lotID,irtName,errmsg) == false){
                            logMessage(ERROR, errmsg);
                            return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                        }                     
                        if(lts_rAssociateUnit(stkType,stkName,barcodeID,rep->responseMsg.unitName,irtName,errmsg) == false){
                            logMessage(ERROR, errmsg);
                            return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                        }
                        memset(lotID, 0x00, sizeof(lotID));
                        memcpy(lotID, rep->responseMsg.unitName, strlen(rep->responseMsg.unitName));
                    }
                }
    	        memcpy(lotID, rep->responseMsg.unitName, strlen(rep->responseMsg.unitName));
    	    } else {
    	        /* 바코드가 없을 경우 tag로 읽은 unitName 복사 */
    	        memcpy(lotID, rep->responseMsg.unitName, strlen(rep->responseMsg.unitName));
    	    }
	    }
	} else {
	    memset(rep, 0x00, sizeof(rQuerySensorReply));
	    sprintf(errmsg,"ERROR: STK[%s] ridian msg send and recv fail",stkName);
	    logMessage(ERROR, errmsg);
	    stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
	    return false;
	}
    
    if(tag == 'Y' && bcr == 'Y'){
        useTag = 'I';
    } else if(tag == 'N' && bcr == 'Y'){
        useTag = 'B';
    } else if(tag == 'Y' && bcr == 'N'){
        useTag = 'T';
    } else {
        sprintf(errmsg,"ERROR: STK[%s] must use teltag or barcode",stkName);
    	logMessage(ERROR, errmsg);
        return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
    }
    
    sprintf(errmsg,"INFO : STK[%s] PORTID[%s], USETAG[%c], TAGID[%s], CSTID[%s], LOGICALID[%s], PORTTYPE[%s]", 
                           stkName,irtName,useTag,teltag, barcodeID, lotID, portType);
    logMessage(INFO, errmsg);
    
    if(useTag == 'B'){
        time_t ctime    = time(&ctime);
        memset(rep,0x00,sizeof(rQuerySensorReply));
        rep->msgLen     = sizeof(rQuerySensorReply);
        rep->msgType	= msgTypeQuerySensorLoc;
        rep->lastFlag   = 1;
        rep->result	    = 0;
        rep->totalNum	= 1;
        rep->numItems	= 1;
        memcpy(rep->responseMsg.unitID,barcodeID,strlen(barcodeID));
    	memcpy(rep->responseMsg.unitName,lotID,strlen(lotID));
    	memcpy(rep->responseMsg.sensorID,irtID,strlen(irtID));
    	memcpy(rep->responseMsg.sensorName,irtName,strlen(irtName));
    	rep->responseMsg.unittype		= 1;
    	rep->responseMsg.updateTime	    = ctime;
    	rep->responseMsg.moveTime		= ctime;
    	rep->responseMsg.motionTime	    = ctime;
    	rep->responseMsg.unitCategory	= 1;
    	rep->responseMsg.sensorCategory = 1;
    	rep->responseMsg.unitTransit	= 0;
    }
    
    if(useTag != 'T'){
        memcpy(rep->responseMsg.unitID, barcodeID, strlen(barcodeID)+1);
    }
    
    if(useTag != 'T'){
        /* LOT이나 RETICLE POD type Stocker 이면 */
        if(portType[0] == 'I'){
            if(strlen(lotID) == 0 || lotID[0] == NULL || lotID[0] == ' '){
                sprintf(errmsg,"ERROR: STK[%s] must logicalID connected CSTID[%s]",stkName, barcodeID);
            	logMessage(ERROR, errmsg);
                return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
            }
            sprintf(errmsg, "INFO : STK[%s] podtype LOGICALID[%s] input start", stkName, lotID);
            logMessage(INFO, errmsg);
            if(lts_inputRequest(stkType, stkName, barcodeID, lotID, irtName, errmsg) == false){
                if(useTag == 'B'){
                    return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                }
            }
            sprintf(errmsg, "INFO : STK[%s] podtype LOGICALID[%s] input end", stkName, lotID);
            logMessage(INFO, errmsg);
        } else if(portType[0] == 'O'){
            if(strlen(lotID) == 0 || lotID[0] == NULL || lotID[0] == ' '){
                sprintf(errmsg,"ERROR: STK[%s] must logicalID connected CSTID[%s]",stkName, barcodeID);
            	logMessage(ERROR, errmsg);
                return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
            }
            sprintf(errmsg, "INFO : STK[%s] podtype LOGICALID[%s] output start", stkName, lotID);
            logMessage(INFO, errmsg);  
            if(lts_outputRequest(stkType, stkName, barcodeID, lotID, irtName, errmsg) == false){
                if(useTag=='B'){
                    return stk_rListUnitAtIrtErrReply(csock, recvBuf, stkName, stkType, errmsg);
                }
            }
            sprintf(errmsg, "INFO : STK[%s] podtype LOGICALID[%s], output end", stkName, lotID);
            logMessage(INFO, errmsg);        
        }
    }
    
    stk_rListUnitAtIrt_ntoh(rep);
    if(write(csock, rep, sizeof(rQuerySensorReply)) <= 0){
        sprintf(errmsg,"ERROR: STK[%s] disconnet network",stkName);
	    logMessage(ERROR, errmsg);
	    return false;
	}
	
	stk_rListUnitAtIrt_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
	    sprintf(errmsg, "ERROR: STK[%s] rListUnitAtIrt send log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    sprintf(errmsg,"INFO : STK[%s] rListUnitAtIrt end  RESULT[%d], CSTID[%s], LOGICALID[%s]", 
                     stkName, rep->result,rep->responseMsg.unitID, rep->responseMsg.unitName);
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: stk_rListUnitAtIrtErrReply                              */
/* 2. Description  : STK Teltag read 요청 에러 처리                          */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   int stkType     - STK type                              */
/*                   PortTable *pt   - STK PORT 정보                         */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rListUnitAtIrtErrReply(int csock, char *recvBuf, char *stkName, int stkType, char *errmsg)
{
    rQuerySensorReply trep, *rep;
    rep = &trep;
    memset(rep, 0x00, sizeof(rQuerySensorReply));
    sprintf(errmsg,"ERROR: STK[%s] rListUnitAtIrt error reply start ", stkName);
    logMessage(ERROR, errmsg);
    time_t ctime    = time(&ctime);
    memset(rep,0x00,sizeof(rQuerySensorReply));
    rep->msgLen     = sizeof(rQuerySensorReply);
    rep->msgType	= msgTypeQuerySensorLoc;
    rep->lastFlag   = 1;
    rep->result	    = 11;
    rep->totalNum	= 0;
    rep->numItems	= 0; 
	rep->responseMsg.unittype		= 1;
	rep->responseMsg.updateTime	    = ctime;
	rep->responseMsg.moveTime		= ctime;
	rep->responseMsg.motionTime	    = ctime;
	rep->responseMsg.unitCategory	= 1;
	rep->responseMsg.sensorCategory = 1;
	rep->responseMsg.unitTransit	= 0;
	
	stk_rListUnitAtIrt_ntoh(rep);
    if(write(csock, rep, sizeof(rQuerySensorReply)) <= 0){
        sprintf(errmsg,"ERROR: STK[%s] disconnet network...",stkName);
	    logMessage(ERROR, errmsg);
	    return false;
	}
	
	stk_rListUnitAtIrt_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
	    sprintf(errmsg,"ERROR: STK[%s] rListUnitAtIrt send log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    sprintf(errmsg,"INFO : STK[%s] rListUnitAtIrt end  RESULT[%d], CSTID[%s], LOGICALID[%s]", 
                     stkName, rep->result,rep->responseMsg.unitID, rep->responseMsg.unitName);
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: stk_rReadMemory                                         */
/* 2. Description  : STK Logical info 요청                                   */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   int stkType     - STK type                              */
/*                   char* lotInfo   - LOT 공정 정보                         */
/*                   PortTable *pt   - STK PORT 정보                         */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rReadMemory(int csock, char *recvBuf, char *stkName, int stkType, char *lotInfo, char *errmsg)
{
    unsigned int address=0x00;
    char readData[17]={0,};
    rReadRAMRequest treq, *req;
    req = &treq;
    memset(req, 0x00, sizeof(rReadRAMRequest));
    memcpy(req, recvBuf, sizeof(rReadRAMRequest));              
    address = bigToLittl(req->addr);
    
    sprintf(errmsg,"INFO : STK[%s] rReadMemory start LOGICALID[%s], ADDR=[0x%X]",
                                            stkName, req->unitName, address);
    logMessage(INFO, errmsg);
    
    stk_rReadMemory_hton(req);
    
    if(stk_RecvLogSvr((void*)req ,req->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rReadMemory recv log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    rReadRAMReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rReadRAMReply));
    rep->msgLen     = bigToLitts(sizeof(rReadRAMReply));
	rep->msgType    = msgTypeReadMemory;
	rep->result	    = bigToLitts(0);
	
	if(stkType == LOTPODTYPE){
    	if(address == 0x400){
    	    memset(lotInfo, 0x00, 32*6);
	        if(GetLotInfo(req->unitName, lotInfo, errmsg) == false){
	            rep->result = bigToLitts(11);
	        }                    	    
	    }
    	if(address <= 0x490){
    	    memcpy(rep->data,&lotInfo[address-0x400], READLEN);
    	    if(address == 0x490){
    	        memset(lotInfo, 0x00, 32*6);
    	    }
    	} else {
    	    memcpy(rep->data,"                ", READLEN);
    	}
	} else {
	    if(address == 0x400){
	        sprintf(readData,"%-16s",req->unitName);
	        memcpy(rep->data, readData, READLEN);
	    } else {
	        memcpy(rep->data,"                ", READLEN);
	    }
	}
	rep->addr = bigToLittl(address);
	
	if(write(csock, rep, sizeof(rReadRAMReply)) <= 0){
	    sprintf(errmsg,"ERROR: STK[%s] disconnet network...",stkName);
	    logMessage(ERROR, errmsg);
        return false;
	}
	stk_rReadMemory_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rReadMemory send log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    memcpy(readData, rep->data, 16);
    sprintf(errmsg,"INFO : STK[%s] rReadMemory end  RESULT[%d] READ[%s]", stkName, rep->result,readData);
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: stk_rReadMemoryErrReply                                 */
/* 2. Description  : STK Logical info 요청 에러 처리                         */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   int stkType     - STK type                              */
/*                   char* lotInfo   - LOT 공정 정보                         */
/*                   PortTable *pt   - STK PORT 정보                         */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rReadMemoryErrReply(int csock, char *recvBuf, char *stkName, int stkType, char *lotInfo, char *errmsg)
{
    sprintf(errmsg,"ERROR: STK[%s] rReadMemory error reply start ", stkName);
    logMessage(ERROR, errmsg);
    
    rReadRAMReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rReadRAMReply));
    rep->msgLen     = bigToLitts(sizeof(rReadRAMReply));
	rep->msgType    = msgTypeReadMemory;
	rep->result	    = bigToLitts(11);
		
	if(write(csock, rep, sizeof(rReadRAMReply)) <= 0){
	    sprintf(errmsg,"ERROR: STK[%s] disconnet network...",stkName);
	    logMessage(ERROR, errmsg);
        return false;
	}
	stk_rReadMemory_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
	    sprintf(errmsg,"ERROR: STK[%s] rReadMemory send log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    sprintf(errmsg,"INFO : STK[%s] rReadMemory end  RESULT[%d]", stkName, rep->result);
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: stk_rAssociateUnit                                      */
/* 2. Description  : STK Logical connect 요청 처리                           */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   int  *ridsock   - ridian server 소켓 디스크립터         */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   int stkType     - STK type                              */
/*                   PortTable *pt   - STK PORT 정보                         */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/                                         
int stk_rAssociateUnit(int csock, int *ridsock, char *recvBuf, char *stkName, int stkType, char *errmsg)
{
    char bcrID[12]={0,};
    char tagID[12]={0,};
    unsigned short int reqLen;
    char lts_sendBuf[BUFSIZ]={0,};
    char lts_recvBuf[BUFSIZ]={0,};
    char msg[200]={0,};
    char receiver[200]={0,};
    char tmpLogicalID[24]={0,};
    
    rGenRequest treq, *req;
    req = &treq;
    memset(req, 0x00, sizeof(rGenRequest));
    memcpy(req, recvBuf, sizeof(rGenRequest));
    reqLen = sizeof(rGenRequest);
    
    rGenReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rGenReply));
    
    sprintf(errmsg,"INFO : STK[%s] rAssociateUnit start CSTID[%s], LOGICALID[%s]", stkName, req->physicalID, req->logicalName);
    logMessage(INFO, errmsg);
    
    stk_rAssociateUnit_hton(req);
    if(stk_RecvLogSvr((void*)req ,req->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rAssociateUnit recv log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    if(gReticleParallelMaint == 1 && stkType == RETICLEBARETYPE){
        if(GetTagIDByBcrID(req->physicalID, tagID, errmsg) == false){
            logMessage(ERROR, errmsg);
            sprintf(msg, "Connect error^BCR TAG not mapping^STK[%s]^BCR[%s]^RET[%s]", 
                                    stkName, req->physicalID, req->logicalName);
            if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                logMessage(ERROR, errmsg);
            }
            return stk_rAssociateUnitErrReply(csock, recvBuf, stkName, stkType, errmsg);
        }
        
        memcpy(bcrID, req->physicalID, strlen(req->physicalID));
        memset(req->physicalID, 0x00, sizeof(req->physicalID));
        memcpy(req->physicalID, tagID, strlen(tagID));
    } else {
        memcpy(bcrID, req->physicalID, strlen(req->physicalID));
    }
    // ZZZ-UNKNOWN 스토커가 못읽었을 경우 처리 //
    if(stkType == RETICLEBARETYPE){
        if(memcmp(req->logicalName, UNKNOWN, strlen(UNKNOWN)) == 0){
        /*
            sprintf(errmsg,"INFO : STK[%s] vision system fail connect sequence start CSTID[%s]",stkName, bcrID);
            logMessage(INFO, errmsg);
            if(GetCurrentHistoryByBcrID(bcrID, stkName, tmpLogicalID, errmsg) == false){
                logMessage(ERROR, errmsg);
                sprintf(errmsg,"ERROR: STK[%s] vision system fail connect sequence end(error) CSTID[%s]",stkName, bcrID);
                logMessage(ERROR, errmsg);
                return stk_rAssociateUnitErrReply(csock, recvBuf, stkName, stkType, errmsg);
            } else {
                if(strlen(tmpLogicalID) != 0 && tmpLogicalID[0] != NULL){
                    memset(req->logicalName, 0x00, sizeof(req->logicalName));
                    memcpy(req->logicalName, tmpLogicalID, strlen(tmpLogicalID));
                    sprintf(errmsg, "INFO : STK[%s] vision system fail connect sequence end CSTID[%s] LOGICALID[%s]",stkName, bcrID,req->logicalName);
                    logMessage(INFO, errmsg);
                } else {
                    logMessage(ERROR, errmsg);
                    sprintf(errmsg,"ERROR: STK[%s] vision system fail connect sequence end(error) CSTID[%s]",stkName, bcrID);
                    logMessage(ERROR, errmsg);
                    return stk_rAssociateUnitErrReply(csock, recvBuf, stkName, stkType, errmsg);
                }
            }
        */
        }
    }
    
    if(lts_rAssociateUnit(stkType,stkName,bcrID,req->logicalName,stkName,errmsg) == false){
        logMessage(ERROR, errmsg);
        sprintf(msg, "Connect error^STK[%s]^BCR[%s]^RET[%s]", 
                                    stkName, req->physicalID, req->logicalName);
        if(hht_sendErrMsg(stkName, msg, errmsg) == false){
            logMessage(ERROR, errmsg);
        }
        return stk_rAssociateUnitErrReply(csock, recvBuf, stkName, stkType, errmsg);
    }
    
    if(rid_SendRecv(ridsock, (char*)req, reqLen, (char*)rep, sizeof(rGenReply), stkName)=='K'){
        /* Ridian 결과값이 error일경우 처리 */
        if(rep->result != 0){
            if(lts_rDisassociateUnit(stkType,stkName,bcrID,req->logicalName,stkName,errmsg) == false){
                logMessage(ERROR, errmsg);
                sprintf(msg, "Connect error^STK[%s]^BCR[%s]^RET[%s]", 
                                    stkName, req->physicalID, req->logicalName);
                if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                    logMessage(ERROR, errmsg);
                }
                return stk_rAssociateUnitErrReply(csock, recvBuf, stkName, stkType, errmsg);
            }
            stk_rAssociateUnit_ntoh(rep);
            if(write(csock, rep, sizeof(rGenReply)) <= 0){
                sprintf(errmsg,"ERROR: STK[%s] disconnet network",stkName);
        	    logMessage(ERROR, errmsg);
        	    sprintf(msg, "Network error^STK[%s]",stkName);
                if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                    logMessage(ERROR, errmsg);
                }
        	    return false;
        	}
            
            sprintf(msg, "Connect error^STK[%s]^TAG[%s]^RET[%s]",
                                                stkName, req->physicalID, req->logicalName);
            if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                logMessage(ERROR, errmsg);
            }
                
        	stk_rAssociateUnit_ntoh(rep);
        	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
                sprintf(errmsg, "ERROR: STK[%s] rAssociateUnit send log transfer fail", stkName);
                logMessage(ERROR, errmsg);
            }
            
            sprintf(errmsg,"INFO : STK[%s] rAssociateUnit end  RESULT[%d] BCRID[%s] TAGID[%s] LOGICALID[%s]", 
                                            stkName, rep->result, bcrID, tagID, rep->logicalName);
            logMessage(INFO, errmsg);
            return true;
        }
	} else {
    /* ridian과 통신에러 발생 처리 */
        memset(rep,0x00,sizeof(rGenReply));
        if(lts_rDisassociateUnit(stkType,stkName,bcrID,req->logicalName,stkName,errmsg) == false){
            logMessage(ERROR, errmsg);
            sprintf(msg, "Disconnect error^STK[%s]^BCR[%s]^RET[%s]",
                                        stkName, bcrID, req->logicalName);
            if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                logMessage(ERROR, errmsg);
            }
            stk_rAssociateUnitErrReply(csock, recvBuf, stkName, stkType, errmsg);
            return false;
        }
	    sprintf(errmsg,"ERROR: STK[%s] ridian rAssociateUnit send and recv fail",stkName);
	    logMessage(ERROR, errmsg);
	    sprintf(msg, "Connect error^STK[%s]^BCR[%s]^RET[%s]",
	                                    stkName, bcrID, req->logicalName);
        if(hht_sendErrMsg(stkName, msg, errmsg) == false){
            logMessage(ERROR, errmsg);
        }
	    stk_rAssociateUnitErrReply(csock, recvBuf, stkName, stkType, errmsg);
        return false;
	}
    
    stk_rAssociateUnit_ntoh(rep);
    if(write(csock, rep, sizeof(rGenReply)) <= 0){
        sprintf(errmsg,"ERROR: STK[%s] disconnet network",stkName);
	    logMessage(ERROR, errmsg);
	    sprintf(msg, "Network error^STK[%s]",stkName);
        if(hht_sendErrMsg(stkName, msg, errmsg) == false){
            logMessage(ERROR, errmsg);
        }
	    if(lts_rDisassociateUnit(stkType,stkName,bcrID,req->logicalName,stkName,errmsg) == false){
            logMessage(ERROR, errmsg);
            return false;
        }
	    return false;
	}
    
	stk_rAssociateUnit_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rAssociateUnit send log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    if(lts_outputRequest(stkType,stkName,bcrID,req->logicalName,stkName, errmsg) == false){
	    logMessage(ERROR, errmsg);
	    return true;
    }
    sprintf(errmsg,"INFO : STK[%s] rAssociateUnit end  RESULT[%d] CSTID[%s] LOGICALID[%s]", 
                                        stkName, rep->result, rep->physicalID, rep->logicalName);
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: stk_rAssociateUnitErrReply                              */
/* 2. Description  : STK Logical connect 요청 에러 처리                      */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   int stkType     - STK type                              */
/*                   PortTable *pt   - STK PORT 정보                         */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rAssociateUnitErrReply(int csock, char *recvBuf, char *stkName, int stkType, char *errmsg)
{
    sprintf(errmsg,"ERROR: STK[%s] rAssociateUnit error reply start ", stkName);
    logMessage(ERROR, errmsg);
    
    rGenReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rGenReply));
    
    rep->msgLen	    = sizeof(rGenReply);
	rep->msgType	= msgTypeAssociateUnit;
	rep->result	    = 11;
	
	stk_rAssociateUnit_ntoh(rep);
    if(write(csock, rep, sizeof(rGenReply)) <= 0){
        sprintf(errmsg,"ERROR: STK[%s] disconnet network",stkName);
	    logMessage(ERROR, errmsg);
	    return false;
	}
    
	stk_rAssociateUnit_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rAssociateUnit send log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    sprintf(errmsg,"INFO : STK[%s] rAssociateUnit error reply end  RESULT[%d]", stkName, rep->result);
    logMessage(INFO, errmsg);
    return true;    
}

/*****************************************************************************/
/* 1. Function Name: stk_rDisassociateUnit                                   */
/* 2. Description  : STK Logical disconnect 요청 처리                        */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   int   ridsock   - ridian server 소켓 디스크립터         */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   int stkType     - STK type                              */
/*                   PortTable *pt   - STK PORT 정보                         */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/ 
int stk_rDisassociateUnit(int csock, int *ridsock, char *recvBuf, char *stkName, int stkType, char *errmsg)
{
    char bcrID[12]={0,};
    char tagID[12]={0,};
    unsigned short int reqLen;
    char msg[100]={0,};
    int status = false;
    
    rGenRequest treq, *req;
    req = &treq;
    memset(req, 0x00, sizeof(rGenRequest));
    memcpy(req, recvBuf, sizeof(rGenRequest));
    reqLen = sizeof(rGenRequest);
    
    rGenReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rGenReply));
    
    sprintf(errmsg,"INFO : STK[%s] rDisassociateUnit start LOGICALID[%s]", 
                                                    stkName, req->logicalName);
    logMessage(INFO, errmsg);
    
    stk_rDisassociateUnit_hton(req);
    if(stk_RecvLogSvr((void*)req ,req->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rDisassociateUnit recv log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }

    if(GetBcrIDByLogicalID(req->logicalName, bcrID, errmsg) == false){
        if(gReticleParallelMaint == 1 && stkType == RETICLEBARETYPE){
            if(stk_rLogicalToPhysicalUnit(csock, ridsock, tagID, req->logicalName, stkName, errmsg) == false){
                logMessage(ERROR, errmsg);
                sprintf(msg, "Disconnect error^STK[%s]^RET[%s]",stkName, req->logicalName);
                if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                    logMessage(ERROR, errmsg);
                }
                return stk_rDisassociateUnitErrReply(csock,recvBuf,stkName,stkType,errmsg);
            }
            if(GetBcrIDByTagID(tagID, bcrID, errmsg) == false){
                logMessage(ERROR, errmsg);
                sprintf(msg, "Disconnect error^STK[%s]^RET[%s]",stkName, req->logicalName);
                if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                    logMessage(ERROR, errmsg);
                }
                return stk_rDisassociateUnitErrReply(csock,recvBuf,stkName,stkType,errmsg);
            }
        } else {
            if(memcmp(req->logicalName, "EMPTY" ,5) != 0){
                logMessage(ERROR, errmsg);
                sprintf(msg, "Disconnect error^STK[%s]^RET[%s]",stkName, req->logicalName);
                if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                    logMessage(ERROR, errmsg);
                }
                return stk_rDisassociateUnitErrReply(csock,recvBuf,stkName,stkType,errmsg);
            }
        }
    } else {
        if(bcrID[0] != NULL){
            if(gReticleParallelMaint == 1 && stkType == RETICLEBARETYPE){
                if(GetTagIDByBcrID(bcrID, tagID, errmsg) == false){
                    logMessage(ERROR, errmsg);
                    sprintf(msg, "Disconnect error^BCR TAG not mapping^STK[%s]^BCR[%s]^RET[%s]",
                                                stkName, bcrID,req->logicalName);
                    if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                        logMessage(ERROR, errmsg);
                    }
            	    return stk_rDisassociateUnitErrReply(csock,recvBuf,stkName,stkType,errmsg);
                }
            }
    
            if(lts_rDisassociateUnit(stkType,stkName,bcrID,req->logicalName,stkName,errmsg) == false){
                logMessage(ERROR, errmsg);
                sprintf(msg, "Disconnect error^STK[%s]^BCR[%s]^RET[%s]",
                                            stkName, bcrID, req->logicalName);
                if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                    logMessage(ERROR, errmsg);
                }
            	return stk_rDisassociateUnitErrReply(csock,recvBuf,stkName,stkType,errmsg);
            }
            status = true;
        }
    }
    
    if(rid_SendRecv(ridsock, (char*)req, reqLen, (char*)rep, sizeof(rGenReply), stkName)=='K'){
        if(rep->result != 0){
            if(lts_rAssociateUnit(stkType,stkName,bcrID,req->logicalName,stkName,errmsg) == false){
                logMessage(ERROR, errmsg);
                sprintf(msg, "Disconnect error^STK[%s]^BCR[%s]^RET[%s]",
                                            stkName, bcrID, req->logicalName);
                if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                    logMessage(ERROR, errmsg);
                }
                return stk_rDisassociateUnitErrReply(csock, recvBuf, stkName, stkType, errmsg);
            }
            stk_rDisassociateUnit_ntoh(rep);
            if(write(csock, rep, sizeof(rGenReply)) <= 0){
                sprintf(errmsg,"ERROR: STK[%s] disconnet network...",stkName);
        	    logMessage(ERROR, errmsg);
        	    sprintf(msg, "Network error^STK[%s]",stkName);
                if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                    logMessage(ERROR, errmsg);
                }
        	    return false;
        	}
        	sprintf(msg, "Disconnect error^STK[%s]^TAG[%s]^RET[%s]",
        	                                    stkName, tagID, req->logicalName);
            if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                logMessage(ERROR, errmsg);
            }
        	stk_rDisassociateUnit_ntoh(rep);
        	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
                sprintf(errmsg, "ERROR: STK[%s] rDisassociateUnit send log transfer fail", stkName);
                logMessage(ERROR, errmsg);
            }
            
            sprintf(errmsg,"ERROR: STK[%s] rDisassociateUnit end RESULT[%d] BCRID[%s] TAGID[%s] LOGICALID[%s]", 
                                               stkName, rep->result, bcrID, tagID, req->logicalName);
            logMessage(ERROR, errmsg);
            return true;
        }
	} else {
	    memset(rep,0x00,sizeof(rGenReply));
	    if(lts_rAssociateUnit(stkType,stkName,bcrID,req->logicalName,stkName,errmsg) == false){
            logMessage(ERROR, errmsg);
            sprintf(msg, "Disconnect error^STK[%s]^BCR[%s]^RET[%s]",
                                                    stkName, bcrID, req->logicalName);
            if(hht_sendErrMsg(stkName, msg, errmsg) == false){
                logMessage(ERROR, errmsg);
            }
            stk_rDisassociateUnitErrReply(csock, recvBuf, stkName, stkType, errmsg);
            return false;
        }
	    sprintf(errmsg, "ERROR: STK[%s] ridian rDisassociateUnit send and recv fail", stkName);
	    logMessage(ERROR, errmsg);
	    sprintf(msg, "Disconnect error^STK[%s]^TAG[%s]^RET[%s]",stkName, tagID, req->logicalName);
        if(hht_sendErrMsg(stkName, msg, errmsg) == false){
            logMessage(ERROR, errmsg);
        }
        stk_rDisassociateUnitErrReply(csock,recvBuf,stkName,stkType,errmsg);
	    return false;
    }
    
    stk_rDisassociateUnit_ntoh(rep);
    if(write(csock, rep, sizeof(rGenReply)) <= 0){
        sprintf(errmsg,"ERROR: STK[%s] disconnet network",stkName);
	    logMessage(ERROR, errmsg);
	    sprintf(msg, "Network error^STK[%s]",stkName);
        if(hht_sendErrMsg(stkName, msg, errmsg) == false){
            logMessage(ERROR, errmsg);
        }
	    return false;
	}
	
	stk_rDisassociateUnit_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rDisassociateUnit send log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    if(memcmp(req->logicalName, "EMPTY", 5) != 0 && status == true){
        if(lts_inputRequest(stkType, stkName, bcrID, req->logicalName, stkName, errmsg) == false){
            logMessage(ERROR, errmsg);
            return true;
        }
    }
    sprintf(errmsg,"INFO : STK[%s] rDisassociateUnit end RESULT[%d] BCRID[%s] TAGID[%s] LOGICALID[%s]", 
                                    stkName, rep->result, bcrID, tagID, req->logicalName);
    logMessage(INFO, errmsg);
    
    return true;
}

/*****************************************************************************/
/* 1. Function Name: stk_rDisassociateUnitErrReply                           */
/* 2. Description  : STK Logical disconnect 요청 에러 처리                   */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   int stkType     - STK type                              */
/*                   PortTable *pt   - STK PORT 정보                         */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/ 
int stk_rDisassociateUnitErrReply(int csock, char *recvBuf, char *stkName, int stkType, char *errmsg)
{
    sprintf(errmsg,"ERROR: STK[%s] stocker rDisassociateUnit error reply start ", stkName);
    logMessage(ERROR, errmsg);
    
    rGenReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rGenReply));
    
    rep->msgLen	    = sizeof(rGenReply);
	rep->msgType	= msgTypeDisassociateUnit;
	rep->result	    = 11;
	
	stk_rDisassociateUnit_ntoh(rep);
    if(write(csock, rep, sizeof(rGenReply)) <= 0){
        sprintf(errmsg,"INFO : STK[%s] disconnet network...",stkName);
	    logMessage(INFO, errmsg);
	    return false;
	}
	
	stk_rDisassociateUnit_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rDisassociateUnit send log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    sprintf(errmsg,"INFO : STK[%s] rDisassociateUnit end RESULT[%d]", stkName, rep->result);
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: stk_rDisplayMsg                                         */
/* 2. Description  : STK Tag write 요청 처리                                 */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   int   ridsock   - ridian server 소켓 디스크립터         */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   int stkType     - STK type                              */
/*                   PortTable *pt   - STK PORT 정보                         */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/ 
int stk_rDisplayMsg(int csock, int *ridsock, char *recvBuf, char *stkName, int stkType, char *errmsg)
{
    unsigned short int reqLen;
    char location[24]={0,};
    char tagID[12]={0,};
    char bcrID[12]={0,};
    
    rPostLineRequest treq, *req;
    req = &treq;
    memset(req, 0x00, sizeof(rPostLineRequest));
    memcpy(req, recvBuf, sizeof(rPostLineRequest));
    reqLen = sizeof(rPostLineRequest);
    
    rSimpleReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rSimpleReply));
    
    sprintf(errmsg,"INFO : STK[%s] rDisplayMsg start LOGICALID[%s], MSG[%s]", stkName, req->unitName, req->msg);
    logMessage(INFO, errmsg);
    
    stk_rDisplayMsg_hton(req);
    if(stk_RecvLogSvr((void*)req ,req->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rDisplayMsg recv log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    if(gReticleParallelMaint == 1 && stkType == RETICLEBARETYPE) {
        if(stk_rLogicalToPhysicalUnit(csock, ridsock, tagID, req->unitName, stkName, errmsg) == false){
            logMessage(ERROR, errmsg);
	        stk_rDisplayMsgErrReply(csock, recvBuf, stkName, stkType, errmsg);
	        return false;
        } else {
            if(strlen(tagID) == 0 || tagID[0] == NULL){
                rep->msgLen	    = sizeof(rSimpleReply);
            	rep->msgType	= msgTypeDisplayMsg;
            	rep->numItems	= 1;
            	rep->result	    = 0;
            } else {
                if(rid_SendRecv(ridsock, (char*)req, reqLen, (char*)rep, sizeof(rSimpleReply), stkName)=='K'){
            	} else {
            	    memset(rep,0x00,sizeof(rSimpleReply));
            	    sprintf(errmsg,"ERROR: STK[%s] ridian rDisplayMsg send and recv fail",stkName);
            	    logMessage(ERROR, errmsg);
            	    stk_rDisplayMsgErrReply(csock, recvBuf, stkName, stkType, errmsg);
            	    return false;
            	}
            }
        }
    } else {
        rep->msgLen	    = sizeof(rSimpleReply);
    	rep->msgType	= msgTypeDisplayMsg;
    	rep->numItems	= 1;
    	rep->result	    = 0;
    }

	stk_rDisplayMsg_ntoh(rep);
	if(write(csock, rep, sizeof(rSimpleReply)) <= 0){
	    sprintf(errmsg,"INFO : STK[%s] disconnet network",stkName);
	    logMessage(INFO, errmsg);
	    return false;
	} 
	
	stk_rDisplayMsg_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
	    sprintf(errmsg, "ERROR: STK[%s] rDisplayMsg send log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    sprintf(errmsg,"INFO : STK[%s] rDisplayMsg end RESULT[%d] LOGICALID[%s] MSG[%s]", 
                        stkName, rep->result, req->unitName, req->msg);
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: stk_rDisplayMsgErrReply                                 */
/* 2. Description  : STK Tag write 요청 에러 처리                            */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   int stkType     - STK type                              */
/*                   PortTable *pt   - STK PORT 정보                         */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/ 
int stk_rDisplayMsgErrReply(int csock, char *recvBuf, char *stkName, int stkType, char *errmsg)
{
    rSimpleReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rSimpleReply));
    
    rep->msgLen	    = sizeof(rSimpleReply);
	rep->msgType	= msgTypeDisplayMsg;
	rep->numItems	= 1;
	rep->result	    = 11;
	
    stk_rDisplayMsg_ntoh(rep);
	if(write(csock, rep, sizeof(rSimpleReply)) <= 0){
	    sprintf(errmsg,"INFO : STK[%s] disconnet network...",stkName);
	    logMessage(INFO, errmsg);
	    return false;
	} 
	
	stk_rDisplayMsg_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rDisplayMsg send log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    sprintf(errmsg,"INFO : STK[%s] rDisplayMsg end  RESULT[%d]", stkName, rep->result);
    logMessage(INFO, errmsg);
    return true;
}




/*****************************************************************************/
/* 1. Function Name: stk_rClose                                              */
/* 2. Description  : STK connection close 요청 처리                          */
/* 3. Parameters   : int   csock     - Client 소켓 디스크립터                */
/*                   char* recvBuf   - recv 한 메세지                        */
/*                   char* stkName   - STK name                              */
/*                   char* errMsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/ 
int stk_rClose(int csock, char *recvBuf, char *stkName, char *errmsg)
{
    rSimpleRequest treq, *req;
    req = &treq;
    
    memset(req, 0x00, sizeof(rSimpleRequest));
    memcpy(req, recvBuf, sizeof(rSimpleRequest));
    sprintf(errmsg,"INFO : STK[%s] rClose start",stkName);
    logMessage(INFO, errmsg);
    
    stk_rClose_hton(req);
    if(stk_RecvLogSvr((void*)req ,req->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rClose recv log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    rSimpleReply trep, *rep; 
    rep = &trep;
    memset(rep,0x00,sizeof(rSimpleReply));
    rep->msgLen	    = bigToLitts(sizeof(rSimpleReply));
	rep->msgType	= msgTypeCloseRequest;
	rep->result	    = bigToLitts(0);
	rep->numItems	= 0;
	if(write(csock,(void*)rep, sizeof(rSimpleReply)) <= 0){
	    sprintf(errmsg,"ERROR: STK[%s] disconnet network",stkName);
	    logMessage(ERROR, errmsg);
        return false;
	}
	stk_rClose_ntoh(rep);
	if(stk_SendLogSvr((void*)rep ,rep->msgType, stkName, errmsg, true) == -1){
        sprintf(errmsg, "ERROR: STK[%s] rClose send log transfer fail", stkName);
        logMessage(ERROR, errmsg);
    }
    
    sprintf(errmsg,"INFO : STK[%s] rClose end",stkName);
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: rid_SendRecv                                            */
/* 2. Description  : ridian server 통신 모듈                                 */
/* 3. Parameters   : int   ridsock   - ridian server 소켓 디스크립터         */
/*                   char* s_buff    - ridian send 할 메세지                 */
/*                   int   s_buffLen - ridian send 할 메세지 size            */
/*                   char* r_buff    - ridian recv 할 메세지                 */
/*                   char* stkName   - STK name                              */
/* 4. Return Value : int                                                     */
/*****************************************************************************/ 
char rid_SendRecv(int *ridiansock, char* s_buff, unsigned short int s_buffLen, char* r_buff, unsigned short int r_size, char *stkName)
{
    int         s_buflen = -1;
    char        msgType = 0;
    char        msg[1024]={0,};
    char        tmpBuff[1024]={0,};
    struct      sockaddr_in ridAddr_in;
    struct      timeval waittime;
    fd_set      r_set;
    fd_set      s_set;
    fd_set      tmp_set;
    int         ret;
    int         retryCount = 0;
    int         stkType = -1;
    unsigned short int r_buffLen;
    int         tmpLen = -1;
    int         totTmpLen = 0;
    int ridsock;
    
    ridsock = *ridiansock;
    
    stkType = GetStkTypeByStkName(stkName, msg);
    if(stkType == false) {
        logMessage(ERROR, msg);
        return 'F';
    }
    
    if(stkType == LOTPODTYPE){
        if(gLotParallelMaint == 0){
            if(stk_MakeSendMsg((void *)s_buff, (void *)r_buff, (char)s_buff[TYPEBYTE], msg) == false){
                logMessage(ERROR, msg);
            }
            return 'K';
        }
    } else {
        if(gReticleParallelMaint == 0){
            if(stk_MakeSendMsg((void *)s_buff, (void *)r_buff, (char)s_buff[TYPEBYTE], msg) == false){
                logMessage(ERROR, msg);
            }
            return 'K';
        }
    }
    
    if(msgVerify((char)s_buff[TYPEBYTE]) == false){
        sprintf(msg, "ERROR: STK[%s] ridian send message type[%d]", stkName,(char)s_buff[TYPEBYTE]);
        logMessage(ERROR, msg);
        return 'F';
    }
    
    if(ridsock == -1 ){
        ridsock = rid_connect(stkName);
        if(ridsock == false){
            sprintf(msg, "ERROR: STK[%s] ridian server connect fail", stkName);
            logMessage(ERROR, msg);
            return 'F';
        }
        *ridiansock = ridsock;
    }

    while(1){
        waittime.tv_sec =  10;
        waittime.tv_usec = 0;
        FD_ZERO(&s_set);
        FD_SET(ridsock,&s_set);
        sprintf(msg,"DEBUG: STK[%s] ridian send select socket num[%d]",stkName,ridsock);
        logMessage(DEBUG, msg);
        ret = select(ridsock+1, NULL, &s_set, NULL, &waittime); 
        if(ret == -1){
            if(retryCount == RETRY){
                sprintf(msg,"ERROR: STK[%s] ridian send select func error retry count over",stkName);
                logMessage(ERROR, msg);
                return 'F';
            }
            sprintf(msg,"ERROR: STK[%s] ridian send select func RET[%d] errorno[%d]::%s",stkName,ret,errno,strerror(errno));
            logMessage(ERROR, msg);
            close(ridsock);
            ridsock = -1;
            ridsock = rid_connect(stkName);
            if(ridsock == false){
                sprintf(msg, "ERROR: STK[%s] ridian server reconnect fail", stkName);
                logMessage(ERROR, msg);
                return 'F';
            }
            *ridiansock = ridsock;
            sprintf(msg,"DEBUG: STK[%s] ridian reconnect socket num[%d]",stkName, ridsock);
            logMessage(DEBUG, msg);
            retryCount++;
            continue;
        } else if( ret == 0){
            if(retryCount == RETRY){
                sprintf(msg, "ERROR: STK[%s] ridian send time out error retry count over", stkName);
                logMessage(ERROR, msg);
                return 'F';
            }
            retryCount++;
            sprintf(msg,"ERROR: STK[%s] ridian send time out error",stkName);
            logMessage(ERROR, msg);
            continue;
        } else {
            retryCount = 0;
            break;
        }
    }
    
    while(1){
        if(FD_ISSET(ridsock, &s_set)) {
            s_buflen = write(ridsock, s_buff, s_buffLen);
            if(s_buflen == -1){
                if(retryCount == RETRY){
                    sprintf(msg, "ERROR: STK[%s] ridian server network disconnect..retry count over", stkName);
                    logMessage(ERROR, msg);
                    return 'F';
                } else {
                    close(ridsock);
                    sprintf(msg, "ERROR: STK[%s] ridian server network disconnect", stkName);
                    logMessage(ERROR, msg);
                    ridsock = rid_connect(stkName);
                    if(ridsock == false){
                        sprintf(msg, "ERROR: STK[%s] ridian server reconnect fail", stkName);
                        logMessage(ERROR, msg);
                        return 'F';
                    }
                    *ridiansock = ridsock;
                    sprintf(msg,"DEBUG: STK[%s] ridian reconnect socket num[%d]",stkName, ridsock);
                    logMessage(DEBUG, msg);
                    retryCount++;
                    continue;
                }
            } else if(s_buflen != s_buffLen){
                sprintf(msg,"ERROR: STK[%s] ridian send MSGTYPE[%d]  SENDSIZE[%d]  STRUCTSIZE[%d]",
                                                        stkName,s_buff[TYPEBYTE],s_buflen,s_buffLen);
                logMessage(ERROR, msg);
                return 'S';
            } else {
                retryCount = 0;
                break;
            }
        }
    }
    
    if(stk_RecvLogSvr((void*)s_buff ,s_buff[TYPEBYTE], stkName, msg, false) == -1){
        sprintf(msg, "ERROR: STK[%s] ridian send log transfer fail", stkName);
        logMessage(ERROR, msg);
    }
    
    while(1){
        waittime.tv_sec =  10;
        waittime.tv_usec = 0;
        
        FD_ZERO(&r_set);
        FD_SET(ridsock,&r_set);
        
        sprintf(msg,"DEBUG: STK[%s] ridian recv select socket num[%d]",stkName,ridsock);
        logMessage(DEBUG, msg);
        
        ret = select(ridsock+1, &r_set, NULL, NULL, &waittime); 
        if(ret == -1){
            if(retryCount == RETRY){
                sprintf(msg,"ERROR: STK[%s] stocker recv select func error retry count over",stkName);
                logMessage(ERROR, msg);
                return 'F';
            }
            sprintf(msg,"ERROR: STK[%s] stocker recv select func RET[%d] errorno[%d]::%s",stkName,ret,errno,strerror(errno));
            logMessage(ERROR, msg);
            close(ridsock);
            ridsock = rid_connect(stkName);
            if(ridsock == false){
                sprintf(msg,"ERROR: STK[%s] ridian reconnect fail",stkName);
                logMessage(ERROR, msg);
                return 'F';
            }
            *ridiansock = ridsock;
            sprintf(msg,"DEBUG: STK[%s] ridian reconnect socket num[%d]",stkName, ridsock);
            logMessage(DEBUG, msg);
            retryCount++;
            continue;
        } else if( ret == 0){
            if(retryCount == RETRY){
                sprintf(msg,"ERROR: STK[%s] ridian recv time out error retry count over",stkName);
                logMessage(ERROR, msg);
                return 'F';
            } else {
                sprintf(msg,"ERROR: STK[%s] ridian recv time out error",stkName);
                logMessage(ERROR, msg);
                retryCount++;
                continue;
            }
        } else {
            retryCount=0;
            break;
        }
    }
    if(FD_ISSET(ridsock, &r_set)) {
        s_buflen = read(ridsock, r_buff,HEADERSIZE);
        memcpy(&r_buffLen,r_buff,LENGTHSIZE);
        if(r_buffLen != r_size) {
            while(1){
                waittime.tv_sec =  1;
                waittime.tv_usec = 0;
                
                FD_ZERO(&tmp_set);
                FD_SET(ridsock,&tmp_set);
                ret = select(ridsock+1, &tmp_set, NULL, NULL, &waittime); 
                if(ret < 0){
                    sprintf(msg,"ERROR: STK[%s] stocker garbage recv select func error",stkName);
                    logMessage(ERROR, msg);
                    return 'F';
                } else if(ret == 0){
                    sprintf(msg,"ERROR: STK[%s] stocker garbage recv select time out",stkName);
                    logMessage(ERROR, msg);
                    continue;
                }
                if((r_buffLen-totTmpLen-HEADERSIZE) > 1024){
                    tmpLen = read(ridsock, tmpBuff, 1024);
                } else if((r_buffLen-totTmpLen-HEADERSIZE) <= 1024){
                    tmpLen = read(ridsock, tmpBuff, r_buffLen-totTmpLen-HEADERSIZE);
                }
                totTmpLen +=tmpLen;
                sprintf(msg,"DEBUG: STK[%s] stocker garbage recv size[%d]",stkName,totTmpLen+HEADERSIZE);
                logMessage(DEBUG, msg);
                if(totTmpLen >= r_buffLen-HEADERSIZE){
                    sprintf(msg,"DEBUG: STK[%s] stocker garbage recv size[%d] end",stkName,totTmpLen+HEADERSIZE);
                    logMessage(DEBUG, msg);
                    if(stk_MakeSendMsg((void *)s_buff, (void *)r_buff, (char)s_buff[TYPEBYTE], msg) == false){
                        logMessage(ERROR, msg);
                    }
                    break;
                }
            }
        } else {
            if(msgVerify((char)r_buff[TYPEBYTE]) == false){
                sprintf(msg, "ERROR: STK[%s] ridian recv message type[%d]", stkName,(char)r_buff[TYPEBYTE]);
                logMessage(ERROR, msg);
                return 'F';
            } else {
                if(s_buflen != HEADERSIZE){
                    sprintf(msg,"ERROR: STK[%s] ridian recv MSGTYPE[%d] headersize is different",stkName, r_buff[TYPEBYTE]);
                    logMessage(ERROR, msg);
                    return 'F';
                } else if(s_buff[TYPEBYTE] != r_buff[TYPEBYTE]){
                    sprintf(msg,"ERROR: STK[%s] ridian SEND MSGTYPE[%d] RECV MSGTYPE[%d] headertype is different",
                                                               stkName, s_buff[TYPEBYTE],r_buff[TYPEBYTE]);
                    logMessage(ERROR, msg);
                    return 'F';
                }
                s_buflen = read(ridsock, r_buff+HEADERSIZE,r_buffLen-HEADERSIZE);
                if(s_buflen != r_buffLen-HEADERSIZE){
                    sprintf(msg,"ERROR: STK[%s] ridian RECV MSGTYPE[%d] BODYSIZE[%d] READING SIZE[%d] is different",
                                                               stkName, r_buff[TYPEBYTE],r_buffLen,s_buflen);
                    logMessage(ERROR, msg);
                    return 'F';
                }
            }
        }
    }
    
    if(stk_SendLogSvr((void*)r_buff ,r_buff[TYPEBYTE], stkName, msg, false) == -1){
        sprintf(msg, "ERROR: STK[%s] ridian recv log transfer fail", stkName);
        logMessage(ERROR, msg);
    }
    return 'K';
}

/*****************************************************************************/
/* 1. Function Name: rid_connect                                             */
/* 2. Description  : ridian server 통신 모듈                                 */
/* 3. Parameters   : char *stkName    - stocker name                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/ 
int rid_connect(char *stkName)
{
    int ridiansock;
    int retry = 0;
    struct sockaddr_in ridAddr_in;
    char msg[200]={0,};

    memset(&ridAddr_in,0x00,sizeof(ridAddr_in));
    ridAddr_in.sin_family = AF_INET;
    ridAddr_in.sin_port = htons(RIDSERVER_PORT);
    ridAddr_in.sin_addr.s_addr = inet_addr(RIDSERVER_IP);
    
    while(1){
        retry = 0;
        if((ridiansock = socket(AF_INET,SOCK_STREAM,0)) < 0){
            sprintf(msg, "ERROR: STK[%s] ridian socket create fail..retry[%d]",stkName,retry);
            logMessage(ERROR, msg);
            if(retry >= RETRY){
                ridiansock = -1;
                return false;
            } else {
                retry++;
                continue;
            }
        }
        retry = 0;
        if(connect(ridiansock, (struct sockaddr *)&ridAddr_in, sizeof(struct sockaddr)) == -1){
            sprintf(msg, "ERROR: STK[%s] ridian connect fail..retry[%d]",stkName,retry);
            logMessage(ERROR, msg);
            if(retry >= RETRY){
                close(ridiansock);
                return false;
            } else {
                retry++;
                close(ridiansock);
                continue;
            }
        } else {
            sprintf(msg, "INFO : STK[%s] ridian socket create success socket num[%d]",stkName, ridiansock);
            logMessage(INFO, msg);
            return ridiansock;
        }
    }
}

/*****************************************************************************/
/* 1. Function Name: rid_close                                               */
/* 2. Description  : ridian server 통신 모듈                                 */
/* 3. Parameters   : int  ridsock     - ridian socket                        */
/*                   char *stkName    - stocker name                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/ 
int rid_close(int ridsock, char *stkName)
{
    rSimpleRequest treq, *req;
    req = &treq;
    memset(req, 0x00, sizeof(rSimpleRequest));    
    
    req->msgLen = sizeof(rSimpleRequest);
    req->msgType = msgTypeCloseRequest;
    
    rSimpleReply trep, *rep;
    rep = &trep;
    memset(rep,0x00,sizeof(rSimpleReply));
    
    if(rid_SendRecv(ridsock, (char*)req, sizeof(rSimpleRequest), (char*)rep, sizeof(rSimpleReply), stkName) == 'K'){
        close(ridsock);
        return true;
    } else {
        close(ridsock);
        return false;
    }
}

/*****************************************************************************/
/* 1. Function Name: stk_recv                                                */
/* 2. Description  : STK client recv 모듈                                    */
/* 3. Parameters   : int   sock      - STK client socket 디스크립터          */
/*                   char *r_buf     - STK client recv 할 buffer             */
/*                   char *stkName   - STK client name                       */
/*                   char *msg       - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/ 
int stk_recv(int sock, char* r_buf, char* stkName, char* msg)
{
    unsigned short int msgLen;
    int n_rBuf;
    int n_rBuf_tot=0;
    int ret;
    int count =0;
    
    struct timeval waittime;
    fd_set r_set;
    
    waittime.tv_sec =  60*60;
    waittime.tv_usec = 0;
    
    while(1){
        FD_ZERO(&r_set);
        FD_SET(sock,&r_set);
        ret = select(sock+1, &r_set, NULL, NULL, &waittime);
        
        if(ret == -1){
            sprintf(msg,"ERROR: STK[%s] select func error",stkName);
            logMessage(ERROR, msg);
            return -1;
        } else if( ret == 0){
            if(count > RETRY){
                sprintf(msg,"ERROR: STK[%s] recv timeout retry count over",stkName);
                logMessage(ERROR, msg);
                return 0;
            }
            sprintf(msg,"INFO : STK[%s] recv timeout",stkName);
            logMessage(INFO, msg);
            count++;
        } else if(ret > 0){
            break;
        }
    }
    if(FD_ISSET(sock, &r_set)) {
        n_rBuf = read(sock,r_buf,HEADERSIZE);
        if(n_rBuf != HEADERSIZE && n_rBuf > 0){
            sprintf(msg,"ERROR: STK[%s] recv msg header read fail",stkName);
            logMessage(ERROR, msg);
            return -1;
        } else if(n_rBuf <= 0){
            sprintf(msg, "ERROR: STK[%s] disconnect", stkName);
            logMessage(ERROR, msg);
            return -1;
        }
        if(msgVerify((char)r_buf[TYPEBYTE]) == false){
            sprintf(msg, "ERROR: STK[%s] unknown message type[%d]", stkName,r_buf[TYPEBYTE]); 
            logMessage(ERROR, msg);
            return -1;
        }
        memcpy(&msgLen, r_buf, LENGTHSIZE);
        msgLen = bigToLitts(msgLen);
        while(1){
            n_rBuf_tot += n_rBuf;
            if(msgLen > n_rBuf_tot){
                n_rBuf = read(sock,&r_buf[n_rBuf_tot], msgLen - n_rBuf_tot);
                if(n_rBuf <= 0){
                    sprintf(msg,"ERROR: STK[%s] disconnect",stkName);
                    logMessage(ERROR, msg);
                    return -1;
                }
            } else {
                return n_rBuf_tot;
            }
        }
    }
}

/*****************************************************************************/
/* 1. Function Name: bcr_connect                                             */
/* 2. Description  : STK 고정식 BCR connetion 모듈                           */
/* 3. Parameters   : char *bcrIP     - STK client 고정식 BCR IP              */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int bcr_connect(char* bcrIP)
{
    int bcrsock;
    struct sockaddr_in bcrAddr_in;
    memset(&bcrAddr_in,0x00,sizeof(bcrAddr_in));
    bcrAddr_in.sin_family = AF_INET;
    bcrAddr_in.sin_port = htons(MOD_PORT);
    bcrAddr_in.sin_addr.s_addr = inet_addr(bcrIP);
    
    if((bcrsock = socket(AF_INET,SOCK_STREAM,0)) < 0){
        return -2;
    }
    if(connect(bcrsock, (struct sockaddr *)&bcrAddr_in, sizeof(struct sockaddr)) == -1){
        close(bcrsock);
        return -1;
    } else {
        return bcrsock;
    }
}

/*****************************************************************************/
/* 1. Function Name: bcr_SendRecv                                            */
/* 2. Description  : STK client 고정식 BCR 통신 모듈                         */
/* 3. Parameters   : char *bcrIP     - STK client 고정식 BCR IP              */
/*                   char *cstID     - 고정식 BCR에서 읽은 BCR ID            */
/*                   char *stkName   - STK client name                       */
/*                   char *msg       - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int bcr_SendRecv(char* bcrIP, char* cstID, char* stkName, char* msg)
{
    int retry = 0;
    int bcrsock = -1;
    int n_rBuf;
    char s_buf[1];
    int ret;
	char temp_str[8];
	char temp_pod_id[7] = {0,};
    
    struct timeval waittime;
    fd_set s_set;
    fd_set r_set;
    
    waittime.tv_sec =  7;
    waittime.tv_usec = 0;

    while(1){
        bcrsock = bcr_connect(bcrIP);
        if(bcrsock == -1){
            retry++;
            if(retry == RETRY){
                sprintf(msg,"ERROR: STK[%s] BCRIP[%s] connect fail retry count over",stkName,bcrIP);
                logMessage(ERROR, msg);
                return -2;
            }
            sprintf(msg,"ERROR: STK[%s] BCRIP[%s] connect fail retry count[%d]", stkName,bcrIP,retry);
            logMessage(ERROR, msg);
        } else if(bcrsock == -2){
            retry++;
            if(retry == RETRY){
                sprintf(msg,"ERROR: STK[%s] BCRIP[%s] create fail too many socket resource",stkName,bcrIP);
                logMessage(ERROR, msg);
                return -2; 
            }
            sprintf(msg,"ERROR: STK[%s] BCRIP[%s] socket create fail retry count[%d]",stkName,bcrIP,retry);
            logMessage(ERROR, msg);
        } else if(bcrsock > 0){
            break;
        }
    }
    retry = 0;
    s_buf[0] = 0x05;

    while(1){
        FD_ZERO(&s_set);
        FD_SET(bcrsock,&s_set);
        ret = select(bcrsock+1, NULL, &s_set, NULL, &waittime);

        if(ret == -1){
            sprintf(msg,"ERROR: STK[%s] BCRIP[%s] send func error",stkName,bcrIP);
            logMessage(ERROR, msg);
            close(bcrsock);
            return -1;
        } else if( ret == 0){
            sprintf(msg,"ERROR: STK[%s] BCRIP[%s] send time out",stkName,bcrIP);
            logMessage(ERROR, msg);
            close(bcrsock);
            return -1;
        } else if(ret > 0){
            if(FD_ISSET(bcrsock, &s_set)){
                if(write(bcrsock, s_buf, sizeof(s_buf)) <= 0){
                    sprintf(msg,"ERROR: STK[%s] bcr reading command send fail",stkName);
                    logMessage(ERROR, msg);
                    close(bcrsock);
                    return -1;
                }
            }
        } 
        
        FD_ZERO(&r_set);
        FD_SET(bcrsock,&r_set);
        ret = select(bcrsock+1, &r_set, NULL, NULL, &waittime);
        
        if(ret == -1){
            sprintf(msg,"ERROR: STK[%s] BCRIP[%s] recv func error",stkName,bcrIP);
            logMessage(ERROR, msg);
            close(bcrsock);
            return -1;
        } else if( ret == 0){
            sprintf(msg,"ERROR: STK[%s] BCRIP[%s] recv time out",stkName,bcrIP);
            logMessage(ERROR, msg);
            close(bcrsock);
            return -1;
        } else if(ret > 0){
            if(FD_ISSET(bcrsock, &r_set)){
                n_rBuf = read(bcrsock, cstID, BCRLEN);
                if(n_rBuf <= 0){
                    sprintf(msg,"ERROR: STK[%s] BCRIP[%s] disconnect bcr",stkName,bcrIP);
                    logMessage(ERROR, msg);
                    close(bcrsock);
                    return -1;
                }
            }

			//2016.01.13 카메라 Type Bar Code Reaader 기에서 Data 가 7 자리 발생하여 Data 전환 실시
            sprintf(msg,"INFO : BCR_ID_TRANS1 STK[%s] BCRIP[%s] read CSTID[%s]",stkName,bcrIP,cstID);
            logMessage(INFO, msg);

			memset (temp_str, 0x00, sizeof (temp_str));
			memcpy (temp_str, cstID, 6);
			memset (cstID, 0x00, 12);
			memcpy (cstID, temp_str, 6);

            sprintf(msg,"INFO : BCR_ID_TRANS2 STK[%s] BCRIP[%s] read CSTID[%s]",stkName,bcrIP,cstID);
            logMessage(INFO, msg);

			if ( 0 == memcmp (stkName, "CPST", 4) && cstID[0] == 'S' )
			{
				//2019.07.29 Pod ID -> Cst ID 로 전환, 임시 Test 를 위해 CPST27 로 제한
				memcpy (temp_pod_id, cstID, 6);
				memset (cstID, 0x00, 12);
				getPodIDToCstID (temp_pod_id, cstID);

				sprintf(msg,"INFO : BCR_ID_TRANS3 STK[%s] BCRIP[%s] read CSTID[%s] PODID[%s]",stkName,bcrIP,cstID, temp_pod_id);
				logMessage(INFO, msg);

			}

			//2016.11.21 신규 Bar Code Reader 기에서 7 자리 Data 가 발생 하여 Program 수정
			//if ( 0 == memcmp (stkName, "CPST27", 6) || 0 == memcmp (stkName, "CPST15", 6) || 0 == memcmp (stkName, "CPST16", 6) || 0 == memcmp (stkName, "CPST08", 6))
			//{
   //             sprintf(msg,"INFO : BCR_ID_TRANS1 STK[%s] BCRIP[%s] read CSTID[%s]",stkName,bcrIP,cstID);
   //             logMessage(INFO, msg);

			//	memset (temp_str, 0x00, sizeof (temp_str));
			//	memcpy (temp_str, cstID, 6);
			//	memset (cstID, 0x00, 12);
			//	memcpy (cstID, temp_str, 6);

   //             sprintf(msg,"INFO : BCR_ID_TRANS2 STK[%s] BCRIP[%s] read CSTID[%s]",stkName,bcrIP,cstID);
   //             logMessage(INFO, msg);

			//}

            if(strncmp(cstID,"?",1)==0 || strlen(cstID) != 6 || cstID[0] == NULL || cstID[0] == ' '){
                retry++;
                sprintf(msg,"ERROR: STK[%s] BCRIP[%s] can't read data[%s] bcr retry count[%d]"
                                                                            ,stkName,bcrIP,cstID,retry);
                logMessage(ERROR, msg);
                if(retry == RETRY){
                    sprintf(msg,"ERROR: STK[%s] BCRIP[%s] can't read bcr",stkName,bcrIP);
                    logMessage(ERROR, msg);
                    
                    s_buf[0]=0x04;
                    FD_ZERO(&s_set);
                    FD_SET(bcrsock,&s_set);
                    ret = select(bcrsock+1, NULL, &s_set, NULL, &waittime);
            
                    if(ret == -1){
                        sprintf(msg,"ERROR: STK[%s] BCRIP[%s] send timeout command func error",stkName,bcrIP);
                        logMessage(ERROR, msg);
                        close(bcrsock);
                        return -1;
                    } else if( ret == 0){
                        sprintf(msg,"ERROR: STK[%s] BCRIP[%s] send timeout command time out",stkName,bcrIP);
                        logMessage(ERROR, msg);
                        close(bcrsock);
                        return -1;
                    } else if(ret > 0){
                        if(FD_ISSET(bcrsock, &s_set)){
                            if(write(bcrsock, s_buf, sizeof(s_buf)) <= 0){
                                sprintf(msg,"ERROR: STK[%s] bcr reading timeout command send fail",stkName);
                                logMessage(ERROR, msg);
                                close(bcrsock);
                                return -1;
                            }
                        }
                    }
                    memset(cstID, 0x00, 12);
                    close(bcrsock);
                    return -1;
                }
            } else {
                sprintf(msg,"INFO : STK[%s] BCRIP[%s] read CSTID[%s] read success",stkName,bcrIP,cstID);
                logMessage(INFO, msg);
                
                s_buf[0]=0x04;
                FD_ZERO(&s_set);
                FD_SET(bcrsock,&s_set);
                ret = select(bcrsock+1, NULL, &s_set, NULL, &waittime);
        
                if(ret == -1){
                    sprintf(msg,"ERROR: STK[%s] BCRIP[%s] send timeout command func error",stkName,bcrIP);
                    logMessage(ERROR, msg);
                    close(bcrsock);
                    return true;
                } else if( ret == 0){
                    sprintf(msg,"ERROR: STK[%s] BCRIP[%s] send timeout command time out",stkName,bcrIP);
                    logMessage(ERROR, msg);
                    close(bcrsock);
                    return true;
                } else if(ret > 0){
                    if(FD_ISSET(bcrsock, &s_set)){
                        if(write(bcrsock, s_buf, sizeof(s_buf)) <= 0){
                            sprintf(msg,"ERROR: STK[%s] bcr reading timeout command send fail",stkName);
                            logMessage(ERROR, msg);
                            close(bcrsock);
                            return true;
                        }
                    }
                }
                sprintf(msg,"DEBUG: STK[%s] BCRIP[%s] send timeout command success", stkName, bcrIP);
                close(bcrsock);
                return 0;
            }
        }
    }
    close(bcrsock);
    return 0;
} 

/*****************************************************************************/
/* 1. Function Name: hht_sendErrMsg                                          */
/* 2. Description  : Error send HHTinf                                       */
/* 3. Parameters   : char *stkName   - Stocker Name                          */
/*                   char *msg       - Transfer message                      */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int hht_sendErrMsg(char* stkName, char *msg, char *errmsg)
{
    int hhtresult;
    char receiver[200]={0,};
    
    if(getErrorReceiver(receiver, errmsg) == false){
        logMessage(ERROR, errmsg);
        return false;
    }
    
    hhtresult = hht_SendRecv(stkName, receiver, msg, errmsg);
    if(hhtresult == false){
        logMessage(ERROR, errmsg);
        return false;
    } else {
        return true;
    }
}

/*****************************************************************************/
/* 1. Function Name: getErrorReceiver                                        */
/* 2. Description  : Error receiver id file read                             */
/* 3. Parameters   : char *receiver  - receiverID                            */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int getErrorReceiver(char *receiver, char *errmsg)
{
    FILE *fp = NULL;
	char *ldenv = NULL;
	char *token = NULL;
    char *tail = NULL;
	char fconf[BUFSIZ + 1]={0,};
	char tmp[BUFSIZ + 1]={0,};
    char lp[BUFSIZ + 1]={0,};
    
    if ( (ldenv = getenv("LTSSERVER_CONF")) == NULL ) {
    	sprintf(errmsg, "ERROR: 환경변수 [%s]가 정의되지 않았습니다", "LTSSERVER_CONF");
        return false;
    }
    strcat(strcat(strcpy(fconf, ldenv), "/"), CONFIG_FILE);    
    if ( (fp = fopen(fconf, "r")) == NULL ) {
    	sprintf(errmsg, "ERROR: 환경파일 [%s]을 읽을 수 없습니다", CONFIG_FILE);
    	return false;
    }
	while (fgets(lp, BUFSIZ + 1, fp) != NULL) {
		if (*lp == '#') continue;
        trimEx(lp, tmp);
        token = strtok(lp, "=");
        if (token == NULL) {
        	sprintf(errmsg, "ERROR: 환경파일 구성이 잘못되었습니다");
        	fclose(fp);
        	return false;
		}
		else if (strcmp("STKinf.superbiser.id", token) == 0) {
			strcpy(receiver, &tmp[strlen(token) + 1]);
			fclose(fp);
	        return true;
		}
	}
	sprintf(errmsg, "ERROR: Error Msg receiver 설정이 되어있지 않습니다");
	fclose(fp);
	return false;
}

/*****************************************************************************/
/* 1. Function Name: hht_connect                                             */
/* 2. Description  : HHTinf connetion 모듈                                   */
/* 3. Parameters   :                                                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int hht_connect()
{
    int hhtsock;
    struct sockaddr_in hhtAddr_in;
    memset(&hhtAddr_in,0x00,sizeof(hhtAddr_in));
    hhtAddr_in.sin_family = AF_INET;
    hhtAddr_in.sin_port = htons(HHTINF_PORT);
    hhtAddr_in.sin_addr.s_addr = inet_addr(HHTINF_IP);
    
    if((hhtsock = socket(AF_INET,SOCK_STREAM,0)) < 0){
        return -2;
    }
    if(connect(hhtsock, (struct sockaddr *)&hhtAddr_in, sizeof(struct sockaddr)) == -1){
        close(hhtsock);
        return -1;
    } else {
        return hhtsock;
    }
}

/*****************************************************************************/
/* 1. Function Name: hht_SendRecv                                            */
/* 2. Description  : HHT inf 통신 모듈                                       */
/* 3. Parameters   : char *stkName   - STK client name                       */
/*                   char *receiver  - Msg receiver                          */
/*                   char *msg       - Transfer msg                          */
/*                   char *errmsg    - Error message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int hht_SendRecv(char* stkName, char *receiver, char *msg, char *errmsg)
{
    int hhtresult;
    int logresult;
    int hhtsock;
    int msgID=0;
    char sendBuf[BUFSIZ]={0,};
    char recvBuf[BUFSIZ]={0,};
    char sendMsgName[7]={0,};
    char recvMsgName[7]={0,};
    char tm[100]={0,};
    
    int logsock;
    if ( (logsock = connSocket_unix(logFile, errmsg) ) == false ) {
		logMessage(ERROR, errmsg);
		logsock = -1;
    }
    
    hhtsock = hht_connect();
    if(hhtsock == -1){
        sprintf(errmsg, "ERROR: STK[%s] is HHTinf connect fail", stkName);
        logMessage(ERROR, errmsg);
        return false;
    }
    
    getCurrTime("%Y-%m-%d %H-%M-%S",tm);
    
    sprintf(sendMsgName, "HMTR");
    sprintf(sendBuf, "REQ_ID=%s|REQ_USER=%s|REQ_DATE=%s|MSG_CODE=MC06|RECEIVER=%s|MSG=%s|SOURCE=%s",
                                                    stkName, stkName, tm, receiver, msg, SERVER_NAME);
    
    hhtresult = sendMessage(hhtsock, msgID, sendMsgName, sendBuf, errmsg);
    if(hhtresult < 0){
        sprintf(errmsg,"ERROR: HHTinf MSG[%-6s] send fail network error[%s]",sendMsgName, strerror(errno));
        close(hhtsock);
        if(logsock > 0){
            close(logsock);
        }
        return false;
    } 
    
    if(logsock > 0){
        logresult = sendLogMessage(logsock, SERVER_NAME, "->HHTinf", sendMsgName, sendBuf, errmsg);
    }
    if(logresult < 0){
        sprintf(errmsg,"ERROR: MSG[%-6s] [%s->HHTinf] log send fail MSGBODY:%s",sendMsgName,SERVER_NAME,sendBuf);
        logMessage(ERROR, errmsg);
    }  
    
    hhtresult = recvMessage(hhtsock, &msgID, recvMsgName, recvBuf, errmsg);
    if(hhtresult < 0){
        sprintf(errmsg,"ERROR: HHTinf MSG[%-6s] recv fail network error[%s]", recvMsgName, strerror(errno));
        close(hhtsock);
        if(logsock > 0){
            close(logsock);
        }
        return false;
    }
    if(logsock > 0){
        logresult = sendLogMessage(logsock, SERVER_NAME, "<-HHTinf", recvMsgName, recvBuf, errmsg);
    }
    if(logresult < 0){
        sprintf(errmsg,"ERROR: MSG[%-6s] [%s<-HHTinf] log recv fail MSGBODY:%s",recvMsgName,SERVER_NAME,recvBuf);
        logMessage(ERROR, errmsg);
    }
    if(logsock > 0){
        close(logsock);
    }
    close(hhtsock);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: stk_RecvLogSvr                                          */
/* 2. Description  : STKinf recv message LOGsvr write                        */
/* 3. Parameters   : void *s_buf     - LOGsvr에 보낼 log message             */
/*                   char msgType    - log message type                      */
/*                   char *stkName   - STK client name                       */
/*                   char *msg       - Error Message                         */
/*                   int  falg       - ridian server or STK client           */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_RecvLogSvr(void* s_buf ,char msgType, char* stkName, char* msg, int flag)
{
    int s_len = -1;
    char buff[BUFSIZ/2]={0,};
    unsigned short int msgLen;
    char name[30]={0,};
    char dest[10]={0,};
    int logsock;
    if ( (logsock = connSocket_unix(logFile, msg) ) == false ) {
		logMessage(ERROR, msg);
		return -1;
    }
    switch(msgType){
        case msgTypeConnectRequest :
        {
            rConnectRequest *req = (rConnectRequest *)s_buf;
            sprintf(buff,"STK_ID=%s",req->name); 
            strcpy(name,"rConnect");
            break;
        }
        case msgTypeCloseRequest :
        {
            sprintf(buff,"STK_ID=%s",stkName);
            strcpy(name,"rClose");
            break;
        }
        case msgTypePTLSensor :
        {
            rGenRequest *req = (rGenRequest *)s_buf;
            sprintf(buff,"STK_ID=%s|IRT_ID=%s",stkName,req->physicalID);
            strcpy(name,"rPhysicalToLogicalSensor");
            break;
        }
        case msgTypeLTPSensor :
        {
            rGenRequest *req = (rGenRequest *)s_buf;
            sprintf(buff,"STK_ID=%s|IRT_NAME=%s",stkName,req->logicalName);
            strcpy(name,"rLogicalToPhysicalSensor");
            break;
        }
        case msgTypeQuerySensorLoc :
        {
            rQuerySensorRequest *req = (rQuerySensorRequest *)s_buf;
            sprintf(buff,"STK_ID=%s|IRT_NAME=%s",stkName,req->nameList.name);
            strcpy(name,"rListUnitAtIrt");
            break;
        }
        case msgTypeReadMemory :
        {
            rReadRAMRequest *req = (rReadRAMRequest *)s_buf;
            sprintf(buff,"STK_ID=%s|LOT_ID=%s|ADDR=0x%X",stkName,req->unitName,req->addr);
            strcpy(name,"rReadMemory");
            break;
        }
        case msgTypeAssociateUnit :  
        {          
            rGenRequest *req = (rGenRequest *)s_buf;
            sprintf(buff,"STK_ID=%s|CST_ID=%s|LOT_ID=%s",stkName,req->physicalID,req->logicalName);
            strcpy(name,"rAssociateUnit");
            break;
        }
        case msgTypeDisassociateUnit :
        {
            rGenRequest *req = (rGenRequest *)s_buf;
            sprintf(buff,"STK_ID=%s|LOT_ID=%s",stkName,req->logicalName);
            strcpy(name,"rDisassociateUnit");
            break;
        }
        case msgTypeDisplayMsg :
        {
            rPostLineRequest *req = (rPostLineRequest *)s_buf;
            sprintf(buff,"STK_ID=%s|LOT_ID=%s|LINE=%d|MSG=%s",stkName,req->unitName,req->line, req->msg);
            strcpy(name,"rDisplayMsg");
            break;
        }
        case msgTypeLTPUnit :
        {
            rGenRequest *req = (rGenRequest *)s_buf;
            sprintf(buff,"STK_ID=%s|LOT_ID=%s",stkName,req->logicalName);
            strcpy(name,"rLogicalToPhysicalUnit");
            break;
        }
        default:
        {
            sprintf(msg, "ERROR: STK[%s] unknown message type[%d]",stkName, msgType);
		    logMessage(ERROR, msg);    
		    close(logsock);
            return -1;
        }
    }
    if(flag == true){
        sprintf(dest, "<-%6s", stkName);
    } else {
        sprintf(dest, "->RIDsvr");
    }
    if(s_len = sendLogMessage(logsock, SERVER_NAME, dest, name, buff, msg) == -1){
        logMessage(ERROR, msg);
        close(logsock);
        return -1;
    }
    close(logsock);
    return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_SendLogSvr                                          */
/* 2. Description  : STKinf send message LOGsvr write                        */
/* 3. Parameters   : void *s_buf     - LOGsvr에 보낼 log message             */
/*                   char msgType    - log message type                      */
/*                   char *stkName   - STK client name                       */
/*                   char *msg       - Error Message                         */
/*                   int  falg       - ridian server or STK client           */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_SendLogSvr(void* s_buf ,char msgType, char* stkName, char* msg, int flag)
{
    int s_len = -1;
    char buff[BUFSIZ]={0,};
    unsigned short int msgLen;
    char name[30]={0,};
    char dest[10]={0,};
    int logsock;
    if ( (logsock = connSocket_unix(logFile, msg) ) == false ) {
		logMessage(ERROR, msg);
		return -1;
    }
    switch(msgType){  
        case msgTypeConnectRequest :
        {
            rConnectReply *req = (rConnectReply *)s_buf;
            sprintf(buff,"STK_ID=%s|Major=%d|Minor=%d|Result=%d|ByteOrder=%d",
                   stkName, req->major, req->minor, req->result , req->byteOrder); 
            strcpy(name,"rConnect_R");
            break;
        }
        case msgTypeCloseRequest :
        {
            sprintf(buff,"STK_ID=%s",stkName);
            strcpy(name,"rClose_R");
            break;
        }
        case msgTypePTLSensor :
        {
            rGenReply *req = (rGenReply *)s_buf;
            sprintf(buff,"STK_ID=%s|RESULT=%d|IRT_ID=%s|IRT_NAME=%s",stkName,req->result,req->physicalID,req->logicalName);
            strcpy(name,"rPhysicalToLogicalSensor_R");
            break;
        }
        case msgTypeLTPSensor :
        {
            rGenReply *req = (rGenReply *)s_buf;
            sprintf(buff,"STK_ID=%s|RESULT=%d|IRT_ID=%s|IRT_NAME=%s",stkName,req->result,req->physicalID,req->logicalName);
            strcpy(name,"rLogicalToPhysicalSensor_R");
            break;
        }
        case msgTypeQuerySensorLoc :
        {    
            rQuerySensorReply *req = (rQuerySensorReply *)s_buf;
            sprintf(buff,"STK_ID=%s|RESULT=%d|CST_ID=%s|LOGICAL_ID=%s|IRT_ID=%s|IRT_NAME=%s",
                    stkName,req->result,req->responseMsg.unitID,req->responseMsg.unitName,
                    req->responseMsg.sensorID,req->responseMsg.sensorName);
            strcpy(name,"rListUnitAtIrt_R");
            break;
        }
        case msgTypeReadMemory :
        {
            rReadRAMReply *req = (rReadRAMReply *)s_buf;
            sprintf(buff,"STK_ID=%s|RESULT=%d|ADDR=0x%X|READ_DATA=%s",stkName,req->result,req->addr,req->data);
            strcpy(name,"rReadMemory_R");
            break;
        }
        case msgTypeAssociateUnit :
        {            
            rGenReply *req = (rGenReply *)s_buf;
            sprintf(buff,"STK_ID=%s|RESULT=%d|CST_ID=%s|RETICLE_ID=%s",stkName,req->result,req->physicalID,req->logicalName);
            strcpy(name,"rAssociateUnit_R");
            break;
        }
        case msgTypeDisassociateUnit :
        {
            rGenReply *req = (rGenReply *)s_buf;
            sprintf(buff,"STK_ID=%s|RESULT=%d|CST_ID=%s|RETICLE_ID=%s",stkName,req->result,req->physicalID,req->logicalName);
            strcpy(name,"rDisassociateUnit_R");
            break;
        }
        case msgTypeDisplayMsg :
        {
            rSimpleReply *req = (rSimpleReply *)s_buf;
            sprintf(buff,"STK_ID=%s|RESULT=%d",stkName, req->result);
            strcpy(name,"rDisplayMsg_R");
            break;
        }
        case msgTypeLTPUnit :
        {
            rGenReply *req = (rGenReply *)s_buf;
            sprintf(buff,"STK_ID=%s|RESULT=%d|CST_ID=%s|LOGICAL_ID=%s",
                                        stkName,req->result,req->physicalID,req->logicalName);
            strcpy(name,"rLogicalToPhysicalUnit_R");
            break;
        }
        default:
        {
            sprintf(msg, "ERROR: STK[%s] unknown message type[%d]", stkName, msgType);
		    logMessage(ERROR, msg);
		    close(logsock);
            return -1;
        }
    }
    if(flag == true){
        sprintf(dest, "->%6s", stkName);
    } else {
        sprintf(dest, "<-RIDsvr");
    }
    if(s_len = sendLogMessage(logsock, SERVER_NAME, dest, name, buff, msg) == -1){
        logMessage(ERROR, msg);
        close(logsock);
        return -1;
    }
    close(logsock);
    return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_MakeSendMsg                                         */
/* 2. Description  : STKinf make send message                                */
/* 3. Parameters   : void *r_buf     - recv message                          */
/*                   void *s_buf     - send message                          */
/*                   char msgType    - log message type                      */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_MakeSendMsg(void* r_buf, void* s_buf ,char msgType, char* errmsg)
{
    switch(msgType){  
        case msgTypeConnectRequest :
        {
            rConnectReply *rep = (rConnectReply *)s_buf;
            rep->msgLen	    = sizeof(rConnectReply);
        	rep->msgType	= msgTypeConnectRequest;
        	rep->result	    = 0;
        	rep->major	    = 2;
        	rep->minor	    = 5;
        	rep->byteOrder  = LITTLE_ENDIAN;
        	rep->bitOrder	= 0;
        	rep->point	    = 0;
            break;
        }
        case msgTypeCloseRequest :
        {
            rSimpleReply *rep = (rSimpleReply *)s_buf;
            rep->msgLen	    = sizeof(rSimpleReply);
        	rep->msgType	= msgTypeCloseRequest;
        	rep->result	    = 0;
        	rep->numItems	= 0;
            break;
        }
        case msgTypePTLSensor :
        {
            rGenReply *rep   = (rGenReply *)s_buf;
            rep->msgLen = sizeof(rGenReply);
            rep->msgType = msgTypePTLSensor;
            rep->result = 0;
            break;
        }
        case msgTypeLTPSensor :
        {
            rGenReply *rep = (rGenReply *)s_buf;
            rep->msgLen = sizeof(rGenReply);
            rep->msgType = msgTypeLTPSensor;
            rep->result = 0;
            break;
        }
        case msgTypeQuerySensorLoc :
        {    
            rQuerySensorReply *rep = (rQuerySensorReply *)s_buf;
            time_t ctime    = time(&ctime);
            rep->msgLen     = sizeof(rQuerySensorReply);
            rep->msgType	= msgTypeQuerySensorLoc;
            rep->lastFlag   = 1;
            rep->result	    = 0;
            rep->totalNum	= 1;
            rep->numItems	= 1; 
            memcpy(rep->responseMsg.unitID,0x00,sizeof(rep->responseMsg.unitID));
        	memcpy(rep->responseMsg.unitName,0x00,sizeof(rep->responseMsg.unitName));
        	memcpy(rep->responseMsg.sensorID,0x00,sizeof(rep->responseMsg.sensorID));
        	memcpy(rep->responseMsg.sensorName,0x00,sizeof(rep->responseMsg.sensorName));
        	rep->responseMsg.unittype		= 1;
        	rep->responseMsg.updateTime	    = ctime;
        	rep->responseMsg.moveTime		= ctime;
        	rep->responseMsg.motionTime	    = ctime;
        	rep->responseMsg.unitCategory	= 1;
        	rep->responseMsg.sensorCategory = 1;
        	rep->responseMsg.unitTransit	= 0;
            break;
        }
        case msgTypeReadMemory :
        {
            rReadRAMRequest *req = (rReadRAMRequest *)r_buf;
            rReadRAMReply *rep = (rReadRAMReply *)s_buf;
            rep->msgLen     = sizeof(rReadRAMReply);
        	rep->msgType    = msgTypeReadMemory;
        	rep->result	    = 0;
        	rep->addr       = bigToLittl(req->addr);
            break;
        }
        case msgTypeAssociateUnit :
        {            
            rGenReply *rep = (rGenReply *)s_buf;
            rep->msgLen = sizeof(rGenReply);
            rep->msgType = msgTypeAssociateUnit;
            rep->result = 0;
            break;
        }
        case msgTypeDisassociateUnit :
        {
            rGenReply *rep = (rGenReply *)s_buf;
            rep->msgLen = sizeof(rGenReply);
            rep->msgType = msgTypeDisassociateUnit;
            rep->result = 0;
            break;
        }
        case msgTypeDisplayMsg :
        {
            rSimpleReply *rep = (rSimpleReply *)s_buf;
            rep->msgLen	    = sizeof(rSimpleReply);
        	rep->msgType	= msgTypeDisplayMsg;
        	rep->result	    = 0;
        	rep->numItems	= 0;
            break;
        }
        case msgTypeLTPUnit :
        {
            rGenReply *rep = (rGenReply *)s_buf;
            rep->msgLen = sizeof(rGenReply);
            rep->msgType = msgTypeLTPUnit;
            rep->result = 0;
            break;
        }
        default:
        {
            sprintf(errmsg, "ERROR: unknown message type[%d]", msgType);
		    return false;
        }
    }
    return true;
}

/*****************************************************************************/
/* 1. Function Name: GetLogicalIDByBcrID                                     */
/* 2. Description  : Logical ID DB query 함수                                */
/* 3. Parameters   : char *bcrID     - Cassete ID                            */
/*                   char *logicalID - Logical ID                            */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetLogicalIDByBcrID(char *bcrID, char *logicalID, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};
    int l_length = -1;
    
    pthread_mutex_lock(&msg_mtx);	
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        "SELECT RTRIM(CST_ID) CST_ID, RTRIM(LOGICAL_ID) LOGICAL_ID, LENGTH(RTRIM(LOGICAL_ID)) L_LENGTH FROM LTSCST WHERE CST_ID=:v1");
        /*
        "SELECT CST_ID, LOGICAL_ID, LENGTH(LOGICAL_ID) L_LENGTH FROM LTSCST WHERE CST_ID=:v1");
        */
    strcpy( ga_bindframe_stt.bind_str[0], bcrID );
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
   
    if( ret_i > 0 ){
        if(getSubstr(resultSet, "^L_LENGTH=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                l_length = atoi(tmp_str);   
            } else {
                logicalID[0] = NULL;
                return true;
            }
        }
        if( getSubstr(resultSet, "^LOGICAL_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
            	if (memcmp(bcrID,"R",1) == 0) {
            	     sprintf (logicalID, "%-14s", tmp_str);
            	} else {            	
                    strcpy(logicalID, tmp_str);
                }
                /*
                if((strlen(logicalID) + 1) == l_length){
                    strcat(logicalID, " ");
                } else if(strlen(logicalID) == l_length){
                } else {
                    logicalID[0] = NULL;
                    sprintf(errmsg, "ERROR: GetLogicalIDByBcrID Fail CSTID[%s]::LEN[%d] DBLEN[%d]",
                                                bcrID,strlen(logicalID),l_length);
                    return false;
                }
                */
            } else {
                logicalID[0] = NULL;
                return true;
            }
        }
    } else {
        sprintf(errmsg, "ERROR: GetLogicalIDByBcrID Fail CSTID[%s]::%s", bcrID, resultSet);
        return false;
    }
     return true;
}

/*****************************************************************************/
/* 1. Function Name: GetLogicalIDByBcrIDEmpty                                */
/* 2. Description  : Logical ID DB query 함수                                */
/* 3. Parameters   : char *bcrID     - Cassete ID                            */
/*                   char *logicalID - Logical ID                            */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetLogicalIDByBcrIDEmpty(char *bcrID, char *logicalID, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};
    int l_length = -1;
    
    pthread_mutex_lock(&msg_mtx);	
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        "SELECT DECODE (LOGICAL_ID, ' ', 'ZZEMPTY-' || CST_ID, LOGICAL_ID) LOGICAL_ID FROM LTSCST WHERE CST_ID = :v1");

    strcpy( ga_bindframe_stt.bind_str[0], bcrID );
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
   
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^LOGICAL_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') strcpy(logicalID, tmp_str );
        }
    } else {
        sprintf(errmsg, "ERROR: GetLogicalIDByBcrID Emtpy Fail CSTID[%s]::%s", bcrID, resultSet);
        return false;
    }
    return true;
}

/*****************************************************************************/
/* 1. Function Name: getPodIDToCstID                                     */
/* 2. Description  : Logical ID DB query 함수                                */
/* 3. Parameters   : char *bcrID     - Cassete ID                            */
/*                   char *logicalID - Logical ID                            */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int getPodIDToCstID(char *PodID, char *CstID, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    
    char resultSet[BUFSIZ]={0,};
    pthread_mutex_lock(&msg_mtx);
    
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str, 
            "SELECT RTRIM(LOGICAL_ID) CST_ID FROM LTSCST WHERE CST_ID =:v1");
    strcpy( ga_bindframe_stt.bind_str[0], PodID );
    
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
    
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^CST_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') strcpy(CstID, tmp_str );
        }
    } else {
        sprintf(errmsg, "ERROR: getPodIDToCstID Fail PodID[%s]::%s",PodID, resultSet);
        return -1;
    }
    return true;
}


/*****************************************************************************/
/* 1. Function Name: GetIrtNameByIrt                                         */
/* 2. Description  : IrtName DB query 함수                                   */
/* 3. Parameters   : char *irtID     - IRT ID                                */
/*                   char *irtName   - IrtName ID                            */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetIrtNameByIrt(char *irtID, char *irtName, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    
    char resultSet[BUFSIZ]={0,};
    pthread_mutex_lock(&msg_mtx);
    
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str, 
            "SELECT PORT_ID FROM ltsstkportinfo WHERE IRT_ID=:v1");
    strcpy( ga_bindframe_stt.bind_str[0], irtID );
    
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
    
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^PORT_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') strcpy( irtName, tmp_str );
        }
    } else {
        sprintf(errmsg, "ERROR: GetIrtNameByIrt Fail IRTID[%s]::%s",irtID, resultSet);
        return -1;
    }
    return true;
}

/*****************************************************************************/
/* 1. Function Name: GetBcrIPByIrt                                           */
/* 2. Description  : 고정식 BCR IP DB query 함수                             */
/* 3. Parameters   : char *irtName   - IRT name                              */
/*                   char *irtID     - IRT ID                                */
/*                   char *portType  - port type                             */
/*                   char *bcrIP     - 고정식 BCR IP                         */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetBcrIPByIrt(char *irtName, char *irtID, char* portType, char *bcrIP, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};
    
    pthread_mutex_lock(&msg_mtx);
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        "SELECT A.IRT_ID, B.IP_ADDR, PORT_TYPE FROM LTSSTKPORTINFO A, LTSTERMINAL B \
         WHERE  A.PORT_ID = :v1 \
         AND	A.SCANNER_ID = B.SCANNER_ID");
         
    strcpy( ga_bindframe_stt.bind_str[0], irtName );         
	
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
	
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^IP_ADDR=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') strcpy( bcrIP, tmp_str );
        }
        if( getSubstr(resultSet, "^IRT_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') strcpy( irtID, tmp_str );
        }
        if( getSubstr(resultSet, "^PORT_TYPE=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                if(memcmp(tmp_str, "PT01", 4) == 0){
                    portType[0] = 'I';
                } else if(memcmp(tmp_str, "PT02", 4) == 0){
                    portType[0] = 'O';
                } else if(memcmp(tmp_str, "PT03", 4) == 0){
                    portType[0] = 'C';
                } else {
                    sprintf(errmsg, "ERROR: PORTID[%s] unknown port type[%s]", irtName, tmp_str);
                    return false;
                }
            } else {
                sprintf(errmsg, "ERROR: PORTID[%s] is not seting port type", irtName);
                return false;
            }
        }
        return true;
    } else {
        sprintf(errmsg, "ERROR: GetBcrIPByIrt Fail PORTID[%s]::%s",irtName, resultSet);
        return -1;
    }
    return true;
}

/*****************************************************************************/
/* 1. Function Name: GetBcrInfoByBcrIP                                       */
/* 2. Description  : Bcr Info DB query 함수                                  */
/* 3. Parameters   : char *bcrIP     - barcode reader ip                     */
/*                   char *stkName   - stocker name                          */
/*                   char *bcrName   - barcode reader name                   */
/*                   char *portName  - stocker port name                     */
/*                   char *portType  - stocker port type                     */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetBcrInfoByBcrIP(char *bcrIP, char *stkName, char *bcrName, char *portName, char *portType, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    
    char resultSet[BUFSIZ]={0,};
    pthread_mutex_lock(&msg_mtx);
    
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str, 
            "SELECT PORT_ID, PORT_TYPE,SCANNER_ID,STK_ID FROM LTSSTKPORTINFO \
             WHERE SCANNER_ID=(SELECT SCANNER_ID FROM LTSTERMINAL WHERE IP_ADDR=:v1)");
    strcpy( ga_bindframe_stt.bind_str[0], bcrIP );
    
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
    
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^PORT_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') strcpy( portName, tmp_str );
        }
        if( getSubstr(resultSet, "^STK_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') strcpy( stkName, tmp_str );
        }
        if( getSubstr(resultSet, "^PORT_TYPE=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                if(strcmp(tmp_str,"PT01") == 0){
                    portType[0] = 'I';
                } else if(strcmp(tmp_str, "PT02") == 0){
                    portType[0] = 'O';
                } else if(strcmp(tmp_str, "PT03") == 0){
                    portType[0] = 'C';
                }
            }
        }
        if( getSubstr(resultSet, "^SCANNER_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') strcpy( bcrName, tmp_str );
        }
    } else {
        sprintf(errmsg, "ERROR: GetBcrInfoByBcrIP Fail BCRIP[%s]::%s",bcrIP, resultSet);
        return -1;
    }
    return true;
}

/*****************************************************************************/
/* 1. Function Name: GetLocationByBcrID                                      */
/* 2. Description  : Cassete 위치 정보 query                                 */
/* 3. Parameters   : char* bcrID     - Cassete ID                            */
/*                   char* location  - 장비 ID                               */
/*                   char* portID    - 장비 PORT ID                          */
/*                   char* errmsg    - error message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetLocationByBcrID(char *bcrID, char* location, char* portID, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};
    
    pthread_mutex_lock(&msg_mtx);	
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        "SELECT RTRIM(CST_ID) CST_ID, RTRIM(LOCATION) LOCATION, RTRIM(PORT_ID) PORT_ID FROM LTSCST WHERE CST_ID=:v1");
        /*
        "SELECT CST_ID, LOCATION, PORT_ID FROM LTSCST WHERE CST_ID=:v1");
        */
    strcpy( ga_bindframe_stt.bind_str[0], bcrID );
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
   
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^LOCATION=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(location, tmp_str);   
            } else {
                sprintf(errmsg, "ERROR: CSTID[%s] location is null",bcrID);
                location[0] = NULL;
                return true;
            }
        }
        if( getSubstr(resultSet, "^PORT_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(portID, tmp_str);   
            } else {
                sprintf(errmsg, "ERROR: CSTID[%s] location port is null",bcrID);
                portID[0] = NULL;
                return true;
            }
        }
    } else {
        sprintf(errmsg, "ERROR: GetLocationByBcrID Fail CSTID[%s]::%s", bcrID, resultSet);
        return false;
    }
     return true;
}

/*****************************************************************************/
/* 1. Function Name: GetCurrentHistoryByBcrID                                */
/* 2. Description  : Cassete 위치 정보 query                                 */
/* 3. Parameters   : char* bcrID     - Cassete ID                            */
/*                   char* location  - 장비 ID                               */
/*                   char* logicalID - logical   ID                          */
/*                   char* errmsg    - error message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetCurrentHistoryByBcrID(char *bcrID, char* location, char* logicalID, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};
    
    pthread_mutex_lock(&msg_mtx);	
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        "SELECT RTRIM(LOGICAL_ID) LOGICAL_ID, RTRIM(LOCATION) LOCATION FROM LTSINOUT_HIST \
            WHERE CST_ID=:v1 \
            AND LOCATION=:v2 \
            AND BOUND='IN'   \
            AND CREATE_DATE = (SELECT MAX(CREATE_DATE) FROM LTSINOUT_HIST WHERE CST_ID=:v3)");

    strcpy( ga_bindframe_stt.bind_str[0], bcrID );
    strcpy( ga_bindframe_stt.bind_str[1], location );
    strcpy( ga_bindframe_stt.bind_str[2], bcrID );
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
   
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^LOGICAL_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(logicalID, tmp_str);   
            } else {
                sprintf(errmsg, "ERROR: CSTID[%s] is not input state STK[%s]",bcrID, location);
                return false;
            }
        }
    } else {
        sprintf(errmsg, "ERROR: GetCurrentHistoryByBcrID Fail CSTID[%s],STK[%s]::%s", bcrID, location, resultSet);
        return false;
    }
     return true;
}

/*****************************************************************************/
/* 1. Function Name: GetStkTypeByIP                                          */
/* 2. Description  : STK name DB query 함수                                  */
/* 3. Parameters   : char *stkIP     - STK IP                                */
/*                   char *stkName   - STK name                              */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetStkTypeByIP(char *stkIP, char *stkName, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};
    
    pthread_mutex_lock(&msg_mtx);
    
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        "SELECT STK_TYPE, STK_ID FROM LTSSTK WHERE IP_ADDR=:v1");
    strcpy( ga_bindframe_stt.bind_str[0], stkIP );         
	
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
	
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^STK_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') strcpy( stkName, tmp_str );
        }
        if( getSubstr(resultSet, "^STK_TYPE=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                if(memcmp(tmp_str, "ST01", 4) == 0){
                    return LOTPODTYPE;
                } else if(memcmp(tmp_str, "ST02", 4) == 0){
                    return RETICLEPODTYPE;
                } else if(memcmp(tmp_str, "ST03", 4) == 0){
                    return RETICLEBARETYPE;
                } else {
                    sprintf(errmsg, "ERROR: STKIP[%s] is not stocker or stocker type",stkIP);
                    return false;
                }
            } else {
                sprintf(errmsg, "ERROR: STKIP[%s] is not stocker or stocker type",stkIP);
                return false;
            }
        } else {
            sprintf(errmsg,"ERROR: QUERY[%s] parsing resultset error",resultSet);
            return false;
        }
    } else {
        sprintf(errmsg, "ERROR: GetStkTypeByIP Fail STKIP[%s]::%s", stkIP, resultSet);
        return false;
    }
    return true;
}

/*****************************************************************************/
/* 1. Function Name: GetStkTypeByStkName                                     */
/* 2. Description  : STK name DB query 함수                                  */
/* 3. Parameters   : char *stkName   - STK name                              */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetStkTypeByStkName(char *stkName, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};

    pthread_mutex_lock(&msg_mtx);
    
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        "SELECT STK_TYPE, STK_ID FROM LTSSTK WHERE STK_ID=:v1");
    strcpy( ga_bindframe_stt.bind_str[0], stkName);         
	
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);

    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^STK_TYPE=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                if(memcmp(tmp_str, "ST01", 4) == 0){
                    return LOTPODTYPE;
                } else if(memcmp(tmp_str, "ST02", 4) == 0){
                    return RETICLEPODTYPE;
                } else if(memcmp(tmp_str, "ST03", 4) == 0){
                    return RETICLEBARETYPE;
                } else {
                    sprintf(errmsg, "ERROR: STK[%s] is not stocker or stocker type",stkName);
                    return false;
                }
            } else {
                sprintf(errmsg, "ERROR: STK[%s] is not stocker or stocker type",stkName);
                return false;
            }
        } else {
            sprintf(errmsg,"ERROR: QUERY[%s] parsing resultset error",resultSet);
            return false;
        }
    } else {
        sprintf(errmsg, "ERROR: GetStkTypeByStkName Fail STKNAME[%s]::%s",
                                                                    stkName, resultSet);
        return false;
    }
    return true;
}

/*****************************************************************************/
/* 1. Function Name: GetTagIDByBcrID                                         */
/* 2. Description  : Teltag ID DB query 함수                                 */
/* 3. Parameters   : char *bcrID     - Barcode ID                            */
/*                   char *tagID     - Teltag ID                             */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetTagIDByBcrID(char *bcrID, char *tagID, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};
    
    pthread_mutex_lock(&msg_mtx);
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        "SELECT TAG_ID FROM LTSBCRTAG WHERE BARCODE=:v1");
        
    strcpy( ga_bindframe_stt.bind_str[0], bcrID );         
	
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
	
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^TAG_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(tagID, tmp_str);   
            } else {
                sprintf(errmsg, "ERROR: BCRID[%s] is not mapping Tag ID",bcrID);
                return false;
            }
        } else {
            sprintf(errmsg,"ERROR: QUERY[%s] parsing resultset error",resultSet);
            return false;
        }
    } else {
        sprintf(errmsg, "ERROR: GetTagIDByBcrID Fail CSTID[%s]::%s",bcrID, resultSet);
        return false;
    }
    return true;
}

/*****************************************************************************/
/* 1. Function Name: GetTagIDByBcrID                                         */
/* 2. Description  : Teltag ID DB query 함수                                 */
/* 3. Parameters   : char *bcrID     - Barcode ID                            */
/*                   char *tagID     - Teltag ID                             */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetBcrIDByTagID(char *tagID, char *bcrID, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};
    
    pthread_mutex_lock(&msg_mtx);
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        "SELECT BARCODE FROM LTSBCRTAG WHERE TAG_ID=:v1");
        
    strcpy( ga_bindframe_stt.bind_str[0], tagID );         
	
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
	
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^BARCODE=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(bcrID, tmp_str);   
            } else {
                sprintf(errmsg, "ERROR: TAG[%s] is not mapping Tag ID",tagID);
                return false;
            }
        } else {
            sprintf(errmsg,"ERROR: QUERY[%s] parsing resultset error",resultSet);
            return false;
        }
    } else {
        sprintf(errmsg, "ERROR: GetBcrIDByTagID Fail TAGID[%s]::%s", tagID, resultSet);
        return false;
    }
    return true;
}

/*****************************************************************************/
/* 1. Function Name: GetBcrIDByLogicalID                                     */
/* 2. Description  : Cassete ID DB query 함수                                */
/* 3. Parameters   : char *logicalID - Logical ID                            */
/*                   char *bcrID     - Cassete ID                            */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetBcrIDByLogicalID(char *logicalID, char *bcrID, char *errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};
    
    pthread_mutex_lock(&msg_mtx);
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        //"SELECT RTRIM(CST_ID) CST_ID, RTRIM(LOGICAL_ID) LOGICAL_ID FROM LTSLOGICAL WHERE RTRIM(LOGICAL_ID)=RTRIM(:v1)");
        /*2022.12.05 장비에 Bar Code Reading 시 마지막에 공백이 존재 하는 경우 발생. RTRIM 필요 */
        "SELECT RTRIM(CST_ID) CST_ID, RTRIM(LOGICAL_ID) LOGICAL_ID FROM LTSLOGICAL WHERE LOGICAL_ID=RTRIM(:v1)");
        
    strcpy( ga_bindframe_stt.bind_str[0], logicalID );
	
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
	
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^CST_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(bcrID, tmp_str);   
            } else {
                bcrID[0] = NULL;
                return true;
            }
        } else {
            sprintf(errmsg,"ERROR: QUERY[%s] parsing resultset error",resultSet);
            return false;
        }
    } else {
        sprintf(errmsg, "ERROR: GetBcrIDByLogicalID Fail LOGICALID[%s]::%s",logicalID, resultSet);
        return false;
    }
    return true;
}

/*****************************************************************************/
/* 1. Function Name: GetNextCleanData                                       */
/* 2. Description  : 다음 세정일 표                                         */
/* 3. Parameters   : char *logicalID - Logical ID                            */
/*                   char *bcrID     - Cassete ID                            */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetNextCleanData (char *logicalID, char *NextCleanData, char *tmpCstID, char *errmsg)
{
    int ret_i;
	/*
	char tmpCstID[12]={0,};
	*/
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};
    
    pthread_mutex_lock(&msg_mtx);
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        //"SELECT RTRIM(CST_ID) CST_ID, RTRIM(LOGICAL_ID) LOGICAL_ID FROM LTSLOGICAL WHERE RTRIM(LOGICAL_ID)=RTRIM(:v1)");
        /*2022.12.05 장비에 Bar Code Reading 시 마지막에 공백이 존재 하는 경우 발생. RTRIM 필요 */
        "SELECT RTRIM(CST_ID) CST_ID, RTRIM(LOGICAL_ID) LOGICAL_ID FROM LTSLOGICAL WHERE LOGICAL_ID=RTRIM(:v1)");
        
    strcpy( ga_bindframe_stt.bind_str[0], logicalID );
	
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
	
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^CST_ID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(tmpCstID, tmp_str);
				/*
			    sprintf(errmsg, "DEBUG : GetNextCleanData1 LOT[%s] CST_ID [%s] info:%s ", logicalID, tmpCstID, resultSet);
				logMessage(DEBUG, errmsg);
				*/

            } else {
				/*
			    sprintf(errmsg, "DEBUG : GetNextCleanData2 LOT[%s] CST_ID [%s] info:%s ", logicalID, tmpCstID, resultSet);
				logMessage(DEBUG, errmsg);
				*/

                tmpCstID[0] = NULL;
				NextCleanData[0] = NULL;
                return true;
            }
        } else {
			/*
		    sprintf(errmsg, "DEBUG : GetNextCleanData3 LOT[%s] CST_ID [%s] info:%s ", logicalID, tmpCstID, resultSet);
			logMessage(DEBUG, errmsg);
			*/

            sprintf(errmsg,"ERROR: QUERY[%s] parsing resultset error",resultSet);
            return false;
        }
    } else {
        sprintf(errmsg, "ERROR: GetNextCleanData Fail1 LOGICALID[%s]::%s",logicalID, resultSet);
        return false;
    }
	/*
    sprintf(errmsg, "DEBUG : GetNextCleanData4 LOT[%s] CST_ID [%s] info:%s ", logicalID, tmpCstID, resultSet);
    logMessage(DEBUG, errmsg);
	*/

    pthread_mutex_lock(&msg_mtx);
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
        "SELECT TO_CHAR (SYSDATE + NVL(RTRIM(CST_CMF_3),0) ,'YYYY/MM/DD') NEXT_CLEAN FROM LTSCST WHERE CST_ID = :v1");
        
    strcpy( ga_bindframe_stt.bind_str[0], tmpCstID );
	
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
	
    if( ret_i > 0 ){
        if( getSubstr(resultSet, "^NEXT_CLEAN=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(NextCleanData, tmp_str);
				/*
				sprintf(errmsg, "DEBUG : GetNextCleanData5 LOT[%s] CST_ID [%s] NEXT [%s] info:%s ", logicalID, tmpCstID, NextCleanData, resultSet);
				logMessage(DEBUG, errmsg);
				*/

            } else {
				/*
				sprintf(errmsg, "DEBUG : GetNextCleanData6 LOT[%s] CST_ID [%s] NEXT [%s] info:%s ", logicalID, tmpCstID, NextCleanData, resultSet);
				logMessage(DEBUG, errmsg);
				*/

				NextCleanData[0] = NULL;
                return true;
            }
        } else {
			/*
			sprintf(errmsg, "DEBUG : GetNextCleanData7 LOT[%s] CST_ID [%s] NEXT [%s] info:%s ", logicalID, tmpCstID, NextCleanData, resultSet);
			logMessage(DEBUG, errmsg);
			*/
			
			sprintf(errmsg,"ERROR: QUERY[%s] parsing resultset error",resultSet);
            return false;
        }
    } else {
		/*
		sprintf(errmsg, "DEBUG : GetNextCleanData8 LOT[%s] CST_ID [%s] NEXT [%s] info:%s ", logicalID, tmpCstID, NextCleanData, resultSet);
		logMessage(DEBUG, errmsg);
		*/

        sprintf(errmsg, "ERROR: GetNextCleanData Fail2 LOGICALID[%s]::%s",logicalID, resultSet);
        return false;
    }
	
	/*
    sprintf(errmsg, "DEBUG : GetNextCleanData9 LOT[%s] CST_ID [%s] NEXT [%s] info:%s ", logicalID, tmpCstID, NextCleanData, resultSet);
    logMessage(DEBUG, errmsg);
	*/

    return true;
}

/*****************************************************************************/
/* 1. Function Name: GetLotInfo                                              */
/* 2. Description  : LOT info DB query 함수                                  */
/* 3. Parameters   : char *lotID     - LOT ID                                */
/*                   char* lotInfo   - LOT Info                              */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int GetLotInfo(char *lotID, char* lotInfo, char* errmsg)
{
    int ret_i;
    char tmp_str[MAX_ITEMS]={0,};
    char resultSet[BUFSIZ]={0,};
    char qty[20]={0,};
    char operation[12+1]={0,};
    char opDesc[50+1]={0,};
    char recipe[30+1]={0,};
    char block[12+1]={0,};
    char blDesc[50+1]={0,};
    char device[25+1]={0,};
	char hold_code[10+1]={0,};
    char empty[]="                        ";
    char operation_merge[12+1]={0,};
    char lot_pri[20]={0,};
    char temp_count[5]={0,};
    char tmpNextCleanData[12]={0,};
	char cstID[12]={0,};
	int lot_found_flag = 0;

    if ( 0 == memcmp (lotID, "ZZEMPTY-", strlen ("ZZEMPTY-")))
    {
        memset (lotInfo, ' ', 160);

		sprintf(errmsg, "DEBUG: LOT[%s] LotInfo Emtpy[%s]", lotID, lotInfo);
		logMessage(DEBUG, errmsg);

		return true;    
    }

    
    pthread_mutex_lock(&msg_mtx);
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
    
    sprintf(ga_sqlframe_stt.sqlstat_str,
       "SELECT A.LOT_ID, RTRIM(A.QTY_1) QUANTITY, \
        '['||DECODE(A.LOT_PRIORITY,'8','8','7','7','9','D1','6','D2','5','5',' ')||'] ' LOT_PRI, \
         A.OPER OPERATION, \
         C.OPER_DESC DESCRIPTION , \
         A.RECIPE RECIPEID, A.FLOW BLOCK, B.FLOW_DESC DESCRIPTION2, \
         A.LOT_DESC PRODUCT, \
		 A.HOLD_CODE HOLD_CODE \
         FROM MWIPLOTSTS A,MWIPFLWDEF B,MWIPOPRDEF C \
         WHERE  A.FLOW = B.FLOW \
         AND A.OPER = C.OPER \
         AND A.FACTORY=B.FACTORY \
         AND A.FACTORY=C.FACTORY \
         AND A.LOT_ID = :v1");

        /*    
        "SELECT L.WORKNAME,A.QUANTITY,L.OPERATION, O.DESCRIPTION,L.RECIPEID,L.BLOCK, \
                B.DESCRIPTION DESCRIPTION2 ,L.PRODUCT \
         FROM   WORK L, OPERATION O, AMOUNT A, BLOCK B \
         WHERE  L.WORKNAME  = :v1 \
         AND	L.WORKNAME  = A.WORKNAME(+) \
         AND	L.BLOCK	   = B.BLOCK \
         AND	L.OPERATION = O.OPERATION ");
        */
        
    strcpy( ga_bindframe_stt.bind_str[0], lotID );         
	
    ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
    memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
	pthread_mutex_unlock(&msg_mtx);
	
	if( ret_i > 0 ){

        if( getSubstr(resultSet, "^QUANTITY=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(qty, tmp_str);
            } else {
                qty[0] = NULL;
            }
        } else {
            sprintf(errmsg, "ERROR: LOT[%s] info parsing error msg:%s", lotID,resultSet);
            return false;
        }
        if( getSubstr(resultSet, "^OPERATION=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(operation, tmp_str);
            } else {
                operation[0] = NULL;
            }
        } else {
            sprintf(errmsg, "ERROR: LOT[%s] info parsing error msg:%s", lotID,resultSet);
            return false;
        }
        if( getSubstr(resultSet, "^LOT_PRI=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(lot_pri, tmp_str);
            } else {
                lot_pri[0] = NULL;
            }
        } else {
            sprintf(errmsg, "ERROR: LOT[%s] info parsing error msg:%s", lotID,resultSet);
            return false;
        }
        if( getSubstr(resultSet, "^DESCRIPTION=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(opDesc, tmp_str);
            } else {
                opDesc[0] = NULL;
            }
        } else {
            sprintf(errmsg, "ERROR: LOT[%s] info parsing error msg:%s", lotID,resultSet);
            return false;
        }
        if( getSubstr(resultSet, "^RECIPEID=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(recipe, tmp_str);
            } else {
                recipe[0] = NULL;
            }
        } else {
            sprintf(errmsg, "ERROR: LOT[%s] info parsing error msg:%s", lotID,resultSet);
            return false;
        }
        if( getSubstr(resultSet, "^BLOCK=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(block, tmp_str);
            } else {
                block[0] = NULL;
            }
        } else {
            sprintf(errmsg, "ERROR: LOT[%s] info parsing error msg:%s", lotID,resultSet);
            return false;
        }
        if( getSubstr(resultSet, "^DESCRIPTION2=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(blDesc, tmp_str);
            } else {
                blDesc[0] = NULL;
            }
        } else {
            sprintf(errmsg, "ERROR: LOT[%s] info parsing error msg:%s", lotID,resultSet);
            return false;
        }
        if( getSubstr(resultSet, "^PRODUCT=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(device, tmp_str);
            } else {
                device[0] = NULL;
            }
        } else {
            sprintf(errmsg, "ERROR: LOT[%s] info parsing error msg:%s", lotID,resultSet);
            return false;
        }
        if( getSubstr(resultSet, "^HOLD_CODE=", "^", tmp_str) == SUCCESS ){
            if(tmp_str[0] != '-') {
                strcpy(hold_code, tmp_str);
            } else {
                hold_code[0] = NULL;
            }
        } else {
            sprintf(errmsg, "ERROR: LOT[%s] info parsing error msg:%s", lotID,resultSet);
            return false;
		}

		lot_found_flag = 1;
	}
	else
	{
		lot_found_flag = 0;
	}


	if ( lot_found_flag == 1)
	{
		//2021.04.12 ENGR, DMHD Hold 가 같이 발생하는 경우가 있어 DMHD 만 검출 하도록 수정. 정미원 책임 요청
		//2021.04.11 DMHD 외 CUST Hold Dipslay 요청 정미원 책임 

		memset (hold_code, 0x00, sizeof (hold_code));
	      
		pthread_mutex_lock(&msg_mtx);
		memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
		memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
	    
		// SELECT HOLD_CODE FROM MESMGR.MWIPMHDSTS WHERE LOT_ID = '7146660' ORDER BY DECODE (HOLD_CODE, 'DMHD', 1, 'CUST', 2, 3); 

		sprintf(ga_sqlframe_stt.sqlstat_str,
			"SELECT HOLD_CODE FROM (SELECT HOLD_CODE FROM MESMGR.MWIPMHDSTS WHERE LOT_ID = :V1 AND HOLD_CODE IN ('CUST', 'DMHD') ORDER BY HOLD_CODE DESC) WHERE ROWNUM = 1");
	        
		strcpy( ga_bindframe_stt.bind_str[0], lotID );         
		
		ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
		memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
  		pthread_mutex_unlock(&msg_mtx);
		
		if( ret_i > 0 ){
			if( getSubstr(resultSet, "^HOLD_CODE=", "^", tmp_str) == SUCCESS ){
				if(tmp_str[0] != '-') {
					strcpy(hold_code, tmp_str);
				} else {
					hold_code[0] = NULL;
				}
			} else {
				hold_code[0] = NULL;
			}
		}

		/* Lot PRI 변경 */
		if ( 0 == memcmp(lot_pri, "[5]", 3)) 
		{
			pthread_mutex_lock(&msg_mtx);
			memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
			memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );
	    
			sprintf(ga_sqlframe_stt.sqlstat_str,
				 "SELECT SUBSTR(A.PRIORITY,1,2) TEMP_COUNT \
				  FROM CERSABNINS A, MWIPLOTSTS B \
				  WHERE \
				  A.LOT_ID = B.LOT_ID \
				  AND A.FLOW = B.FLOW \
				  AND A.OPER = B.OPER \
				  AND A.LOT_ID = :v1");
		        
			strcpy( ga_bindframe_stt.bind_str[0], lotID );         
			
			ret_i = eDB_query( SQL_COMMAND, (char *)0, 1 );
			memcpy(resultSet, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
  			pthread_mutex_unlock(&msg_mtx);
		
			if( ret_i > 0 ){
				if( getSubstr(resultSet, "^TEMP_COUNT=", "^", tmp_str) == SUCCESS ){
					if(tmp_str[0] != '-') {
						strcpy(temp_count, tmp_str);
					} else {
						temp_count[0] = NULL;
					}
				} else {
						temp_count[0] = NULL;
				}
			}

			  if ( 0 == memcmp(temp_count, "P3",2) ) {
      			   sprintf (lot_pri, "[%s]","D3");
			  }
		}
	    
		sprintf (operation_merge, "%s %s",  lot_pri, operation);
	    
			sprintf(errmsg, "DEBUG: LOT[%s] info:%s ", lotID,resultSet);
			logMessage(DEBUG, errmsg);

			/*2007.07.12 Recipe Query 부분 변경 */
			ret_i = getRecipe ( lotID, recipe );
    		ret_i = GetNextCleanData (lotID, tmpNextCleanData, cstID, errmsg);

			//2021.03.31 주의 Host 2 줄이 STK 1줄로 표시됨
			//Host 에서 32 Bye Download 시 장비에서 최대 21 자 인식
			//STK 는 21 자 x 5 줄 인식


			sprintf(errmsg, "DEBUG: LOT[%s] recipe:[%s][%d]", lotID,recipe,ret_i);
			logMessage(DEBUG, errmsg);
			/*
			sprintf(lotInfo,"%-12s %-19s%-5s %-26s%12s%-.20s%-10s%-22s%15s%-.17s",
							 lotID, qty, operation, opDesc, recipe, empty, block, blDesc, product, empty);
			*/
			/*
			sprintf(lotInfo,"%-12s %-19s%-10s %-21s%-32s%-10s%-22s%15s%-.17s",
							 lotID, qty, operation_merge, opDesc, recipe, block, blDesc, product, empty);
			*/
			/*
			sprintf(lotInfo,"%-12s %-19s%-10s %-21s%-32s%-10s%-22s%15s %-10s%-.6s",
							 lotID, qty, operation_merge, opDesc, recipe, block, blDesc, product, tmpNextCleanData, empty);
			*/
			/*
			sprintf(lotInfo,"%-12s %-19s%-10s %-21s%-32s%-10s%-22s     %-4s %-10s%-.12s",
							 lotID, qty, operation_merge, opDesc, recipe, block, blDesc, product, tmpNextCleanData, empty);
			*/
			/*
			sprintf(lotInfo,"%-12s%-3s%-17s%-10s %-21s%-32s%-10s%-22s     %-4s %-10s%-.12s",
							 lotID, qty, cstID, operation_merge, opDesc, recipe, block, blDesc, product, tmpNextCleanData, empty);
			*/

			sprintf(lotInfo,"%-12s%-3s%-17s%-11s%-21s%-32s%-10s%-22s%-5s%-5s%-10s%-.12s",
							 lotID, qty, cstID, operation_merge, opDesc, recipe, block, blDesc, hold_code, device, tmpNextCleanData, empty);

			sprintf(errmsg, "DEBUG: LOT[%s] LotInfo[%s]", lotID, lotInfo);
			logMessage(DEBUG, errmsg);

			return true;                         
	} else {
        sprintf(errmsg, "ERROR: LOT[%s] is not found. GetLotInfo Fail::%s", lotID,resultSet);
        return false;
    }	

    return true;
}

/*****************************************************************************/
/* 1. Function Name: InsertBcrTagMapping                                     */
/* 2. Description  : Bcr Tag mapping table Insert                            */
/* 3. Parameters   : char* bcrID     - Barcode ID                            */
/*                   char* tagID     - Teltag ID                             */
/*                   char* stkName   - stocker Name                          */
/*                   char* errmsg    - error message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int InsertBcrTagMapping(char *bcrID, char *tagID, char *stkName, char *errmsg)
{
    int ret_i;
    int resultset[BUFSIZ]={0,};
    
    pthread_mutex_lock(&msg_mtx);
    memset( &ga_sqlframe_stt, NULL, sizeof(SQLFRAME) );
    memset( &ga_bindframe_stt, NULL, sizeof(BINDFRAME) );    

    sprintf(ga_sqlframe_stt.sqlstat_str,
            "INSERT INTO LTSBCRTAG VALUES(:v1, :v2, :v3, sysdate)");
    strcpy( ga_bindframe_stt.bind_str[0], bcrID );  
    strcpy( ga_bindframe_stt.bind_str[1], tagID );  
    strcpy( ga_bindframe_stt.bind_str[2], stkName );
    memcpy(resultset, ga_sqlframe_stt.result_str, strlen(ga_sqlframe_stt.result_str));
    pthread_mutex_unlock(&msg_mtx);
    
    ret_i = eDB_update();
    if( ret_i == FAIL )
    {
         sprintf(errmsg, "ERROR: InsertBcrTagMapping Fail BCRID[%s] TAGID[%s]::%s",bcrID,tagID,resultset);
         return false;
    }
    return true;
}

/*****************************************************************************/
/* 1. Function Name: stk_rConnect_hton                                       */
/* 2. Description  : host -> network  Endian 변환                            */
/* 3. Parameters   : rConnectRequest *req   - host byte 구조체               */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rConnect_hton(rConnectRequest *req)
{
    req->msgLen     = bigToLitts(req->msgLen);   
    return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rClose_hton                                         */
/* 2. Description  : host -> network  Endian 변환                            */
/* 3. Parameters   : rSimpleRequest *req    - host byte 구조체               */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rClose_hton(rSimpleRequest *req)
{
    req->msgLen     = bigToLitts(req->msgLen);
    return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rPhysicalToLogicalSensor_hton                       */
/* 2. Description  : host -> network  Endian 변환                            */
/* 3. Parameters   : rGenRequest* req    - host byte 구조체                  */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rPhysicalToLogicalSensor_hton(rGenRequest* req)
{
    req->msgLen     = bigToLitts(req->msgLen);
    req->itemType   = bigToLittl(req->itemType);
    req->period     = bigToLittl(req->period);
    
    return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rListUnitAtIrt_hton                                 */
/* 2. Description  : host -> network  Endian 변환                            */
/* 3. Parameters   : rQuerySensorRequest* req    - host byte 구조체          */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rListUnitAtIrt_hton(rQuerySensorRequest* req)
{
    req->msgLen     = bigToLitts(req->msgLen);
    req->pingTime   = bigToLittl(req->pingTime);
    req->numItems   = bigToLittl(req->numItems);
    
    return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rReadMemory_hton                                    */
/* 2. Description  : host -> network  Endian 변환                            */
/* 3. Parameters   : rReadRAMRequest* req    - host byte 구조체              */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rReadMemory_hton(rReadRAMRequest* req)
{
    req->msgLen     = bigToLitts(req->msgLen);
    req->period     = bigToLittl(req->period);
    req->addr       = bigToLittl(req->addr);
    
    return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rDisplayMsg_hton                                    */
/* 2. Description  : host -> network  Endian 변환                            */
/* 3. Parameters   : rPostLineRequest *req    - host byte 구조체             */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rDisplayMsg_hton(rPostLineRequest *req)
{
    req->msgLen     = bigToLitts(req->msgLen);
    req->line       = bigToLittl(req->line);
    req->period     = bigToLittl(req->period);
    
    return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rAssociateUnit_hton                                 */
/* 2. Description  : host -> network  Endian 변환                            */
/* 3. Parameters   : rGenRequest* req   - host byte 구조체                   */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rAssociateUnit_hton(rGenRequest* req)
{
    req->msgLen     = bigToLitts(req->msgLen);
    req->itemType   = bigToLittl(req->itemType);
    req->period     = bigToLittl(req->period);
    
    return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rDisassociateUnit_hton                              */
/* 2. Description  : host -> network  Endian 변환                            */
/* 3. Parameters   : rGenRequest* req   - host byte 구조체                   */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rDisassociateUnit_hton(rGenRequest* req)
{
    req->msgLen     = bigToLitts(req->msgLen);
    req->itemType   = bigToLittl(req->itemType);
    req->period     = bigToLittl(req->period);
    
    return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rListUnitAtIrt_ntoh                                 */
/* 2. Description  : network -> host  Endian 변환                            */
/* 3. Parameters   : rQuerySensorReply *rep - network byte 구조체            */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rListUnitAtIrt_ntoh(rQuerySensorReply *rep)
{
    rep->msgLen     = bigToLitts(rep->msgLen);
    rep->result	    = bigToLittl(rep->result);
    rep->totalNum	= bigToLittl(rep->totalNum);
    rep->numItems	= bigToLittl(rep->numItems); 
	rep->responseMsg.unittype		= bigToLitts(rep->responseMsg.unittype);
	rep->responseMsg.updateTime	    = bigToLittl(rep->responseMsg.updateTime);
	rep->responseMsg.moveTime		= bigToLittl(rep->responseMsg.moveTime);
	rep->responseMsg.motionTime	    = bigToLittl(rep->responseMsg.motionTime);
	rep->responseMsg.unitCategory	= bigToLittl(rep->responseMsg.unitCategory);
	rep->responseMsg.sensorCategory = bigToLitts(rep->responseMsg.sensorCategory);
	
	return 0;
} 

/*****************************************************************************/
/* 1. Function Name: stk_rReadMemory_ntoh                                    */
/* 2. Description  : network -> host  Endian 변환                            */
/* 3. Parameters   : rReadRAMReply *rep - network byte 구조체                */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rReadMemory_ntoh(rReadRAMReply *rep)
{
    rep->msgLen     = bigToLitts(rep->msgLen);
	rep->result	    = bigToLitts(rep->result);
	rep->addr       = bigToLittl(rep->addr);
	return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rAssociateUnit_ntoh                                 */
/* 2. Description  : network -> host  Endian 변환                            */
/* 3. Parameters   : rGenReply *rep - network byte 구조체                    */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rAssociateUnit_ntoh(rGenReply *rep)
{
    rep->msgLen	    = bigToLitts(rep->msgLen);
	rep->result	    = bigToLittl(rep->result);
	return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rDisassociateUnit_ntoh                              */
/* 2. Description  : network -> host  Endian 변환                            */
/* 3. Parameters   : rGenReply *rep - network byte 구조체                    */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rDisassociateUnit_ntoh(rGenReply *rep)
{
    rep->msgLen	    = bigToLitts(rep->msgLen);
	rep->result	    = bigToLittl(rep->result);
	return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rPhysicalToLogicalSensor_ntoh                       */
/* 2. Description  : network -> host  Endian 변환                            */
/* 3. Parameters   : rGenReply *rep - network byte 구조체                    */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rPhysicalToLogicalSensor_ntoh(rGenReply *rep)
{
    rep->msgLen	    = bigToLitts(rep->msgLen);
	rep->result	    = bigToLittl(rep->result);
	
	return 0;    
}

/*****************************************************************************/
/* 1. Function Name: stk_rDisplayMsg_ntoh                                    */
/* 2. Description  : network -> host  Endian 변환                            */
/* 3. Parameters   : rSimpleReply *rep - network byte 구조체                 */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rDisplayMsg_ntoh(rSimpleReply *rep)
{
    rep->msgLen	    = bigToLitts(rep->msgLen);
	rep->result	    = bigToLitts(rep->result);
	
    return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rConnect_ntoh                                       */
/* 2. Description  : network -> host  Endian 변환                            */
/* 3. Parameters   : rConnectReply *rep - network byte 구조체                */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rConnect_ntoh(rConnectReply *rep)
{
    rep->msgLen	    = bigToLitts(rep->msgLen);
	rep->result	    = bigToLitts(rep->result);
	rep->major	    = bigToLitts(rep->major);
	rep->minor	    = bigToLitts(rep->minor);
	rep->point	    = bigToLitts(rep->point);
	
	return 0;
}

/*****************************************************************************/
/* 1. Function Name: stk_rClose_ntoh                                         */
/* 2. Description  : network -> host  Endian 변환                            */
/* 3. Parameters   : rSimpleReply *rep - network byte 구조체                 */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int stk_rClose_ntoh(rSimpleReply *rep)
{
    rep->msgLen	    = bigToLitts(rep->msgLen);
	rep->result	    = bigToLitts(rep->result);
	
	return 0;
}

/*****************************************************************************/
/* 1. Function Name: bigToLitts                                              */
/* 2. Description  : Endian 변환 함수                                        */
/* 3. Parameters   : unsinged short num - Enidian 변환할 변수                */
/* 4. Return Value : unsigned short - Endian 변환하여 반환할 변수            */
/*****************************************************************************/
unsigned short bigToLitts(unsigned short num)
{
    unsigned short a,b;
    a = b = num;
    a = a >> 8;
    b = b << 8;
    b = a | b;
    return b;
}

/*****************************************************************************/
/* 1. Function Name: bigToLittl                                              */
/* 2. Description  : Endian 변환 함수                                        */
/* 3. Parameters   : unsigned int num - Enidian 변환할 변수                  */
/* 4. Return Value : unsigned int - Endian 변환하여 반환할 변수              */
/*****************************************************************************/
unsigned int bigToLittl(unsigned int num)
{
    char    temp[4];
    unsigned int ret;
    
    temp[0] = ((char*)&num)[3];
    temp[1] = ((char*)&num)[2];
    temp[2] = ((char*)&num)[1];
    temp[3] = ((char*)&num)[0];
    
    memcpy(&ret,temp,sizeof(int));
    return ret;
}

/*****************************************************************************/
/* 1. Function Name: lts_rAssociateUnit                                      */
/* 2. Description  : Logical ID connect 처리                                 */
/* 3. Parameters   : int   stkType   - STK Type                              */
/*                   char *stkName   - STK name                              */
/*                   char *cstID     - Cassete ID                            */
/*                   char *logicalID - Logical ID                            */
/*                   char *irtName   - STK PORT name                         */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int lts_rAssociateUnit(int stkType, char* stkName, char* cstID, char* logicalID, char* irtName, char* errmsg)
{
    char recvMsgName[7]={0,};
    char sendMsgName[7]={0,};
    char recvBuf[BUFSIZ]={0,};
    char sendBuf[BUFSIZ]={0,};
    char tmpCstID[12]={0,};
    char tmpLogicalID[24]={0,};
    
    char rtnCode[3]={0,};
    char errorMsg[200]={0,};
    int msgID = 99;
    char type;
    
    sprintf(errmsg, "INFO : STK[%s] connect start CSTID[%s] LOGICALID[%s]", stkName, cstID, logicalID);
    logMessage(INFO, errmsg);
    
    if(memcmp(logicalID, "EMPTY", 5) == 0){
        sprintf(errmsg, "INFO : STK[%s] connect end CSTID[%s] LOGICALID[%s]", stkName, cstID, logicalID);
        logMessage(INFO, errmsg);
        return true;
    }
    
    if(stkType == LOTPODTYPE){
        type = 'L';
    } else if(stkType == RETICLEPODTYPE){
        type = 'R';        
    } else if(stkType == RETICLEBARETYPE){
        type = 'R';
    }
    
    if(GetBcrIDByLogicalID(logicalID, tmpCstID, errmsg) == true){
        if(tmpCstID[0] != NULL){
            sprintf(sendMsgName, "LLDR");
            sprintf(sendBuf, "CST_ID=%s|LOGICAL_ID=%s|TYPE=%c|SOURCE=%s|EMP_ID=%s|HHT_NAME=%s", 
                                                tmpCstID, logicalID, type, SERVER_NAME, stkName, stkName);
    
            if(lts_SendRecv(msgID, sendMsgName, sendBuf, recvMsgName, recvBuf, errmsg) == false){
                sprintf(errmsg,"ERROR: STK[%s] reticle disconnect request ltssvr fail",stkName);
                return false;
            } else {
                getSubstr(recvBuf, "RTN_CD=", "|", rtnCode);
                if(atoi(rtnCode) == 0){
                    sprintf(errmsg, "INFO : STK[%s] disconnect success CSTID[%s], LOGICALID[%s]",
                                                                          stkName, tmpCstID, logicalID);
                    logMessage(INFO, errmsg);
                } else {
                    getSubstr(recvBuf, "ERR_MSG=", "|", errorMsg);
                    sprintf(errmsg, "ERROR: STK[%s] LOGICALID[%s] disconnect to CSTID[%s] request ltssvr RESULT[%s], ERRMSG[%s]", 
                                                        stkName, logicalID, tmpCstID, rtnCode,errorMsg);
                    return false;
                }
            }
            if(type == 'R'){
                sprintf(errmsg, "ERROR: STK[%s] LOGICALID[%s] can't connect CSTID[%s] because LOGICALID[%s] already connect BCRID[%s]", 
                                                stkName, logicalID, cstID, logicalID, tmpCstID);
                return false;
            }
        }
    }
    if(GetLogicalIDByBcrID(cstID, tmpLogicalID, errmsg) == true){
        if(tmpLogicalID[0] != NULL){
            sprintf(sendMsgName, "LLDR");
            sprintf(sendBuf, "CST_ID=%s|LOGICAL_ID=%s|TYPE=%c|SOURCE=%s|EMP_ID=%s|HHT_NAME=%s", 
                                                cstID, tmpLogicalID, type, SERVER_NAME, stkName, stkName);
    
            if(lts_SendRecv(msgID, sendMsgName, sendBuf, recvMsgName, recvBuf, errmsg) == false){
                sprintf(errmsg,"ERROR: STK[%s] reticle disconnect request ltssvr fail",stkName);
                return false;
            } else {
                getSubstr(recvBuf, "RTN_CD=", "|", rtnCode);
                if(atoi(rtnCode) == 0){
                    sprintf(errmsg, "INFO : STK[%s] disconnect success CSTID[%s], LOGICALID[%s]",
                                                                            stkName, cstID, tmpLogicalID);
                    logMessage(INFO, errmsg);
                } else {
                    getSubstr(recvBuf, "ERR_MSG=", "|", errorMsg);
                    sprintf(errmsg, "ERROR: STK[%s] LOGICALID[%s] disconnect to BCRID[%s] request ltssvr RESULT[%s], ERRMSG[%s]", 
                                                        stkName, tmpLogicalID, cstID, rtnCode,errorMsg);
                    return false;
                }
            }
            if(type == 'R'){
                sprintf(errmsg, "ERROR: STK[%s] CSTID[%s] can't connect LOGICALID[%s] because CSTID[%s] already connect LOGICALID[%s]", 
                                                stkName, cstID, logicalID, cstID, tmpLogicalID);
                return false;
            }
        }
    }

    memset(sendBuf, 0x00, BUFSIZ);
    memset(recvBuf, 0x00, BUFSIZ);
    sprintf(sendMsgName, "LLCR");
    sprintf(sendBuf, "CST_ID=%s|LOGICAL_ID=%s|TYPE=%c|SOURCE=%s|EMP_ID=%s|HHT_NAME=%s", 
                                        cstID, logicalID, type, SERVER_NAME, stkName, stkName);

    if(lts_SendRecv(msgID, sendMsgName, sendBuf, recvMsgName, recvBuf, errmsg) == false){
        sprintf(errmsg,"ERROR: STK[%s] reticle connect request ltssvr fail",stkName);
        return false;
    } else {
        getSubstr(recvBuf, "RTN_CD=", "|", rtnCode);
        if(atoi(rtnCode) == 0){
            sprintf(errmsg, "INFO : STK[%s] connect success CSTID[%s], LOGICALID[%s]",
                                                                    stkName, cstID, logicalID);
            logMessage(INFO, errmsg);
            return true;
        } else {
            getSubstr(recvBuf, "ERR_MSG=", "|", errorMsg);
            sprintf(errmsg, "ERROR: STK[%s] LOGICALID[%s] connect to CSTID[%s] request ltssvr RESULT[%s], ERRMSG[%s]", 
                                            stkName, logicalID, cstID, rtnCode,errorMsg);
            return false;
        }
    }
    sprintf(errmsg, "INFO : STK[%s] connect end CSTID[%s] LOGICALID[%s]", stkName, cstID, logicalID);
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: lts_rDisassociateUnit                                   */
/* 2. Description  : Logical ID disconnect 처리                              */
/* 3. Parameters   : int   stkType   - STK Type                              */
/*                   char *stkName   - STK name                              */
/*                   char *cstID     - Cassete ID                            */
/*                   char *logicalID - Logical ID                            */
/*                   char *irtName   - STK PORT name                         */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int lts_rDisassociateUnit(int stkType, char* stkName, char* cstID, char* logicalID, char* irtName, char* errmsg)
{
    char recvMsgName[7]={0,};
    char sendMsgName[7]={0,};
    char recvBuf[BUFSIZ]={0,};
    char sendBuf[BUFSIZ]={0,};
    char rtnCode[3]={0,};
    char errorMsg[100]={0,};
    
    char tmpLogicalID[24]={0,};
    char tmpCstID[12]={0,};
    
    int msgID = 99;
    char type;
    
    sprintf(errmsg, "INFO : STK[%s] disconnect start CSTID[%s] LOGICALID[%s]", stkName, cstID, logicalID);
    logMessage(INFO, errmsg);
    
    if(memcmp(logicalID, "EMPTY", 5) == 0){
        sprintf(errmsg, "INFO : STK[%s] disconnect end CSTID[%s] LOGICALID[%s]", stkName, cstID, logicalID);
        logMessage(INFO, errmsg);
        return true;
    }
    
    if(stkType == LOTPODTYPE){
        type = 'L';
    } else if(stkType == RETICLEPODTYPE){
        type = 'R';
    } else if(stkType == RETICLEBARETYPE){
        type = 'R';
    }
    
    sprintf(sendMsgName, "LLDR");
    sprintf(sendBuf, "CST_ID=%s|LOGICAL_ID=%s|TYPE=%c|SOURCE=%s|EMP_ID=%s|HHT_NAME=%s", 
                                        cstID, logicalID, type, SERVER_NAME, stkName, stkName);

    if(lts_SendRecv(msgID, sendMsgName, sendBuf, recvMsgName, recvBuf, errmsg) == false){
        sprintf(errmsg,"ERROR: STK[%s] LOGICALID[%s] disconnect request ltssvr fail",stkName,logicalID);
        return false;
    } else {
        getSubstr(recvBuf, "RTN_CD=", "|", rtnCode);
        if(atoi(rtnCode) == 0){
            sprintf(errmsg, "INFO : STK[%s] disconnect success CSTID[%s], LOGICALID[%s]",
                                                                    stkName, cstID, logicalID);
            logMessage(INFO, errmsg);
        } else {
            getSubstr(recvBuf, "ERR_MSG=", "|", errorMsg);
            sprintf(errmsg, "ERROR: STK[%s] LOGICALID[%s] disconnect to CSTID[%s] request ltssvr RESULT[%s], ERRMSG[%s]", 
                                                stkName, logicalID, cstID, rtnCode,errorMsg);
            return false;
        }
    }
    sprintf(errmsg, "INFO : STK[%s] disconnect end CSTID[%s] LOGICALID[%s]", stkName, cstID, logicalID);
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: lts_inputRequest                                        */
/* 2. Description  : LOT/Reticle 입고 처리                                   */
/* 3. Parameters   : int   stkType   - STK Type                              */
/*                   char *stkName   - STK name                              */
/*                   char *cstID     - Cassete ID                            */
/*                   char *logicalID - Logical ID                            */
/*                   char *irtName   - STK PORT name                         */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int lts_inputRequest(int stkType, char* stkName, char* cstID, char* logicalID, char* irtName, char* errmsg)
{
    char recvMsgName[7]={0,};
    char sendMsgName[7]={0,};
    char recvBuf[BUFSIZ]={0,};
    char sendBuf[BUFSIZ]={0,};
    char rtnCode[3]={0,};
    char errorMsg[200]={0,};
    
    int msgID = 99;
    char type;
    
    if(stkType == LOTPODTYPE){
        type = 'L';
    } else if(stkType == RETICLEPODTYPE){
        type = 'R';        
    } else if(stkType == RETICLEBARETYPE){
        type = 'R';
    }

	//2017.07.20 위치 정보 Update 기능이 EAP 로 전환 되어 Logic 수정
	if ( 0 == memcmp (stkName, "CPST", strlen ("CPST")))
	{
	    sprintf(errmsg, "INFO : STK[%s] inputRequest Skip CSTID[%s] LOGICALID[%s]", stkName, cstID, logicalID);
		logMessage(INFO, errmsg);

		return true;
	}

    sprintf(errmsg, "INFO : STK[%s] inputRequest start CSTID[%s] LOGICALID[%s]", stkName, cstID, logicalID);
    logMessage(INFO, errmsg);
    
    sprintf(sendMsgName, "LIPR");
    sprintf(sendBuf, "CST_ID=%s|LOGICAL_ID=%s|LOCATION=%s|PORT_ID=%s|TERMINAL_ID=%s|EQ_TYPE=STOCK|SOURCE=%s|EMP_ID=%s|HHT_NAME=%s",
                       cstID, logicalID, stkName, irtName, stkName, SERVER_NAME, stkName, stkName);

    if(lts_SendRecv(msgID, sendMsgName, sendBuf, recvMsgName, recvBuf, errmsg) == false){
        sprintf(errmsg,"ERROR: STK[%s] lts input request fail..",stkName);
        return false;
    } else {
        getSubstr(recvBuf, "RTN_CD=", "|", rtnCode);
        if(atoi(rtnCode) == 0){
            sprintf(errmsg, "INFO : STK[%s] inputRequest success CSTID[%s], LOGICALID[%s]",
                                                                    stkName, cstID, logicalID);
            logMessage(INFO, errmsg);
        } else {
            getSubstr(recvBuf, "ERR_MSG=", "|", errorMsg);
            sprintf(errmsg, "ERROR: STK[%s] CSTID[%s] LOGICALID[%s] input request ltssvr RESULT[%s], ERRMSG[%s]", 
                                                stkName, cstID, logicalID, rtnCode,errorMsg);
            return false;
        }
    }
    sprintf(errmsg, "INFO : STK[%s] inputRequest end CSTID[%s] LOGICALID[%s]", stkName, cstID, logicalID);
    logMessage(INFO, errmsg);                
    return true;
}

/*****************************************************************************/
/* 1. Function Name: lts_outputRequest                                       */
/* 2. Description  : LOT/Reticle 출고 처리                                   */
/* 3. Parameters   : int   stkType   - STK Type                              */
/*                   char *stkName   - STK name                              */
/*                   char *cstID     - Cassete ID                            */
/*                   char *logicalID - Logical ID                            */
/*                   char *irtName   - STK PORT name                         */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int lts_outputRequest(int stkType, char* stkName, char* cstID, char* logicalID, char* irtName, char* errmsg)
{
    char recvMsgName[7]={0,};
    char sendMsgName[7]={0,};
    char recvBuf[BUFSIZ]={0,};
    char sendBuf[BUFSIZ]={0,};
    char rtnCode[3]={0,};
    char errorMsg[200]={0,};
    
    int msgID = 99;
    char type;
    
    if(stkType == LOTPODTYPE){
        type = 'L';
    } else if(stkType == RETICLEPODTYPE){
        type = 'R';        
    } else if(stkType == RETICLEBARETYPE){
        type = 'R';
    }

	//2017.07.20 위치 정보 Update 기능이 EAP 로 전환 되어 Logic 수정
	if ( 0 == memcmp (stkName, "CPST", strlen ("CPST")))
	{
	    sprintf(errmsg, "INFO : STK[%s] outRequest Skip CSTID[%s] LOGICALID[%s]", stkName, cstID, logicalID);
		logMessage(INFO, errmsg);

		return true;
	}


    sprintf(errmsg, "INFO : STK[%s] outRequest start CSTID[%s] LOGICALID[%s]", stkName, cstID, logicalID);
    logMessage(INFO, errmsg);
    
    sprintf(sendMsgName, "LOPR");
    sprintf(sendBuf, "CST_ID=%s|LOGICAL_ID=%s|LOCATION=%s|PORT_ID=%s|TERMINAL_ID=%s|EQ_TYPE=STOCK|SOURCE=%s|EMP_ID=%s|HHT_NAME=%s",
                       cstID, logicalID, stkName, irtName, stkName, SERVER_NAME, stkName, stkName);
    if(lts_SendRecv(msgID, sendMsgName, sendBuf, recvMsgName, recvBuf, errmsg) == false){
        sprintf(errmsg, "ERROR: STK[%s] output request ltssvr fail", stkName);
        return false;
    } else {
        getSubstr(recvBuf, "RTN_CD=", "|", rtnCode);
        if(atoi(rtnCode) == 0){
            sprintf(errmsg, "INFO : STK[%s] outRequest success CSTID[%s], LOGICALID[%s]",
                                                                    stkName, cstID, logicalID);
            logMessage(INFO, errmsg);
        } else {
            getSubstr(recvBuf, "ERR_MSG=", "|", errorMsg);
            sprintf(errmsg, "ERROR: STK[%s] CSTID[%s] LOGICALID[%s] output request ltssvr RESULT[%s], ERRMSG[%s]", 
                                    stkName, cstID, logicalID, rtnCode, errorMsg);
            return false;
        }
    }
    sprintf(errmsg, "INFO : STK[%s] outRequest end CSTID[%s] LOGICALID[%s]", stkName, cstID, logicalID);
    logMessage(INFO, errmsg);
    return true;
}

/*****************************************************************************/
/* 1. Function Name: lts_SendRecv                                            */
/* 2. Description  : LTSsvr 통신 모듈                                        */
/* 3. Parameters   : int   msgID     - message ID                            */
/*                   char *s_msgName - LTSsvr send 할 message name           */
/*                   char *sendBuf   - LTSsvr send 할 message buffer         */
/*                   char *r_msgName - LTSsvr recv 할 message name           */
/*                   char *recvBuf   - LTSsvr send 할 message buffer         */
/*                   char *errmsg    - Error Message                         */
/* 4. Return Value : int                                                     */
/*****************************************************************************/
int lts_SendRecv(int msgID, char *s_msgName, char *sendBuf, char *r_msgName, char *recvBuf, char* errMsg)
{
    int ltssock;
    int logsock;
    int result;
    int count = 0;

    if ( (logsock = connSocket_unix(logFile, errMsg) ) == false ) {
		logMessage(errMsg);
    }
    memset(errMsg, 0x00, BUFSIZ);
    while(1){
        if ( (ltssock = connSocket_unix(ltsFile, errMsg) ) == false ) {
		    logMessage(ERROR, errMsg);
    		count++;
    		if(count > RETRY){
    		    if(logsock >= 0){
                    close(logsock);
                }
    		    return false;
    		}
        } else {
            break;
        }
    }    

    result = sendMessage(ltssock, msgID, s_msgName, sendBuf, errMsg);
    if(result < 0){
        logMessage(ERROR, errMsg);
        close(ltssock);
        if(logsock >= 0){
            close(logsock);
        }
        return false;
    }
    if(logsock >= 0){
        if(sendLogMessage(logsock, SERVER_NAME, "->LTSsvr", s_msgName, sendBuf, errMsg) < 0){
            logMessage(ERROR, errMsg);
        }
    }
    memset(r_msgName, 0x00, 7);
    memset(recvBuf, 0x00, BUFSIZ);
    result = recvMessage(ltssock, &msgID, r_msgName, recvBuf, errMsg);
    if(result < 0){
        logMessage(ERROR, errMsg);
        close(ltssock);
        if(logsock != false){
            close(logsock);
        }
        return false;
    }
    close(ltssock);
    if(logsock >= 0){
        if(sendLogMessage(logsock, SERVER_NAME, "<-LTSsvr", r_msgName, recvBuf, errMsg) < 0){
            logMessage(ERROR, errMsg);
        }
    }
    if(logsock >= 0){
        close(logsock);
    }
    return true;
}

int msgVerify(char msgType)
{
    switch(msgType){  
        case msgTypeConnectRequest :
        {
            return true;
        }
        case msgTypeCloseRequest :
        {
            return true;
        }
        case msgTypePTLSensor :
        {
            return true;
        }
        case msgTypeLTPSensor :
        {
            return true;
        }
        case msgTypeQuerySensorLoc :
        {    
            return true;
        }
        case msgTypeReadMemory :
        {
            return true;
        }
        case msgTypeAssociateUnit :
        {            
            return true;
        }
        case msgTypeDisassociateUnit :
        {
            return true;
        }
        case msgTypeDisplayMsg :
        {
            return true;
        }
        case msgTypeLTPUnit :
        {
            return true;
        }
        default :
        {
            return false;
        }
    }
    return false;
}
