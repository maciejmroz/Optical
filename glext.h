/***************************************************************************
                          glext.h  -  description
                             -------------------
    begin                : Fri May 5 2000 (modified to support other 
	                       extensions)
    copyright            : Silicon Graphics
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/

#ifndef __glext_h_
#define __glext_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
** Copyright 1992-1999 Silicon Graphics, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of Silicon Graphics, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
*/

#ifndef APIENTRY
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

//GL_EXT_texture_edge_clamp
#define GL_CLAMP_TO_EDGE_EXT                0x812F

#define GL_ARB_multitexture                 1
#define GL_EXT_abgr                         1
#define GL_EXT_bgra                         1
#define GL_EXT_clip_volume_hint             1
#define GL_EXT_compiled_vertex_array        1
#define GL_EXT_cull_vertex                  1
#define GL_EXT_packed_pixels                1
#define GL_EXT_point_parameters             1
#define GL_EXT_stencil_wrap                 1
#define GL_EXT_texture_env_add              1
#define GL_EXT_texture_env_combine          1
#define GL_EXT_vertex_array                 1
#define GL_NV_texgen_reflection             1
#define GL_NV_texture_env_combine4          1
#define GL_WIN_swap_hint                    1

/* Moje wlasne zmiany: */

#define GL_EXT_fog_coord					1
#define GL_EXT_rescale_normal				1
#define GL_EXT_secondary_color				1
#define GL_EXT_separate_specular_color		1
#define GL_EXT_texture_lod_bias				1
#define GL_EXT_vertex_weighting				1
#define GL_NV_fog_distance					1
#define GL_NV_light_max_exponent			1

/* EXT_abgr */
#define GL_ABGR_EXT                         0x8000

/* EXT_packed_pixels */
#define GL_UNSIGNED_BYTE_3_3_2_EXT          0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4_EXT       0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1_EXT       0x8034
#define GL_UNSIGNED_INT_8_8_8_8_EXT         0x8035
#define GL_UNSIGNED_INT_10_10_10_2_EXT      0x8036

/* EXT_vertex_array */
#define GL_VERTEX_ARRAY_EXT                 0x8074
#define GL_NORMAL_ARRAY_EXT                 0x8075
#define GL_COLOR_ARRAY_EXT                  0x8076
#define GL_INDEX_ARRAY_EXT                  0x8077
#define GL_TEXTURE_COORD_ARRAY_EXT          0x8078
#define GL_EDGE_FLAG_ARRAY_EXT              0x8079
#define GL_VERTEX_ARRAY_SIZE_EXT            0x807A
#define GL_VERTEX_ARRAY_TYPE_EXT            0x807B
#define GL_VERTEX_ARRAY_STRIDE_EXT          0x807C
#define GL_VERTEX_ARRAY_COUNT_EXT           0x807D
#define GL_NORMAL_ARRAY_TYPE_EXT            0x807E
#define GL_NORMAL_ARRAY_STRIDE_EXT          0x807F
#define GL_NORMAL_ARRAY_COUNT_EXT           0x8080
#define GL_COLOR_ARRAY_SIZE_EXT             0x8081
#define GL_COLOR_ARRAY_TYPE_EXT             0x8082
#define GL_COLOR_ARRAY_STRIDE_EXT           0x8083
#define GL_COLOR_ARRAY_COUNT_EXT            0x8084
#define GL_INDEX_ARRAY_TYPE_EXT             0x8085
#define GL_INDEX_ARRAY_STRIDE_EXT           0x8086
#define GL_INDEX_ARRAY_COUNT_EXT            0x8087
#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT     0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT     0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT   0x808A
#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT    0x808B
#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT       0x808C
#define GL_EDGE_FLAG_ARRAY_COUNT_EXT        0x808D
#define GL_VERTEX_ARRAY_POINTER_EXT         0x808E
#define GL_NORMAL_ARRAY_POINTER_EXT         0x808F
#define GL_COLOR_ARRAY_POINTER_EXT          0x8090
#define GL_INDEX_ARRAY_POINTER_EXT          0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT  0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER_EXT      0x8093

/* EXT_bgra */
#define GL_BGR_EXT                          0x80E0
#define GL_BGRA_EXT                         0x80E1

/* EXT_clip_volume_hint */
#define GL_CLIP_VOLUME_CLIPPING_HINT_EXT    0x80F0

/* EXT_point_parameters */
#define GL_POINT_SIZE_MIN_EXT               0x8126
#define GL_POINT_SIZE_MAX_EXT               0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_EXT    0x8128
#define GL_DISTANCE_ATTENUATION_EXT         0x8129

/* EXT_compiled_vertex_array */
#define GL_ARRAY_ELEMENT_LOCK_FIRST_EXT     0x81A8
#define GL_ARRAY_ELEMENT_LOCK_COUNT_EXT     0x81A9

