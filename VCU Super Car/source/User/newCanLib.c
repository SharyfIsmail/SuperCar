/*
 * newCanLib.c
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: User
 */

#include "sys_vim.h"
#include "newCanLib.h"
#include "stdarg.h"

static void initializeSendingMessageBox(canBASE_t *node, uint32 messageBox, can_identifier_t canId);
static void initializeReceivingMessageBoxStd(canBASE_t *node, uint32 messageBox, uint32_t mask, uint32_t filter);
static void initializeReceivingMessageBoxExt(canBASE_t *node, uint32 messageBox, uint32_t mask, uint32_t filter);
static void setMessageBoxLowInterrupt(canBASE_t *node ,uint8_t number, ...);


#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
#else
    static const uint32 s_canByteOrder[8U] = {3U, 2U, 1U, 0U, 7U, 6U, 5U, 4U};
#endif

static void messageBoxInitialize(canBASE_t *node, uint32 messageBox, uint32_t ide, uint32_t mask, uint32_t filter, nodType_t nodeType )
{
    if ( messageBox % 2 == 0)
    {
        if(nodeType == SENDING_NODE)
        {
           // node->IF2MSK  = 0x00;
          //  node->IF2ARB  = 0x00;
            node->IF2MCTL = 0x00001000U | (uint32)nodeType | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
            node->IF2CMD  = (uint8) 0xF8U;
            node->IF2NO   = messageBox;

        }

        else
        {
            if(ide == CAN_Id_Standard)
            {
                node->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)mask & (uint32)0x000007FFU) << (uint32)18U);
                node->IF2ARB  = (uint32)0x80000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)((uint32)((uint32)filter & (uint32)0x000007FFU) << (uint32)18U);
                node->IF2MCTL = 0x00001000U | (uint32)nodeType | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                node->IF2CMD  = (uint8) 0xF8U;
                node->IF2NO   = messageBox;
            }
            else
            {
                node->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)mask & (uint32)0x1FFFFFFFU));
                node->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x00000000U | (uint32)((uint32)((uint32)filter & (uint32)0x1FFFFFFFU));
                node->IF2MCTL = 0x00001000U | (uint32)nodeType | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                node->IF2CMD  = (uint8) 0xF8U;
                node->IF2NO   = messageBox;
            }
        }
        while ((node->IF2STAT & 0x80U) == 0x80U)
        {
        } /* Wait */
    }
    else
    {
        if(nodeType == SENDING_NODE)
        {
          //  node->IF1MSK  = 0x00;
           // node->IF1ARB  = 0x00;
            node->IF1MCTL = 0x00001000U | (uint32)nodeType | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
            node->IF1CMD  = (uint8) 0xF8U;
            node->IF1NO   = messageBox;
        }

        else
        {
            if(ide == CAN_Id_Standard)
            {
                node->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)mask & (uint32)0x000007FFU) << (uint32)18U);
                node->IF1ARB  = (uint32)0x80000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)((uint32)((uint32)filter & (uint32)0x000007FFU) << (uint32)18U);
                node->IF1MCTL = 0x00001000U | (uint32)nodeType | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                node->IF1CMD  = (uint8) 0xF8U;
                node->IF1NO   = messageBox;
            }
            else
            {
                node->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)mask & (uint32)0x1FFFFFFFU));
                node->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x00000000U | (uint32)((uint32)((uint32)filter & (uint32)0x1FFFFFFFU));
                node->IF1MCTL = 0x00001000U | (uint32)nodeType | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                node->IF1CMD  = (uint8) 0xF8U;
                node->IF1NO   = messageBox;
            }
        }
        while ((node->IF1STAT & 0x80U) ==0x80U)
        {
        } /* Wait */
    }
}
static void messageBoxInitReg1()
{
    /*Sending node*/
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX1, CAN_Id_Standard); // Semicron NMT_Command
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX2, CAN_Id_Standard); // Semicron Sync
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX3, CAN_Id_Standard); // Semicron NMT_NodeGuarding
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX4, CAN_Id_Standard); // SemicronRx Handler
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX5, CAN_Id_Standard); // All current error
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX6, CAN_Id_Standard); // Error from external memory
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX7, CAN_Id_Standard); // Real Time in y:m:d:h:m:s;
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX8, CAN_Id_Extended); // vSelectorRxHandler
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX9, CAN_Id_Extended); // VCU BMS
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX10, CAN_Id_Extended); // VCU DCDC
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX11, CAN_Id_Extended); // VCU ALL 1
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX12, CAN_Id_Extended); // VCU ALL 2
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX13, CAN_Id_Extended); // INV VCU 02
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX14, CAN_Id_Extended); // INV VCU 01
    initializeSendingMessageBox(canREG1, canMESSAGE_BOX23, CAN_Id_Extended); // INV VCU 03


