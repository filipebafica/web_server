#ifndef SRC_SIGNALS_HPP_
#define SRC_SIGNALS_HPP_

#include <csignal>
#include <cstdlib>

class Monitor;

class Signals {
 public:
    explicit Signals(Monitor* monitor);
    ~Signals();

    static void handler(int signal);

 private:
    static Monitor *monitor;
};

#endif /* SRC_SIGNALS_HPP_ */