/* EXT_cull_vertex */
#define GL_CULL_VERTEX_EXT                  0x81AA
#define GL_CULL_VERTEX_EYE_POSITION_EXT     0x81AB
#define GL_CULL_VERTEX_OBJECT_POSITION_EXT  0x81AC

/* ARB_multitexture */
#define GL_ACTIVE_TEXTURE_ARB               0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB            0x84E2
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define GL_TEXTURE4_ARB                     0x84C4
#define GL_TEXTURE5_ARB                     0x84C5
#define GL_TEXTURE6_ARB                     0x84C6
#define GL_TEXTURE7_ARB                     0x84C7
#define GL_TEXTURE8_ARB                     0x84C8
#define GL_TEXTURE9_ARB                     0x84C9
#define GL_TEXTURE10_ARB                    0x84CA
#define GL_TEXTURE11_ARB                    0x84CB
#define GL_TEXTURE12_ARB                    0x84CC
#define GL_TEXTURE13_ARB                    0x84CD
#define GL_TEXTURE14_ARB                    0x84CE
#define GL_TEXTURE15_ARB                    0x84CF
#define GL_TEXTURE16_ARB                    0x84D0
#define GL_TEXTURE17_ARB                    0x84D1
#define GL_TEXTURE18_ARB                    0x84D2
#define GL_TEXTURE19_ARB                    0x84D3
#define GL_TEXTURE20_ARB                    0x84D4
#define GL_TEXTURE21_ARB                    0x84D5
#define GL_TEXTURE22_ARB                    0x84D6
#define GL_TEXTURE23_ARB                    0x84D7
#define GL_TEXTURE24_ARB                    0x84D8
#define GL_TEXTURE25_ARB                    0x84D9
#define GL_TEXTURE26_ARB                    0x84DA
#define GL_TEXTURE27_ARB                    0x84DB
#define GL_TEXTURE28_ARB                    0x84DC
#define GL_TEXTURE29_ARB                    0x84DD
#define GL_TEXTURE30_ARB                    0x84DE
#define GL_TEXTURE31_ARB                    0x84DF

/* EXT_stencil_wrap */
#define GL_INCR_WRAP_EXT                    0x8507
#define GL_DECR_WRAP_EXT                    0x8508

/* NV_texgen_reflection */
#define GL_NORMAL_MAP_NV                    0x8511
#define GL_REFLECTION_MAP_NV                0x8512

/* EXT_texture_env_combine */
#define GL_COMBINE_EXT                      0x8570
#define GL_COMBINE_RGB_EXT                  0x8571
#define GL_COMBINE_ALPHA_EXT                0x8572
#define GL_RGB_SCALE_EXT                    0x8573
#define GL_ADD_SIGNED_EXT                   0x8574
#define GL_INTERPOLATE_EXT                  0x8575
#define GL_CONSTANT_EXT                     0x8576
#define GL_PRIMARY_COLOR_EXT                0x8577
#define GL_PREVIOUS_EXT                     0x8578
#define GL_SOURCE0_RGB_EXT                  0x8580
#define GL_SOURCE1_RGB_EXT                  0x8581
#define GL_SOURCE2_RGB_EXT                  0x8582
#define GL_SOURCE0_ALPHA_EXT                0x8588
#define GL_SOURCE1_ALPHA_EXT                0x8589
#define GL_SOURCE2_ALPHA_EXT                0x858A
#define GL_OPERAND0_RGB_EXT                 0x8590
#define GL_OPERAND1_RGB_EXT                 0x8591
#define GL_OPERAND2_RGB_EXT                 0x8592
#define GL_OPERAND0_ALPHA_EXT               0x8598
#define GL_OPERAND1_ALPHA_EXT               0x8599
#define GL_OPERAND2_ALPHA_EXT               0x859A

/* NV_texture_env_combine4 */
#define GL_COMBINE4_NV                      0x8503
#define GL_SOURCE3_RGB_NV                   0x8583
#define GL_SOURCE3_ALPHA_NV                 0x858B
#define GL_OPERAND3_RGB_NV                  0x8593
#define GL_OPERAND3_ALPHA_NV                0x859B

/* Moje wlasne zmiany: */

/* EXT_fog_coord */
#define GL_FOG_COORDINATE_SOURCE_EXT		0x8450
#define GL_FOG_COORDINATE_EXT				0x8451
#define GL_FRAGMENT_DEPTH_EXT				0x8452
#define GL_CURRENT_FOG_COORDINATE_EXT		0x8453
#define GL_FOG_COORDINATE_ARRAY_TYPE_EXT	0x8454
#define GL_FOG_COORDINATE_ARRAY_STRIDE_EXT	0x8455
#define GL_FOG_COORDINATE_ARRAY_POINTER_EXT 0x8456
#define GL_FOG_COORDINATE_ARRAY_EXT			0x8457

/* EXT_rescale_normal */
#define GL_RESCALE_NORMAL_EXT				0x803A

