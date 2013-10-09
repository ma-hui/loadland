#include "load.h"

int main(void)
{
    COORD size = {SCR_COL, SCR_ROW};              /*窗口缓冲区大小*/
    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出设备句柄*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* 获取标准输入设备句柄*/
    SetConsoleTitle(gp_sys_name);                 /*设置窗口标题*/
    SetConsoleScreenBufferSize(gh_std_out, size); /*设置窗口缓冲区大小80*25*/
    LoadData();                   /*数据加载*/
    InitInterface();              /*界面初始化*/
    RunSys(&hd_land);             /*系统功能模块的选择及运行*/
    CloseSys(hd_land);            /*退出系统*/
    return 0;
}

/**
 * 函数名称: LoadData
 * 函数功能: 将三类基础数据从数据文件载入到内存缓冲区和十字链表中.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: BOOL类型, 功能函数中除了函数ExitSys的返回值可以为FALSE外,
 *           其他函数的返回值必须为TRUE.
 * 调用说明: 为了能够以统一的方式调用各功能函数, 将这些功能函数的原型设为
 *           一致, 即无参数且返回值为BOOL. 返回值为时, 结束程序运行.
 */
 BOOL LoadData()
{
    int Re = 0;


    gc_sys_state |= 1;

    gc_sys_state |= 2;


    Re = CreateList(&hd_land);
    gc_sys_state |= Re;
    gc_sys_state &= ~(4 + 8 + 16 - Re);
    if (gc_sys_state < (1 | 2 | 4 | 8 | 16))
    {
        /*数据加载提示信息*/
        printf("\n\n\n\t\t系统基础数据不完整!\n");
        printf("\n\t\t按任意键继续...\n");
        getch();
    }
    else {

        printf("\n\n\t\t\t  欢迎进入楼盘管理系统\n");
        printf("\n\t\t\t\t按任意键进入");
        getch();
    }
    return TRUE;
}

/**
 * 函数名称: CreateList
 * 函数功能: 从数据文件读取基础数据, 并存放到所创建的十字链表中.
 * 输入参数: 无
 * 输出参数: hd 主链头指针的地址, 用来返回所创建的十字链.
 * 返 回 值: int型数值, 表示链表创建的情况.
 *           0  空链, 无数据
 *           4  已加载楼盘信息数据，无楼栋信息和房屋信息数据
 *           12 已加载楼盘信息和楼栋基本信息数据，无房屋信息数据
 *           28 三类基础数据都已加载
 *
 * 调用说明:
 */
int  CreateList (struct land_node **hd){
    struct land_node *p_hd = NULL, * p_land = NULL;
    struct block_node *p_block = NULL;
    struct room_node  *p_room = NULL;
    FILE  *p_file = NULL;
    int sign = 0;  /*sign 为标识 0表示没有结点 1所属楼盘结点2 找到所属楼栋结点*/
    int re = 0;

    if ((p_file = fopen(gp_load_info_filename,"r"))== NULL){
        printf("\n\n\n\t\t\t楼盘基础信息文件打开失败");
        return re;
    }
    printf("\n\n\n\n\n\t\t\t楼盘基础信息文件打开成功！！");

     /*从数据文件中读楼盘基础信息数据，存入以后进先出方式建立的主链*/
    while(! feof(p_file)){
        p_land =(struct land_node*)malloc(sizeof(struct land_node));
        p_land -> hd_block = NULL;
        fscanf(p_file,"%s\t%s\t%s\t%s\t%s\t%s\t%s\n",p_land->land_id,p_land->land_name,p_land->land_add,p_land->land_tel, p_land->land_area,p_land->developer,p_land->pro_man_com);
        p_land->next_land = p_hd;
        p_hd=p_land;
    }
    fclose(p_file);

    if(p_hd == NULL){
        printf("\n\t\t\t楼盘基础信息文件加载失败!!");
        return re;
    }
    *hd = p_hd;
    re += 4;

    if ((p_file=fopen(gp_bloc_info_filename,"r"))==NULL){
        printf("\n\t\t\t楼栋基本信息文件打开失败!!");
        return re;
    }
    printf("\n\t\t\t楼栋基本信息文件打开成功！！");
    re += 8;

    /*从数据文件读取楼栋基本信息存入主链对应结点的楼栋基本信息结点中 */
    while (!feof (p_file)){
        p_block = (struct block_node*)malloc(sizeof (struct block_node));
        p_block->hd_room=NULL;
        fscanf(p_file,"%s\t%s\t%s\t%s\t%s\n",p_block->block_id,p_block->land_id,p_block->pool_rote,p_block->unit,p_block->floor);
        p_land = p_hd;

        /*在主链上查找楼栋所在楼盘的主链结点*/
        while(p_land!=NULL && strcmp(p_land->land_id ,p_block->land_id)!=0 )
        {
            p_land = p_land->next_land;
        }

         /*如果找到，则将结点以后进先出的方式插入*/
        if(p_land!=NULL)
        {
            p_block->next_block = p_land->hd_block;
            p_land->hd_block = p_block;

        }
        /*如果没有找到，则释放所创建结点的内存空间*/
        else
        {
            free(p_block);
        }
    }
    fclose(p_file);

    if ((p_file=fopen(gp_room_info_filename,"r"))==NULL){
        printf("\n\t\t\t房屋基础信息文件打开失败!!");
        return re;
    }
    printf("\n\t\t\t房屋基础信息文件打开成功！！");
    re += 16;

    /* 从数据文件中读取楼房基本信息，存入楼房基本信息支链对应结点的楼栋支链中*/
    while (!feof(p_file)){
        p_room = (struct room_node *)malloc(sizeof(struct room_node));
        fscanf (p_file,"%s\t%s\t%s\t%s\t%s\t%s\n",p_room->room_id,p_room->block_id,p_room->land_id,p_room->type,p_room->area,p_room->price);
        p_land = p_hd;
        sign = 0;
        while(p_land && sign!=1) {
            if ( strcmp(p_land->land_id,p_room->land_id)==0 ){
                sign = 1;
            }
            else
                p_land = p_land ->next_land;
        }
        if(p_land!=NULL)
             p_block=p_land->hd_block;

        while(sign==1 && p_block){
            if(strcmp(p_room->block_id,p_block->block_id)!=0){
                p_block = p_block->next_block;
            }
            /*如果找到，则以后进先出的方式插入房屋信息支链中*/
            else{
            p_room->next_room=p_block->hd_room;
            p_block->hd_room=p_room;
            sign=2;
            }
        }
        /*如果没有找到，释放所创建的结点的内存空间*/
        if(sign!=2)
            free(p_room);
    }
    fclose(p_file);
    return re;
}

/**
 * 函数名称: InitInterface
 * 函数功能: 初始化界面.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 调用说明:
 */
void InitInterface()
{
    WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
               | BACKGROUND_BLUE;  /*黄色前景和蓝色背景*/

    SetConsoleTextAttribute(gh_std_out, att);  /*设置控制台屏幕缓冲区字符属性*/

    ClearScreen();  /* 清屏*/

    /*创建弹出窗口信息堆栈，将初始化后的屏幕窗口当作第一层弹出窗口*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*屏幕字符属性*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*弹出窗口的层号为0*/
    gp_top_layer->rcArea.Left = 0;  /*弹出窗口的区域为整个屏幕窗口*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*显示菜单栏*/
    ShowState();    /*显示状态栏*/

    return;
}
/**
 * 函数名称: ClearScreen
 * 函数功能: 清除屏幕信息.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void ClearScreen(void)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );/*取屏幕缓冲区信息*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*计算屏幕缓冲区字符单元数*/

    /*将屏幕缓冲区所有单元的字符属性设置为当前屏幕缓冲区字符属性*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &num_written);

    /*将屏幕缓冲区所有单元填充为空格字符*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &num_written);

    return;
}
/**
 * 函数名称: ShowMenu
 * 函数功能: 在屏幕上显示主菜单, 并设置热区, 在主菜单第一项上置选中标记
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 调用说明:
 */
void ShowMenu()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    COORD size;
    COORD pos = {0, 0};
    int i, j;
    int PosA = 2, PosB;
    char ch;
    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SetConsoleCursorPosition(gh_std_out, pos);
    for (i=0; i < 5; i++) /*在窗口第一行第一列处输出主菜单项*/
    {
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = 0;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*隐藏光标*/

    /*申请动态存储区作为存放菜单条屏幕区字符信息的缓冲区*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu = {0, 0, size.X-1, 0} ;

    /*将窗口第一行的内容读入到存放菜单条屏幕区字符信息的缓冲区中*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*将这一行中英文字母置为红色，其他字符单元置为白底黑字*/
    for (i=0; i<size.X; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*修改后的菜单条字符信息回写到窗口的第一行*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*将光标位置设置在第2行第1列*/

    /*将菜单项置为热区，热区编号为菜单项号，热区类型为0(按钮型)*/
    i = 0;
    do
    {
        PosB = PosA + strlen(ga_main_menu[i]);  /*定位第i+1号菜单项的起止位置*/
        for (j=PosA; j<PosB; j++)
        {
            gp_scr_att[j] |= (i+1) << 2; /*设置菜单项所在字符单元的属性值*/
        }
        PosA = PosB + 4;
        i++;
    }
    while (i<5);

    TagMainMenu(gi_sel_menu);  /*在选中主菜单项上做标记，gi_sel_menu初值为1*/

    return;
}

/**
 * 函数名称: ShowState
 * 函数功能: 显示状态条.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 调用说明: 状态条字符属性为白底黑字, 初始状态无状态信息.
 */
void ShowState()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int i;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SMALL_RECT rcMenu = {0, bInfo.dwSize.Y-1, size.X-1, bInfo.dwSize.Y-1};

    if (gp_buff_stateBar_info == NULL)
    {
        gp_buff_stateBar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
        ReadConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);
    }

    for (i=0; i<size.X; i++)
        (gp_buff_stateBar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                        | BACKGROUND_RED;
    WriteConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);

    return;
}
/**
 * 函数名称: TagMainMenu
 * 函数功能: 在指定主菜单项上置选中标志.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 2, PosB;
    char ch;
    int i;

    if (num == 0) /*num为0时，将会去除主菜单项选中标记*/
    {
        PosA = 0;
        PosB = 0;
    }
    else  /*否则，定位选中主菜单项的起止位置: PosA为起始位置, PosB为截止位置*/
    {
        for (i=1; i<num; i++)
        {
            PosA += strlen(ga_main_menu[i-1]) + 4;
        }
        PosB = PosA + strlen(ga_main_menu[num-1]);
    }

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    /*去除选中菜单项前面的菜单项选中标记*/
    for (i=0; i<PosA; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (gp_buff_menubar_info+i)->Char.AsciiChar;
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*在选中菜单项上做标记，黑底白字*/
    for (i=PosA; i<PosB; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED;
    }
    /*去除选中菜单项后面的菜单项选中标记*/
    for (i=PosB; i<bInfo.dwSize.X; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }
    /*将做好标记的菜单条信息写到窗口第一行*/
    SMALL_RECT rcMenu = {0, 0, size.X-1, 0};
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * 函数名称: PopMenu
 * 函数功能: 弹出指定主菜单项对应的子菜单.
 * 输入参数: num 指定的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 * 调用说明:
 */
void PopMenu(int num)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh;
    int i, j, loc = 0;

    if (num  != gi_sel_menu)       /*如果指定主菜单不是已选中菜单*/
    {
        if (gp_top_layer->LayerNo != 0) /*如果此前已有子菜单弹出*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNo != 0) /*若已弹出该子菜单，则返回*/
    {
        return;
    }

    gi_sel_menu = num;    /*将选中主菜单项置为指定的主菜单项*/
    TagMainMenu(gi_sel_menu); /*在选中的主菜单项上做标记*/
    LocSubMenu(gi_sel_menu, &rcPop); /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += ga_sub_menu_count[i-1];
    }
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel = ga_sub_menu + loc;   /*标签束第一个标签字符串的地址*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*标签束中标签字符串的个数*/
    COORD aLoc[labels.num];/*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    for (i=0; i<labels.num; i++) /*确定标签字符串的输出位置，存放在坐标数组中*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = labels.num;       /*热区的个数，等于标签的个数，即子菜单的项数*/
    SMALL_RECT aArea[areas.num];                    /*定义数组存放所有热区位置*/
    char aSort[areas.num];                      /*定义数组存放所有热区对应类别*/
    char aTag[areas.num];                         /*定义数组存放每个热区的编号*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*热区定位*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*热区类别都为0(按钮型)*/
        aTag[i] = i + 1;           /*热区按顺序编号*/
    }
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/
    pos.X = rcPop.Left + 2;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        /*此循环用来在空串子菜项位置画线形成分隔，并取消此菜单项的热区属性*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) /*串长为0，表明为空串*/
        {
            /*首先画横线*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &num_written);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++)
            {
                /*取消该区域字符单元的热区属性*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*按位与的结果保留了低两位*/
            }
        }

    }
    /*将子菜单项的功能键设为白底红字*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0)
        {
            continue;  /*跳过空串*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &num_written);
    }
    return;
}

/**
 * 函数名称: PopUp
 * 函数功能: 在指定区域输出弹出窗口信息, 同时设置热区, 将弹出窗口位置信息入栈.
 * 输入参数: pRc 弹出窗口位置数据存放的地址
 *           att 弹出窗口区域字符属性
 *           pLabel 弹出窗口中标签束信息存放的地址
             pHotArea 弹出窗口中热区信息存放的地址
 * 输出参数: 无
 * 返 回 值: 无
 * 调用说明:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*弹出窗口所在位置字符单元信息入栈*/
    size.X = pRc->Right - pRc->Left + 1;    /*弹出窗口的宽度*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*弹出窗口的高度*/
    /*申请存放弹出窗口相关信息的动态存储区*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*将弹出窗口覆盖区域的字符信息保存，用于在关闭弹出窗口时恢复原样*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        /*此二重循环将所覆盖字符单元的原先属性值存入动态存储区，便于以后恢复*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }
    gp_top_layer = nextLayer;  /*完成弹出窗口相关信息入栈操作*/
    /*设置弹出窗口区域字符的新属性*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &num_written);
        pos.Y++;
    }
    /*将标签束中的标签字符串在设定的位置输出*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], NULL);
        }
    }
    /*设置弹出窗口区域字符单元的新属性*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        /*此二重循环设置字符单元的层号*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {
        /*此二重循环设置所有热区中字符单元的热区类型和热区编号*/
        row = pHotArea->pArea[i].Top;
        for (j=pHotArea->pArea[i].Left; j<=pHotArea->pArea[i].Right; j++)
        {
            gp_scr_att[row*SCR_COL+j] |= (pHotArea->pSort[i] << 6)
                                         | (pHotArea->pTag[i] << 2);
        }
    }
    return;
}

