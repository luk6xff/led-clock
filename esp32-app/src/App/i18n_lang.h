#pragma once

typedef enum
{
    ENGLISH,
    POLISH,
    LAST_LANGUAGE
} i18n_lang;


typedef struct
{
    /*
     * Maximum length
     */
    int const len;

    /*
     * Array of pointers to language-specific string
     */
    char const * const text[LAST_LANGUAGE];

} i18n_str;


typedef enum
{
    MONDAY, TUESDAY, WEDNESDAY, THURSDAY,  LAST_STR
} str;

static const STRING strings[LAST_STR] =
{
    { /* JET_BLACK */
        15,
        {
            "Jet Black ",		/* English */
            "Rabenschwarz ",	/* German */
            ...
        }
    },
    { /* RED */
        15,
        {
            "Red ",			/* English */
            "Rot ",			/* German */
            ...
        }
    },
    ...
};