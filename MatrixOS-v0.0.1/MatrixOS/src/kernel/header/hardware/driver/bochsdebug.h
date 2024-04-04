/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: video.h			|
	| Date: 23/11/2004			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

/*
	**************************
	**	 Include	**
	**************************
*/

#include <kdef.h>


namespace hardware{

	namespace driver{

		namespace vm{

			enum TraceStatus{
				tsOFF,
				tsON
			};
		
			class BochsDebug{
			public:
				static void Init();
				
				static void OutputMsg(const char c);
				static void OutputMsg(const char *c);
				
				static void IstructionTrace(TraceStatus status);
				static void RegisterTrace(TraceStatus status);
				
				static void Stop();
			};
		
		}
	}
}
