/* control - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 * added: 20160509, wirbel
 */
#ifndef _terminal_escape_codes_
#define _terminal_escape_codes_

/*******************************************************************************
 * Terminal Escape Codes
 *
 *  \033     = escape character (ascii 27 decimal, 33 octal)
 *  <v>      = single or double digit number.  Vertical coordinate
 *  <h>      = single or double digit number.  Horizontal coordinate
 *  <n>      = single or double digit number.  Number of chars/lines
 *  others   = single characters just as they appear.
 *******************************************************************************/

#define EC_SHOWCURSOR   "\033[?25h"      // rarely used at all, but..
#define EC_HIDECURSOR   "\033[?25l"      // rarely used at all, but..

#define EC_HOME         "\033[H"         // Move cursor to upper left corner 
#define EC_CURPOS       "\033[%i;%if"    // Move cursor to screen location v,h: '\033[<v>;<h>H'


#define EC_CLRSCR       "\033[2J"        // Clear entire screen (erase display)
#define EC_CLREOL       "\033[K"         // Erase to end of current line
#define EC_CLRBOL       "\033[1K"        // Erase to begin of current line
#define EC_CLRLINE      "\033[2K"        // Erase entire current line
#define EC_CLRTABS      "\033[3g"        // Clear all tabs
#define EC_TABSET       "\033H"          // Set a tab at the current column

