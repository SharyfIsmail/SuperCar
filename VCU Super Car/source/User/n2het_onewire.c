/*
 * n2het_onewire.c
 *
 *  Created on: Sep 18, 2019
 *      Author: Dmitriy Shulpenkov
 */

#include "stdbool.h"
#include "n2het_onewire.h"

/** @typedef functionalCommandQueue_t
*   @brief Functional command queue type definition
*
*   This type is used to save the sequence of execution of commands.
*/
typedef struct {
    uint8_t cmd[10];
    uint8_t head;
    uint8_t tail;
    uint8_t cmdCount;
} functionalCommandQueue_t;

/** @struct oneWire
*   @brief OneWire connection type definition
*
*   This type is used to control OneWire connection.
*/
struct oneWire{
    owDeviceCode_t device;                  /* Unique device code */
    uint8_t operationMode;                  /* Current operation mode N2HET program */
    bool isDevicePresence;                  /* State of presence signal from a slave device */
    functionalCommandQueue_t funcCmdQueue;  /* The queue of executable functional commands */
} ow;

/* Interface Functions of Functional Command Queue */
static void queueInit(functionalCommandQueue_t *queue);
static void pushFuncCommand(functionalCommandQueue_t *queue, const uint8_t command);
static bool popFuncCommand(functionalCommandQueue_t *queue, uint8_t *command);
static bool isEmpty(functionalCommandQueue_t *queue);
static bool isFull(functionalCommandQueue_t *queue);

/** @fn void OneWire_Init()
*   @brief Initializes the OneWire Driver
*
*   This function initializes the OneWire module.
*/
void OneWire_Init()
{
    ow.operationMode = STANDBY_OPERATION_MODE;
    ow.isDevicePresence = false;
    queueInit(&ow.funcCmdQueue);
}

/** @fn bool OneWire_TryReset()
*   @brief Tries to execute the initialization procedure(RESET AND PRESENCE PULSE)
*   @return The function will return:
*           - FALSE (0): Initialization procedure could not be executed.
*           - TRUE (1): Initialization procedure has been successfully sent for execution
*
*   This function tries to sent reset pulse if functional command queue is empty
*   and operation mode is standby.
*/
bool OneWire_TryReset()
{
    if(isEmpty(&ow.funcCmdQueue) && ow.operationMode == STANDBY_OPERATION_MODE)
    {
        ow.isDevicePresence = false;
        ow.operationMode = RESET_OPERATION_MODE;
        hetRAM1->Instruction[SELECT_OPERATION_MODE_INSTRUCTION_NUMBER].Data = RESET_OPERATION_MODE << 7;
        return true;
    }

    return false;
}

/** @fn void OneWire_WriteByte(uint8_t value)
*   @brief Transmit a data byte
*   @param[in] value Transmitting value
*
*   This function transmit value to slave device if initialization procedure(RESET AND PRESENCE PULSE) completed
*   and operation mode is standby otherwise value will add to command queue.
*/
void OneWire_WriteByte(uint8_t value)
{
    if(ow.isDevicePresence && ow.operationMode == STANDBY_OPERATION_MODE)
    {
        hetRAM1->Instruction[DATA_WRITE_INSTRUCTION_NUMBER].Data = value << 7;

        hetRAM1->Instruction[SELECT_OPERATION_MODE_INSTRUCTION_NUMBER].Data = WRITE_OPERATION_MODE << 7;//WRITE
        ow.operationMode = WRITE_OPERATION_MODE;
    }
    else
    {
        pushFuncCommand(&ow.funcCmdQueue, value);
    }
}

/** @fn bool OneWire_TryReadByte(uint8_t *value)
*   @brief Tries to execute reading byte
*   @param[out] value Pointer to data value
*   @return The function will return:
*           - FALSE (0): Reading could not be executed.
*           - TRUE (1): Reading data byte completed successfully
*
*   This function send command to start read data byte if initialization procedure(RESET AND PRESENCE PULSE) completed
*   and operation mode is standby. Then this function waits for an interrupt of the end of byte reading to save its
*/
bool OneWire_TryReadByte(uint8_t *value)
{
    if(ow.isDevicePresence)
    {
        if(ow.operationMode == STANDBY_OPERATION_MODE)
        {
            hetRAM1->Instruction[READ_NUMBER_OF_BITS_INSTRUCTION_NUMBER].Data = 8 << 7;//read 8 bit

            hetRAM1->Instruction[SELECT_OPERATION_MODE_INSTRUCTION_NUMBER].Data = READ_OPERATION_MODE << 7;//READ
            ow.operationMode = READ_OPERATION_MODE;
        }

        if(hetREG1->FLG & (1 << N2HET_END_READING_BYTE_INTERRUPT_FLAG))
        {
            hetREG1->FLG = (1 << N2HET_END_READING_BYTE_INTERRUPT_FLAG);
            ow.operationMode = hetRAM1->Instruction[SELECT_OPERATION_MODE_INSTRUCTION_NUMBER].Data >> 7;
            *value = (uint8_t)(hetRAM1->Instruction[DATA_READ_INSTRUCTION_NUMBER].Data >> 7);
            return true;
        }
    }

    return false;
}

