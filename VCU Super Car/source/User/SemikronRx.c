/*
 * SemikronRx.c
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "newCanLib.h"
#include "SemikronRx.h"
#include "message_buffer.h"
#include "acceleratorBrakeJoystick.h"
#include "canMessageLostCheck.h"
#include "vcuStateManagement.h"

void vSemicronRxHandler (void *pvParameters);
void vSemicronNmtCommand (void *pvParameters);
void vSemicronSyn(void *pvParameters);
void vSemicronNmtNodeGuarding(void *pvParameters);

MessageBufferHandle_t xMessageBuffer;
TaskHandle_t xSemicronRxHandler;
TaskHandle_t xNMTCommand;

static VcuStateMangement_t  vcuStatus = VCU_Status_Init;
void semikronRxInit(void)
{
    if(xTaskCreate(vSemicronRxHandler, "SemicronRxHandler", configMINIMAL_STACK_SIZE, (void*)CAN_PERIOD_MS_SEMICRON_HANDLER, 1, &xSemicronRxHandler) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    if(xTaskCreate(vSemicronNmtCommand, "NMT_Command", configMINIMAL_STACK_SIZE, NULL, 1, &xNMTCommand ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    if(xTaskCreate(vSemicronSyn, "SemicronSync", configMINIMAL_STACK_SIZE, (void*)CAN_PERIOD_MS_SEMICRON_SYN, 1, NULL) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    if(xTaskCreate(vSemicronNmtNodeGuarding, "NMT_NodeGuarding", configMINIMAL_STACK_SIZE, (void*)CAN_PERIOD_MS_NMT_NODE_GUARDING, 1, NULL) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    xMessageBuffer = xMessageBufferCreate(sizeof(nmtCommandSpecifier_t) + 4);

}

static void clearErrorAction(canMessage_t *ptr)
{
    setRx_PDO_03ControlMode(ptr, (uint8_t)DISABLED);
    setRX_PDO_03ClearError(ptr, (uint8_t)CLEAR_ERROR);
    setRx_PDO_03TorqueRefLim(ptr, RX_PDO_03_TORQUE_REF_LIM(0));
    setRx_PDO_03SpeedRefLim(ptr, RX_PDO_03_SPEED_REF_LIM(0));
}

static void carInMotion(canMessage_t *ptr, int TorqueValue)
{
    setRx_PDO_03ControlMode(ptr, (uint8_t)TORQUE_CONTROL_MODE);
    setRX_PDO_03ClearError(ptr, (uint8_t)DO_NOT_CLEAR);
    setRx_PDO_03TorqueRefLim(ptr, RX_PDO_03_TORQUE_REF_LIM(TorqueValue));
    setRx_PDO_03SpeedRefLim(ptr, RX_PDO_03_SPEED_REF_LIM(0));
}
static void carStop(canMessage_t *ptr)
{
    setRx_PDO_03ControlMode(ptr, (uint8_t)DISABLED);
    setRX_PDO_03ClearError(ptr, (uint8_t)DO_NOT_CLEAR);
    setRx_PDO_03TorqueRefLim(ptr, RX_PDO_03_TORQUE_REF_LIM(0));
    setRx_PDO_03SpeedRefLim(ptr, RX_PDO_03_SPEED_REF_LIM(0));
}

/**
 * @void vSemicronRxHandler (void *pvParameters)
 * @brief To control the inverter giving it speed or torque, setting its state and clearing errors if exists
 * */
void vSemicronRxHandler (void *pvParameters)
{
    TickType_t lastWeakTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters );

    VcuStateMangement_t  vcuStatus = VCU_Status_Init;;
    int torqueValue = 0 ;

    RX_PDO_03LimitationMode_t limitationMode = SYMMETRIC;
    //clearError_t clearError = DO_NOT_CLEAR;
    Rx_PDO_03ControlMode_t controlMode = DISABLED;

    canMessage_t rxPdo_03 =
    {
     .id  = SEMICRON_SEMICRON_HANDLER,
     .dlc = SEMICRON_HANDLER_DLC,
     .ide = (uint8_t)CAN_Id_Standard,
    };
    setRx_PDO_03ControlMode(&rxPdo_03, (uint8_t)controlMode);
    setRx_PDO_03TorqueRefLim(&rxPdo_03, RX_PDO_03_TORQUE_REF_LIM(0));
    setRx_PDO_03SpeedRefLim(&rxPdo_03, RX_PDO_03_SPEED_REF_LIM(0));
    setRX_PDO_03LimitationMode(&rxPdo_03, (uint8_t)limitationMode);

    lastWeakTime = xTaskGetTickCount();
    for(;;)
    {
        xQueuePeek(xQueueVcuStatus, &vcuStatus, pdMS_TO_TICKS(0));
        if (vcuStatus == VCU_CLEAR_ERROR)
            clearErrorAction(&rxPdo_03);
        else if (vcuStatus == VCU_Status_Forward || vcuStatus == VCU_Status_Reverse)
        {
            xQueuePeek(xqueueAcceleratorValue, &torqueValue, pdMS_TO_TICKS(0));
            carInMotion(&rxPdo_03, torqueValue);
        }
        else
            carStop(&rxPdo_03);

        newCanTransmit(canREG2, canMESSAGE_BOX4, &rxPdo_03);
        vTaskDelayUntil( &lastWeakTime, transmitPeriod);
    }
}
/**
 * void vSemicronSyn(void *pvParameters)
 * @brief heart beat which needs to be sent every 100ms
 * */
