#include "him_user_process.h"
#include "him_user_driver.h"
#include "Hydraulic_process.h"
#include "usart.h"
#include "rtc.h" 
#include "Data_process.h"

uint16 current_screen_id = 0;     //当前画面ID
uint16 previous_screen_id = 6;    //之前画面ID


/*! 
*  \brief     画面切换
*  \param  screen_id   要切换的画面iD
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
*  \brief     压力值显示
*  \param  screen_id   当前画面iD
*  \param  control_id   控件id
*  \param  value   压力值
*  \param  Unit   压力单位
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
*  \brief     设置指向光标
*  \param  screen_id   当前画面iD
*  \param  Cursor_id   光标编号
*/
void HIM_Cursor(uint16 screen_id,uint16 Cursor_id)
{
    if(screen_id == Menu_Main_Interface)    //菜单界面选择框
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
    else if(screen_id == Menu_Torque_Interface)     //扭矩界面选择框
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
    else if(screen_id == Menu_Bolt_Interface)   //螺栓界面选择框
    {
        switch(Cursor_id)
        {
            case 0:   //清空选择
                SetControlForeColor(2,1,Bule);
                SetControlForeColor(2,2,Bule);
                SetControlForeColor(2,4,Bule);
                SetControlForeColor(2,6,Bule);
                break;
            case 1:   //选择第1个
                SetControlForeColor(2,1,Green);
                SetControlForeColor(2,2,Bule);
                SetControlForeColor(2,4,Bule);
                SetControlForeColor(2,6,Bule);
                break;
            case 2:   //选择第2个
                SetControlForeColor(2,1,Bule);
                SetControlForeColor(2,2,Green);
                SetControlForeColor(2,4,Bule);
                SetControlForeColor(2,6,Bule);
                break;
            case 3:   //选择第3个
                SetControlForeColor(2,1,Bule);
                SetControlForeColor(2,2,Bule);
                SetControlForeColor(2,4,Green);
                SetControlForeColor(2,6,Bule);
                break;
            case 4:   //选择第4个
                SetControlForeColor(2,1,Bule);
                SetControlForeColor(2,2,Bule);
                SetControlForeColor(2,4,Bule);
                SetControlForeColor(2,6,Green);
                break;
        }
    }
    else if(screen_id == Menu_Angle_Interface)   //角度界面选择框
    {
        switch(Cursor_id)
        {
            case 0:   //清空选择
                SetControlForeColor(3,1,Bule);
                SetControlForeColor(3,2,Bule);
                SetControlForeColor(3,3,Bule);
                SetControlForeColor(3,4,Bule);
                SetControlForeColor(3,6,Bule);
                break;
            case 1:   //选择第一个
                SetControlForeColor(3,1,Green);
                SetControlForeColor(3,2,Bule);
                SetControlForeColor(3,3,Bule);
                SetControlForeColor(3,4,Bule);
                SetControlForeColor(3,6,Bule);
                break;
            case 2:   //选择第二个
                SetControlForeColor(3,1,Bule);
                SetControlForeColor(3,2,Green);
                SetControlForeColor(3,3,Bule);
                SetControlForeColor(3,4,Bule);
                SetControlForeColor(3,6,Bule);
                break;
            case 3:   //选择第三个
                SetControlForeColor(3,1,Bule);
                SetControlForeColor(3,2,Bule);
                SetControlForeColor(3,3,Green);
                SetControlForeColor(3,4,Bule);
                SetControlForeColor(3,6,Bule);
                break;
            case 4:   //选择第四个
                SetControlForeColor(3,1,Bule);
                SetControlForeColor(3,2,Bule);
                SetControlForeColor(3,3,Bule);
                SetControlForeColor(3,4,Green);
                SetControlForeColor(3,6,Bule);
                break;
            case 5:   //选择第五个
                SetControlForeColor(3,1,Bule);
                SetControlForeColor(3,2,Bule);
                SetControlForeColor(3,3,Bule);
                SetControlForeColor(3,4,Bule);
                SetControlForeColor(3,6,Green);
                break;
        }
    }
    else if(screen_id == Menu_Gear_Interface)   //档位界面选择框
    {
        switch(Cursor_id)
        {
            case 0:   //清空选择
                SetControlForeColor(4,1,Bule);
                SetControlForeColor(4,2,Bule);
                SetControlForeColor(4,4,Bule);
                SetControlForeColor(4,6,Bule);
                break;
            case 1:   //选择第一个
                SetControlForeColor(4,1,Green);
                SetControlForeColor(4,2,Bule);
                SetControlForeColor(4,4,Bule);
                SetControlForeColor(4,6,Bule);
                break;
            case 2:   //选择第二个
                SetControlForeColor(4,1,Bule);
                SetControlForeColor(4,2,Green);
                SetControlForeColor(4,4,Bule);
                SetControlForeColor(4,6,Bule);
                break;
            case 3:   //选择第个
                SetControlForeColor(4,1,Bule);
                SetControlForeColor(4,2,Bule);
                SetControlForeColor(4,4,Green);
                SetControlForeColor(4,6,Bule);
                break;
            case 4:   //选择第四个
                SetControlForeColor(4,1,Bule);
                SetControlForeColor(4,2,Bule);
                SetControlForeColor(4,4,Bule);
                SetControlForeColor(4,6,Green);
                break;
        }
    }
    else if(screen_id == Menu_Sensor_Interface)   //传感器界面选择框
    {
        switch(Cursor_id)
        {
            case 0:   //清空选择
                SetControlForeColor(5,1,Bule);
                SetControlForeColor(5,2,Bule);
                SetControlForeColor(5,3,Bule);
                SetControlForeColor(5,4,Bule);
                SetControlForeColor(5,5,Bule);
                SetControlForeColor(5,7,Bule);
                break;
            case 1:   //选择第一个
                SetControlForeColor(5,1,Green);
                SetControlForeColor(5,2,Bule);
                SetControlForeColor(5,3,Bule);
                SetControlForeColor(5,4,Bule);
                SetControlForeColor(5,5,Bule);
                SetControlForeColor(5,7,Bule);
                break;
            case 2:   //选择第二个
                SetControlForeColor(5,1,Bule);
                SetControlForeColor(5,2,Green);
                SetControlForeColor(5,3,Bule);
                SetControlForeColor(5,4,Bule);
                SetControlForeColor(5,5,Bule);
                SetControlForeColor(5,7,Bule);
                break;
            case 3:   //选择第三个
                SetControlForeColor(5,1,Bule);
                SetControlForeColor(5,2,Bule);
                SetControlForeColor(5,3,Green);
                SetControlForeColor(5,4,Bule);
                SetControlForeColor(5,7,Bule);
                break;
            case 4:   //选择第四个
                SetControlForeColor(5,1,Bule);
                SetControlForeColor(5,2,Bule);
                SetControlForeColor(5,3,Bule);
                SetControlForeColor(5,4,Green);
                SetControlForeColor(5,5,Bule);
                SetControlForeColor(5,7,Bule);
                break;
            case 5:   //选择第五个
                SetControlForeColor(5,1,Bule);
                SetControlForeColor(5,2,Bule);
                SetControlForeColor(5,3,Bule);
                SetControlForeColor(5,4,Bule);
                SetControlForeColor(5,5,Green);
                SetControlForeColor(5,7,Bule);
                break;
            case 6:   //选择第六个
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
*  \brief     文本变量闪烁
*  \param  screen_id   画面ID
*  \param  Cursor_id   选项ID
*/
void Variable_Blink(uint16 screen_id,uint16 Cursor_id)
{
     if(screen_id == Menu_Main_Interface)    //菜单界面选择框
    {
        switch(Cursor_id)
        {
            case 0:
                SetTextFlickerCycle(0,1,0); //文本闪烁
                SetTextFlickerCycle(0,2,0); //文本闪烁
                SetTextFlickerCycle(0,3,0); //文本闪烁
                SetTextFlickerCycle(0,4,0); //文本闪烁
                SetTextFlickerCycle(0,5,0); //文本闪烁
                SetTextFlickerCycle(0,6,0); //文本闪烁
                SetTextFlickerCycle(0,7,0); //文本闪烁
                SetTextFlickerCycle(0,8,0); //文本闪烁
                break;
            case 1:
                SetTextFlickerCycle(0,1,50); //文本闪烁
                SetTextFlickerCycle(0,2,0); //文本闪烁
                SetTextFlickerCycle(0,3,0); //文本闪烁
                SetTextFlickerCycle(0,4,0); //文本闪烁
                SetTextFlickerCycle(0,5,0); //文本闪烁
                SetTextFlickerCycle(0,6,0); //文本闪烁
                SetTextFlickerCycle(0,7,0); //文本闪烁
                SetTextFlickerCycle(0,8,0); //文本闪烁
                break;
            case 2:
                SetTextFlickerCycle(0,1,0); //文本闪烁
                SetTextFlickerCycle(0,2,50); //文本闪烁
                SetTextFlickerCycle(0,3,0); //文本闪烁
                SetTextFlickerCycle(0,4,0); //文本闪烁
                SetTextFlickerCycle(0,5,0); //文本闪烁
                SetTextFlickerCycle(0,6,0); //文本闪烁
                SetTextFlickerCycle(0,7,0); //文本闪烁
                SetTextFlickerCycle(0,8,0); //文本闪烁
                break;
            case 3:
                SetTextFlickerCycle(0,1,0); //文本闪烁
                SetTextFlickerCycle(0,2,0); //文本闪烁
                SetTextFlickerCycle(0,3,50); //文本闪烁
                SetTextFlickerCycle(0,4,0); //文本闪烁
                SetTextFlickerCycle(0,5,0); //文本闪烁
                SetTextFlickerCycle(0,6,0); //文本闪烁
                SetTextFlickerCycle(0,7,0); //文本闪烁
                SetTextFlickerCycle(0,8,0); //文本闪烁
                break;
            case 4:
                SetTextFlickerCycle(0,1,0); //文本闪烁
                SetTextFlickerCycle(0,2,0); //文本闪烁
                SetTextFlickerCycle(0,3,0); //文本闪烁
                SetTextFlickerCycle(0,4,50); //文本闪烁
                SetTextFlickerCycle(0,5,0); //文本闪烁
                SetTextFlickerCycle(0,6,0); //文本闪烁
                SetTextFlickerCycle(0,7,0); //文本闪烁
                SetTextFlickerCycle(0,8,0); //文本闪烁
                break;
            case 5:
                SetTextFlickerCycle(0,1,0); //文本闪烁
                SetTextFlickerCycle(0,2,0); //文本闪烁
                SetTextFlickerCycle(0,3,0); //文本闪烁
                SetTextFlickerCycle(0,4,0); //文本闪烁
                SetTextFlickerCycle(0,5,50); //文本闪烁
                SetTextFlickerCycle(0,6,0); //文本闪烁
                SetTextFlickerCycle(0,7,0); //文本闪烁
                SetTextFlickerCycle(0,8,0); //文本闪烁
                break;
            case 6:
                SetTextFlickerCycle(0,1,0); //文本闪烁
                SetTextFlickerCycle(0,2,0); //文本闪烁
                SetTextFlickerCycle(0,3,0); //文本闪烁
                SetTextFlickerCycle(0,4,0); //文本闪烁
                SetTextFlickerCycle(0,5,0); //文本闪烁
                SetTextFlickerCycle(0,6,50); //文本闪烁
                SetTextFlickerCycle(0,7,0); //文本闪烁
                SetTextFlickerCycle(0,8,0); //文本闪烁
                break;
            case 7:
                SetTextFlickerCycle(0,1,0); //文本闪烁
                SetTextFlickerCycle(0,2,0); //文本闪烁
                SetTextFlickerCycle(0,3,0); //文本闪烁
                SetTextFlickerCycle(0,4,0); //文本闪烁
                SetTextFlickerCycle(0,5,0); //文本闪烁
                SetTextFlickerCycle(0,6,0); //文本闪烁
                SetTextFlickerCycle(0,7,50); //文本闪烁
                SetTextFlickerCycle(0,8,0); //文本闪烁
                break;
            case 8:
                SetTextFlickerCycle(0,1,0); //文本闪烁
                SetTextFlickerCycle(0,2,0); //文本闪烁
                SetTextFlickerCycle(0,3,0); //文本闪烁
                SetTextFlickerCycle(0,4,0); //文本闪烁
                SetTextFlickerCycle(0,5,0); //文本闪烁
                SetTextFlickerCycle(0,6,0); //文本闪烁
                SetTextFlickerCycle(0,7,0); //文本闪烁
                SetTextFlickerCycle(0,8,50); //文本闪烁
                break;
        }
    }
    else if(screen_id == Menu_Torque_Interface)     //扭矩界面选择框
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
    else if(screen_id == Menu_Bolt_Interface)   //扭矩界面选择框
    {
        switch(Cursor_id)
        {
            case 0:   //清空选择
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,0);
                break;
            case 1:   //选择第一个
                SetTextFlickerCycle(0,1,50);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,0);
                break;
            case 2:   //选择第二个
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,50);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,0);
                break;
            case 3:   //选择第三个
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,50);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,0);
                break;
            case 4:   //选择第四个
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,50);
                SetTextFlickerCycle(0,5,0);
                break;
            case 5:   //选择第五个
                SetTextFlickerCycle(0,1,0);
                SetTextFlickerCycle(0,2,0);
                SetTextFlickerCycle(0,3,0);
                SetTextFlickerCycle(0,4,0);
                SetTextFlickerCycle(0,5,50);
                break;
        }
    }
    else if(screen_id == Menu_Angle_Interface)   //角度界面选择框
    {
        switch(Cursor_id)
        {
            case 0:   //清空选择
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 1:   //选择第一个
                SetControlForeColor(0,1,50);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 2:   //选择第二个
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,50);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 3:   //选择第三个
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,50);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 4:   //选择第四个
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,50);
                SetControlForeColor(0,5,0);
                break;
            case 5:   //选择第五个
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,50);
                break;
        }
    }
    else if(screen_id == Menu_Gear_Interface)   //档位界面选择框
    {
        switch(Cursor_id)
        {
            case 0:   //清空选择
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 1:   //选择第一个
                SetControlForeColor(0,1,50);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 2:   //选择第二个
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,50);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 3:   //选择第三个
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,50);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 4:   //选择第四个
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,50);
                SetControlForeColor(0,5,0);
                break;
            case 5:   //选择第五个
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,50);
                break;
        }
    }
    else if(screen_id == Menu_Sensor_Interface)   //传感器界面选择框
    {
        switch(Cursor_id)
        {
            case 0:   //清空选择
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                SetControlForeColor(0,6,0);
                break;
            case 1:   //选择第一个
                SetControlForeColor(0,1,50);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                SetControlForeColor(0,6,0);
                break;
            case 2:   //选择第二个
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,50);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                SetControlForeColor(0,6,0);
                break;
            case 3:   //选择第三个
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,50);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,0);
                break;
            case 4:   //选择第四个
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,50);
                SetControlForeColor(0,5,0);
                SetControlForeColor(0,6,0);
                break;
            case 5:   //选择第五个
                SetControlForeColor(0,1,0);
                SetControlForeColor(0,2,0);
                SetControlForeColor(0,3,0);
                SetControlForeColor(0,4,0);
                SetControlForeColor(0,5,50);
                SetControlForeColor(0,6,0);
                break;
            case 6:   //选择第六个
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
*  \brief     刷新显示时间
*  \param  无
*/
void DisplayTime(void)
{ 
    static u8 Last_hour = 1;                 //记录之前时间
    static u8 Last_min = 1;
    static u8 Last_sec = 1;
    static u8 Last_w_date = 0;
    static u8 Last_w_month = 0;
    static u16 Last_w_year = 0;
    //当时间发生改变的时候刷新显示时间
    if(calendar.sec != Last_sec)           //秒
    {
        SetTextInt32(0,8,calendar.sec,0,2);
        Last_sec = calendar.sec;
    }
    else if(calendar.min != Last_min)       //分
    {
        SetTextInt32(0,7,calendar.min,0,2); 
        Last_min = calendar.min; 
    }
    else if(calendar.hour != Last_hour)//时
    {
        SetTextInt32(0,6,calendar.hour,0,2);
        Last_hour = calendar.hour;
    }
    else if(calendar.w_date != Last_w_date)//日
    {
        SetTextInt32(0,5,calendar.w_date,0,2);        
        Last_w_date = calendar.w_date; 
    }
    else if(calendar.w_month != Last_w_month)//月
    {
        SetTextInt32(0,4,calendar.w_month,0,2);
        Last_w_month = calendar.w_month;  

    }
    else if(calendar.w_year != Last_w_year)//年
    {
        SetTextInt32(0,3,calendar.w_year,0,4); 
        Last_w_year = calendar.w_year;    
    }  
}


