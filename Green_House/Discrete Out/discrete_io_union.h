/*
 * dicrete_io_union.h
 *
 *  Created on: 22 мар. 2023 г.
 *      Author: akcel
 */

#ifndef DISCRETE_OUT_DISCRETE_IO_UNION_H_
#define DISCRETE_OUT_DISCRETE_IO_UNION_H_

#include <stdint.h>

namespace DiscreteIO {
	union DiscreteIoUnion {

		uint32_t all_value;
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
			uint32_t bit16:1;
			uint32_t bit17:1;
			uint32_t bit18:1;
			uint32_t bit19:1;
			uint32_t bit20:1;
			uint32_t bit21:1;
			uint32_t bit22:1;
			uint32_t bit23:1;
			uint32_t bit24:1;
			uint32_t bit25:1;
			uint32_t bit26:1;
			uint32_t bit27:1;
			uint32_t bit28:1;
			uint32_t bit29:1;
			uint32_t bit30:1;
			uint32_t bit31:1;
		} bit;
	};
}




#endif /* DISCRETE_OUT_DISCRETE_IO_UNION_H_ */
