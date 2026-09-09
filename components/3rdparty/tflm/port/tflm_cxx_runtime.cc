/**
 * @file tflm_cxx_runtime.cc
 * @brief Bare-metal C++ runtime bits TFLM needs from the platform.
 *
 * TFLM uses function-local statics with non-trivial destructors (the op
 * resolver and the interpreter in any normal driver). GCC emits
 * __cxa_atexit(dtor, obj, &__dso_handle) for those, and a bare-metal link has
 * no __dso_handle. The firmware never returns from main, so the destructors
 * would never run anyway - a null handle is the correct answer, not a stub.
 *
 * The Matter apps under apps/matter carry a private copy of this same declaration.
 * Providing it here means TFLM consumers - including customer apps - do not
 * have to know about it. Kept weak so an app that already defines its own
 * (a Matter app that also uses TFLM) still links.
 */

extern "C" {
void *__dso_handle __attribute__((weak)) = nullptr;
}
