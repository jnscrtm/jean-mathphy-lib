#pragma once

#include "Misc.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

struct TestReport {
    std::string title;

    size_t total_attempt;
    double total_time;

    double min_time;
    double max_time;
    double avg_time;

    double std_deviation;
    double rel_deviation;
};

std::string interval_to_str(double t) {
    if(t < 1e-6)
        return std::to_string(t * 1000000000.0) + " ns";
    else if(t < 1e-3)
        return std::to_string(t * 1000000.0) + " us";
    else if(t < 1)
        return std::to_string(t * 1000.0) + " ms";
    else
        return std::to_string(t) + " s";
}

std::ostream& operator<<(std::ostream& os, const TestReport& report) {
    os << "[" << report.title << "]\n"
        << "Attempts   : " << report.total_attempt << "\n" 
        << "Total time : " << interval_to_str(report.total_time) << "\n"
        << "Min. time  : " << interval_to_str(report.min_time) << "\n"
        << "Max. time  : " << interval_to_str(report.max_time) << "\n"
        << "Avg. time  : " << interval_to_str(report.avg_time) << "\n"
        << "Std. Dev.  : " << interval_to_str(report.std_deviation) << "\n"
        << "Rel. Dev.  : " << (report.rel_deviation * 100.0) << "%"
        ;
    return os;
}

template <class F>
TestReport test_execute(F f, size_t n_attempt, const std::string& title) {
    using namespace std::chrono;

    std::vector<double> intervals = std::vector<double>();
    for(size_t i = 0; i < 1000; i++) {
        f();
    }

    for(size_t i = 0; i < n_attempt; i++) {
        auto _begin = high_resolution_clock::now();
        f();
        auto _end = high_resolution_clock::now();
        intervals.push_back(double((_end - _begin).count()) / 1000000000.0);
    }
    std::sort(intervals.begin(), intervals.end());
    
    TestReport rep;
    rep.title = title;
    rep.total_attempt = n_attempt;
    rep.total_time = std::accumulate(intervals.begin(), intervals.end(), 0.0);
    rep.min_time = intervals.front();
    rep.max_time = intervals.back();
    rep.avg_time = rep.total_time / n_attempt;
    rep.std_deviation = [&]() {
        double variance = 0.0;
        for(auto& x : intervals)
            variance += (x - rep.avg_time) * (x - rep.avg_time);

        variance /= n_attempt;
        return Sqrt(variance);
    }();
    rep.rel_deviation = rep.std_deviation / rep.avg_time;

    return rep;
}