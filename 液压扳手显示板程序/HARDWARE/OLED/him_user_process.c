#include "him_user_process.h"
#include "him_user_driver.h"
#include "Hydraulic_process.h"
#include "usart.h"
#include "rtc.h" 
#include "Data_process.h"

uint16 current_screen_id = 0;     //��ǰ����ID
uint16 previous_screen_id = 6;    //֮ǰ����ID


/*! 
*  \brief     �����л�
*  \param  screen_id   Ҫ�л��Ļ���iD
*/
void HIM_Home_Switcher(uint16 screen_id)
{
        switch(screen_id)
        {
            case  Menu_Main_Interface:
                    SetScreen(0x00);
            break;
            case  Menu_Torque_Interface:
                    SetScreen(0x01);
            break;
            case  Menu_Bolt_Interface:
                    SetScreen(0x02);
            break;
            case  Menu_Angle_Interface:
                    SetScreen(0x03);
            break;
            case  Menu_Gear_Interface:
                    SetScreen(0x04);
            break;
            case  Menu_Sensor_Interface:
                    SetScreen(0x05);
            break;
        }
}
/*! 
*  \brief     ѹ��ֵ��ʾ
*  \param  screen_id   ��ǰ����iD
*  \param  control_id   �ؼ�id
*  \param  value   ѹ��ֵ
*  \param  Unit   ѹ����λ
*/
void HIM_Pressure_display(uint16 screen_id,uint16 control_id,float value,u8 Unit)
{
    float Data = 0;
    switch(Unit)
    {
        case 0:   //bar
              Data = value*10.0;
            SetTextFloat(screen_id,control_id, Data,1,0);
        break;
        case 1:   //Mpa
            Data = value;
            SetTextFloat(screen_id,control_id, Data,1,1);
        break;
        case 2:   //psi
            Data = value*145.0;
            SetTextFloat(screen_id,control_id, Data,1,0);
        break;
    }  
}


