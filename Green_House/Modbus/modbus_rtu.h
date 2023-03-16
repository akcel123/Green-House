/*
 * modbus_rtu.h
 *
 *  Created on: 9 мар. 2023 г.
 *      Author: akcel
 */

#ifndef MODBUS_RTU_H_
#define MODBUS_RTU_H_

#include "timer_interface.h"

#include "hardware_interface.h"
#include "crc_calculator.h"

#include "modbus_enums.h"
#include "modbus_init_structs.h"
#include "modbus_responses.h"
#include "modbus_requests.h"

//-------------------------------------------------------------
//	Определяет максимальный размер пакета (в байтах),
//	принятого за одну транзакцию
//-------------------------------------------------------------
#define MAX_NUM_PACK 255

namespace Modbus {


	class ModbusRtu {
	private:

		TimerInterface* timer_;			// свойство, определяющее программный/аппаратный таймер
		HardwareInterface* interface_;	// свойство, определяющее аппаратный интерфейс передачи данных

		Mode mode_;							// master or slave
		State state_;						// state machine
		Error error_state_;					// for errors

		// структура ниже определяет работу с данными, которые приходят по физическому интерфейсу
		struct {
			PackageState state_;
			struct {
				uint32_t overflow_packages_;
				uint32_t corrupted_packages_;
			} error_counters_;
		} data_;


		// helper struct for configurate resp and req
		struct{
			uint16_t addr_;
			uint16_t val_;
			uint8_t id_;
			uint8_t command_;
			uint8_t numReg_;
			uint8_t numBytes_;
			uint8_t len_;
			void* buf_for_rw_;
			Commands current_command_;
		} params_;

		DeviceParameters device_parameters_;	// параметры устройства, определяемые пользователем


		// enum определяет какой коллбек по приему байта вызывать
		enum {
			kRxTrueCallback,
			kRxCorruptedCallback,
			kRxMasterCallback,
		} current_rx_callback_function_;


		//TODO логика, связанная со свойствами ниже немного странная, по сути какой то псевдо программный таймер для определения таймаута
		// таймаут нужен обязательно, вопрос в реализации открыт.
		uint16_t callback_frequency_;						// частота вызова функции update
		uint16_t reseive_timeout_counter_;					// счетчик для таймаута приема ответа от slave'a
		ExceptionCode exception_code_;


		void Init(Mode mode);

		void SlaveUpdate();
		void MasterUpdate();
		void Reset();

		void SlaveRxCorruptCallback();
		void SlaveRxTrueCallback();
		void RxMasterCallback();

		Error AccReq();
		bool CheckPackageCrc();
		Error SlaveParseData();

		State CalculateTimings15t(uint16_t baudrate);
		State CalculateTimings35t(uint16_t baudrate);
		State CalculateTimings20t(uint16_t baudrate);


		void ResponseMasterWithCommend(Commands current_command);
		void ResponseSlaveWithCommand(Commands current_command);
		void RequestMasterWithCommend(Commands current_command);

		void MasterCheckResponce0x01();
		void MasterCheckResponce0x02();
		void MasterCheckResponce0x03();
		void MasterCheckResponce0x05();
		void MasterCheckResponce0x0F();
		void MasterCheckResponce0x10();
		void MasterCheckResponce0x11();

		void MasterSendRequest0x01();
		void MasterSendRequest0x02();
		void MasterSendRequest0x03();
		void MasterSendRequest0x05();
		void MasterSendRequest0x0F();
		void MasterSendRequest0x10();
		void MasterSendRequest0x11();

		void SlaveResponse0x01();
		void SlaveResponse0x02();
		void SlaveResponse0x03();
		void SlaveResponse0x05();
		void SlaveResponse0x0F();
		void SlaveResponse0x10();
		void SlaveResponse0x11();
		void SlaveError();


	public:

		uint8_t id = 0x01;


		//---------------------------------------------------------------------------------------------------
		//	Конструктор
		//
		// сюда необходимо поместить ссылки на экземпляры реализованных вами таймер и интерфейса
		// а также modbus роль (master or slave)
		//---------------------------------------------------------------------------------------------------
		ModbusRtu(TimerInterface* timer, HardwareInterface* interface, Mode mode);
		//---------------------------------------------------------------------------------------------------
		//	функция SetDeviceParameters()
		//
		//
		///		parameters - заполненная структура параметров устройства (обязательна для заполнения)
		//---------------------------------------------------------------------------------------------------
		void SetDeviceParameters(DeviceParameters parameters) {this->device_parameters_ = parameters;}

		//---------------------------------------------------------------------------------------------------
		//	функция Update()
		//	Данная функция помещается в основной коллбек
		//---------------------------------------------------------------------------------------------------
		void Update();

		//---------------------------------------------------------------------------------------------------
		//	функция RxCallback()
		//	Данная функция помещается в колбек по приему 1го байта данных ПОСЛЕ!!! записи данных в буфер интерфейса
		//---------------------------------------------------------------------------------------------------
		void RxCallback();

		//---------------------------------------------------------------------------------------------------
		//	функция SetTransmitCompleteFlag()
		//	Данная функция вызывается после успешной отправки всех данных по линии tx
		//---------------------------------------------------------------------------------------------------
		void SetTransmitCompleteFlag();

		//---------------------------------------------------------------------------------------------------
		//	функция Request()
		//
		//	Данная функция инициализирует отправку запроса слейву в соответствии со структурой запроса
		//
		///		request - указатель на заполненнуж структуру запроса (см. modbus_requests.h)
		/// 	command - команда modbus в соответствие с перечислением MbCommands
		//---------------------------------------------------------------------------------------------------
		Error Request(void* request, Commands command);
		//---------------------------------------------------------------------------------------------------
		//	функция ReturnSlaveResponce()
		//
		//	Данная функция заполняет соответствующую структуру ответа Slave'а, если нет ошибок,
		//	если есть ошибки - возвращает ошибку
		//
		///		response - указатель на структуру, в которую запишется ответ Slave'a (см. modbus_responses.h)
		//---------------------------------------------------------------------------------------------------
		Error ReturnSlaveResponce(void* response);

		State GetState() {return this->state_;}
		Error GetErrorState() {return this->error_state_;}
		uint32_t GetCountOverflowPackage() {return this->data_.error_counters_.overflow_packages_;}
		uint32_t GetCountCorrupdetPackage() {return this->data_.error_counters_.corrupted_packages_;}

		static void Callback15t(ModbusRtu* obj);
		static void Callback35t(ModbusRtu* obj);
		virtual ~ModbusRtu();

	};






}


#endif /* MODBUS_RTU_H_ */
