#ifdef __FPU_EMU__

#ifndef __FPU_TRIG_VALUE__
#define __FPU_TRIG_VALUE__

#include <stddef.h>
#include <fpu_type.h>

	namespace fpu_trig_value {
	    //valori per i seni
	    static const long double VSIN0  = (0xBFFF00000000LL << 48) + 0x000000000000LL;
	    static const long double VSIN1  = (0xBFFF3FFD8483LL << 48) + 0xEE0C98347F4DLL;
	    static const long double VSIN2  = (0xBFFF3FFE8000LL << 48) + 0x000000000000LL;
	    static const long double VSIN3  = (0xBFFF3FFEB504LL << 48) + 0xF333F9DE62F5LL;
	    static const long double VSIN4  = (0xBFFF3FFEDDB3LL << 48) + 0xD742C2654F7ALL;
	    static const long double VSIN5  = (0xBFFF3FFEF746LL << 48) + 0xEA3A45F8A698LL;
	    static const long double VSIN6  = (0xBFFF3FFF8000LL << 48) + 0x000000000000LL;
	    static const long double VSIN7  = (0xBFFF3FFEF746LL << 48) + 0xEA3A45F8A8E1LL;
	    static const long double VSIN8  = (0xBFFF3FFEDDB3LL << 48) + 0xD742C2655BE4LL;
	    static const long double VSIN9  = (0xBFFF3FFEB504LL << 48) + 0xF333F9DE6932LL;
	    static const long double VSIN10 = (0xBFFF3FFE8000LL << 48) + 0x000000000000LL;
	    static const long double VSIN11 = (0xBFFF3FFD8483LL << 48) + 0xEE0C9834A787LL;
        static const long double VSIN12 = (0xBFFF00000000LL << 48) + 0x000000000000LL;
	    static const long double VSIN13 = (0xBFFFBFFD8483LL << 48) + 0xEE0C9834856FLL;
	    static const long double VSIN14 = (0xBFFFBFFE8000LL << 48) + 0x000000000000LL;
	    static const long double VSIN15 = (0xBFFFBFFEB504LL << 48) + 0xF333F9DE5C8BLL;
	    static const long double VSIN16 = (0xBFFFBFFEDDB3LL << 48) + 0xD742C2654311LL;
	    static const long double VSIN17 = (0xBFFFBFFEF746LL << 48) + 0xEA3A45F8A873LL;
	    static const long double VSIN18 = (0xBFFFBFFF8000LL << 48) + 0x000000000000LL;
	    static const long double VSIN19 = (0xBFFFBFFEF746LL << 48) + 0xEA3A45F8AF4DLL;
	    static const long double VSIN20 = (0xBFFFBFFEF746LL << 48) + 0xEA3A45F8AF4DLL;
	    static const long double VSIN21 = (0xBFFFBFFEB504LL << 48) + 0xF333F9DE6F70LL;
	    static const long double VSIN22 = (0xBFFFBFFE8000LL << 48) + 0x000000000000LL;
	    static const long double VSIN23 = (0xBFFFBFFD8483LL << 48) + 0xEE0C98347AC2LL;

	    //valori per i coseni
	    static const long double VCOS0  = (0xBFFF3FFF8000LL << 48) + 0x000000000000LL;
	    static const long double VCOS1  = (0xBFFF3FFEF746LL << 48) + 0xEA3A45F8A8E1LL;
	    static const long double VCOS2  = (0xBFFF3FFEDDB3LL << 48) + 0xD742C2655BE4LL;
	    static const long double VCOS3  = (0xBFFF3FFEB504LL << 48) + 0xF333F9DE6932LL;
	    static const long double VCOS4  = (0xBFFF3FFE8000LL << 48) + 0x000000000000LL;
	    static const long double VCOS5  = (0xBFFF3FFD8483LL << 48) + 0xEE0C9834A787LL;
	    static const long double VCOS6  = (0xBFFF00000000LL << 48) + 0x000000000000LL;
	    static const long double VCOS7  = (0xBFFFBFFEB504LL << 48) + 0xF333F9DE5C8BLL;
	    static const long double VCOS8  = (0xBFFFBFFE8000LL << 48) + 0x000000000000LL;
	    static const long double VCOS9  = (0xBFFFBFFEB504LL << 48) + 0xF333F9DE5C8BLL;
	    static const long double VCOS10 = (0xBFFFBFFEDDB3LL << 48) + 0xD742C2654311LL;
	    static const long double VCOS11 = (0xBFFFBFFEF746LL << 48) + 0xEA3A45F8A873LL;
	    static const long double VCOS12 = (0xBFFFBFFF8000LL << 48) + 0x000000000000LL;
	    static const long double VCOS13 = (0xBFFFBFFEF746LL << 48) + 0xEA3A45F8AF4DLL;
	    static const long double VCOS14 = (0xBFFFBFFEF746LL << 48) + 0xEA3A45F8AF4DLL;
	    static const long double VCOS15 = (0xBFFFBFFEB504LL << 48) + 0xF333F9DE6F70LL;
	    static const long double VCOS16 = (0xBFFFBFFE8000LL << 48) + 0x000000000000LL;
	    static const long double VCOS17 = (0xBFFFBFFD8483LL << 48) + 0xEE0C98347AC2LL;
	    static const long double VCOS18 = (0xBFFF00000000LL << 48) + 0x000000000000LL;
	    static const long double VCOS19 = (0xBFFF3FFD8483LL << 48) + 0xEE0C98347F4DLL;
	    static const long double VCOS20 = (0xBFFF3FFE8000LL << 48) + 0x000000000000LL;
	    static const long double VCOS21 = (0xBFFF3FFEB504LL << 48) + 0xF333F9DE62F5LL;
	    static const long double VCOS22 = (0xBFFF3FFEDDB3LL << 48) + 0xD742C2654F7ALL;
	    static const long double VCOS23 = (0xBFFF3FFEF746LL << 48) + 0xEA3A45F8A698LL;

	    //valori per gli angoli
	    static const long double VVETT0  = (0xBFFF0000000LL << 48) + 0x0000000000000LL;
	    static const long double VVETT1  = (0xBFFF3FFD860LL << 48) + 0xA91C16B9B2AABLL;
	    static const long double VVETT2  = (0xBFFF3FFE860LL << 48) + 0xA91C16B9B2AABLL;
	    static const long double VVETT3  = (0xBFFF3FFEC90LL << 48) + 0xFDAA22168C000LL;
	    static const long double VVETT4  = (0xBFFF3FFF860LL << 48) + 0xA91C16B9B2AABLL;
	    static const long double VVETT5  = (0xBFFF3FFFA78LL << 48) + 0xD3631C681F555LL;
	    static const long double VVETT6  = (0xBFFF3FFFC90LL << 48) + 0xFDAA22168C000LL;
	    static const long double VVETT7  = (0xBFFF3FFFEA9LL << 48) + 0x27F127C4F8AABLL;
	    static const long double VVETT8  = (0xBFFF4000860LL << 48) + 0xA91C16B9B2AABLL;
	    static const long double VVETT9  = (0xBFFF400096CLL << 48) + 0xBE3F9990E9000LL;
	    static const long double VVETT10 = (0xBFFF4000A78LL << 48) + 0xD3631C681F555LL;
	    static const long double VVETT11 = (0xBFFF4000B84LL << 48) + 0xE8869F3F55AABLL;
	    static const long double VVETT12 = (0xBFFF4000C90LL << 48) + 0xFDAA22168C000LL;
	    static const long double VVETT13 = (0xBFFF4000D9DLL << 48) + 0x12CDA4EDC2555LL;
	    static const long double VVETT14 = (0xBFFF4000EA9LL << 48) + 0x27F127C4F8AABLL;
	    static const long double VVETT15 = (0xBFFF4000FB5LL << 48) + 0x3D14AA9C2F000LL;
	    static const long double VVETT16 = (0xBFFF4001860LL << 48) + 0xA91C16B9B2AABLL;
	    static const long double VVETT17 = (0xBFFF40018E6LL << 48) + 0xB3ADD8254DD55LL;
	    static const long double VVETT18 = (0xBFFF400196CLL << 48) + 0xBE3F9990E9000LL;
	    static const long double VVETT19 = (0xBFFF40019F2LL << 48) + 0xC8D15AFC842ABLL;
	    static const long double VVETT20 = (0xBFFF4001A78LL << 48) + 0xD3631C681F555LL;
	    static const long double VVETT21 = (0xBFFF4001AFELL << 48) + 0xDDF4DDD3BA800LL;
	    static const long double VVETT22 = (0xBFFF4001B84LL << 48) + 0xE8869F3F55AABLL;
	    static const long double VVETT23 = (0xBFFF4001C0ALL << 48) + 0xF31860AAF0D55LL;

	    //valori per le vicinanze
	    static const long double VNEAR0  = (0xBFFF3FFC860ALL << 48) + 0x91C16B9B2AABLL;
	    static const long double VNEAR1  = (0xBFFF3FFDC90FLL << 48) + 0xDAA22168C000LL;
	    static const long double VNEAR2  = (0xBFFF3FFEA78DLL << 48) + 0x3631C681F555LL;
	    static const long double VNEAR3  = (0xBFFF3FFEEA92LL << 48) + 0x7F127C4f8AABLL;
	    static const long double VNEAR4  = (0xBFFF3FFF96CBLL << 48) + 0xE3F9990E9000LL;
	    static const long double VNEAR5  = (0xBFFF3FFFB84ELL << 48) + 0x8869F3F55AABLL;
	    static const long double VNEAR6  = (0xBFFF3FFFD9d1LL << 48) + 0x2CDA4EDC2555LL;
	    static const long double VNEAR7  = (0xBFFF3FFFFB53LL << 48) + 0xD14AA9C2F000LL;
	    static const long double VNEAR8  = (0xBFFF40008E6BLL << 48) + 0x3ADD8254DD55LL;
	    static const long double VNEAR9  = (0xBFFF40009F2CLL << 48) + 0x8D15AFC842ABLL;
	    static const long double VNEAR10 = (0xBFFF4000AFEDLL << 48) + 0xDF4DDD3BA800LL;
	    static const long double VNEAR11 = (0xBFFF4000C0AFLL << 48) + 0x31860AAF0D55LL;
	    static const long double VNEAR12 = (0xBFFF4000D170LL << 48) + 0x83BE382272ABLL;
	    static const long double VNEAR13 = (0xBFFF4000E231LL << 48) + 0xD5F66595D800LL;
	    static const long double VNEAR14 = (0xBFFF4000F2F3LL << 48) + 0x282E93093D55LL;
	    static const long double VNEAR15 = (0xBFFF400181DALL << 48) + 0x3D33603E5155LL;
	    static const long double VNEAR16 = (0xBFFF40018A3ALL << 48) + 0xE64F76F80400LL;
	    static const long double VNEAR17 = (0xBFFF4001929BLL << 48) + 0x8F6B8DB1B6ABLL;
	    static const long double VNEAR18 = (0xBFFF40019AfCLL << 48) + 0x3887A46B6955LL;
	    static const long double VNEAR19 = (0xBFFF4001A35CLL << 48) + 0xE1A3BB251C00LL;
	    static const long double VNEAR20 = (0xBFFF4001ABBDLL << 48) + 0x8ABFD1DECEABLL;
	    static const long double VNEAR21 = (0xBFFF4001B41ELL << 48) + 0x33EBE8988155LL;
	    static const long double VNEAR22 = (0xBFFF4001BC7ELL << 48) + 0xDCF7FF523400LL;
	    static const long double VNEAR23 = (0xBFFF4001C4DFLL << 48) + 0x8614160BE6ABLL;
	};

#endif //__FPU_TRIG_VALUE__
#endif //__FPU_EMU__
