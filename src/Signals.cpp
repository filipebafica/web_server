#include <Monitor.hpp>
#include <Signals.hpp>

Signals::Signals(Monitor *monitor) {
    Signals::monitor = monitor;
    std::signal(SIGINT, Signals::handler);
    std::signal(SIGQUIT, Signals::handler);
    std::signal(SIGPIPE, SIG_IGN);
}

Signals::~Signals() {}

void Signals::handler(int signal) {
    // TODO: Check the line below
    (void)signal;
    if (Signals::monitor) {
        Signals::monitor->stopLoop();
    }
}

Monitor * Signals::monitor = 0;
