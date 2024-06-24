/**
 * This file is part of Patternshop Project.
 * 
 * Patternshop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Patternshop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Patternshop.  If not, see <http://www.gnu.org/licenses/>
*/

#include "PsMacCursor.h"
#include "PsController.h"
#include "PsMacView.h"

static Cursor DEFAULT, MOVE, ROTATE1, ROTATE2, ROTATE3, ROTATE4, 
	SCROLL, SIZE1, SIZE2, HAND, HAND2, MAGNIFY, MAGNIFY2, 
	MAGNIFY3, FINGER;

Bits16 DEFAULT_image = {0x0000,0x0000,0x4000,0x6000,0x7000,0x7800,0x7C00,0x7E00,0x7F00,0x7000,0x6000,0x4000,0x0000,0x0000,0x0000,0x0000};
Bits16 DEFAULT_mask = {0x8000,0xC000,0xE000,0xF000,0xF800,0xFC00,0xFE00,0xFF00,0xFF80,0xFFC0,0xF000,0xE000,0xC000,0x8000,0x0000,0x0000};

Bits16 MOVE_image = {0x0000,0x0000,0x4000,0x6000,0x7000,0x7800,0x7C00,0x7E20,0x7F70,0x7020,0x6124,0x43FE,0x0124,0x0020,0x0070,0x0020};
Bits16 MOVE_mask = {0x8000,0xC000,0xE000,0xF000,0xF800,0xFC00,0xFE20,0xFF70,0xFFF8,0xFFF4,0xF3FE,0xE7FF,0xC3FE,0x0174,0x00F8,0x0070};

Bits16 ROTATE1_image = {0x0600,0x0900,0x1580,0x2C60,0x5F90,0x2C68,0x15A8,0x0954,0x06D6,0x0111,0x017D,0x00BA,0x0054,0x0028,0x0010,0x0000};
Bits16 ROTATE1_mask = {0x0600,0x0F00,0x1F80,0x3FE0,0x7FF0,0x3FF8,0x1FB8,0x0F7C,0x06FE,0x01FF,0x01FF,0x00FE,0x007C,0x0038,0x0010,0x0000};

Bits16 ROTATE2_image = {0x0000,0x0010,0x0028,0x0054,0x00BA,0x017D,0x0111,0x06D6,0x0954,0x15A8,0x2C68,0x5F90,0x2C60,0x1580,0x0900,0x0600};
Bits16 ROTATE2_mask = {0x0000,0x0010,0x0038,0x007C,0x00FE,0x01FF,0x01FF,0x06FE,0x0F7C,0x1FB8,0x3FF8,0x7FF0,0x3FE0,0x1F80,0x0F00,0x0600};

Bits16 ROTATE3_image = {0x0000,0x0800,0x1400,0x2A00,0x5D00,0xBE80,0x8880,0x6B60,0x2A90,0x15A8,0x1634,0x09FA,0x0634,0x01A8,0x0090,0x0060};
Bits16 ROTATE3_mask = {0x0000,0x0800,0x1C00,0x3E00,0x7F00,0xFF80,0xFF80,0x7F60,0x3EF0,0x1DF8,0x1FFC,0x0FFE,0x07FC,0x01F8,0x00F0,0x0060};

Bits16 ROTATE4_image = {0x0060,0x0090,0x01A8,0x0634,0x09FA,0x1634,0x15A8,0x2A90,0x6B60,0x8880,0xBE80,0x5D00,0x2A00,0x1400,0x0800,0x0000};
Bits16 ROTATE4_mask = {0x0060,0x00F0,0x01F8,0x07FC,0x0FFE,0x1FFC,0x1DF8,0x3EF0,0x7F60,0xFF80,0xFF80,0x7F00,0x3E00,0x1C00,0x0800,0x0000};

