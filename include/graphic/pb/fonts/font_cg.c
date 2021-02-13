const char font_cg[] = {
	/* 0x00-0x7F */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* NUL */
	0x3c, 0x42, 0xa5, 0x81, 0xa5, 0x99, 0x42, 0x3c, /* SOH */
	0x3c, 0x7e, 0xdb, 0xff, 0xff, 0xdb, 0x66, 0x3c, /* STX */
	0x6c, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x10, 0x00, /* ETX */
	0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x10, 0x00, /* EOT */
	0x10, 0x38, 0x54, 0xfe, 0x54, 0x10, 0x38, 0x00, /* ENQ */
	0x10, 0x38, 0x7c, 0xfe, 0xfe, 0x10, 0x38, 0x00, /* ACK */
	0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, /* BEL */
	0xff, 0xff, 0xff, 0xe7, 0xe7, 0xff, 0xff, 0xff, /* BS  */
	0x38, 0x44, 0x82, 0x82, 0x82, 0x44, 0x38, 0x00, /* HT  */
	0xc7, 0xbb, 0x7d, 0x7d, 0x7d, 0xbb, 0xc7, 0xff, /* LF  */
	0x0f, 0x03, 0x05, 0x79, 0x88, 0x88, 0x88, 0x70, /* VT  */
	0x38, 0x44, 0x44, 0x44, 0x38, 0x10, 0x7c, 0x10, /* FF  */
	0x30, 0x28, 0x24, 0x24, 0x28, 0x20, 0xe0, 0xc0, /* CR  */
	0x3c, 0x24, 0x3c, 0x24, 0x24, 0xe4, 0xdc, 0x18, /* SO  */
	0x10, 0x54, 0x38, 0xee, 0x38, 0x54, 0x10, 0x00, /* SI  */
	0x10, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x10, 0x10, /* DLE */
	0x10, 0x10, 0x10, 0xff, 0x00, 0x00, 0x00, 0x00, /* DC1 */
	0x00, 0x00, 0x00, 0xff, 0x10, 0x10, 0x10, 0x10, /* DC2 */
	0x10, 0x10, 0x10, 0xf0, 0x10, 0x10, 0x10, 0x10, /* DC3 */
	0x10, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x10, 0x10, /* DC4 */
	0x10, 0x10, 0x10, 0xff, 0x10, 0x10, 0x10, 0x10, /* NAK */
	0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, /* SYN */
	0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, /* ETB */
	0x00, 0x00, 0x00, 0x1f, 0x10, 0x10, 0x10, 0x10, /* CAN */
	0x00, 0x00, 0x00, 0xf0, 0x10, 0x10, 0x10, 0x10, /* EM  */
	0x10, 0x10, 0x10, 0x1f, 0x00, 0x00, 0x00, 0x00, /* SUB */
	0x10, 0x10, 0x10, 0xf0, 0x00, 0x00, 0x00, 0x00, /* ESC */
	0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81, /* FS  */
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, /* GS  */
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, /* RS  */
	0x00, 0x10, 0x10, 0xff, 0x10, 0x10, 0x00, 0x00, /* US  */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* (WHITESPACE) */
	0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x20, 0x00, /* ! */
	0x50, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, /* " */
	0x50, 0x50, 0xf8, 0x50, 0xf8, 0x50, 0x50, 0x00, /* # */
	0x20, 0x78, 0xa0, 0x70, 0x28, 0xf0, 0x20, 0x00, /* $ */
	0xc0, 0xc8, 0x10, 0x20, 0x40, 0x98, 0x18, 0x00, /* % */
	0x40, 0xa0, 0x40, 0xa8, 0x90, 0x98, 0x60, 0x00, /* & */
	0x10, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' */
	0x10, 0x20, 0x40, 0x40, 0x40, 0x20, 0x10, 0x00, /* ( */
	0x40, 0x20, 0x10, 0x10, 0x10, 0x20, 0x40, 0x00, /* ) */
	0x20, 0xa8, 0x70, 0x20, 0x70, 0xa8, 0x20, 0x00, /* * */
	0x00, 0x20, 0x20, 0xf8, 0x20, 0x20, 0x00, 0x00, /* + */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x40, /* , */
	0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, /* - */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00, /* . */
	0x00, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00, /* / */
	0x70, 0x88, 0x98, 0xa8, 0xc8, 0x88, 0x70, 0x00, /* 0 */
	0x20, 0x60, 0xa0, 0x20, 0x20, 0x20, 0xf8, 0x00, /* 1 */
	0x70, 0x88, 0x08, 0x10, 0x60, 0x80, 0xf8, 0x00, /* 2 */
	0x70, 0x88, 0x08, 0x30, 0x08, 0x88, 0x70, 0x00, /* 3 */
	0x10, 0x30, 0x50, 0x90, 0xf8, 0x10, 0x10, 0x00, /* 4 */
	0xf8, 0x80, 0xe0, 0x10, 0x08, 0x10, 0xe0, 0x00, /* 5 */
	0x30, 0x40, 0x80, 0xf0, 0x88, 0x88, 0x70, 0x00, /* 6 */
	0xf8, 0x88, 0x10, 0x20, 0x20, 0x20, 0x20, 0x00, /* 7 */
	0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70, 0x00, /* 8 */
	0x70, 0x88, 0x88, 0x78, 0x08, 0x10, 0x60, 0x00, /* 9 */
	0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x00, 0x00, /* : */
	0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x20, 0x40, /* ; */
	0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x00, /* < */
	0x00, 0x00, 0xf8, 0x00, 0xf8, 0x00, 0x00, 0x00, /* = */
	0xc0, 0x60, 0x30, 0x18, 0x30, 0x60, 0xc0, 0x00, /* > */
	0x70, 0x88, 0x08, 0x10, 0x20, 0x00, 0x20, 0x00, /* ? */
	0x70, 0x88, 0x08, 0x68, 0xa8, 0xa8, 0x70, 0x00, /* @ */
	0x70, 0x88, 0x88, 0x88, 0xf8, 0x88, 0x88, 0x00, /* A */
	0xf0, 0x88, 0x88, 0xf0, 0x88, 0x88, 0xf0, 0x00, /* B */
	0x78, 0x80, 0x80, 0x80, 0x80, 0x80, 0x78, 0x00, /* C */
	0xf0, 0x88, 0x88, 0x88, 0x88, 0x88, 0xf0, 0x00, /* D */
	0xf8, 0x80, 0x80, 0xf0, 0x80, 0x80, 0xf8, 0x00, /* E */
	0xf8, 0x80, 0x80, 0xf0, 0x80, 0x80, 0x80, 0x00, /* F */
	0x78, 0x80, 0x80, 0xb8, 0x88, 0x88, 0x78, 0x00, /* G */
	0x88, 0x88, 0x88, 0xf8, 0x88, 0x88, 0x88, 0x00, /* H */
	0x70, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00, /* I */
	0x38, 0x08, 0x08, 0x08, 0x08, 0x88, 0x70, 0x00, /* J */
	0x88, 0x90, 0xa0, 0xc0, 0xa0, 0x90, 0x88, 0x00, /* K */
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf8, 0x00, /* L */
	0x88, 0xd8, 0xa8, 0xa8, 0x88, 0x88, 0x88, 0x00, /* M */
	0x88, 0xc8, 0xc8, 0xa8, 0x98, 0x98, 0x88, 0x00, /* N */
	0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, /* O */
	0xf0, 0x88, 0x88, 0xf0, 0x80, 0x80, 0x80, 0x00, /* P */
	0x70, 0x88, 0x88, 0x88, 0xa8, 0x98, 0x78, 0x00, /* Q */
	0xf0, 0x88, 0x88, 0xf0, 0x88, 0x88, 0x88, 0x00, /* R */
	0x78, 0x80, 0x80, 0x70, 0x08, 0x08, 0xf0, 0x00, /* S */
	0xf8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, /* T */
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, /* U */
	0x88, 0x88, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00, /* V */
	0x88, 0x88, 0x88, 0xa8, 0xa8, 0xd8, 0x88, 0x00, /* W */
	0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00, /* X */
	0x88, 0x88, 0x88, 0x50, 0x20, 0x20, 0x20, 0x00, /* Y */
	0xf8, 0x08, 0x10, 0x20, 0x40, 0x80, 0xf8, 0x00, /* Z */
	0x70, 0x40, 0x40, 0x40, 0x40, 0x40, 0x70, 0x00, /* [ */
	0x00, 0x80, 0x40, 0x20, 0x10, 0x08, 0x00, 0x00, /* \ */
	0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x70, 0x00, /* ] */
	0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, /* ^ */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, /* _ */
	0x40, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, /* ` */
	0x00, 0x00, 0x78, 0x88, 0x88, 0x98, 0x68, 0x00, /* a */
	0x80, 0x80, 0xf0, 0x88, 0x88, 0x88, 0xf0, 0x00, /* b */
	0x00, 0x00, 0x78, 0x80, 0x80, 0x80, 0x78, 0x00, /* c */
	0x08, 0x08, 0x78, 0x88, 0x88, 0x88, 0x78, 0x00, /* d */
	0x00, 0x00, 0x70, 0x88, 0xf8, 0x80, 0x78, 0x00, /* e */
	0x18, 0x20, 0x20, 0xf8, 0x20, 0x20, 0x20, 0x00, /* f */
	0x00, 0x00, 0x78, 0x88, 0x88, 0x78, 0x08, 0x70, /* g */
	0x80, 0x80, 0xf0, 0x88, 0x88, 0x88, 0x88, 0x00, /* h */
	0x20, 0x00, 0x60, 0x20, 0x20, 0x20, 0x70, 0x00, /* i */
	0x08, 0x00, 0x18, 0x08, 0x08, 0x08, 0x88, 0x70, /* j */
	0x80, 0x80, 0xa0, 0xc0, 0xa0, 0x90, 0x88, 0x00, /* k */
	0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00, /* l */
	0x00, 0x00, 0xd0, 0xa8, 0xa8, 0xa8, 0xa8, 0x00, /* m */
	0x00, 0x00, 0xf0, 0x88, 0x88, 0x88, 0x88, 0x00, /* n */
	0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, /* o */
	0x00, 0x00, 0xf0, 0x88, 0x88, 0x88, 0xf0, 0x80, /* p */
	0x00, 0x00, 0x78, 0x88, 0x88, 0x88, 0x78, 0x08, /* q */
	0x00, 0x00, 0xb8, 0xc0, 0x80, 0x80, 0x80, 0x00, /* r */
	0x00, 0x00, 0x78, 0x80, 0x70, 0x08, 0xf0, 0x00, /* s */
	0x40, 0x40, 0xf8, 0x40, 0x40, 0x40, 0x38, 0x00, /* t */
	0x00, 0x00, 0x88, 0x88, 0x88, 0x98, 0x68, 0x00, /* u */
	0x00, 0x00, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00, /* v */
	0x00, 0x00, 0x88, 0xa8, 0xa8, 0xa8, 0x50, 0x00, /* w */
	0x00, 0x00, 0x88, 0x50, 0x20, 0x50, 0x88, 0x00, /* x */
	0x00, 0x00, 0x88, 0x88, 0x88, 0x78, 0x08, 0x70, /* y */
	0x00, 0x00, 0xf8, 0x10, 0x20, 0x40, 0xf8, 0x00, /* z */
	0x18, 0x20, 0x20, 0x40, 0x20, 0x20, 0x18, 0x00, /* { */
	0x20, 0x20, 0x20, 0x00, 0x20, 0x20, 0x20, 0x00, /* | */
	0xc0, 0x20, 0x20, 0x10, 0x20, 0x20, 0xc0, 0x00, /* } */
	0x40, 0xa8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, /* ~ */
	0x00, 0x00, 0x20, 0x50, 0xf8, 0x00, 0x00, 0x00,  /* (DEL) */
	
	/* 0x80-0xFF */
	0x00, 0x20, 0x70, 0xa8, 0x20, 0x20, 0x00, 0x00, /* PSP_CTRL_UP */
	0x00, 0x20, 0x10, 0xf8, 0x10, 0x20, 0x00, 0x00, /* PSP_CTRL_RIGHT */
	0x00, 0x20, 0x20, 0xa8, 0x70, 0x20, 0x00, 0x00, /* PSP_CTRL_DOWN */
	0x00, 0x20, 0x40, 0xf8, 0x40, 0x20, 0x00, 0x00, /* PSP_CTRL_LEFT */
	0x20, 0x20, 0x50, 0x50, 0x88, 0xf8, 0x00, 0x00, /* PSP_CTRL_TRIANGLE */
	0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, /* PSP_CTRL_CIRCLE */
	0x00, 0x88, 0x50, 0x20, 0x50, 0x88, 0x00, 0x00, /* PSP_CTRL_CROSS */
	0x00, 0xf8, 0x88, 0x88, 0x88, 0xf8, 0x00, 0x00, /* PSP_CTRL_SQUARE */
	0x20, 0x30, 0x28, 0x28, 0x20, 0x60, 0xe0, 0xc0, /* PSP_CTRL_NOTE */

};
