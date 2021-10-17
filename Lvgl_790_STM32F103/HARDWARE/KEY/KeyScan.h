#ifndef KeyScan_H
#define KeyScan_H

#define KEY_DIR_EFFECT_DEFAULT      0//�������Ұ�����Ч������һ�㲻�ᷢ���������Ұ������µ���Ϣ
#define KEY_DIR_TO_START            1//���Ұ�����Ч��һ�������б���ͼ(�б���ͼ�н϶�����ʱ�����ͻ�ҳ)����ͼ��ؼ��������׻���βʱ���������TOEND �Ҽ�����TOSTART
#define KEY_DIR_TO_END              2//���Ұ�����Ч��һ�������б���ͼ(�б���ͼ�н϶�����ʱ�����ͻ�ҳ)����ͼ��ؼ��������׻���βʱ���������TOEND �Ҽ�����TOSTART
#define KEY_DIR_LR_PAGEDOWN3        3//���Ұ������ض����淢��PAGEDOWN��Ϣ����������Ӱ�죬Ŀǰʹ�õ���Ϊ ���������Ͷ��ӹ����������
#define KEY_DIR_UP_VAL              4//����ֵ���棬���޸����������ֵ�����UP�𵽱���
#define KEY_DIR_UD_PAGEDOWN         5//���°�������PAGEDOWN��Ϣ��������Ч
#define KEY_DIR_LR_PAGEDOWN6        6//���Ұ������ض����淢��PAGEDOWN��Ϣ����������Ӱ�죬Ŀǰʹ�õ���Ϊʾ�������ܽ���
#define KEY_DIR_UD_CHANGE_PAGE      7//��ؽ��滻ҳ

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
    KEY_FUN=0x100,//�л��۽�
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
extern SEND_FLAG eSendFlag;//����ʲô��Ϣ ��Ӧ��λ��1�Ϳ���

extern KEY_STATE xKeyState;

extern unsigned char ucSendKeyFlag;
extern unsigned char ucHallFlag;
extern unsigned short usControlInfo;
extern unsigned char ucEditMoveFlag;

extern void KeyScan_KeyMessage(void);

#endif