/** @fn const owDeviceCode_t * OneWire_ReadROM()
*   @brief Reads a unique device code
*   @return The function will return unique device code
*
*   This function sends commands to the READ ROM function and after that it waits for a unique device code.
*/
const owDeviceCode_t * OneWire_ReadROM()
{
    OneWire_WriteByte(ONEWIRE_CMD_READ_ROM);

    while(!OneWire_TryReadByte(&ow.device.familyCode));

    for(uint8_t i = 0, size = sizeof(ow.device.serialNumber); i < size;){
        if(OneWire_TryReadByte(&ow.device.serialNumber[i])){
            i++;
        }
    }

    while(!OneWire_TryReadByte(&ow.device.crc));

    return &ow.device;
}

/** @fn void OneWire_SkipROM()
*   @brief Transmit SKIP ROM command
*
*   This function transmits SKIP ROM command.
*/
void OneWire_SkipROM()
{
    OneWire_WriteByte(ONEWIRE_CMD_SKIP_ROM);
}

/** @fn void N2Het_OneWireInterruptHandle(uint32_t offset)
*   @brief Interrupt handler for OneWire module on N2HET
*   @param[in] offset N2HET interrupt offset / Source number
*
*   This function is required to call the N2HET interrupt callback function to handle the OneWire module interrupt on N2HET.
*/
void N2Het_OneWireInterruptHandle(uint32_t offset)
{
    uint32_t instructionNumber = offset - N2HET_INTERRUPT_INSTRUCTION_OFFSET;

    ow.operationMode = hetRAM1->Instruction[SELECT_OPERATION_MODE_INSTRUCTION_NUMBER].Data >> 7;
    if(instructionNumber == DEVICE_READY_INSTRUCTION_NUMBER)
    {
        ow.isDevicePresence ^= 1;

        uint8_t cmd;
        if(popFuncCommand(&ow.funcCmdQueue, &cmd))
        {
            OneWire_WriteByte(cmd);
        }
    }
    else if(ow.isDevicePresence && instructionNumber == END_OF_DATA_WRITE_INSTRUCTION_NUMBER)
    {
        uint8_t cmd;
        if(popFuncCommand(&ow.funcCmdQueue, &cmd))
        {
            OneWire_WriteByte(cmd);
        }
    }
}

/** @fn static void queueInit(functionalCommandQueue_t *queue)
*   @brief Initializes the functional command queue
*   @param[out] queue - Pointer to queue in OneWire connection object
*
*   This function initializes the functional command queue for OneWire connection object.
*/
static void queueInit(functionalCommandQueue_t *queue)
{
    queue->head = 0;
    queue->tail = 0;
    queue->cmdCount = 0;
    memset(queue->cmd, 0, sizeof(queue->cmd));
}

/** @fn static void pushFuncCommand(functionalCommandQueue_t *queue, const uint8_t command)
*   @brief Push new command to queue
*   @param[in/out] queue Pointer to queue in OneWire connection object
*   @param[in] command Pushing value to queue
*
*   This function pushes command to queue if it isn't full.
*/
static void pushFuncCommand(functionalCommandQueue_t *queue, const uint8_t command)
{
    if(!isFull(queue))
    {
        queue->cmd[queue->tail] = command;
        queue->cmdCount++;

        if(queue->tail + 1 == sizeof(queue->cmd)){
            queue->tail = 0;
        }
        else {
            queue->tail++;
        }
    }
}

/** @fn static bool popFuncCommand(functionalCommandQueue_t *queue, uint8_t *command)
*   @brief Extracts value from the queue
*   @param[in/out] queue Pointer to queue in OneWire connection object
*   @param[out] command Pointer to value extracting from queue
*   @return The function will return:
*           - FALSE (0): Queue is empty
*           - TRUE (1): Extracting value completed successfully
*
*   This function extracts command from queue if it isn't empty.
*/
static bool popFuncCommand(functionalCommandQueue_t *queue, uint8_t *command)
{
    if(!isEmpty(queue))
    {
        *command = queue->cmd[queue->head];
        queue->cmdCount--;

        if(queue->head + 1 == sizeof(queue->cmd)){
            queue->head = 0;
        }
        else {
            queue->head++;
        }
        return true;
    }

    return false;
}

/** @fn static bool isEmpty(functionalCommandQueue_t *queue)
*   @brief Checks the queue for missing values
*   @param[in/out] queue Pointer to queue in OneWire connection object
*   @return The function will return:
*           - FALSE (0): Queue is not empty
*           - TRUE (1): Queue is empty
*
*   This function checks the queue for missing values.
*/
static bool isEmpty(functionalCommandQueue_t *queue)
{
    return queue->head == queue->tail && !queue->cmdCount;
}

/** @fn static bool isFull(functionalCommandQueue_t *queue)
*   @brief Checking the queue for fullness
*   @param[in/out] queue Pointer to queue in OneWire connection object
*   @return The function will return:
*           - FALSE (0): Queue is not full
*           - TRUE (1): Queue is full
*
*   This function checks if the queue is full.
*/
static bool isFull(functionalCommandQueue_t *queue)
{
    return queue->cmdCount == sizeof(queue->cmd);
}
