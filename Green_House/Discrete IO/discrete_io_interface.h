/*
 * discrete_io.h
 *
 *  Created on: 23 мар. 2023 г.
 *      Author: akcel
 */

#ifndef DISCRETE_IO_DISCRETE_IO_INTERFACE_H_
#define DISCRETE_IO_DISCRETE_IO_INTERFACE_H_

#include "discrete_io_union.h"

namespace DiscreteIO {

	class DiscreteIOInterface {
	protected:
		DiscreteIoUnion state_;
	public:
		virtual uint8_t GetNumOfDiscreteIO() = 0;
		virtual void Init() = 0;
		virtual void Update() = 0;

		uint16_t* GetDiscreteOutStateLink() {return &(this->state_.all_value);}


	};

}



#endif /* DISCRETE_IO_DISCRETE_IO_INTERFACE_H_ */
