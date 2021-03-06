double sigmoid (double x, double K)
    {
    return 1 / (1 + pow(10, - K * x / 400));
    }

bool iswhitespace (char c)
    {
    return c == ' ' || c == '\t' || (int)c == 10 || (int)c == 13;
    }

char * rtrim (char * buf)
    {
    while (strlen(buf) && iswhitespace(buf[strlen(buf) - 1]))
        {
        buf[strlen(buf) - 1] = 0;
        }
    return buf;
    }

char * ltrim (char * buf)
    {
    while (iswhitespace(buf[0]))
        {
        memcpy(buf, buf + 1, strlen(buf));
        }
    return buf;
    }

char * trim (char * buf)
    {
    return ltrim(rtrim(buf));
    }

int tokenize (char * input, char * tokens [], int max_tokens)
    {
    int num_tokens = 0;
    char * token = strtok(input, " ");

    while (token != NULL && num_tokens < max_tokens)
        {
        tokens[num_tokens] = new char[strlen(token) + 1];
        strcpy(tokens[num_tokens++], token);
        token = strtok(NULL, " ");
        }
    return num_tokens;
    }

static int pow2 (int x)
    {
    return (int)pow(2.0, x);
    }

bool strieq (const char * s1, const char * s2)
    {
    if (strlen(s1) != strlen(s2))
        {
        return false;
        }

    for ( size_t i = 0; i < strlen(s1); i++ )
        {
        if (::tolower(*(s1 + i)) != ::tolower(*(s2 + i)))
            {
            return false;
            }
        }
    return true;
    }

const char * FENfromParams (const char * params [], int num_params, int & param, char * fen)
    {
    if ((num_params - param - 1) < 6)
        {
        return NULL;
        }
    fen[0] = 0;

    for ( int i = 0; i < 6; i++ )
        {
        if (strlen(fen) + strlen(params[++ param]) + 1 >= 128)
            {
            return NULL;
            }
        sprintf(& fen[strlen(fen)], "%s ", params[param]);
        }
    return fen;
    }

//#if defined(_MSC_VER)

const char * dateAndTimeString (char * buf)
    {
    time_t now = time(NULL);
    struct tm * time = localtime(& now);
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour,
        time->tm_min, time->tm_sec);
    return buf;
    }

const char * timeString (char * buf)
    {
    struct _timeb tb;
    _ftime(& tb);
    struct tm * time = localtime(& tb.time);
    sprintf(buf, "%02d:%02d:%02d.%03d", time->tm_hour, time->tm_min, time->tm_sec, tb.millitm);
    return buf;
    }

class Stopwatch
    {
    public:
    LARGE_INTEGER start1_;
    static LARGE_INTEGER frequency_;
    uint64_t start2_;

    Stopwatch ()
        {
        start();
        }

    Stopwatch (int)
        {
        QueryPerformanceFrequency(& frequency_);
        }

    __forceinline void start ()
        {
        QueryPerformanceCounter(& start1_);
        start2_ = GetTickCount64();
        }

    __forceinline uint64_t microsElapsedHighRes () const
        {
        LARGE_INTEGER now;
        QueryPerformanceCounter(& now);
        return (now.QuadPart - start1_.QuadPart) * 1000000 / frequency_.QuadPart;
        }

    __forceinline uint64_t millisElapsedHighRes () const
        {
        LARGE_INTEGER now;
        QueryPerformanceCounter(& now);
        return (now.QuadPart - start1_.QuadPart) * 1000 / frequency_.QuadPart;
        }

    __forceinline uint64_t millisElapsed () const
        {
        return GetTickCount64() - start2_;
        }
    };

LARGE_INTEGER Stopwatch::frequency_;
static Stopwatch stopwatch_init (1);