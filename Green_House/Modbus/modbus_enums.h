#ifndef MODBUS_ENUMS_H_
#define MODBUS_ENUMS_H_

namespace Modbus {

	//перечисление, определяет состояние модбаса
	typedef enum {
		kNotInit = 0,		//драйвер не проинициализирован
		kZeroState,			//начальное состояние драйвера после инициализации
		kWaitData,
		kNeedResp,			//нужен ответ мастеру
		kResReq,			//успешно принят запрос
		kNeedSendResp,		//нужно отправить ответ (устанавливается, когда ответ сформирован)
		kError,
		//ниже прописаны стейты для мастера
		kReseivingResp,
		kResResp,
		kNeedSendReq,
		kNeedReadResp
	}State;



	typedef enum {
		kMaster = 0,
		kSlave
	}Mode;


	typedef enum {
		kClearPackage = 0,
		kCorrupted,
		kOverflow,
	} PackageState;

	//Пока не используется
	typedef enum {
		kNotError = 0,
		kNotSupportedFunctionCode,
		kNotAvaibleAddrCode,
		kDataValueError,
		kCrcError,
		kCommandExecutionError,
		kSlaveResponseError,		// поля ответа слейва не совпадают с полями мастера.
		kSlaveResponseTimeoutError,	// тайматут ответа Slave'a

		kBusyError		//ошибка возникает когда мы пытаемся отправить запрос, но драйвер занят или когда пытаемся прочитать ответ, но ответа нет или когда пытаемся отправить запрос, не прождав 3.5t после ответа слейва
	} Error;



	//все поддерживаемые команды
	typedef enum {
		kReadCoild0x01 = 0,
		kReadDiscreteInputs0x02 = 1,
		kReadHoldingRegisters0x03 = 2,
		kWriteSingleCoil0x05 = 3,
		kWriteMultipleCoils0x0F = 4,
		kWriteMultipleRegisters0x10 = 5,
		kReportServerID0x11 = 6,

		kAllFunctions = 7

	} Commands;


	//все коды ошибок, формируемые при отправки пакета ошибки
	typedef enum {
		kExceptionCode1 = 1,
		kExceptionCode2 = 2,
		kExceptionCode3 = 3,
		kExceptionCode4 = 4,
	} ExceptionCode;

}


#endif /* MODBUS_ENUMS_H_ */