Bits16 SCROLL_image = {0x7FFF,0x3FFF,0x5FFF,0x6FFF,0x77FF,0x7BFF,0x7DDF,0x7EAF,0x7F77,0x702B,0x6D25,0x5BFE,0x3D25,0xFEAB,0xFF77,0xFFAF};
Bits16 SCROLL_mask = {0x8000,0xC000,0xE000,0xF000,0xF800,0xFC00,0xFE20,0xFF70,0xFFF8,0xFFF4,0xF3FE,0xE7FF,0xC3FE,0x0174,0x00F8,0x0070};

Bits16 SIZE1_image = {0x0000,0x001E,0x000E,0x000E,0x0012,0x0020,0x0140,0x0080,0x0140,0x0200,0x2400,0x3800,0x3800,0x3C00,0x0000,0x0000};
Bits16 SIZE1_mask = {0x003F,0x003F,0x001F,0x001F,0x003F,0x0173,0x03E0,0x01C0,0x03E0,0x6740,0x7E00,0x7C00,0x7C00,0x7E00,0x7E00,0x0000};

Bits16 SIZE2_image = {0x0000,0x7800,0x7000,0x7000,0x4800,0x0400,0x0280,0x0100,0x0280,0x0040,0x0024,0x001C,0x001C,0x003C,0x0000,0x0000};
Bits16 SIZE2_mask = {0xFC00,0xFC00,0xF800,0xF800,0xFC00,0xCE80,0x07C0,0x0380,0x07C0,0x02E6,0x007E,0x003E,0x003E,0x007E,0x007E,0x0000};

Bits16 HAND_image = {0x0180,0x1A70,0x2648,0x264A,0x124D,0x1249,0x6809,0x9801,0x8802,0x4002,0x2002,0x2004,0x1004,0x0808,0x0408,0x0408};
Bits16 HAND_mask = {0x0180,0x1BF0,0x3FF8,0x3FFA,0x1FFF,0x1FFF,0x6FFF,0xFFFF,0xFFFE,0x7FFE,0x3FFE,0x3FFC,0x1FFC,0x0FF8,0x07F8,0x07F8};

Bits16 HAND2_image = {
	0x0000,
	0x0000,
	0x1B60,
	0x2498,
	0x2014,
	0x1004,
	0x3004,
	0x4004,
	0x4004,
	0x4008,
	0x2008,
	0x1010,
	0x0810,
	0x0810,
	0x0000,
	0x0000
};

Bits16 HAND2_mask = {
	0x0000,
	0x0000,
	0x1B60,
	0x3FF8,
	0x3FFC,
	0x1FFC,
	0x3FFC,
	0x7FFC,
	0x7FFC,
	0x7FF8,
	0x3FF8,
	0x1FF0,
	0x0FF0,
	0x0FF0,
	0x0000,
	0x0000,
};

Bits16 MAGNIFY_image = {
	0xF0FF,
	0xCF3F,
	0xBFDF,
	0xBFDF,
	0x7FEF,
	0x7FEF,
	0x7FEF,
	0x7FEF,
	0xBFDF,
	0xBFDF,
	0xCF0F,
	0xF0C7,
	0xFFE3,
	0xFFF1,
	0xFFF8,
	0xFFFD
};

Bits16 MAGNIFY_mask = {
	0xF0FF,
	0xC03F,
	0x801F,
	0x801F,
	0x000F,
	0x000F,
	0x000F,
	0x000F,
	0x801F,
	0x801F,
	0xC00F,
	0xF0C7,
	0xFFE3,
	0xFFF1,
	0xFFF8,
	0xFFFD
};

/*
Bits16 MAGNIFY_mask = {
	1111000011111111
	1100000000111111
	1000000000011111
	1000000000011111
	0000000000001111
	0000000000001111
	0000000000001111
	0000000000001111
	1000000000011111
	1000000000011111
	1100000000001111
	1111000011000111
	1111111111100011
	1111111111110001
	1111111111111000
	1111111111111101
};
*/