void vSemicronSyn(void *pvParameters)
{
    TickType_t lastWeakTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters );

    canMessage_t semicronSync =
    {
     .id = SEMICRON_SYN,
     .dlc = SEMICRON_SYN_DLC,
     .ide = (uint8_t)CAN_Id_Standard,
    };

    lastWeakTime = xTaskGetTickCount();

    for(;;)
    {
        newCanTransmit(canREG2, canMESSAGE_BOX2, &semicronSync);

        vTaskDelayUntil( &lastWeakTime, transmitPeriod );
    }
}

/**
 * @static boolean statusNmtGuardingChanged( nmtNodeGuardingState_t*  nmtNodeGuardingState)
 * @brief checks if the state has changed
 * @return true if it is changed or false if not
 * */
static boolean isStatusNmtGuardingChanged( nmtNodeGuardingState_t  NodeGuardingState, nmtCommandSpecifier_t *nmtCommandSpecifier  )
{
    static nmtNodeGuardingState_t nmtNodeGuardingState = PRE_OPERATIONAL;

    if (nmtNodeGuardingState == NodeGuardingState)
        return false;

    else
    {
        if(NodeGuardingState == OPERATIONAL)
            *nmtCommandSpecifier = START_REMOTE_NODE;
        else if(NodeGuardingState == PRE_OPERATIONAL)
            *nmtCommandSpecifier = RESET_NODE;
        else
            *nmtCommandSpecifier = STOP_REMOTE_NODE;

        nmtNodeGuardingState = NodeGuardingState;
        return true;
    }
}

/**
 * void vSemicronNmtNodeGuarding(void *pvParameters)
 * @brief sending nmt node guarding every 500ms
 * */
void vSemicronNmtNodeGuarding(void *pvParameters)
{
    TickType_t lastWeakTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters );

    nmtNodeGuardingState_t  nmtNodeGuardingState = PRE_OPERATIONAL;
    nmtCommandSpecifier_t nmtCommandSpecifier;

    canMessage_t semicronNodeGuarding =
    {
         .id = SEMICRON_RX_NMT_NODE_GUARDING,
         .dlc = SEMICRON_RX_NMT_NODE_GUARDING_DLC,
         .ide =  (uint8_t)CAN_Id_Standard,
    };
    lastWeakTime = xTaskGetTickCount();

    for(;;)
    {
        xQueuePeek(xQueueVcuStatus, &vcuStatus, pdMS_TO_TICKS(0));

        if(vcuStatus == VCU_Status_Parking || vcuStatus == VCU_Status_Neutral)
            nmtNodeGuardingState = OPERATIONAL;
        else if (vcuStatus ==  VCU_Status_Init)
            nmtNodeGuardingState = PRE_OPERATIONAL;

        if(isStatusNmtGuardingChanged(nmtNodeGuardingState, &nmtCommandSpecifier))
        {
            xMessageBufferSend(xMessageBuffer, ( void * )nmtCommandSpecifier, sizeof(nmtCommandSpecifier), pdMS_TO_TICKS(0));
        }/* else not needed */

        setNmtNodeGuardingState(&semicronNodeGuarding, (uint8_t) nmtNodeGuardingState);
        ToggleNmtNodeGuardingToggleBit(&semicronNodeGuarding);

        newCanTransmit(canREG2, canMESSAGE_BOX3, &semicronNodeGuarding);

        vTaskDelayUntil( &lastWeakTime, transmitPeriod);
    }
}

/**
 * void vSemicronNmtCommand (void *pvParameters)
 * @brief to set command specifier when state (node guarding state ) is changed
 * */
void vSemicronNmtCommand (void *pvParameters)
{
    nmtCommandSpecifier_t nmtCommandSpecifier;
    canMessage_t semicronNmtCommand =
    {
     .id = SEMICRON_RX_NMT_NMT_COMMAND,
     .dlc = SEMICRON_RX_NMT_NMT_COMMAND_DLC,
     .ide = (uint8_t)CAN_Id_Standard,
    };
    for(;;)
    {
        xMessageBufferReceive(xMessageBuffer, ( void * )nmtCommandSpecifier, sizeof(nmtCommandSpecifier), portMAX_DELAY);

        setNmtCommandSpecifier(&semicronNmtCommand, (uint8_t)nmtCommandSpecifier);

        newCanTransmit(canREG2, canMESSAGE_BOX1, &semicronNmtCommand);
        taskYIELD();
    }
}
