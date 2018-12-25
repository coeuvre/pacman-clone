static int
IsStringStartWith(const char *String, const char *Prefix)
{
    const char *A = String;
    const char *B = Prefix;

    int result = 1;

    for (;;)
    {
        if (*B == 0)
        {
            break;
        }

        if (*A == 0)
        {
            result = 0;
            break;
        }

        if (*A != *B)
        {
            result = 0;
            break;
        }

        ++A;
        ++B;
    }

    return result;
}

static void
ConcatString(char *Buffer, size_t BufferSize, const char *String1, const char *String2)
{
    char *End = Buffer + BufferSize;
    char *Dst = Buffer;

    const char *Src = String1;
    while ((Dst + 1) < End && *Src)
    {
        *Dst++ = *Src++;
    }

    Src = String2;
    while ((Dst + 1) < End && *Src)
    {
        *Dst++ = *Src++;
    }

    if (Dst < End)
    {
        *Dst++ = 0;
    }
}
