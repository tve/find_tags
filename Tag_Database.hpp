#ifndef TAG_DATABASE_HPP
#define TAG_DATABASE_HPP

#include "find_tags_common.hpp"

#include "Freq_Setting.hpp"
#include "Tag.hpp"

#include <map>

class Tag_Database {

 private:
  typedef std::map < Nominal_Frequency_kHz, TagSet > TagSetSet;
  
  TagSetSet tags;

  Freq_Set nominal_freqs;

public:
  Tag_Database (string filename);

  void populate_from_csv_file(string filename);

  void populate_from_sqlite_file(string filename);

  Freq_Set & get_nominal_freqs();

  TagSet * get_tags_at_freq(Nominal_Frequency_kHz freq);
};

#endif // TAG_DATABASE_HPP