/* EXT_secondary_color */
#define GL_COLOR_SUM_EXT					0x8458
#define GL_CURRENT_SECONDARY_COLOR_EXT		0x8459
#define GL_SECONDARY_COLOR_ARRAY_SIZE_EXT	0x845A
#define GL_SECONDARY_COLOR_ARRAY_TYPE_EXT	0x845B
#define GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT 0x845C
#define GL_SECONDARY_COLOR_ARRAY_POINTER_EXT 0x845D
#define GL_SECONDARY_COLOR_ARRAY_EXT		0x845E

/* EXT_separate_specular_color */
#define GL_LIGHT_MODEL_COLOR_CONTROL_EXT	0x81F8
#define GL_SINGLE_COLOR_EXT					0x81F9
#define GL_SEPARATE_SPECULAR_COLOR_EXT		0x81FA

/* EXT_texture_lod_bias */
#define GL_TEXTURE_FILTER_CONTROL_EXT		0x8500
#define GL_TEXTURE_LOD_BIAS_EXT				0x8501
#define GL_MAX_TEXTURE_LOD_BIAS_EXT			0x84FD

/* EXT_vertex_weighting */
#define GL_VERTEX_WEIGHTING_EXT				0x8509
#define GL_MODELVIEW0_EXT					0x1700  //(alias to MODELVIEW enumerant)
#define GL_MODELVIEW1_EXT					0x850A
#define GL_CURRENT_VERTEX_WEIGHT_EXT		0x850B
#define GL_VERTEX_WEIGHT_ARRAY_EXT			0x850C
#define GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT		0x850D
#define GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT		0x850E
#define GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT	0x850F
#define GL_MODELVIEW0_STACK_DEPTH_EXT		0x0BA3 //(alias to MODELVIEW_STACK_DEPTH)
#define GL_MODELVIEW1_STACK_DEPTH_EXT		0x8502
#define GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT	0x8510

/* NV_fog_distance */
#define GL_FOG_DISTANCE_MODE_NV				0x855A
#define GL_EYE_RADIAL_NV					0x855B
#define GL_EYE_PLANE_ABSOLUTE_NV			0x855C

/* NV_light_max_exponent */
#define GL_MAX_SHININESS_NV					0x8507
#define GL_MAX_SPOT_EXPONENT_NV				0x8508

/* NV_vertex_array_range */
#define GL_VERTEX_ARRAY_RANGE_NV			0x851D
#define GL_VERTEX_ARRAY_RANGE_LENGTH_NV		0x851E
#define GL_VERTEX_ARRAY_RANGE_VALID_NV		0x851F
#define GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV 0x8520
#define GL_VERTEX_ARRAY_RANGE_POINTER_NV	0x8521

/* ARB_texture_cube_map */
#define GL_NORMAL_MAP_ARB					0x8511
#define GL_REFLECTION_MAP_ARB				0x8512
#define GL_TEXTURE_CUBE_MAP_ARB				0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_ARB		0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB	0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB	0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB	0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB	0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB	0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB	0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARB		0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB	0x851C

/* NV_register_combiners */
#define GL_REGISTER_COMBINERS_NV			0x8522
#define GL_COMBINER0_NV						0x8550
#define GL_COMBINER1_NV						0x8551
#define GL_COMBINER2_NV						0x8552
#define GL_COMBINER3_NV						0x8553
#define GL_COMBINER4_NV						0x8554
#define GL_COMBINER5_NV						0x8555
#define GL_COMBINER6_NV						0x8556
#define GL_COMBINER7_NV						0x8557
#define GL_VARIABLE_A_NV					0x8523
#define GL_VARIABLE_B_NV					0x8524
#define GL_VARIABLE_C_NV					0x8525
#define GL_VARIABLE_D_NV					0x8526
#define GL_VARIABLE_E_NV					0x8527
#define GL_VARIABLE_F_NV					0x8528
#define GL_VARIABLE_G_NV					0x8529
#define GL_CONSTANT_COLOR0_NV				0x852A
#define GL_CONSTANT_COLOR1_NV				0x852B
#define GL_PRIMARY_COLOR_NV					0x852C
#define GL_SECONDARY_COLOR_NV				0x852D
#define GL_SPARE0_NV						0x852E
#define GL_SPARE1_NV						0x852F
#define GL_UNSIGNED_IDENTITY_NV				0x8536
#define GL_UNSIGNED_INVERT_NV				0x8537
#define GL_EXPAND_NORMAL_NV					0x8538
#define GL_EXPAND_NEGATE_NV					0x8539
#define GL_HALF_BIAS_NORMAL_NV				0x853A
#define GL_HALF_BIAS_NEGATE_NV				0x853B
#define GL_SIGNED_IDENTITY_NV				0x853C
#define GL_SIGNED_NEGATE_NV					0x853D
#define GL_E_TIMES_F_NV						0x8531
#define GL_SPARE0_PLUS_SECONDARY_COLOR_NV	0x8532
#define GL_SCALE_BY_TWO_NV					0x853E
#define GL_SCALE_BY_FOUR_NV					0x853F
#define GL_SCALE_BY_ONE_HALF_NV				0x8540
#define GL_BIAS_BY_NEGATIVE_ONE_HALF_NV		0x8541
#define GL_DISCARD_NV						0x8530
#define GL_COMBINER_INPUT_NV				0x8542
#define GL_COMBINER_MAPPING_NV				0x8543
#define GL_COMBINER_COMPONENT_USAGE_NV		0x8544
#define GL_COMBINER_AB_DOT_PRODUCT_NV		0x8545
#define GL_COMBINER_CD_DOT_PRODUCT_NV		0x8546
#define GL_COMBINER_MUX_SUM_NV				0x8547
#define GL_COMBINER_SCALE_NV				0x8548
#define GL_COMBINER_BIAS_NV					0x8549
#define GL_COMBINER_AB_OUTPUT_NV			0x854A
#define GL_COMBINER_CD_OUTPUT_NV			0x854B
#define GL_COMBINER_SUM_OUTPUT_NV			0x854C
#define GL_NUM_GENERAL_COMBINERS_NV			0x854E
#define GL_COLOR_SUM_CLAMP_NV				0x854F
#define GL_MAX_GENERAL_COMBINERS_NV			0x854D

