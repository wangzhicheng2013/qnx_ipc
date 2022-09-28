#include <thread>
#include "qnx_ipc.hpp"
bool g_exit_flag = false;
qnx_ipc ipc;
static const int MSG_LEN = 10;
static void signal_handler(int sig) {
    printf("\nsignal %d...\n", sig);
    g_exit_flag = true;
}
void server_thread() {
    if (!ipc.init_server()) {
        return;
    }
    while (!g_exit_flag) {
        char *msg = new char[MSG_LEN] { 0 };
        if (ipc.reveive(msg, MSG_LEN)) {
            LOG_I("server recv msg:%s success!", msg);
        }
        delete [] msg;
    }
    ipc.uninit_service();
}
void client_thread() {
    if (!ipc.init_client()) {
        return;
    }
    while (!g_exit_flag) {
        char *msg = new char[MSG_LEN] { 0 };
        snprintf(msg, MSG_LEN, "%ld", time(0));
        if (ipc.send(msg, MSG_LEN)) {
            LOG_I("client send msg:%s success!", msg);
        }
        delete [] msg;
        sleep(2);
    }
    ipc.uninit_client();
}
int main() {
    signal(SIGTERM,signal_handler);
    signal(SIGINT, signal_handler);
    std::thread s(server_thread);
    std::thread c(client_thread);
    s.join();
    c.join();

    return 0;
}