/**
 * 函数名称: PopOff
 * 函数功能: 关闭顶层弹出窗口, 恢复覆盖区域原外观和字符单元原属性.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopOff(void)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j;

    if ((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL))
    {
        /*栈底存放的主界面屏幕信息，不用关闭*/
        return;
    }
    nextLayer = gp_top_layer->next;
    /*恢复弹出窗口区域原外观*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*恢复字符单元原属性*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }
    free(gp_top_layer->pContent);    /*释放动态存储区*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;
    return;
}

/**
 * 函数名称: DrawBox
 * 函数功能: 在指定区域画边框.
 * 输入参数: pRc 存放区域位置信息的地址
 * 输出参数: 无
 * 返 回 值: 无
 * 调用说明:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+','-','|'};  /*画框用的字符*/
    COORD pos = {pRc->Left, pRc->Top};  /*定位在区域的左上角*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, NULL);/*画边框左上角*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {
        /*此循环画上边框横线*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, NULL);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, NULL);/*画边框右上角*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++)
    {
        /*此循环画边框左边线和右边线*/
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, NULL);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, NULL);
    }
    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, NULL);/*画边框左下角*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {
        /*画下边框横线*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, NULL);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, NULL);/*画边框右下角*/
    return;
}

/**
 * 函数名称: TagSubMenu
 * 函数功能: 在指定子菜单项上做选中标记.
 * 输入参数: num 选中的子菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 * 调用说明:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {
        /*如果子菜单项号越界，或该项子菜单已被选中，则返回*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) /*首先取消原选中子菜单项上的标记*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &num_written);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*白底红字*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &num_written);
    }
    /*在制定子菜单项上做选中标记*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &num_written);
    gi_sel_sub_menu = num;  /*修改选中子菜单项号*/
    return;
}

/**
 * 函数名称: LocSubMenu
 * 函数功能: 计算弹出子菜单区域左上角和右下角的位置.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: rc 存放区域位置信息的地址
 * 返 回 值: 无
 *
 * 调用说明:
 */
