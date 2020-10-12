/* -------------------------------------- . ---------------------------------- .
| Filename : SmodeSpoutMetaData.h         | SmodeSpoutMetaData exchanged in    |
| Author   : Alexandre Buge               | Spout name Description             |
| Started  : 06/10/2020 17:41             |                                    |
` --------------------------------------- . --------------------------------- */

#ifndef PLUGINS_SPOUT_SMODE_META_DATA_H_
# define PLUGINS_SPOUT_SMODE_META_DATA_H_

# include "SpoutSharedMemory.h"

namespace smode
{

# ifdef WIN32
#  pragma pack(push, 1)
# else // WIN32
#  pragma pack(1)
# endif // WIN32

struct SmodeSpoutMetaData
{
  SmodeSpoutMetaData()
  {
    memcpy(smodeHeader, L"Smode", sizeof(smodeHeader));
  }

  wchar_t smodeHeader[6];
  std::chrono::high_resolution_clock::time_point frameTime;
  size_t frameNumber = ~0ULL;
  float matrix1[16] = { 0, };
  float matrix2[16] = { 0, };

  bool isValid() const
    {return memcmp(smodeHeader, L"Smode", sizeof(smodeHeader)) == 0;}

  void reset()
  {
    frameNumber = ~0ULL;
    frameTime = {};
    memset(matrix1, 0, sizeof(matrix1));
    memset(matrix2, 0, sizeof(matrix2));
  }
};

static_assert(sizeof(SmodeSpoutMetaData) < 256, "SmodeSpoutMetaData structure cannot be greater than Spout description field");

#ifdef WIN32
#	pragma pack(pop)
#else // !WIN32
#	pragma pack()
#endif // !WIN32

struct ScopedSpoutSharedMemoryLock
{
  explicit ScopedSpoutSharedMemoryLock(const char* sender)
  {
    if (!sharedMemory.Open(sender))
      return;
    if (!sharedMemory.Lock())
    {
      sharedMemory.Close();
      return;
    }
    succeed = true;
  }

  ~ScopedSpoutSharedMemoryLock()
  {
    if (succeed)
    {
      sharedMemory.Unlock();
      sharedMemory.Close();
    }
    succeed = false;
  }

  bool hasSucceed() const
    {return succeed;}

private:
  SpoutSharedMemory sharedMemory;
  bool succeed = false;
};


}; /* namespace smode */

#endif // !PLUGINS_SPOUT_SMODE_METADATA_H_