// for colors, see http://ezprompt.net/
#define EC_BK_WT        "\033[30;47m"    // FG black, BG white
#define EC_BK_CY        "\033[30;46m"    // FG black, BG cyan
#define EC_BK_MG        "\033[30;45m"    // FG black, BG magenta
#define EC_BK_BL        "\033[30;44m"    // FG black, BG blue
#define EC_BK_YE        "\033[30;43m"    // FG black, BG yellow
#define EC_BK_GN        "\033[30;42m"    // FG black, BG green
#define EC_BK_RD        "\033[30;41m"    // FG black, BG red
#define EC_BK_BK        "\033[30;40m"    // FG black, BG black -> useless.
//--
#define EC_RD_WT        "\033[31;47m"    // FG red, BG white
#define EC_RD_CY        "\033[31;46m"    // FG red, BG cyan
#define EC_RD_MG        "\033[31;45m"    // FG red, BG magenta
#define EC_RD_BL        "\033[31;44m"    // FG red, BG blue
#define EC_RD_YE        "\033[31;43m"    // FG red, BG yellow
#define EC_RD_GN        "\033[31;42m"    // FG red, BG green
#define EC_RD_RD        "\033[31;41m"    // FG red, BG red -> useless.
#define EC_RD_BK        "\033[31;40m"    // FG red, BG black
//--
#define EC_GN_WT        "\033[32;47m"    // FG green, BG white
#define EC_GN_CY        "\033[32;46m"    // FG green, BG cyan
#define EC_GN_MG        "\033[32;45m"    // FG green, BG magenta
#define EC_GN_BL        "\033[32;44m"    // FG green, BG blue
#define EC_GN_YE        "\033[32;43m"    // FG green, BG yellow
#define EC_GN_GN        "\033[32;42m"    // FG green, BG green -> useless.
#define EC_GN_RD        "\033[32;41m"    // FG green, BG red
#define EC_GN_BK        "\033[32;40m"    // FG green, BG black
//--                                                          
#define EC_YE_WT        "\033[33;47m"    // FG yellow, BG white
#define EC_YE_CY        "\033[33;46m"    // FG yellow, BG cyan 
#define EC_YE_MG        "\033[33;45m"    // FG yellow, BG magenta
#define EC_YE_BL        "\033[33;44m"    // FG yellow, BG blue 
#define EC_YE_YE        "\033[33;43m"    // FG yellow, BG yellow -> useless.
#define EC_YE_GN        "\033[33;42m"    // FG yellow, BG green
#define EC_YE_RD        "\033[33;41m"    // FG yellow, BG red  
#define EC_YE_BK        "\033[33;40m"    // FG yellow, BG black
//--                                                          
#define EC_BL_WT        "\033[34;47m"    // FG blue, BG white
#define EC_BL_CY        "\033[34;46m"    // FG blue, BG cyan 
#define EC_BL_MG        "\033[34;45m"    // FG blue, BG magenta
#define EC_BL_BL        "\033[34;44m"    // FG blue, BG blue  -> useless.
#define EC_BL_YE        "\033[34;43m"    // FG blue, BG yellow
#define EC_BL_GN        "\033[34;42m"    // FG blue, BG green
#define EC_BL_RD        "\033[34;41m"    // FG blue, BG red  
#define EC_BL_BK        "\033[34;40m"    // FG blue, BG black
//--                                                          
#define EC_MG_WT        "\033[35;47m"    // FG magenta, BG white
#define EC_MG_CY        "\033[35;46m"    // FG magenta, BG cyan 
#define EC_MG_MG        "\033[35;45m"    // FG magenta, BG magenta -> useless.
#define EC_MG_BL        "\033[35;44m"    // FG magenta, BG blue
#define EC_MG_YE        "\033[35;43m"    // FG magenta, BG yellow
#define EC_MG_GN        "\033[35;42m"    // FG magenta, BG green
#define EC_MG_RD        "\033[35;41m"    // FG magenta, BG red  
#define EC_MG_BK        "\033[35;40m"    // FG magenta, BG black
//--                                                          
#define EC_CY_WT        "\033[36;47m"    // FG cyan, BG white
#define EC_CY_CY        "\033[36;46m"    // FG cyan, BG cyan  -> useless.
#define EC_CY_MG        "\033[36;45m"    // FG cyan, BG magenta
#define EC_CY_BL        "\033[36;44m"    // FG cyan, BG blue
#define EC_CY_YE        "\033[36;43m"    // FG cyan, BG yellow
#define EC_CY_GN        "\033[36;42m"    // FG cyan, BG green
#define EC_CY_RD        "\033[36;41m"    // FG cyan, BG red  
#define EC_CY_BK        "\033[36;40m"    // FG cyan, BG black
//--                                                          
#define EC_WT_WT        "\033[37;47m"    // FG white, BG white -> useless.
#define EC_WT_CY        "\033[37;46m"    // FG white, BG cyan
#define EC_WT_MG        "\033[37;45m"    // FG white, BG magenta
#define EC_WT_BL        "\033[37;44m"    // FG white, BG blue
#define EC_WT_YE        "\033[37;43m"    // FG white, BG yellow
#define EC_WT_GN        "\033[37;42m"    // FG white, BG green
#define EC_WT_RD        "\033[37;41m"    // FG white, BG red  
#define EC_WT_BK        "\033[37;40m"    // FG white, BG black
//-- 
#define EC_FG_LGY       "\033[0;37m"     // FG light-grey   eq: \033[37m
#define EC_FG_CY        "\033[0;36m"     // FG cyan         eq: \033[36m
#define EC_FG_MG        "\033[0;35m"     // FG magenta      eq: \033[35m
#define EC_FG_BL        "\033[0;34m"     // FG blue         eq: \033[34m
#define EC_FG_YE        "\033[0;33m"     // FG yellow       eq: \033[33m
#define EC_FG_GN        "\033[0;32m"     // FG green        eq: \033[32m
#define EC_FG_RD        "\033[0;31m"     // FG red          eq: \033[31m
#define EC_FG_BK        "\033[0;30m"     // FG black        eq: \033[30m
//--
#define EC_FG_WH        "\033[1;37m"     // FG white
#define EC_FG_LCY       "\033[1;36m"     // FG light-cyan
#define EC_FG_LMG       "\033[1;35m"     // FG light-magenta
#define EC_FG_LBL       "\033[1;34m"     // FG light-blue
#define EC_FG_LYE       "\033[1;33m"     // FG light-yellow
#define EC_FG_LGN       "\033[1;32m"     // FG light-green
#define EC_FG_LRD       "\033[1;31m"     // FG light-red
#define EC_FG_DGY       "\033[1;30m"     // FG dark-grey
//--
#define EC_BG_LGY       "\033[0;47m"     // BG light-grey   eq: \033[47m
#define EC_BG_CY        "\033[0;46m"     // BG cyan         eq: \033[46m
#define EC_BG_MG        "\033[0;45m"     // BG magenta      eq: \033[45m
#define EC_BG_BL        "\033[0;44m"     // BG blue         eq: \033[44m
#define EC_BG_YE        "\033[0;43m"     // BG yellow       eq: \033[43m
#define EC_BG_GN        "\033[0;42m"     // BG green        eq: \033[42m
#define EC_BG_RD        "\033[0;41m"     // BG red          eq: \033[41m
#define EC_BG_BK        "\033[0;40m"     // BG black        eq: \033[40m
//--
#define EC_BG_WT        "\033[1;47m"     // BG white
#define EC_BG_LCY       "\033[1;46m"     // BG light-cyan
#define EC_BG_LMG       "\033[1;45m"     // BG light-magenta
#define EC_BG_LBL       "\033[1;44m"     // BG light-blue
#define EC_BG_LYE       "\033[1;43m"     // BG light-yellow
#define EC_BG_LGN       "\033[1;42m"     // BG light-green
#define EC_BG_LRD       "\033[1;41m"     // BG light-red
#define EC_BG_DGY       "\033[1;40m"     // BG dark-grey
//--
#define EC_INVISIBLE    "\033[8m"        // Turn invisible text mode on
#define EC_NEGATIVE     "\033[7m"        // Turn reverse video on
#define EC_RAPIDBLINK   "\033[6m"        // 
#define EC_BLINK        "\033[5m"        // Turn blinking mode on
#define EC_UNDERLINE    "\033[4m"        // Turn underline mode on
#define EC_ITALIC       "\033[3m"        // 
#define EC_FAINT        "\033[2m"        // Turn low intensity mode on
#define EC_BOLD         "\033[1m"        // Turn bold mode on
#define EC_DEFAULT      "\033[m"         // back to plain text, eq: \033[0m,