void LocSubMenu(int num, SMALL_RECT *rc){
    int i, len, loc = 0;

    rc->Top = 1; /*区域的上边定在第2行，行号为1*/
    rc->Left = 1;
    for (i=1; i<num; i++)
    {
        /*计算区域左边界位置, 同时计算第一个子菜单项在子菜单字符串数组中的位置*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*暂时存放第一个子菜单项字符串长度*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++)
    {
        /*查找最长子菜单字符串，将其长度存放在rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len)
        {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*计算区域的右边界*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*计算区域下边的行号*/
    if (rc->Right >= SCR_COL)  /*右边界越界的处理*/
    {
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}

/**
 * 函数名称: RunSys
 * 函数功能: 运行系统, 在系统主界面下运行用户所选择的功能模块.
 * 输入参数: 无
 * 输出参数: hd主链头指针的地址
 * 返 回 值: 无
 * 调用说明:
 */

void RunSys(struct land_node * *hd){
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:字符单元层号, cAtt:字符单元属性*/
    char vkc, asc;      /*vkc:虚拟键代码, asc:字符的ASCII码值*/

    while (bRet)
    {
        /*从控制台输入缓冲区中读一条记录*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) /*如果记录由鼠标事件产生*/
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*获取鼠标坐标位置*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*取该位置的层号*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*取该字符单元属性*/
            if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
            {
                /* cAtt > 0 表明该位置处于热区(主菜单项字符单元)
                 * cAtt != gi_sel_menu 表明该位置的主菜单项未被选中
                 * gp_top_layer->LayerNo > 0 表明当前有子菜单弹出
                 */
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                    PopMenu(cAtt);       /*弹出鼠标所在主菜单项对应的子菜单*/
                }
            }
            else if (cAtt > 0) /*鼠标所在位置为弹出子菜单的菜单项字符单元*/
            {
                TagSubMenu(cAtt); /*在该子菜单项上做选中标记*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                    == FROM_LEFT_1ST_BUTTON_PRESSED) /*如果按下鼠标左边第一键*/
            {
                if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
                {
                    if (cAtt > 0) /*如果该位置处于热区(主菜单项字符单元)*/
                    {
                        PopMenu(cAtt);   /*弹出鼠标所在主菜单项对应的子菜单*/
                    }
                    /*如果该位置不属于主菜单项字符单元，且有子菜单弹出*/
                    else if (gp_top_layer->LayerNo > 0)
                    {
                        PopOff();            /*关闭弹出的子菜单*/
                        gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                    }
                }
                else /*层号不为0，表明该位置被弹出子菜单覆盖*/
                {
                    if (cAtt > 0) /*如果该位置处于热区(子菜单项字符单元)*/
                    {
                        PopOff(); /*关闭弹出的子菜单*/
                        gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/

                        /*执行对应功能函数:gi_sel_menu主菜单项号,cAtt子菜单项号*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);

                    }
                }
            }
            else if (inRec.Event.MouseEvent.dwButtonState
                     == RIGHTMOST_BUTTON_PRESSED) /*如果按下鼠标右键*/
            {
                if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT  /*如果记录由按键产生*/
                 && inRec.Event.KeyEvent.bKeyDown) /*且键被按下*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*获取按键的虚拟键码*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*获取按键的ASC码*/

            /*系统快捷键的处理*/
            if (vkc == 112) /*如果按下F1键*/
            {
                if (gp_top_layer->LayerNo != 0) /*如果当前有子菜单弹出*/
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                }
                bRet = ExeFunction(5, 1);  /*运行帮助主题功能函数*/
            }
            else if (inRec.Event.KeyEvent.dwControlKeyState
                     & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            {
                /*如果按下左或右Alt键*/
                switch (vkc)  /*判断组合键Alt+字母*/
                {
                case 88:  /*Alt+X 退出*/
                    if (gp_top_layer->LayerNo != 0)
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    bRet = ExeFunction(1,4);
                    break;
                case 70:  /*Alt+F*/
                    PopMenu(1);
                    break;
                case 77: /*Alt+M*/
                    PopMenu(2);
                    break;
                case 81: /*Alt+Q*/
                    PopMenu(3);
                    break;
                case 83: /*Alt+S*/
                    PopMenu(4);
                    break;
                case 72: /*Alt+H*/
                    PopMenu(5);
                    break;
                }
            }
            else if (asc == 0) /*其他控制键的处理*/
            {
                if (gp_top_layer->LayerNo == 0) /*如果未弹出子菜单*/
                {
                    switch (vkc) /*处理方向键(左、右、下)，不响应其他控制键*/
                    {
                    case 37:
                        gi_sel_menu--;
                        if (gi_sel_menu == 0)
                        {
                            gi_sel_menu = 5;
                        }
                        TagMainMenu(gi_sel_menu);
                        break;
                    case 39:
                        gi_sel_menu++;
                        if (gi_sel_menu == 6)
                        {
                            gi_sel_menu = 1;
                        }
                        TagMainMenu(gi_sel_menu);
                        break;
                    case 40:
                        PopMenu(gi_sel_menu);
                        TagSubMenu(1);
                        break;
                    }
                }
                else  /*已弹出子菜单时*/
                {
                    for (loc=0,i=1; i<gi_sel_menu; i++)
                    {
                        loc += ga_sub_menu_count[i-1];
                    }  /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                    switch (vkc) /*方向键(左、右、上、下)的处理*/
                    {
                    case 37:
                        gi_sel_menu--;
                        if (gi_sel_menu < 1)
                        {
                            gi_sel_menu = 5;
                        }
                        TagMainMenu(gi_sel_menu);
                        PopOff();
                        PopMenu(gi_sel_menu);
                        TagSubMenu(1);
                        break;
                    case 38:
                        num = gi_sel_sub_menu - 1;
                        if (num < 1)
                        {
                            num = ga_sub_menu_count[gi_sel_menu-1];
                        }
                        if (strlen(ga_sub_menu[loc+num-1]) == 0)
                        {
                            num--;
                        }
                        TagSubMenu(num);
                        break;
                    case 39:
                        gi_sel_menu++;
                        if (gi_sel_menu > 5)
                        {
                            gi_sel_menu = 1;
                        }
                        TagMainMenu(gi_sel_menu);
                        PopOff();
                        PopMenu(gi_sel_menu);
                        TagSubMenu(1);
                        break;
                    case 40:
                        num = gi_sel_sub_menu + 1;
                        if (num > ga_sub_menu_count[gi_sel_menu-1])
                        {
                            num = 1;
                        }
                        if (strlen(ga_sub_menu[loc+num-1]) == 0)
                        {
                            num++;
                        }
                        TagSubMenu(num);
                        break;
                    }
                }
            }
            else if ((asc-vkc == 0) || (asc-vkc == 32))   /*按下普通键*/
            {
                if (gp_top_layer->LayerNo == 0)  /*如果未弹出子菜单*/
                {
                    switch (vkc)
                    {
                    case 70: /*f或F*/
                        PopMenu(1);
                        break;
                    case 77: /*m或M*/
                        PopMenu(2);
                        break;
                    case 81: /*q或Q*/
                        PopMenu(3);
                        break;
                    case 83: /*s或S*/
                        PopMenu(4);
                        break;
                    case 72: /*h或H*/
                        PopMenu(5);
                        break;
                    case 13: /*回车*/
                        PopMenu(gi_sel_menu);
                        TagSubMenu(1);
                        break;
                    }
                }
                else /*已弹出子菜单时的键盘输入处理*/
                {
                    if (vkc == 27) /*如果按下ESC键*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    else if(vkc == 13) /*如果按下回车键*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    }
                    else /*其他普通键的处理*/
                    {
                        /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++)
                        {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*依次与当前子菜单中每一项的代表字符进行比较*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++)
                        {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1])
                            {
                                /*如果匹配成功*/
                                PopOff();
                                gi_sel_sub_menu = 0;
                                bRet = ExeFunction(gi_sel_menu, i-loc+1);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * 函数名称: ExeFunction
 * 函数功能: 执行由主菜单号和子菜单号确定的功能函数.
 * 输入参数: m 主菜单项号
 *           s 子菜单项号
 * 输出参数: 无
 * 返 回 值: BOOL类型,TRUE或FALSE
 * 调用说明: 仅在执行函数ExitSys时, 才可能返回FALSE, 其他情况下总是返回TRUE
 */
BOOL ExeFunction(int m, int s)
{
    BOOL  bRet = TRUE;
    /*函数指针数组，用来存放所有功能函数的入口地址*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*将功能函数入口地址存入与功能函数所在主菜单号和子菜单号对应下标的数组元素*/
    pFunction[0] = SaveData;				/**保存数据*/
    pFunction[1] = BackupData;              /**备份数据*/
    pFunction[2] = RestoreData;             /**恢复数据*/
    pFunction[3] = NULL;
    pFunction[4] = ExitSys;                 /**退出系统*/
    pFunction[5] = Maintainl_Info;			/**维护楼盘信息*/
    pFunction[6] = Maintainb_Info;          /**维护楼栋信息*/
    pFunction[7] = Maintainr_Info;          /**维护房屋信息*/
    pFunction[8] = Queryl_Info;             /**查询楼盘信息*/
    pFunction[9] = Queryb_Info;             /**查询楼栋信息*/
    pFunction[10] = Queryr_Info;            /**查询房屋信息*/
    pFunction[11] = QueryLandArea;          /**按照地区查询楼盘信息*/
    pFunction[12] = QueryRoomType;           /**按照房屋类型查询通讯信息*/
    pFunction[13] = QueryRoomPrice;          /**按照房屋价格查询通讯信息*/
    pFunction[14] = NULL;
    pFunction[15] = Queryap;                 /**组合条件查找*/
    pFunction[16] = Stat_a_Info;	        /**按照楼盘所在的城区统计*/
    pFunction[17] = Stat_t_Info;	        /**按照房屋的户型统计*/
    pFunction[18] = HelpTopic;	 	        /**帮助*/
    pFunction[19] = AboutDorm;              /**关于*/

    for (i=1,loc=0; i<m; i++)  /*根据主菜单号和子菜单号计算对应下标*/
    {
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL)
    {
        bRet = (*pFunction[loc])();  /*用函数指针调用所指向的功能函数*/
    }

    return bRet;
}

/**
 * 函数名称: SaveData
 * 函数功能: 保存数据的功能函数.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE 或 FALSE
 * 调用说明: 仅在执行函数ExitSys时, 才可能返回FALSE, 其他情况下总是返回TRUE
 */
BOOL SaveData(void)
{
    InitInterface();
    BOOL bret = TRUE;
    struct land_node *p_l;
    struct block_node *p_b;
    struct room_node  *p_r;
    FILE *pfout=NULL;
    pfout=fopen(gp_load_info_filename,"w+");

    for(p_l = hd_land ; p_l != NULL; p_l = p_l -> next_land){

        fprintf(pfout,"%s\t%s\t%s\t%s\t%s\t%s\t%s\n",p_l->land_id,p_l->land_name,p_l->land_add,p_l->land_tel,p_l->land_area,p_l->developer,p_l->pro_man_com);
    }

    fclose(pfout);


    pfout=fopen(gp_bloc_info_filename,"w+");
    for(p_l=hd_land;p_l!=NULL;p_l=p_l->next_land){

        p_b=p_l->hd_block;
        while(p_b!=NULL)
        {
            fprintf(pfout,"%s\t%s\t%s\t%s\t%s\n",p_b->block_id,p_b->land_id,p_b->pool_rote,p_b->unit,p_b->floor);
            p_b=p_b->next_block;
        }
    }
    fclose(pfout);

    pfout=fopen(gp_room_info_filename,"w+");
    for(p_l=hd_land;p_l!=NULL;p_l=p_l->next_land)
    {
        p_b=p_l->hd_block;
        while(p_b!=NULL)
        {
            p_r=p_b->hd_room;
            while(p_r!=NULL)
            {
                fprintf(pfout,"%s\t%s\t%s\t%s\t%s\t%s\n",p_r->room_id,p_r->block_id,p_r->land_id,p_r->type,p_r->area,p_r->price);
                p_r = p_r ->next_room;
            }
            p_b = p_b->next_block;
        }
    }

    fclose(pfout);
    Success();
    return bret;
}

BOOL BackupData(void)                            /*备份数据，将三类基础数据备份到一个数据文件*/
{
    BOOL bRet = TRUE;
    struct land_node *p_l;
    struct block_node *p_b;
    struct room_node *p_r;
    unsigned long l_num = 0;
    unsigned long b_num = 0;
    unsigned long r_num = 0;
    FILE *fhandle;

    /*遍历十字表，分别统计三种基础数据信息的记录总数*/
    for(p_l = hd_land; p_l != NULL; p_l = p_l ->next_land){
        l_num++;
        p_b = p_l -> hd_block;
        while(p_b != NULL)
        {
            b_num++;
            p_r = p_b -> hd_room;
            while(p_r != NULL){

                r_num++;
                p_r = p_r->next_room;
            }
            p_b = p_b->next_block;
        }
    }

    fhandle = fopen(gp_filename,"w+");

    /*保存三类基础数据的记录总数*/
    fprintf(fhandle,"%ld\t",l_num);
    fprintf(fhandle,"%ld\t",b_num);
    fprintf(fhandle,"%ld\n",r_num);

	for(p_l = hd_land; p_l != NULL; p_l = p_l -> next_land){

        fprintf(fhandle,"%s\t%s\t%s\t%s\t%s\t%s\t%s\n",p_l->land_id,p_l->land_name,p_l->land_add,p_l->land_tel,p_l->land_area,p_l->developer,p_l->pro_man_com);
    }
    for(p_l = hd_land; p_l != NULL; p_l = p_l ->next_land){

        p_b = p_l ->hd_block;
        while(p_b != NULL)
        {
            fprintf(fhandle,"%s\t%s\t%s\t%s\t%s\n",p_b->block_id,p_b->land_id,p_b->pool_rote,p_b->unit,p_b->floor);
            p_b = p_b ->next_block;
        }
    }
    for(p_l = hd_land; p_l != NULL; p_l = p_l ->next_land){

        p_b = p_l ->hd_block;
        while(p_b != NULL){

            p_r = p_b ->hd_room;
            while(p_r != NULL)
            {
                fprintf(fhandle,"%s\t%s\t%s\t%s\t%s\t%s\n",p_r->room_id,p_r->block_id,p_r->land_id,p_r->type,p_r->area,p_r->price);
                p_r = p_r ->next_room;
            }
            p_b = p_b ->next_block ;
        }
    }

    fclose(fhandle);
    Success();
    return bRet;
}

BOOL RestoreData(void)                          /*恢复数据*/
{
    InitInterface();
    BOOL bRet = TRUE;
    struct land_node  *hd = NULL;
    struct land_node  *p_l;
    struct block_node *p_b;
    struct room_node  *p_r;
    unsigned long l_num = 0;
    unsigned long b_num = 0;
    unsigned long r_num = 0;
    unsigned long ulloop;
    // int handle;
    FILE *fhandle;
    int find;
    /*读取三种基础信息的记录数*/
    fhandle = fopen(gp_filename,"r+");
    /*读取楼盘信息，建立十字链主链*/
    fscanf(fhandle,"%ld",&l_num);
    fgetc(fhandle);
    fscanf(fhandle,"%ld",&b_num);
    fgetc(fhandle);
    fscanf(fhandle,"%ld",&r_num);
    fgetc(fhandle);
    for(ulloop = 1; ulloop <= l_num; ulloop++)
    {
        p_l = (struct land_node* )malloc(sizeof(struct land_node ));
        p_l -> hd_block = NULL;
        fscanf(fhandle,"%s\t%s\t%s\t%s\t%s\t%s\t%s\n",p_l->land_id,p_l->land_name,p_l->land_add,p_l->land_tel,p_l->land_area,p_l->developer,p_l->pro_man_com);

        p_l -> next_land = hd;
        hd = p_l;
    }
    hd_land = hd;
    /*读取楼栋信息，建立楼栋信息支链*/
    for(ulloop = 1; ulloop <= b_num; ulloop++)
    {
        p_b = (struct block_node * )malloc(sizeof(struct block_node ));
        p_b -> hd_room= NULL;
        fscanf(fhandle,"%s\t%s\t%s\t%s\t%s\n",p_b->block_id,p_b->land_id,p_b->pool_rote,p_b->unit,p_b->floor);

        p_l = hd;
        while(p_l != NULL && strcmp(p_l ->land_id,p_b ->land_id) != 0)
        {
            p_l = p_l -> next_land;
        }

        if(p_l != NULL)
        {
            p_b -> next_block = p_l->hd_block;
            p_l -> hd_block = p_b;
        }
        else
        {
            free(p_b);
        }
    }

    /*读取房屋信息，建立房屋信息支链*/
    for(ulloop = 1; ulloop <= r_num; ulloop++)
    {
        p_r = (struct room_node *)malloc(sizeof(struct room_node));
        fscanf(fhandle,"%s\t%s\t%s\t%s\t%s\t%s\n",p_r->room_id,p_r->block_id,p_r->land_id,p_r->type,p_r->area,p_r->price);
        p_l = hd;
        find = 0;

        while(p_l != NULL && find == 0)
        {
            p_b = p_l -> hd_block;
            while(p_b != NULL && find ==0)
            {
                if((strcmp(p_b->land_id,p_r->land_id)&&strcmp(p_b->block_id,p_r->block_id))==0)
                {
                    find = 1;
                    break;
                }
                p_b = p_b ->next_block;
            }
            p_l = p_l ->next_land;
        }

        if(find)
        {
            p_r ->next_room = p_b->hd_room;
            p_b ->hd_room = p_r;
        }
        else
        {
            free(p_r);
        }
    }
    fclose(fhandle);
    SaveData();                         /*将内存中的数据保存到数据文件*/
    Success();
    return bRet;
}

BOOL ExitSys(void)
{
    InitInterface();
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"确认退出系统吗？", "确定      取消"};
    int iHot = 1;
    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
        {rcPop.Left+5, rcPop.Top+5}
    };
    labels.pLoc = aLoc;
    areas.num = 2;
    SMALL_RECT aArea[] = {{
            rcPop.Left + 5, rcPop.Top + 5,
            rcPop.Left + 8, rcPop.Top + 5
        },
        {
            rcPop.Left + 13, rcPop.Top + 5,
            rcPop.Left + 16, rcPop.Top + 5
        }
    };
    char aSort[] = {0, 0};
    char aTag[] = {1, 2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &num_written);
    DrawBox(&rcPop);
    if (DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
    }

    PopOff();

    return bRet;
}

/**
 * 函数名称: CloseSys
 * 函数功能: 关闭系统.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 * 调用说明:
 */
void CloseSys(struct land_node *hd){
    struct land_node * p_l;
    struct block_node * p_b, *p_b_n;
    struct room_node *p_r, *p_r_n;
    while(hd ){
        p_l = hd->next_land;                                /* 释放十字交叉链表所申请 的动态储存区域*/
        p_b=hd->hd_block;

        while (p_b){                               /* 释放楼栋结点的动态储存区域*/
            p_b_n=p_b->next_block;
            p_r = p_b->hd_room;

            while (p_r){                                 /* 释放房屋结点的动态储存区域*/
                p_r_n = p_r -> next_room;
                free(p_r);
                p_r=p_r_n;
            }

            free(p_b);
            p_b=p_b_n;
        }
                                                                /* 释放楼盘结点的动态储存区域*/
        free(hd);
        hd = p_l;
    }
    ClearScreen();        /*清屏*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);
    /*关闭标准输入和输出设备句柄*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);
    /*将窗口标题栏置为运行结束*/
    SetConsoleTitle("运行结束");
    return;
}

BOOL Maintainl_Info(void)                     /**维护楼盘信息*/
{
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1、删除某楼盘信息", "2、插入某楼盘信息","3、修改某楼盘信息","返回"};
    char *pCh2[] = {"请输入：   楼盘编号","开始输入","取消"};
    char *pCh3[] = {"请输入： a.楼盘编号          b.楼盘名称         c.楼盘地址         d.联系电话            e.所属城区       f.开发商           g.物业公司","开始输入","取消"};
    char *pCh4[] = {"请输入： a.要修改的楼盘信息的编号   ","请输入新的信息：b.楼盘名称      c.楼盘地址     d.联系电话    e. 所属城区   ","             f.开发商      g.物业公司 ","开始输入","取消"};
    int iHot = 4;/*热键所在行数*/
    //弹框位置
    pos.X = strlen(pCh[0]) +12;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 4;//行数
    labels.ppLabel = pCh;
    //字的位置
    COORD aLoc[] = {{rcPop.Left+6, rcPop.Top+2},
        {rcPop.Left+6, rcPop.Top+5},
        {rcPop.Left+6, rcPop.Top+8},
        {rcPop.Left+10, rcPop.Top+12}
    };
    labels.pLoc = aLoc;
    areas.num = 4;
    //热区区域
    SMALL_RECT aArea[] =
    {
        {
            rcPop.Left+6, rcPop.Top+2,
            rcPop.Left+22, rcPop.Top+2
        },
        {
            rcPop.Left + 6, rcPop.Top + 5,
            rcPop.Left +22, rcPop.Top +5
        },
        {
            rcPop.Left + 6, rcPop.Top + 8,
            rcPop.Left +22, rcPop.Top + 8
        },
        {
            rcPop.Left + 10, rcPop.Top + 12,
            rcPop.Left + 13, rcPop.Top + 12
        }
    };
    char aSort[] = {0, 0,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);//形成弹窗
    DrawBox(&rcPop);
    if (DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
        /*删除某楼盘信息*/
        PopOff();
        iHot = 2;
        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = 0;
        rcPop.Right = SCR_COL-1;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;
        att = BACKGROUND_BLUE| BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        labels.num = 3;
        labels.ppLabel = pCh2;
        COORD aLoc2[] =
        {
            {rcPop.Left+0, rcPop.Top+1},
            {rcPop.Left+28, rcPop.Top+13},
            {rcPop.Left+46, rcPop.Top+13}
        };
        labels.pLoc = aLoc2;

        areas.num = 2;
        SMALL_RECT aArea2[] =
        {
            {
                rcPop.Left+26, rcPop.Top+13,
                rcPop.Left+36, rcPop.Top+13
            },
            {
                rcPop.Left + 46, rcPop.Top + 13,
                rcPop.Left +49, rcPop.Top +13
            }
        };
        char aSort2[] = {0, 0};
        char aTag2[] = {1, 2};
        areas.pArea = aArea2;
        areas.pSort = aSort2;
        areas.pTag = aTag2;
        PopUp(&rcPop, att, &labels, &areas);
    if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
        {
            /*开始执行查找和删除操作*/
            CONSOLE_CURSOR_INFO lp,*plp = &lp;
            lp.dwSize = 1;
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
            char l_id[90];
            scanf("%s",l_id);
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
            PopOff();
            if(DelLandnode(&hd_land,l_id))
            {
                Success();
            }
            else
            {
                Fail();
            }
        }
        else
        {
            PopOff();
        }
    }
	else
    {
        if(iHot == 2)
        {
            /*插入某楼盘信息*/
            PopOff();
            iHot = 2;
            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right = SCR_COL-1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;
            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
            labels.num = 3;
            labels.ppLabel = pCh3;
            COORD aLoc3[] =
            {
                {rcPop.Left+0, rcPop.Top+1},
                {rcPop.Left+28, rcPop.Top+13},
                {rcPop.Left+46, rcPop.Top+13}
            };
            labels.pLoc = aLoc3;
            areas.num = 2;
            SMALL_RECT aArea3[] =
            {
                {
                    rcPop.Left+26, rcPop.Top+13,
                    rcPop.Left+36, rcPop.Top+13
                },
                {
                    rcPop.Left + 46, rcPop.Top + 13,
                    rcPop.Left +49, rcPop.Top +13
                }
            };
            char aSort2[] = {0, 0};
            char aTag2[] = {1, 2};
            areas.pArea = aArea3;
            areas.pSort = aSort2;
            areas.pTag = aTag2;
            PopUp(&rcPop, att, &labels, &areas);
            if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
            {
                /*开始执行查找和插入操作*/
                CONSOLE_CURSOR_INFO lp,*plp = &lp;
                lp.dwSize = 1;
                lp.bVisible = TRUE;
                SetConsoleCursorInfo(gh_std_out,plp);
                SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
                struct land_node *p_l = (struct land_node*)malloc(sizeof(struct land_node));
                scanf("%s%s%s%s%s%s%s",p_l->land_id,p_l->land_name,p_l->land_add,p_l->land_tel,p_l->land_area,p_l->developer,p_l->pro_man_com);
                lp.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out,plp);
                PopOff();
                if(AddLandnode(&hd_land, p_l))
                {
                    Success();
                }
                else
                {
                   free(p_l);
                   Fail();
                }
            }
            else
            {
                PopOff();
            }
        }
        else
        {
            if(iHot == 3)
            {
                /*修改某楼盘信息*/
                PopOff();
                iHot = 2;
                pos.X = 30;
                pos.Y = 14;
                rcPop.Left = 0;
                rcPop.Right = SCR_COL-1;
                rcPop.Top = (25 - pos.Y) / 2;
                rcPop.Bottom = rcPop.Top + pos.Y - 1;
                att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
                labels.num = 5;
                labels.ppLabel = pCh4;
                COORD aLoc3[] =
                {
                    {rcPop.Left+0, rcPop.Top+1},
                    {rcPop.Left+0, rcPop.Top+2},
                    {rcPop.Left+0, rcPop.Top+3},
                    {rcPop.Left+28, rcPop.Top+13},
                    {rcPop.Left+46, rcPop.Top+13}
                };
                labels.pLoc = aLoc3;
                areas.num = 2;
                SMALL_RECT aArea3[] =
                {
                    {
                        rcPop.Left+26, rcPop.Top+13,
                        rcPop.Left+36, rcPop.Top+13
                    },
                    {
                        rcPop.Left + 46, rcPop.Top + 13,
                        rcPop.Left +49, rcPop.Top +13
                    }
                };
                char aSort2[] = {0, 0};
                char aTag2[] = {1, 2};
                areas.pArea = aArea3;
                areas.pSort = aSort2;
                areas.pTag = aTag2;
                PopUp(&rcPop, att, &labels, &areas);
                if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
                {
                    /*开始执行查找和修改操作*/
                    CONSOLE_CURSOR_INFO lp,*plp = &lp;
                    lp.dwSize = 1;
                    lp.bVisible = TRUE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    SetConsoleCursorPosition(gh_std_out,(COORD)
                    {
                        rcPop.Left+0, rcPop.Top+4
                    });
                    struct land_node *p_l= (struct land_node *)malloc(sizeof(struct land_node));
                    scanf("%s%s%s%s%s%s%s",p_l->land_id,p_l->land_name,p_l->land_add,p_l->land_tel,p_l->land_area,p_l->developer,p_l->pro_man_com);
                    lp.bVisible = FALSE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    PopOff();
                    if( ModifLandnode(hd_land,p_l->land_id,p_l))
                    {
                        Success();
                    }
                    else
                    {
                        free(p_l);
                        Fail();
                    }
                }
                else
                {
                    PopOff();
                }
            }
            else
            {
                PopOff();
            }
        }
    }
    PopOff();
    return bRet;
}

/**
* 函数名称: DelLandnode
* 函数功能: 删除指定的楼盘信息结点
* 输入参数: hd主链头指针地址
*           l_id指定的楼盘编号
* 输出参数: 无
* 返 回 值: BOOL类型, TRUE表示修改成功, FALSE表示修改失败
* 调用说明:
*/

/**删除楼盘结点 */
BOOL DelLandnode(struct land_node * *hd, char * l_id) {
    struct land_node  *prior_l = NULL, *p_l = *hd;
    struct block_node *p_b, *p_b_n;
    struct room_node  *p_r, *p_r_n;
    int sign = 0;
    BOOL  bRet= FALSE;
    /* 判断是否为头指针符合条件*/
    if (strcmp(l_id,p_l->land_id)==0){
        *hd = p_l -> next_land;
        sign = 1;
    }
    /*找到符合条件的非头指针的楼盘结点的位置*/
    else {
        while(p_l && strcmp(p_l->land_id,l_id) != 0){
            prior_l = p_l;
            p_l = p_l -> next_land ;
        }
        if(p_l){
            prior_l -> next_land = p_l -> next_land;
            sign = 1;
        }
    }
    /* sign 作为标识，p_l指向要被删除的land_node的结点*/
    /* 删除p_l所指向的结点的block支链以及room支链的储存空间*/
    if (sign){
        bRet = TRUE;
        p_b = p_l -> hd_block;
        while(p_b){
            p_b_n = p_b->next_block;
            p_r = p_b->hd_room;
            while(p_r){
                p_r_n = p_r->next_room;
                free(p_r);
                p_r = p_r_n;
            }
            free(p_b);
            p_b = p_b_n;
        }
        free (p_l);
    }
    else
        bRet = FALSE;
    return  bRet;
}

/**
* 函数名称: ModifLandnode
* 函数功能: 修改指定的楼盘信息结点内容
* 输入参数: hd 主链头指针
*            l_id 指定的楼盘编号
*            p_mod指向存放插入结点的指针
* 输出参数: 无
* 返 回 值: BOOL类型, TRUE表示修改成功, FALSE表示修改失败
* 调用说明:
*/

BOOL ModifLandnode(struct land_node *hd,char *l_id, struct land_node *p_mod){
    struct land_node *l_p = NULL, *l_p_n=NULL;
    l_p = SeekLandnode(hd,l_id);
    if(l_p){
        l_p_n = l_p->next_land;
        p_mod ->hd_block = l_p ->hd_block;  //赋值前保存指针域
        * l_p = * p_mod;
        l_p ->next_land = l_p_n;
        return TRUE;
    }
    else{
        return FALSE;
    }
}
/**
* 函数名称: AddLandnode
* 函数功能: 向十字链表中插入楼盘信息结点
* 输入参数: hd主链头指针地址
*            p_add指向存放插入结点的指针
* 输出参数: 无
* 返 回 值: BOOL类型, 总是为TURE
*
* 调用说明:
*/

/**增加楼盘结点 */
BOOL AddLandnode(struct land_node  **hd, struct land_node * p_add){
     struct land_node * p_l = *hd;
     p_add -> next_land = p_l;
     p_l = p_add;
     *hd = p_l;
     return TRUE;
}

BOOL Maintainb_Info(void){                     /**维护楼栋信息*/
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1、删除某楼栋信息", "2、插入某楼栋信息","3、修改某楼栋信息","返回"};
    char *pCh2[] = {"请输入：  楼盘编号      楼栋编号 ","开始输入","取消"};
    char *pCh3[] = {"请输入：  a.楼栋编号        b.楼盘编号        c.公摊率                d.楼层            e.单元","开始输入","取消"};
    char *pCh4[] = {"请输入原楼栋的信息：  a.楼栋编号                   b.楼盘编号    ","请输入要修改的信息：  c.公摊率        d.楼层        e.单元"," ","开始输入","取消"};
    int iHot = 4;
    pos.X = strlen(pCh[0]) +11;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 4;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+6, rcPop.Top+2},
        {rcPop.Left+6, rcPop.Top+5},
        {rcPop.Left+6, rcPop.Top+8},
        {rcPop.Left+10, rcPop.Top+12}
    };
    labels.pLoc = aLoc;
    areas.num = 4;
    SMALL_RECT aArea[] =
    {
        {
            rcPop.Left+6, rcPop.Top+2,
            rcPop.Left+22, rcPop.Top+2
        },
        {
            rcPop.Left + 6, rcPop.Top + 5,
            rcPop.Left +22, rcPop.Top +5
        },
        {
            rcPop.Left + 6, rcPop.Top + 8,
            rcPop.Left +22, rcPop.Top + 8
        },
        {
            rcPop.Left + 10, rcPop.Top + 12,
            rcPop.Left +13, rcPop.Top + 12
        }
    };
    char aSort[] = {0, 0,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);
    if (DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
        /*删除某楼栋信息*/
        PopOff();
        iHot = 2;
        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = 0;
        rcPop.Right = SCR_COL-1 ;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        labels.num = 3;
        labels.ppLabel = pCh2;
        COORD aLoc2[] =
        {
            {rcPop.Left+0, rcPop.Top+1},
            {rcPop.Left+26, rcPop.Top+13},
            {rcPop.Left+46, rcPop.Top+13}
        };
        labels.pLoc = aLoc2;
        areas.num = 2;
        SMALL_RECT aArea2[] =
        {
            {
                rcPop.Left+28, rcPop.Top+13,
                rcPop.Left+36, rcPop.Top+13
            },
            {
                rcPop.Left + 46, rcPop.Top + 13,
                rcPop.Left +49, rcPop.Top +13
            }
        };
        char aSort2[] = {0, 0};
        char aTag2[] = {1, 2};
        areas.pArea = aArea2;
        areas.pSort = aSort2;
        areas.pTag = aTag2;
        PopUp(&rcPop, att, &labels, &areas);
        if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
        {
            /*开始执行查找和删除操作*/
            CONSOLE_CURSOR_INFO lp,*plp = &lp;
            lp.dwSize = 1;
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
            char ld_id[NUM];
            char bk_id[NUM];
            scanf("%s%s",ld_id,bk_id);
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
            PopOff();
            if(DelBlocknode(hd_land,ld_id,bk_id))
            {
                Success();
            }
            else
            {
                Fail();
            }
        }
        else
        {
            PopOff();
        }
    }
    else
    {
        if(iHot == 2)
        {
            /*插入某楼栋信息结点*/
            PopOff();
            iHot = 2;
            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right =SCR_COL-1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;
            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
            labels.num = 4;
            labels.ppLabel = pCh3;
            COORD aLoc3[] =
            {
                {rcPop.Left+0, rcPop.Top+1},
                {rcPop.Left+26, rcPop.Top+13},
                {rcPop.Left+46, rcPop.Top+13}
            };
            labels.pLoc = aLoc3;
            areas.num = 2;
            SMALL_RECT aArea3[] =
            {
                {
                    rcPop.Left+28, rcPop.Top+13,
                    rcPop.Left+36, rcPop.Top+13
                },
                {
                    rcPop.Left + 46, rcPop.Top + 13,
                    rcPop.Left +49, rcPop.Top +13
                }
            };
            char aSort2[] = {0, 0};
            char aTag2[] = {1, 2};
            areas.pArea = aArea3;
            areas.pSort = aSort2;
            areas.pTag = aTag2;
            PopUp(&rcPop, att, &labels, &areas);
            if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
            {
                /*开始执行查找和插入操作*/
                CONSOLE_CURSOR_INFO lp,*plp = &lp;
                lp.dwSize = 1;
                lp.bVisible = TRUE;
                SetConsoleCursorInfo(gh_std_out,plp);
                SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
                struct block_node * p_b_2 = (struct block_node *)malloc(sizeof(struct block_node));
                scanf("%s%s%s%s%s",p_b_2 -> block_id,p_b_2->land_id,p_b_2->pool_rote,p_b_2->unit,p_b_2->floor);
                lp.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out,plp);
                PopOff();
                if(AddBlocknode(hd_land,p_b_2) )
                {
                    Success();
                }
                else
                {
                    free(p_b_2);
                    Fail();
                }
            }
            else
            {
                PopOff();
            }
        }
        else
        {
            if(iHot == 3)
            {
                /*修改某楼栋信息*/
                PopOff();
                iHot = 2;
                pos.X = 48;
                pos.Y = 15;
                rcPop.Left = 0;
                rcPop.Right = SCR_COL-1;
                rcPop.Top = (25 - pos.Y) / 2;
                rcPop.Bottom = rcPop.Top + pos.Y - 1;
                att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
                labels.num = 5;
                labels.ppLabel = pCh4;
                COORD aLoc3[] =
                {
                    {rcPop.Left+0, rcPop.Top+1},
                    {rcPop.Left+0, rcPop.Top+2},
                    {rcPop.Left+0, rcPop.Top+3},
                    {rcPop.Left+28, rcPop.Top+14},
                    {rcPop.Left+46, rcPop.Top+14}
                };
                labels.pLoc = aLoc3;
                areas.num = 2;
                SMALL_RECT aArea3[] =
                {
                    {
                        rcPop.Left+26, rcPop.Top+14,
                        rcPop.Left+33, rcPop.Top+13
                    },
                    {
                        rcPop.Left + 46, rcPop.Top + 14,
                        rcPop.Left +49, rcPop.Top +13
                    }
                };
                char aSort2[] = {0, 0};
                char aTag2[] = {1, 2};
                areas.pArea = aArea3;
                areas.pSort = aSort2;
                areas.pTag = aTag2;
                PopUp(&rcPop, att, &labels, &areas);
                if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
                {
                    /*开始执行查找和修改操作*/
                    CONSOLE_CURSOR_INFO lp,*plp = &lp;
                    lp.dwSize = 1;
                    lp.bVisible = TRUE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    SetConsoleCursorPosition(gh_std_out,(COORD)
                    {
                        rcPop.Left+0, rcPop.Top+4
                    });
                    struct block_node * p_b_1= (struct block_node *)malloc (sizeof(struct block_node));
                    p_b_1 ->hd_room = NULL;
                    scanf("%s%s%s%s%s",p_b_1->block_id,p_b_1->land_id,p_b_1->pool_rote,p_b_1->floor,p_b_1->unit);
                    lp.bVisible = FALSE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    PopOff();
                    if(ModifBlocknode(hd_land,p_b_1->land_id,p_b_1->block_id,p_b_1))
                    {
                        Success();
                    }
                    else
                    {
                        free(p_b_1);
                        Fail();
                    }
                }
                else
                {
                    PopOff();
                }
            }
            else
            {
                PopOff();
            }
        }
    }
    PopOff();
    return bRet;
}

/**
 * 函数名称:  AddBlocknode
 * 函数功能: 向十字链表中插入楼栋信息结点
 * 输入参数: hd 主链头指针
 *            p_add 指向存放插入结点的指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示修改成功, FALSE表示修改失败
 * 调用说明:
 */
 /**增加楼栋结点*/
BOOL AddBlocknode(struct land_node *hd , struct block_node * p_add){
    struct land_node * p_l = NULL;
    p_l = SeekLandnode(hd,p_add->land_id);
    if (p_l){
        p_add ->next_block = p_l ->hd_block;
        p_l ->hd_block = p_add;
        return  TRUE;
    }
    else {
        free (p_add);
        return FALSE;
    }
}


/**
 * 函数名称:DelBlocknode
 * 函数功能: 对指定的楼栋信息结点进行删除操作包括其下的房屋信息.
 * 输入参数: hd 主链头指针
 *            l_id 楼盘编号  b_id楼栋编号
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示删除成功, FALSE表示删除失败
 *
 * 调用说明:
 */
 /**删除楼栋结点*/
BOOL DelBlocknode(struct land_node *hd , char * l_id, char * b_id){
    struct land_node *p_l;
    struct block_node * prior_b = NULL, *p_b = NULL;
    struct room_node * p_r, *p_r_n;
    int  sign = 0;
    p_l = SeekLandnode(hd,l_id);
    if(p_l){
       p_b =  p_l ->hd_block;
       if(strcmp(p_b->block_id,b_id)==0){
            p_l -> hd_block = p_b->next_block;
            sign = 1;
       }
       else {
        while(p_b && strcmp(b_id,p_b->block_id) != 0){
            prior_b = p_b;
            p_b = p_b ->next_block;
        }
        if(p_b){
            prior_b->next_block = p_b ->next_block;
            sign = 1;
        }

       }
       if (sign){                     //释放楼栋下的房屋支链的储存空间
           p_r = p_b ->hd_room;
           while(p_r){
           p_r_n = p_r->next_room;
           free(p_r);
           p_r = p_r_n;
          }
          free(p_b);
          return TRUE;
        }
    }
    return FALSE;
}
/**
* 函数名称: ModifBlocknode
* 函数功能: 对指定的楼栋信息结点内容进行修改.
* 输入参数: hd 主链头指针
*            l_id楼盘编号 b_id 楼栋编号
*            p_mod 指向存放修改内容结点的指针
* 输出参数: 无
* 返 回 值: BOOL类型, TRUE表示修改成功, FALSE表示修改失败
* 调用说明:
*/
/**修改楼栋信息*/
BOOL ModifBlocknode(struct land_node *hd, char *l_id, char *b_id,struct block_node * p_mod){
    struct block_node * p_b = NULL, *p_b_n = NULL;
    p_b = SeekBlocknode(hd,l_id,b_id);  //查找符合指定信息的楼栋结点
    if(p_b){  //如果找到，则进行修改
        p_b_n = p_b ->next_block;
        p_mod ->hd_room = p_b ->hd_room;                                        //赋值前保存指针域
        * p_b = * p_mod;
        p_b ->next_block = p_b_n;               //恢复指向关系
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/**
 * 函数名称: SeekLandnode
 * 函数功能: 按楼盘编号查找楼盘信息结点.
 * 输入参数: hd 主链头指针
 *            l_id 楼盘编号
 * 输出参数: 无
 * 返 回 值: 查中时返回结点的地址, 否则返回NULL
 * 调用说明:
 */
/**查询符合条件的楼盘*/
struct land_node * SeekLandnode(struct land_node * hd, char * l_id){
        while( hd && strcmp(hd->land_id,l_id)!=0){
            hd = hd ->next_land;
        }
        if(hd){
            return hd;
        }
        else
            return NULL;
}


/**
 * 函数名称: SeekBlocknode
 * 函数功能: 按楼栋编号，楼盘编号查找楼栋信息结点.
 * 输入参数: hd主链头指针
 *            l_id 楼盘编号 b_id楼栋编号
 * 输出参数: 无
 * 返 回 值: 查中时返回结点的地址, 否则返回NULL
 * 调用说明:
 */

struct block_node * SeekBlocknode(struct land_node * hd, char * l_id, char * b_id){
    struct block_node * p_b = NULL;
    while(hd && strcmp(l_id, hd ->land_id)!= 0){
        hd = hd ->next_land;
    }
    if(hd){
        p_b = hd ->hd_block;
        while( p_b && strcmp (b_id, p_b ->block_id) != 0){
            p_b = p_b ->next_block;
        }
        if (p_b){
            return p_b;
        }
    }
    return NULL;
}


/**
 * 函数名称: SeekRoomnode
* 函数功能: 按楼栋编号，楼盘编号，房屋编号查找房屋信息结点.
 * 输入参数: hd主链头指针
 *           l_id 楼盘编号 b_id楼栋编号 r_id房屋编号
 * 输出参数: 无
 * 返 回 值: 查中时返回结点的地址, 否则返回NULL
 * 调用说明:
 */
/**查询符合条件的房屋 */
struct room_node * SeekRoomnode(struct land_node * hd, char * l_id, char * b_id,char * r_id){
    struct block_node * p_b = NULL;
    struct room_node  * p_r = NULL;
    while ( hd && strcmp(l_id, hd ->land_id) != 0){
        hd = hd ->next_land;
    }
    if(hd != NULL){
        p_b = hd->hd_block;

        while (p_b &&strcmp(b_id,p_b ->block_id) != 0){
            p_b = p_b ->next_block;
        }
        if(p_b){
            p_r = p_b->hd_room;
            while(p_r && strcmp(r_id, p_r->room_id) != 0){
                p_r = p_r->next_room;
            }
            if(p_r){
                return p_r;
            }
        }
    }
    return NULL;
}

BOOL Maintainr_Info(void)                   /*维护房屋信息*/
{
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1、删除某房屋信息", "2、插入某房屋信息","3、修改某房屋信息","返回"};
    char *pCh2[] = {"请输入：   a.楼盘编号     b.楼栋编号     c.房屋编号","开始输入", "取消"};
    char *pCh3[] = {"请输入：   a.楼盘编号     b.楼栋编号     c.房屋编号","请输入：   d.房屋户型     e. 房屋面积    f.价格(/m2)","开始输入","取消"};
    char *pCh4[] = {" 请 输 入 原房屋的： a.楼盘编号     b.楼栋编号     c.房屋编号","请输入要修改的信息：   d.房屋户型     e. 房屋面积    f.价格(/m2)"," ","开始输入","取消"};
    int iHot = 4;
    pos.X = strlen(pCh[0]) +12;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 4;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+6, rcPop.Top+2},
        {rcPop.Left+6, rcPop.Top+5},
        {rcPop.Left+6, rcPop.Top+8},
        {rcPop.Left+10, rcPop.Top+12}
    };
    labels.pLoc = aLoc;
    areas.num = 4;
    SMALL_RECT aArea[] =
    {
        {
            rcPop.Left+6, rcPop.Top+2,
            rcPop.Left+22, rcPop.Top+2
        },
        {
            rcPop.Left + 6, rcPop.Top + 5,
            rcPop.Left +22, rcPop.Top +5
        },
        {
            rcPop.Left + 6, rcPop.Top + 8,
            rcPop.Left +22, rcPop.Top + 8
        },
        {
            rcPop.Left + 10, rcPop.Top + 12,
            rcPop.Left +13, rcPop.Top + 12
        }
    };
    char aSort[] = {0, 0,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);
    if (DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
        /*删除某房间信息*/
        PopOff();
        iHot = 2;
        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = 0;
        rcPop.Right = SCR_COL-1;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        labels.num = 3;
        labels.ppLabel = pCh2;
        COORD aLoc2[] =
        {
            {rcPop.Left+0, rcPop.Top+1},
            {rcPop.Left+28, rcPop.Top+13},
            {rcPop.Left+46, rcPop.Top+13}
        };
        labels.pLoc = aLoc2;
        areas.num = 2;
        SMALL_RECT aArea2[] =
        {
            {
                rcPop.Left+26, rcPop.Top+13,
                rcPop.Left+36, rcPop.Top+13
            },
            {
                rcPop.Left + 46, rcPop.Top + 13,
                rcPop.Left +49, rcPop.Top +13
            }
        };
        char aSort2[] = {0, 0};
        char aTag2[] = {1, 2};
        areas.pArea = aArea2;
        areas.pSort = aSort2;
        areas.pTag = aTag2;
        PopUp(&rcPop, att, &labels, &areas);
        if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
        {
            /*开始执行查找和删除操作*/
            CONSOLE_CURSOR_INFO lp,*plp = &lp;
            lp.dwSize = 1;
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
            char l_id[90];
            char b_id[90];
            char r_id[90];
            scanf("%s%s%s",l_id,b_id,r_id);
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
            PopOff();
            if(DelRoomnode(hd_land,l_id,b_id,r_id))
            {
                Success();
            }
            else
            {
                Fail();
            }
        }
        else
        {
            PopOff();
        }
    }
    else
    {
        if(iHot == 2)
        {
            /*插入某房屋信息*/
            PopOff();
            iHot = 2;
            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right = SCR_COL-1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;
            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
            labels.num = 4;
            labels.ppLabel = pCh3;
            COORD aLoc3[] =
            {
                {rcPop.Left+0, rcPop.Top+1},
                {rcPop.Left+0, rcPop.Top+2},
                {rcPop.Left+28, rcPop.Top+13},
                {rcPop.Left+46, rcPop.Top+13}
            };
            labels.pLoc = aLoc3;
            areas.num = 2;
            SMALL_RECT aArea3[] =
            {
                {
                    rcPop.Left+26, rcPop.Top+13,
                    rcPop.Left+36, rcPop.Top+13
                },
                {
                    rcPop.Left + 46, rcPop.Top + 13,
                    rcPop.Left +49, rcPop.Top +13
                }
            };
            char aSort2[] = {0, 0};
            char aTag2[] = {1, 2};
            areas.pArea = aArea3;
            areas.pSort = aSort2;
            areas.pTag = aTag2;
            PopUp(&rcPop, att, &labels, &areas);
            if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
            {
                /*开始执行查找和插入操作*/
                CONSOLE_CURSOR_INFO lp,*plp = &lp;
                lp.dwSize = 1;
                lp.bVisible = TRUE;
                SetConsoleCursorInfo(gh_std_out,plp);
                SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+4});
                struct room_node * p_r = (struct room_node *)malloc(sizeof(struct room_node));
                scanf("%s%s%s%s%s%s",p_r->land_id,p_r->block_id,p_r->room_id,p_r->type,p_r->area,p_r->price);
                lp.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out,plp);
                PopOff();
                if(AddRoomnode(hd_land,p_r))
                {
                    Success();
                }
                else
                {
                    free(p_r);
                    Fail();
                }
            }
            else
            {
                PopOff();
            }
        }
        else
        {
            if(iHot == 3)
            {
                /*修改某房屋信息*/
                PopOff();
                iHot = 2;
                pos.X = 30;
                pos.Y = 14;
                rcPop.Left = 0;
                rcPop.Right = SCR_COL-1;
                rcPop.Top = (25 - pos.Y) / 2;
                rcPop.Bottom = rcPop.Top + pos.Y - 1;
                att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
                labels.num = 4;
                labels.ppLabel = pCh4;
                COORD aLoc3[] =
                {
                    {rcPop.Left+0, rcPop.Top+1},
                    {rcPop.Left+0, rcPop.Top+2},
                    {rcPop.Left+0, rcPop.Top+3},
                    {rcPop.Left+28, rcPop.Top+13},
                    {rcPop.Left+46, rcPop.Top+13}
                };
                labels.pLoc = aLoc3;
                areas.num = 2;
                SMALL_RECT aArea3[] =
                {
                    {
                        rcPop.Left+26, rcPop.Top+13,
                        rcPop.Left+36, rcPop.Top+13
                    },
                    {
                        rcPop.Left + 46, rcPop.Top + 13,
                        rcPop.Left +49, rcPop.Top +13
                    }
                };
                char aSort2[] = {0, 0};
                char aTag2[] = {1, 2};
                areas.pArea = aArea3;
                areas.pSort = aSort2;
                areas.pTag = aTag2;
                PopUp(&rcPop, att, &labels, &areas);
                if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
                {
                    /*开始执行查找和修改操作*/
                    CONSOLE_CURSOR_INFO lp,*plp = &lp;
                    lp.dwSize = 1;
                    lp.bVisible = TRUE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    SetConsoleCursorPosition(gh_std_out,(COORD)
                    {
                        rcPop.Left+0, rcPop.Top+4
                    });
                    struct room_node * p_r = (struct room_node *)malloc(sizeof (struct room_node));

                    scanf("%s%s%s%s%s%s",p_r->land_id,p_r->block_id,p_r->room_id,p_r->type,p_r->area,p_r->price);
                    lp.bVisible = FALSE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    PopOff();
                    if(ModifRoomnode(hd_land,p_r->land_id,p_r->block_id,p_r->room_id,p_r))
                    {
                        Success();
                    }
                    else
                    {
                        free(p_r);
                        Fail();
                    }
                }
                else
                {
                    PopOff();
                }
            }
            else
            {
                PopOff();
            }
        }
    }
    return bRet;
}

