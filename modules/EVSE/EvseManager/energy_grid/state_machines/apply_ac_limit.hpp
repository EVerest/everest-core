#pragma once
#include "generated/types/energy.hpp"
#include <boost/mpl/vector.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/euml/euml.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace module::energy_grid::fsm {

namespace apply_ac_limit_impl {

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;
using namespace msm::front::euml;

struct ApplyCurrent : public state<> {};
struct ApplyPower : public state<> {};
struct Done : public state<> {};

struct has_current_limit {
    template <class Fsm, class Evt, class SourceState, class TargetState>
    bool operator()(Evt const&, Fsm& fsm, SourceState&, TargetState&) {
        return fsm.energy_limits.ac_max_current_A.has_value();
    }
};

struct has_power_limit {
    template <class Fsm, class Evt, class SourceState, class TargetState>
    bool operator()(Evt const&, Fsm& fsm, SourceState&, TargetState&) {
        return fsm.energy_limits.total_power_W.has_value();
    }
};

struct apply_current_limit {
    template <class Fsm, class Evt, class SourceState, class TargetState>
    void operator()(Evt const&, Fsm& fsm, SourceState&, TargetState&) {
        fsm.limit = fsm.energy_limits.ac_max_current_A->value;
    }
};

struct apply_power_limit {
    template <class Fsm, class Evt, class SourceState, class TargetState>
    void operator()(Evt const&, Fsm& fsm, SourceState&, TargetState&) {
        fsm.publish(fsm.energy_limits.total_power_W->value);
        auto val = static_cast<float>(fsm.energy_limits.total_power_W->value / fsm.ac_nominal_voltage /
                                      fsm.ac_nr_phases_active);
        if (val < fsm.limit) {
            fsm.limit = val;
        }
    }
};

// clang-format off
struct FSM_ : msm::front::state_machine_def<FSM_> {
    using initial_state = ApplyCurrent;

    FSM_(types::energy::LimitsRes const& energy_limits,
         int ac_nr_phases_active, double ac_nominal_voltage,
         std::function<void(float)> const& publish) :
        energy_limits(energy_limits),
        ac_nr_phases_active(ac_nr_phases_active), ac_nominal_voltage(ac_nominal_voltage),
        publish(publish)
        {}

    struct transition_table : mpl::vector<
        //   Source         + Event              ->Target        / Action  [Guard]
        // +----------------+------------------+-----------------+--------+-------------------------+
        Row< ApplyCurrent   , none             , ApplyPower      , none   , has_current_limit      >,
        Row< ApplyCurrent   , none             , ApplyPower      , none   , Not_<has_current_limit>>,
        Row< ApplyPower     , none             , Done            , none   , has_power_limit        >,
        Row< ApplyPower     , none             , Done            , none   , Not_<has_power_limit>  >
        // +----------------+------------------+-----------------+--------+-------------------------+

        > {};

    types::energy::LimitsRes const& energy_limits;
    int ac_nr_phases_active;
    double ac_nominal_voltage;
    std::function<void(float)> const& publish;
    float limit{0};
};

using FSM = msm::back::state_machine<FSM_>;

// clang-format on
} // namespace apply_ac_limit_impl
} // namespace module::energy_grid::fsm
