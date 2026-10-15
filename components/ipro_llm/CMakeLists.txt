# IPRO on-device tool-calling model engine (tools/ipro_llm). Portable C11, no
# allocation; host tests against the PyTorch reference in test_host/.

__generate_library()

__component_add_include_dirs(include)

set(IPRO_LLM_SRC
    src/llm_math.c
    src/llm_cq.c
    src/llm_tok.c
    src/llm_model.c
    src/llm_grammar.c
    src/llm_engine.c
    src/llm_accel.c
)
# The IPRO7 NPU for the attention projections (port/ipro7, needs components/ai)
if(CONFIG_IPRO_LLM_NPU)
    __component_add_include_dirs(port/ipro7)
    list(APPEND IPRO_LLM_SRC port/ipro7/llm_accel_npu.c)
endif()
foreach(src ${IPRO_LLM_SRC})
    __component_add_sources(${src})
endforeach()

# The SDK appends a global -Os after component options; a source-level option
# comes last on the command line, so the engine really builds at -O2.
list(TRANSFORM IPRO_LLM_SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
# -fno-math-errno lets lrintf / sqrtf become fcvt.w.s / fsqrt.s (same rounding);
# -ffp-contract=off keeps the batched and single-token kernels bit-identical.
set(IPRO_LLM_OPTS "-O2;-fno-math-errno;-ffp-contract=off")
if(CONFIG_IPRO_LLM_PROF_CYCLES)
    list(APPEND IPRO_LLM_OPTS "-DLLM_PROF_CYCLES")
endif()
set_source_files_properties(${IPRO_LLM_SRC} PROPERTIES COMPILE_OPTIONS "${IPRO_LLM_OPTS}")
