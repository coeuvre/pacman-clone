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
    char *Dest = Buffer;

    const char *Source = String1;
    while (Dest < End && *Source)
    {
        *Dest++ = *Source++;
    }

    Source = String2;
    while (Dest < End && *Source)
    {
        *Dest++ = *Source++;
    }
}
