/* -------------------------------------- . ---------------------------------- .
| Filename : SmodeSpoutMetaData.h         | SmodeSpoutMetaData exchanged in    |
| Author   : Alexandre Buge               | Spout name Description             |
| Started  : 06/10/2020 17:41             |                                    |
` --------------------------------------- . --------------------------------- */

#ifndef PLUGINS_SPOUT_SMODE_META_DATA_H_
# define PLUGINS_SPOUT_SMODE_META_DATA_H_

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

  bool isValid() const
    {return memcmp(smodeHeader, L"Smode", sizeof(smodeHeader)) == 0;}

  void reset()
  {
    frameNumber = ~0ULL;
    frameTime = {};
  }
};

#ifdef WIN32
#	pragma pack(pop)
#else // !WIN32
#	pragma pack()
#endif // !WIN32


}; /* namespace smode */

#endif // !PLUGINS_SPOUT_SMODE_METADATA_H_