Bits16 MAGNIFY2_image = {
	0x0F00,
	0x30C0,
	0x4020,
	0x4020,
	0x8010,
	0x9F90,
	0x9F90,
	0x8010,
	0x4020,
	0x4020,
	0x30F0,
	0x0F38,
	0x001C,
	0x000E,
	0x0007,
	0x0002
};

Bits16 MAGNIFY3_image = {
	0x0F00,
	0x30C0,
	0x4020,
	0x4620,
	0x8610,
	0x9F90,
	0x9F90,
	0x8610,
	0x4620,
	0x4020,
	0x30F0,
	0x0F38,
	0x001C,
	0x000E,
	0x0007,
	0x0002
};

Bits16 FINGER_image = {
	0x0300,
	0x0480,
	0x0480,
	0x0480,
	0x04E0,
	0x04B8,
	0x64AE,
	0x94AA,
	0x8C0A,
	0x4402,
	0x2402,
	0x2002,
	0x1004,
	0x0804,
	0x0408,
	0x03F0
};

Bits16 FINGER_mask = {
	0x300,
	0x780,
	0x780,
	0x780,
	0x7E0,
	0x7F8,
	0x67FE,
	0xF7FE,
	0xFFFE,
	0x7FFE,
	0x3FFE,
	0x3FFE,
	0x1FFC,
	0xFFC,
	0x7F8,
	0x3F0
};

// gestion de big ou little endian
static void BigEndianBits16Copy(Bits16 &to, Bits16 &from)
{
	for (short i = 0; i < 16; ++i)
		to[i] = EndianS16_BtoN(from[i]);
}

void InitCustomCursors()
{
	static bool load = false;
	
	if (!load) 
	{
		//DEFAULT
		BigEndianBits16Copy(DEFAULT.data, DEFAULT_image);
		BigEndianBits16Copy(DEFAULT.mask, DEFAULT_mask);
		DEFAULT.hotSpot.v = 0;
		DEFAULT.hotSpot.h = 0;
		
		//MOVE
		BigEndianBits16Copy(MOVE.data, MOVE_image);
		BigEndianBits16Copy(MOVE.mask, MOVE_mask);
		MOVE.hotSpot.v = 0;
		MOVE.hotSpot.h = 0;

		//SCROLL
		BigEndianBits16Copy(SCROLL.data, SCROLL_image);
		BigEndianBits16Copy(SCROLL.mask, SCROLL_mask);
		SCROLL.hotSpot.v = 0;
		SCROLL.hotSpot.h = 0;
		
		//ROTATE1
		BigEndianBits16Copy(ROTATE1.data, ROTATE1_image);
		BigEndianBits16Copy(ROTATE1.mask, ROTATE1_mask);
		ROTATE1.hotSpot.v = 7;
		ROTATE1.hotSpot.h = 7;
		
		//ROTATE2
		BigEndianBits16Copy(ROTATE2.data, ROTATE2_image);
		BigEndianBits16Copy(ROTATE2.mask, ROTATE2_mask);
		ROTATE2.hotSpot.v = 7;
		ROTATE2.hotSpot.h = 7;
		
		//ROTATE3
		BigEndianBits16Copy(ROTATE3.data, ROTATE3_image);
		BigEndianBits16Copy(ROTATE3.mask, ROTATE3_mask);
		ROTATE3.hotSpot.v = 7;
		ROTATE3.hotSpot.h = 7;
		
		//ROTATE4
		BigEndianBits16Copy(ROTATE4.data, ROTATE4_image);
		BigEndianBits16Copy(ROTATE4.mask, ROTATE4_mask);
		ROTATE4.hotSpot.v = 7;
		ROTATE4.hotSpot.h = 7;
		
		//SIZE1
		BigEndianBits16Copy(SIZE1.data, SIZE1_image);
		BigEndianBits16Copy(SIZE1.mask, SIZE1_mask);
		SIZE1.hotSpot.v = 7;
		SIZE1.hotSpot.h = 7;
		
		//SIZE2
		BigEndianBits16Copy(SIZE2.data, SIZE2_image);
		BigEndianBits16Copy(SIZE2.mask, SIZE2_mask);
		SIZE2.hotSpot.v = 7;
		SIZE2.hotSpot.h = 7;
		
		//HAND
		BigEndianBits16Copy(HAND.data, HAND_image);
		BigEndianBits16Copy(HAND.mask, HAND_mask);
		HAND.hotSpot.v = 7;
		HAND.hotSpot.h = 7;
		
		//HAND2
		BigEndianBits16Copy(HAND2.data, HAND2_image);
		BigEndianBits16Copy(HAND2.mask, HAND2_mask);
		HAND2.hotSpot.v = 7;
		HAND2.hotSpot.h = 7;
		
		//MAGNIFY
		for (int i = 0; i < 16; ++i) MAGNIFY.data[i] = EndianS16_BtoN(0xFFFF - MAGNIFY_image[i]);
		for (int i = 0; i < 16; ++i) MAGNIFY.mask[i] = EndianS16_BtoN(0xFFFF - MAGNIFY_mask[i]);
		MAGNIFY.hotSpot.v = 7;
		MAGNIFY.hotSpot.h = 7;
		
		//MAGNIFY2
		BigEndianBits16Copy(MAGNIFY2.data, MAGNIFY2_image);
		for (int i = 0; i < 16; ++i) MAGNIFY2.mask[i] = EndianS16_BtoN(0xFFFF - MAGNIFY_mask[i]);
		MAGNIFY2.hotSpot.v = 7;
		MAGNIFY2.hotSpot.h = 7;

		//MAGNIFY3
		BigEndianBits16Copy(MAGNIFY3.data, MAGNIFY3_image);
		for (int i = 0; i < 16; ++i) MAGNIFY3.mask[i] = EndianS16_BtoN(0xFFFF - MAGNIFY_mask[i]);
		MAGNIFY3.hotSpot.v = 7;
		MAGNIFY3.hotSpot.h = 7;

		//FINGER
		BigEndianBits16Copy(FINGER.data, FINGER_image);
		BigEndianBits16Copy(FINGER.mask, FINGER_mask);
		FINGER.hotSpot.v = 0;
		FINGER.hotSpot.h = 7;
		
		load = true;
	}
}

