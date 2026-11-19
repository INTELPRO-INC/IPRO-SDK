/*
 *  Copyright (c) 2020, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements Backbone Thread Management Framework (TMF) functionalities.
 */

#include "backbone_tmf.hpp"

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE

#include "common/log.hpp"
#include "instance/instance.hpp"

namespace ot {
namespace BackboneRouter {

RegisterLogModule("Bbr");

BackboneTmfAgent::BackboneTmfAgent(Instance &aInstance)
    : Coap::Coap(aInstance)
    , mSubscribed{false, false}
    , mNeedsCleanup(false)
{
    SetInterceptor(&Filter, this);
    SetResourceHandler(&HandleResource);
}

Error BackboneTmfAgent::Start(void)
{
    Error error = kErrorNone;
    bool  wasBound = mSocket.IsBound();

    VerifyOrExit(Get<Local>().IsEnabled(), error = kErrorInvalidState);
    // A failed bind whose cleanup close also failed leaves an open, unbound
    // socket. Udp::Open asserts on that state; finish cleanup before retrying.
    if (mNeedsCleanup || (!wasBound && mSocket.IsOpen()))
    {
        SuccessOrExit(error = Stop());
        wasBound = false;
    }
    error = Coap::Start(kBackboneUdpPort, Ip6::kNetifBackbone);
    if (error != kErrorNone)
    {
        // UDP may set its logical port before the platform bind fails. Even
        // IsBound cannot prove successful binding after failed cleanup.
        mNeedsCleanup = mSocket.IsOpen();
        ExitNow();
    }
    if (!wasBound)
    {
        LogInfo("Start listening on port %u", kBackboneUdpPort);
    }
    error = UpdateMulticastSubscriptions();

exit:
    return error;
}

Error BackboneTmfAgent::Stop(void)
{
    Error error = (mSocket.IsOpen() && !mSocket.IsBound()) ? mSocket.Close() : Coap::Stop();

    if (error == kErrorNone)
    {
        mSubscribed[0] = false;
        mSubscribed[1] = false;
        mNeedsCleanup = false;
    }
    else
    {
        mNeedsCleanup = true;
    }
    return error;
}

bool BackboneTmfAgent::HandleResource(CoapBase               &aCoapBase,
                                      const char             *aUriPath,
                                      ot::Coap::Message      &aMessage,
                                      const Ip6::MessageInfo &aMessageInfo)
{
    return static_cast<BackboneTmfAgent &>(aCoapBase).HandleResource(aUriPath, aMessage, aMessageInfo);
}

bool BackboneTmfAgent::HandleResource(const char             *aUriPath,
                                      ot::Coap::Message      &aMessage,
                                      const Ip6::MessageInfo &aMessageInfo)
{
    OT_UNUSED_VARIABLE(aMessage);
    OT_UNUSED_VARIABLE(aMessageInfo);

    bool didHandle = true;
    Uri  uri       = UriFromPath(aUriPath);

#define Case(kUri, Type)                                     \
    case kUri:                                               \
        Get<Type>().HandleTmf<kUri>(aMessage, aMessageInfo); \
        break

    switch (uri)
    {
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE
        Case(kUriBackboneQuery, Manager);
        Case(kUriBackboneAnswer, Manager);
#endif

    default:
        didHandle = false;
        break;
    }

#undef Case

    return didHandle;
}

Error BackboneTmfAgent::Filter(const ot::Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo, void *aContext)
{
    OT_UNUSED_VARIABLE(aMessage);

    return static_cast<BackboneTmfAgent *>(aContext)->IsBackboneTmfMessage(aMessageInfo) ? kErrorNone : kErrorNotTmf;
}

bool BackboneTmfAgent::IsBackboneTmfMessage(const Ip6::MessageInfo &aMessageInfo) const
{
    const Ip6::Address &dst = aMessageInfo.GetSockAddr();
    const Ip6::Address &src = aMessageInfo.GetPeerAddr();

    // A Backbone TMF message must comply with following rules:
    // The destination must be one of:
    //     1. All Network BBRs (Link-Local scope)
    //     2. All Domain BBRs (Link-Local scope)
    //     3. A Backbone Link-Local address
    // The source must be a Backbone Link-local address.
    return (Get<BackboneRouter::Local>().IsEnabled() && src.IsLinkLocalUnicast() &&
            (dst.IsLinkLocalUnicast() || dst == Get<BackboneRouter::Local>().GetAllNetworkBackboneRoutersAddress() ||
             dst == Get<BackboneRouter::Local>().GetAllDomainBackboneRoutersAddress()));
}

Error BackboneTmfAgent::UpdateMulticastSubscriptions(void)
{
    Error               error = kErrorNone;
    const Ip6::Address  *desired[2] = {&Get<Local>().GetAllNetworkBackboneRoutersAddress(), nullptr};

    VerifyOrExit(Get<Local>().IsEnabled() && mSocket.IsBound() && !mNeedsCleanup, error = kErrorInvalidState);
    if (Get<Leader>().GetDomainPrefix() != nullptr &&
        Get<Local>().GetAllDomainBackboneRoutersAddress() != *desired[0])
    {
        desired[1] = &Get<Local>().GetAllDomainBackboneRoutersAddress();
    }

    // Remove obsolete actual memberships before acquiring replacements. Keep
    // failed leaves recorded, including across another prefix update. A set,
    // rather than fixed network/domain slots, preserves shared group references.
    for (unsigned i = 0; i < 2; ++i)
    {
        if (!mSubscribed[i] || mSubscriptions[i] == *desired[0] ||
            (desired[1] != nullptr && mSubscriptions[i] == *desired[1]))
        {
            continue;
        }
        Error result = mSocket.LeaveNetifMulticastGroup(Ip6::kNetifBackbone, mSubscriptions[i]);
        LogError("Backbone TMF unsubscribes", mSubscriptions[i], result);
        if (result == kErrorNone || result == kErrorNotFound)
        {
            mSubscribed[i] = false;
        }
        else if (error == kErrorNone)
        {
            error = result;
        }
    }

    for (const Ip6::Address *address : desired)
    {
        int  freeSlot = -1;
        bool found    = false;

        if (address == nullptr)
        {
            continue;
        }
        for (unsigned i = 0; i < 2; ++i)
        {
            if (!mSubscribed[i])
            {
                freeSlot = static_cast<int>(i);
            }
            else if (mSubscriptions[i] == *address)
            {
                found = true;
            }
        }
        if (found)
        {
            continue;
        }
        Error result = (freeSlot < 0) ? kErrorNoBufs
                                     : mSocket.JoinNetifMulticastGroup(Ip6::kNetifBackbone, *address);
        LogError("Backbone TMF subscribes", *address, result);
        if (result == kErrorNone)
        {
            mSubscriptions[freeSlot] = *address;
            mSubscribed[freeSlot]    = true;
        }
        else if (error == kErrorNone)
        {
            error = result;
        }
    }

exit:
    return error;
}

void BackboneTmfAgent::LogError(const char *aText, const Ip6::Address &aAddress, Error aError) const
{
    OT_UNUSED_VARIABLE(aText);
    OT_UNUSED_VARIABLE(aAddress);

    if (aError == kErrorNone)
    {
        LogInfo("%s %s: %s", aText, aAddress.ToString().AsCString(), ErrorToString(aError));
    }
    else
    {
        LogWarn("%s %s: %s", aText, aAddress.ToString().AsCString(), ErrorToString(aError));
    }
}

} // namespace BackboneRouter
} // namespace ot

#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