/*! 
*  \brief     ����ָ����
*  \param  screen_id   ��ǰ����iD
*  \param  Cursor_id   �����
*/
void HIM_Cursor(uint16 screen_id,uint16 Cursor_id)
{
    if(screen_id == Menu_Main_Interface)    //�˵�����ѡ���
    {
        switch(Cursor_id)
        {
            case 0:
                SetControlForeColor(0,1,Bule);
                SetControlForeColor(0,2,Bule);
                SetControlForeColor(0,3,Bule);
                SetControlForeColor(0,4,Bule);
                SetControlForeColor(0,5,Bule);
                SetControlForeColor(0,6,Bule);
                SetControlForeColor(0,7,Bule);
                SetControlForeColor(0,8,Bule);
                break;
            case 1:
                SetControlForeColor(0,1,Green);
                SetControlForeColor(0,2,Bule);
                SetControlForeColor(0,3,Bule);
                SetControlForeColor(0,4,Bule);
                SetControlForeColor(0,5,Bule);
                SetControlForeColor(0,6,Bule);
                SetControlForeColor(0,7,Bule);
                SetControlForeColor(0,8,Bule);
                break;
            case 2:
                SetControlForeColor(0,1,Bule);
                SetControlForeColor(0,2,Green);
                SetControlForeColor(0,3,Bule);
                SetControlForeColor(0,4,Bule);
                SetControlForeColor(0,5,Bule);
                SetControlForeColor(0,6,Bule);
                SetControlForeColor(0,7,Bule);
                SetControlForeColor(0,8,Bule);
                break;
            case 3:
                SetControlForeColor(0,1,Bule);
                SetControlForeColor(0,2,Bule);
                SetControlForeColor(0,3,Green);
                SetControlForeColor(0,4,Bule);
                SetControlForeColor(0,5,Bule);
                SetControlForeColor(0,6,Bule);
                SetControlForeColor(0,7,Bule);
                SetControlForeColor(0,8,Bule);
                break;
            case 4:
                SetControlForeColor(0,1,Bule);
                SetControlForeColor(0,2,Bule);
                SetControlForeColor(0,3,Bule);
                SetControlForeColor(0,4,Green);
                SetControlForeColor(0,5,Bule);
                SetControlForeColor(0,6,Bule);
                SetControlForeColor(0,7,Bule);
                SetControlForeColor(0,8,Bule);
                break;
            case 5:
                SetControlForeColor(0,1,Bule);
                SetControlForeColor(0,2,Bule);
                SetControlForeColor(0,3,Bule);
                SetControlForeColor(0,4,Bule);
                SetControlForeColor(0,5,Green);
                SetControlForeColor(0,6,Bule);
                SetControlForeColor(0,7,Bule);
                SetControlForeColor(0,8,Bule);
                break;
            case 6:
                SetControlForeColor(0,1,Bule);
                SetControlForeColor(0,2,Bule);
                SetControlForeColor(0,3,Bule);
                SetControlForeColor(0,4,Bule);
                SetControlForeColor(0,5,Bule);
                SetControlForeColor(0,6,Green);
                SetControlForeColor(0,7,Bule);
                SetControlForeColor(0,8,Bule);
                break;
            case 7:
                SetControlForeColor(0,1,Bule);
                SetControlForeColor(0,2,Bule);
                SetControlForeColor(0,3,Bule);
                SetControlForeColor(0,4,Bule);
                SetControlForeColor(0,5,Bule);
                SetControlForeColor(0,6,Bule);
                SetControlForeColor(0,7,Green);
                SetControlForeColor(0,8,Bule);
                break;
            case 8:
                SetControlForeColor(0,1,Bule);
                SetControlForeColor(0,2,Bule);
                SetControlForeColor(0,3,Bule);
                SetControlForeColor(0,4,Bule);
                SetControlForeColor(0,5,Bule);
                SetControlForeColor(0,6,Bule);
                SetControlForeColor(0,7,Bule);
                SetControlForeColor(0,8,Green);
                break;
        }
    }
    else if(screen_id == Menu_Torque_Interface)     //Ť�ؽ���ѡ���
    {
         switch(Cursor_id)
        {
            case 0:
                SetControlForeColor(1,1,Bule);
                SetControlForeColor(1,3,Bule);
                SetControlForeColor(1,4,Bule);
                SetControlForeColor(1,5,Bule);
                SetControlForeColor(1,7,Bule);
                break;
            case 1:
                SetControlForeColor(1,1,Green);
                SetControlForeColor(1,3,Bule);
                SetControlForeColor(1,4,Bule);
                SetControlForeColor(1,5,Bule);
                SetControlForeColor(1,7,Bule);
                break;
            case 2:
                SetControlForeColor(1,1,Bule);
                SetControlForeColor(1,3,Green);
                SetControlForeColor(1,4,Bule);
                SetControlForeColor(1,5,Bule);
                SetControlForeColor(1,7,Bule);
                break;
            case 3:
                SetControlForeColor(1,1,Bule);
                SetControlForeColor(1,3,Bule);
                SetControlForeColor(1,4,Green);
                SetControlForeColor(1,5,Bule);
                SetControlForeColor(1,7,Bule);
                break;
            case 4:
                SetControlForeColor(1,1,Bule);
                SetControlForeColor(1,3,Bule);
                SetControlForeColor(1,4,Bule);
                SetControlForeColor(1,5,Green);
                SetControlForeColor(1,7,Bule);
                break;
            case 5:
                SetControlForeColor(1,1,Bule);
                SetControlForeColor(1,3,Bule);
                SetControlForeColor(1,4,Bule);
                SetControlForeColor(1,5,Bule);
                SetControlForeColor(1,7,Green);
                break;
        }
    }
    else if(screen_id == Menu_Bolt_Interface)   //��˨����ѡ���
    {
        switch(Cursor_id)
        {
            case 0:   //���ѡ��
                SetControlForeColor(2,1,Bule);
                SetControlForeColor(2,2,Bule);
                SetControlForeColor(2,4,Bule);
                SetControlForeColor(2,6,Bule);
                break;
            case 1:   //ѡ���1��
                SetControlForeColor(2,1,Green);
                SetControlForeColor(2,2,Bule);
                SetControlForeColor(2,4,Bule);
                SetControlForeColor(2,6,Bule);
                break;
            case 2:   //ѡ���2��
                SetControlForeColor(2,1,Bule);
                SetControlForeColor(2,2,Green);
                SetControlForeColor(2,4,Bule);
                SetControlForeColor(2,6,Bule);
                break;
            case 3:   //ѡ���3��
                SetControlForeColor(2,1,Bule);
                SetControlForeColor(2,2,Bule);
                SetControlForeColor(2,4,Green);
                SetControlForeColor(2,6,Bule);
                break;
            case 4:   //ѡ���4��
                SetControlForeColor(2,1,Bule);
                SetControlForeColor(2,2,Bule);
                SetControlForeColor(2,4,Bule);
                SetControlForeColor(2,6,Green);
                break;
        }
    }
    else if(screen_id == Menu_Angle_Interface)   //�ǶȽ���ѡ���
    {
        switch(Cursor_id)
        {
            case 0:   //���ѡ��
                SetControlForeColor(3,1,Bule);
                SetControlForeColor(3,2,Bule);
                SetControlForeColor(3,3,Bule);
                SetControlForeColor(3,4,Bule);
                SetControlForeColor(3,6,Bule);
                break;
            case 1:   //ѡ���һ��
                SetControlForeColor(3,1,Green);
                SetControlForeColor(3,2,Bule);
                SetControlForeColor(3,3,Bule);
                SetControlForeColor(3,4,Bule);
                SetControlForeColor(3,6,Bule);
                break;
            case 2:   //ѡ��ڶ���
                SetControlForeColor(3,1,Bule);
                SetControlForeColor(3,2,Green);
                SetControlForeColor(3,3,Bule);
                SetControlForeColor(3,4,Bule);
                SetControlForeColor(3,6,Bule);
                break;
            case 3:   //ѡ�������
                SetControlForeColor(3,1,Bule);
                SetControlForeColor(3,2,Bule);
                SetControlForeColor(3,3,Green);
                SetControlForeColor(3,4,Bule);
                SetControlForeColor(3,6,Bule);
                break;
            case 4:   //ѡ����ĸ�
                SetControlForeColor(3,1,Bule);
                SetControlForeColor(3,2,Bule);
                SetControlForeColor(3,3,Bule);
                SetControlForeColor(3,4,Green);
                SetControlForeColor(3,6,Bule);
                break;
            case 5:   //ѡ������
                SetControlForeColor(3,1,Bule);
                SetControlForeColor(3,2,Bule);
                SetControlForeColor(3,3,Bule);
                SetControlForeColor(3,4,Bule);
                SetControlForeColor(3,6,Green);
                break;
        }
    }
    else if(screen_id == Menu_Gear_Interface)   //��λ����ѡ���
    {
        switch(Cursor_id)
        {
            case 0:   //���ѡ��
                SetControlForeColor(4,1,Bule);
                SetControlForeColor(4,2,Bule);
                SetControlForeColor(4,4,Bule);
                SetControlForeColor(4,6,Bule);
                break;
            case 1:   //ѡ���һ��
                SetControlForeColor(4,1,Green);
                SetControlForeColor(4,2,Bule);
                SetControlForeColor(4,4,Bule);
                SetControlForeColor(4,6,Bule);
                break;
            case 2:   //ѡ��ڶ���
                SetControlForeColor(4,1,Bule);
                SetControlForeColor(4,2,Green);
                SetControlForeColor(4,4,Bule);
                SetControlForeColor(4,6,Bule);
                break;
            case 3:   //ѡ��ڸ�
                SetControlForeColor(4,1,Bule);
                SetControlForeColor(4,2,Bule);
                SetControlForeColor(4,4,Green);
                SetControlForeColor(4,6,Bule);
                break;
            case 4:   //ѡ����ĸ�
                SetControlForeColor(4,1,Bule);
                SetControlForeColor(4,2,Bule);
                SetControlForeColor(4,4,Bule);
                SetControlForeColor(4,6,Green);
                break;
        }
    }
    else if(screen_id == Menu_Sensor_Interface)   //����������ѡ���
    {
        switch(Cursor_id)
        {
            case 0:   //���ѡ��
                SetControlForeColor(5,1,Bule);
                SetControlForeColor(5,2,Bule);
                SetControlForeColor(5,3,Bule);
                SetControlForeColor(5,4,Bule);
                SetControlForeColor(5,5,Bule);
                SetControlForeColor(5,7,Bule);
                break;
            case 1:   //ѡ���һ��
                SetControlForeColor(5,1,Green);
                SetControlForeColor(5,2,Bule);
                SetControlForeColor(5,3,Bule);
                SetControlForeColor(5,4,Bule);
                SetControlForeColor(5,5,Bule);
                SetControlForeColor(5,7,Bule);
                break;
            case 2:   //ѡ��ڶ���
                SetControlForeColor(5,1,Bule);
                SetControlForeColor(5,2,Green);
                SetControlForeColor(5,3,Bule);
                SetControlForeColor(5,4,Bule);
                SetControlForeColor(5,5,Bule);
                SetControlForeColor(5,7,Bule);
                break;
            case 3:   //ѡ�������
                SetControlForeColor(5,1,Bule);
                SetControlForeColor(5,2,Bule);
                SetControlForeColor(5,3,Green);
                SetControlForeColor(5,4,Bule);
                SetControlForeColor(5,7,Bule);
                break;
            case 4:   //ѡ����ĸ�
                SetControlForeColor(5,1,Bule);
                SetControlForeColor(5,2,Bule);
                SetControlForeColor(5,3,Bule);
                SetControlForeColor(5,4,Green);
                SetControlForeColor(5,5,Bule);
                SetControlForeColor(5,7,Bule);
                break;
            case 5:   //ѡ������
                SetControlForeColor(5,1,Bule);
                SetControlForeColor(5,2,Bule);
                SetControlForeColor(5,3,Bule);
                SetControlForeColor(5,4,Bule);
                SetControlForeColor(5,5,Green);
                SetControlForeColor(5,7,Bule);
                break;
            case 6:   //ѡ�������
                SetControlForeColor(5,1,Bule);
                SetControlForeColor(5,2,Bule);
                SetControlForeColor(5,3,Bule);
                SetControlForeColor(5,4,Bule);
                SetControlForeColor(5,5,Bule);
                SetControlForeColor(5,7,Green);
                break;
        }
    }      
}

