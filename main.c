#include "load.h"

int main(void)
{
    COORD size = {SCR_COL, SCR_ROW};              /*���ڻ�������С*/
    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼����豸���*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* ��ȡ��׼�����豸���*/
    SetConsoleTitle(gp_sys_name);                 /*���ô��ڱ���*/
    SetConsoleScreenBufferSize(gh_std_out, size); /*���ô��ڻ�������С80*25*/
    LoadData();                   /*���ݼ���*/
    InitInterface();              /*�����ʼ��*/
    RunSys(&hd_land);             /*ϵͳ����ģ���ѡ������*/
    CloseSys(hd_land);            /*�˳�ϵͳ*/
    return 0;
}

/**
 * ��������: LoadData
 * ��������: ������������ݴ������ļ����뵽�ڴ滺������ʮ��������.
 * �������: ��
 * �������: ��
 * �� �� ֵ: BOOL����, ���ܺ����г��˺���ExitSys�ķ���ֵ����ΪFALSE��,
 *           ���������ķ���ֵ����ΪTRUE.
 * ����˵��: Ϊ���ܹ���ͳһ�ķ�ʽ���ø����ܺ���, ����Щ���ܺ�����ԭ����Ϊ
 *           һ��, ���޲����ҷ���ֵΪBOOL. ����ֵΪʱ, ������������.
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
        /*���ݼ�����ʾ��Ϣ*/
        printf("\n\n\n\t\tϵͳ�������ݲ�����!\n");
        printf("\n\t\t�����������...\n");
        getch();
    }
    else {

        printf("\n\n\t\t\t  ��ӭ����¥�̹���ϵͳ\n");
        printf("\n\t\t\t\t�����������");
        getch();
    }
    return TRUE;
}

/**
 * ��������: CreateList
 * ��������: �������ļ���ȡ��������, ����ŵ���������ʮ��������.
 * �������: ��
 * �������: hd ����ͷָ��ĵ�ַ, ����������������ʮ����.
 * �� �� ֵ: int����ֵ, ��ʾ�����������.
 *           0  ����, ������
 *           4  �Ѽ���¥����Ϣ���ݣ���¥����Ϣ�ͷ�����Ϣ����
 *           12 �Ѽ���¥����Ϣ��¥��������Ϣ���ݣ��޷�����Ϣ����
 *           28 ����������ݶ��Ѽ���
 *
 * ����˵��:
 */
int  CreateList (struct land_node **hd){
    struct land_node *p_hd = NULL, * p_land = NULL;
    struct block_node *p_block = NULL;
    struct room_node  *p_room = NULL;
    FILE  *p_file = NULL;
    int sign = 0;  /*sign Ϊ��ʶ 0��ʾû�н�� 1����¥�̽��2 �ҵ�����¥�����*/
    int re = 0;

    if ((p_file = fopen(gp_load_info_filename,"r"))== NULL){
        printf("\n\n\n\t\t\t¥�̻�����Ϣ�ļ���ʧ��");
        return re;
    }
    printf("\n\n\n\n\n\t\t\t¥�̻�����Ϣ�ļ��򿪳ɹ�����");

     /*�������ļ��ж�¥�̻�����Ϣ���ݣ������Ժ���ȳ���ʽ����������*/
    while(! feof(p_file)){
        p_land =(struct land_node*)malloc(sizeof(struct land_node));
        p_land -> hd_block = NULL;
        fscanf(p_file,"%s\t%s\t%s\t%s\t%s\t%s\t%s\n",p_land->land_id,p_land->land_name,p_land->land_add,p_land->land_tel, p_land->land_area,p_land->developer,p_land->pro_man_com);
        p_land->next_land = p_hd;
        p_hd=p_land;
    }
    fclose(p_file);

    if(p_hd == NULL){
        printf("\n\t\t\t¥�̻�����Ϣ�ļ�����ʧ��!!");
        return re;
    }
    *hd = p_hd;
    re += 4;

    if ((p_file=fopen(gp_bloc_info_filename,"r"))==NULL){
        printf("\n\t\t\t¥��������Ϣ�ļ���ʧ��!!");
        return re;
    }
    printf("\n\t\t\t¥��������Ϣ�ļ��򿪳ɹ�����");
    re += 8;

    /*�������ļ���ȡ¥��������Ϣ����������Ӧ����¥��������Ϣ����� */
    while (!feof (p_file)){
        p_block = (struct block_node*)malloc(sizeof (struct block_node));
        p_block->hd_room=NULL;
        fscanf(p_file,"%s\t%s\t%s\t%s\t%s\n",p_block->block_id,p_block->land_id,p_block->pool_rote,p_block->unit,p_block->floor);
        p_land = p_hd;

        /*�������ϲ���¥������¥�̵��������*/
        while(p_land!=NULL && strcmp(p_land->land_id ,p_block->land_id)!=0 )
        {
            p_land = p_land->next_land;
        }

         /*����ҵ����򽫽���Ժ���ȳ��ķ�ʽ����*/
        if(p_land!=NULL)
        {
            p_block->next_block = p_land->hd_block;
            p_land->hd_block = p_block;

        }
        /*���û���ҵ������ͷ������������ڴ�ռ�*/
        else
        {
            free(p_block);
        }
    }
    fclose(p_file);

    if ((p_file=fopen(gp_room_info_filename,"r"))==NULL){
        printf("\n\t\t\t���ݻ�����Ϣ�ļ���ʧ��!!");
        return re;
    }
    printf("\n\t\t\t���ݻ�����Ϣ�ļ��򿪳ɹ�����");
    re += 16;

    /* �������ļ��ж�ȡ¥��������Ϣ������¥��������Ϣ֧����Ӧ����¥��֧����*/
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
            /*����ҵ������Ժ���ȳ��ķ�ʽ���뷿����Ϣ֧����*/
            else{
            p_room->next_room=p_block->hd_room;
            p_block->hd_room=p_room;
            sign=2;
            }
        }
        /*���û���ҵ����ͷ��������Ľ����ڴ�ռ�*/
        if(sign!=2)
            free(p_room);
    }
    fclose(p_file);
    return re;
}

/**
 * ��������: InitInterface
 * ��������: ��ʼ������.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 * ����˵��:
 */
