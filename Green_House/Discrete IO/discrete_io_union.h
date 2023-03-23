/*
 * dicrete_io_union.h
 *
 *  Created on: 22 мар. 2023 г.
 *      Author: akcel
 */

#ifndef DISCRETE_IO_DISCRETE_IO_UNION_H_
#define DISCRETE_IO_DISCRETE_IO_UNION_H_

#include <stdint.h>

namespace DiscreteIO {
	union DiscreteIoUnion {

		uint16_t all_value;
		struct {
			uint32_t bit0:1;
			uint32_t bit1:1;
			uint32_t bit2:1;
			uint32_t bit3:1;
			uint32_t bit4:1;
			uint32_t bit5:1;
			uint32_t bit6:1;
			uint32_t bit7:1;
			uint32_t bit8:1;
			uint32_t bit9:1;
			uint32_t bit10:1;
			uint32_t bit11:1;
			uint32_t bit12:1;
			uint32_t bit13:1;
			uint32_t bit14:1;
			uint32_t bit15:1;
		} bit;
	};
}




#endif /* DISCRETE_IO_DISCRETE_IO_UNION_H_ */