/*! 
*  \brief     �ı�������˸
*  \param  screen_id   ����ID
*  \param  Cursor_id   ѡ��ID
*/
void Variable_Blink(uint16 screen_id,uint16 Cursor_id)
{
     if(screen_id == Menu_Main_Interface)    //�˵�����ѡ���
    {
        switch(Cursor_id)
        {
            case 0:
                SetTextFlickerCycle(0,1,0); //�ı���˸
                SetTextFlickerCycle(0,2,0); //�ı���˸
                SetTextFlickerCycle(0,3,0); //�ı���˸
                SetTextFlickerCycle(0,4,0); //�ı���˸
                SetTextFlickerCycle(0,5,0); //�ı���˸
                SetTextFlickerCycle(0,6,0); //�ı���˸
                SetTextFlickerCycle(0,7,0); //�ı���˸
                SetTextFlickerCycle(0,8,0); //�ı���˸
                break;
            case 1:
                SetTextFlickerCycle(0,1,50); //�ı���˸
                SetTextFlickerCycle(0,2,0); //�ı���˸
                SetTextFlickerCycle(0,3,0); //�ı���˸
                SetTextFlickerCycle(0,4,0); //�ı���˸
                SetTextFlickerCycle(0,5,0); //�ı���˸
                SetTextFlickerCycle(0,6,0); //�ı���˸
                SetTextFlickerCycle(0,7,0); //�ı���˸
                SetTextFlickerCycle(0,8,0); //�ı���˸
                break;
            case 2:
                SetTextFlickerCycle(0,1,0); //�ı���˸
                SetTextFlickerCycle(0,2,50); //�ı���˸
                SetTextFlickerCycle(0,3,0); //�ı���˸
                SetTextFlickerCycle(0,4,0); //�ı���˸
                SetTextFlickerCycle(0,5,0); //�ı���˸
                SetTextFlickerCycle(0,6,0); //�ı���˸
                SetTextFlickerCycle(0,7,0); //�ı���˸
                SetTextFlickerCycle(0,8,0); //�ı���˸
                break;
            case 3:
                SetTextFlickerCycle(0,1,0); //�ı���˸
                SetTextFlickerCycle(0,2,0); //�ı���˸
                SetTextFlickerCycle(0,3,50); //�ı���˸
                SetTextFlickerCycle(0,4,0); //�ı���˸
                SetTextFlickerCycle(0,5,0); //�ı���˸
                SetTextFlickerCycle(0,6,0); //�ı���˸
                SetTextFlickerCycle(0,7,0); //�ı���˸
                SetTextFlickerCycle(0,8,0); //�ı���˸
                break;
            case 4:
                SetTextFlickerCycle(0,1,0); //�ı���˸
                SetTextFlickerCycle(0,2,0); //�ı���˸
                SetTextFlickerCycle(0,3,0); //�ı���˸
                SetTextFlickerCycle(0,4,50); //�ı���˸
                SetTextFlickerCycle(0,5,0); //�ı���˸
                SetTextFlickerCycle(0,6,0); //�ı���˸
                SetTextFlickerCycle(0,7,0); //�ı���˸
                SetTextFlickerCycle(0,8,0); //�ı���˸
                break;
            case 5:
                SetTextFlickerCycle(0,1,0); //�ı���˸
                SetTextFlickerCycle(0,2,0); //�ı���˸
                SetTextFlickerCycle(0,3,0); //�ı���˸
                SetTextFlickerCycle(0,4,0); //�ı���˸
                SetTextFlickerCycle(0,5,50); //�ı���˸
                SetTextFlickerCycle(0,6,0); //�ı���˸
                SetTextFlickerCycle(0,7,0); //�ı���˸
                SetTextFlickerCycle(0,8,0); //�ı���˸
                break;
            case 6:
                SetTextFlickerCycle(0,1,0); //�ı���˸
                SetTextFlickerCycle(0,2,0); //�ı���˸
                SetTextFlickerCycle(0,3,0); //�ı���˸
                SetTextFlickerCycle(0,4,0); //�ı���˸
                SetTextFlickerCycle(0,5,0); //�ı���˸
                SetTextFlickerCycle(0,6,50); //�ı���˸
                SetTextFlickerCycle(0,7,0); //�ı���˸
                SetTextFlickerCycle(0,8,0); //�ı���˸
                break;
            case 7:
                SetTextFlickerCycle(0,1,0); //�ı���˸
                SetTextFlickerCycle(0,2,0); //�ı���˸
                SetTextFlickerCycle(0,3,0); //�ı���˸
                SetTextFlickerCycle(0,4,0); //�ı���˸
                SetTextFlickerCycle(0,5,0); //�ı���˸
                SetTextFlickerCycle(0,6,0); //�ı���˸
                SetTextFlickerCycle(0,7,50); //�ı���˸
                SetTextFlickerCycle(0,8,0); //�ı���˸
                break;
            case 8:
                SetTextFlickerCycle(0,1,0); //�ı���˸
                SetTextFlickerCycle(0,2,0); //�ı���˸
                SetTextFlickerCycle(0,3,0); //�ı���˸
                SetTextFlickerCycle(0,4,0); //�ı���˸
                SetTextFlickerCycle(0,5,0); //�ı���˸
                SetTextFlickerCycle(0,6,0); //�ı���˸
                SetTextFlickerCycle(0,7,0); //�ı���˸
                SetTextFlickerCycle(0,8,50); //�ı���˸
                break;
        }
    }
    else if(screen_id == Menu_Torque_Interface)     //Ť�ؽ���ѡ���
    {
         switch(Cursor_id)
        {
            case 0:
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,0);
                break;
            case 1:
                SetTextFlickerCycle(0,1,50);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,0);
                break;
            case 2:
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,50);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,0);
                break;
            case 3:
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,50);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,0);
                break;
            case 4:
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,50);
                SetTextFlickerCycle(0,5,0);
                break;
            case 5:
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,50);
                break;
        }
    }
    else if(screen_id == Menu_Bolt_Interface)   //Ť�ؽ���ѡ���
    {
        switch(Cursor_id)
        {
            case 0:   //���ѡ��
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,0);
                break;
            case 1:   //ѡ���һ��
                SetTextFlickerCycle(0,1,50);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,0);
                break;
            case 2:   //ѡ��ڶ���
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,50);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,0);
                break;
            case 3:   //ѡ�������
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,50);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,0);
                break;
            case 4:   //ѡ����ĸ�
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,50);
                SetTextFlickerCycle(0,5,0);
                break;
            case 5:   //ѡ������
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,50);
                break;
        }
    }
    else if(screen_id == Menu_Angle_Interface)   //�ǶȽ���ѡ���
    {
        switch(Cursor_id)
        {
            case 0:   //���ѡ��
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 1:   //ѡ���һ��
                SetControlForeColor(0,1,50);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 2:   //ѡ��ڶ���
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,50);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 3:   //ѡ�������
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,50);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 4:   //ѡ����ĸ�
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,50);
                SetControlForeColor(0,5,0);
                break;
            case 5:   //ѡ������
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,50);
                break;
        }
    }
    else if(screen_id == Menu_Gear_Interface)   //��λ����ѡ���
    {
        switch(Cursor_id)
        {
            case 0:   //���ѡ��
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 1:   //ѡ���һ��
                SetControlForeColor(0,1,50);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 2:   //ѡ��ڶ���
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,50);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 3:   //ѡ�������
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,50);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 4:   //ѡ����ĸ�
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,50);
                SetControlForeColor(0,5,0);
                break;
            case 5:   //ѡ������
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,50);
                break;
        }
    }
    else if(screen_id == Menu_Sensor_Interface)   //����������ѡ���
    {
        switch(Cursor_id)
        {
            case 0:   //���ѡ��
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                SetControlForeColor(0,6,0);
                break;
            case 1:   //ѡ���һ��
                SetControlForeColor(0,1,50);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                SetControlForeColor(0,6,0);
                break;
            case 2:   //ѡ��ڶ���
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,50);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                SetControlForeColor(0,6,0);
                break;
            case 3:   //ѡ�������
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,50);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 4:   //ѡ����ĸ�
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,50);
                SetControlForeColor(0,5,0);
                SetControlForeColor(0,6,0);
                break;
            case 5:   //ѡ������
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,50);
                SetControlForeColor(0,6,0);
                break;
            case 6:   //ѡ�������
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                SetControlForeColor(0,6,50);
                break;
        }
    }
}