/**
 * 函数名称: AddRoomnode
 * 函数功能: 在十字链表中插入一个房屋信息结点.
 * 输入参数: hd主链头指针
 *            p_add指向所要插入结点的指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示插入成功, FALSE表示插入失败
 * 调用说明:
 */
 /**增加房屋节点*/
BOOL AddRoomnode (struct land_node *hd, struct room_node * p_add){
    struct block_node * p_b = NULL;
    p_b = SeekBlocknode(hd,p_add->land_id,p_add ->block_id);
    if (p_b){
        p_add ->next_room = p_b ->hd_room;
        p_b ->hd_room = p_add;
        return TRUE;
    }
    else {
        free(p_add);
        return FALSE;
    }
}

/**
 * 函数名称: DelRoomnode
 * 函数功能: 从十字链表中删除指定的房屋信息结点.
 * 输入参数: hd 主链头指针
 *            l_id楼盘编号
 *            b_id楼栋编号
 *            r_id房屋编号
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示删除成功, FALSE表示删除失败
 * 调用说明: 根据楼盘编号、楼栋编号和房屋编号可以确定唯一的房屋信息
 */

 BOOL DelRoomnode (struct land_node *hd , char * l_id, char * b_id, char * r_id){
    struct block_node * p_b;
    struct room_node  * p_r, * prior_r;
    int sign = 0;

    p_b = SeekBlocknode(hd,l_id,b_id);    //找到符合条件的楼栋结点
    if(p_b){     //如果找到
        p_r = p_b ->hd_room;
        if(strcmp(r_id,p_r->room_id)==0){
            p_b->hd_room = p_r->next_room;
            sign = 1;
        }
        else {
            while(p_r &&strcmp(p_r->room_id,r_id)!= 0){
                prior_r = p_r;
                p_r = p_r->next_room;
            }
            if(p_r){                     //找到符合条件的房屋结点
                prior_r->next_room = p_r ->next_room;
                sign = 1;
            }
        }
    }
    if(sign){
        free(p_r);                        //进行删除操作
        return TRUE;
    }
    return FALSE;
}