//WGL_ARB_pixel_format
#define WGL_NUMBER_PIXEL_FORMATS_ARB		0x2000
#define WGL_DRAW_TO_WINDOW_ARB				0x2001
#define WGL_DRAW_TO_BITMAP_ARB				0x2002
#define WGL_ACCELERATION_ARB				0x2003
#define WGL_NEED_PALETTE_ARB				0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB			0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB			0x2006
#define WGL_SWAP_METHOD_ARB					0x2007
#define WGL_NUMBER_OVERLAYS_ARB				0x2008
#define WGL_NUMBER_UNDERLAYS_ARB			0x2009
#define WGL_TRANSPARENT_ARB					0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB		0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB		0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB		0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB		0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB		0x203B
#define WGL_SHARE_DEPTH_ARB					0x200C
#define WGL_SHARE_STENCIL_ARB				0x200D
#define WGL_SHARE_ACCUM_ARB					0x200E
#define WGL_SUPPORT_GDI_ARB					0x200F
#define WGL_SUPPORT_OPENGL_ARB				0x2010
#define WGL_DOUBLE_BUFFER_ARB				0x2011
#define WGL_STEREO_ARB						0x2012
#define WGL_PIXEL_TYPE_ARB					0x2013
#define WGL_COLOR_BITS_ARB					0x2014
#define WGL_RED_BITS_ARB					0x2015
#define WGL_RED_SHIFT_ARB					0x2016
#define WGL_GREEN_BITS_ARB					0x2017
#define WGL_GREEN_SHIFT_ARB					0x2018
#define WGL_BLUE_BITS_ARB					0x2019
#define WGL_BLUE_SHIFT_ARB					0x201A
#define WGL_ALPHA_BITS_ARB					0x201B
#define WGL_ALPHA_SHIFT_ARB					0x201C
#define WGL_ACCUM_BITS_ARB					0x201D
#define WGL_ACCUM_RED_BITS_ARB				0x201E
#define WGL_ACCUM_GREEN_BITS_ARB			0x201F
#define WGL_ACCUM_BLUE_BITS_ARB				0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB			0x2021
#define WGL_DEPTH_BITS_ARB					0x2022
#define WGL_STENCIL_BITS_ARB				0x2023
#define WGL_AUX_BUFFERS_ARB					0x2024
#define WGL_NO_ACCELERATION_ARB				0x2025
#define WGL_GENERIC_ACCELERATION_ARB		0x2026
#define WGL_FULL_ACCELERATION_ARB			0x2027
#define WGL_SWAP_EXCHANGE_ARB				0x2028
#define WGL_SWAP_COPY_ARB					0x2029
#define WGL_SWAP_UNDEFINED_ARB				0x202A
#define WGL_TYPE_RGBA_ARB					0x202B
#define WGL_TYPE_COLORINDEX_ARB				0x202C

//WGL_ARB_pbuffer
#define WGL_DRAW_TO_PBUFFER_ARB				0x202D
#define WGL_DRAW_TO_PBUFFER_ARB				0x202D
#define WGL_MAX_PBUFFER_PIXELS_ARB			0x202E
#define WGL_MAX_PBUFFER_WIDTH_ARB			0x202F
#define WGL_MAX_PBUFFER_HEIGHT_ARB			0x2030
#define WGL_PBUFFER_LARGEST_ARB				0x2033
#define WGL_PBUFFER_WIDTH_ARB				0x2034
#define WGL_PBUFFER_HEIGHT_ARB				0x2035
#define WGL_PBUFFER_LOST_ARB				0x2036