/*! 
*  \brief     ˢ����ʾʱ��
*  \param  ��
*/
void DisplayTime(void)
{ 
    static u8 Last_hour = 1;                 //��¼֮ǰʱ��
    static u8 Last_min = 1;
    static u8 Last_sec = 1;
    static u8 Last_w_date = 0;
    static u8 Last_w_month = 0;
    static u16 Last_w_year = 0;
    //��ʱ�䷢���ı��ʱ��ˢ����ʾʱ��
    if(calendar.sec != Last_sec)           //��
    {
        SetTextInt32(0,8,calendar.sec,0,2);
        Last_sec = calendar.sec;
    }
    else if(calendar.min != Last_min)       //��
    {
        SetTextInt32(0,7,calendar.min,0,2); 
        Last_min = calendar.min; 
    }
    else if(calendar.hour != Last_hour)//ʱ
    {
        SetTextInt32(0,6,calendar.hour,0,2);
        Last_hour = calendar.hour;
    }
    else if(calendar.w_date != Last_w_date)//��
    {
        SetTextInt32(0,5,calendar.w_date,0,2);        
        Last_w_date = calendar.w_date; 
    }
    else if(calendar.w_month != Last_w_month)//��
    {
        SetTextInt32(0,4,calendar.w_month,0,2);
        Last_w_month = calendar.w_month;  

    }
    else if(calendar.w_year != Last_w_year)//��
    {
        SetTextInt32(0,3,calendar.w_year,0,4); 
        Last_w_year = calendar.w_year;    
    }  
}