//    initializeSendingMessageBox(canREG1, canMESSAGE_BOX1, CAN_Id_Extended);// Bms Heart Beat
//    initializeSendingMessageBox(canREG1, canMESSAGE_BOX2, CAN_Id_Extended);// Bms Rx Handler

    /*Receiving node */
    /*High level */
    initializeReceivingMessageBoxStd(canREG1, canMESSAGE_BOX15, (uint32_t) 0xFF, (uint32_t) 0xFA); // semicron
    initializeReceivingMessageBoxStd(canREG1, canMESSAGE_BOX16, (uint32_t) 0x7FF, (uint32_t) 0x1BA); // semicron
    initializeReceivingMessageBoxExt(canREG1, canMESSAGE_BOX17, (uint32_t) 0xFF0FFFF,(uint32_t) 0xCB0D0F3); // bms
    /*Low level */
    initializeReceivingMessageBoxExt(canREG1, canMESSAGE_BOX18, (uint32_t) 0x1FFFFFFF,(uint32_t) 0xC91D028); // selector
    initializeReceivingMessageBoxExt(canREG1, canMESSAGE_BOX19, (uint32_t) 0x1FFFFFFF,(uint32_t) 0x800D025); // pedal
    initializeReceivingMessageBoxStd(canREG1, canMESSAGE_BOX20, (uint32_t) 0x7FF,(uint32_t) 0x6DE); // external memory
    initializeReceivingMessageBoxStd(canREG1, canMESSAGE_BOX21, (uint32_t) 0x7FF,(uint32_t) 0x6DB); // time setter
    initializeReceivingMessageBoxExt(canREG1, canMESSAGE_BOX22, (uint32_t) 0x1FFFFFFF,(uint32_t) 0x18C1D08F); // dcdc


   // messageBoxInitialize(canREG1, canMESSAGE_BOX15, CAN_Id_Standard, (uint32_t) 0x7CF, (uint32_t) 0x10F, RECEIVEING_NODE); // Selector , joystick, brake: 11F
  //  messageBoxInitialize(canREG1, canMESSAGE_BOX16, CAN_Id_Standard, (uint32_t) 0x7ff, (uint32_t) 0x16, RECEIVEING_NODE);   // Command to external memory
  //  initializeReceivingMessageBoxStd(canREG1, canMESSAGE_BOX15, (uint32_t) 0x7CF, (uint32_t) 0x10F);
  //  initializeReceivingMessageBoxStd(canREG1, canMESSAGE_BOX16, (uint32_t) 0x7ff, (uint32_t) 0x16);

}

