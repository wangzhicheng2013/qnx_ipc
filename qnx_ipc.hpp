#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/dispatch.h>
#include <sys/iomsg.h>
#include <process.h>
#include "color_log.hpp"

static const char *const ATTACH_POINT = "some_qnx_point";

class qnx_ipc {
public:
    bool init_server() {
        if (nullptr == (attach_ = name_attach(NULL, ATTACH_POINT, 0))) {
            LOG_E("qnx server init failed!");
            return false;
        }
        return true;
    }
    bool init_client() {
        if ((server_coid_ = name_open(ATTACH_POINT, 0)) < 0) {
            LOG_E("qnx client init failed!");
            return false;
        }
        return true;
    }
    void uninit_service() {
        name_detach(attach_, 0);
    }
    void uninit_client() {
        name_close(server_coid_);
    }
    bool send(const char *data, size_t len) {
        if (server_coid_ < 0) {
            LOG_E("qnx client does not init success!");
            return false;
        }
        // block
        if (-1 == MsgSend(server_coid_, data, len , NULL, 0)) {
            LOG_E("send to server failed!");
            return false;
        }
        return true;
    }
    bool reveive(char *data, size_t len) {
        if (!attach_) {
            LOG_E("qbx server does not init success!");
            return false;
        }
        // block
        int rcvid = MsgReceive(attach_->chid, data, len, NULL);
        if (rcvid != 0) {
            MsgReply(rcvid, 0, NULL, 0);
        }
        return true;
    }  
private:
    int server_coid_ = -1;
	name_attach_t *attach_ = nullptr;
};