/*! 
*  \brief     ��Ʒ�����ʾ
*  \param  screen_id    ����id
*  \param  control_id   �ؼ�id
*  \param  type  ��Ʒ�ͺ�
*/
void HIM_Standard_display(uint16 screen_id,uint16 control_id,uint8 type)
{  
    
    SetTextValues(screen_id,control_id,(uchar*)SpannerType[type].type);    //��ȡ�ṹ�屣������ͺ�
    
}


/*! 
*  \brief     ������ʾ
*  \param  type   ������ʾ����������
*/
void HIM_Language_display(uint8 type)
{
    switch(type)
    {
        case 0:
            SetTextValues(0,1,"English");
            SetLanguage(0,0);   //�����л�Ӣ��
        break;
        case 1:
            SetTextValues(0,1,"����");
            SetLanguage(1,0);   //�����л�����
        break;
    }
}

/*! 
*  \brief     ͼ���׼��ʾ
*  \param  type   ������ʾ��ͼ���׼����
*/
void HIM_ChartStandard_display(uint8 type,uint8 Languege_type)
{
 
    switch(Languege_type)
    {
        case 0:
            switch(type)
            {
                case 0:
                    SetTextValues(0,2,"Imperial");
                break;
                case 1:
                    SetTextValues(0,2,"Metric");
                break;
            }
        break;
        case 1:
            switch(type)
            {
                case 0:
                    SetTextValues(0,2,"Ӣ��");
                break;
                case 1:
                    SetTextValues(0,2,"����");
                break;
            }
        break;
    }
    

}

