
/////////////////////////////////////////////////////////////////////////////////
//
// Date        : 03/21/2004
//
// Filename    : ml_gencon.h
//
// Author      : Hani Al-Dayaa
//
// Topic       : Macbine Learning
//
// Description : General Constants that can be used in many 
//               applications. Using these constants will make the 
//               programs easily configurable without the need to edit
//               the functions code.
//
//
// Revision History :
// ----------------------------------------------------------------------------
// <Rev> | <Author>  | <Date>   | <Changes Made>
//       |           |          |
// 1.00  |  H.A.     | 03/21/04 | - Initial Revision.
//       |           |          |
// 2.00  |  M.M.     | 08/07/09 | - Modified to meet new ANSI standards
// ----------------------------------------------------------------------------
//      
/////////////////////////////////////////////////////////////////////////////////



#ifndef ML_GENCON_H
#define ML_GENCON_H


/////////////////////////////////////////////////////////////////////////////////
////////////////////////////// ENUMERATED TYPES /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// Direction-Value enumerated type.
typedef enum
{
  NO_DIR,       // 0.
  UP,           // 1.
  DOWN,         // 2.
  RIGHT,        // 3.
  LEFT          // 4.
} dirVal_t;
#define NUM_DIR_VALS 5

// Action Check enumerated type.
typedef enum
{
  NO_CHECK,        // 0.
  CHECK_LIMIT,     // 1.
  CHECK_BARRIER,   // 2.
  CHECK_BOTH       // 3.
} actCheck_t;


// Location-State enumerated type.
typedef enum
{
  START   = 'S',
  GOAL    = 'G',
  BARRIER = 'B',
  OPEN    = 'O',
  BLANK   = '?'
} locState_t;


// Boolean enumerated type.
typedef enum
{
  NEVER  = -2,
  NOMORE = -1,
  FALSE_t  = 0,
  TRUE_t   = 1, 
  ALWAYS = 2
} bool_t;


// Side enumerated type.
typedef enum
{
  U_SIDE  = 0,   // Up Side.
  UR_SIDE,       // Up-Left Side.
  R_SIDE,        // Right Side.
  DR_SIDE,       // Down-Right Side.
  D_SIDE,        // Down Side.
  DL_SIDE,       // Down-Left Side.
  L_SIDE,        // Left Side.
  UL_SIDE        // Up-Left Side.
} side_t;


// Learn enumerated type.
typedef enum
{
  QVAL_ONLY        = 0,    // Learning based on Q values only.
  DISTG_ONLY       = 1,    // Learning based on distance only.
  QVAL_NVSTED      = 2,    // Learning based on Q values and visited.
  DISTG_NVSTED     = 3,    // Learning based on distance and visited.
  QVAL_DISTG       = 4,    // Learning based on Q values and distance.
  COMBINE_ALL      = 5,    // Learning based Q values, distance, and visited.
  PHS_DISTG_ONLY        = 10,   // Physical based on distance only.
  PHS_DISTG_NVSTED      = 11,   // Physical based on distance and visited.
  PHS_DISTG_NVSTED_CLO  = 12,   // Physical based on distance and visited using Closed-Loop Opening.
  PHS_DISTG_NVSTED_LP   = 13    // Physical based on distance and visited using Loop Pinching.
} learnMethod_t;

  
// Path-File Mode enumerated type.
typedef enum
{
  NO_PATH_FILE = 0,
  PHS_PATH_FILE,
  LRN_PATH_FILE,
  BOTH_PATH_FILE
} pathFileMode_t;


// Action type enumerated type.
typedef enum
{
  NO_TYPE = 0,
  RAND,
  ANY_RAND,
  POLICY,
  CNTRL,
  DIST,
  DIST_VIST
} actionType_t;


// Error type enumerated type.
typedef enum
{
  NO_ERR = 0,
  UNREACH_ERR,
  REACHMAX_ERR,
  MOVED
} error_t;



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// MACROS //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr   // Swap.
#define MIN(a,b) (a)<(b)?(a):(b)                // Minimum.
#define MAX(a,b) (a)>(b)?(a):(b)                // Maximum.
#define SQUARE(x) ((x)*(x))                     // Square.
#define ABS(x) (((x)<0)?(0-(x)):(x))            // Absolute. 


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////// CONSTANT/MACRO DEFINES //////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


// Reiforcement Learning constants.
#define D_ALPHA         0.1             // Default Step-Size parameter (Alpha).
#define D_GAMMA         0.9             // Default Discount rate (Gamma).
#define D_LAMBDA        0.5             // Default Lookahead weight factor (lambda). 
#define INIT_TAW        50000           // Initial/Default Learning-rate parameter (Taw).
#define D_TAW_DEC       250             // Default Taw decrement factor.
#define GREEDY_TAW      1               // Taw used for greedy mode, as in physical paths.
#define RWRD_FACTOR     1               // Reward factor.
#define DYNAMIC_ENV     0

// Agent constants.
//#define MAX_AGENT_STEPS  2048           // Maximum number of Agent steps in any path.
#define MAX_AGENT_STEPS(X,Y) (25*(X)*(Y))


// Miscellaneous constants.
#define STRING_LEN      32              // Number of chars in string (including the \0).
#define VERY_SMALL_VAL  0.00001         // A Very small value, very close to 0.


// Exit status constants.
//#define EXIT_SUCCESS    1
//#define EXIT_FAILURE   -1


#endif