#endif






/*******************************************************************************
 * NO SOURCE CODE BELOW THIS LINE.
 *
 * The following lines are just for reference and not in use in the
 * vdr control plugin. They are by purpose commented.
 ******************************************************************************/

/* 
// define EC_SAVE_CUR_POS        "\033[s"
// define EC_REST_CUR_POS        "\033[u"
// define EC_MV_CUR_UP           "\033[A"
// define EC_MV_CUR_DN           "\033[B"
// --------------------//--------------------//-------------------------------
// Name                // Esc Code           // Description
// --------------------//--------------------//-------------------------------
// define EC_SETNL          "\033[20h"       // Set new line mode                     
// define EC_SETAPPL        "\033[?1h"       // Set cursor key to application         
// define EC_SETCOL         "\033[?3h"       // Set number of columns to 132          
// define EC_SETSMOOTH      "\033[?4h"       // Set smooth scrolling                  
// define EC_SETREVSCRN     "\033[?5h"       // Set reverse video on screen           
// define EC_SETORGREL      "\033[?6h"       // Set origin to relative                
// define EC_SETWRAP        "\033[?7h"       // Set auto-wrap mode                    
// define EC_SETREP         "\033[?8h"       // Set auto-repeat mode                  
// define EC_SETINTER       "\033[?9h"       // Set interlacing mode                  
// define EC_SETLF          "\033[20l"       // Set line feed mode                    
// define EC_SETCURSOR      "\033[?1l"       // Set cursor key to cursor              
// define EC_SETVT52        "\033[?2l"       // Set VT52 (versus ANSI)                
// define EC_RESETCOL       "\033[?3l"       // Set number of columns to 80           
// define EC_SETJUMP        "\033[?4l"       // Set jump scrolling                    
// define EC_SETNORMSCRN    "\033[?5l"       // Set normal video on screen            
// define EC_SETORGABS      "\033[?6l"       // Set origin to absolute                
// define EC_RESETWRAP      "\033[?7l"       // Reset auto-wrap mode                  
// define EC_RESETREP       "\033[?8l"       // Reset auto-repeat mode                
// define EC_RESETINTER     "\033[?9l"       // Reset interlacing mode                
// define EC_ALTKEYPAD      "\033="          // Set alternate keypad mode             
// define EC_NUMKEYPAD      "\033>"          // Set numeric keypad mode               
// define EC_SETUKG0        "\033(A"         // Set United Kingdom G0 character set   
// define EC_SETUKG1        "\033)A"         // Set United Kingdom G1 character set   
// define EC_SETUSG0        "\033(B"         // Set United States G0 character set    
// define EC_SETUSG1        "\033)B"         // Set United States G1 character set    
// define EC_SETSPECG0      "\033(0"         // Set G0 special chars. & line set      
// define EC_SETSPECG1      "\033)0"         // Set G1 special chars. & line set      
// define EC_SETALTG0       "\033(1"         // Set G0 alternate character ROM        
// define EC_SETALTG1       "\033)1"         // Set G1 alternate character ROM        
// define EC_SETALTSPECG0   "\033(2"         // Set G0 alt char ROM and spec. graphics
// define EC_SETALTSPECG1   "\033)2"         // Set G1 alt char ROM and spec. graphics
// define EC_SETSS2         "\033N"          // Set single shift 2                    
// define EC_SETSS3         "\033O"          // Set single shift 3                    
// define EC_SETWIN         "\033[<v>;<v>r"  // Set top and bottom line#s of a window 
// define EC_CURSORUP(N)    "\033[<n>A"      // Move cursor up n lines                
// define EC_CURSORDN(N)    "\033[<n>B"      // Move cursor down n lines              
// define EC_CURSORRT(N)    "\033[<n>C"      // Move cursor right n lines             
// define EC_CURSORLF(N)    "\033[<n>D"      // Move cursor left n lines              
// define EC_CURSORHOME     "\033[;H"        // Move cursor to upper left corner    
// define EC_HVHOME         "\033[f"         // Move cursor to upper left corner      
// define EC_HVHOME         "\033[;f"        // Move cursor to upper left corner      
// define EC_INDEX          "\033D"          // Move/scroll window up one line        
// define EC_REVINDEX       "\033M"          // Move/scroll window down one line      
// define EC_NEXTLINE       "\033E"          // Move to next line                     
// define EC_SAVECURSOR     "\0337"          // Save cursor position and attributes   
// define EC_RESTORECURSOR  "\0338"          // Restore cursor position and attributes
// define EC_TABCLR         "\033[g"         // Clear a tab at the current column     
// define EC_TABCLR         "\033[0g"        // Clear a tab at the current column     
// define EC_DHTOP          "\033#3"         // Double-height letters, top half       
// define EC_DHBOT          "\033#4"         // Double-height letters, bottom half    
// define EC_SWSH           "\033#5"         // Single width, single height letters   
// define EC_DWSH           "\033#6"         // Double width, single height letters                        
// define EC_CLEAREOS       "\033[J"         // Clear screen from cursor down (erase to end of display)
// define EC_CLEAREOS       "\033[0J"        // Clear screen from cursor down         
// define EC_CLEARBOS       "\033[1J"        // Clear screen from cursor up           
// define EC_DEVSTAT        "\0335n"         // Device status report                  
// define EC_TERMOK         "\0330n"         // Response: terminal is OK    
// define EC_TERMNOK        "\0333n"         // Response: terminal is not OK
// define EC_CURSORPOS      "\033<v>;<h>R"   // Response: cursor is at v,h  
// define EC_GETCURSOR      "\0336n"         // Get cursor position         
// define EC_IDENT          "\033[c"         // Identify what terminal type 
// define EC_IDENT          "\033[0c"        // Identify what terminal type (another)
// define EC_GETTYPE        "\033[?1;<n>0c"  //    Response: terminal type code n
// define EC_RESET          "\033c"          // Reset terminal to initial state
// define EC_ALIGN          "\033#8"         // Screen alignment display 
// define EC_TESTPU         "\033[2;1y"      // Confidence power up test 
// define EC_TESTLB         "\033[2;2y"      // Confidence loopback test 
// define EC_TESTPUREP      "\033[2;9y"      // Repeat power up test     
// define EC_TESTLBREP      "\033[2;10y"     // Repeat loopback test     
// define EC_LEDSOFF        "\033[0q"        // Turn off all four leds
// define EC_LED1           "\033[1q"        // Turn on LED #1
// define EC_LED2           "\033[2q"        // Turn on LED #2
// define EC_LED3           "\033[3q"        // Turn on LED #3
// define EC_LED4           "\033[4q"        // Turn on LED #4
//-------------------------------------------------------------------------
//    All codes below are for use in VT52 compatibility mode.    
//-------------------------------------------------------------------------
// define EC_SETANSI        "\033<"          // Enter/exit ANSI mode (VT52)       
// define EC_ALTKEYPAD      "\033="          // Enter alternate keypad mode       
// define EC_NUMKEYPAD      "\033>"          // Exit alternate keypad mode        
// define EC_SETGR          "\033F"          // Use special graphics character set
// define EC_RESETGR        "\033G"          // Use normal US/UK character set    
// define EC_CURSORUP       "\033A"          // Move cursor up one line           
// define EC_CURSORDN       "\033B"          // Move cursor down one line         
// define EC_CURSORRT       "\033C"          // Move cursor right one char        
// define EC_CURSORLF       "\033D"          // Move cursor left one char         
// define EC_CURSORHOME     "\033H"          // Move cursor to upper left corner  
// define EC_CURSORPOS(V,H) "\033<v><h>"     // Move cursor to v,h location       
// define EC_REVINDEX       "\033I"          // Generate a reverse line-feed      
// define EC_CLEAREOS       "\033J"          // Erase to end of screen            
// define EC_IDENT          "\033Z"          // Identify what the terminal is     
// define EC_IDENTRESP      "\033/Z"         // Correct response to ident         
//-------------------------------------------------------------------------
//   VT100 Special Key Codes
//   
//   These are sent from the terminal back to the computer when the
//   particular key is pressed.  Note that the numeric keypad keys
//   send different codes in numeric mode than in alternate mode.
//   See escape codes above to change keypad mode.
//-------------------------------------------------------------------------
// Function Keys:
// PF1                \033OP
// PF2                \033OQ
// PF3                \033OR
// PF4                \033OS
//
//---------------------------------------
//  Arrow Keys:   | Reset     |    Set
//----------------|-----------|----------
// up             |  \033A    | \033OA
// down           |  \033B    | \033OB
// right          |  \033C    | \033OC
// left           |  \033D    | \033OD
//
//---------------------------------------
// Numeric        |      Keypad Mode
// Keypad Key     | Numeric  | Alternate
//---------------------------|-----------
// 0              |  0       | \033Op
// 1              |  1       | \033Oq
// 2              |  2       | \033Or
// 3              |  3       | \033Os
// 4              |  4       | \033Ot
// 5              |  5       | \033Ou
// 6              |  6       | \033Ov
// 7              |  7       | \033Ow
// 8              |  8       | \033Ox
// 9              |  9       | \033Oy
// - (minus)      |  -       | \033Om
// , (comma)      |  ,       | \033Ol
// . (period)     |  .       | \033On
// ENTER          | ^M       | \033OM
//
// define  T_IL  "\033[L" // insert line 
// define  T_DL  "\033[M" // delete line 
*/
