//static inline void      __set_errno ( int  _errno )     { errno = _errno; }
#define __set_errno(_errno)   {errno=_errno;}



//_ strtoll.c
// Copyright (C) 1986-2001 by Digital Mars
// All Rights Reserved
// Written by Walter Bright
// www.digitalmars.com

/*

int64 wcstoll(const wchar_t *p, wchar_t **pend, int base)
{
    int64 number = 0;
    int positive,c;
    int error;
    const wchar_t *pstart;
    int anytrans = 0;

    pstart = p;

    while (iswspace(*p))  // skip leading white space
 p++;

    switch (*p)
    {   case '-':
     positive = 0;
     p++;
     break;
 case '+':
     p++;
     // FALL-THROUGH
 default:
     positive = 1;
     break;
    }
    switch (base)
    {   case 0:
     base = 10;  // assume decimal base
     if (*p == '0')
     {   base = 8; // could be octal
  p++;
  anytrans++; // count 0 as a translated char
  switch (*p)
  {   case 'x':
      case 'X':
   base = 16; // hex
   p++;
   anytrans--; // oops, the previous 0 was part of a 0x, now look for valid chars
   break;
#if BINARY
      case 'b':
      case 'B':
   base = 2; // binary
   p++;
   anytrans--;
   break;
#endif
  }
     }
     break;
 case 16:   // skip over '0x' and '0X'
     if (*p == '0' && (p[1] == 'x' || p[1] == 'X'))
  p += 2;
     break;
#if BINARY
 case 2:   // skip over '0b' and '0B'
     if (*p == '0' && (p[1] == 'b' || p[1] == 'B'))
  p += 2;
     break;
#endif
    }
    error = 0;
    while (1)
    {   c = *p;
 if (c >= '0' && c <= '9')
     c -= '0';
 else if (c >= 'a' && c <= 'z')
     c = c - ('a' - 10);
 else if (c >= 'A' && c <= 'Z')
     c = c - ('A' - 10);
 else
 {   // unrecognized character
     // Go back to beginning of string if nothing
     // was dealt with properly
     if (anytrans == 0)
  p = pstart;
     break;
 }
 if (c >= base)  // not in number base
 {
     // Go back to beginning of string if no characters
     // were successfully dealt with
     if (anytrans == 0)
  p = pstart;
     break;
 }
 if ((LLONG_MIN + c) / base > number)
     error = 1;
 number = number * base - c;
 p++;
 anytrans++;
    }
    if (pend)
 *pend = (wchar_t *) p;
    if (positive && number == LLONG_MIN || error)
    {
 /*  a range error has occurred, set errno and return
     LLONG_MAX or LLONG_MIN dependant upon positive.
     I.e. errange on a negative, means it was under
     LLONG_MIN, on a positive, was above LLONG_MAX
  */
/*number = (positive) ? LLONG_MAX : LLONG_MIN;
 __set_errno (ERANGE);
 return number;
    }
    return (positive) ? -number : number;
}



int64 strtoll(const char *p,char **pend,int base)
{
    int64 number = 0;
    int positive,c;
    int error;
    const char *pstart;
    int anytrans = 0;

    pstart = p;

    while (isspace(*p))  // skip leading white space
 p++;

    switch (*p)
    {   case '-':
     positive = 0;
     p++;
     break;
 case '+':
     p++;
     // FALL-THROUGH
 default:
     positive = 1;
     break;
    }
    switch (base)
    {   case 0:
     base = 10;  // assume decimal base
     if (*p == '0')
     {   base = 8; // could be octal
  p++;
  anytrans++; // count 0 as a translated char
  switch (*p)
  {   case 'x':
      case 'X':
   base = 16; // hex
   p++;
   anytrans--; // oops, the previous 0 was part of a 0x, now look for valid chars
   break;
#if BINARY
      case 'b':
      case 'B':
   base = 2; // binary
   p++;
   anytrans--;
   break;
#endif
  }
     }
     break;
 case 16:   // skip over '0x' and '0X'
     if (*p == '0' && (p[1] == 'x' || p[1] == 'X'))
  p += 2;
     break;
#if BINARY
 case 2:   // skip over '0b' and '0B'
     if (*p == '0' && (p[1] == 'b' || p[1] == 'B'))
  p += 2;
     break;
#endif
    }
    error = 0;
    while (1)
    {   c = *p;
 if (c >= '0' && c <= '9')
     c -= '0';
 else if (c >= 'a' && c <= 'z')
     c = c - ('a' - 10);
 else if (c >= 'A' && c <= 'Z')
     c = c - ('A' - 10);
 else
 {   // unrecognized character
     // Go back to beginning of string if nothing
     // was dealt with properly
     if (anytrans == 0)
  p = pstart;
     break;
 }
 if (c >= base)  // not in number base
 {
     // Go back to beginning of string if no characters
     // were successfully dealt with
     if (anytrans == 0)
  p = pstart;
     break;
 }
 if ((LLONG_MIN + c) / base > number)
     error = 1;
 number = number * base - c;
 p++;
 anytrans++;
    }
    if (pend)
 *pend = (char *) p;
    if (positive && number == LLONG_MIN || error)
    {
 /*  a range error has occurred, set errno and return
     LLONG_MAX or LLONG_MIN dependant upon positive.
     I.e. errange on a negative, means it was under
     LLONG_MIN, on a positive, was above LLONG_MAX
  */
/* number = (positive) ? LLONG_MAX : LLONG_MIN;
 __set_errno (ERANGE);
 return number;
    }
    return (positive) ? -number : number;
}

*/
/*
uint64 wcstoull(const wchar_t *p, wchar_t **pend, int base)
{
 uint64 number = 0;
 int c;
 int error;

 while (_istspace(*p))  // skip leading white space
     p++;
 if (*p == '+')
     p++;
 switch (base)
 {   case 0:
  base = 10;  // assume decimal base
  if (*p == '0')
  { base = 8; // could be octal
   p++;
   switch (*p)
   {   case 'x':
       case 'X':
    base = 16; // hex
    p++;
    break;
#if BINARY
       case 'b':
       case 'B':
    base = 2; // binary
    p++;
    break;
#endif
   }
  }
  break;
     case 16:   // skip over '0x' and '0X'
  if (*p == '0' && (p[1] == 'x' || p[1] == 'X'))
   p += 2;
  break;
#if BINARY
     case 2:   // skip over '0b' and '0B'
  if (*p == '0' && (p[1] == 'b' || p[1] == 'B'))
   p += 2;
  break;
#endif
 }
 error = 0;
 while (1)
 { c = *p;
  if ('0' <= c && c <= '9')
   c -= '0';
  else if ('a' <= c && c <= 'z')
   c -= 'a' - 10;
  else if ('A' <= c && c <= 'Z')
   c -= 'A' - 10;
  else   // unrecognized character
   break;
  if (c >= base)  // not in number base
   break;
  if ((ULLONG_MAX - c) / base < number)
   error = 1;
  number = number * base + c;
  p++;
 }
 if (pend)
  *pend = (wchar_t *)p;
 if (error)
 { number = ULLONG_MAX;
  __set_errno (ERANGE);
 }
 return number;
}

*/

uint64 strtoull(const char *p,char **pend,int base)
{
 uint64 number = 0;
 return number;
}

uint64 strtoll(const char *nptr, char **endptr, int base) 
{
 uint64 number = 0;
 return number;
}
