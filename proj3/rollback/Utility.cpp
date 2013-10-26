#include "Utility.h"

void throw_invalid_data()
{
  throw Error{"Invalid data found in file!"};
}

int normalized_time(const int time)
{
  return (time < 6) ? time + 12 : time;
}
