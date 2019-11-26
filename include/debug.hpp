#pragma once

#include <fb/mutex.hpp>

#include <iostream>
#include <utility>

#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

extern fb::Mutex loglock;

template <typename Arg, typename... Args>
void log(std::ostream &out, Arg &&arg, Args&&... args) {
	/*
    fb::AutoLock lock(loglock);
    out << syscall(SYS_gettid) << ":\t"
        << std::forward<Arg>(arg);
    ((out << std::forward<Args>(args)), ...);
    out << std::flush;
    */
}