//ARB_texture_compession
#define GL_COMPRESSED_ALPHA_ARB					0x84E9
#define GL_COMPRESSED_LUMINANCE_ARB				0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB		0x84EB
#define GL_COMPRESSED_INTENSITY_ARB				0x84EC
#define GL_COMPRESSED_RGB_ARB					0x84ED
#define GL_COMPRESSED_RGBA_ARB					0x84EE
#define GL_TEXTURE_COMPRESSION_HINT_ARB			0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB	0x86A0
#define GL_TEXTURE_COMPRESSED_ARB				0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB	0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB		0x86A3

//ARB_texture_env_dot3
#define GL_DOT3_RGB_ARB							0x8740
#define GL_DOT3_RGBA_ARB						0x8741

//ATI_fragment_shader
#define GL_FRAGMENT_SHADER_ATI						0x8920
#define GL_REG_0_ATI								0x8921
#define GL_REG_1_ATI								0x8922
#define GL_REG_2_ATI								0x8923
#define GL_REG_3_ATI								0x8924
#define GL_REG_4_ATI								0x8925
#define GL_REG_5_ATI								0x8926
#define GL_REG_6_ATI								0x8927
#define GL_REG_7_ATI								0x8928
#define GL_REG_8_ATI								0x8929
#define GL_REG_9_ATI								0x892A
#define GL_REG_10_ATI								0x892B
#define GL_REG_11_ATI								0x892C
#define GL_REG_12_ATI								0x892D
#define GL_REG_13_ATI								0x892E
#define GL_REG_14_ATI								0x892F
#define GL_REG_15_ATI								0x8930
#define GL_REG_16_ATI								0x8931
#define GL_REG_17_ATI								0x8932
#define GL_REG_18_ATI								0x8933
#define GL_REG_19_ATI								0x8934
#define GL_REG_20_ATI								0x8935
#define GL_REG_21_ATI								0x8936
#define GL_REG_22_ATI								0x8937
#define GL_REG_23_ATI								0x8938
#define GL_REG_24_ATI								0x8939
#define GL_REG_25_ATI								0x893A
#define GL_REG_26_ATI								0x893B
#define GL_REG_27_ATI								0x893C
#define GL_REG_28_ATI								0x893D
#define GL_REG_29_ATI								0x893E
#define GL_REG_30_ATI								0x893F
#define GL_REG_31_ATI								0x8940
#define GL_CON_0_ATI								0x8941
#define GL_CON_1_ATI								0x8942
#define GL_CON_2_ATI								0x8943
#define GL_CON_3_ATI								0x8944
#define GL_CON_4_ATI								0x8945
#define GL_CON_5_ATI								0x8946
#define GL_CON_6_ATI								0x8947
#define GL_CON_7_ATI								0x8948
#define GL_CON_8_ATI								0x8949
#define GL_CON_9_ATI								0x894A
#define GL_CON_10_ATI								0x894B
#define GL_CON_11_ATI								0x894C
#define GL_CON_12_ATI								0x894D
#define GL_CON_13_ATI								0x894E
#define GL_CON_14_ATI								0x894F
#define GL_CON_15_ATI								0x8950
#define GL_CON_16_ATI								0x8951
#define GL_CON_17_ATI								0x8952
#define GL_CON_18_ATI								0x8953
#define GL_CON_19_ATI								0x8954
#define GL_CON_20_ATI								0x8955
#define GL_CON_21_ATI								0x8956
#define GL_CON_22_ATI								0x8957
#define GL_CON_23_ATI								0x8958
#define GL_CON_24_ATI								0x8959
#define GL_CON_25_ATI								0x895A
#define GL_CON_26_ATI								0x895B
#define GL_CON_27_ATI								0x895C
#define GL_CON_28_ATI								0x895D
#define GL_CON_29_ATI								0x895E
#define GL_CON_30_ATI								0x895F
#define GL_CON_31_ATI								0x8960
#define GL_MOV_ATI									0x8961
#define GL_ADD_ATI									0x8963
#define GL_MUL_ATI									0x8964
#define GL_SUB_ATI									0x8965
#define GL_DOT3_ATI									0x8966
#define GL_DOT4_ATI									0x8967
#define GL_MAD_ATI									0x8968
#define GL_LERP_ATI									0x8969
#define GL_CND_ATI									0x896A
#define GL_CND0_ATI									0x896B
#define GL_DOT2_ADD_ATI								0x896C
#define GL_SECONDARY_INTERPOLATOR_ATI				0x896D
#define GL_NUM_FRAGMENT_REGISTERS_ATI				0x896E
#define GL_NUM_FRAGMENT_CONSTANTS_ATI				0x896F
#define GL_NUM_PASSES_ATI							0x8970
#define GL_NUM_INSTRUCTIONS_PER_PASS_ATI			0x8971
#define GL_NUM_INSTRUCTIONS_TOTAL_ATI				0x8972
#define GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_ATI	0x8973
#define GL_NUM_LOOPBACK_COMPONENTS_ATI				0x8974
#define GL_COLOR_ALPHA_PAIRING_ATI					0x8975
#define GL_SWIZZLE_STR_ATI							0x8976
#define GL_SWIZZLE_STQ_ATI							0x8977
#define GL_SWIZZLE_STR_DR_ATI						0x8978
#define GL_SWIZZLE_STQ_DQ_ATI						0x8979
#define GL_SWIZZLE_STRQ_ATI							0x897A
#define GL_SWIZZLE_STRQ_DQ_ATI						0x897B
#define GL_RED_BIT_ATI								0x00000001
#define GL_GREEN_BIT_ATI							0x00000002
#define GL_BLUE_BIT_ATI								0x00000004
#define GL_2X_BIT_ATI								0x00000001
#define GL_4X_BIT_ATI								0x00000002
#define GL_8X_BIT_ATI								0x00000004
#define GL_HALF_BIT_ATI								0x00000008
#define GL_QUARTER_BIT_ATI							0x00000010
#define GL_EIGHTH_BIT_ATI							0x00000020
#define GL_SATURATE_BIT_ATI							0x00000040
#define GL_COMP_BIT_ATI								0x00000002
#define GL_NEGATE_BIT_ATI							0x00000004
#define GL_BIAS_BIT_ATI								0x00000008