void InitInterface()
{
    WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
               | BACKGROUND_BLUE;  /*��ɫǰ������ɫ����*/

    SetConsoleTextAttribute(gh_std_out, att);  /*���ÿ���̨��Ļ�������ַ�����*/

    ClearScreen();  /* ����*/

    /*��������������Ϣ��ջ������ʼ�������Ļ���ڵ�����һ�㵯������*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*��Ļ�ַ�����*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*�������ڵĲ��Ϊ0*/
    gp_top_layer->rcArea.Left = 0;  /*�������ڵ�����Ϊ������Ļ����*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*��ʾ�˵���*/
    ShowState();    /*��ʾ״̬��*/

    return;
}
/**
 * ��������: ClearScreen
 * ��������: �����Ļ��Ϣ.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ClearScreen(void)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );/*ȡ��Ļ��������Ϣ*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*������Ļ�������ַ���Ԫ��*/

    /*����Ļ���������е�Ԫ���ַ���������Ϊ��ǰ��Ļ�������ַ�����*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &num_written);

    /*����Ļ���������е�Ԫ���Ϊ�ո��ַ�*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &num_written);

    return;
}
/**
 * ��������: ShowMenu
 * ��������: ����Ļ����ʾ���˵�, ����������, �����˵���һ������ѡ�б��
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 * ����˵��:
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
    for (i=0; i < 5; i++) /*�ڴ��ڵ�һ�е�һ�д�������˵���*/
    {
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = 0;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*���ع��*/

    /*���붯̬�洢����Ϊ��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu = {0, 0, size.X-1, 0} ;

    /*�����ڵ�һ�е����ݶ��뵽��Ų˵�����Ļ���ַ���Ϣ�Ļ�������*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*����һ����Ӣ����ĸ��Ϊ��ɫ�������ַ���Ԫ��Ϊ�׵׺���*/
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

    /*�޸ĺ�Ĳ˵����ַ���Ϣ��д�����ڵĵ�һ��*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*�����λ�������ڵ�2�е�1��*/

    /*���˵�����Ϊ�������������Ϊ�˵���ţ���������Ϊ0(��ť��)*/
    i = 0;
    do
    {
        PosB = PosA + strlen(ga_main_menu[i]);  /*��λ��i+1�Ų˵������ֹλ��*/
        for (j=PosA; j<PosB; j++)
        {
            gp_scr_att[j] |= (i+1) << 2; /*���ò˵��������ַ���Ԫ������ֵ*/
        }
        PosA = PosB + 4;
        i++;
    }
    while (i<5);

    TagMainMenu(gi_sel_menu);  /*��ѡ�����˵���������ǣ�gi_sel_menu��ֵΪ1*/

    return;
}

/**
 * ��������: ShowState
 * ��������: ��ʾ״̬��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 * ����˵��: ״̬���ַ�����Ϊ�׵׺���, ��ʼ״̬��״̬��Ϣ.
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
 * ��������: TagMainMenu
 * ��������: ��ָ�����˵�������ѡ�б�־.
 * �������: num ѡ�е����˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 2, PosB;
    char ch;
    int i;

    if (num == 0) /*numΪ0ʱ������ȥ�����˵���ѡ�б��*/
    {
        PosA = 0;
        PosB = 0;
    }
    else  /*���򣬶�λѡ�����˵������ֹλ��: PosAΪ��ʼλ��, PosBΪ��ֹλ��*/
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
    /*ȥ��ѡ�в˵���ǰ��Ĳ˵���ѡ�б��*/
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

    /*��ѡ�в˵���������ǣ��ڵװ���*/
    for (i=PosA; i<PosB; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED;
    }
    /*ȥ��ѡ�в˵������Ĳ˵���ѡ�б��*/
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
    /*�����ñ�ǵĲ˵�����Ϣд�����ڵ�һ��*/
    SMALL_RECT rcMenu = {0, 0, size.X-1, 0};
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * ��������: PopMenu
 * ��������: ����ָ�����˵����Ӧ���Ӳ˵�.
 * �������: num ָ�������˵����
 * �������: ��
 * �� �� ֵ: ��
 * ����˵��:
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

    if (num  != gi_sel_menu)       /*���ָ�����˵�������ѡ�в˵�*/
    {
        if (gp_top_layer->LayerNo != 0) /*�����ǰ�����Ӳ˵�����*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNo != 0) /*���ѵ������Ӳ˵����򷵻�*/
    {
        return;
    }

    gi_sel_menu = num;    /*��ѡ�����˵�����Ϊָ�������˵���*/
    TagMainMenu(gi_sel_menu); /*��ѡ�е����˵����������*/
    LocSubMenu(gi_sel_menu, &rcPop); /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += ga_sub_menu_count[i-1];
    }
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel = ga_sub_menu + loc;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[labels.num];/*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    for (i=0; i<labels.num; i++) /*ȷ����ǩ�ַ��������λ�ã����������������*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ��������Ӳ˵�������*/
    SMALL_RECT aArea[areas.num];                    /*������������������λ��*/
    char aSort[areas.num];                      /*��������������������Ӧ���*/
    char aTag[areas.num];                         /*����������ÿ�������ı��*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*������λ*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*�������Ϊ0(��ť��)*/
        aTag[i] = i + 1;           /*������˳����*/
    }
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/
    pos.X = rcPop.Left + 2;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        /*��ѭ�������ڿմ��Ӳ���λ�û����γɷָ�����ȡ���˲˵������������*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) /*����Ϊ0������Ϊ�մ�*/
        {
            /*���Ȼ�����*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &num_written);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++)
            {
                /*ȡ���������ַ���Ԫ����������*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*��λ��Ľ�������˵���λ*/
            }
        }

    }
    /*���Ӳ˵���Ĺ��ܼ���Ϊ�׵׺���*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0)
        {
            continue;  /*�����մ�*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &num_written);
    }
    return;
}

/**
 * ��������: PopUp
 * ��������: ��ָ�������������������Ϣ, ͬʱ��������, ����������λ����Ϣ��ջ.
 * �������: pRc ��������λ�����ݴ�ŵĵ�ַ
 *           att �������������ַ�����
 *           pLabel ���������б�ǩ����Ϣ��ŵĵ�ַ
             pHotArea ����������������Ϣ��ŵĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 * ����˵��:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*������������λ���ַ���Ԫ��Ϣ��ջ*/
    size.X = pRc->Right - pRc->Left + 1;    /*�������ڵĿ��*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*�������ڵĸ߶�*/
    /*�����ŵ������������Ϣ�Ķ�̬�洢��*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*���������ڸ���������ַ���Ϣ���棬�����ڹرյ�������ʱ�ָ�ԭ��*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        /*�˶���ѭ�����������ַ���Ԫ��ԭ������ֵ���붯̬�洢���������Ժ�ָ�*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }
    gp_top_layer = nextLayer;  /*��ɵ������������Ϣ��ջ����*/
    /*���õ������������ַ���������*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &num_written);
        pos.Y++;
    }
    /*����ǩ���еı�ǩ�ַ������趨��λ�����*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], NULL);
        }
    }
    /*���õ������������ַ���Ԫ��������*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        /*�˶���ѭ�������ַ���Ԫ�Ĳ��*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {
        /*�˶���ѭ�����������������ַ���Ԫ���������ͺ��������*/
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
 * ��������: PopOff
 * ��������: �رն��㵯������, �ָ���������ԭ��ۺ��ַ���Ԫԭ����.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
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
        /*ջ�״�ŵ���������Ļ��Ϣ�����ùر�*/
        return;
    }
    nextLayer = gp_top_layer->next;
    /*�ָ�������������ԭ���*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*�ָ��ַ���Ԫԭ����*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }
    free(gp_top_layer->pContent);    /*�ͷŶ�̬�洢��*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;
    return;
}

/**
 * ��������: DrawBox
 * ��������: ��ָ�����򻭱߿�.
 * �������: pRc �������λ����Ϣ�ĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 * ����˵��:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+','-','|'};  /*�����õ��ַ�*/
    COORD pos = {pRc->Left, pRc->Top};  /*��λ����������Ͻ�*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, NULL);/*���߿����Ͻ�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {
        /*��ѭ�����ϱ߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, NULL);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, NULL);/*���߿����Ͻ�*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++)
    {
        /*��ѭ�����߿�����ߺ��ұ���*/
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, NULL);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, NULL);
    }
    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, NULL);/*���߿����½�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {
        /*���±߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, NULL);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, NULL);/*���߿����½�*/
    return;
}

