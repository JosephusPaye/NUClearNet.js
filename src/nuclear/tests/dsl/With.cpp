/*
 * Copyright (C) 2013      Trent Houliston <trent@houliston.me>, Jake Woods <jake.f.woods@gmail.com>
 *               2014-2017 Trent Houliston <trent@houliston.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <catch.hpp>

#include "nuclear"

namespace {

struct DifferentOrderingMessage1 {
    int a;
};
struct DifferentOrderingMessage2 {
    int a;
};
struct DifferentOrderingMessage3 {
    int a;
};

class DifferentOrderingReactor : public NUClear::Reactor {
public:
    DifferentOrderingReactor(std::unique_ptr<NUClear::Environment> environment) : Reactor(std::move(environment)) {
        // Check that the lists are combined, and that the function args are in order
        on<With<DifferentOrderingMessage1>, Trigger<DifferentOrderingMessage3>, With<DifferentOrderingMessage2>>().then(
            [this](const DifferentOrderingMessage1&,
                   const DifferentOrderingMessage3&,
                   const DifferentOrderingMessage2&) { this->powerplant.shutdown(); });

        // Make sure we can pass an empty function in here
        on<Trigger<DifferentOrderingMessage1>, With<DifferentOrderingMessage1, DifferentOrderingMessage2>>().then(
            [this] {});
    }
};
}  // namespace

TEST_CASE("Testing poorly ordered on arguments", "[api][with]") {

    NUClear::PowerPlant::Configuration config;
    config.thread_count = 1;
    NUClear::PowerPlant plant(config);
    plant.install<DifferentOrderingReactor>();

    plant.emit(std::make_unique<DifferentOrderingMessage1>());
    plant.emit(std::make_unique<DifferentOrderingMessage2>());
    plant.emit(std::make_unique<DifferentOrderingMessage3>());

    plant.start();
}