/*! 
*  \brief     ��ʾ�������Ť��
*  \param  Unit    ��λ
*  \param  value  ����ϵ��
*  \param  Max_value     ����ֵ �������Ť��
*/  
float HIM_MaxTorque_display(uint8 Unit,float value)
{
    float Max_value;
    switch(Unit)
    {
        case  0:   //N.m
            Max_value = value*70;
            SetTextInt32(1,2,ROUND_TO_UINT32(Max_value),0,0);
        break;
        case  1:   //ft.lbs
            Max_value = (value*70)/1.3549;
            SetTextInt32(1,2,ROUND_TO_UINT32(Max_value),0,0);
        break;     
    }
    return Max_value;
}

/*! 
*  \brief     ��ȡ��λŤ��
*  \param  Unit    ��λ
*  \param  value  ����ϵ��
*  \param  Gearvalue  ����ϵ��
*  \param  Gear_value     ����ֵ ��λŤ��
*/  
u32 Get_GearTorqueValue(float value,u32 Gearvalue)
{
    u32 Gear_value;
    Gear_value = ROUND_TO_UINT32(value*Gearvalue/10000.0);      
    return Gear_value;
}
//u32 Get_GearTorqueValue(u8 Gearvalue)
//{
//    u32 Gear_value;
//    Gear_value = Gears_data[Gearvalue].parameter/10000;      
//    return Gear_value;
//}