void SetMacCursor(PsCursor num)
{
	InitCustomCursors();
	if (active)
	{
		active->iCurrentCursor = num;
		if (active->bTracking)
		{
			switch(num) 
			{
				case CURSOR_DEFAULT: SetCursor(&DEFAULT); break;
				case CURSOR_MOVE: SetCursor(&MOVE); break;
				case CURSOR_ROTATE1: SetCursor(&ROTATE1); break;
				case CURSOR_ROTATE2: SetCursor(&ROTATE2); break;
				case CURSOR_ROTATE3: SetCursor(&ROTATE3); break;
				case CURSOR_ROTATE4: SetCursor(&ROTATE4); break;
				case CURSOR_SCROLL1: SetCursor(&HAND); break;
				case CURSOR_SCROLL2: SetCursor(&HAND2); break;
				case CURSOR_SIZE1: SetCursor(&SIZE1); break;
				case CURSOR_SIZE2: SetCursor(&SIZE2); break;
				case CURSOR_TORSIO1: SetCursor(&HAND); break;
				case CURSOR_TORSIO2: SetCursor(&HAND2); break;
				case CURSOR_MAGNIFY1: SetCursor(&MAGNIFY); break;
				case CURSOR_MAGNIFY2: SetCursor(&MAGNIFY2); break;
				case CURSOR_MAGNIFY3: SetCursor(&MAGNIFY3); break;
				default: break;
			}
		}
	}
	
	switch(num) 
	{
		case CURSOR_SCROLL2: SetCursor(&HAND2); break;
		case CURSOR_FINGER: SetCursor(&FINGER); break;
		default: break;
	}

}

