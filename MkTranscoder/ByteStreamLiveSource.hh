#ifndef _BYTE_STREAM_LIVE_SOURCE_HH  
#define _BYTE_STREAM_LIVE_SOURCE_HH  
  
#ifndef _FRAMED_SOURCE_HH  
#include "FramedSource.hh"  
#endif  
  
#ifndef __LIVE_TS_BUFFER_TYPE_HH  
#include "LiveTSType.hh"  
#endif  
  
  
class ByteStreamLiveSource: public FramedSource   
{  
public:  
    static ByteStreamLiveSource* createNew(UsageEnvironment& env,
                         unsigned preferredFrameSize = 0,  
                         unsigned playTimePerFrame = 0);  
  
  
    //void seekToByteAbsolute(u_int64_t byteNumber, u_int64_t numBytesToStream = 0);  
    // if "numBytesToStream" is >0, then we limit the stream to that number of bytes, before treating it as EOF  
    //void seekToByteRelative(int64_t offset, u_int64_t numBytesToStream = 0);  
    //void seekToEnd(); // to force EOF handling on the next read  
    int SubmitTsData(unsigned char* data, int size);
protected:  
    ByteStreamLiveSource(UsageEnvironment& env, 
               unsigned preferredFrameSize,  
               unsigned playTimePerFrame);  
    // called only by createNew()  
  
    virtual ~ByteStreamLiveSource();  
  
    //static void fileReadableHandler(ByteStreamFileSource* source, int mask);  
    void doReadFromBuffer();  
    
private:  
    // redefined virtual functions:  
    virtual void doGetNextFrame();  
    virtual void doStopGettingFrames();  
  
private:  
    unsigned fPreferredFrameSize;  
    unsigned fPlayTimePerFrame;  
    //Boolean fFidIsSeekable;  
    unsigned fLastPlayTime;  
    Boolean fHaveStartedReading;  
    Boolean fLimitNumBytesToStream;  
    u_int64_t fNumBytesToStream; // used iff "fLimitNumBytesToStream" is True  

    QRingBuffer* fLocalBuf;
    // char *buff;
};

#endif  