//ATI_pn_triangles
#define GL_PN_TRIANGLES_ATI							0x87F0
#define GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI	0x87F1
#define GL_PN_TRIANGLES_POINT_MODE_ATI				0x87F2
#define GL_PN_TRIANGLES_NORMAL_MODE_ATI				0x87F3
#define GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI		0x87F4
#define GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI		0x87F5
#define GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI		0x87F6
#define GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI		0x87F7
#define GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI   0x87F8

/*************************************************************/

#ifdef _WIN32 //moje

/* EXT_vertex_array */
typedef void (APIENTRY * PFNGLARRAYELEMENTEXTPROC) (GLint i);
typedef void (APIENTRY * PFNGLCOLORPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLDRAWARRAYSEXTPROC) (GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLEDGEFLAGPOINTEREXTPROC) (GLsizei stride, GLsizei count, const GLboolean *pointer);
typedef void (APIENTRY * PFNGLGETPOINTERVEXTPROC) (GLenum pname, GLvoid* *params);
typedef void (APIENTRY * PFNGLINDEXPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLNORMALPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLTEXCOORDPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLVERTEXPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);

/* ARB_multitexture */
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DARBPROC) (GLenum target, GLdouble s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FARBPROC) (GLenum target, GLfloat s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IARBPROC) (GLenum target, GLint s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SARBPROC) (GLenum target, GLshort s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DARBPROC) (GLenum target, GLdouble s, GLdouble t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IARBPROC) (GLenum target, GLint s, GLint t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SARBPROC) (GLenum target, GLshort s, GLshort t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IARBPROC) (GLenum target, GLint s, GLint t, GLint r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IARBPROC) (GLenum target, GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum target);

/* EXT_compiled_vertex_array */
typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);

/* EXT_cull_vertex */
typedef void (APIENTRY * PFNGLCULLPARAMETERDVEXTPROC) (GLenum pname, GLdouble* params);
typedef void (APIENTRY * PFNGLCULLPARAMETERFVEXTPROC) (GLenum pname, GLfloat* params);

/* WIN_swap_hint */
typedef void (APIENTRY * PFNGLADDSWAPHINTRECTWINPROC) (GLint x, GLint y, GLsizei width, GLsizei height);

/* EXT_point_parameter */
typedef void (APIENTRY * PFNGLPOINTPARAMETERFEXTPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFVEXTPROC) (GLenum pname, const GLfloat *params);

/* Moje wlasne zmiany: */

/* EXT_fog_coord */
typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);
typedef void (APIENTRY * PFNGLFOGCOORDDEXTPROC) (GLdouble coord);
typedef void (APIENTRY * PFNGLFOGCOORDFVEXTPROC) (GLfloat* coord);
typedef void (APIENTRY * PFNGLFOGCOORDDVEXTPROC) (GLdouble* coord);
typedef void (APIENTRY * PFNGLFOGCOORDPOINTEREXTPROC) (GLenum type, GLsizei stride, void *pointer);