/**
 * 函数名称:  ModifRoomnode
 * 函数功能: 对指定的房屋信息结点内容进行修改.
 * 输入参数: hd 主链头指针
 *            l_id楼盘编号
 *            b_id楼栋编号
 *            r_id房屋编号
 *            p_mod 指向存放修改内容结点的指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示修改成功, FALSE表示修改失败
 * 调用说明:
 */
/**修改房屋信息*/
BOOL ModifRoomnode(struct land_node *hd, char *l_id, char *b_id, char *r_id ,struct room_node *p_mod){
         struct room_node *p_r = NULL, *p_r_n = NULL;
         p_r = SeekRoomnode(hd,l_id,b_id,r_id);  //查找指定的房间结点
         if(p_r){        //如果找到
            p_r_n = p_r ->next_room;    //赋值前保存指针域
            * p_r = * p_mod;
            p_r->next_room = p_r_n;   //恢复指向
            return TRUE;
         }
         else{
            return FALSE;
         }
}

BOOL Success(void)                 /*某项操作成功，并弹出提醒窗口*/
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"操作成功", "返回"};
    int iHot = 1;
    pos.X = strlen(pCh[0]) +6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED|FOREGROUND_RED|FOREGROUND_INTENSITY;  /*白底黑字*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
        {rcPop.Left+5, rcPop.Top+5}
    };
    labels.pLoc = aLoc;
    areas.num = 2;
    SMALL_RECT aArea[] =
    {
        {
            rcPop.Left + 5, rcPop.Top + 5,
            rcPop.Left + 8, rcPop.Top +5
        }
    };
    char aSort[] = {0};
    char aTag[] = {1};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);
    if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
        PopOff();
    }
    return bRet;
}

BOOL Fail(void)                    /*某项操作失败，并弹出提醒窗口*/
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"操作失败", "返回"};
    int iHot = 1;
    pos.X = strlen(pCh[0]) +6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED|FOREGROUND_RED|FOREGROUND_INTENSITY;  /*白底黑字*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
        {rcPop.Left+5, rcPop.Top+5}
    };
    labels.pLoc = aLoc;
    areas.num = 2;
    SMALL_RECT aArea[] =
    {
        {
            rcPop.Left + 5, rcPop.Top + 5,
            rcPop.Left + 8, rcPop.Top +5
        }
    };
    char aSort[] = {0};
    char aTag[] = {1};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);
    if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
        PopOff();
    }
    return bRet;
}

 BOOL Queryl_Info(void)                        /*查询楼盘信息*/
{
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入：    楼盘编号","开始输入","取消"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc2[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+7, rcPop.Top+13},
        {rcPop.Left+18, rcPop.Top+13}
    };
    labels.pLoc = aLoc2;
    areas.num = 2;
    SMALL_RECT aArea2[] =
    {
        {
            rcPop.Left+7, rcPop.Top+13,
            rcPop.Left+15, rcPop.Top+13
        },
        {
            rcPop.Left + 18, rcPop.Top + 13,
            rcPop.Left +21, rcPop.Top +13
        }
    };
    char aSort2[] = {0, 0};
    char aTag2[] = {1, 2};
    areas.pArea = aArea2;
    areas.pSort = aSort2;
    areas.pTag = aTag2;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
		CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
        char l_id[40];
        struct land_node * p;
        scanf("%s",l_id);
        lp.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out,plp);
        PopOff();
        if((p = SeekLandnode(hd_land,l_id)))
        {
            /*输出楼盘信息到屏幕*/

            SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            printf("\n");
            printf("楼盘编号：%s\n",p->land_id);
            printf("楼盘名称：%s\n",p->land_name);
            printf("楼盘地址：%s\t楼盘地区;%s\n",p->land_add,p->land_area);
            printf("楼盘电话：%s\n",p->land_tel);
            printf("开 发 商：%s\n",p->developer);
            printf("物业公司：%s\n",p->pro_man_com);
            printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
        }
        else
        {
            Fail();
        }
    }
    else
    {
        PopOff();
    }
    return bRet;
}