static void messageBoxInitReg2()
{
    /*Sending node*/
  //  messageBoxInitialize(canREG2, canMESSAGE_BOX1 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE); // Semicron NMT_Command
  //  messageBoxInitialize(canREG2, canMESSAGE_BOX2 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE); // Semicron Sync
  //  messageBoxInitialize(canREG2, canMESSAGE_BOX3 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE); // Semicron NMT_NodeGuarding
   // messageBoxInitialize(canREG2, canMESSAGE_BOX4 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE); // SemicronRx Handler

    /*Receiving node */
    /*High level */
  //  messageBoxInitialize(canREG2, canMESSAGE_BOX9, CAN_Id_Standard, (uint32_t) 0xFF , (uint32_t) 0xFA , RECEIVEING_NODE); //SEMICRON : 1FA, 2FA, 3FA, 4FA
   // messageBoxInitialize(canREG2, canMESSAGE_BOX10, CAN_Id_Standard, (uint32_t) 0x7FF, (uint32_t) 0x1BA, RECEIVEING_NODE); //SEMICRON : 1BA

}
uint32 newCanTransmit(canBASE_t *node, uint32 messageBox, canMessage_t* ptr)
{
    uint8_t i;
    uint32 success  = 0U;
    uint32 regIndex = (messageBox - 1U) >> 5U;
    uint32 bitIndex = 1U << ((messageBox - 1U) & 0x1FU);

    if ((node->TXRQx[regIndex] & bitIndex) != 0U)
    {
        success = 0U;
    }

    else
    {
        while ((node->IF1STAT & 0x80U) ==0x80U);

        node->IF1CMD = 0x87U;

        for (i = 0U; i < ptr->dlc; i++)
        {
            #if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
                    node->IF1DATx[i] = ptr->data[i];
                    ptr->data++;
            #else
                    node->IF1DATx[s_canByteOrder[i]] = ptr->data[i];
                 //   ptr->data[i++];
            #endif
        }
        node->IF1CMD |=0xB0U;
        if(ptr->ide == CAN_Id_Extended)
        {
            node->IF1ARB = 0xE0000000U;
            node->IF1ARB |= (ptr->id & 0x1FFFFFFFU);
            node->IF1MCTL = 0x00001000U | (uint32_t)ptr->dlc;
        }
        else
        {
            node->IF1ARB = 0xA0000000U;
            node->IF1ARB |= (ptr->id << 18) & 0x1FFC0000U;
            node->IF1MCTL = 0x00001000U | (uint32_t)ptr->dlc;
        }

        node->IF1NO = (uint8) messageBox;

        success = 1U;
    }
    return success;
}

void boardCanInit(canBASE_t *node)
{
    node->CTL = (uint32)0x00000000U
              | (uint32)0x00000000U
              | (uint32)((uint32)0x00000005U  << 10U)
              | (uint32)0x00020043U;
    //Clear pending error flags
    node->ES |= 0xFFFFFFFFU;
    /** - Assign interrupt level for messages */
    if(node == canREG1)
    {
        setMessageBoxLowInterrupt(node, 5, canMESSAGE_BOX18, canMESSAGE_BOX19, canMESSAGE_BOX20, canMESSAGE_BOX21, canMESSAGE_BOX22);
    }
    else
    {
        //setMessageBoxLowInterrupt(node, canMESSAGE_BOX15, canMESSAGE_BOX16);
    }
    /** - Setup auto bus on timer period */
    node->ABOTR = (uint32) 1U;

    if(node == canREG1)
        messageBoxInitReg1();
    else
        messageBoxInitReg2();
    /** - Setup IF1 for data transmission
     *     - Wait until IF1 is ready for use
     *     - Set IF1 control byte
     */
    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
    while ((node->IF1STAT & 0x80U) ==0x80U)
    {
    } /* Wait */
    node->IF1CMD  = 0x87U;

    /** - Setup IF2 for reading data
     *     - Wait until IF1 is ready for use
     *     - Set IF1 control byte
     */
    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
    while ((node->IF2STAT & 0x80U) == 0x80U)
    {
    } /* Wait */
    node->IF2CMD = 0x17U;

    /** - Setup bit timing
     *     - Setup baud rate prescaler extension
     *     - Setup TSeg2
     *     - Setup TSeg1
     *     - Setup sample jump width
     *     - Setup baud rate prescaler
     */
    node->BTR = (uint32)((uint32)0U << 16U) |
               (uint32)((uint32)(6U - 1U) << 12U) |
               (uint32)((uint32)((3U + 6U) - 1U) << 8U) |
               (uint32)((uint32)(4U - 1U) << 6U) |
               (uint32)19U;



    /** - CAN1/CAN2 Port output values */
    node->TIOC =  (uint32)((uint32)1U  << 18U )
                | (uint32)((uint32)0U  << 17U )
                | (uint32)((uint32)0U  << 16U )
                | (uint32)((uint32)1U  << 3U )
                | (uint32)((uint32)1U  << 2U )
                | (uint32)((uint32)1U  << 1U );

    node->RIOC =  (uint32)((uint32)1U  << 18U )
                | (uint32)((uint32)0U  << 17U )
                | (uint32)((uint32)0U  << 16U )
                | (uint32)((uint32)1U  << 3U )
                | (uint32)((uint32)0U  << 2U )
                | (uint32)((uint32)0U  <<1U );

    /** - Leave configuration and initialization mode  */
    node->CTL &= ~(uint32)(0x00000041U);
}

