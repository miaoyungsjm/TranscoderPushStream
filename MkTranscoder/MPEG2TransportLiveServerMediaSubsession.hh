#ifndef _MPEG2_TRANSPORT_LIVE_SERVER_MEDIA_SUBSESSION_HH  
#define _MPEG2_TRANSPORT_LIVE_SERVER_MEDIA_SUBSESSION_HH  
  
#ifndef _ON_DEMAND_SERVER_MEDIA_SUBSESSION_HH  
#include "OnDemandServerMediaSubsession.hh"  
#endif  
#ifndef _MPEG2_TRANSPORT_STREAM_FRAMER_HH  
#include "MPEG2TransportStreamFramer.hh"  
#endif  
#ifndef _BYTE_STREAM_LIVE_SOURCE_HH  
#include "ByteStreamLiveSource.hh"  
#endif  
#ifndef _MPEG2_TRANSPORT_STREAM_TRICK_MODE_FILTER_HH  
#include "MPEG2TransportStreamTrickModeFilter.hh"  
#endif  
#ifndef _MPEG2_TRANSPORT_STREAM_FROM_ES_SOURCE_HH  
#include "MPEG2TransportStreamFromESSource.hh"  
#endif  
  
#ifndef __LIVE_TS_BUFFER_TYPE_HH  
#include "LiveTSType.hh"  
#endif  
  
  
//class ClientTrickPlayState; // forward  
  
class MPEG2TransportLiveServerMediaSubsession: public OnDemandServerMediaSubsession  
{  
public:  
    static MPEG2TransportLiveServerMediaSubsession*
    createNew(UsageEnvironment& env, Boolean reuseFirstSource);

    int SubmitTsData(unsigned char* data, int size);

protected:
    MPEG2TransportLiveServerMediaSubsession(UsageEnvironment& env,
                Boolean reuseFirstSource);

    virtual ~MPEG2TransportLiveServerMediaSubsession();

    //virtual ClientTrickPlayState* newClientTrickPlayState();

private: // redefined virtual functions
  // Note that because - to implement 'trick play' operations - we're operating on
  // more than just the input source, we reimplement some functions that are
  // already implemented in "OnDemandServerMediaSubsession", rather than
  // reimplementing "seekStreamSource()" and "setStreamSourceScale()":
  virtual void startStream(unsigned clientSessionId, void* streamToken,
                           TaskFunc* rtcpRRHandler,
                           void* rtcpRRHandlerClientData,
                           unsigned short& rtpSeqNum,
                           unsigned& rtpTimestamp,
			   ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
                           void* serverRequestAlternativeByteHandlerClientData);
  virtual void pauseStream(unsigned clientSessionId, void* streamToken);
  virtual void seekStream(unsigned clientSessionId, void* streamToken, double& seekNPT, double streamDuration, u_int64_t& numBytes);
  virtual void setStreamScale(unsigned clientSessionId, void* streamToken, float scale);
  virtual void deleteStream(unsigned clientSessionId, void*& streamToken);


  // The virtual functions that are usually implemented by "ServerMediaSubsession"s:
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
					      unsigned& estBitrate);
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
                                    unsigned char rtpPayloadTypeIfDynamic,
				    FramedSource* inputSource);

  virtual void testScaleFactor(float& scale);
  virtual float duration() const;

private:
  //ClientTrickPlayState* lookupClient(unsigned clientSessionId);

private:
  //MPEG2TransportStreamIndexFile* fIndexFile;
  float fDuration;
  HashTable* fClientSessionHashTable; // indexed by client session id

  ByteStreamLiveSource* mpLiveSource;

};

#endif