/* EXT_secondary_color */
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BEXTPROC) (GLbyte red,GLbyte green, GLbyte blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SEXTPROC) (GLshort red,GLshort green, GLshort blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IEXTPROC) (GLint red,GLint green, GLint blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FEXTPROC) (GLfloat red,GLfloat green, GLbyte blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DEXTPROC) (GLdouble red,GLdouble green, GLdouble blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBEXTPROC) (GLubyte red,GLubyte green, GLubyte blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USEXTPROC) (GLushort red,GLushort green, GLushort blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIEXTPROC) (GLuint red,GLuint green, GLuint blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BVEXTPROC) (GLbyte *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SVEXTPROC) (GLshort *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IVEXTPROC) (GLint *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FVEXTPROC) (GLfloat *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DVEXTPROC) (GLdouble *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBVEXTPROC) (GLubyte *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USVEXTPROC) (GLushort *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIVEXTPROC) (GLuint *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLORPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride,
															 void* pointer);

/* EXT_vertex_weighting */
typedef void (APIENTRY * PFNGLVERTEXWEIGHTFEXTPROC) (GLfloat weight);
typedef void (APIENTRY * PFNGLVERTEXWEIGHTFVEXTPROC) (GLfloat* weight);
typedef void (APIENTRY * PFNGLVERTEXWEIGHTPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride,
														   void* pointer);

/*WGL_EXT_swap_control*/
typedef BOOL (APIENTRY * PFNWGLSWAPINTERVALEXTPROC) (GLint);
typedef GLint (APIENTRY * PFNWGLGETSWAPINTERVALEXTPROC) (void);

/*NV_vertex_array_range*/
typedef void (APIENTRY * PFNGLVERTEXARRAYRANGENVPROC) (GLsizei length,void *pointer);
typedef void (APIENTRY * PFNGLFLUSHVERTEXARRAYRANGENVPROC) (void);
typedef void * (APIENTRY * PFNWGLALLOCATEMEMORYNVPROC) (int size, float readfreq, float writefreq,
														float priority);
typedef void (APIENTRY * PFNWGLFREEMEMORYNVPROC) (void *pointer);

/*NV_register_combiners*/
typedef void (APIENTRY *PFNGLCOMBINERPARAMETERFVNVPROC) (GLenum pname,const GLfloat *params);
typedef void (APIENTRY *PFNGLCOMBINERPARAMETERIVNVPROC) (GLenum pname,const GLint *params);
typedef void (APIENTRY *PFNGLCOMBINERPARAMETERFNVPROC) (GLenum pname,const GLfloat param);
typedef void (APIENTRY *PFNGLCOMBINERPARAMETERINVPROC) (GLenum pname,const GLint param);
typedef void (APIENTRY *PFNGLCOMBINERINPUTNVPROC) (GLenum stage,GLenum portion, GLenum variable,
												   GLenum input,GLenum mapping,
												   GLenum componentUsage);
typedef void (APIENTRY *PFNGLCOMBINEROUTPUTNVPROC) (GLenum stage,GLenum portion,
													GLenum abOutput,GLenum cdOutput,
													GLenum sumOutput,
													GLenum scale,GLenum bias,
													GLboolean abDotProduct,
													GLboolean cdDotProduct,
													GLboolean muxSum);
typedef void (APIENTRY *PFNGLFINALCOMBINERINPUTNVPROC) (GLenum variable,GLenum input,
														GLenum mapping,GLenum componentUsage);
typedef void (APIENTRY *PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC) (GLenum stage,GLenum portion,
													GLenum variable,GLenum pname,
													GLfloat *params);
typedef void (APIENTRY *PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC) (GLenum stage,GLenum portion,
													GLenum variable,GLenum pname,
													GLint *params);
typedef void (APIENTRY *PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC) (GLenum stage,GLenum portion,
													GLenum pname,GLfloat *params);
typedef void (APIENTRY *PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC) (GLenum stage,GLenum portion,
													GLenum pname,GLint *params);
typedef void (APIENTRY *PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC) (GLenum variable,
													GLenum pname,GLfloat *params);
typedef void (APIENTRY *PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC) (GLenum variable,
													GLenum pname,GLint *params);

//for WGL extensions
typedef char* (APIENTRY *PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC dc);

