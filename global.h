/* control - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _CTRLGLOBAL_H_
#define _CTRLGLOBAL_H_


//#define DEBUG

#define FREENULL(x) do { free(x); x=NULL; } while(0)

#include <string>
#include <vector>

/*******************************************************************************
 * cStrList
 ******************************************************************************/
class cStrList {
private:
  std::vector<std::string> v;
public:
  cStrList();
  void Add(const char* s);
  void Clear();
  int  Count();
  int  Find(const char* s);
  const char* operator[](int Index);
  void Update(unsigned Index, const char* s);
};

void fix_utf8_str(std::string & str);


#ifdef DEBUG
/* DEBUG defined */
#define debug_plugin( x... ) \
  do  { printf("%-15s:%.3d %-50s: ",__FILE__,__LINE__,__PRETTY_FUNCTION__); \
        printf(x); \
        printf("\n"); \
        fflush(stdout); \
      } while(0)


void hexdump(const char * intro, const unsigned char * buf, int len);

#else
/* DEBUG undefined */
#define debug_plugin( x... )
#define hexdump(intro,buf,len)

#endif



#endif //_CTRLGLOBAL_H_
