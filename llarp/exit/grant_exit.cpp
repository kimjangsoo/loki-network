#include <llarp/messages/exit.hpp>
#include <llarp/routing/handler.hpp>

namespace llarp
{
  namespace routing
  {
    bool
    GrantExitMessage::BEncode(llarp_buffer_t* buf) const
    {
      if(!bencode_start_dict(buf))
        return false;
      if(!BEncodeWriteDictMsgType(buf, "A", "G"))
        return false;
      if(!BEncodeWriteDictInt("S", S, buf))
        return false;
      if(!BEncodeWriteDictInt("T", T, buf))
        return false;
      if(!BEncodeWriteDictInt("V", version, buf))
        return false;
      if(!BEncodeWriteDictEntry("Y", Y, buf))
        return false;
      if(!BEncodeWriteDictEntry("Z", Z, buf))
        return false;
      return bencode_end(buf);
    }

    bool
    GrantExitMessage::DecodeKey(llarp_buffer_t k, llarp_buffer_t* buf)
    {
      bool read = false;
      if(!BEncodeMaybeReadDictInt("S", S, read, k, buf))
        return false;
      if(!BEncodeMaybeReadDictInt("T", T, read, k, buf))
        return false;
      if(!BEncodeMaybeReadDictInt("V", version, read, k, buf))
        return false;
      if(!BEncodeMaybeReadDictEntry("Y", Y, read, k, buf))
        return false;
      if(!BEncodeMaybeReadDictEntry("Z", Z, read, k, buf))
        return false;
      return read;
    }

    bool
    GrantExitMessage::Verify(llarp_crypto* c, const llarp::PubKey& pk) const
    {
      byte_t tmp[512] = {0};
      auto buf        = llarp::StackBuffer< decltype(tmp) >(tmp);
      GrantExitMessage copy;
      copy = *this;
      copy.Z.Zero();
      if(!copy.BEncode(&buf))
        return false;
      buf.sz = buf.cur - buf.base;
      return c->verify(pk, buf, Z);
    }

    bool
    GrantExitMessage::Sign(llarp_crypto* c, const llarp::SecretKey& sk)
    {
      byte_t tmp[512] = {0};
      auto buf        = llarp::StackBuffer< decltype(tmp) >(tmp);
      Z.Zero();
      Y.Randomize();
      if(!BEncode(&buf))
        return false;
      buf.sz = buf.cur - buf.base;
      return c->sign(Z, sk, buf);
    }

    GrantExitMessage&
    GrantExitMessage::operator=(const GrantExitMessage& other)
    {
      S       = other.S;
      T       = other.T;
      version = other.version;
      Y       = other.Y;
      Z       = other.Z;
      return *this;
    }

    bool
    GrantExitMessage::HandleMessage(IMessageHandler* h, llarp_router* r) const
    {
      return h->HandleGrantExitMessage(this, r);
    }

  }  // namespace routing
}  // namespace llarp