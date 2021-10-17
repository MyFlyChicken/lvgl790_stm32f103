#ifndef KeyScan_H
#define KeyScan_H

#define KEY_DIR_EFFECT_DEFAULT      0//上下左右按键有效，非零一般不会发送上下左右按键按下的消息
#define KEY_DIR_TO_START            1//左右按键无效，一般用于列表视图(列表视图有较多数据时，发送换页)，有图标控件且在行首或行尾时，左键发送TOEND 右键发送TOSTART
#define KEY_DIR_TO_END              2//左右按键无效，一般用于列表视图(列表视图有较多数据时，发送换页)，有图标控件且在行首或行尾时，左键发送TOEND 右键发送TOSTART
#define KEY_DIR_LR_PAGEDOWN3        3//左右按键在特定界面发生PAGEDOWN消息，对上下无影响，目前使用到的为 参数拷贝和段子功能这个界面
#define KEY_DIR_UP_VAL              4//在数值界面，若修改数大于最大值，会对UP起到保护
#define KEY_DIR_UD_PAGEDOWN         5//上下按键发送PAGEDOWN消息，左右无效
#define KEY_DIR_LR_PAGEDOWN6        6//左右按键在特定界面发生PAGEDOWN消息，对上下无影响，目前使用到的为示波器功能界面
#define KEY_DIR_UD_CHANGE_PAGE      7//监控界面换页

typedef  enum _KEY_STATE_{
    KEY_NONE=0x00,
    KEY_PRG=0x01,
    KEY_UP=0x02,
    KEY_ENTER=0x04,
    KEY_LEFT=0x08,
    KEY_DOWN=0x10,
    KEY_RIGHT=0x20,
    KEY_RUN=0x40,
    KEY_STOP_RES=0x80,
    KEY_FUN=0x100,//切换聚焦
}KEY_STATE;

typedef enum _SEND_FLAG_{
    Send_Default = 0x01,
    Send_AddLine = 0x02,
    Send_Insert = 0x04,
    Send_Tab = 0x08,
    Send_ToLastWin = 0x10,
    Send_NextPage = 0x20,
    Send_None = 0x40
}SEND_FLAG;



extern unsigned char ucLRKeyDefault;
extern SEND_FLAG eSendFlag;//发送什么信息 对应的位置1就可以

extern KEY_STATE xKeyState;

extern unsigned char ucSendKeyFlag;
extern unsigned char ucHallFlag;
extern unsigned short usControlInfo;
extern unsigned char ucEditMoveFlag;

extern void KeyScan_KeyMessage(void);

#endif
