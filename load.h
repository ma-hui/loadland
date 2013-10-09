
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

#define SCR_ROW 25             /*屏幕行数*/
#define SCR_COL 80             /*屏幕列数*/

/*房屋链结点结构*/
 struct room_node{
                char  room_id   [NUM];              /**房屋编号*/
                char  block_id  [NUM];              /**楼栋编号*/
                char  land_id   [NUM];              /**楼盘编号*/
                char  type      [NUM];              /**户型*/
                char  area      [NUM];              /**面积*/
                char  price     [NUM];              /**价格*/
                struct   room_node * next_room ;    /**指向下一结点的指针*/
                };

/*楼栋链结点结构*/
 struct block_node{
                char    block_id  [NUM] ;             /**楼栋编号*/
                char    land_id   [NUM];              /**楼盘编号*/
                char    pool_rote  [NUM];             /**公摊率*/
                char    unit [NUM] ;                  /**单元*/
                char    floor[NUM];                   /**楼层*/
                struct block_node * next_block;     /**指向下一结点的指针*/
                struct room_node  * hd_room;        /**指向房屋信息支链的指针*/
                };

/*楼盘链结点结构*/
        struct  land_node{
                char  land_id   [NUM];               /**楼盘编号*/
                char    land_name    [NUM];          /**楼盘名称*/
                char    land_add     [NUM];          /**楼盘地址*/
                char    land_tel     [NUM];          /**联系方式*/
                char    land_area    [NUM];          /**所属城区*/
                char    developer    [NUM];          /**开发商*/
                char    pro_man_com  [NUM];          /**物业公司*/
                struct  land_node  * next_land;      /**指向下一结点的指针*/
                struct  block_node * hd_block;       /**指向楼栋链的指针*/
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
 *屏幕窗口信息链结点结点结构
 */
typedef struct layer_node
{
    char LayerNo;            /**< 弹出窗口层数*/
    SMALL_RECT rcArea;       /**< 弹出窗口区域坐标*/
    CHAR_INFO *pContent;     /**< 弹出窗口区域字符单元原信息存储缓冲区*/
    char *pScrAtt;           /**< 弹出窗口区域字符单元原属性值存储缓冲区*/
    struct layer_node *next; /**< 指向下一结点的指针*/
} LAYER_NODE;

/**
 *标签束结构
 */
typedef struct labe1_bundle
{
    char **ppLabel;        /**< 标签字符串数组首地址*/
    COORD *pLoc;           /**< 标签定位数组首地址*/
    int num;               /**< 标签个数*/
} LABEL_BUNDLE;

/**
 *热区结构
 */
typedef struct hot_area
{
    SMALL_RECT *pArea;     /**< 热区定位数组首地址*/
    char *pSort;           /**< 热区类别(按键、文本框、选项框)数组首地址*/
    char *pTag;            /**< 热区序号数组首地址*/
    int num;               /**< 热区个数*/
} HOT_AREA;

LAYER_NODE *gp_top_layer = NULL;               /*弹出窗口信息链链头*/
struct land_node * hd_land = NULL;        /*楼盘链总的头指针*/


char *gp_sys_name = "楼盘管理系统";                  /*系统名称*/
char *gp_load_info_filename = "load.txt";        /*楼盘数据文件*/
char *gp_bloc_info_filename = "block.txt";       /*楼栋数据文件*/
char *gp_room_info_filename = "room.txt";        /*房屋数据文件*/
char *gp_filename = "backupdata.txt";            /*备份数据文件*/




char *ga_main_menu[] = {
                        "文件(F)",             /*系统主菜单名*/
                        "数据维护(M)",
                        "数据查询(Q)",
                        "数据统计(S)",
                        "其他(O)"
                       };

char *ga_sub_menu[] =
{
    "[S] 数据保存",                             /*系统子菜单名*/
    "[B] 数据备份",
    "[R] 数据恢复",
    "",
    "[X] 退出    Alt+X",
                                                /*空串用来在弹出菜单中分隔子菜单项，下同*/
    "[L] 楼盘信息维护",
    "[B] 楼栋信息维护",
    "[R] 房屋信息维护",

    "[L] 楼盘信息查询",
    "[B] 楼栋信息查询",
    "[R] 房屋信息查询",
    "[A] 按照区域查询",
    "[T] 按照类型查询",
    "[P] 按照价格查询",
    "",
    "[C] 组合条件查找",

    "[A] 按楼盘地区统计",
    "[T] 按房屋户型统计",

    "[H] 帮助",
    "[A] 关于",
};

unsigned long num_written;                  /* Win 7 中使用, 该变量用于某几个控制台输出函数 */

int ga_sub_menu_count[] = {5, 3, 8, 2, 2};  /*各主菜单项下子菜单的个数*/
int gi_sel_menu = 1;                        /*被选中的主菜单项号,初始为1*/
int gi_sel_sub_menu = 0;                    /*被选中的子菜单项号,初始为0,表示未选中*/
CHAR_INFO *gp_buff_menubar_info = NULL;     /*存放菜单条屏幕区字符信息的缓冲区*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*存放状态条屏幕区字符信息的缓冲区*/

char *gp_scr_att = NULL;    /*存放屏幕上字符单元属性值的缓冲区*/
char gc_sys_state = '\0';   /*用来保存系统状态的字符*/

HANDLE gh_std_out;          /*标准输出设备句柄*/
HANDLE gh_std_in;           /*标准输入设备句柄*/



int  CreateList (struct land_node **hd);            /*数据链表初始化*/
void InitInterface(void);                       /*系统界面初始化*/
void ClearScreen(void);                         /*清屏*/
void ShowMenu(void);                            /*显示菜单栏*/
void PopMenu(int num);                          /*显示下拉菜单*/
void PopPrompt(int num);                        /*显示弹出窗口*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*弹出窗口屏幕信息维护*/
void PopOff(void);                              /*关闭顶层弹出窗口*/
void DrawBox(SMALL_RECT *parea);                /*绘制边框*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*主菜单下拉菜单定位*/
void ShowState(void);                           /*显示状态栏*/
void TagMainMenu(int num);                      /*标记被选中的主菜单项*/
void TagSubMenu(int num);                       /*标记被选中的子菜单项*/
int  DealConInput(HOT_AREA *phot_area, int *pihot_num);  /*控制台输入处理*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*设置热区*/
void RunSys(struct land_node * *hd);                  /*系统功能模块的选择和运行*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*功能模块的调用*/
void CloseSys(struct land_node *hd);/* 退出系统 */

BOOL LoadData(void);           /*数据加载*/
BOOL SaveData(void);           /*保存数据*/
BOOL BackupData(void);         /*备份数据*/
BOOL RestoreData(void);        /*恢复数据*/
BOOL ExitSys(void);            /*退出系统*/
BOOL HelpTopic(void);          /*帮助主体*/
BOOL AboutDorm(void);          /*关于系统*/

BOOL Maintainl_Info(void);    /*维护楼房信息*/
BOOL Maintainb_Info(void);    /*维护楼栋信息*/
BOOL Maintainr_Info(void);    /*维护房屋信息*/

BOOL Queryl_Info(void);        /*查询楼盘信息*/
BOOL Queryb_Info(void);        /*查询楼栋信息*/
BOOL Queryr_Info(void);        /*查询楼房信息*/
BOOL QueryLandArea(void);      /*按地区查询楼盘通讯信息*/
BOOL QueryRoomType(void);       /*按照类型查询房屋通讯信息*/
BOOL QueryRoomPrice(void);      /*按照价格查询房屋通讯信息*/
BOOL Queryap(void);             /*组合条件查询房屋信息*/

BOOL Stat_a_Info(void);         /*按楼盘所在的地区统计*/
BOOL Stat_t_Info(void);         /*按房屋的类型统计*/


BOOL Success(void);    /*打印操作成功*/
BOOL Fail(void);       /*打印操作失败*/


BOOL * SeekRoom_ap(struct land_node *hd, char *area,char *h_price, char * l_price,int sign );

BOOL find(char *  h_price, char *l_price,char *price);

BOOL AddLandnode(struct land_node  * *hd , struct land_node * p_add);/*增加楼盘结点*/
BOOL ModifLandnode(struct land_node *hd,char *l_id, struct land_node *p_mod);/*修楼盘信息 */
BOOL DelLandnode(struct land_node * *hd, char * l_id);/*删除楼盘结点 */

BOOL AddBlocknode(struct land_node *hd , struct block_node * p_add);/*增加楼栋结点*/
BOOL ModifBlocknode(struct land_node *hd, char *l_id, char *b_id,struct block_node * p_mod);/*修改楼栋信息*/
BOOL DelBlocknode(struct land_node *hd , char * l_id, char * b_id); /* 删除楼栋结点*/

BOOL AddRoomnode (struct land_node *hd, struct room_node * p_add);/*增加房屋节点*/
BOOL ModifRoomnode(struct land_node *hd, char *l_id, char *b_id, char *r_id ,struct room_node *p_mod);/*修改房屋信息*/
BOOL DelRoomnode (struct land_node *hd , char * l_id, char * b_id, char * r_id);/*删除房屋结点 */



struct land_node * SeekLandnode(struct land_node * hd, char * l_id);/*查询符合条件的楼盘*/
struct block_node * SeekBlocknode(struct land_node * hd, char * l_id, char * b_id);/* 查询符合条件的楼栋*/
struct room_node * SeekRoomnode(struct land_node * hd, char * l_id, char * b_id,char * r_id);/*查询符合条件的房屋 */

struct land_sta * StTypenode(struct land_node *hd, char * type);/*统计符合条件的房屋类型*/
struct land_node * StAreanode(struct land_node *hd , char * area);/*统计指定城区的楼盘信息*/

BOOL SeekLandArea(struct land_node *hd , char * area, int n ); //寻找符合地区条件的楼盘信息
//查询符合房型的房屋信息
BOOL SeekRoomType(struct land_node * hd, char * type);
//查询符合价格的房屋通讯信息
BOOL SeekRoomPrice(struct land_node * hd, char * h_price, char * l_price);

#endif /**< TYPE_H_INCLUDED*/