void HIM_GearTorqueValue(uint16 screen_id,uint16 control_id,u32 value)
{     
    SetTextInt32(screen_id,control_id,value,0,0);  //��ʾ��λŤ��
}


/*! 
*  \brief     ���Ť�ص�λ��ʾ
*  \param  type   ������ʾ�ĵ�λ����
*/
void HIM_MaxTorqueUnit_display(uint8 type)
{
    switch(type)
    {
        case 0:
            SetTextValues(1,3,"N.m");
        break;
        case 1:
            SetTextValues(1,3,"Ft.lbs");
        break;
    }
}
/*! 
*  \brief     ��ʾ����Ŀ��Ť��
*  \param  screen_id    ����id
*  \param  control_id   �ؼ�id
*  \param  value  ����ϵ��
*/  
void HIM_TargetTorque_display(uint16 screen_id,uint16 control_id,u32 value)
{
    SetTextInt32(screen_id,control_id,value,0,0);
}
/*! 
*  \brief     Ŀ��Ť�ص�λ��ʾ
*  \param  screen_id    ����id
*  \param  control_id   �ؼ�id
*  \param  type   ������ʾ�ĵ�λ����
*/
void HIM_TargetTorqueuint_display(uint16 screen_id,uint16 control_id,uint8 type)
{
    switch(type)
    {
        case 0:
            SetTextValues(screen_id,control_id,"N.m");
        break;
        case 1:
            SetTextValues(screen_id,control_id,"Ft.lbs");
        break;
    }
}