BOOL Queryb_Info(void)                        /*查询楼栋信息*/
{
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入：楼盘编号   楼栋编号","开始输入","取消"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc2[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+7, rcPop.Top+13},
        {rcPop.Left+18, rcPop.Top+13}
    };
    labels.pLoc = aLoc2;
    areas.num = 2;
    SMALL_RECT aArea2[] =
    {
        {
            rcPop.Left+7, rcPop.Top+13,
            rcPop.Left+15, rcPop.Top+13
        },
        {
            rcPop.Left + 18, rcPop.Top + 13,
            rcPop.Left +21, rcPop.Top +13
        }
    };
    char aSort2[] = {0, 0};
    char aTag2[] = {1, 2};
    areas.pArea = aArea2;
    areas.pSort = aSort2;
    areas.pTag = aTag2;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
		CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
        char l_id[40];
        char b_id[40];
        struct block_node * p = NULL;
        scanf("%s",l_id);
        printf("\t\t\t  ");
        scanf("%s",b_id);
        lp.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out,plp);
        PopOff();
        if((p = SeekBlocknode(hd_land,l_id,b_id))){

            /*输出楼栋信息到屏幕*/
            struct room_node * p_r = NULL;
            int n = 1;
            SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            printf("\n");
            printf("楼盘编号：%s\t",p->land_id);
            printf("楼栋编号：%s\t",p->block_id);
            printf("公 摊 率：%s\n",p->pool_rote);
            printf(" 单  元 ：%s\n",p->unit);
            printf(" 楼  层 ：%s\n",p->floor);
            printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
            printf("\t此楼栋下的楼房信息如下\n");
            printf("\t户型      \t面积\t价格\n");
            for(p_r = p ->hd_room; p_r != NULL; p_r= p_r ->next_room)
            {
                if(n == 0){
                    printf("\t此楼栋下的楼房信息如下\n");
                    printf("房屋编号       户型       \t面积     \t价格   \n");
                }

                printf("%s\t    %s\t      %s\t  %s\n",p_r->room_id,p_r->type,p_r->area,p_r->price);
                n++;

                if(n == 10){
                    printf("\n按任意键查看下一页的房屋信息\n");
                    getch();
                    ClearScreen();
                    n = 0;
                }
            }
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
        }
        else
        {
            Fail();
        }
    }
    else
    {
        PopOff();
    }
    return bRet;
}


