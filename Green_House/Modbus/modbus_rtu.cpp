/*
 * modbus_rtu.cpp
 *
 *  Created on: 9 мар. 2023 г.
 *      Author: akcel
 */

#include "modbus_rtu.h"
#include "crc_calculator.h"
// XXX Необходимо реализовать доступ к глобальным регистрам и подумать как это сделать


namespace Modbus {

#define CRC16_SIZE									2

	const uint8_t kFunctionCode[kAllFunctions] = {
			0x01,
			0x02,
			0x03,
			0x05,
			0x0F,
			0x10,
			0x11

	};

	void Callback35tWrapper(void* obj) {
		ModbusRtu* mb_obg = (ModbusRtu*)obj;
		ModbusRtu::Callback35t(mb_obg);
	}

	void Callback15tWrapper(void* obj) {
		ModbusRtu* mb_obg = (ModbusRtu*)obj;
		ModbusRtu::Callback15t(mb_obg);
	}



	ModbusRtu::ModbusRtu(TimerInterface* timer, HardwareInterface* interface, Mode mode) {
		this->timer_ = timer;
		this->interface_ = interface;
		this->mode_ = mode;
		Init(mode);
	}



	ModbusRtu::~ModbusRtu() {

	}

	void ModbusRtu::Init(Mode mode) {

		this->state_ = kZeroState;
		this->params_.addr_ = 0;
		this->params_.val_ = 0;
		this->params_.id_ = 0;
		this->params_.command_ = 0;
		this->params_.numReg_ = 0;
		this->params_.numBytes_ = 0;
		this->params_.len_ = 0;

		this->data_.error_counters_.corrupted_packages_ = 0;
		this->data_.error_counters_.overflow_packages_ = 0;

		this->interface_->state_ = kSucsecc;
		this->interface_->rx_len = 0;
		this->interface_->tx_len = 0;
		this->data_.state_ = kClearPackage;
		this->timer_->SetCallBackWithArg(Callback15tWrapper, (void*)this);

		switch(mode) {
			case kSlave: // !!!
				this->current_rx_callback_function_ = kRxTrueCallback;
				break;
			case kMaster:
				this->current_rx_callback_function_ = kRxMasterCallback;
				break;
		}

	}



	void ModbusRtu::RxCallback() {
		switch (this->current_rx_callback_function_) {
		case kRxTrueCallback:
			this->SlaveRxTrueCallback();
			break;
		case kRxCorruptedCallback:
			this->SlaveRxCorruptCallback();
			break;
		case kRxMasterCallback:
			this->RxMasterCallback();
			break;
		}
	}



	void ModbusRtu::Update() {
		switch (this->mode_) {
		case kMaster:
			this->MasterUpdate();
			break;
		case kSlave:
			this->SlaveUpdate();
			break;
		}
	}


	void ModbusRtu::Callback15t(ModbusRtu* obj)	{

		if(obj->mode_ == kMaster) return;
		obj->timer_->Stop();

		//FIXME исправить условие ниже (вроде исправил)
		if(!(obj->data_.state_ == kOverflow))
			obj->current_rx_callback_function_ = kRxCorruptedCallback;
		obj->CalculateTimings20t(obj->interface_->GetCurrentBaudrate());
		obj->timer_->SetCallBackWithArg(Callback35tWrapper, (void*)obj);
		obj->timer_->Start();

	}

	void ModbusRtu::Callback35t(ModbusRtu* obj)	{
		obj->timer_->Stop();

		if(obj->mode_ == kMaster) {

			if(obj->state_ == kNeedSendReq)	{
				obj->current_rx_callback_function_ = kRxMasterCallback;
				obj->state_ = kReseivingResp;
			} else if(obj->state_ == kReseivingResp) {
				obj->state_ = kResResp;
			}
			return;
		}

		if (obj->state_ == kNeedResp) {
			obj->state_ = kNeedSendResp;
//			m->programm_timer.state = kOff;
			return;
		}


		switch (obj->data_.state_) {
		case kOverflow:
			++(obj->data_.error_counters_.overflow_packages_);
			obj->state_ = kZeroState;
			break;
		case kCorrupted:
			++(obj->data_.error_counters_.corrupted_packages_);
			obj->state_ = kZeroState;
			break;
		case kClearPackage:
			//устанавливаем флаг успешного приема пакета
			obj->state_ = kResReq;
			break;
		}

//		m->programm_timer.state = kOff;
	}

	// данная функция вызывается после отправки ВСЕХ данных
	void ModbusRtu::SetTransmitCompleteFlag() {
		if(this->mode_ == kMaster) {

			this->timer_->SetCallBackWithArg(Callback35tWrapper, (void*)this);
			this->CalculateTimings35t(this->interface_->GetCurrentBaudrate());
			this->timer_->Start();
		} else {
			this->state_ = kZeroState;
			this->timer_->Stop();
		}


	}



	void ModbusRtu::SlaveUpdate()
	{


		if(this->timer_->GetTimerState() == kTimerOn) return;

	    switch (this->state_) {
		case kNotInit: 			//what are u doing here?
			return;
			break;
		case kZeroState:
			this->Reset();
			break;
		case kWaitData:
			return;
			break;
		case kNeedResp:			//нужен ответ


			break;
		case kResReq:			//успешно принят запрос
			//m->interface_functions.rx_it_disable(m->interface_functions.arg);
			this->error_state_ = this->AccReq();
			if(this->error_state_ == kNotError)	{
				ResponseSlaveWithCommand(this->params_.current_command_);
			}
			else if (this->error_state_ == kCrcError) return;
			this->timer_->SetCallBackWithArg(Callback35tWrapper, (void*)this);
			this->CalculateTimings35t(this->interface_->GetCurrentBaudrate());
			this->timer_->Start();

			break;
		case kNeedSendResp:		//нужно отправить ответ
			if(!(this->interface_->state_ == kBusy)) {
				this->interface_->SendData();
			}

			//устанавливаем флаг занятости периферии, передающей данные.
			break;
		default:
			return;//???

		}



	}





	void ModbusRtu::Reset() {
		if(this->mode_ == kSlave) {

			this->current_rx_callback_function_ = kRxTrueCallback;
			this->interface_->rx_len = 0;
			this->interface_->tx_len = 0;
			this->state_ = kWaitData;
			this->data_.state_ = kClearPackage;

			this->timer_->Stop();
			this->timer_->SetCallback( (TimerCallback)this->Callback15t);
			this->CalculateTimings15t(this->interface_->GetCurrentBaudrate());

		} else if (this->mode_ == kMaster) {

			this->current_rx_callback_function_ = kRxMasterCallback;
			this->interface_->rx_len = 0;
			this->interface_->tx_len = 0;
			this->state_ = kWaitData;
			this->data_.state_ = kClearPackage;

			// XXX почему в функцию передается коллбек 35t???
			this->timer_->Stop();
			this->timer_->SetCallBackWithArg(Callback35tWrapper, (void*)this);
			this->CalculateTimings35t(this->interface_->GetCurrentBaudrate());
		}

	}