static void initializeSendingMessageBox(canBASE_t *node, uint32 messageBox, can_identifier_t canId)
{
    do
    {
        if(canId == CAN_Id_Extended)
        {
            if((node->IF1STAT & 0x80U) != 0x80U)
            {
                node->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                node->IF1MCTL = 0x00001000U | (uint32)0x00 | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                node->IF1CMD  = (uint8) 0xF8U;
                node->IF1NO   = messageBox;
                break;
            }
            if((node->IF2STAT & 0x80U) != 0x80U)
            {
                node->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                node->IF2MCTL = 0x00001000U | (uint32)0x00 | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                node->IF2CMD  = (uint8) 0xF8U;
                node->IF2NO   = messageBox;
                break;
            }
        }
        else
        {
            if((node->IF1STAT & 0x80U) != 0x80U)
            {
                node->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x000007FFU) << (uint32)18U);
                node->IF1MCTL = 0x00001000U | (uint32)0x00 | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                node->IF1CMD  = (uint8) 0xF8U;
                node->IF1NO   = messageBox;
                break;
            }
            if((node->IF2STAT & 0x80U) != 0x80U)
            {
                node->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x000007FFU) << (uint32)18U);
                node->IF2MCTL = 0x00001000U | (uint32)0x00 | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                node->IF2CMD  = (uint8) 0xF8U;
                node->IF2NO   = messageBox;
                break;
            }
        }
    }while(1);
}

static void initializeReceivingMessageBoxStd(canBASE_t *node, uint32 messageBox, uint32_t mask, uint32_t filter)
{
    do
    {
        if((node->IF1STAT & 0x80U) != 0x80U)
        {
            node->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)mask & (uint32)0x000007FFU) << (uint32)18U);
            node->IF1ARB  = (uint32)0x80000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)((uint32)((uint32)filter & (uint32)0x000007FFU) << (uint32)18U);
            node->IF1MCTL = 0x00001000U | (uint32)0x400 | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
            node->IF1CMD  = (uint8) 0xF8U;
            node->IF1NO   = messageBox;
            break;
        }
        if((node->IF2STAT & 0x80U) != 0x80U)
        {
            node->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)mask & (uint32)0x000007FFU) << (uint32)18U);
            node->IF2ARB  = (uint32)0x80000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)((uint32)((uint32)filter & (uint32)0x000007FFU) << (uint32)18U);
            node->IF2MCTL = 0x00001000U | (uint32)0x400 | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
            node->IF2CMD  = (uint8) 0xF8U;
            node->IF2NO   = messageBox;
            break;
        }
    }while(1);
}

static void initializeReceivingMessageBoxExt(canBASE_t *node, uint32 messageBox, uint32_t mask, uint32_t filter)
{
    do
    {
        if((node->IF1STAT & 0x80U) != 0x80U)
        {
            node->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)mask & (uint32)0x1FFFFFFFU));
            node->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x00000000U | (uint32)((uint32)((uint32)filter & (uint32)0x1FFFFFFFU));
            node->IF1MCTL = 0x00001000U | (uint32)0x400  | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
            node->IF1CMD  = (uint8) 0xF8U;
            node->IF1NO   = messageBox;
        }
        if((node->IF2STAT & 0x80U) != 0x80U)
        {

            node->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)mask & (uint32)0x1FFFFFFFU));
            node->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x00000000U | (uint32)((uint32)((uint32)filter & (uint32)0x1FFFFFFFU));
            node->IF2MCTL = 0x00001000U | (uint32)0x400  | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
            node->IF2CMD  = (uint8) 0xF8U;
            node->IF2NO   = messageBox;
            break;
        }
    }while(1);
}

static void setMessageBoxLowInterrupt(canBASE_t *node ,uint8_t number, ...)
{
    uint32_t data;
    va_list args;
    va_start(args, number);
    for(; number; number--)
    {
        data = va_arg(args, uint32_t);
        node->INTMUXx[0U] |= (uint32_t)(1 << (data));
    }
    va_end(args);
}
