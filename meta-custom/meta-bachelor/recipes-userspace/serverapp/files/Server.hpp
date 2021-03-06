#ifndef _SERVER_H_
#define _SERVER_H_

#include <algorithm>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <netlink/attr.h>
#include <netlink/cache.h>
#include <netlink/data.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <netlink/netlink.h>
#include <netlink/route/link.h>
#include <netlink/socket.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

using namespace std;
using namespace Pistache;

struct command {
    uint8_t id;     
    uint8_t len;    
    char *words;    
};

struct msg_to_send {
	struct nl_sock *sk;
	struct nl_msg *msg;
	int sent;
};

struct kernel_response {
	size_t len;     
	void *msg;      
};

enum
{
    ATTR_UNSPEC,       
    ATTR_CTRL_WORD_ID, 
    ATTR_WORDS,     
    ATTR_RESPONSE,
    __ATTR_COUNT
};
#define ATTR_MAX (__ATTR_COUNT - 1) 

enum
{
    CMD_UNSPEC,               
    CMD_HANDLE_MSG_FROM_USER, 
    __CMD_COUNT
};
#define CMD_MAX (__CMD_COUNT - 1) 

#define GET_RANDOM 0x02
#define SET_LED_MODE 0x03
#define GET_LED_MODE 0x04
#define FAMILY_NAME "bachelor_degree"

class StatsEndpoint
{
public:
    StatsEndpoint(Address addr)
        : m_ret(0), m_family_id(0), httpEndpoint(std::make_shared<Http::Endpoint>(addr))
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
    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;

    int on_response(struct nl_sock *sock, std::function<void(struct nl_msg *)> on_response)
    {
        auto nl_cb = nl_socket_get_cb(sock);
        auto cb = nl_cb_clone(nl_cb);
        nl_cb_put(nl_cb);

        int err = 0;
        nl_cb_err(cb, NL_CB_CUSTOM, [] (auto nla, auto nlerr, auto arg) -> int {
            *static_cast<int *>(arg) = -1;
            return NL_STOP;
        }, static_cast<void *>(&err));

        nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, [] (auto msg, auto arg) {
            return (*static_cast<std::function<int(struct nl_msg*)> *>(arg))(msg);
        }, static_cast<void*>(&on_response));

        auto ret = nl_recvmsgs(sock, cb);

        nl_cb_put(cb);

