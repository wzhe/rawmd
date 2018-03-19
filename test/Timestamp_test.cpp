#include <base/Timestamp.h>
#include <vector>
#include <stdio.h>

using rawmd::Timestamp;

void passByConstReference(const Timestamp& x)
{
  char buf[32];
  snprintf(buf,sizeof(buf),"%s\n", x.toString().c_str());
}

void passByValue(Timestamp x)
{
  char buf[32];
  snprintf(buf,sizeof(buf),"%s\n", x.toString().c_str());
}


const int kNumber = 1000*1000;

void benchmark()
{

  std::vector<Timestamp> stamps;
  stamps.reserve(kNumber);
  for (int i = 0; i < kNumber; ++i)
  {
    stamps.push_back(Timestamp::now());
  }
  printf("%s\n", stamps.front().toString().c_str());
  printf("%s\n", stamps.back().toString().c_str());
  printf("%f\n", timeDifference(stamps.back(), stamps.front()));

  int increments[100] = {0};
  int64_t start = stamps.front().microSecondsSinceEpoch();
  for (int i = 1; i < kNumber; ++i)
  {
    int64_t next = stamps[i].microSecondsSinceEpoch();
    int64_t inc = next - start;
    start = next;
    if (inc < 0)
    {
      printf("reserve!\n");
    }
    else if (inc < 100)
    {
      ++increments[inc];
    }
    else
    {
      printf("big gap %d\n", static_cast<int>(inc));
    }
  }

  for (int i = 0; i < 100; i++)
  {
    printf("%2d: %d\n", i, increments[i]);
  }
}

void benchmarkByConstReferenceAndByValue()
{
  Timestamp byvaluestart(Timestamp::now());
   for (int i = 1; i < kNumber; ++i)
  {
    passByValue(Timestamp::now());
  }
   Timestamp byvalueend(Timestamp::now());
   printf("passByValue : %f \n",timeDifference(byvalueend,byvaluestart));

  Timestamp byConstReferencestart(Timestamp::now());
   for (int i = 1; i < kNumber; ++i)
  {
    passByConstReference(Timestamp::now());
  }
   Timestamp byConstReferenceend(Timestamp::now());
   printf("passByConstReference : %f \n",timeDifference(byConstReferenceend,byConstReferencestart));
}

int main()
{
  benchmarkByConstReferenceAndByValue(); //好像因为缓存的问题，这个测试并不准确。
  Timestamp now(Timestamp::now());
  printf("%s\n", now.toString().c_str());
  passByValue(now);
  passByConstReference(now);
  //benchmark();
  benchmarkByConstReferenceAndByValue();
}

