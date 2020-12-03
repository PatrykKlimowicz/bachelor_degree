#ifndef _SERVER_H_
#define _SERVER_H_

#include <algorithm>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

#define FAMILY_NAME "embedded_c"

using namespace Pistache;

class StatsEndpoint
{
public:
    StatsEndpoint(Address addr)
        : httpEndpoint(std::make_shared<Http::Endpoint>(addr)), m_ret(0), m_family_id(0), m_ledMode(0)
    {
    }

    void init(size_t thr = 2)
    {
        auto opts = Http::Endpoint::options()
                        .threads(thr)
                        .flags(Tcp::Options::InstallSignalHandler);
        httpEndpoint->init(opts);
        setupRoutes();
    }

    void start()
    {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }

    void shutdown()
    {
        httpEndpoint->shutdown();
    }

private:
    int m_ret;
    int m_family_id;
    int m_ledMode;
    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;

    void setupRoutes()
    {
        using namespace Rest;

        Routes::Post(router, "/led-mode/:value", Routes::bind(&StatsEndpoint::ledHandler, this));
        Routes::Get(router, "/random-sequence/:value/", Routes::bind(&StatsEndpoint::randomHandler, this));
        Routes::Get(router, "/", Routes::bind(&StatsEndpoint::readyHandler, this));
    }

    int sendMsg(struct nl_sock *sock, int family_id, int cmd, std::function<int(struct nl_msg *msg)> filler)
    {
        auto nlmsg = nlmsg_alloc();
        if (!nlmsg)
        {
            return -1;
        }

        auto data = genlmsg_put(nlmsg, 0, NL_AUTO_SEQ, family_id, 0, 0, cmd, 0);
        if (!data)
        {
            nlmsg_free(nlmsg);
            return -1;
        }

        m_ret = filler(nlmsg);
        if (m_ret)
        {
            nlmsg_free(nlmsg);
            return -1;
        }

        auto result = nl_send_auto(sock, nlmsg);
        nlmsg_free(nlmsg);
        if (result >= 0)
        {
            return 0;
        }

        return -1;
    }

    int ledHandler(const Rest::Request &request, Http::ResponseWriter response)
    {
        response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

        auto nl_sock = nl_socket_alloc();
        if (!nl_sock)
        {
            return -1;
        }

        m_ret = genl_connect(nl_sock);
        if (m_ret)
        {
            nl_socket_free(nl_sock);
            return -1;
        }

        m_family_id = genl_ctrl_resolve(nl_sock, FAMILY_NAME);
        if (m_family_id < 0)
        {
            nl_socket_free(nl_sock);
            return -1;
        }

        if (request.hasParam(":value"))
        {
            auto httpValue = request.param(":value");
            m_ledMode = httpValue.as<int>();

            switch (m_ledMode)
            {
            case 0:
                // always off
                m_ret = sendMsg(nl_sock, m_family_id, GENL_LED_CMD_WITH_ARG, [&](auto nlmsg) {
                    NLA_PUT_U8(nlmsg, GENL_LED_ATTR_LED, 0);
                    return 0;
                nla_put_failure:
                    return -1;
                });

                if (m_ret)
                {
                    nl_socket_free(nl_sock);
                    return -1;
                }

                response.send(Http::Code::Ok, "Wrote 0.\n");
                break;
            case 1:
                // always on
                m_ret = sendMsg(nl_sock, m_family_id, GENL_LED_CMD_WITH_ARG, [&](auto nlmsg) {
                    NLA_PUT_U8(nlmsg, GENL_LED_ATTR_LED, 1);
                    return 0;
                nla_put_failure:
                    return -1;
                });

                if (m_ret)
                {
                    nl_socket_free(nl_sock);
                    return -1;
                }

                response.send(Http::Code::Ok, "Wrote 1.\n");
                break;
            case 2:
                // blink mode
                break;
            default:
                response.send(Http::Code::Ok, "Write 0 or 1.\n");
                break;
            };
        }

        nl_socket_free(nl_sock);
        return 0;
    }

    int onMulticastGroupResponse(struct nl_sock *sock, std::function<void(struct nl_msg *)> onMulticastGroupResponse)
    {
        auto nl_cb = nl_socket_get_cb(sock);
        auto cb = nl_cb_clone(nl_cb);
        nl_cb_put(nl_cb);

        int err = 0;
        nl_cb_err(
            cb, NL_CB_CUSTOM, [](auto nla, auto nlerr, auto arg) -> int {
                *static_cast<int *>(arg) = -1;
                return NL_STOP;
            },
            static_cast<void *>(&err));

        nl_cb_set(
            cb, NL_CB_VALID, NL_CB_CUSTOM, [](auto msg, auto arg) {
                return (*static_cast<std::function<int(struct nl_msg *)> *>(arg))(msg);
            },
            static_cast<void *>(&onMulticastGroupResponse));

        auto ret = nl_recvmsgs(sock, cb);

        nl_cb_put(cb);

        if (err)
            return err;
        return ret;
    }

    int btnHandler(const Rest::Request &request, Http::ResponseWriter response)
    {
        response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

        auto nl_sock = nl_socket_alloc();
        if (!nl_sock)
        {
            return -1;
        }

        m_ret = genl_connect(nl_sock);

        if (m_ret)
        {
            nl_socket_free(nl_sock);
            return -1;
        }

        m_family_id = genl_ctrl_resolve(nl_sock, FAMILY_NAME);
        if (m_family_id < 0)
        {
            nl_socket_free(nl_sock);
            return -1;
        }

        auto group_id = genl_ctrl_resolve_grp(nl_sock, "btn", "btn_group");
        if (group_id < 0)
        {
            nl_socket_free(nl_sock);
            return -1;
        }

        nl_socket_disable_seq_check(nl_sock);

        m_ret = nl_socket_add_membership(nl_sock, group_id);
        if (m_ret)
        {
            nl_socket_free(nl_sock);
            return -1;
        }

        while (!m_ret)
        {
            m_ret = onMulticastGroupResponse(nl_sock, [&](auto msg) {
                struct nla_policy policy[BTN_MULTICAST_ATTR_MAX + 1];

                policy[BTN_MULTICAST_ATTR].type = NLA_STRING;
                policy[BTN_MULTICAST_ATTR].minlen = 0;
                policy[BTN_MULTICAST_ATTR].maxlen = 64;

                struct nlattr *attrs[BTN_MULTICAST_ATTR_MAX + 1];
                auto err = genlmsg_parse(nlmsg_hdr(msg), 0, attrs, BTN_MULTICAST_ATTR_MAX, policy);
                if (err < 0)
                    return NL_STOP;

                auto header = static_cast<struct genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));

                if (header->cmd != BTN_MULTICAST_CMD)
                {
                    return NL_STOP;
                }

                if (attrs[BTN_MULTICAST_ATTR])
                {
                    std::string msgBtn = nla_get_string(attrs[BTN_MULTICAST_ATTR]);
                    response.send(Http::Code::Ok, msgBtn);
                }

                return NL_STOP;
            });
            break;
        }

        nl_socket_free(nl_sock);
        return m_ret;
    }

    void readyHandler(const Rest::Request &, Http::ResponseWriter response)
    {
        response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, "Ready.");
    }
};

#endif /*_SERVER_H*/
