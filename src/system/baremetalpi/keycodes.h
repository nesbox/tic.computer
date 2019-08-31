#include <tic.h>

// derived from keymap_it

static const tic_keycode TicKeyboardCodes[256] = 
	{
tic_key_unknown,	 // KeyNone, 0x00
tic_key_unknown,	 // KeyNone, 0x01
tic_key_unknown,	 // KeyNone, 0x02
tic_key_unknown,	 // KeyNone, 0x03
tic_key_a,	 // 'a', 0x04
tic_key_b,	 // 'b', 0x05
tic_key_c,	 // 'c', 0x06
tic_key_d,	 // 'd', 0x07
tic_key_e,	 // 'e', 0x08
tic_key_f,	 // 'f', 0x09
tic_key_g,	 // 'g', 0x0A
tic_key_h,	 // 'h', 0x0B
tic_key_i,	 // 'i', 0x0C
tic_key_j,	 // 'j', 0x0D
tic_key_k,	 // 'k', 0x0E
tic_key_l,	 // 'l', 0x0F
tic_key_m,	 // 'm', 0x10
tic_key_n,	 // 'n', 0x11
tic_key_o,	 // 'o', 0x12
tic_key_p,	 // 'p', 0x13
tic_key_q,	 // 'q', 0x14
tic_key_r,	 // 'r', 0x15
tic_key_s,	 // 's', 0x16
tic_key_t,	 // 't', 0x17
tic_key_u,	 // 'u', 0x18
tic_key_v,	 // 'v', 0x19
tic_key_w,	 // 'w', 0x1A
tic_key_x,	 // 'x', 0x1B
tic_key_y,	 // 'y', 0x1C
tic_key_z,	 // 'z', 0x1D
tic_key_1,	 // '1', 0x1E
tic_key_2,	 // '2', 0x1F
tic_key_3,	 // '3', 0x20
tic_key_4,	 // '4', 0x21
tic_key_5,	 // '5', 0x22
tic_key_6,	 // '6', 0x23
tic_key_7,	 // '7', 0x24
tic_key_8,	 // '8', 0x25
tic_key_9,	 // '9', 0x26
tic_key_0,	 // '0', 0x27
tic_key_return,	 // KeyReturn, 0x28
tic_key_escape,	 // KeyEscape, 0x29
tic_key_backspace,	 // KeyBackspace, 0x2A
tic_key_tab,	 // KeyTabulator, 0x2B
tic_key_space,	 // KeySpace, 0x2C
tic_key_apostrophe,	 // '\'', 0x2D
tic_key_unknown,	 // C('?'), 0x2E -- ^
tic_key_leftbracket,	 // C('?'), 0x2F --  [
tic_key_rightbracket,	 // '+', 0x30 --  ]
tic_key_unknown,	 // C('?'), 0x31 
tic_key_unknown,	 // C('?'), 0x32  
tic_key_unknown,	 // C('?'), 0x33 -- @
tic_key_unknown,	 // C('?'), 0x34 -- #
tic_key_backslash,	 // '\\', 0x35
tic_key_comma,	 // ',', 0x36
tic_key_period,	 // '.', 0x37
tic_key_minus,	 // '-', 0x38
tic_key_capslock,	 // KeyCapsLock, 0x39
tic_key_f1,		 // KeyF1, 0x3A
tic_key_f2,		 // KeyF2, 0x3B
tic_key_f3,		 // KeyF3, 0x3C
tic_key_f4,		 // KeyF4, 0x3D
tic_key_f5,		 // KeyF5, 0x3E
tic_key_f6,		 // KeyF6, 0x3F
tic_key_f7,		 // KeyF7, 0x40
tic_key_f8,		 // KeyF8, 0x41
tic_key_f9,		 // KeyF9, 0x42
tic_key_f10,		 // KeyF10, 0x43
tic_key_f11,		 // KeyF11, 0x44
tic_key_f12,		 // KeyF12, 0x45
tic_key_unknown,	 // KeyPrintScreen, 0x46
tic_key_unknown,	 // KeyScrollLock, 0x47
tic_key_unknown,	 // KeyPause, 0x48
tic_key_insert,		 // KeyInsert, 0x49
tic_key_home,		 // KeyHome, 0x4A
tic_key_pageup,		 // KeyPageUp, 0x4B
tic_key_delete,		 // KeyDelete, 0x4C
tic_key_end,		 // KeyEnd, 0x4D
tic_key_pagedown,	 // KeyPageDown, 0x4E
tic_key_right,		 // KeyRight, 0x4F
tic_key_left,		 // KeyLeft, 0x50
tic_key_down,		 // KeyDown, 0x51
tic_key_up,		 // KeyUp, 0x52
tic_key_unknown,	 // KeyNumLock, 0x53
tic_key_unknown,	 // KeyKP_Divide, 0x54
tic_key_unknown,	 // KeyKP_Multiply, 0x55
tic_key_unknown,	 // KeyKP_Subtract, 0x56
tic_key_unknown,	 // KeyKP_Add, 0x57
tic_key_unknown,	 // KeyKP_Enter, 0x58
tic_key_unknown,	 // KeyEnd, 0x59
tic_key_unknown,	 // KeyDown, 0x5A
tic_key_unknown,	 // KeyPageDown, 0x5B
tic_key_unknown,	 // KeyLeft, 0x5C
tic_key_unknown,	 // KeyKP_Center, 0x5D
tic_key_unknown,	 // KeyRight, 0x5E
tic_key_unknown,	 // KeyHome, 0x5F
tic_key_unknown,	 // KeyUp, 0x60
tic_key_unknown,	 // KeyPageUp, 0x61
tic_key_unknown,	 // KeyInsert, 0x62
tic_key_unknown,	 // KeyDelete, 0x63
tic_key_unknown,	 // '<', 0x64
tic_key_unknown,	 // KeyApplication, 0x65
tic_key_unknown,	 // KeyNone, 0x66
tic_key_unknown,	 // KeyNone, 0x67
tic_key_unknown,	 // KeyNone, 0x68
tic_key_unknown,	 // KeyNone, 0x69
tic_key_unknown,	 // KeyNone, 0x6A
tic_key_unknown,	 // KeyNone, 0x6B
tic_key_unknown,	 // KeyNone, 0x6C
tic_key_unknown,	 // KeyNone, 0x6D
tic_key_unknown,	 // KeyNone, 0x6E
tic_key_unknown,	 // KeyNone, 0x6F
tic_key_unknown,	 // KeyNone, 0x70
tic_key_unknown,	 // KeyNone, 0x71
tic_key_unknown,	 // KeyNone, 0x72
tic_key_unknown,	 // KeyNone, 0x73
tic_key_unknown,	 // KeyNone, 0x74
tic_key_unknown,	 // KeyNone, 0x75
tic_key_unknown,	 // KeyNone, 0x76
tic_key_unknown,	 // KeyNone, 0x77
tic_key_unknown,	 // KeyNone, 0x78
tic_key_unknown,	 // KeyNone, 0x79
tic_key_unknown,	 // KeyNone, 0x7A
tic_key_unknown,	 // KeyNone, 0x7B
tic_key_unknown,	 // KeyNone, 0x7C
tic_key_unknown,	 // KeyNone, 0x7D
tic_key_unknown,	 // KeyNone, 0x7E
tic_key_unknown // KeyNone, 0x7F

};
