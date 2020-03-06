/*
 * newCanLib.c
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: User
 */

#include "sys_vim.h"
#include "newCanLib.h"

#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
#else
    static const uint32 s_canByteOrder[8U] = {3U, 2U, 1U, 0U, 7U, 6U, 5U, 4U};
#endif


static void messageBoxInitialize(canBASE_t *node, uint32 messageBox, uint32_t ide, uint32_t mask, uint32_t filter, uint32_t nodeType )
{
    if ( messageBox % 2 == 0)
    {
        if(nodeType == SENDING_NODE)
        {
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
    }
    else
    {
        if(nodeType == SENDING_NODE)
        {
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
    }
}
static void messageBoxInitReg1(canBASE_t *node)
{
    messageBoxInitialize(node, canMESSAGE_BOX1 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE); // Semicron NMT_Command
    messageBoxInitialize(node, canMESSAGE_BOX2 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE); // Semicron Sync
    messageBoxInitialize(node, canMESSAGE_BOX3 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE); // Semicron NMT_NodeGuarding
    messageBoxInitialize(node, canMESSAGE_BOX4 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE); // SemicronRx Handler
    messageBoxInitialize(node, canMESSAGE_BOX5 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE); // Bms Heart Beat
    messageBoxInitialize(node, canMESSAGE_BOX6 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE); // Bms Rx Handler
    messageBoxInitialize(node, canMESSAGE_BOX7 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE); // Semicron NMT_Command
    messageBoxInitialize(node, canMESSAGE_BOX8 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE); // Semicron NMT_Command

}

static void messageBoxInitReg2(canBASE_t *node)
{
    messageBoxInitialize(node, canMESSAGE_BOX1 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE);
    messageBoxInitialize(node, canMESSAGE_BOX2 ,(uint32_t) NULL,(uint32_t) NULL,(uint32_t) NULL, SENDING_NODE);

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
                    ptr->data[i++];
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
    node->INTMUXx[0U] = (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U
                      | (uint32)0x00000000U;

    /** - Setup auto bus on timer period */
    node->ABOTR = (uint32)0U;

    if(node == canREG1)
        messageBoxInitReg1(node);
    else
        messageBoxInitReg2(node);
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
    while ((node->IF2STAT & 0x80U) ==0x80U)
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