/**
 * ��������: TagSubMenu
 * ��������: ��ָ���Ӳ˵�������ѡ�б��.
 * �������: num ѡ�е��Ӳ˵����
 * �������: ��
 * �� �� ֵ: ��
 * ����˵��:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {
        /*����Ӳ˵����Խ�磬������Ӳ˵��ѱ�ѡ�У��򷵻�*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) /*����ȡ��ԭѡ���Ӳ˵����ϵı��*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &num_written);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &num_written);
    }
    /*���ƶ��Ӳ˵�������ѡ�б��*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &num_written);
    gi_sel_sub_menu = num;  /*�޸�ѡ���Ӳ˵����*/
    return;
}

/**
 * ��������: LocSubMenu
 * ��������: ���㵯���Ӳ˵��������ϽǺ����½ǵ�λ��.
 * �������: num ѡ�е����˵����
 * �������: rc �������λ����Ϣ�ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void LocSubMenu(int num, SMALL_RECT *rc){
    int i, len, loc = 0;

    rc->Top = 1; /*������ϱ߶��ڵ�2�У��к�Ϊ1*/
    rc->Left = 1;
    for (i=1; i<num; i++)
    {
        /*����������߽�λ��, ͬʱ�����һ���Ӳ˵������Ӳ˵��ַ��������е�λ��*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*��ʱ��ŵ�һ���Ӳ˵����ַ�������*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++)
    {
        /*������Ӳ˵��ַ��������䳤�ȴ����rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len)
        {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*����������ұ߽�*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*���������±ߵ��к�*/
    if (rc->Right >= SCR_COL)  /*�ұ߽�Խ��Ĵ���*/
    {
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}

/**
 * ��������: RunSys
 * ��������: ����ϵͳ, ��ϵͳ�������������û���ѡ��Ĺ���ģ��.
 * �������: ��
 * �������: hd����ͷָ��ĵ�ַ
 * �� �� ֵ: ��
 * ����˵��:
 */

void RunSys(struct land_node * *hd){
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:�ַ���Ԫ���, cAtt:�ַ���Ԫ����*/
    char vkc, asc;      /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    while (bRet)
    {
        /*�ӿ���̨���뻺�����ж�һ����¼*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) /*�����¼������¼�����*/
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*��ȡ�������λ��*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*ȡ��λ�õĲ��*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*ȡ���ַ���Ԫ����*/
            if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
            {
                /* cAtt > 0 ������λ�ô�������(���˵����ַ���Ԫ)
                 * cAtt != gi_sel_menu ������λ�õ����˵���δ��ѡ��
                 * gp_top_layer->LayerNo > 0 ������ǰ���Ӳ˵�����
                 */
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    PopMenu(cAtt);       /*��������������˵����Ӧ���Ӳ˵�*/
                }
            }
            else if (cAtt > 0) /*�������λ��Ϊ�����Ӳ˵��Ĳ˵����ַ���Ԫ*/
            {
                TagSubMenu(cAtt); /*�ڸ��Ӳ˵�������ѡ�б��*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                    == FROM_LEFT_1ST_BUTTON_PRESSED) /*������������ߵ�һ��*/
            {
                if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                {
                    if (cAtt > 0) /*�����λ�ô�������(���˵����ַ���Ԫ)*/
                    {
                        PopMenu(cAtt);   /*��������������˵����Ӧ���Ӳ˵�*/
                    }
                    /*�����λ�ò��������˵����ַ���Ԫ�������Ӳ˵�����*/
                    else if (gp_top_layer->LayerNo > 0)
                    {
                        PopOff();            /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    }
                }
                else /*��Ų�Ϊ0��������λ�ñ������Ӳ˵�����*/
                {
                    if (cAtt > 0) /*�����λ�ô�������(�Ӳ˵����ַ���Ԫ)*/
                    {
                        PopOff(); /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/

                        /*ִ�ж�Ӧ���ܺ���:gi_sel_menu���˵����,cAtt�Ӳ˵����*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);

                    }
                }
            }
            else if (inRec.Event.MouseEvent.dwButtonState
                     == RIGHTMOST_BUTTON_PRESSED) /*�����������Ҽ�*/
            {
                if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT  /*�����¼�ɰ�������*/
                 && inRec.Event.KeyEvent.bKeyDown) /*�Ҽ�������*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*��ȡ�������������*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*��ȡ������ASC��*/

            /*ϵͳ��ݼ��Ĵ���*/
            if (vkc == 112) /*�������F1��*/
            {
                if (gp_top_layer->LayerNo != 0) /*�����ǰ���Ӳ˵�����*/
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
                bRet = ExeFunction(5, 1);  /*���а������⹦�ܺ���*/
            }
            else if (inRec.Event.KeyEvent.dwControlKeyState
                     & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            {
                /*������������Alt��*/
                switch (vkc)  /*�ж���ϼ�Alt+��ĸ*/
                {
                case 88:  /*Alt+X �˳�*/
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
            else if (asc == 0) /*�������Ƽ��Ĵ���*/
            {
                if (gp_top_layer->LayerNo == 0) /*���δ�����Ӳ˵�*/
                {
                    switch (vkc) /*�������(���ҡ���)������Ӧ�������Ƽ�*/
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
                else  /*�ѵ����Ӳ˵�ʱ*/
                {
                    for (loc=0,i=1; i<gi_sel_menu; i++)
                    {
                        loc += ga_sub_menu_count[i-1];
                    }  /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                    switch (vkc) /*�����(���ҡ��ϡ���)�Ĵ���*/
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
            else if ((asc-vkc == 0) || (asc-vkc == 32))   /*������ͨ��*/
            {
                if (gp_top_layer->LayerNo == 0)  /*���δ�����Ӳ˵�*/
                {
                    switch (vkc)
                    {
                    case 70: /*f��F*/
                        PopMenu(1);
                        break;
                    case 77: /*m��M*/
                        PopMenu(2);
                        break;
                    case 81: /*q��Q*/
                        PopMenu(3);
                        break;
                    case 83: /*s��S*/
                        PopMenu(4);
                        break;
                    case 72: /*h��H*/
                        PopMenu(5);
                        break;
                    case 13: /*�س�*/
                        PopMenu(gi_sel_menu);
                        TagSubMenu(1);
                        break;
                    }
                }
                else /*�ѵ����Ӳ˵�ʱ�ļ������봦��*/
                {
                    if (vkc == 27) /*�������ESC��*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    else if(vkc == 13) /*������»س���*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    }
                    else /*������ͨ���Ĵ���*/
                    {
                        /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++)
                        {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*�����뵱ǰ�Ӳ˵���ÿһ��Ĵ����ַ����бȽ�*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++)
                        {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1])
                            {
                                /*���ƥ��ɹ�*/
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
 * ��������: ExeFunction
 * ��������: ִ�������˵��ź��Ӳ˵���ȷ���Ĺ��ܺ���.
 * �������: m ���˵����
 *           s �Ӳ˵����
 * �������: ��
 * �� �� ֵ: BOOL����,TRUE��FALSE
 * ����˵��: ����ִ�к���ExitSysʱ, �ſ��ܷ���FALSE, ������������Ƿ���TRUE
 */
BOOL ExeFunction(int m, int s)
{
    BOOL  bRet = TRUE;
    /*����ָ�����飬����������й��ܺ�������ڵ�ַ*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*�����ܺ�����ڵ�ַ�����빦�ܺ����������˵��ź��Ӳ˵��Ŷ�Ӧ�±������Ԫ��*/
    pFunction[0] = SaveData;				/**��������*/
    pFunction[1] = BackupData;              /**��������*/
    pFunction[2] = RestoreData;             /**�ָ�����*/
    pFunction[3] = NULL;
    pFunction[4] = ExitSys;                 /**�˳�ϵͳ*/
    pFunction[5] = Maintainl_Info;			/**ά��¥����Ϣ*/
    pFunction[6] = Maintainb_Info;          /**ά��¥����Ϣ*/
    pFunction[7] = Maintainr_Info;          /**ά��������Ϣ*/
    pFunction[8] = Queryl_Info;             /**��ѯ¥����Ϣ*/
    pFunction[9] = Queryb_Info;             /**��ѯ¥����Ϣ*/
    pFunction[10] = Queryr_Info;            /**��ѯ������Ϣ*/
    pFunction[11] = QueryLandArea;          /**���յ�����ѯ¥����Ϣ*/
    pFunction[12] = QueryRoomType;           /**���շ������Ͳ�ѯͨѶ��Ϣ*/
    pFunction[13] = QueryRoomPrice;          /**���շ��ݼ۸��ѯͨѶ��Ϣ*/
    pFunction[14] = NULL;
    pFunction[15] = Queryap;                 /**�����������*/
    pFunction[16] = Stat_a_Info;	        /**����¥�����ڵĳ���ͳ��*/
    pFunction[17] = Stat_t_Info;	        /**���շ��ݵĻ���ͳ��*/
    pFunction[18] = HelpTopic;	 	        /**����*/
    pFunction[19] = AboutDorm;              /**����*/

    for (i=1,loc=0; i<m; i++)  /*�������˵��ź��Ӳ˵��ż����Ӧ�±�*/
    {
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL)
    {
        bRet = (*pFunction[loc])();  /*�ú���ָ�������ָ��Ĺ��ܺ���*/
    }

    return bRet;
}

/**
 * ��������: SaveData
 * ��������: �������ݵĹ��ܺ���.
 * �������: ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE �� FALSE
 * ����˵��: ����ִ�к���ExitSysʱ, �ſ��ܷ���FALSE, ������������Ƿ���TRUE
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

BOOL BackupData(void)                            /*�������ݣ�������������ݱ��ݵ�һ�������ļ�*/
{
    BOOL bRet = TRUE;
    struct land_node *p_l;
    struct block_node *p_b;
    struct room_node *p_r;
    unsigned long l_num = 0;
    unsigned long b_num = 0;
    unsigned long r_num = 0;
    FILE *fhandle;

    /*����ʮ�ֱ��ֱ�ͳ�����ֻ���������Ϣ�ļ�¼����*/
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

    /*��������������ݵļ�¼����*/
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

BOOL RestoreData(void)                          /*�ָ�����*/
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
    /*��ȡ���ֻ�����Ϣ�ļ�¼��*/
    fhandle = fopen(gp_filename,"r+");
    /*��ȡ¥����Ϣ������ʮ��������*/
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
    /*��ȡ¥����Ϣ������¥����Ϣ֧��*/
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

    /*��ȡ������Ϣ������������Ϣ֧��*/
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
    SaveData();                         /*���ڴ��е����ݱ��浽�����ļ�*/
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
    char *pCh[] = {"ȷ���˳�ϵͳ��", "ȷ��      ȡ��"};
    int iHot = 1;
    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
 * ��������: CloseSys
 * ��������: �ر�ϵͳ.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 * ����˵��:
 */
void CloseSys(struct land_node *hd){
    struct land_node * p_l;
    struct block_node * p_b, *p_b_n;
    struct room_node *p_r, *p_r_n;
    while(hd ){
        p_l = hd->next_land;                                /* �ͷ�ʮ�ֽ������������� �Ķ�̬��������*/
        p_b=hd->hd_block;

        while (p_b){                               /* �ͷ�¥�����Ķ�̬��������*/
            p_b_n=p_b->next_block;
            p_r = p_b->hd_room;

            while (p_r){                                 /* �ͷŷ��ݽ��Ķ�̬��������*/
                p_r_n = p_r -> next_room;
                free(p_r);
                p_r=p_r_n;
            }

            free(p_b);
            p_b=p_b_n;
        }
                                                                /* �ͷ�¥�̽��Ķ�̬��������*/
        free(hd);
        hd = p_l;
    }
    ClearScreen();        /*����*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);
    /*�رձ�׼���������豸���*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);
    /*�����ڱ�������Ϊ���н���*/
    SetConsoleTitle("���н���");
    return;
}

BOOL Maintainl_Info(void)                     /**ά��¥����Ϣ*/
{
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1��ɾ��ĳ¥����Ϣ", "2������ĳ¥����Ϣ","3���޸�ĳ¥����Ϣ","����"};
    char *pCh2[] = {"�����룺   ¥�̱��","��ʼ����","ȡ��"};
    char *pCh3[] = {"�����룺 a.¥�̱��          b.¥������         c.¥�̵�ַ         d.��ϵ�绰            e.��������       f.������           g.��ҵ��˾","��ʼ����","ȡ��"};
    char *pCh4[] = {"�����룺 a.Ҫ�޸ĵ�¥����Ϣ�ı��   ","�������µ���Ϣ��b.¥������      c.¥�̵�ַ     d.��ϵ�绰    e. ��������   ","             f.������      g.��ҵ��˾ ","��ʼ����","ȡ��"};
    int iHot = 4;/*�ȼ���������*/
    //����λ��
    pos.X = strlen(pCh[0]) +12;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    labels.num = 4;//����
    labels.ppLabel = pCh;
    //�ֵ�λ��
    COORD aLoc[] = {{rcPop.Left+6, rcPop.Top+2},
        {rcPop.Left+6, rcPop.Top+5},
        {rcPop.Left+6, rcPop.Top+8},
        {rcPop.Left+10, rcPop.Top+12}
    };
    labels.pLoc = aLoc;
    areas.num = 4;
    //��������
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
    PopUp(&rcPop, att, &labels, &areas);//�γɵ���
    DrawBox(&rcPop);
    if (DealConInput(&areas, &iHot) == 13 && iHot == 1)
    {
        /*ɾ��ĳ¥����Ϣ*/
        PopOff();
        iHot = 2;
        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = 0;
        rcPop.Right = SCR_COL-1;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;
        att = BACKGROUND_BLUE| BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
            /*��ʼִ�в��Һ�ɾ������*/
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
            /*����ĳ¥����Ϣ*/
            PopOff();
            iHot = 2;
            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right = SCR_COL-1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;
            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                /*��ʼִ�в��ҺͲ������*/
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
                /*�޸�ĳ¥����Ϣ*/
                PopOff();
                iHot = 2;
                pos.X = 30;
                pos.Y = 14;
                rcPop.Left = 0;
                rcPop.Right = SCR_COL-1;
                rcPop.Top = (25 - pos.Y) / 2;
                rcPop.Bottom = rcPop.Top + pos.Y - 1;
                att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                    /*��ʼִ�в��Һ��޸Ĳ���*/
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
* ��������: DelLandnode
* ��������: ɾ��ָ����¥����Ϣ���
* �������: hd����ͷָ���ַ
*           l_idָ����¥�̱��
* �������: ��
* �� �� ֵ: BOOL����, TRUE��ʾ�޸ĳɹ�, FALSE��ʾ�޸�ʧ��
* ����˵��:
*/

/**ɾ��¥�̽�� */
BOOL DelLandnode(struct land_node * *hd, char * l_id) {
    struct land_node  *prior_l = NULL, *p_l = *hd;
    struct block_node *p_b, *p_b_n;
    struct room_node  *p_r, *p_r_n;
    int sign = 0;
    BOOL  bRet= FALSE;
    /* �ж��Ƿ�Ϊͷָ���������*/
    if (strcmp(l_id,p_l->land_id)==0){
        *hd = p_l -> next_land;
        sign = 1;
    }
    /*�ҵ����������ķ�ͷָ���¥�̽���λ��*/
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
    /* sign ��Ϊ��ʶ��p_lָ��Ҫ��ɾ����land_node�Ľ��*/
    /* ɾ��p_l��ָ��Ľ���block֧���Լ�room֧���Ĵ���ռ�*/
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
* ��������: ModifLandnode
* ��������: �޸�ָ����¥����Ϣ�������
* �������: hd ����ͷָ��
*            l_id ָ����¥�̱��
*            p_modָ���Ų������ָ��
* �������: ��
* �� �� ֵ: BOOL����, TRUE��ʾ�޸ĳɹ�, FALSE��ʾ�޸�ʧ��
* ����˵��:
*/

BOOL ModifLandnode(struct land_node *hd,char *l_id, struct land_node *p_mod){
    struct land_node *l_p = NULL, *l_p_n=NULL;
    l_p = SeekLandnode(hd,l_id);
    if(l_p){
        l_p_n = l_p->next_land;
        p_mod ->hd_block = l_p ->hd_block;  //��ֵǰ����ָ����
        * l_p = * p_mod;
        l_p ->next_land = l_p_n;
        return TRUE;
    }
    else{
        return FALSE;
    }
}
/**
* ��������: AddLandnode
* ��������: ��ʮ�������в���¥����Ϣ���
* �������: hd����ͷָ���ַ
*            p_addָ���Ų������ָ��
* �������: ��
* �� �� ֵ: BOOL����, ����ΪTURE
*
* ����˵��:
*/

/**����¥�̽�� */
BOOL AddLandnode(struct land_node  **hd, struct land_node * p_add){
     struct land_node * p_l = *hd;
     p_add -> next_land = p_l;
     p_l = p_add;
     *hd = p_l;
     return TRUE;
}

BOOL Maintainb_Info(void){                     /**ά��¥����Ϣ*/
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1��ɾ��ĳ¥����Ϣ", "2������ĳ¥����Ϣ","3���޸�ĳ¥����Ϣ","����"};
    char *pCh2[] = {"�����룺  ¥�̱��      ¥����� ","��ʼ����","ȡ��"};
    char *pCh3[] = {"�����룺  a.¥�����        b.¥�̱��        c.��̯��                d.¥��            e.��Ԫ","��ʼ����","ȡ��"};
    char *pCh4[] = {"������ԭ¥������Ϣ��  a.¥�����                   b.¥�̱��    ","������Ҫ�޸ĵ���Ϣ��  c.��̯��        d.¥��        e.��Ԫ"," ","��ʼ����","ȡ��"};
    int iHot = 4;
    pos.X = strlen(pCh[0]) +11;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
        /*ɾ��ĳ¥����Ϣ*/
        PopOff();
        iHot = 2;
        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = 0;
        rcPop.Right = SCR_COL-1 ;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
            /*��ʼִ�в��Һ�ɾ������*/
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
            /*����ĳ¥����Ϣ���*/
            PopOff();
            iHot = 2;
            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right =SCR_COL-1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;
            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                /*��ʼִ�в��ҺͲ������*/
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
                /*�޸�ĳ¥����Ϣ*/
                PopOff();
                iHot = 2;
                pos.X = 48;
                pos.Y = 15;
                rcPop.Left = 0;
                rcPop.Right = SCR_COL-1;
                rcPop.Top = (25 - pos.Y) / 2;
                rcPop.Bottom = rcPop.Top + pos.Y - 1;
                att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                    /*��ʼִ�в��Һ��޸Ĳ���*/
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
 * ��������:  AddBlocknode
 * ��������: ��ʮ�������в���¥����Ϣ���
 * �������: hd ����ͷָ��
 *            p_add ָ���Ų������ָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ�޸ĳɹ�, FALSE��ʾ�޸�ʧ��
 * ����˵��:
 */
 /**����¥�����*/
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
 * ��������:DelBlocknode
 * ��������: ��ָ����¥����Ϣ������ɾ�������������µķ�����Ϣ.
 * �������: hd ����ͷָ��
 *            l_id ¥�̱��  b_id¥�����
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾɾ���ɹ�, FALSE��ʾɾ��ʧ��
 *
 * ����˵��:
 */
 /**ɾ��¥�����*/
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
       if (sign){                     //�ͷ�¥���µķ���֧���Ĵ���ռ�
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
* ��������: ModifBlocknode
* ��������: ��ָ����¥����Ϣ������ݽ����޸�.
* �������: hd ����ͷָ��
*            l_id¥�̱�� b_id ¥�����
*            p_mod ָ�����޸����ݽ���ָ��
* �������: ��
* �� �� ֵ: BOOL����, TRUE��ʾ�޸ĳɹ�, FALSE��ʾ�޸�ʧ��
* ����˵��:
*/
/**�޸�¥����Ϣ*/
BOOL ModifBlocknode(struct land_node *hd, char *l_id, char *b_id,struct block_node * p_mod){
    struct block_node * p_b = NULL, *p_b_n = NULL;
    p_b = SeekBlocknode(hd,l_id,b_id);  //���ҷ���ָ����Ϣ��¥�����
    if(p_b){  //����ҵ���������޸�
        p_b_n = p_b ->next_block;
        p_mod ->hd_room = p_b ->hd_room;                                        //��ֵǰ����ָ����
        * p_b = * p_mod;
        p_b ->next_block = p_b_n;               //�ָ�ָ���ϵ
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/**
 * ��������: SeekLandnode
 * ��������: ��¥�̱�Ų���¥����Ϣ���.
 * �������: hd ����ͷָ��
 *            l_id ¥�̱��
 * �������: ��
 * �� �� ֵ: ����ʱ���ؽ��ĵ�ַ, ���򷵻�NULL
 * ����˵��:
 */
/**��ѯ����������¥��*/
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
 * ��������: SeekBlocknode
 * ��������: ��¥����ţ�¥�̱�Ų���¥����Ϣ���.
 * �������: hd����ͷָ��
 *            l_id ¥�̱�� b_id¥�����
 * �������: ��
 * �� �� ֵ: ����ʱ���ؽ��ĵ�ַ, ���򷵻�NULL
 * ����˵��:
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
 * ��������: SeekRoomnode
* ��������: ��¥����ţ�¥�̱�ţ����ݱ�Ų��ҷ�����Ϣ���.
 * �������: hd����ͷָ��
 *           l_id ¥�̱�� b_id¥����� r_id���ݱ��
 * �������: ��
 * �� �� ֵ: ����ʱ���ؽ��ĵ�ַ, ���򷵻�NULL
 * ����˵��:
 */
/**��ѯ���������ķ��� */
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

BOOL Maintainr_Info(void)                   /*ά��������Ϣ*/
{
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1��ɾ��ĳ������Ϣ", "2������ĳ������Ϣ","3���޸�ĳ������Ϣ","����"};
    char *pCh2[] = {"�����룺   a.¥�̱��     b.¥�����     c.���ݱ��","��ʼ����", "ȡ��"};
    char *pCh3[] = {"�����룺   a.¥�̱��     b.¥�����     c.���ݱ��","�����룺   d.���ݻ���     e. �������    f.�۸�(/m2)","��ʼ����","ȡ��"};
    char *pCh4[] = {" �� �� �� ԭ���ݵģ� a.¥�̱��     b.¥�����     c.���ݱ��","������Ҫ�޸ĵ���Ϣ��   d.���ݻ���     e. �������    f.�۸�(/m2)"," ","��ʼ����","ȡ��"};
    int iHot = 4;
    pos.X = strlen(pCh[0]) +12;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
        /*ɾ��ĳ������Ϣ*/
        PopOff();
        iHot = 2;
        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = 0;
        rcPop.Right = SCR_COL-1;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
            /*��ʼִ�в��Һ�ɾ������*/
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
            /*����ĳ������Ϣ*/
            PopOff();
            iHot = 2;
            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right = SCR_COL-1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;
            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                /*��ʼִ�в��ҺͲ������*/
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
                /*�޸�ĳ������Ϣ*/
                PopOff();
                iHot = 2;
                pos.X = 30;
                pos.Y = 14;
                rcPop.Left = 0;
                rcPop.Right = SCR_COL-1;
                rcPop.Top = (25 - pos.Y) / 2;
                rcPop.Bottom = rcPop.Top + pos.Y - 1;
                att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                    /*��ʼִ�в��Һ��޸Ĳ���*/
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
 * ��������: AddRoomnode
 * ��������: ��ʮ�������в���һ��������Ϣ���.
 * �������: hd����ͷָ��
 *            p_addָ����Ҫ�������ָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ����ɹ�, FALSE��ʾ����ʧ��
 * ����˵��:
 */
 /**���ӷ��ݽڵ�*/
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
 * ��������: DelRoomnode
 * ��������: ��ʮ��������ɾ��ָ���ķ�����Ϣ���.
 * �������: hd ����ͷָ��
 *            l_id¥�̱��
 *            b_id¥�����
 *            r_id���ݱ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾɾ���ɹ�, FALSE��ʾɾ��ʧ��
 * ����˵��: ����¥�̱�š�¥����źͷ��ݱ�ſ���ȷ��Ψһ�ķ�����Ϣ
 */

 BOOL DelRoomnode (struct land_node *hd , char * l_id, char * b_id, char * r_id){
    struct block_node * p_b;
    struct room_node  * p_r, * prior_r;
    int sign = 0;

    p_b = SeekBlocknode(hd,l_id,b_id);    //�ҵ�����������¥�����
    if(p_b){     //����ҵ�
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
            if(p_r){                     //�ҵ����������ķ��ݽ��
                prior_r->next_room = p_r ->next_room;
                sign = 1;
            }
        }
    }
    if(sign){
        free(p_r);                        //����ɾ������
        return TRUE;
    }
    return FALSE;
}

/**
 * ��������:  ModifRoomnode
 * ��������: ��ָ���ķ�����Ϣ������ݽ����޸�.
 * �������: hd ����ͷָ��
 *            l_id¥�̱��
 *            b_id¥�����
 *            r_id���ݱ��
 *            p_mod ָ�����޸����ݽ���ָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ�޸ĳɹ�, FALSE��ʾ�޸�ʧ��
 * ����˵��:
 */
/**�޸ķ�����Ϣ*/
BOOL ModifRoomnode(struct land_node *hd, char *l_id, char *b_id, char *r_id ,struct room_node *p_mod){
         struct room_node *p_r = NULL, *p_r_n = NULL;
         p_r = SeekRoomnode(hd,l_id,b_id,r_id);  //����ָ���ķ�����
         if(p_r){        //����ҵ�
            p_r_n = p_r ->next_room;    //��ֵǰ����ָ����
            * p_r = * p_mod;
            p_r->next_room = p_r_n;   //�ָ�ָ��
            return TRUE;
         }
         else{
            return FALSE;
         }
}

BOOL Success(void)                 /*ĳ������ɹ������������Ѵ���*/
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�����ɹ�", "����"};
    int iHot = 1;
    pos.X = strlen(pCh[0]) +6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED|FOREGROUND_RED|FOREGROUND_INTENSITY;  /*�׵׺���*/
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

BOOL Fail(void)                    /*ĳ�����ʧ�ܣ����������Ѵ���*/
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"����ʧ��", "����"};
    int iHot = 1;
    pos.X = strlen(pCh[0]) +6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED|FOREGROUND_RED|FOREGROUND_INTENSITY;  /*�׵׺���*/
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

 BOOL Queryl_Info(void)                        /*��ѯ¥����Ϣ*/
{
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�����룺    ¥�̱��","��ʼ����","ȡ��"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
            /*���¥����Ϣ����Ļ*/

            SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            printf("\n");
            printf("¥�̱�ţ�%s\n",p->land_id);
            printf("¥�����ƣ�%s\n",p->land_name);
            printf("¥�̵�ַ��%s\t¥�̵���;%s\n",p->land_add,p->land_area);
            printf("¥�̵绰��%s\n",p->land_tel);
            printf("�� �� �̣�%s\n",p->developer);
            printf("��ҵ��˾��%s\n",p->pro_man_com);
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

BOOL Queryb_Info(void)                        /*��ѯ¥����Ϣ*/
{
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�����룺¥�̱��   ¥�����","��ʼ����","ȡ��"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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

            /*���¥����Ϣ����Ļ*/
            struct room_node * p_r = NULL;
            int n = 1;
            SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            printf("\n");
            printf("¥�̱�ţ�%s\t",p->land_id);
            printf("¥����ţ�%s\t",p->block_id);
            printf("�� ̯ �ʣ�%s\n",p->pool_rote);
            printf(" ��  Ԫ ��%s\n",p->unit);
            printf(" ¥  �� ��%s\n",p->floor);
            printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
            printf("\t��¥���µ�¥����Ϣ����\n");
            printf("\t����      \t���\t�۸�\n");
            for(p_r = p ->hd_room; p_r != NULL; p_r= p_r ->next_room)
            {
                if(n == 0){
                    printf("\t��¥���µ�¥����Ϣ����\n");
                    printf("���ݱ��       ����       \t���     \t�۸�   \n");
                }

                printf("%s\t    %s\t      %s\t  %s\n",p_r->room_id,p_r->type,p_r->area,p_r->price);
                n++;

                if(n == 10){
                    printf("\n��������鿴��һҳ�ķ�����Ϣ\n");
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


BOOL Queryr_Info(void)                        /*��ѯ¥����Ϣ*/
{
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"������:¥�̱��,¥�����,���ݱ��","��ʼ����","ȡ��"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
            /*���¥����Ϣ����Ļ*/

            SetConsoleCursorPosition(gh_std_out,(COORD){1, 1});
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            printf("\n");
            printf("¥�̱��:%s\t",p->land_id);
            printf("¥�����:%s\t",p->block_id);
            printf("¥�����:%s\n",p->room_id);
            printf("��������:%s\n",p->type );
            printf("���%s\n",p->area);
            printf("�۸�%s\n",p->price);
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
    char *pCh[] = {"�����룺   ����    ","��ʼ����","ȡ��"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
            /*���¥����Ϣ����Ļ*/

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

///��ѯָ��������¥��ͨѶ��Ϣ
BOOL SeekLandArea(struct land_node *hd , char * area, int n){
    struct land_node * p_l = hd;
    int sign = 0;
    BOOL Bre = FALSE;
    while(p_l){
        if(strcmp(p_l->land_area, area) == 0){
            Bre = TRUE;
           if(n){
                if(sign == 0 ){
                    printf("\n�� * %s * ��¥�̵�ͨѶ��ϢΪ",area);

                }

                sign = 1;
                printf("\n¥�̱��: %s\n¥������: %s\n¥�̵�ַ: %s\n��ϵ��ʽ: %s",p_l->land_id,p_l->land_name,p_l->land_area,p_l->land_tel);

           }
        }
        p_l = p_l->next_land;
    }

    return Bre;
}

/*�������Ͳ�ѯ����ͨѶ��Ϣ*/
BOOL QueryRoomType(void){
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�����룺   ��������","��ʼ����","ȡ��"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
  ///��ѯ���Ϸ��͵ķ�����Ϣ
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
                        printf(" \n\n����¥����Ϣ\n¥������: %s\n¥�̱��: %s\n¥�̵�ַ: %s\n��ϵ��ʽ: %s\n",p_l->land_name,p_l->land_id,p_l->land_add,p_l->land_tel);
                        printf("¥���š������\t¥���š������\t¥���š������\t¥���š������\n");
                        sign = 0;
                        next += 5;
                    }
                    printf(" %s��-%s��\t",p_b->block_id,p_r->room_id);
                    n++;
                    if( n % 4 == 0){
                        printf("\n");
                        next ++;
                        n = 0;
                    }
                     if(next % 15 == 0){

                            printf("\n��������鿴��һҳ�еķ��������ķ�����Ϣ\n");
                            getch();
                            ClearScreen();
                            printf(" \n����¥����Ϣ\n¥������: %s\n¥�̱��: %s\n¥�̵�ַ: %s\n��ϵ��ʽ: %s\n",p_l->land_name,p_l->land_id,p_l->land_add,p_l->land_tel);
                            printf("¥���š������\t¥���š������\t¥���š������\t¥���š������\n");
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

/**���ռ۸��ѯ����ͨѶ��Ϣ*/
BOOL QueryRoomPrice(void){
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"������۸�Χ�����-��ߣ�","��ʼ����","ȡ��"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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

///��ѯ���ϼ۸�ķ���ͨѶ��Ϣ
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
                        printf("\n����¥����Ϣ:\t¥������:%s \t¥�̱��:%s\n\t\t¥�̵�ַ:%s\n\t\t��ϵ��ʽ:%s",p_l->land_name,p_l->land_id,p_l->land_add,p_l->land_tel);
                        printf("\n¥��������� \t�۸�  ¥��������� \t�۸�  ¥���������   \t�۸�\n");
                        sign = 0;
                        next += 5;

                    }
                    printf(" %s��-%s��\t%s\t",p_b->block_id,p_r->room_id,p_r->price);
                    n++;
                    if( n % 4 == 0){
                        printf("\n");
                        next  ++ ;
                        n = 1;
                    }
                     if(next == 17){
                            printf("\n��������鿴��һҳ�еķ��������ķ�����Ϣ\n");
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

/** ���������ѯ������Ϣ����*/
BOOL Queryap(void){
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"��������ϲ���ĳ���  �Լ��������ļ۸�Χ","��ʼ����","ȡ��"};
    int iHot = 2;
    pos.X = 40;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  //�׵׺���
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
            printf("\n¥�̡�¥�������ݱ��     ��ϵ��ʽ        ����      �۸�/m2        ��̯��   ");
        }
        while(p_l){
            if(strcmp(area,p_l->land_area)==0){    //������������
                p_b = p_l->hd_block;
                while(p_b){
                    p_r = p_b ->hd_room;
                    while(p_r){
                                   // �ڷ���֧���� Ѱ�ҷ��ϼ۸������ķ���
                        if(find(h_price,l_price,p_r->price)){
                            re= TRUE;
                            if(sign){
                            printf("\n%s",p_r->land_id);
                            printf("��%s",p_r->block_id);
                            printf("��%s",p_r->room_id);
                            printf(" %s",p_l->land_tel);
                            printf("   \t%s",p_r->type);
                            printf("\t%s",p_r->price);
                            printf("  %s%\n",p_b->pool_rote);
                            n++;
                            }
                            if( n == 10){
                            printf("\n��������鿴��һҳ����Ϣ\n");
                            getch();
                            ClearScreen();
                            printf("\n¥�̡�¥�������ݱ��     ��ϵ��ʽ       ����    �۸�/m2        ��̯��   ");
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

///��¥�����ڵĵ���ͳ��
BOOL Stat_a_Info(void){
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�����룺  ��Ҫͳ�Ƶĳ���","��ʼ����","ȡ��"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  //�׵׺���
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
        struct land_node *l_p = StAreanode(hd_land,area);  //��ѯ����������¥����Ϣ

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
            printf("\n%s ��������¥�� : %d  ��\n", area, n);

            printf("\n¥�̻�����Ϣ���£�\n");
            p = l_p;
            while(l_p){
                    printf("\n¥�̱��:%s \n¥������ : %s\n��ϵ��ʽ: %s \n������: %s\n",l_p->land_id,l_p->land_name,l_p->land_tel,l_p->developer);
                    l_p = l_p->next_land;
            }
            printf("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
            printf("\n�������֪��������ϸ��¥����Ϣ���밴��¥�̱�Ų�ѯ");
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

///Ѱ��ָ��������¥����Ϣ
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


BOOL Stat_t_Info(void)        /*�����ݵĻ���ͳ��    */
{
    InitInterface();
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"������Ҫͳ�Ƶķ��ݵ����ͣ�      ","��ʼ����","ȡ��"};
    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
        /**���շ�������ͳ�ƺ���*/

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
            printf(" *  %s  *�͵ķ��ݵ�ͳ�Ƶ���Ϣ ������£�\n",type);


            while(r_p) {
                printf("\n ¥�� %s: %d �� \n",r_p->land_id,r_p->num);
                p = r_p -> hd_block_sta;
                if(p){
                    printf("��¥������\n");
                    printf("¥����--������\t¥����--������\t¥����--������\t¥����--������\t¥����--������\n");
                }

                while(p){
                printf("  %s��  %d��\t",p->block_id,p->num);
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

/**ͳ�Ʒ��ϻ��������ķ�������*/
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
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    SetHotPoint(pHotArea, *piHot);
    while (TRUE)
    {
        /*ѭ��*/
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
                    /*�����(���ϡ��ҡ���)�Ĵ���*/
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
                /*ESC��*/
                iRet = 27;
                break;
            }
            else if (vkc == 13 || vkc == 32)
            {
                /*�س�����ո��ʾ���µ�ǰ��ť*/
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
    att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
    att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    for (i=0; i<pHotArea->num; i++)
    {
        /*����ť��������Ϊ�׵׺���*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0)
        {
            /*�����ǰ�ť��*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &num_written);
        }
    }
    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0)
    {
        /*�����������ǰ�ť��*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &num_written);
    }
    else if (pHotArea->pSort[iHot-1] == 1)
    {
        /*�������������ı�����*/
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
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>¥�̹���ϵͳ>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n\n\n\n\n\n");
    printf("\n\t\t***********************************");
    printf("\n\t\t*     ��Ϣ��ȫ1203 ������       *");
    printf("\n\t\t*                                 *");
    printf("\n\t\t*     ��ϵ��ʽ��13260539228       *");
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
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>����>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    printf("1.����¼���ɾ�����ݺ���б��档\n");
    printf("2.���밴��Ų���ĳ��¥�̣�¥����������ʹ�����ݲ�ѯ��\n");
    printf("3.����밴�ռ۸�ͳ������ҷ��ݵĻ�����Ϣ����ʹ����ϲ��ҡ�\n");
    printf("4.���������ϲ���Ļ��͵ķ��ݣ���ϲ���ĳ�����¥����Ϣ��ʹ������ͳ��\n");
    lp.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out,plp);
}