BOOL Queryr_Info(void)                        /*查询楼房信息*/
{
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入:楼盘编号,楼栋编号,房屋编号","开始输入","取消"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc2[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+7, rcPop.Top+13},
        {rcPop.Left+18, rcPop.Top+13}
    };
    labels.pLoc = aLoc2;
    areas.num = 2;
    SMALL_RECT aArea2[] =
    {
        {
            rcPop.Left+7, rcPop.Top+13,
            rcPop.Left+15, rcPop.Top+13
        },
        {
            rcPop.Left + 18, rcPop.Top + 13,
            rcPop.Left +21, rcPop.Top +13
        }
    };
    char aSort2[] = {0, 0};
    char aTag2[] = {1, 2};
    areas.pArea = aArea2;
    areas.pSort = aSort2;
    areas.pTag = aTag2;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
		CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
        char l_id[40],b_id[40],r_id[40];
        struct room_node *p = NULL;
        scanf("%s%s%s",l_id,b_id,r_id);
        lp.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out,plp);
        PopOff();
        if((p = SeekRoomnode(hd_land,l_id,b_id,r_id)))
        {
            /*输出楼房信息到屏幕*/

            SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            printf("\n");
            printf("楼盘编号:%s\t",p->land_id);
            printf("楼栋编号:%s\t",p->block_id);
            printf("楼房编号:%s\n",p->room_id);
            printf("房屋类型:%s\n",p->type );
            printf("面积%s\n",p->area);
            printf("价格%s\n",p->price);
            printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
        }
        else
        {
            Fail();
        }
    }
    else
    {
        PopOff();
    }
    return bRet;
}


BOOL QueryLandArea(void) {
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入：   城区    ","开始输入","取消"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc2[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+7, rcPop.Top+13},
        {rcPop.Left+18, rcPop.Top+13}
    };
    labels.pLoc = aLoc2;
    areas.num = 2;
    SMALL_RECT aArea2[] =
    {
        {
            rcPop.Left+7, rcPop.Top+13,
            rcPop.Left+15, rcPop.Top+13
        },
        {
            rcPop.Left + 18, rcPop.Top + 13,
            rcPop.Left +21, rcPop.Top +13
        }
    };
    char aSort2[] = {0, 0};
    char aTag2[] = {1, 2};
    areas.pArea = aArea2;
    areas.pSort = aSort2;
    areas.pTag = aTag2;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
		CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
        char area[NUM];
        scanf("%s",area);
        lp.bVisible = FALSE;
        BOOL  bre = FALSE;
        SetConsoleCursorInfo(gh_std_out,plp);
        PopOff();
        if((bre = SeekLandArea(hd_land,area,0)))
        {
            /*输出楼盘信息到屏幕*/

            SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            printf("\n");
            bre = SeekLandArea(hd_land,area,1);
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
        }
        else
        {
            Fail();
        }
    }
    else
    {
        PopOff();
    }
    return bRet;
}

///查询指定城区的楼盘通讯信息
BOOL SeekLandArea(struct land_node *hd , char * area, int n){
    struct land_node * p_l = hd;
    int sign = 0;
    BOOL Bre = FALSE;
    while(p_l){
        if(strcmp(p_l->land_area, area) == 0){
            Bre = TRUE;
           if(n){
                if(sign == 0 ){
                    printf("\n在 * %s * 的楼盘的通讯信息为",area);

                }

                sign = 1;
                printf("\n楼盘编号: %s\n楼盘名称: %s\n楼盘地址: %s\n联系方式: %s",p_l->land_id,p_l->land_name,p_l->land_area,p_l->land_tel);

           }
        }
        p_l = p_l->next_land;
    }

    return Bre;
}

/*按照类型查询房屋通讯信息*/
BOOL QueryRoomType(void){
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入：   房屋类型","开始输入","取消"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc2[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+7, rcPop.Top+13},
        {rcPop.Left+18, rcPop.Top+13}
    };
    labels.pLoc = aLoc2;
    areas.num = 2;
    SMALL_RECT aArea2[] =
    {
        {
            rcPop.Left+7, rcPop.Top+13,
            rcPop.Left+15, rcPop.Top+13
        },
        {
            rcPop.Left + 18, rcPop.Top + 13,
            rcPop.Left +21, rcPop.Top +13
        }
    };
    char aSort2[] = {0, 0};
    char aTag2[] = {1, 2};
    areas.pArea = aArea2;
    areas.pSort = aSort2;
    areas.pTag = aTag2;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
		CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
        char type[NUM];
        BOOL bre = FALSE;
        scanf("%s",type);
        lp.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out,plp);
        PopOff();

        SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        bre = SeekRoomType(hd_land,type);
        lp.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out,plp);

        if(!bre){
            Fail();
        }

    }
    else
    {
        PopOff();
    }
    return bRet;
}
  ///查询符合房型的房屋信息
BOOL SeekRoomType(struct land_node * hd, char * type){
    struct land_node * p_l = hd;
    struct block_node * p_b = NULL;
    struct room_node  *p_r = NULL;
    BOOL bre = FALSE;
    int sign = 1;
    int n = 0;
    int next = 1;
    while( p_l){
        p_b = p_l->hd_block;
        sign = 1;
        while(p_b){
            p_r = p_b ->hd_room;
            while(p_r){
                if(strcmp(p_r->type , type ) == 0){
                    bre = TRUE;

                    if(sign){
                        n = 0;
                        printf(" \n\n所属楼盘信息\n楼盘名称: %s\n楼盘编号: %s\n楼盘地址: %s\n联系方式: %s\n",p_l->land_name,p_l->land_id,p_l->land_add,p_l->land_tel);
                        printf("楼栋号—房间号\t楼栋号—房间号\t楼栋号—房间号\t楼栋号—房间号\n");
                        sign = 0;
                        next += 5;
                    }
                    printf(" %s栋-%s室\t",p_b->block_id,p_r->room_id);
                    n++;
                    if( n % 4 == 0){
                        printf("\n");
                        next ++;
                        n = 0;
                    }
                     if(next % 15 == 0){

                            printf("\n按任意键查看下一页中的符合条件的房屋信息\n");
                            getch();
                            ClearScreen();
                            printf(" \n所属楼盘信息\n楼盘名称: %s\n楼盘编号: %s\n楼盘地址: %s\n联系方式: %s\n",p_l->land_name,p_l->land_id,p_l->land_add,p_l->land_tel);
                            printf("楼栋号—房间号\t楼栋号—房间号\t楼栋号—房间号\t楼栋号—房间号\n");
                            next = 5;
                    }
                }
                p_r = p_r ->next_room;
            }
            p_b = p_b->next_block;
        }
        p_l = p_l ->next_land;
    }
    return bre;
}

/**按照价格查询房屋通讯信息*/
BOOL QueryRoomPrice(void){
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入价格范围（最低-最高）","开始输入","取消"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc2[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+7, rcPop.Top+13},
        {rcPop.Left+18, rcPop.Top+13}
    };
    labels.pLoc = aLoc2;
    areas.num = 2;
    SMALL_RECT aArea2[] =
    {
        {
            rcPop.Left+7, rcPop.Top+13,
            rcPop.Left+15, rcPop.Top+13
        },
        {
            rcPop.Left + 18, rcPop.Top + 13,
            rcPop.Left +21, rcPop.Top +13
        }
    };
    char aSort2[] = {0, 0};
    char aTag2[] = {1, 2};
    areas.pArea = aArea2;
    areas.pSort = aSort2;
    areas.pTag = aTag2;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
		CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
        char l_price[NUM], h_price[NUM];
        scanf("%s%s",l_price,h_price);
        BOOL bre = FALSE;
        lp.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out,plp);
        PopOff();

        SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        bre = SeekRoomPrice(hd_land,h_price,l_price);
        lp.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out,plp);

        if(!bre){
            Fail();
        }

    }
    else
    {
        PopOff();
    }
    return bRet;
}

///查询符合价格的房屋通讯信息
BOOL SeekRoomPrice(struct land_node * hd, char * h_price, char * l_price){
    struct land_node * p_l = hd;
    struct block_node * p_b = NULL;
    struct room_node  *p_r = NULL;
    BOOL bre = FALSE;
    int sign = 1;
    int n = 1;
    int next = 1;
    float h_pri = atoi(h_price);
    float l_pri = atoi(l_price);
    while( p_l){
        p_b = p_l->hd_block;
        sign = 1;
        while(p_b){
            p_r = p_b ->hd_room;
            while(p_r){
                if(atoi(p_r->price) <= h_pri && atoi(p_r->price)>=l_pri ){
                    bre = TRUE;

                    if(sign){
                        n = 1;
                        printf("\n所属楼盘信息:\t楼盘名称:%s \t楼盘编号:%s\n\t\t楼盘地址:%s\n\t\t联系方式:%s",p_l->land_name,p_l->land_id,p_l->land_add,p_l->land_tel);
                        printf("\n楼栋—房间号 \t价格  楼栋—房间号 \t价格  楼栋—房间号   \t价格\n");
                        sign = 0;
                        next += 5;

                    }
                    printf(" %s栋-%s室\t%s\t",p_b->block_id,p_r->room_id,p_r->price);
                    n++;
                    if( n % 4 == 0){
                        printf("\n");
                        next  ++ ;
                        n = 1;
                    }
                     if(next == 17){
                            printf("\n按任意键查看下一页中的符合条件的房屋信息\n");
                            getch();
                            next = 1;
                    }
                }
                p_r = p_r ->next_room;
            }
            p_b = p_b->next_block;
        }
        p_l = p_l ->next_land;
    }
    return bre;

}

/** 组合条件查询房屋信息函数*/
BOOL Queryap(void){
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入你喜欢的城区  以及你期望的价格范围","开始输入","取消"};
    int iHot = 2;
    pos.X = 40;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  //白底黑字
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc2[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+7, rcPop.Top+13},
        {rcPop.Left+18, rcPop.Top+13}
    };
    labels.pLoc = aLoc2;
    areas.num = 2;
    SMALL_RECT aArea2[] =
    {
        {
            rcPop.Left+7, rcPop.Top+13,
            rcPop.Left+15, rcPop.Top+13
        },
        {
            rcPop.Left + 18, rcPop.Top + 13,
            rcPop.Left +21, rcPop.Top +13
        }
    };
    char aSort2[] = {0, 0};
    char aTag2[] = {1, 2};
    areas.pArea = aArea2;
    areas.pSort = aSort2;
    areas.pTag = aTag2;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
		CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
        char area[NUM],h_price[NUM],l_price[NUM];
        scanf("%s",area);
        scanf("%s",l_price);
        scanf("%s",h_price);

        int sign = 0;
        BOOL bre = FALSE;
        lp.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out,plp);
        PopOff();
        if(SeekRoom_ap(hd_land,area,h_price,l_price,sign)){
            sign = 1;
        }
        if(sign)
        {
            SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            printf("\n");
            bre = SeekRoom_ap(hd_land,area,h_price,l_price,sign);
            printf("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
        }
        else
        {
            Fail();
        }
    }
    else
    {
        PopOff();
    }

    return bRet;
}

