
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wincon.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <ctype.h>
#include <time.h>

#define NUM 80

#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#define SCR_ROW 25             /*��Ļ����*/
#define SCR_COL 80             /*��Ļ����*/

/*���������ṹ*/
 struct room_node{
                char  room_id   [NUM];              /**���ݱ��*/
                char  block_id  [NUM];              /**¥�����*/
                char  land_id   [NUM];              /**¥�̱��*/
                char  type      [NUM];              /**����*/
                char  area      [NUM];              /**���*/
                char  price     [NUM];              /**�۸�*/
                struct   room_node * next_room ;    /**ָ����һ����ָ��*/
                };

/*¥�������ṹ*/
 struct block_node{
                char    block_id  [NUM] ;             /**¥�����*/
                char    land_id   [NUM];              /**¥�̱��*/
                char    pool_rote  [NUM];             /**��̯��*/
                char    unit [NUM] ;                  /**��Ԫ*/
                char    floor[NUM];                   /**¥��*/
                struct block_node * next_block;     /**ָ����һ����ָ��*/
                struct room_node  * hd_room;        /**ָ������Ϣ֧����ָ��*/
                };

/*¥�������ṹ*/
        struct  land_node{
                char  land_id   [NUM];               /**¥�̱��*/
                char    land_name    [NUM];          /**¥������*/
                char    land_add     [NUM];          /**¥�̵�ַ*/
                char    land_tel     [NUM];          /**��ϵ��ʽ*/
                char    land_area    [NUM];          /**��������*/
                char    developer    [NUM];          /**������*/
                char    pro_man_com  [NUM];          /**��ҵ��˾*/
                struct  land_node  * next_land;      /**ָ����һ����ָ��*/
                struct  block_node * hd_block;       /**ָ��¥������ָ��*/
                };

        struct block_sta{
            char block_id [NUM];
            int  num ;
            struct block_sta * nx_block_sta;

        };

        struct land_sta{
            char land_id  [NUM];
            char land_name [NUM];
            int  num;
            struct land_sta * nx_land_sta;
            struct block_sta * hd_block_sta;
        };

/**
 *��Ļ������Ϣ�������ṹ
 */
typedef struct layer_node
{
    char LayerNo;            /**< �������ڲ���*/
    SMALL_RECT rcArea;       /**< ����������������*/
    CHAR_INFO *pContent;     /**< �������������ַ���Ԫԭ��Ϣ�洢������*/
    char *pScrAtt;           /**< �������������ַ���Ԫԭ����ֵ�洢������*/
    struct layer_node *next; /**< ָ����һ����ָ��*/
} LAYER_NODE;

/**
 *��ǩ���ṹ
 */
typedef struct labe1_bundle
{
    char **ppLabel;        /**< ��ǩ�ַ��������׵�ַ*/
    COORD *pLoc;           /**< ��ǩ��λ�����׵�ַ*/
    int num;               /**< ��ǩ����*/
} LABEL_BUNDLE;

/**
 *�����ṹ
 */
typedef struct hot_area
{
    SMALL_RECT *pArea;     /**< ������λ�����׵�ַ*/
    char *pSort;           /**< �������(�������ı���ѡ���)�����׵�ַ*/
    char *pTag;            /**< ������������׵�ַ*/
    int num;               /**< ��������*/
} HOT_AREA;

LAYER_NODE *gp_top_layer = NULL;               /*����������Ϣ����ͷ*/
struct land_node * hd_land = NULL;        /*¥�����ܵ�ͷָ��*/


char *gp_sys_name = "¥�̹���ϵͳ";                  /*ϵͳ����*/
char *gp_load_info_filename = "load.txt";        /*¥�������ļ�*/
char *gp_bloc_info_filename = "block.txt";       /*¥�������ļ�*/
char *gp_room_info_filename = "room.txt";        /*���������ļ�*/
char *gp_filename = "backupdata.txt";            /*���������ļ�*/




char *ga_main_menu[] = {
                        "�ļ�(F)",             /*ϵͳ���˵���*/
                        "����ά��(M)",
                        "���ݲ�ѯ(Q)",
                        "����ͳ��(S)",
                        "����(O)"
                       };

char *ga_sub_menu[] =
{
    "[S] ���ݱ���",                             /*ϵͳ�Ӳ˵���*/
    "[B] ���ݱ���",
    "[R] ���ݻָ�",
    "",
    "[X] �˳�    Alt+X",
                                                /*�մ������ڵ����˵��зָ��Ӳ˵����ͬ*/
    "[L] ¥����Ϣά��",
    "[B] ¥����Ϣά��",
    "[R] ������Ϣά��",

    "[L] ¥����Ϣ��ѯ",
    "[B] ¥����Ϣ��ѯ",
    "[R] ������Ϣ��ѯ",
    "[A] ���������ѯ",
    "[T] �������Ͳ�ѯ",
    "[P] ���ռ۸��ѯ",
    "",
    "[C] �����������",

    "[A] ��¥�̵���ͳ��",
    "[T] �����ݻ���ͳ��",

    "[H] ����",
    "[A] ����",
};

unsigned long num_written;                  /* Win 7 ��ʹ��, �ñ�������ĳ��������̨������� */

int ga_sub_menu_count[] = {5, 3, 8, 2, 2};  /*�����˵������Ӳ˵��ĸ���*/
int gi_sel_menu = 1;                        /*��ѡ�е����˵����,��ʼΪ1*/
int gi_sel_sub_menu = 0;                    /*��ѡ�е��Ӳ˵����,��ʼΪ0,��ʾδѡ��*/
CHAR_INFO *gp_buff_menubar_info = NULL;     /*��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*���״̬����Ļ���ַ���Ϣ�Ļ�����*/