/*! 
*  \brief     产品规格显示
*  \param  screen_id    画面id
*  \param  control_id   控件id
*  \param  type  产品型号
*/
void HIM_Standard_display(uint16 screen_id,uint16 control_id,uint8 type)
{  
    
    SetTextValues(screen_id,control_id,(uchar*)SpannerType[type].type);    //获取结构体保存扳手型号
    
}


/*! 
*  \brief     语言显示
*  \param  type   输入显示的语言类型
*/
void HIM_Language_display(uint8 type)
{
    switch(type)
    {
        case 0:
            SetTextValues(0,1,"English");
            SetLanguage(0,0);   //语言切换英文
        break;
        case 1:
            SetTextValues(0,1,"中文");
            SetLanguage(1,0);   //语言切换中文
        break;
    }
}

/*! 
*  \brief     图表标准显示
*  \param  type   输入显示的图表标准类型
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
                    SetTextValues(0,2,"英制");
                break;
                case 1:
                    SetTextValues(0,2,"公制");
                break;
            }
        break;
    }
    

}

/*! 
*  \brief     显示扳手最大扭矩
*  \param  Unit    单位
*  \param  value  扳手系数
*  \param  Max_value     返回值 扳手最大扭矩
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
*  \brief     获取档位扭矩
*  \param  Unit    单位
*  \param  value  扳手系数
*  \param  Gearvalue  扳手系数
*  \param  Gear_value     返回值 档位扭矩
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
    SetTextInt32(screen_id,control_id,value,0,0);  //显示档位扭矩
}


/*! 
*  \brief     最大扭矩单位显示
*  \param  type   输入显示的单位类型
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
*  \brief     显示扳手目标扭矩
*  \param  screen_id    画面id
*  \param  control_id   控件id
*  \param  value  扳手系数
*/  
void HIM_TargetTorque_display(uint16 screen_id,uint16 control_id,u32 value)
{
    SetTextInt32(screen_id,control_id,value,0,0);
}
/*! 
*  \brief     目标扭矩单位显示
*  \param  screen_id    画面id
*  \param  control_id   控件id
*  \param  type   输入显示的单位类型
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
*  \brief     目标扭矩单位显示
*  \param  screen_id    画面id
*  \param  control_id   控件id
*  \param  type   输入显示的单位类型
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
*  \brief     目标扭矩单位显示
*  \param  screen_id    画面id
*  \param  control_id   控件id
*  \param  type   输入显示的单位类型
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
*  \brief    螺栓型号标准显示
*  \param  Standard_Type   图表标准 Imperial:英制  Metric：公制
*  \param  Bolt_Diameter   螺栓直径
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
*  \brief    螺栓等级显示
*  \param  Standard_Type   图表标准 Imperial:英制  Metric：公制
*  \param  Bolt_Garde   螺栓直径
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

