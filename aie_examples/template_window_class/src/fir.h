#pragma once

#include "adf.h"

template<size_t NUM_COEFFS, typename ELEMENT_TYPE> class FIR
{
private:
    ELEMENT_TYPE (&coeffs)[NUM_COEFFS];
    ELEMENT_TYPE tapDelayLine[NUM_COEFFS];
    uint32 numSamples;

public:
    FIR(ELEMENT_TYPE(&coefficients)[NUM_COEFFS], uint32 samples);

    void filter(adf::input_buffer<ELEMENT_TYPE> & in, adf::output_buffer<ELEMENT_TYPE> & out);

    //user needs to write this function to register necessary info
    static void registerKernelClass()
    {
        REGISTER_FUNCTION(FIR::filter);
		REGISTER_PARAMETER(coeffs);
    }
};