        if (err)
            return err;
        return ret;
    }

    void setupRoutes()
    {
        using namespace Rest;

        Routes::Post(router, "/led-mode/:value", Routes::bind(&StatsEndpoint::ledHandler, this));
        Routes::Get(router, "/random/:value", Routes::bind(&StatsEndpoint::randomHandler, this));
        Routes::Get(router, "/", Routes::bind(&StatsEndpoint::readyHandler, this));
    }

    struct nl_msg * sendMsg(struct nl_sock *sock, int family_id, int nl_cmd, command cmd_to_send)
    {
        struct nl_msg *nlmsg = nlmsg_alloc();
        if (!nlmsg)
            return NULL;

        auto data = genlmsg_put(nlmsg, 0, NL_AUTO_SEQ, family_id, 0, 0, nl_cmd, 0);
        if (!data)
        {
            nlmsg_free(nlmsg);
            return NULL;
        }

        m_ret = nla_put_u8(nlmsg, ATTR_CTRL_WORD_ID, cmd_to_send.id);
        if (m_ret)
        {
            nlmsg_free(nlmsg);
            return NULL;
        }

        if (cmd_to_send.words && nla_put(nlmsg, ATTR_WORDS, cmd_to_send.len * 4, cmd_to_send.words)) {
		    printf("Cannot add payload to message\n");
	    	nlmsg_free(nlmsg);
		    return NULL;
	    }

        auto result = nl_send_auto(sock, nlmsg);
        nlmsg_free(nlmsg);
        if (result >= 0)
            return nlmsg;

        return NULL;
    }

    int ledHandler(const Rest::Request &request, Http::ResponseWriter response)
    {
        response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");
        if (!request.hasParam(":value"))
        {
            response.send(Http::Code::Forbidden, R"({"message": "No value?"})");
            return -1;
        };

        nl_msg *ret = NULL;
        auto httpValue = request.param(":value");
        auto ledMode = httpValue.as<std::string>();
        char word[ledMode.length() + 1];
        strcpy(word, ledMode.c_str());

        struct command cmd;
        cmd.id = (uint8_t)SET_LED_MODE;
        cmd.len = 1;
        cmd.words = word;

        auto nl_sock = nl_socket_alloc();
        if (!nl_sock) {
            response.send(Http::Code::Forbidden, R"({"message": "nl_socket_alloc error"})");
            return -1;
        }

        m_ret = genl_connect(nl_sock);
        if (m_ret) {
            nl_socket_free(nl_sock);
            response.send(Http::Code::Forbidden, R"({"message": "genl_connect error"})");
            return -1;
        }

        m_family_id = genl_ctrl_resolve(nl_sock, FAMILY_NAME);
        if (m_family_id < 0) {
            nl_socket_free(nl_sock);

            // there is no netlink family which means simulation mode is off
            // use IOCTL to communiacte with arduino
            int fd = open("/dev/spidev0.0", O_RDWR);
            int speed = 1000000;
            ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

            struct spi_ioc_transfer spi;
            memset (&spi, 0, sizeof (spi));
            unsigned char rxDat;
            char txBuf =  httpValue.as<char>();;
            char rxBuffer;
            int err;
            switch (httpValue.as<int>())
            {
            case 0:
                spi.tx_buf = (unsigned long)&txBuf;
                spi.rx_buf = (unsigned long)&rxBuffer;
                spi.len = 1;
                ioctl (fd, SPI_IOC_MESSAGE(1), &spi);
                break;
            case 1:
                spi.tx_buf = (unsigned long)&txBuf;
                spi.rx_buf = (unsigned long)&rxBuffer;
                spi.len = 1;
                err = ioctl (fd, SPI_IOC_MESSAGE(1), &spi);
                break;
            case 2:
                spi.tx_buf = (unsigned long)&txBuf;
                spi.rx_buf = (unsigned long)&rxBuffer;
                spi.len = 1;
                ioctl (fd, SPI_IOC_MESSAGE(1), &spi);
                break;
            default:
                break;
            }
            close(fd);

            std::string answer = "{\"ledMode\": \"";
            answer += ledMode;
            answer += "\"}";

            // inform administrator
            response.send(Http::Code::Ok, answer);
            return 0;
        }

        auto switch_int = httpValue.as<int>();
        switch (switch_int) {
        case 0:
            // always off
            ret = sendMsg(nl_sock, m_family_id, CMD_HANDLE_MSG_FROM_USER, cmd);

            if (!ret) {
                nl_socket_free(nl_sock);
                response.send(Http::Code::Forbidden, R"({"message": "Error occured while try to process the data"})");
                return -1;
            }

            response.send(Http::Code::Ok, R"({"ledMode": 0})");
            break;
        case 1:
            // always ON
            ret = sendMsg(nl_sock, m_family_id, CMD_HANDLE_MSG_FROM_USER, cmd);

            if (!ret) {
                nl_socket_free(nl_sock);
                response.send(Http::Code::Forbidden, R"({"message": "Error occured while try to process the data"})");
                return -1;
            }

            response.send(Http::Code::Ok, R"({"ledMode": 1})");
            break;
        case 2:
            // blink
            ret = sendMsg(nl_sock, m_family_id, CMD_HANDLE_MSG_FROM_USER, cmd);

            if (!ret) {
                nl_socket_free(nl_sock);
                response.send(Http::Code::Forbidden, R"({"message": "Error occured while try to process the data"})");
                return -1;
            }

            response.send(Http::Code::Ok, R"({"ledMode": 2})");
            break;
        default:
            response.send(Http::Code::Forbidden, R"({"message": "This is not valid value!"})");
            break;
        };

        nl_socket_free(nl_sock);
        return 0;
    }

    int randomHandler(const Rest::Request &request, Http::ResponseWriter response)
    {
        response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");
        if (!request.hasParam(":value"))
        {
            response.send(Http::Code::Forbidden, R"({"message": "No value?"})");
            return -1;
        };
        
        auto httpValue = request.param(":value");
        auto randomLength = httpValue.as<int>();

        uint32_t tmp =( uint32_t)randomLength;

        struct command cmd;
        cmd.id = (uint8_t)GET_RANDOM;
        cmd.len = 1;
        cmd.words = static_cast<char *>(calloc(4 * (cmd.len) + 1, sizeof(char)));
        int n = sizeof tmp + 1;
        
        int i;
        for (i = 0; i < cmd.len; i++) {
            for (int y = 0; n-- > 0; y++)
				cmd.words[4 * i + y] = (tmp >> ((sizeof(tmp) - n) * 8)) & 0xff;
        }
        cmd.words[4 * i] = '\0';

        auto nl_sock = nl_socket_alloc();
        if (!nl_sock) {
            response.send(Http::Code::Forbidden, R"({"message": "nl_socket_alloc error"})");
            return -1;
        }

        msg_to_send *msg = static_cast<msg_to_send *>(malloc(sizeof(struct msg_to_send)));;
        msg->sk = nl_sock;

        m_ret = genl_connect(nl_sock);
        if (m_ret) {
            nl_socket_free(nl_sock);
            response.send(Http::Code::Forbidden, R"({"message": "genl_connect error"})");
            return -1;
        }

        m_family_id = genl_ctrl_resolve(nl_sock, FAMILY_NAME);
        if (m_family_id < 0) {
            response.send(Http::Code::Forbidden, R"({"message": "genl_ctrl_resolve error"})");
            nl_socket_free(nl_sock);
            return -1;
        }

        msg->msg = sendMsg(nl_sock, m_family_id, CMD_HANDLE_MSG_FROM_USER, cmd);
        if (!msg->msg) {
            nl_socket_free(nl_sock);
            response.send(Http::Code::Forbidden, R"({"message": "Error occured while try to process the data"})");
            return -1;
        }

        struct kernel_response res;
        m_ret = on_response(nl_sock, [&] (auto msg) {
            struct nla_policy policy[ATTR_MAX + 1];

            policy[ATTR_RESPONSE].type = NLA_UNSPEC;
            policy[ATTR_RESPONSE].minlen = 0;
            policy[ATTR_RESPONSE].maxlen = 0;

            struct nlattr *attrs[ATTR_MAX + 1];
            auto err = genlmsg_parse(nlmsg_hdr(msg), 0, attrs, ATTR_MAX, policy);
            if (err < 0)
                return NL_STOP;

            auto header = static_cast<struct genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));

            if (header->cmd != CMD_HANDLE_MSG_FROM_USER) {
                return NL_STOP;
            }

            
            res.msg = nullptr;
            if (attrs[ATTR_RESPONSE]) {
                res.len = nla_len(attrs[ATTR_RESPONSE]);
                res.msg = (malloc(sizeof(char) * res.len));
                memcpy(res.msg, nla_data(attrs[ATTR_RESPONSE]), res.len);
            }

                return NL_STOP;
            });

        if (!res.msg) {
			    response.send(Http::Code::Forbidden, R"({"message": "Nullptr?"})");
        } else {
            std::string answer = "{\"random\": [";
            std::stringstream ss;
            ss << std::hex << std::setfill('0');
            
            for (int j = 0; j < res.len; ++j) {
                ss << "\"" << std::setw(2)  << static_cast<unsigned int>(static_cast<char *>(res.msg)[j]) << "\"";
                if (j != res.len - 1)
                    ss << ", ";
            }

            answer += ss.str();
            answer += "]}";
            response.send(Http::Code::Ok, answer);
        }
        
        nl_socket_free(nl_sock);
        return 0;
    }

    void readyHandler(const Rest::Request &request, Http::ResponseWriter response)
    {
        response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");
        std::string str = "Ready";
        response.send(Http::Code::Ok, str);
    }
};

#endif /*_SERVER_H*/