#include "Tag_Database.hpp"

Tag_Database::Tag_Database(string filename) {

  ifstream inf(filename.c_str(), ifstream::in);
  char buf[MAX_LINE_SIZE + 1];

  inf.getline(buf, MAX_LINE_SIZE);
  if (string(buf) != "\"proj\",\"id\",\"tagFreq\",\"fcdFreq\",\"g1\",\"g2\",\"g3\",\"bi\",\"dfreq\",\"g1.sd\",\"g2.sd\",\"g3.sd\",\"bi.sd\",\"dfreq.sd\",\"filename\"")
    throw std::runtime_error("Tag file header missing or incorrect\n");

  int num_lines = 1;
  while (! inf.eof()) {
    inf.getline(buf, MAX_LINE_SIZE);
    if (inf.gcount() == 0)
      break;
    ++num_lines;

    char proj[MAX_LINE_SIZE+1], filename[MAX_LINE_SIZE];
    int id;
    float freq_MHz, fcd_freq, gaps[4], dfreq, g1sd, g2sd, g3sd, bisd, dfreqsd;
    int num_par = sscanf(buf, "\"%[^\"]\",%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,\"%[^\"]\"", proj, &id, &freq_MHz, &fcd_freq, &gaps[0], &gaps[1], &gaps[2], &gaps[3], &dfreq, &g1sd, &g2sd, &g3sd, &bisd, &dfreqsd, filename);
    if (num_par < 15) {
      std::cerr << "Ignoring faulty line " << num_lines << " of tag database file:\n   " << buf << std::endl;
      continue;
    }
    Nominal_Frequency_kHz nom_freq = Freq_Setting::as_Nominal_Frequency_kHz(freq_MHz);
    // convert gaps to seconds
    for (int i=0; i < 3; ++i)
      gaps[i] /= 1000.0;

    if (nominal_freqs.count(nom_freq) == 0) {
      // we haven't seen this nominal frequency before
      // add it to the list and create a place to hold stuff
      nominal_freqs.insert(nom_freq);
      tags[nom_freq] = Tag_Set();
    }
    tags[nom_freq].insert (new Known_Tag (id, string(proj), freq_MHz, fcd_freq, dfreq, &gaps[0]));
  };
  if (tags.size() == 0)
    throw std::runtime_error("No tags registered.");
};

Freq_Set & Tag_Database::get_nominal_freqs() {
  return nominal_freqs;
};

Tag_Set *
Tag_Database::get_tags_at_freq(Nominal_Frequency_kHz freq) {
  return & tags[freq];
};

Known_Tag *
Tag_Database::get_tag(Tag_ID id) {
  return id;
};