	Error ModbusRtu::AccReq() {

		bool crcMatch = this->CheckPackageCrc();
		Error error = kNotError;
		if(crcMatch) {
			error = SlaveParseData();
			if(this->params_.id_ == 0x00) {
				// TODO вот тут нужно сделать общий запрос!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if(error == kNotError) {

				} else
					this->SlaveError();
			} else if(this->params_.id_ == this->id) {
				this->state_ = kNeedResp;
				if(error != kNotError)
					this->SlaveError();
			} else {
				this->SlaveError();
			}

		} else {
			this->state_ = kZeroState;	//вот тут надо подумать какое состояние устанавливать (вроде на данный момент установлено верно)
			error = kCrcError;
		}
		return error;
	}

	bool ModbusRtu::CheckPackageCrc() {
		uint16_t checked_crc = this->interface_->rx_buf[this->interface_->rx_len - 2] << 8 | this->interface_->rx_buf[this->interface_->rx_len - 1];
		bool crc_match = Crc::CrcCalculator::getInstance()->CheckCrc(this->interface_->rx_buf, this->interface_->rx_len - 2, checked_crc);
		return crc_match;
	}

	Error ModbusRtu::SlaveParseData() {
		uint8_t temp_id = this->interface_->rx_buf[0];
		uint8_t tempFunc = this->interface_->rx_buf[1];
		uint16_t tempAddr = (this->interface_->rx_buf[2] << 8) | (this->interface_->rx_buf[3]);
		uint16_t tempNumReg = (this->interface_->rx_buf[4] << 8) | (this->interface_->rx_buf[5]);
		uint8_t tempNumBytes = 0;
		Error error = kNotError;

		this->params_.addr_ = 0;
		this->params_.numReg_ = 0;
		this->params_.numBytes_ = 0;
		this->params_.command_ = 0;
		this->params_.len_ = 0;
		this->params_.val_ = 0;

		this->params_.id_ = temp_id;



		switch(tempFunc) {

		case 0x01:

			this->params_.command_ = tempFunc;
			this->params_.current_command_ = kReadCoild0x01;


			if((tempNumReg < 0x0001) || (tempNumReg > 0x07D0)) {
				this->exception_code_ = kExceptionCode3;
				error = kDataValueError;
			} else if((tempAddr >= this->device_parameters_.num_of_coils) || (tempAddr + tempNumReg >= this->device_parameters_.num_of_coils)) {
				this->exception_code_ = kExceptionCode2;
				error = kNotAvaibleAddrCode;
			} else {
				this->params_.addr_ = tempAddr;
				this->params_.len_ = tempNumReg;
			}

			break;

		case 0x02:
			this->params_.command_ = tempFunc;
			this->params_.current_command_ = kReadDiscreteInputs0x02;

			if((tempNumReg < 0x0001) || (tempNumReg > 0x07D0)) {
				this->exception_code_ = kExceptionCode3;
				error = kDataValueError;
			} else if((tempAddr >= this->device_parameters_.num_of_discrete_in) || (tempAddr + tempNumReg >= this->device_parameters_.num_of_discrete_in)) {
				this->exception_code_ = kExceptionCode2;
				error = kNotAvaibleAddrCode;
			} else {
				this->params_.addr_ = tempAddr;
				this->params_.len_ = tempNumReg;
			}



			break;

		case 0x05:
			this->params_.command_ = tempFunc;
			this->params_.current_command_ = kWriteSingleCoil0x05;

			if((tempNumReg != 0x0000) && (tempNumReg != 0xFF00)) {
				this->exception_code_ = kExceptionCode3;
				error = kDataValueError;
			} else if(tempAddr >= this->device_parameters_.num_of_coils) {
				this->exception_code_ = kExceptionCode2;
				error = kNotAvaibleAddrCode;
			} else {
				this->params_.addr_ = tempAddr;
				this->params_.val_ = tempNumReg;
			}

			break;

		case 0x0F:
			this->params_.command_ = tempFunc;
			this->params_.current_command_ = kWriteMultipleCoils0x0F;
			tempNumBytes = this->interface_->rx_buf[6];
			if(((tempNumReg < 0x0001) || (tempNumReg > 0x07B0))) {
				this->exception_code_ = kExceptionCode3;
				error = kDataValueError;

			} else if((tempAddr >= this->device_parameters_.num_of_coils) || (tempAddr + tempNumReg >= this->device_parameters_.num_of_coils)) {
				this->exception_code_ = kExceptionCode2;
				error = kNotAvaibleAddrCode;

			} else {
				this->params_.addr_ = tempAddr;
				this->params_.len_ = tempNumReg;
				//this->params_.val_ = (this->interface_->rx_buf[7] << 8) | (this->interface_->rx_buf[8]);
				this->params_.val_ = this->interface_->rx_buf[7];
			}

			break;

		case 0x11:
			this->params_.command_ = tempFunc;
			this->params_.current_command_ = kReportServerID0x11;

			break;

		case 0x03:

			this->params_.command_ = tempFunc;
			this->params_.numBytes_ = (uint8_t)(tempNumReg * 2);
			this->params_.current_command_ = kReadHoldingRegisters0x03;

			if(((tempNumReg < 0x0001) || (tempNumReg > 0x007D))) {
				this->exception_code_ = kExceptionCode3;
				error = kDataValueError;
			} else if((tempAddr >= this->device_parameters_.num_of_holding_registers) || (tempAddr + tempNumReg >= this->device_parameters_.num_of_holding_registers)) {
				this->exception_code_ = kExceptionCode2;
				error = kNotAvaibleAddrCode;
			}
			// блок кода ниже можно раскомментировать для обработки режима только чтение
	//		else if((tempAddr >= kBeginSystemVar) && (!global_var.opened_system_var))		//иначе если это системные переменные и нельзя их читать
	//		{
	//			this->exception_code_ = kExceptionCode2;
	//			error = kReadWriteSystemVarWithoutPassw;
	//		}
			else {
				this->params_.addr_ = tempAddr;
				this->params_.numReg_ = tempNumReg;
			}

			break;
		case 0x10:

			this->params_.current_command_ = kWriteMultipleRegisters0x10;
			this->params_.command_ = tempFunc;

			tempNumBytes = this->interface_->rx_buf[6];

			if(((tempNumReg < 0x0001) || (tempNumReg > 0x07D0)) && (tempNumBytes != tempNumReg * 2)) {
				this->exception_code_ = kExceptionCode3;
				error = kDataValueError;
			} else if((tempAddr >= this->device_parameters_.num_of_holding_registers) || (tempAddr + tempNumReg >= this->device_parameters_.num_of_holding_registers)) {
				this->exception_code_ = kExceptionCode2;
				error = kNotAvaibleAddrCode;
			} else {
				//закомментированный блок кода ниже необходим для реализации режима только чтение (плохо реализован, править)
//				uint8_t check_addr = tempAddr / 2;
//				uint8_t check_len = (tempNumReg % 2) ? tempNumReg / 2  + 1: tempNumReg / 2;

//				for(int i = check_addr; i < (check_addr + check_len); i++)
//					if(var_pointers[i]->modbus_access == kModbusRead)
//					{
//						this->exception_code_ = kExceptionCode2;
//						error = kWriteOnlyReadVar;
//						return error;
//					}

				this->params_.addr_ = tempAddr;
				this->params_.numReg_ = tempNumReg;
				this->params_.numBytes_ = tempNumBytes;
			}


			if((this->interface_->rx_len - CRC16_SIZE - 7) == tempNumBytes) {
				if(tempNumBytes == tempNumReg * 2) {
					this->params_.numBytes_ = tempNumBytes;
				} else {
					error = kNotAvaibleAddrCode; // NOT_AVAILIBLE_ADDR_CODE
				}
			} else {
				error = kNotAvaibleAddrCode; // NOT_AVAILIBLE_ADDR_CODE
			}
			break;

			default:
				error = kNotSupportedFunctionCode; //  NOT_SUPPORTED_FUNCTION_CODE
				this->exception_code_ = kExceptionCode1;
			break;
		}
		return error;
	}


	void ModbusRtu::SlaveRxTrueCallback() {
		// если мы попали сюда в ненужный стейт или если пакет сломан, а мы все равно оказались по какой то причине в данной функции
		if((this->state_ != kWaitData) || (this->data_.state_ != kClearPackage) || ((this->state_ == kError))) return;

		this->timer_->Stop();

		//если количество принятых байт равно максимальному значению
		if(this->interface_->rx_len >= MAX_NUM_PACK) {
			//устанавливаем флаг переполнения
			this->data_.state_ = kOverflow;
			this->interface_->rx_len = 0;
			//отключаем прием, чтобы обновить драйвер
		} else {
			//Устанавливаем период 1.5t
			this->CalculateTimings15t(this->interface_->GetCurrentBaudrate());
			this->timer_->SetCallBackWithArg(Callback15tWrapper, (void*)this);
			this->timer_->Start();
		}


	}

	void ModbusRtu::SlaveRxCorruptCallback() {

		if((this->data_.state_ != kClearPackage) || ((this->state_ == kError))) return;
		this->timer_->Stop();

		this->data_.state_ = kCorrupted;
		this->interface_->rx_len = 0;

		this->timer_->SetCallBackWithArg(Callback35tWrapper, (void*)this);
		this->CalculateTimings35t(this->interface_->GetCurrentBaudrate());
		this->timer_->Start();
	}




	//Функции, использующиеся Master'ом

	//в данной функции мы формируем структуру modbus для запроса, а затем формируем массив tx_buf

	Error ModbusRtu::Request(void* request, Commands command) {

		if(this->state_ != kWaitData) return kBusyError;
		Error error = kNotError;


		switch (command) {

		case kReadCoild0x01:
		{
			Request0x01 *req = (Request0x01*) request;
			this->params_.id_ = req->id;
			this->params_.addr_ = req->addr;
			this->params_.current_command_ = command;
			this->params_.command_ = kFunctionCode[kReadCoild0x01];
			this->params_.numReg_ = req->num_coils; //кол-во койлов
			break;
		}
		case kReadDiscreteInputs0x02:
		{
			Request0x02 *req = (Request0x02*) request;
			this->params_.id_ = req->id;
			this->params_.addr_ = req->addr;
			this->params_.current_command_ = command;
			this->params_.command_ = kFunctionCode[kReadDiscreteInputs0x02];
			this->params_.numReg_ = req->num_inputs; //кол-во дискретных входов
			break;
		}
		case kReadHoldingRegisters0x03:
		{
			Request0x03 *req = (Request0x03*) request;
			this->params_.id_ = req->id;
			this->params_.addr_ = req->addr;
			this->params_.current_command_ = command;
			this->params_.command_ = kFunctionCode[kReadHoldingRegisters0x03];
			this->params_.numReg_ = req->num_reg; //кол-во регистров
			break;
		}
		case kWriteSingleCoil0x05:
		{
			Request0x05 *req = (Request0x05*) request;
			this->params_.id_ = req->id;
			this->params_.addr_ = req->addr;
			this->params_.current_command_ = command;
			this->params_.command_ = kFunctionCode[kWriteSingleCoil0x05];
			this->params_.val_ = req->coil_val;
			break;
		}
		case kWriteMultipleCoils0x0F:
		{
			Request0x0F *req = (Request0x0F*) request;
			this->params_.id_ = req->id;
			this->params_.addr_ = req->addr;
			this->params_.current_command_ = command;
			this->params_.command_ = kFunctionCode[kWriteMultipleCoils0x0F];
			this->params_.numReg_ = req->num_coils; //кол-во койлов

			this->params_.buf_for_rw_ = req->coil_value;
			break;
		}
		case kWriteMultipleRegisters0x10:
		{
			Request0x10 *req = (Request0x10*) request;
			this->params_.id_ = req->id;
			this->params_.addr_ = req->addr;
			this->params_.current_command_ = command;
			this->params_.command_ = kFunctionCode[kWriteMultipleRegisters0x10];
			this->params_.numReg_ = req->num_reg; //кол-во регистров
			this->params_.buf_for_rw_ = req->write_buf;
			break;
		}
		case kReportServerID0x11:
		{
			Request0x11 *req = (Request0x11*) request;
			this->params_.id_ = req->id;
			this->params_.current_command_ = command;
			this->params_.command_ = kFunctionCode[kReportServerID0x11];
			break;
		}
		default:
			error = kNotSupportedFunctionCode;
			break;

		}

		RequestMasterWithCommend(command);

		this->state_ = kNeedSendReq;
		return error;
	}



	Error ModbusRtu::ReturnSlaveResponce(void* response) {
		if(this->state_ != kNeedReadResp) return kBusyError;



		if(this->error_state_ != kNotError) {
			this->state_ = kZeroState;
			return this->error_state_;
		}

		Error error = kNotError;


		switch (this->params_.current_command_) {

		case kReadCoild0x01:
		{
			Response0x01 *res = (Response0x01*) response;

			uint8_t temp_num_bytes = this->interface_->rx_buf[2];
			*(res->num_of_bytes) = temp_num_bytes;
			for(int i = 0; i < temp_num_bytes; i++)
				res->coils_values[i] = this->interface_->rx_buf[3 + i];

			break;
		}
		case kReadDiscreteInputs0x02:
		{
			Response0x02 *res = (Response0x02*) response;
			uint8_t temp_num_bytes = this->interface_->rx_buf[2];
			*(res->num_of_bytes) = temp_num_bytes;
			for(int i = 0; i < temp_num_bytes; i++)
				res->inputs_status[i] = this->interface_->rx_buf[3 + i];
			break;
		}
		case kReadHoldingRegisters0x03:
		{
			Response0x03 *res = (Response0x03*) response;
			uint8_t temp_num_bytes = this->interface_->rx_buf[2];
			*(res->num_of_registers) = temp_num_bytes / 2;
			uint8_t rx_counter = 3;
			for(int i = 0; i < temp_num_bytes / 2; i++)
			{
				res->read_buf[i] = ((uint16_t)this->interface_->rx_buf[rx_counter++] << 8) & 0xFF00;
				res->read_buf[i] |= this->interface_->rx_buf[rx_counter++];
			}
			break;
		}
		case kWriteSingleCoil0x05:
		{

			break;
		}
		case kWriteMultipleCoils0x0F:
		{

			break;
		}
		case kWriteMultipleRegisters0x10:
		{

			break;
		}
		case kReportServerID0x11:
		{
			Response0x11 *res = (Response0x11*) response;

			*(res->size_of_server_id) = this->interface_->rx_buf[2];
			for(int i = 0; i < *(res->size_of_server_id); i++) res->server_id[i] = this->interface_->rx_buf[3 + i];


			if(this->state_ != kError)

				this->interface_->tx_buf[3 + this->device_parameters_.size_of_server_id] = 0xFF;
			else
				this->interface_->tx_buf[3 + this->device_parameters_.size_of_server_id] = 0x00;
			//???????????????????
			break;
		}
		default:
			error = kNotSupportedFunctionCode;
			break;

		}
		this->state_ = kZeroState;
		return error;
	}


	void ModbusRtu::ResponseSlaveWithCommand(Commands current_command) {


		switch (current_command) {
		case kReadCoild0x01:
			SlaveResponse0x01();
			break;
		case kReadDiscreteInputs0x02:
			SlaveResponse0x02();
			break;

		case kReadHoldingRegisters0x03:
			SlaveResponse0x03();
			break;
		case kWriteSingleCoil0x05:
			SlaveResponse0x05();
			break;
		case kWriteMultipleCoils0x0F:
			SlaveResponse0x0F();
			break;
		case kWriteMultipleRegisters0x10:
			SlaveResponse0x10();
			break;
		case kReportServerID0x11:
			SlaveResponse0x11();
			break;

		default: return;
		}
	}


	void ModbusRtu::SlaveResponse0x01() {

		this->interface_->tx_len = 3;
		uint16_t crc16 = 0;

		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		this->interface_->tx_buf[2] = (this->params_.len_ % 8 > 0) ? this->params_.len_ / 8 + 1 : this->params_.len_ / 8; //число байт

		uint16_t out_state = *(this->device_parameters_.coils_state);

		out_state = out_state >> (this->params_.addr_);
		out_state = out_state << (16 - this->params_.len_);
		out_state = out_state >> (16 - this->params_.len_);

		this->interface_->tx_buf[(this->interface_->tx_len)++] = out_state & 0x00FF;
		if (this->params_.len_ > 8)
			this->interface_->tx_buf[(this->interface_->tx_len)++] = out_state >> 8;

		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len + 0] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len + 1] = (crc16) & 0xFF;
		this->interface_->tx_len += 2;
	}

	void ModbusRtu::SlaveResponse0x02() {
		this->interface_->tx_len = 3;
		uint16_t crc16 = 0;

		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		this->interface_->tx_buf[2] = (this->params_.len_ % 8 > 0) ? this->params_.len_ / 8 + 1 : this->params_.len_ / 8; //число байт

		uint16_t out_state = *(this->device_parameters_.discrete_in_state);
		out_state = out_state >> (this->params_.addr_);
		out_state = out_state << (16 - this->params_.len_);
		out_state = out_state >> (16 - this->params_.len_);

		this->interface_->tx_buf[(this->interface_->tx_len)++] = out_state & 0x00FF;
		if (this->params_.len_ / 8 > 0)
			this->interface_->tx_buf[(this->interface_->tx_len)++] = out_state >> 8;

		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len + 0] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len + 1] = (crc16) & 0xFF;
		this->interface_->tx_len += 2;
	}

	void ModbusRtu::SlaveResponse0x05() {
		this->interface_->tx_len = 6;
		uint16_t crc16 = 0;

		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;

		this->interface_->tx_buf[2] = (this->params_.addr_ >> 8) & 0xFF;
		this->interface_->tx_buf[3] = (this->params_.addr_) & 0xFF;

		if(this->params_.val_ == 0x0000) *(this->device_parameters_.coils_state) &= ~(1 << this->params_.addr_) ;		//выключить
		else if(this->params_.val_ == 0xFF00) *(this->device_parameters_.coils_state) |= (1 << this->params_.addr_) ;	//включить

		this->interface_->tx_buf[4] = (this->params_.val_ >> 8) & 0xFF;
		this->interface_->tx_buf[5] = (this->params_.val_) & 0xFF;

		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len + 0] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len + 1] = (crc16) & 0xFF;
		this->interface_->tx_len += 2;
	}

	void ModbusRtu::SlaveResponse0x0F() {
		this->interface_->tx_len = 6;
		uint16_t crc16 = 0;

		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		this->interface_->tx_buf[2] = (this->params_.addr_ >> 8) & 0xFF;
		this->interface_->tx_buf[3] = (this->params_.addr_) & 0xFF;

		uint16_t k = 0;
		for(int i = this->params_.addr_; i < this->params_.addr_ + this->params_.len_; i++)
		{
			uint16_t temp_mask = (this->params_.val_ >> k++) & 1;
			if(temp_mask)
				*(this->device_parameters_.coils_state) |= (1 << i);
			else
				*(this->device_parameters_.coils_state) &= ~(1 << i);
		}

		this->interface_->tx_buf[4] = (this->params_.len_ >> 8) & 0xFF;
		this->interface_->tx_buf[5] = (this->params_.len_) & 0xFF;


		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len + 0] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len + 1] = (crc16) & 0xFF;
		this->interface_->tx_len += 2;
	}


	void ReadGlobalVar16b(uint32_t **p_var16, uint16_t addr, uint16_t len, uint16_t *p_user_arr) {
			//пусть начальный адрес равен 3, длина равна 4.
			//тогда вычисляем с какого двойного слова (с какого элемента массива указателей) нужно начать
			uint16_t first_world = addr/2;
			//далее шагаем до этого элемента, заводим указатель
			uint32_t *p;
			//в цикле в котором количество итераций равно first_world инкрементируем указатель
			for(int i = 0; i < first_world; i++)
				p = *p_var16++;
			p = *p_var16++;
			//далее вычисляем с какой части двойного слова начать чтение
			uint8_t part_world = addr % 2;
			//Если part_world == 1, начинаем со второй части слова, если 0 - с первой
			//заподим переменную счетчик, промежуточный буфер для хранения
			uint32_t temp;
			uint16_t temp1[2];
			uint16_t i = 0;
			//проверяем, если считывание начинается со второго слова
			if(part_world == 1) {
				temp = *p;
				temp1[0] = (temp >> 8) & 0x00FF;
				temp1[1] = (temp << 8) & 0xFF00;
				temp = temp1[0] | temp1[1];
				p_user_arr[i++] = (temp >> 16) & 0xFFFF;
				p = *p_var16++;
				if(i == len) goto FINISH; 	//тут подумать как заканчивать алгоритм
			}

			while(1) {

				temp = *p;
				temp1[0] = (temp >> 8) & 0x00FF;
				temp1[1] = (temp << 8) & 0xFF00;
				temp = temp1[0] | temp1[1];
				p_user_arr[i++] = temp & 0xFFFF;
				if(i == len) goto FINISH; 	//тут подумать как заканчивать алгоритм

				temp = *p;
				temp = (temp >> 16) & 0xFFFF;
				temp1[0] = (temp >> 8) & 0x00FF;
				temp1[1] = (temp << 8) & 0xFF00;
				temp = temp1[0] | temp1[1];
				p_user_arr[i++] = (temp) & 0xFFFF;
				if(i == len) goto FINISH; 	//тут подумать как заканчивать алгоритм

				p = *p_var16++;

			}

		FINISH:
			return;

	}

	//функция формирует ответ на запрос 0х03
	void ModbusRtu::SlaveResponse0x03() {

		uint16_t crc16 = 0;

		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		this->interface_->tx_buf[2] = (uint8_t)(this->params_.numReg_ * 2);

		ReadGlobalVar16b(this->device_parameters_.holding_registers, this->params_.addr_, this->params_.numReg_, (uint16_t*)&this->interface_->tx_buf[3]);
		this->interface_->tx_len = 3;
		this->interface_->tx_len += this->params_.numBytes_;

		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len + 0] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len + 1] = (crc16) & 0xFF;
		this->interface_->tx_len += 2;

	}



	void WriteGlobalVar16b(uint32_t **p_var16, uint16_t addr, uint16_t len, uint16_t *p_user_arr) {
			//пусть начальный адрес равен 3, длина равна 4.
			//тогда вычисляем с какого двойного слова (с какого элемента массива указателей) нужно начать
			uint16_t first_world = addr/2;
			//далее шагаем до этого элемента, заводим указатель
			uint32_t *p;
			//в цикле в котором количество итераций равно first_world инкрементируем указатель
			for(int i = 0; i < first_world; i++)
				p = *p_var16++;
			p = *p_var16++;
			//далее вычисляем с какой части двойного слова начать чтение
			uint8_t part_world = addr % 2;
			//Если part_world == 1, начинаем со второй части слова, если 0 - с первой
			//заподим переменную счетчик, промежуточный буфер для хранения
			uint16_t temp;
			uint16_t temp1[2];
			uint16_t i = 0;
			//проверяем, если запись начинается со второго слова
			if(part_world == 1) {
				temp = p_user_arr[i++];
				temp1[0] = (temp >> 8) & 0x00FF;
				temp1[1] = (temp << 8) & 0xFF00;
				temp = temp1[0] | temp1[1];
				*p = (*p & 0x0000FFFF) + (((uint32_t)temp << 16) & 0xFFFF0000);//(*p & 0xFFFF0000) + ((uint32_t)temp & 0x0000FFFF);
				p = *p_var16++;
				if(i == len) goto FINISH; 	//тут подумать как заканчивать алгоритм
			}

			while(1) {
				temp = p_user_arr[i++];
				temp1[0] = (temp >> 8) & 0x00FF;
				temp1[1] = (temp << 8) & 0xFF00;
				temp = temp1[0] | temp1[1];
				*p = (*p & 0xFFFF0000) + ((uint32_t)temp & 0x0000FFFF);
				if(i == len) goto FINISH; 	//тут подумать как заканчивать алгоритм

				temp = p_user_arr[i++];
				temp1[0] = (temp >> 8) & 0x00FF;
				temp1[1] = (temp << 8) & 0xFF00;
				temp = temp1[0] | temp1[1];
				*p = (*p & 0x0000FFFF) + (((uint32_t)temp << 16) & 0xFFFF0000);
				if(i == len) goto FINISH; 	//тут подумать как заканчивать алгоритм

				p = *p_var16++;

			}

		FINISH:
			return;

	}


	//функция формирует ответ на запрос 0х10
	void ModbusRtu::SlaveResponse0x10() {

		this->interface_->tx_len = 8;
		uint16_t crc16 = 0; //
		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		this->interface_->tx_buf[2] = (this->params_.addr_ >> 8) & 0xFF;
		this->interface_->tx_buf[3] = (this->params_.addr_) & 0xFF;


		WriteGlobalVar16b(this->device_parameters_.holding_registers, this->params_.addr_, this->params_.numReg_, (uint16_t*)&this->interface_->rx_buf[7]);

		this->interface_->tx_buf[4] = (this->params_.numReg_ >> 8) & 0xFF;
		this->interface_->tx_buf[5] = (this->params_.numReg_) & 0xFF;

		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len - 2] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len - 1] = (crc16) & 0xFF;
	}

	//функция формирует ответ на запрос 0х11
	void ModbusRtu::SlaveResponse0x11() {

		this->interface_->tx_len = 4;
		uint16_t crc16 = 0; //
		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		this->interface_->tx_buf[2] = this->device_parameters_.size_of_server_id;
		for(int i = 0; i < this->device_parameters_.size_of_server_id; i++) this->interface_->tx_buf[3 + i] = this->device_parameters_.server_id[i];
		this->interface_->tx_len += this->device_parameters_.size_of_server_id;

		if(this->state_ != kError)
			this->interface_->tx_buf[3 + this->device_parameters_.size_of_server_id] = 0xFF;
		else
			this->interface_->tx_buf[3 + this->device_parameters_.size_of_server_id] = 0x00;
		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len - 2] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len - 1] = (crc16) & 0xFF;
	}

	//данная функция формирует ответ "ошибка"
	void ModbusRtu::SlaveError() {

		uint16_t crc16;
		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_ | 0x80;
		this->interface_->tx_buf[2] = this->exception_code_;
		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, 3);
		this->interface_->tx_buf[3] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[4] = (crc16) & 0xFF;
		this->interface_->tx_len = 5;
	}


	void ModbusRtu::MasterUpdate() {

		//условие для проверки таймаута ожидания ответа от Slave'a
		if(this->state_ == kReseivingResp) {
			this->reseive_timeout_counter_++;
			if(this->reseive_timeout_counter_ >= this->callback_frequency_) {
				this->timer_->Stop();
				this->state_ = kZeroState;
				this->error_state_ = kSlaveResponseTimeoutError;
			}

		}

		switch (this->state_) {

		case kNotInit: 			//what are u doing here?
			return;
			break;
		case kZeroState:
			this->Reset();
			break;
		case kWaitData:
			return;
			break;
		case kNeedSendReq:		//нужно отправить запрос

			if(!(this->interface_->state_ == kBusy)) {
				this->interface_->state_ = kBusy;
				this->interface_->SendData();
			}

			break;
		case kReseivingResp:
			return;
			break;
		case kResResp:
		{
			bool crcMatch = CheckPackageCrc();
			if(crcMatch) {
				ResponseMasterWithCommend(this->params_.current_command_);
			} else {
				//m->state = kZeroState;	//вот тут надо подумать какое состояние устанавливать
				this->error_state_ = kCrcError;
			}
			this->state_ = kNeedReadResp;
			//парсим полученный ответ

			break;
		}
		case kNeedReadResp:
			return;
			break;
		default:
			return;//???

		}
	}

	void ModbusRtu::ResponseMasterWithCommend(Commands current_command) {

		switch (current_command) {
		case kReadCoild0x01:
			MasterCheckResponce0x01();
			break;
		case kReadDiscreteInputs0x02:
			MasterCheckResponce0x02();
			break;

		case kReadHoldingRegisters0x03:
			MasterCheckResponce0x03();
			break;
		case kWriteSingleCoil0x05:
			MasterCheckResponce0x05();
			break;
		case kWriteMultipleCoils0x0F:
			MasterCheckResponce0x0F();
			break;
		case kWriteMultipleRegisters0x10:
			MasterCheckResponce0x10();
			break;
		case kReportServerID0x11:
			MasterCheckResponce0x11();
			break;

		default: return;
		}
	}

	void ModbusRtu::RequestMasterWithCommend(Commands current_command) {


			switch (current_command) {
			case kReadCoild0x01:
				MasterSendRequest0x01();
				break;
			case kReadDiscreteInputs0x02:
				MasterSendRequest0x02();
				break;

			case kReadHoldingRegisters0x03:
				MasterSendRequest0x03();
				break;
			case kWriteSingleCoil0x05:
				MasterSendRequest0x05();
				break;
			case kWriteMultipleCoils0x0F:
				MasterSendRequest0x0F();
				break;
			case kWriteMultipleRegisters0x10:
				MasterSendRequest0x10();
				break;
			case kReportServerID0x11:
				MasterSendRequest0x11();
				break;

			default: return;
			}
		}


	void ModbusRtu::RxMasterCallback() {
		// если мы попали сюда в ненужный стейт или если пакет сломан, а мы все равно оказались по какой то причине в данной функции
		if((this->state_ != kReseivingResp) || (this->data_.state_ != kClearPackage) || ((this->state_ == kError))) return;
		this->timer_->Stop();

		//если количество принятых байт равно максимальному значению
		if(this->interface_->rx_len >= MAX_NUM_PACK) {
			//устанавливаем флаг переполнения
			this->data_.state_ = kOverflow;
			this->interface_->rx_len = 0;
			//отключаем прием, чтобы обновить драйвер
		} else {

			//Устанавливаем период 3.5t
			this->CalculateTimings35t(this->interface_->GetCurrentBaudrate());
			this->timer_->SetCallBackWithArg(Callback35tWrapper, (void*)this);
			this->timer_->Start();
		}
	}


	//===============================================================
	// ФУнкции ниже определяют работу с запросами и ответами мастера
	//===============================================================
	void ModbusRtu::MasterSendRequest0x01() {
		uint16_t crc16;

		this->interface_->tx_len = 6;
		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		this->interface_->tx_buf[2] = (this->params_.addr_ >> 8) & 0xFF;
		this->interface_->tx_buf[3] = (this->params_.addr_ >> 0) & 0xFF;
		this->interface_->tx_buf[4] = (this->params_.numReg_ >> 8) & 0xFF;
		this->interface_->tx_buf[5] = (this->params_.numReg_ >> 0) & 0xFF;


		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);

		this->interface_->tx_buf[this->interface_->tx_len + 0] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len + 1] = (crc16) & 0xFF;
		this->interface_->tx_len += 2;
	}
	void ModbusRtu::MasterSendRequest0x02() {
		uint16_t crc16;
		this->interface_->tx_len = 6;
		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		this->interface_->tx_buf[2] = (this->params_.addr_ >> 8) & 0xFF;
		this->interface_->tx_buf[3] = (this->params_.addr_ >> 0) & 0xFF;
		this->interface_->tx_buf[4] = (this->params_.numReg_ >> 8) & 0xFF;
		this->interface_->tx_buf[5] = (this->params_.numReg_ >> 0) & 0xFF;
		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len + 0] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len + 1] = (crc16) & 0xFF;
		this->interface_->tx_len += 2;
	}
	void ModbusRtu::MasterSendRequest0x03() {
		uint16_t crc16;
		this->interface_->tx_len = 6;
		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		this->interface_->tx_buf[2] = (this->params_.addr_ >> 8) & 0xFF;
		this->interface_->tx_buf[3] = (this->params_.addr_ >> 0) & 0xFF;
		this->interface_->tx_buf[4] = (this->params_.numReg_ >> 8) & 0xFF;
		this->interface_->tx_buf[5] = (this->params_.numReg_ >> 0) & 0xFF;
		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len + 0] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len + 1] = (crc16) & 0xFF;
		this->interface_->tx_len += 2;
	}
	void ModbusRtu::MasterSendRequest0x05() {
		uint16_t crc16;
		this->interface_->tx_len = 6;
		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		this->interface_->tx_buf[2] = (this->params_.addr_ >> 8) & 0xFF;
		this->interface_->tx_buf[3] = (this->params_.addr_ >> 0) & 0xFF;
		this->interface_->tx_buf[4] = (this->params_.val_ >> 8) & 0xFF;
		this->interface_->tx_buf[5] = (this->params_.val_ >> 0) & 0xFF;
		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len + 0] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len + 1] = (crc16) & 0xFF;
		this->interface_->tx_len += 2;
	}

	void ModbusRtu::MasterSendRequest0x0F() {
		uint16_t crc16;
		this->interface_->tx_len = 7;
		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		this->interface_->tx_buf[2] = (this->params_.addr_ >> 8) & 0xFF;
		this->interface_->tx_buf[3] = (this->params_.addr_ >> 0) & 0xFF;
		this->interface_->tx_buf[4] = (this->params_.numReg_ >> 8) & 0xFF;
		this->interface_->tx_buf[5] = (this->params_.numReg_ >> 0) & 0xFF;
		this->interface_->tx_buf[6] = (this->params_.numReg_ % 8 == 0) ? this->params_.numReg_ / 8 : this->params_.numReg_ / 8 + 1;

		uint8_t* temp_coil_val = (uint8_t*) this->params_.buf_for_rw_;

		for(int i = 0; i < this->interface_->tx_buf[6]; i++) {
			this->interface_->tx_buf[(this->interface_->tx_len)++] = temp_coil_val[i];
		}

		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len + 0] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len + 1] = (crc16) & 0xFF;
		this->interface_->tx_len += 2;
	}

	void ModbusRtu::MasterSendRequest0x10() {
		uint16_t crc16;
		this->interface_->tx_len = 7;
		this->params_.numBytes_ = this->params_.numReg_ * 2;
		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		this->interface_->tx_buf[2] = (this->params_.addr_ >> 8) & 0xFF;
		this->interface_->tx_buf[3] = (this->params_.addr_ >> 0) & 0xFF;
		this->interface_->tx_buf[4] = (this->params_.numReg_ >> 8) & 0xFF;
		this->interface_->tx_buf[5] = (this->params_.numReg_ >> 0) & 0xFF;
		this->interface_->tx_buf[6] = (this->params_.numBytes_) & 0xFF;

		uint16_t* buff_for_write = (uint16_t*) this->params_.buf_for_rw_;
		for(int i = 0; i < this->params_.numBytes_ / 2; i++) {
			this->interface_->tx_buf[(this->interface_->tx_len)++] = (buff_for_write[i] >> 8) & 0x00FF;
			this->interface_->tx_buf[(this->interface_->tx_len)++] = (buff_for_write[i]) & 0x00FF;
		}

		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len + 0] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len + 1] = (crc16) & 0xFF;
		this->interface_->tx_len += 2;
	}

	void ModbusRtu::MasterSendRequest0x11() {
		uint16_t crc16;
		this->interface_->tx_len = 2;
		this->interface_->tx_buf[0] = this->params_.id_;
		this->interface_->tx_buf[1] = this->params_.command_;
		crc16 = Crc::CrcCalculator::getInstance()->CrcCalc(this->interface_->tx_buf, this->interface_->tx_len);
		this->interface_->tx_buf[this->interface_->tx_len] = (crc16 >> 8) & 0xFF;
		this->interface_->tx_buf[this->interface_->tx_len + 1] = (crc16) & 0xFF;
		this->interface_->tx_len += 2;
	}









	void ModbusRtu::MasterCheckResponce0x01() {

		this->error_state_ = kNotError;
		if((this->params_.command_ | 0x80) == this->interface_->rx_buf[1]) {

			this->exception_code_ = (ExceptionCode)this->interface_->rx_buf[2];
			if(this->exception_code_ == kExceptionCode1)
				this->error_state_ = kNotSupportedFunctionCode;
			else if(this->exception_code_ == kExceptionCode2)
				this->error_state_ = kNotAvaibleAddrCode;
			else if(this->exception_code_ == kExceptionCode3)
				this->error_state_ = kDataValueError;
			else if(this->exception_code_ == kExceptionCode4)
				this->error_state_ = kCommandExecutionError;

			return;
		}

		if((this->params_.id_ != this->interface_->rx_buf[0]) || (this->params_.command_ != this->interface_->rx_buf[1])) {
			this->error_state_ = kSlaveResponseError;
			return;
		}
	}

	void ModbusRtu::MasterCheckResponce0x02() {

		this->error_state_ = kNotError;
		if((this->params_.command_ | 0x80) == this->interface_->rx_buf[1]) {

			this->exception_code_ = (ExceptionCode)this->interface_->rx_buf[2];
			if(this->exception_code_ == kExceptionCode1)
				this->error_state_ = kNotSupportedFunctionCode;
			else if(this->exception_code_ == kExceptionCode2)
				this->error_state_ = kNotAvaibleAddrCode;
			else if(this->exception_code_ == kExceptionCode3)
				this->error_state_ = kDataValueError;
			else if(this->exception_code_ == kExceptionCode4)
				this->error_state_ = kCommandExecutionError;


			return;
		}



		if((this->params_.id_ != this->interface_->rx_buf[0]) || (this->params_.command_ != this->interface_->rx_buf[1])) {
			this->error_state_ = kSlaveResponseError;
			return;
		}

	}

	void ModbusRtu::MasterCheckResponce0x03() {
		this->error_state_ = kNotError;
		if((this->params_.command_ | 0x80) == this->interface_->rx_buf[1]) {

			this->exception_code_ = (ExceptionCode)this->interface_->rx_buf[2];
			if(this->exception_code_ == kExceptionCode1)
				this->error_state_ = kNotSupportedFunctionCode;
			else if(this->exception_code_ == kExceptionCode2)
				this->error_state_ = kNotAvaibleAddrCode;
			else if(this->exception_code_ == kExceptionCode3)
				this->error_state_ = kDataValueError;
			else if(this->exception_code_ == kExceptionCode4)
				this->error_state_ = kCommandExecutionError;


			return;
		}
		if((this->params_.id_ != this->interface_->rx_buf[0]) || (this->params_.command_ != this->interface_->rx_buf[1])) {
			this->error_state_ = kSlaveResponseError;
			return;
		}

	}

	void ModbusRtu::MasterCheckResponce0x05() {
		this->error_state_ = kNotError;
		if((this->params_.command_ | 0x80) == this->interface_->rx_buf[1]) {
			this->exception_code_ = (ExceptionCode)this->interface_->rx_buf[2];
			if(this->exception_code_ == kExceptionCode1)
				this->error_state_ = kNotSupportedFunctionCode;
			else if(this->exception_code_ == kExceptionCode2)
				this->error_state_ = kNotAvaibleAddrCode;
			else if(this->exception_code_ == kExceptionCode3)
				this->error_state_ = kDataValueError;
			else if(this->exception_code_ == kExceptionCode4)
				this->error_state_ = kCommandExecutionError;


			return;
		}
		uint8_t temp_id = this->interface_->rx_buf[0];
		uint8_t temp_com = this->interface_->rx_buf[1];
		uint16_t temp_adr = ((uint16_t)this->interface_->rx_buf[2] << 8) | this->interface_->rx_buf[3];
		uint16_t temp_val = ((uint16_t)this->interface_->rx_buf[4] << 8) | this->interface_->rx_buf[5];




		if((this->params_.id_ != temp_id) || (this->params_.command_ != temp_com) || (this->params_.addr_ != temp_adr) || (this->params_.val_ != temp_val)) {
			this->error_state_ = kSlaveResponseError;
			return;
		}
	}

	void ModbusRtu::MasterCheckResponce0x0F() {

		this->error_state_ = kNotError;
		if((this->params_.command_ | 0x80) == this->interface_->rx_buf[1]) {

			this->exception_code_ = (ExceptionCode)this->interface_->rx_buf[2];
			if(this->exception_code_ == kExceptionCode1)
				this->error_state_ = kNotSupportedFunctionCode;
			else if(this->exception_code_ == kExceptionCode2)
				this->error_state_ = kNotAvaibleAddrCode;
			else if(this->exception_code_ == kExceptionCode3)
				this->error_state_ = kDataValueError;
			else if(this->exception_code_ == kExceptionCode4)
				this->error_state_ = kCommandExecutionError;

			return;
		}
		uint8_t temp_id = this->interface_->rx_buf[0];
		uint8_t temp_com = this->interface_->rx_buf[1];
		uint16_t temp_adr = ((uint16_t)this->interface_->rx_buf[2] << 8) | this->interface_->rx_buf[3];
		uint16_t temp_len = ((uint16_t)this->interface_->rx_buf[4] << 8) | this->interface_->rx_buf[5];

		if((this->params_.id_ != temp_id) || (this->params_.command_ != temp_com) || (this->params_.addr_ != temp_adr) || (this->params_.numReg_ != temp_len)) {
			this->error_state_ = kSlaveResponseError;
			return;
		}

	}

	void ModbusRtu::MasterCheckResponce0x10() {

		this->error_state_ = kNotError;
		if((this->params_.command_ | 0x80) == this->interface_->rx_buf[1]) {

			this->exception_code_ = (ExceptionCode)this->interface_->rx_buf[2];
			if(this->exception_code_ == kExceptionCode1)
				this->error_state_ = kNotSupportedFunctionCode;
			else if(this->exception_code_ == kExceptionCode2)
				this->error_state_ = kNotAvaibleAddrCode;
			else if(this->exception_code_ == kExceptionCode3)
				this->error_state_ = kDataValueError;
			else if(this->exception_code_ == kExceptionCode4)
				this->error_state_ = kCommandExecutionError;

			return;
		}
		uint8_t temp_id = this->interface_->rx_buf[0];
		uint8_t temp_com = this->interface_->rx_buf[1];
		uint16_t temp_adr = ((uint16_t)this->interface_->rx_buf[2] << 8) | this->interface_->rx_buf[3];
		uint16_t temp_len = ((uint16_t)this->interface_->rx_buf[4] << 8) | this->interface_->rx_buf[5];

		if((this->params_.id_ != temp_id) || (this->params_.command_ != temp_com) || (this->params_.addr_ != temp_adr) || (this->params_.numReg_ != temp_len)) {
			this->error_state_ = kSlaveResponseError;
			return;
		}
	}

	void ModbusRtu::MasterCheckResponce0x11() {

		this->error_state_ = kNotError;
		if((this->params_.command_ | 0x80) == this->interface_->rx_buf[1]) {

			this->exception_code_ = (ExceptionCode)this->interface_->rx_buf[2];
			if(this->exception_code_ == kExceptionCode1)
				this->error_state_ = kNotSupportedFunctionCode;
			else if(this->exception_code_ == kExceptionCode4)
				this->error_state_ = kDataValueError;

			return;
		}

		uint8_t temp_id = this->interface_->rx_buf[0];
		uint8_t temp_com = this->interface_->rx_buf[1];

		if((this->params_.id_ != temp_id) || (this->params_.command_ != temp_com)) {
			this->error_state_ = kSlaveResponseError;
			return;
		}

	}



	State ModbusRtu::CalculateTimings15t(uint16_t baudrate) {
		State state = kNotInit;
		if(baudrate > 19200) {

			// 750 us + 10 bit
			float timer_period = 750. / 1000000. + 10. / baudrate;
			float timer_freq = 1. / timer_period;
			this->timer_->CalculateNewTimings((uint32_t)timer_freq);
			if (this->timer_->GetTimingState() == kTimingsError) state = kError;

		} else {

			uint32_t timer_freq = baudrate / 25;
			this->timer_->CalculateNewTimings(timer_freq);
			if (this->timer_->GetTimingState() == kTimingsError) state = kError;
		}

		return state;
	}

	State ModbusRtu::CalculateTimings35t(uint16_t baudrate) {
		State state = kNotInit;
		if(baudrate > 19200) {
			// 1750 us + 10 bit
			float timer_period = 1750. / 1000000. + 10. / baudrate;
			float timer_freq = 1. / timer_period;
			this->timer_->CalculateNewTimings((uint32_t)timer_freq);
			if (this->timer_->GetTimingState() == kTimingsError) state = kError;

		} else {

			uint32_t timer_freq = baudrate / 45;
			this->timer_->CalculateNewTimings(timer_freq);
			if (this->timer_->GetTimingState() == kTimingsError) state = kError;
		}

		return state;
	}

	State ModbusRtu::CalculateTimings20t(uint16_t baudrate) {
		State state = kNotInit;
		if(baudrate > 19200) {

			// 1000 us + 10 bit
			float timer_period = 1000. / 1000000. + 10. / baudrate;
			float timer_freq = 1. / timer_period;
			this->timer_->CalculateNewTimings((uint32_t)timer_freq);
			if (this->timer_->GetTimingState() == kTimingsError) state = kError;

		} else {
			uint32_t timer_freq = baudrate / 20;
			this->timer_->CalculateNewTimings(timer_freq);
			if (this->timer_->GetTimingState() == kTimingsError) state = kError;
		}

		return state;
	}
}

