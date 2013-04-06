#ifndef S3FS_CACHE_H_
#define S3FS_CACHE_H_

#include "common.h"

//
// Struct
//
struct stat_cache_entry {
  struct stat   stbuf;
  unsigned long hit_count;
  time_t        cache_date;
  headers_t     meta;

  stat_cache_entry() : hit_count(0), cache_date(0) {
    memset(&stbuf, 0, sizeof(struct stat));
    meta.clear();
  }
};

typedef std::map<std::string, struct stat_cache_entry> stat_cache_t; // key=path

//
// Class
//
class StatCache
{
  private:
    static StatCache singleton;
    static pthread_mutex_t stat_cache_lock;
    stat_cache_t stat_cache;
    bool IsExpireTime;
    time_t ExpireTime;
    unsigned long CacheSize;

  private:
    bool GetStat(std::string& key, struct stat* pst, headers_t* meta, bool overcheck, const char* petag);
    // Truncate stat cache
    bool TruncateCache(void);

  public:
    StatCache();
    ~StatCache();

    // Reference singleton
    static StatCache* getStatCacheData(void) {
      return &singleton;
    }

    // Attribute
    unsigned long GetCacheSize(void) const;
    unsigned long SetCacheSize(unsigned long size);
    time_t GetExpireTime(void) const;
    time_t SetExpireTime(time_t expire);
    time_t UnsetExpireTime(void);

    // Get stat cache
    bool GetStat(std::string& key, struct stat* pst, headers_t* meta, bool overcheck = true) {
      return GetStat(key, pst, meta, overcheck, NULL);
    }
    bool GetStat(std::string& key, struct stat* pst, bool overcheck = true) {
      return GetStat(key, pst, NULL, overcheck, NULL);
    }
    bool GetStat(std::string& key, headers_t* meta, bool overcheck = true) {
      return GetStat(key, NULL, meta, overcheck, NULL);
    }
    bool HasStat(std::string& key, bool overcheck = true) {
      return GetStat(key, NULL, NULL, overcheck, NULL);
    }
    bool HasStat(std::string& key, const char* etag, bool overcheck = true) {
      return GetStat(key, NULL, NULL, overcheck, etag);
    }

    // Add stat cache
    bool AddStat(std::string& key, headers_t& meta);

    // Delete stat cache
    bool DelStat(const char* key);
    bool DelStat(std::string& key) {
      return DelStat(key.c_str());
    }
};

//
// Functions
//
bool convert_header_to_stat(const char* path, headers_t& meta, struct stat* pst);

#endif // S3FS_CACHE_H_
