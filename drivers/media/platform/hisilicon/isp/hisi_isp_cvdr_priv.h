/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef __CVDR_DRV_PRIV_H__
#define __CVDR_DRV_PRIV_H__

/* Define the union U_CVDR_CFG */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    axiwrite_du_threshold : 6   ; /* [5..0]  */
		unsigned int    reserved_0             : 2   ; /* [7..6]  */
		unsigned int    du_threshold_reached  : 8   ; /* [15..8]  */
		unsigned int    max_axiread_id         : 5   ; /* [20..16]  */
		unsigned int    reserved_1             : 3   ; /* [23..21]  */
		unsigned int    max_axiwrite_id       : 5   ; /* [28..24]  */
		unsigned int    reserved_2             : 1   ; /* [29]  */
		unsigned int    force_rd_clk_on        : 1   ; /* [30]  */
		unsigned int    force_wr_clk_on        : 1   ; /* [31]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_CVDR_CFG;

/* Define the union U_CVDR_DEBUG_EN */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    wr_peak_en            : 1   ; /* [0]  */
		unsigned int    reserved_0            : 7   ; /* [7..1]  */
		unsigned int    rd_peak_en            : 1   ; /* [8]  */
		unsigned int    reserved_1            : 23  ; /* [31..9]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_CVDR_DEBUG_EN;

/* Define the union U_CVDR_DEBUG */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    wr_peak               : 8   ; /* [7..0]  */
		unsigned int    rd_peak               : 8   ; /* [15..8]  */
		unsigned int    reserved_0            : 16  ; /* [31..16]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_CVDR_DEBUG;

/* Define the union U_CVDR_WR_QOS_CFG */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    wr_qos_threshold_01_stop  : 4   ; /* [3..0]  */
		unsigned int    wr_qos_threshold_01_start : 4   ; /* [7..4]  */
		unsigned int    wr_qos_threshold_10_stop  : 4   ; /* [11..8]  */
		unsigned int    wr_qos_threshold_10_start : 4   ; /* [15..12]  */
		unsigned int    wr_qos_threshold_11_stop  : 4   ; /* [19..16]  */
		unsigned int    wr_qos_threshold_11_start : 4   ; /* [23..20]  */
		unsigned int    reserved_0            : 2   ; /* [25..24]  */
		unsigned int    wr_qos_min            : 2   ; /* [27..26]  */
		unsigned int    wr_qos_max            : 2   ; /* [29..28]  */
		unsigned int    wr_qos_sr             : 2   ; /* [31..30]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_CVDR_WR_QOS_CFG;

/* Define the union U_CVDR_RD_QOS_CFG */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    rd_qos_threshold_01_stop  : 4   ; /* [3..0]  */
		unsigned int    rd_qos_threshold_01_start : 4   ; /* [7..4]  */
		unsigned int    rd_qos_threshold_10_stop  : 4   ; /* [11..8]  */
		unsigned int    rd_qos_threshold_10_start : 4   ; /* [15..12]  */
		unsigned int    rd_qos_threshold_11_stop  : 4   ; /* [19..16]  */
		unsigned int    rd_qos_threshold_11_start : 4   ; /* [23..20]  */
		unsigned int    reserved_0            : 2   ; /* [25..24]  */
		unsigned int    rd_qos_min            : 2   ; /* [27..26]  */
		unsigned int    rd_qos_max            : 2   ; /* [29..28]  */
		unsigned int    rd_qos_sr             : 2   ; /* [31..30]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_CVDR_RD_QOS_CFG;

/* Define the union U_OTHER_RO */
typedef union {
	/* Define the struct bits  */
	struct {
		unsigned int other_ro               : 32  ; /* [31..0]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_CVDR_OTHER_RO;

/* Define the union U_OTHER_RW */
typedef union {
	/* Define the struct bits  */
	struct {
		unsigned int other_rw               : 32  ; /* [31..0]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_CVDR_OTHER_RW;


/* Define the union U_VP_WR_CFG */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    vpwr_pixel_format     : 4   ; /* [3..0]  */
		unsigned int    vpwr_pixel_expansion  : 1   ; /* [4]  */
		unsigned int    reserved_0            : 10  ; /* [14..5]  */
		unsigned int    vpwr_last_page        : 17  ; /* [31..15]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_VP_WR_CFG;

/* Define the union U_VP_WR_AXI_FS */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    reserved_0               : 4   ; /* [3..0]  */
		unsigned int    vpwr_address_frame_start : 28  ; /* [31..4]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_VP_WR_AXI_FS;

/* Define the union U_VP_WR_AXI_LINE */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    vpwr_line_stride      : 10  ; /* [9..0]  */
		unsigned int    reserved_0            : 5   ; /* [14..10]  */
		unsigned int    vpwr_line_wrap        : 14  ; /* [28..15]  */
		unsigned int    reserved_1            : 3   ; /* [31..29]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_VP_WR_AXI_LINE;

/* Define the union U_CVDR_RT_VP_WR_IF_CFG */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    reserved_0            : 16  ; /* [15..0]  */
		unsigned int    vp_wr_stop_enable_du_threshold_reached : 1   ; /* [16]  */
		unsigned int    vp_wr_stop_enable_flux_ctrl : 1   ; /* [17]  */
		unsigned int    vp_wr_stop_enable_pressure : 1   ; /* [18]  */
		unsigned int    reserved_1            : 5   ; /* [23..19]  */
		unsigned int    vp_wr_stop_ok      : 1   ; /* [24]  */
		unsigned int    vp_wr_stop         : 1   ; /* [25]  */
		unsigned int    reserved_2            : 5   ; /* [30..26]  */
		unsigned int    vpwr_prefetch_bypass : 1   ; /* [31]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_VP_WR_IF_CFG;


/* Define the union U_LIMITER_VP_WR */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    vpwr_access_limiter_0 : 4   ; /* [3..0]  */
		unsigned int    vpwr_access_limiter_1 : 4   ; /* [7..4]  */
		unsigned int    vpwr_access_limiter_2 : 4   ; /* [11..8]  */
		unsigned int    vpwr_access_limiter_3 : 4   ; /* [15..12]  */
		unsigned int    reserved_0            : 8   ; /* [23..16]  */
		unsigned int    vpwr_access_limiter_reload : 4   ; /* [27..24]  */
		unsigned int    reserved_1            : 4   ; /* [31..28]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_LIMITER_VP_WR;

/* Define the union U_VP_RD_CFG */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    vprd_pixel_format    : 4   ; /* [3..0]  */
		unsigned int    vprd_pixel_expansion : 1   ; /* [4]  */
		unsigned int    vprd_allocated_du    : 5   ; /* [9..5]  */
		unsigned int    reserved_0           : 5   ; /* [14..10]  */
		unsigned int    vprd_last_page       : 17  ; /* [31..15]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;

} U_VP_RD_CFG;

/* Define the union U_VP_RD_LWG */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    vprd_line_size           : 13  ; /* [12..0]  */
		unsigned int    reserved_0               : 3   ; /* [15..13]  */
		unsigned int    vprd_horizontal_blanking : 8   ; /* [23..16]  */
		unsigned int    reserved_1               : 8   ; /* [31..24]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_VP_RD_LWG;

/* Define the union U_VP_RD_FHG */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    vprd_frame_size        : 13  ; /* [12..0]  */
		unsigned int    reserved_0             : 3   ; /* [15..13]  */
		unsigned int    vprd_vertical_blanking : 8   ; /* [23..16]  */
		unsigned int    reserved_1             : 8   ; /* [31..24]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_VP_RD_FHG;

/* Define the union U_VP_RD_AXI_FS */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    reserved_0            : 4   ; /* [3..0]  */
		unsigned int    vprd_axi_frame_start  : 28  ; /* [31..4]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_VP_RD_AXI_FS;

/* Define the union U_VP_RD_AXI_LINE */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    vprd_line_stride   : 10  ; /* [9..0]  */
		unsigned int    reserved_0         : 6   ; /* [15..10]  */
		unsigned int    vprd_line_wrap     : 13  ; /* [28..16]  */
		unsigned int    reserved_1         : 3   ; /* [31..29]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_VP_RD_AXI_LINE;

/* Define the union U_VP_RD_IF_CFG */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    reserved_0            : 16  ; /* [15..0]  */
		unsigned int    vp_rd_stop_enable_du_threshold_reached : 1   ; /* [16]  */
		unsigned int    vp_rd_stop_enable_flux_ctrl : 1   ; /* [17]  */
		unsigned int    vp_rd_stop_enable_pressure : 1   ; /* [18]  */
		unsigned int    reserved_1            : 5   ; /* [23..19]  */
		unsigned int    vp_rd_stop_ok       : 1   ; /* [24]  */
		unsigned int    vp_rd_stop          : 1   ; /* [25]  */
		unsigned int    reserved_2            : 5   ; /* [30..26]  */
		unsigned int    vprd_prefetch_bypass : 1   ; /* [31]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_VP_RD_IF_CFG;

/* Define the union U_VP_RD_DEBUG */
typedef union {
	/* Define the struct bits  */
	struct {
		unsigned int vp_rd_debug         : 32  ; /* [31..0]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_VP_RD_DEBUG;

/* Define the union U_LIMITER_VP_RD */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    vprd_access_limiter_0 : 4   ; /* [3..0]  */
		unsigned int    vprd_access_limiter_1 : 4   ; /* [7..4]  */
		unsigned int    vprd_access_limiter_2 : 4   ; /* [11..8]  */
		unsigned int    vprd_access_limiter_3 : 4   ; /* [15..12]  */
		unsigned int    reserved_0            : 8   ; /* [23..16]  */
		unsigned int    vprd_access_limiter_reload : 4   ; /* [27..24]  */
		unsigned int    reserved_1            : 4   ; /* [31..28]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_LIMITER_VP_RD;

/* Define the union U_NR_WR_CFG */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    reserved_0            : 16  ; /* [15..0]  */
		unsigned int    nr_wr_stop_enable_du_threshold_reached : 1   ; /* [16]  */
		unsigned int    nr_wr_stop_enable_flux_ctrl : 1   ; /* [17]  */
		unsigned int    nr_wr_stop_enable_pressure : 1   ; /* [18]  */
		unsigned int    reserved_1            : 5   ; /* [23..19]  */
		unsigned int    nr_wr_stop_ok       : 1   ; /* [24]  */
		unsigned int    nr_wr_stop          : 1   ; /* [25]  */
		unsigned int    reserved_2            : 5   ; /* [30..26]  */
		unsigned int    nrwr_enable         : 1   ; /* [31]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_NR_WR_CFG;

/* Define the union U_NR_WR_DEBUG */
typedef union {
	/* Define the struct bits  */
	struct {
		unsigned int nr_wr_debug          : 32  ; /* [31..0]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_NR_WR_DEBUG;

/* Define the union U_LIMITER_NR_WR */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    nrwr_access_limiter_0 : 4   ; /* [3..0]  */
		unsigned int    nrwr_access_limiter_1 : 4   ; /* [7..4]  */
		unsigned int    nrwr_access_limiter_2 : 4   ; /* [11..8]  */
		unsigned int    nrwr_access_limiter_3 : 4   ; /* [15..12]  */
		unsigned int    reserved_0            : 8   ; /* [23..16]  */
		unsigned int    nrwr_access_limiter_reload : 4   ; /* [27..24]  */
		unsigned int    reserved_1            : 4   ; /* [31..28]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_LIMITER_NR_WR;

/* Define the union U_NR_RD_CFG */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    reserved_0            : 5   ; /* [4..0]  */
		unsigned int    nrrd_allocated_du   : 5   ; /* [9..5]  */
		unsigned int    reserved_1            : 6   ; /* [15..10]  */
		unsigned int    nr_rd_stop_enable_du_threshold_reached : 1   ; /* [16]  */
		unsigned int    nr_rd_stop_enable_flux_ctrl : 1   ; /* [17]  */
		unsigned int    nr_rd_stop_enable_pressure : 1   ; /* [18]  */
		unsigned int    reserved_2            : 5   ; /* [23..19]  */
		unsigned int    nr_rd_stop_ok       : 1   ; /* [24]  */
		unsigned int    nr_rd_stop          : 1   ; /* [25]  */
		unsigned int    reserved_3            : 5   ; /* [30..26]  */
		unsigned int    nrrd_enable         : 1   ; /* [31]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_NR_RD_CFG;

/* Define the union U_NR_RD_DEBUG */
typedef union {
	/* Define the struct bits  */
	struct {
		unsigned int nr_rd_debug          : 32  ; /* [31..0]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_NR_RD_DEBUG;

/* Define the union U_LIMITER_NR_RD */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    nrrd_access_limiter_0 : 4   ; /* [3..0]  */
		unsigned int    nrrd_access_limiter_1 : 4   ; /* [7..4]  */
		unsigned int    nrrd_access_limiter_2 : 4   ; /* [11..8]  */
		unsigned int    nrrd_access_limiter_3 : 4   ; /* [15..12]  */
		unsigned int    reserved_0            : 8   ; /* [23..16]  */
		unsigned int    nrrd_access_limiter_reload : 4   ; /* [27..24]  */
		unsigned int    reserved_1            : 4   ; /* [31..28]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_LIMITER_NR_RD;

/* Define the union U_DEBUG */
typedef union {
	/* Define the struct bits  */
	struct {
		unsigned int debug                : 32  ; /* [31..0]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_DEBUG;

/* Define the union U_AXI_CFG_NR_WR */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    nr_wr_mid             : 6   ; /* [5..0]  */
		unsigned int    reserved_0            : 26  ; /* [31..6]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;
} U_AXI_CFG_NR_WR;

/* Define the union U_AXI_CFG_NR_RD */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    nr_rd_mid             : 6   ; /* [5..0]  */
		unsigned int    reserved_0            : 26  ; /* [31..6]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;

} U_AXI_CFG_NR_RD;

/* Define the union U_AXI_CFG_VP_WR */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    vp_wr_mid             : 6   ; /* [5..0]  */
		unsigned int    reserved_0            : 26  ; /* [31..6]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;

} U_AXI_CFG_VP_WR;

/* Define the union U_AXI_CFG_VP_RD */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    vp_rd_mid             : 6   ; /* [5..0]  */
		unsigned int    reserved_0            : 26  ; /* [31..6]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;

} U_AXI_CFG_VP_RD;

/* Define the union U_SPARE */
typedef union {
	/* Define the struct bits  */
	struct {
		unsigned int spare                : 32  ; /* [31..0]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;

} U_SPARE;

/* Define the union U_VP_WR_SMOOTHING */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    vpwr_smoothing_access_limiter_0 : 4   ; /* [3..0]  */
		unsigned int    vpwr_smoothing_access_limiter_1 : 4   ; /* [7..4]  */
		unsigned int    vpwr_smoothing_access_limiter_2 : 4   ; /* [11..8]  */
		unsigned int    vpwr_smoothing_access_limiter_3 : 4   ; /* [15..12]  */
		unsigned int    vpwr_smoothing_threshold        : 7   ; /* [22..16]  */
		unsigned int    reserved_0                      : 9   ; /* [31..23]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;

} U_VP_WR_SMOOTHING;

/* Define the union U_VP_WR_DEBUG */
typedef union {
	/* Define the struct bits  */
	struct {
		unsigned int vp_wr_debug         : 32  ; /* [31..0]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;

} U_VP_WR_DEBUG;

typedef struct {
	U_VP_WR_CFG            VP_WR_CFG           ; /* 0x1c~0x26c */
	U_VP_WR_AXI_FS         VP_WR_AXI_FS        ; /* 0x20~0x270 */
	U_VP_WR_AXI_LINE       VP_WR_AXI_LINE      ; /* 0x24~0x274 */
	U_VP_WR_IF_CFG          VP_WR_IF_CFG      ; /* 0x28~0x278 */
} S_VP_WR;

typedef struct {
	U_VP_RD_CFG            VP_RD_CFG            ; /* 0x500~0x7a0 */
	U_VP_RD_LWG            VP_RD_LWG            ; /* 0x504~0x7a4 */
	U_VP_RD_FHG            VP_RD_FHG            ; /* 0x508~0x7a8 */
	U_VP_RD_AXI_FS         VP_RD_AXI_FS         ; /* 0x50c~0x7ac */
	U_VP_RD_AXI_LINE       VP_RD_AXI_LINE       ; /* 0x510~0x7b0 */
	U_VP_RD_IF_CFG         VP_RD_IF_CFG         ; /* 0x514~0x7b4 */
	unsigned int           VP_RD_DEBUG          ; /* 0x51c~0x7bc */
} S_VP_RD;

typedef struct {
	U_NR_WR_CFG            NR_WR_CFG            ;/* 0x900 */
	U_NR_WR_DEBUG          NR_WR_DEBUG          ;/* 0x904 */
	U_LIMITER_NR_WR        LIMITER_NR_WR        ;/* 0x908 */
	unsigned int           reserved_0           ;/* 0x90c */
} S_NR_WR;

typedef struct {
	U_NR_RD_CFG            NR_RD_CFG             ; /* 0xa00 */
	U_NR_RD_DEBUG          NR_RD_DEBUG           ; /* 0xa04 */
	U_LIMITER_NR_RD        LIMITER_NR_RD         ; /* 0xa08 */
	unsigned int           reserved_0            ; /* 0xa0c */
} S_NR_RD;

/* Define the global struct */
typedef struct {
	U_CVDR_CFG          CVDR_CFG             ; /* 0x0 */
	U_CVDR_DEBUG_EN     CVDR_DEBUG_EN        ; /* 0x4 */
	U_CVDR_DEBUG        CVDR_DEBUG           ; /* 0x8 */
	U_CVDR_WR_QOS_CFG   CVDR_WR_QOS_CFG      ; /* 0xc */
	U_CVDR_RD_QOS_CFG   CVDR_RD_QOS_CFG      ; /* 0x10 */
	U_CVDR_OTHER_RO     CVDR_OTHER_RO        ; /* 0x14 */
	U_CVDR_OTHER_RW     CVDR_OTHER_RW        ; /* 0x18 */
	S_VP_WR             VP_WR[38]            ; /* 0x1c~0x278 */
	unsigned int        reserved_0[73]       ; /* 0x27c~0x39c */
	U_LIMITER_VP_WR     LIMITER_VP_WR[38]    ; /* 0x400~0x494 */
	unsigned int        reserved_1[2]        ; /* 0x498~0x49c */
	S_VP_RD             VP_RD[22]            ; /* 0x500~0x7bc */
	unsigned int        reserved_2[48]       ; /* 0x7c0~0x87c */
	U_LIMITER_VP_RD     LIMITER_VP_RD[22]    ; /* 0x880~0x8d4 */
	unsigned int        reserved_3[10]       ; /* 0x8d8~0x8fc */
	S_NR_WR             NR_WR[4]             ; /* 0x900~0x93c */
	unsigned int        reserved_4[48]       ; /* 0x940~0x9fc */
	S_NR_RD            NR_RD[8]            ; /* 0xa00~0xa7c */
	unsigned int        reserved_5[32]       ; /* 0xa80~0xafc */
	U_DEBUG             DEBUG[16]            ; /* 0xb00~0xb3c */
	unsigned int        reserved_6[48]       ; /* 0xb40~0xbfc */
	U_AXI_CFG_NR_WR     AXI_CFG_NR_WR[4]     ; /* 0xc00~0xc0c */
	unsigned int        reserved_7[12]       ; /* 0xc10~0xc3c */
	U_AXI_CFG_NR_RD     AXI_CFG_NR_RD[8]     ; /* 0xc40~0xc5c */
	unsigned int        reserved_8[8]        ; /* 0xc60~0xc7c */
	U_AXI_CFG_VP_WR     AXI_CFG_VP_WR[38]    ; /* 0xc80~0xd14 */
	unsigned int        reserved_9[26]       ; /* 0xd18~0xd7c */
	U_AXI_CFG_VP_RD     AXI_CFG_VP_RD[22]    ; /* 0xd80~0xdd4 */
	unsigned int        reserved_10[10]      ; /* 0xdd8~0xdfc */
	U_SPARE             SPARE[4]             ; /* 0xe00~0xe0c */
	U_VP_WR_SMOOTHING   VP_WR_SMOOTHING[20]  ; /* 0xe10~0xe5c */
	unsigned int        reserved_11[40]      ; /* 0xe60~0xefc */
	U_VP_WR_DEBUG       VP_WR_DEBUG[38]      ; /* 0xf00~0xf94 */
} S_CVDR_REGS_TYPE;

#endif /* __CVDR_DRV_PRIV_H__ */