//WGL_ARB_pixel_format
typedef BOOL (APIENTRY *PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (HDC hdc,int iPixelFormat,
													int iLayerPlane,UINT nAttributes,
													const int *piAttributes,int *piValues);
typedef BOOL (APIENTRY *PFNWGLGETPIXELFORMATATTRIBFVARBPROC) (HDC hdc,int iPixelFormat,
													int iLayerPlane,UINT nAttributes,
													const int *piAttributes,float *pfValues);
typedef BOOL (APIENTRY *PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc,const int *piAttribIList,
													const FLOAT *pfAttribFList,
													UINT nMaxFormats,int *piFormats,
													UINT *nNumFormats);

//WGL_ARB_pbuffer
DECLARE_HANDLE(HPBUFFERARB);
typedef HPBUFFERARB (APIENTRY *PFNWGLCREATEPBUFFERARBPROC) (HDC hDC,int iPixelFormat,
													int iWidth,int iHeight,
													const int *piAttribList);
typedef HDC (APIENTRY *PFNWGLGETPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer);
typedef int (APIENTRY *PFNWGLRELEASEPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer,HDC hDC);
typedef BOOL (APIENTRY *PFNWGLDESTROYPBUFFERARBPROC) (HPBUFFERARB hPbuffer);
typedef BOOL (APIENTRY *PFNWGLQUERYPBUFFERARBPROC) (HPBUFFERARB hPbuffer,int iAttribute,
													int *piValue);

//ARB_texture_compression
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXIMAGE3DARBPROC) (GLenum target,int level,
													GLenum internalformat,GLsizei width,
													GLsizei height, GLsizei depth,
													GLint border, GLsizei imageSize,
													const GLvoid* data);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) (GLenum target,int level,
													GLenum internalformat,GLsizei width,
													GLsizei height, GLint border, 
													GLsizei imageSize, const GLvoid* data);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXIMAGE1DARBPROC) (GLenum target,int level,
													GLenum internalformat,GLsizei width,
													GLint border, GLsizei imageSize,
													const GLvoid* data);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC) (GLenum target,int level,
													int xoffset, int yoffset, int zoffset, 
													GLsizei width, GLsizei height, GLsizei depth, 
													GLenum format,GLsizei imageSize,
													const GLvoid* data);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC) (GLenum target,int level,
													int xoffset, int yoffset, 
													GLsizei width, GLsizei height, 
													GLenum format,GLsizei imageSize,
													const GLvoid* data);
typedef void (APIENTRY *PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC) (GLenum target,int level,
													int xoffset,GLsizei width, 
													GLenum format,GLsizei imageSize,
													const GLvoid* data);
typedef void (APIENTRY *PFNGLGETCOMPRESSEDTEXIMAGEARBPROC) (GLenum target, GLint lod,
													GLvoid *img);

//ATI_fragment_shader
typedef GLuint (APIENTRY *PFNGLGENFRAGMENTSHADERSATIPROC)(GLuint range);
typedef GLvoid (APIENTRY *PFNGLBINDFRAGMENTSHADERATIPROC)(GLuint id);
typedef GLvoid (APIENTRY *PFNGLDELETEFRAGMENTSHADERATIPROC)(GLuint id);
typedef GLvoid (APIENTRY *PFNGLBEGINFRAGMENTSHADERATIPROC)(GLvoid);
typedef GLvoid (APIENTRY *PFNGLENDFRAGMENTSHADERATIPROC)(GLvoid);
typedef GLvoid (APIENTRY *PFNGLPASSTEXCOORDATIPROC)(GLuint dst, GLuint coord, GLenum swizzle);
typedef GLvoid (APIENTRY *PFNGLSAMPLEMAPATIPROC)(GLuint dst, GLuint interp, GLenum swizzle);
typedef GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP1ATIPROC)(GLenum op, GLuint dst, GLuint dstMask,
									   GLuint dstMod, GLuint arg1, GLuint arg1Rep,
									   GLuint arg1Mod);
typedef GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP2ATIPROC)(GLenum op, GLuint dst, GLuint dstMask,
									   GLuint dstMod, GLuint arg1, GLuint arg1Rep,
									   GLuint arg1Mod, GLuint arg2, GLuint arg2Rep,
									   GLuint arg2Mod);
typedef GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP3ATIPROC)(GLenum op, GLuint dst, GLuint dstMask,
									   GLuint dstMod, GLuint arg1, GLuint arg1Rep,
									   GLuint arg1Mod, GLuint arg2, GLuint arg2Rep,
									   GLuint arg2Mod, GLuint arg3, GLuint arg3Rep,
									   GLuint arg3Mod);
typedef GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP1ATIPROC)(GLenum op, GLuint dst, GLuint dstMod,
									   GLuint arg1, GLuint arg1Rep, GLuint arg1Mod);
typedef GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP2ATIPROC)(GLenum op, GLuint dst, GLuint dstMod,
									   GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
									   GLuint arg2, GLuint arg2Rep, GLuint arg2Mod);
typedef GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP3ATIPROC)(GLenum op, GLuint dst, GLuint dstMod,
									   GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
									   GLuint arg2, GLuint arg2Rep, GLuint arg2Mod,
									   GLuint arg3, GLuint arg3Rep, GLuint arg3Mod);
typedef GLvoid (APIENTRY *PFNGLSETFRAGMENTSHADERCONSTANTATIPROC)(GLuint dst, const GLfloat *value);

//ATI_pn_triangles
typedef void (APIENTRY *PFNGLPNTRIANGLESIATIPROC)(GLenum pname, GLint param);
typedef void (APIENTRY *PFNGLPNTRIANGLESFATIPROC)(GLenum pname, GLfloat param);

#endif // _WIN32

#ifdef __cplusplus
}
#endif

#endif /* __glext_h_ */
