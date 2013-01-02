#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN

#if defined(RC_INVOKED)
#undef RC_INVOKED
#endif

#pragma region defines
//#define NOGDICAPMASKS    // - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
//#define NOVIRTUALKEYCODES// - VK_*
////#define NOWINMESSAGES    // - WM_*, EM_*, LB_*, CB_*
//#define NOWINSTYLES      // - WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
//#define NOSYSMETRICS     // - SM_*
#define NOMENUS          // - MF_*
#define NOICONS          // - IDI_*
////#define NOKEYSTATES      // - MK_*
////#define NOSYSCOMMANDS    // - SC_*
#define NORASTEROPS      // - Binary and Tertiary raster ops
//#define NOSHOWWINDOW     // - SW_*
#define OEMRESOURCE      // - OEM Resource values
//#define NOATOM           // - Atom Manager routines
#define NOCLIPBOARD      // - Clipboard routines
//#define NOCOLOR          // - Screen colors
//#define NOCTLMGR         // - Control and Dialog routines
#define NODRAWTEXT       // - DrawText() and DT_*
//#define NOGDI            // - All GDI defines and routines
//#define NOKERNEL         // - All KERNEL defines and routines
//#define NOUSER           // - All USER defines and routines 
//#define NONLS            // - All NLS defines and routines
//#define NOMB             // - MB_* and MessageBox()
//#define NOMEMMGR         // - GMEM_*, LMEM_*, GHND, LHND, associated routines
//#define NOMETAFILE       // - typedef METAFILEPICT
//#define NOMINMAX         // - Macros min(a,b) and max(a,b)
//#define NOMSG            // - typedef MSG and associated routines
//#define NOOPENFILE       // - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL         // - SB_* and scrolling routines
#define NOSERVICE        // - All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND          // - Sound driver routines
//#define NOTEXTMETRIC     // - typedef TEXTMETRIC and associated routines
#define NOWH             // - SetWindowsHook and WH_*
#define NOWINOFFSETS     // - GWL_*, GCL_*, associated routines
#define NOCOMM           // - COMM driver routines
#define NOKANJI          // - Kanji support stuff.
#define NOHELP           // - Help engine interface.
//#define NOPROFILER       // - Profiler interface.
//#define NODEFERWINDOWPOS // - DeferWindowPos routines
#define NOMCX            // - Modem Configuration Extensions
#pragma endregion


#include <windows.h>
#include <d3d9.h>
#include <d3d11.h>

#include <directxmath.h>
#include <directXPackedVector.h> 
#include <directXColors.h>
#include <directXCollision.h>

#include <d3dcompiler.h>

//#include <dxerr.h>

#include <assert.h>
#include <stdint.h>
#include <malloc.h>

#pragma warning(disable : 4530) // C++ exception handler used, but unwind semantics are not enabled
#pragma warning(disable : 4290) // Suppress 'C++ Exception Specification ignored'
#pragma warning(disable : 4996) // unsafe std functions
#pragma warning(disable : 4244) // wchart to char, possible lost of data
#pragma warning(disable : 4127) // conditional expression is constant
#pragma warning(disable : 4238) // nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable : 4505) // unreferenced local function has been removed
#pragma warning(disable : 4324) // structure was padded due to __declspec(align())

#pragma warning(push, 1)        // Set warning levels to a quieter level for the STL

#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include <algorithm>
#include <memory>

#pragma warning(pop) 

 
#include "globals.h"
#include "utils/debug_logger.h"

#pragma comment(lib, "assimp.lib") 

#pragma comment(lib, "PhysX3CHECKED_x86.lib")
#pragma comment(lib, "PhysX3CommonCHECKED_x86.lib")
#pragma comment(lib, "PhysXProfileSDKCHECKED.lib")
#pragma comment(lib, "PhysX3ExtensionsCHECKED.lib")


using namespace DirectX;
using namespace PackedVector;

