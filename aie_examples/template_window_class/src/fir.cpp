#include "fir.h"

template<size_t NUM_COEFFS, typename ELEMENT_TYPE> FIR<NUM_COEFFS, ELEMENT_TYPE>::FIR(ELEMENT_TYPE(&coefficients)[NUM_COEFFS], uint32 samples)
    : coeffs(coefficients)
{
    for (int i = 0; i < NUM_COEFFS; i++)
        tapDelayLine[i] = 0;

    numSamples = samples;
}

template<size_t NUM_COEFFS, typename ELEMENT_TYPE> void FIR<NUM_COEFFS, ELEMENT_TYPE>::filter(adf::input_buffer<ELEMENT_TYPE> & in, adf::output_buffer<ELEMENT_TYPE> & out)
{
    ELEMENT_TYPE* inItr = in.data();
    ELEMENT_TYPE* outItr = out.data();
    for (int i = 0; i < numSamples; i++)
    {
        for (int i = NUM_COEFFS-1; i > 0; i--)
            tapDelayLine[i] = tapDelayLine[i - 1];

        tapDelayLine[0] = *inItr++;

        int32 y = 0;
        for (int i = 0; i < NUM_COEFFS; i++)
        {
            y += coeffs[i] * tapDelayLine[i];
        }

        *outItr++ = y;
    }
}

