#pragma once

template <typename InIter, typename OutIter>
OutIter line_update(InIter begin, InIter end, OutIter result)
{
    ++result;
    // Iterate over the interior: skip the last and first element
    for(InIter it = begin + 1; it != end - 1; ++it, ++result)
    {
        *result = 0.25 * (it.up[-1] + it.up[+1] + it.down[-1]  + it.down[+1])
            - *it.middle;
    }
    ++result;
    return result;
}

template <typename InIter, typename OutIter>
OutIter line_update(InIter begin, InIter end, OutIter result, int& inc)
{
    ++result;
    // Iterate over the interior: skip the last and first element
    for(InIter it = begin + 1; it != end - 1; ++it, ++result)
    {
        *result = inc++;
    }
    ++result;
    return result;
}