char *gp_scr_att = NULL;    /*�����Ļ���ַ���Ԫ����ֵ�Ļ�����*/
char gc_sys_state = '\0';   /*��������ϵͳ״̬���ַ�*/

HANDLE gh_std_out;          /*��׼����豸���*/
HANDLE gh_std_in;           /*��׼�����豸���*/



int  CreateList (struct land_node **hd);            /*���������ʼ��*/
void InitInterface(void);                       /*ϵͳ�����ʼ��*/
void ClearScreen(void);                         /*����*/
void ShowMenu(void);                            /*��ʾ�˵���*/
void PopMenu(int num);                          /*��ʾ�����˵�*/
void PopPrompt(int num);                        /*��ʾ��������*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*����������Ļ��Ϣά��*/
void PopOff(void);                              /*�رն��㵯������*/
void DrawBox(SMALL_RECT *parea);                /*���Ʊ߿�*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*���˵������˵���λ*/
void ShowState(void);                           /*��ʾ״̬��*/
void TagMainMenu(int num);                      /*��Ǳ�ѡ�е����˵���*/
void TagSubMenu(int num);                       /*��Ǳ�ѡ�е��Ӳ˵���*/
int  DealConInput(HOT_AREA *phot_area, int *pihot_num);  /*����̨���봦��*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*��������*/
void RunSys(struct land_node * *hd);                  /*ϵͳ����ģ���ѡ�������*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*����ģ��ĵ���*/
void CloseSys(struct land_node *hd);/* �˳�ϵͳ */

BOOL LoadData(void);           /*���ݼ���*/
BOOL SaveData(void);           /*��������*/
BOOL BackupData(void);         /*��������*/
BOOL RestoreData(void);        /*�ָ�����*/
BOOL ExitSys(void);            /*�˳�ϵͳ*/
BOOL HelpTopic(void);          /*��������*/
BOOL AboutDorm(void);          /*����ϵͳ*/

BOOL Maintainl_Info(void);    /*ά��¥����Ϣ*/
BOOL Maintainb_Info(void);    /*ά��¥����Ϣ*/
BOOL Maintainr_Info(void);    /*ά��������Ϣ*/

BOOL Queryl_Info(void);        /*��ѯ¥����Ϣ*/
BOOL Queryb_Info(void);        /*��ѯ¥����Ϣ*/
BOOL Queryr_Info(void);        /*��ѯ¥����Ϣ*/
BOOL QueryLandArea(void);      /*��������ѯ¥��ͨѶ��Ϣ*/
BOOL QueryRoomType(void);       /*�������Ͳ�ѯ����ͨѶ��Ϣ*/
BOOL QueryRoomPrice(void);      /*���ռ۸��ѯ����ͨѶ��Ϣ*/
BOOL Queryap(void);             /*���������ѯ������Ϣ*/

BOOL Stat_a_Info(void);         /*��¥�����ڵĵ���ͳ��*/
BOOL Stat_t_Info(void);         /*�����ݵ�����ͳ��*/


BOOL Success(void);    /*��ӡ�����ɹ�*/
BOOL Fail(void);       /*��ӡ����ʧ��*/


BOOL * SeekRoom_ap(struct land_node *hd, char *area,char *h_price, char * l_price,int sign );

BOOL find(char *  h_price, char *l_price,char *price);

BOOL AddLandnode(struct land_node  * *hd , struct land_node * p_add);/*����¥�̽��*/
BOOL ModifLandnode(struct land_node *hd,char *l_id, struct land_node *p_mod);/*��¥����Ϣ */
BOOL DelLandnode(struct land_node * *hd, char * l_id);/*ɾ��¥�̽�� */

BOOL AddBlocknode(struct land_node *hd , struct block_node * p_add);/*����¥�����*/
BOOL ModifBlocknode(struct land_node *hd, char *l_id, char *b_id,struct block_node * p_mod);/*�޸�¥����Ϣ*/
BOOL DelBlocknode(struct land_node *hd , char * l_id, char * b_id); /* ɾ��¥�����*/

BOOL AddRoomnode (struct land_node *hd, struct room_node * p_add);/*���ӷ��ݽڵ�*/
BOOL ModifRoomnode(struct land_node *hd, char *l_id, char *b_id, char *r_id ,struct room_node *p_mod);/*�޸ķ�����Ϣ*/
BOOL DelRoomnode (struct land_node *hd , char * l_id, char * b_id, char * r_id);/*ɾ�����ݽ�� */



struct land_node * SeekLandnode(struct land_node * hd, char * l_id);/*��ѯ����������¥��*/
struct block_node * SeekBlocknode(struct land_node * hd, char * l_id, char * b_id);/* ��ѯ����������¥��*/
struct room_node * SeekRoomnode(struct land_node * hd, char * l_id, char * b_id,char * r_id);/*��ѯ���������ķ��� */

struct land_sta * StTypenode(struct land_node *hd, char * type);/*ͳ�Ʒ��������ķ�������*/
struct land_node * StAreanode(struct land_node *hd , char * area);/*ͳ��ָ��������¥����Ϣ*/

BOOL SeekLandArea(struct land_node *hd , char * area, int n ); //Ѱ�ҷ��ϵ���������¥����Ϣ
//��ѯ���Ϸ��͵ķ�����Ϣ
BOOL SeekRoomType(struct land_node * hd, char * type);
//��ѯ���ϼ۸�ķ���ͨѶ��Ϣ
BOOL SeekRoomPrice(struct land_node * hd, char * h_price, char * l_price);

#endif /**< TYPE_H_INCLUDED*/


