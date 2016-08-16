#include "SG_SQLite_Data_Source.hpp"

SG_SQLite_Data_Source::SG_SQLite_Data_Source(DB_Filer * db, unsigned int monoBN) :
  db(db),
  bytesLeft(0),
  offset(0)
{
  db->start_blob_reader(monoBN);
};

bool
SG_SQLite_Data_Source::getline(char * buf, int maxLen) {

  // bytesLeft will be -1 if we read an unterminated line on previous call

  while (bytesLeft <= 0) {
    // Ensure we have some blob data to read.
    // It is guaranteed that lines are not split across blobs.
    // repeat until a non-empty blob is found, or none remain

    if (! db->get_blob(& blob, & bytesLeft, & blobTS))
      return false;
    offset = 0;
  }
  const char * start = blob + offset;
  const char * eol = reinterpret_cast < const char * > (memchr(start, '\n', bytesLeft));

  // if no eol found, line goes to end of blob
  int lineLen = eol ? eol - start : bytesLeft;

  // truncate to max permitted line size; we silently discard the remainder
  int useLineLen = std::min(lineLen, maxLen);

  // copy to output
  memcpy(buf, start, useLineLen); // NB: we know these regions don't overlap; else we'd use memmove
  buf[useLineLen] = '\0';        // terminating 0
  bytesLeft -= lineLen + 1;      // NB: include the '\n' char in the calculation; will get -1 for unterminated line
  offset += lineLen + 1;
  return true;
};

SG_SQLite_Data_Source::~SG_SQLite_Data_Source() {
  db->end_blob_reader();
};

// ugly macro because I couldn't figure out how to make this work
// properly with templates.

#define SERIALIZE_FUN_BODY                   \
   ar & BOOST_SERIALIZATION_NVP( blobTS );   \
   ar & BOOST_SERIALIZATION_NVP( offset );

void
SG_SQLite_Data_Source::serialize(boost::archive::binary_iarchive & ar, const unsigned int version) {

  SERIALIZE_FUN_BODY;

  db->seek_blob(blobTS);
  db->get_blob(& blob, & bytesLeft, & blobTS);
  bytesLeft -= offset;
};

void
SG_SQLite_Data_Source::serialize(boost::archive::binary_oarchive & ar, const unsigned int version) {

  SERIALIZE_FUN_BODY;

};
