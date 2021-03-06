include(bde_package)
include(bde_utils)
include(bde_struct)
include(bde_uor)
include(bde_ufid)

bde_prefixed_override(inteldfp process_standalone_package)
function(inteldfp_process_standalone_package retUOR listFile installOpts)
    bde_assert_no_extra_args()

    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    set(TARGET inteldfp)

    set(headers
        ${rootDir}/LIBRARY/src/inteldfp/bid128_2_str.h
        ${rootDir}/LIBRARY/src/inteldfp/bid128_2_str_macros.h
        ${rootDir}/LIBRARY/src/inteldfp/bid_b2d.h
        ${rootDir}/LIBRARY/src/inteldfp/bid_conf.h
        ${rootDir}/LIBRARY/src/inteldfp/bid_div_macros.h
        ${rootDir}/LIBRARY/src/inteldfp/bid_functions.h
        ${rootDir}/LIBRARY/src/inteldfp/bid_gcc_intrinsics.h
        ${rootDir}/LIBRARY/src/inteldfp/bid_inline_add.h
        ${rootDir}/LIBRARY/src/inteldfp/bid_internal.h
        ${rootDir}/LIBRARY/src/inteldfp/bid_sqrt_macros.h
        ${rootDir}/LIBRARY/src/inteldfp/bid_strtod.h
        ${rootDir}/LIBRARY/src/inteldfp/bid_trans.h
        ${rootDir}/LIBRARY/src/inteldfp/bid_wrap_names.h
        ${rootDir}/LIBRARY/src/inteldfp/dfp754.h
    )

    set(sources
        ${rootDir}/LIBRARY/src/inteldfp/bid64_acos.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_acosh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_asin.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_asinh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_atan.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_atan2.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_atanh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_cbrt.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_cos.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_cosh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_erf.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_erfc.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_exp.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_exp10.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_exp2.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_expm1.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_hypot.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_lgamma.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_log.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_log10.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_log1p.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_log2.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_pow.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_sin.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_sinh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_tan.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_tanh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_tgamma.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_acos.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_acosh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_asin.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_asinh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_atan.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_atan2.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_atanh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_cbrt.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_cos.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_cosh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_erf.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_erfc.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_exp.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_exp10.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_exp2.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_expm1.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_hypot.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_lgamma.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_log.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_log10.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_log1p.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_log2.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_pow.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_sin.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_sinh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_tan.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_tanh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_tgamma.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_add.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_compare.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_div.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_fdimd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_fma.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_fmod.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_frexp.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_ldexp.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_llrintd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_logb.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_logbd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_lrintd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_lround.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_minmax.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_modf.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_mul.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_nearbyintd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_next.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_nexttowardd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_noncomp.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_quantexpd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_quantize.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_rem.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_round_integral.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_scalb.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_scalbl.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_sqrt.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_string.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_to_int16.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_to_int32.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_to_int64.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_to_int8.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_to_uint16.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_to_uint32.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_to_uint64.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_to_uint8.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_add.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_compare.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_div.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_fdimd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_fma.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_fmod.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_frexp.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_ldexp.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_llrintd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_logb.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_logbd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_lrintd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_lround.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_minmax.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_modf.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_mul.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_nearbyintd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_next.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_nexttowardd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_noncomp.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_quantexpd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_quantize.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_rem.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_round_integral.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_scalb.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_scalbl.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_sqrt.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_string.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_to_int16.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_to_int32.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_to_int64.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_to_int8.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_to_uint16.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_to_uint32.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_to_uint64.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_to_uint8.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_add.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_compare.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_div.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_fdimd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_fma.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_fmod.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_frexp.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_ldexp.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_llrintd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_logb.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_logbd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_lrintd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_lround.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_minmax.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_modf.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_mul.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_nearbyintd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_next.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_nexttowardd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_noncomp.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_quantexpd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_quantize.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_rem.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_round_integral.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_scalb.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_scalbl.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_sqrt.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_string.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_to_int16.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_to_int32.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_to_int64.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_to_int8.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_to_uint16.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_to_uint32.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_to_uint64.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_to_uint8.c
        ${rootDir}/LIBRARY/src/inteldfp/strtod32.c
        ${rootDir}/LIBRARY/src/inteldfp/strtod64.c
        ${rootDir}/LIBRARY/src/inteldfp/strtod128.c
        ${rootDir}/LIBRARY/src/inteldfp/wcstod32.c
        ${rootDir}/LIBRARY/src/inteldfp/wcstod64.c
        ${rootDir}/LIBRARY/src/inteldfp/wcstod128.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_acos.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_acosh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_asin.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_asinh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_atan.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_atan2.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_atanh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_cbrt.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_cos.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_cosh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_erf.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_erfc.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_exp.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_exp10.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_exp2.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_expm1.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_hypot.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_lgamma.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_log.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_log10.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_log1p.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_log2.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_pow.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_sin.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_sinh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_tan.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_tanh.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_tgamma.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_sub.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_to_bid128.c
        ${rootDir}/LIBRARY/src/inteldfp/bid32_to_bid64.c
        ${rootDir}/LIBRARY/src/inteldfp/bid64_to_bid128.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128_2_str_tables.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_binarydecimal.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_convert_data.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_decimal_data.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_decimal_globals.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_dpd.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_feclearexcept.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_fegetexceptflag.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_feraiseexcept.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_fesetexceptflag.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_fetestexcept.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_flag_operations.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_from_int.c
        ${rootDir}/LIBRARY/src/inteldfp/bid_round.c
        ${rootDir}/LIBRARY/src/inteldfp/bid128.c
        ${rootDir}/LIBRARY/float128/dpml_ux_bid.c
        ${rootDir}/LIBRARY/float128/dpml_ux_bessel.c
        ${rootDir}/LIBRARY/float128/dpml_ux_cbrt.c
        ${rootDir}/LIBRARY/float128/dpml_ux_erf.c
        ${rootDir}/LIBRARY/float128/dpml_ux_exp.c
        ${rootDir}/LIBRARY/float128/dpml_ux_int.c
        ${rootDir}/LIBRARY/float128/dpml_ux_inv_hyper.c
        ${rootDir}/LIBRARY/float128/dpml_ux_inv_trig.c
        ${rootDir}/LIBRARY/float128/dpml_ux_lgamma.c
        ${rootDir}/LIBRARY/float128/dpml_ux_log.c
        ${rootDir}/LIBRARY/float128/dpml_ux_mod.c
        ${rootDir}/LIBRARY/float128/dpml_ux_powi.c
        ${rootDir}/LIBRARY/float128/dpml_ux_pow.c
        ${rootDir}/LIBRARY/float128/dpml_ux_sqrt.c
        ${rootDir}/LIBRARY/float128/dpml_ux_trig.c
        ${rootDir}/LIBRARY/float128/dpml_ux_ops.c
        ${rootDir}/LIBRARY/float128/dpml_ux_ops_64.c
        ${rootDir}/LIBRARY/float128/dpml_four_over_pi.c
        ${rootDir}/LIBRARY/float128/dpml_exception.c
        ${rootDir}/LIBRARY/float128/sqrt_tab_t.c
        ${rootDir}/LIBRARY/float128/architecture.h
        ${rootDir}/LIBRARY/float128/assert.h
        ${rootDir}/LIBRARY/float128/build.h
        ${rootDir}/LIBRARY/float128/compiler.h
        ${rootDir}/LIBRARY/float128/dpml_acosh_t.h
        ${rootDir}/LIBRARY/float128/dpml_asinh_t.h
        ${rootDir}/LIBRARY/float128/dpml_bessel_x.h
        ${rootDir}/LIBRARY/float128/dpml_bid_x.h
        ${rootDir}/LIBRARY/float128/dpml_cbrt_x.h
        ${rootDir}/LIBRARY/float128/dpml_cons_x.h
        ${rootDir}/LIBRARY/float128/dpml_erf_t.h
        ${rootDir}/LIBRARY/float128/dpml_erf_x.h
        ${rootDir}/LIBRARY/float128/dpml_error_codes.h
        ${rootDir}/LIBRARY/float128/dpml_error_codes_enum.h
        ${rootDir}/LIBRARY/float128/dpml_exception.h
        ${rootDir}/LIBRARY/float128/dpml_exp_x.h
        ${rootDir}/LIBRARY/float128/dpml_function_info.h
        ${rootDir}/LIBRARY/float128/dpml_globals.h
        ${rootDir}/LIBRARY/float128/dpml_int_x.h
        ${rootDir}/LIBRARY/float128/dpml_inv_hyper_x.h
        ${rootDir}/LIBRARY/float128/dpml_inv_trig_x.h
        ${rootDir}/LIBRARY/float128/dpml_lgamma_t.h
        ${rootDir}/LIBRARY/float128/dpml_lgamma_x.h
        ${rootDir}/LIBRARY/float128/dpml_log2_t.h
        ${rootDir}/LIBRARY/float128/dpml_log_t.h
        ${rootDir}/LIBRARY/float128/dpml_log_x.h
        ${rootDir}/LIBRARY/float128/dpml_mod_x.h
        ${rootDir}/LIBRARY/float128/dpml_names.h
        ${rootDir}/LIBRARY/float128/dpml_pow.h
        ${rootDir}/LIBRARY/float128/dpml_pow_x.h
        ${rootDir}/LIBRARY/float128/dpml_powi_x.h
        ${rootDir}/LIBRARY/float128/dpml_private.h
        ${rootDir}/LIBRARY/float128/dpml_rdx_x.h
        ${rootDir}/LIBRARY/float128/dpml_special_exp.h
        ${rootDir}/LIBRARY/float128/dpml_sqrt_x.h
        ${rootDir}/LIBRARY/float128/dpml_trig_x.h
        ${rootDir}/LIBRARY/float128/dpml_ux.h
        ${rootDir}/LIBRARY/float128/dpml_ux_32_64.h
        ${rootDir}/LIBRARY/float128/dpml_ux_alpha_macros.h
        ${rootDir}/LIBRARY/float128/endian.h
        ${rootDir}/LIBRARY/float128/f_format.h
        ${rootDir}/LIBRARY/float128/i_format.h
        ${rootDir}/LIBRARY/float128/ix86_macros.h
        ${rootDir}/LIBRARY/float128/mphoc_functions.h
        ${rootDir}/LIBRARY/float128/mphoc_macros.h
        ${rootDir}/LIBRARY/float128/mtc_macros.h
        ${rootDir}/LIBRARY/float128/op_system.h
        ${rootDir}/LIBRARY/float128/poly_macros.h
        ${rootDir}/LIBRARY/float128/sqrt_macros.h
    )

    bde_ufid_add_library(${TARGET} ${sources} ${headers})

    # Set up PIC
    # This code does not work in 3.8, but will be fixed in later versions.
    # The -fPIC flag is set explicitely in the compile options for now.
    if(${bde_ufid_is_shr} OR ${bde_ufid_is_pic})
        set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE 1)
    endif()

    # Common compile definitions.
    target_compile_definitions(
        ${TARGET}
        PRIVATE
            "DECIMAL_CALL_BY_REFERENCE=0"
            "DECIMAL_GLOBAL_ROUNDING=1"
            "DECIMAL_GLOBAL_EXCEPTION_FLAGS=0"
    )

    target_compile_options(
        ${TARGET}
        PRIVATE
            $<$<C_COMPILER_ID:AppleClang>:
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -fPIC>
            >
            $<$<C_COMPILER_ID:Clang>:
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -fPIC>
            >
            $<$<C_COMPILER_ID:GNU>:
                -std=gnu99
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -fPIC>
            >
            $<$<C_COMPILER_ID:SunPro>:
                -temp=/bb/data/tmp
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -xcode=pic32>
            >
            $<$<C_COMPILER_ID:XL>:
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -qpic>
                $<${bde_ufid_is_mt}: -qthreaded>
            >
    )

    # Detecting platform endianess.
    include (TestBigEndian)
    test_big_endian(IS_BIG_ENDIAN)

    target_compile_definitions(
        ${TARGET}
        PRIVATE
            "BID_BIG_ENDIAN=$<BOOL:${IS_BIG_ENDIAN}>"
    )

    target_compile_definitions(
        ${TARGET}
        PRIVATE
            $<${bde_ufid_is_mt}: _REENTRANT>
            $<$<C_COMPILER_ID:AppleClang>:
                "LINUX"
                "efi2"
            >
            $<$<C_COMPILER_ID:Clang>:
                "USE_REAL_MALLOC"
                "LINUX"
                "efi2"
            >
            $<$<C_COMPILER_ID:GNU>:
                "USE_REAL_MALLOC"
                "LINUX"
                "efi2"
            >
            $<$<C_COMPILER_ID:MSVC>:
                "WINNT"
                "WINDOWS"
                "WNT"
                $<${CMAKE_CL_64}:
                    "ia32"
                >
            >
            $<$<C_COMPILER_ID:SunPro>:
                "SUNOS"
                "efi2"
                "__linux"
                "__float80=double"
                "BID_THREAD="
            >
            $<$<C_COMPILER_ID:XL>:
                "LINUX"
                "efi2"
                "__linux"
                "__QNX__"
                "__thread="
            >
    )

    target_include_directories(
        ${TARGET}
        PUBLIC
            $<BUILD_INTERFACE:${rootDir}/LIBRARY/src>
    )

    bde_struct_get_field(component ${installOpts} COMPONENT)
    bde_struct_get_field(includeInstallDir ${installOpts} INCLUDE_DIR)
    install(
        FILES ${headers}
        COMPONENT "${component}-headers"
        DESTINATION "${includeInstallDir}/${TARGET}"
    )

    target_include_directories(
        ${TARGET}
        PUBLIC
            $<INSTALL_INTERFACE:${includeInstallDir}>
    )

    # Don't create interfaces to only use our own build/usage reqiurements
    bde_struct_create(
        uor
        BDE_UOR_TYPE
        NAME "${TARGET}"
        TARGET "${TARGET}"
    )
    standalone_package_install(${uor} ${listFile} ${installOpts})

    # Meta data install
    install(
        DIRECTORY ${listDir}
        COMPONENT "${component}-meta"
        DESTINATION "share/bdemeta/thirdparty/${component}"
        EXCLUDE_FROM_ALL
    )

    bde_return(${uor})
endfunction()
