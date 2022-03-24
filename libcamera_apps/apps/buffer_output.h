#ifndef BUFFER_OUTPUT_H
#define BUFFER_OUTPUT_H

// 1794 76608401 外呼中心
#include "output/output.hpp"
#include <map>
#include <vector>
#include <deque>
typedef struct buf_t
{
    void * buf;
    size_t buf_size;

    buf_t(void* b, size_t siz){ this->buf=b; this->buf_size = siz;}
    buf_t(){buf=nullptr; buf_size=0;}

} buf_t;

class BufferPollTest
{
public:
    BufferPollTest()
    {
        m_size_max=10;
    }
    int set_buf_max(size_t siz){m_size_max=siz;return 0;};
    int get_buf_max(){return (int)m_size_max;};

    void* get_buf(size_t siz)
    {
        //find free buf
        for(auto it=m_buf_free.begin();it!=m_buf_free.end();++it)
        {
            if (it->second.buf_size>=siz)
            {
                //find
                void* ret=it->first;
                buf_t nbuf(ret, siz);
                m_buf_used[it->first] = nbuf;
                m_buf_free.erase(it);
                return ret;
            }
        }

        //no freebuf
        if (m_buf_used.size()>m_size_max)
        {
            //
            printf("BufferPollTest full\n");
            return nullptr;
        }


        //new buf
        void *ret = malloc(siz);
        buf_t buf_one;
        buf_one.buf = ret;
        buf_one.buf_size = siz;
        m_buf_used[ret] = buf_one;
        return ret;
    }

    int free_buf(void* buf)
    {
        if(m_buf_used.find(buf)==m_buf_used.end())
        {
            //no such buffer. just skip
            printf("BufferPollTest free error. no such pointer:%08x\n", (unsigned int)buf);
            return 0;
        }

        buf_t to_free = m_buf_used[buf];
        m_buf_free[buf]=to_free;
        m_buf_used.erase(m_buf_used.find(buf));
        return 0;
    }


private:
    size_t m_size_max;
    std::map<void*, buf_t> m_buf_used;
    std::map<void*, buf_t> m_buf_free;

};







class BufferOutput : public Output
{
public:
    BufferOutput(VideoOptions const *options);
    ~BufferOutput();

public:
    int getBuffer(char* buf, unsigned len, unsigned long *timestamp_ms, unsigned *flags);
    int getBufSize();

    buf_t getBufferInner();
    int freeBufferInner(buf_t buf);

protected:
    void outputBuffer(void *mem, size_t size, int64_t timestamp_us, uint32_t flags) override;

private:
    unsigned int count_;
    int64_t file_start_time_ms_;

    BufferPollTest m_buf_pool;
    std::deque<buf_t> m_buf_vec;

};

#endif // BUFFER_OUTPUT_H