/*! 
*  \brief     Ŀ��Ť�ص�λ��ʾ
*  \param  screen_id    ����id
*  \param  control_id   �ؼ�id
*  \param  type   ������ʾ�ĵ�λ����
*/
void HIM_TorqueUint_display(uint16 screen_id,uint16 control_id,uint8 type)
{
    switch(type)
    {
        case 0:
            SetTextValues(screen_id,control_id,"N.m");
        break;
        case 1:
            SetTextValues(screen_id,control_id,"Ft.lbs");
        break;
    }
}
/*! 
*  \brief     Ŀ��Ť�ص�λ��ʾ
*  \param  screen_id    ����id
*  \param  control_id   �ؼ�id
*  \param  type   ������ʾ�ĵ�λ����
*/
void HIM_ActualPressureuint_display(uint16 screen_id,uint16 control_id,uint8 type)
{
    switch(type)
    {
        case 0:
            SetTextValues(screen_id,control_id,"Bar");
        break;
        case 1:
            SetTextValues(screen_id,control_id,"Mpa");        
        break;
        case 2:
            SetTextValues(screen_id,control_id,"Psi");
        break;  
    }
}

/*! 
*  \brief    ��˨�ͺű�׼��ʾ
*  \param  Standard_Type   ͼ���׼ Imperial:Ӣ��  Metric������
*  \param  Bolt_Diameter   ��˨ֱ��
*/
void Bolt_Standard_display(uint8 Standard_Type,uint8 Bolt_Diameter)
{
    switch(Standard_Type)
    {
        case Imperial :              
            SetTextValues(2,1,BoltType.Imperial_Diameter[Bolt_Diameter]);
        break;
        case Metric :        
            SetTextValues(2,1,BoltType.Metric_Diameter[Bolt_Diameter]);
        break;
    }        
}

/*! 
*  \brief    ��˨�ȼ���ʾ
*  \param  Standard_Type   ͼ���׼ Imperial:Ӣ��  Metric������
*  \param  Bolt_Garde   ��˨ֱ��
*/
void Bolt_Grade_display(uint8 Standard_Type,uint8 Bolt_Garde)
{
    switch(Standard_Type)
    {
        case Imperial :              
            SetTextValues(2,2,BoltType.Imperial_Grade[Bolt_Garde]);
        break;
        case Metric :        
            SetTextValues(2,2,BoltType.Metric_Grade[Bolt_Garde]);
        break;
    }      
}

