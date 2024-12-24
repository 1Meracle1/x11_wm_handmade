#include "hex.h"
#include <ctype.h>

internal u8 HexDigit(u8 c, bool *ok)
{
  u8 res = {0};
  *ok    = false;
  if (isdigit(c))
  {
    res = c - '0';
    *ok = true;
  }
  else if (isalpha(c))
  {
    if (isupper(c))
    {
      res = c - 'A' + 10;
      *ok = true;
    }
    else
    {
      res = c - 'a' + 10;
      *ok = true;
    }
  }
  return res;
}

const u8 HEX_TABLE[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
};

internal bool HexDecode(u8 *src, u64 len_src, u8 *dst, u64 len_dst)
{
  bool ok = true;
  if (len_src % 2 == 1)
  {
    ok = false;
  }
  else
  {
    u64 i = 0;
    for (u64 j = 1; j < len_src; j += 2)
    {
      u8 p = src[j - 1];
      u8 q = src[j];

      u8 a = HexDigit(p, &ok);
      if (!ok)
      {
        break;
      }
      u8 b = HexDigit(q, &ok);
      if (!ok)
      {
        break;
      }

      if (i < len_dst)
      {
        dst[i] = (a << 4) | b;
        i += 1;
      }
      else
      {
        ok = false;
        break;
      }
    }
  }
  return ok;
}