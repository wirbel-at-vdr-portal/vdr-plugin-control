/* control - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * added: 20160505, wirbel.
 */
#ifndef _circbuf_h_
#define _circbuf_h_

#include <cstddef>
#include <atomic>


/*******************************************************************************
 * cCircularBuffer
 ******************************************************************************/

//#define CIRCBUF_RANGE_CHK 1

template <typename T, size_t size> class cCircularBuffer {
private:
  T mBuf[size];
  std::atomic<size_t> mHead;
  std::atomic<size_t> mTail;
public:
  cCircularBuffer() :  mHead(0), mTail(0) {}
  virtual ~cCircularBuffer() {}
  bool Write(const T* Source, size_t Count) {
     #ifdef CIRCBUF_RANGE_CHK
     if (Count > size)
        return false;
     #endif

     size_t head = mHead.load(std::memory_order_relaxed);
     size_t hsum = head + Count;
     size_t hmod = hsum % size;

     if (hsum > size) { // circle around end of buffer.
        Count -= hmod;
        memcpy(mBuf, Source + Count, hmod);
        }

     memcpy(mBuf + head, Source, Count);
     mHead.store(hmod, std::memory_order_release);
     return true;
     }
 
  size_t Read(T* Dest, size_t Count) {
     size_t tail = mTail.load(std::memory_order_relaxed);
     size_t head = mHead.load(std::memory_order_acquire);
     size_t stored = (head + size - tail) % size;
     if (stored == 0)
        return 0;

     size_t part1 = 0;
     size_t part2 = Count = Count < stored? Count : stored;
     size_t tsum = tail + Count;
     size_t tmod = tsum % size;

     if (tsum > size) { // circle around end of buffer.
        part1 = size - tail;
        memcpy(Dest, mBuf + tail, part1);
        part2 -= part1;
        Dest += part1;
        tail = 0;        
        }
     memcpy(Dest, mBuf + tail, part2);
     mTail.store(tmod, std::memory_order_release);
     return Count;
     }
};

#endif