#define UNUSED(p) ((void)(p))
// alternatives:
// #define UNUSED(x) ( *(volatile typeof(x) *)&(x) = (x); )
// #define UNUSED(X) sizeof(X)
// #define UNUSED(a) ((a)=(a))

typedef signed char         s8;
typedef signed short        s16;
typedef signed int          s32;
typedef signed __int64      s64;

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned __int64    u64;

typedef float               f32;
typedef double              f64;

typedef unsigned int        uint;
typedef unsigned char       byte;

typedef wchar_t             wchar;
typedef unsigned short      ushort;

#define SAFE_DELETE(x)       { if((x)) { delete   (x); (x) = NULL;} }
#define SAFE_DELETE_ARRAY(x) { if((x)) { delete[] (x); (x) = NULL;} }
#define SAFE_RELEASE(x)      { if((x)) { (x)->Release(); (x) = NULL;} }    

#define PI      3.1415926535897932384626433832795f
#define TWOPI   6.283185307f
#define HALFPI  1.570796327f

#define DegToRad( _deg ) ( _deg * (PI / 180.0f) )
#define RadToDeg( _rad ) ( _rad * (180.0f / PI) )

#define CheckError(result)   if(FAILED(result)){return result;}
#define CheckErrorBool(result)   if(FAILED(result)){return false;}
#define CheckErrorVoid(result)   if(FAILED(result)){return;}
#define CheckErrorNULL(result)   if(FAILED(result)){return NULL;}
#define CheckErrorBoolNULL(result)   if(result == false){return NULL;}
#define CheckNULLBool(result)   if(result == false){return NULL;}
#define CheckGDIERROR(status) { if (status != Gdiplus::Ok){ _ASSERT(false);return false;}}
#define CheckNULL(status) { if (!status){ return;}}
#define CheckNULLNULL(status) { if (!status){ return NULL;}}

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))


/*
const unsigned int  MaxUInt32 = 0xFFFFFFFF;
const int           MinInt32 = 0x80000000;
const int           MaxInt32 = 0x7FFFFFFF;
const float         MaxFloat = 3.402823466e+38F;
const float         MinFloat = 1.175494351e-38F;
*/

#define Infinity  3.402823466e+38F

#define GetBit(x)               ( 1 << (x) )
#define TestBit(flag, bit)      ( ((flag) & GetBit((bit))) !=0)
#define TestBitAll(flag, set)   ( ((flag) & (set)) == (set))
#define TestBitIfAny(flag, set) ( ((flag) & (set)) !=0)
#define SetBit(flag, bit)       ( (flag) |= GetBit((bit)))
#define SetAll(flag, set)       ( (flag) |= (set))
#define ClearBit(flag, bit)     ( (flag) &= ~GetBit((bit)))
#define ClearAll(flag, set)     ( (flag) &= ~(set))


#define lerp(t,a,b)             ( (a) + (t) * (float) ((b)-(a)) )
#define unlerp(t,a,b)           ( ((t) - (a)) / (float) ((b) - (a)) )

#define clamp(x,xmin,xmax)      ((x) < (xmin) ? (xmin) : (x) > (xmax) ? (xmax) : (x))

#define bitset_setbit(arr,n)    ((arr)[(n) >> 5] |=  (1 << (n & 31)))
#define bitset_clearbit(arr,n)  ((arr)[(n) >> 5] &= ~(1 << (n & 31)))
#define bitset_testbit(arr,n)   ((arr)[(n) >> 5] &   (1 << (n & 31)))

#define ASSERT_ON_FAIL(hr)  if(hr) assert(false); 


/// Disable copy constructor and assignment operator. 
#define NO_COPY(C) \
    private: \
    C( const C & ); \
    C &operator=( const C & );


/// Disable dynamic allocation on the heap. 
#define NO_HEAPALLOC() \
    private: \
    static void *operator new(size_t size); \
    static void *operator new[](size_t size);