BOOL * SeekRoom_ap(struct land_node *hd, char *area,char *h_price, char * l_price,int sign ){
        struct land_node *p_l = hd;
        struct block_node * p_b = NULL;
        struct room_node *p_r = NULL;
        BOOL  re = FALSE;
        int n = 0;
        if(sign){
            printf("\n楼盘—楼栋—房屋编号     联系方式        类型      价格/m2        公摊率   ");
        }
        while(p_l){
            if(strcmp(area,p_l->land_area)==0){    //城区符合条件
                p_b = p_l->hd_block;
                while(p_b){
                    p_r = p_b ->hd_room;
                    while(p_r){
                                   // 在房屋支链上 寻找符合价格条件的房屋
                        if(find(h_price,l_price,p_r->price)){
                            re= TRUE;
                            if(sign){
                            printf("\n%s",p_r->land_id);
                            printf("—%s",p_r->block_id);
                            printf("—%s",p_r->room_id);
                            printf(" %s",p_l->land_tel);
                            printf("   \t%s",p_r->type);
                            printf("\t%s",p_r->price);
                            printf("  %s%\n",p_b->pool_rote);
                            n++;
                            }
                            if( n == 10){
                            printf("\n按任意键查看下一页的信息\n");
                            getch();
                            ClearScreen();
                            printf("\n楼盘—楼栋—房屋编号     联系方式       类型    价格/m2        公摊率   ");
                            n = 0;
                          }
                        }

                        p_r = p_r -> next_room;
                    }
                    p_b = p_b -> next_block;
                }
            }
            p_l = p_l->next_land;
        }
        return re;
}

BOOL find(char *  h_price, char *l_price,char *price){

        BOOL Bre = FALSE;
        if( atof(price)<=atof(h_price) && atof(price) >= atof(l_price)){
            Bre= TRUE;
        }
        return Bre;
}

///按楼盘所在的地区统计
BOOL Stat_a_Info(void){
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入：  所要统计的城区","开始输入","取消"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  //白底黑字
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc2[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+7, rcPop.Top+13},
        {rcPop.Left+18, rcPop.Top+13}
    };
    labels.pLoc = aLoc2;
    areas.num = 2;
    SMALL_RECT aArea2[] =
    {
        {
            rcPop.Left+7, rcPop.Top+13,
            rcPop.Left+15, rcPop.Top+13
        },
        {
            rcPop.Left + 18, rcPop.Top + 13,
            rcPop.Left +21, rcPop.Top +13
        }
    };
    char aSort2[] = {0, 0};
    char aTag2[] = {1, 2};
    areas.pArea = aArea2;
    areas.pSort = aSort2;
    areas.pTag = aTag2;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
		CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
        char area[NUM];
        scanf("%s",area);
        lp.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out,plp);
        PopOff();
        int n = 0;
        struct land_node *p = NULL;
        struct land_node *l_p = StAreanode(hd_land,area);  //查询符合条件的楼盘信息

         p = l_p;
        if(l_p!=NULL)
        {
            SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);

            while (p!= NULL){
                p = p -> next_land;
                n ++;
            }
            printf("\n%s 地区共有楼盘 : %d  个\n", area, n);

            printf("\n楼盘基本信息如下：\n");
            p = l_p;
            while(l_p){
                    printf("\n楼盘编号:%s \n楼盘名称 : %s\n联系方式: %s \n开发商: %s\n",l_p->land_id,l_p->land_name,l_p->land_tel,l_p->developer);
                    l_p = l_p->next_land;
            }
            printf("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
            printf("\n如果你想知道更加详细的楼盘信息，请按照楼盘编号查询");
            while (p != NULL){
                l_p = p;
                p = p -> next_land;
                free(l_p);
            }
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
        }
        else
        {
            Fail();
        }
    }
    else
    {
        PopOff();
    }
    return bRet;
}

///寻找指定城区的楼盘信息
struct land_node * StAreanode(struct land_node *hd , char * area){
    struct land_node *l_p = NULL,*l_p_n = NULL;
    while(hd){
        if(strcmp(hd->land_area,area)==0){
            l_p_n = (struct land_node *)malloc(sizeof(struct land_node));
            * l_p_n = * hd;
            l_p_n ->next_land = l_p;
            l_p = l_p_n;
        }
            hd = hd->next_land;
    }
    if(l_p){
        return l_p;
    }
    else
        return  NULL;
}


BOOL Stat_t_Info(void)        /*按房屋的户型统计    */
{
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入要统计的房屋的类型：      ","开始输入","取消"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc2[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+7, rcPop.Top+13},
        {rcPop.Left+18, rcPop.Top+13}
    };
    labels.pLoc = aLoc2;
    areas.num = 2;
    SMALL_RECT aArea2[] =
    {
        {
            rcPop.Left+7, rcPop.Top+13,
            rcPop.Left+15, rcPop.Top+13
        },
        {
            rcPop.Left + 18, rcPop.Top + 13,
            rcPop.Left +21, rcPop.Top +13
        }
    };
    char aSort2[] = {0, 0};
    char aTag2[] = {1, 2};
    areas.pArea = aArea2;
    areas.pSort = aSort2;
    areas.pTag = aTag2;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
		CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD){rcPop.Left+0, rcPop.Top+3});
        char  type[NUM];
        scanf("%s",type);
        lp.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out,plp);
        PopOff();
        /**按照房屋类型统计函数*/

        struct land_sta * r_p = StTypenode(hd_land,type);
        struct land_sta * r_p_f = r_p;
        struct block_sta * p =NULL;
        struct block_sta * p_f;

        int n = 0;

        if(r_p!=NULL)
        {
            SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            printf(" *  %s  *型的房屋的统计的信息 情况如下：\n",type);


            while(r_p) {
                printf("\n 楼盘 %s: %d 间 \n",r_p->land_id,r_p->num);
                p = r_p -> hd_block_sta;
                if(p){
                    printf("各楼栋各有\n");
                    printf("楼栋号--房间数\t楼栋号--房间数\t楼栋数--房间数\t楼栋号--房间数\t楼栋号--房间数\n");
                }

                while(p){
                printf("  %s栋  %d间\t",p->block_id,p->num);
                p = p -> nx_block_sta;
                }
                r_p = r_p->nx_land_sta;

            }
            while(r_p_f){
                p_f = r_p_f -> hd_block_sta;
                while(p_f){
                 p = p_f->nx_block_sta;
                 free(p_f);
                 p_f = p;
                }
                r_p = r_p_f->nx_land_sta;
                free(r_p_f);
                r_p_f = r_p;
            }

            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
        }
        else
        {
            Fail();
        }
    }
    else
    {
        PopOff();
    }
    return bRet;
}

/**统计符合户型条件的房屋类型*/
struct land_sta * StTypenode(struct land_node *hd, char * type){
    struct land_sta  * p_re = NULL, * p_r_sta = NULL;
    struct block_sta * p_b_sta1, *p_b_sta;
    struct block_node * p_block;
    struct room_node  * p_room = NULL,  *p_new = NULL,  *p_new_n = NULL;
    while (hd){
        p_r_sta = (struct land_sta *)malloc(sizeof(struct land_sta));
        p_r_sta -> nx_land_sta = p_re;
        p_re = p_r_sta;

        p_r_sta -> num = 0;
        p_r_sta -> hd_block_sta = NULL;
        strcpy(p_r_sta -> land_id,hd->land_id);
        strcpy(p_r_sta ->land_name,hd->land_name);

        p_block = hd -> hd_block;
        while(p_block){
            p_b_sta1 = (struct block_sta *)malloc(sizeof(struct block_sta));
            p_b_sta1->nx_block_sta = p_r_sta -> hd_block_sta;
            p_r_sta->hd_block_sta = p_b_sta1;

            p_b_sta1->num = 0;
            strcpy(p_b_sta1->block_id ,p_block->block_id);

            p_room = p_block ->hd_room;
            while(p_room){
                if(strcmp(type,p_room ->type)==0){
                     p_b_sta1->num ++;
                     p_r_sta ->num ++;
                }
                p_room = p_room ->next_room;
            }
            p_block = p_block ->next_block;
        }
        hd = hd ->next_land;
    }

        return p_re;
}

int DealConInput(HOT_AREA *pHotArea, int *piHot)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    int num, arrow, iRet = 0;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/

    SetHotPoint(pHotArea, *piHot);
    while (TRUE)
    {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if ((inRec.EventType == MOUSE_EVENT) &&
                (inRec.Event.MouseEvent.dwButtonState
                 == FROM_LEFT_1ST_BUTTON_PRESSED))
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3;
            cTag = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2) & 15;
            cSort = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 6) & 3;

            if ((cNo == gp_top_layer->LayerNo) && cTag > 0)
            {
                *piHot = cTag;
                SetHotPoint(pHotArea, *piHot);
                if (cSort == 0)
                {
                    iRet = 13;
                    break;
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown)
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;;
            if (asc == 0)
            {
                arrow = 0;
                switch (vkc)
                {
                    /*方向键(左、上、右、下)的处理*/
                case 37:
                    arrow = 1;
                    break;
                case 38:
                    arrow = 2;
                    break;
                case 39:
                    arrow = 3;
                    break;
                case 40:
                    arrow = 4;
                    break;
                }
                if (arrow > 0)
                {
                    num = *piHot;
                    while (TRUE)
                    {
                        if (arrow < 3)
                        {
                            num--;
                        }
                        else
                        {
                            num++;
                        }
                        if ((num < 1) || (num > pHotArea->num) ||
                                ((arrow % 2) && (pHotArea->pArea[num-1].Top
                                                 == pHotArea->pArea[*piHot-1].Top)) || ((!(arrow % 2))
                                                         && (pHotArea->pArea[num-1].Top
                                                             != pHotArea->pArea[*piHot-1].Top)))
                        {
                            break;
                        }
                    }
                    if (num > 0 && num <= pHotArea->num)
                    {
                        *piHot = num;
                        SetHotPoint(pHotArea, *piHot);
                    }
                }
            }
            else if (vkc == 27)
            {
                /*ESC键*/
                iRet = 27;
                break;
            }
            else if (vkc == 13 || vkc == 32)
            {
                /*回车键或空格表示按下当前按钮*/
                iRet = 13;
                break;
            }
        }
    }
    return iRet;
}

void SetHotPoint(HOT_AREA *pHotArea, int iHot)
{
    CONSOLE_CURSOR_INFO lpCur;
    COORD pos = {0, 0};
    WORD att1, att2;
    int i, width;
    att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
    att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    for (i=0; i<pHotArea->num; i++)
    {
        /*将按钮类热区置为白底黑字*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0)
        {
            /*热区是按钮类*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &num_written);
        }
    }
    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0)
    {
        /*被激活热区是按钮类*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &num_written);
    }
    else if (pHotArea->pSort[iHot-1] == 1)
    {
        /*被激活热区是文本框类*/
        SetConsoleCursorPosition(gh_std_out, pos);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);
    }
}

BOOL AboutDorm(void)
{
    InitInterface();
    CONSOLE_CURSOR_INFO lp,*plp = &lp;
    lp.dwSize = 1;
    lp.bVisible = TRUE;
    SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
    SetConsoleCursorInfo(gh_std_out,plp);
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>楼盘管理系统>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n\n\n\n\n\n");
    printf("\n\t\t***********************************");
    printf("\n\t\t*     信息安全1203 班马鸿菲       *");
    printf("\n\t\t*                                 *");
    printf("\n\t\t*     联系方式：13260539228       *");
    printf("\n\t\t*                                 *");
    printf("\n\t\t***********************************");
    printf("\n\n\n\n\n\n\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    lp.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out,plp);
}


BOOL HelpTopic(void){
    InitInterface();
    CONSOLE_CURSOR_INFO lp,*plp = &lp;
    lp.dwSize = 1;
    lp.bVisible = TRUE;
    SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
    SetConsoleCursorInfo(gh_std_out,plp);
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>帮助>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    printf("1.请在录入和删除数据后进行保存。\n");
    printf("2.如想按编号查找某个楼盘，楼栋，房屋请使用数据查询。\n");
    printf("3.如果想按照价格和城区查找房屋的基本信息找请使用组合查找。\n");
    printf("4.如想查找您喜欢的户型的房屋，你喜欢的城区的楼盘信息请使用数据统计\n");
    lp.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out,plp);
}
